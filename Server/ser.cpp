#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <pthread.h>
#include <vector>
#include <arpa/inet.h>
#include <algorithm>

#include "fileTransfer.h"

using namespace std;

int client_index = 0;

vector<client *> clients;

void *recvMsg(void *data)
{
    client *clientData = (client *)data;
    int connfd = clientData->sockfd;
    int fileconnfd = clientData->sockfd_file_transfer;
    char select_client;
    char buffer[1000];

    while (recv(connfd, buffer, sizeof(buffer), 0) > 0)
    {
        // cout << "\nMessage from Client # " << clientData->index << ": " << buffer << " | ";
        select_client = buffer[0] - '0'; 
        string message = buffer;
        // cout << message << endl;

        message[0] = clientData->index + '0';

        for (client *c : clients)
        {
            if (c->index == select_client)
            {
                // cout << c->index << " | message sent!\n";
                /*
                if (message.substr(1) == "helo")
                    send(c->sockfd, "message.c_str()", strlen("message.c_str()"), 0);
                else */
                //cout << "M= " << message << endl;
                send(c->sockfd, message.c_str(), strlen(message.c_str()), 0);
            }
            else if (select_client == 0)
            {
                if (clientData->index != c->index)
                    send(c->sockfd, message.c_str(), strlen(message.c_str()), 0);
            }
        }
        memset(buffer, 0, sizeof(buffer));

        // if message received is "file",
        if (message.substr(1).compare("file") == 0)
        {
            // string filename = receiveFile(connfd);
            // cout << "File incoming > " << filename << endl;

            for (client *c : clients)
            {
                if (c->index == select_client)
                {
                    // send(c->sockfd, message.c_str(), strlen(message.c_str()), 0);
                    // cout << c->index << " | message sent!\n";
                    // sendFile(receiveFile(connfd), c->sockfd);
                    forwardFile(c->sockfd_file_transfer, fileconnfd);
                }
                else if (select_client == '0')
                    forwardFile(c->sockfd_file_transfer, fileconnfd);
            }
        }
    }
    pthread_exit(NULL);
}

void *sendMsg(void *)
{
    while (1)
    {
        char buffer[1000];
        cout << "Enter Message : ";
        cin.getline(buffer, sizeof(buffer));
        for (client *c : clients)
        {
            send(c->sockfd, buffer, strlen(buffer), 0);
        }
    }
}

int main()
{
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    if (fd == -1)
    {
        perror("socket error : ");
        exit(0);
    }

    int client_len;
    struct sockaddr_in serveraddr, clientaddr;

    client_len = sizeof(clientaddr);

    // create socket
    // populate serveraddr struct
    bzero(&serveraddr, sizeof(serveraddr));
    serveraddr.sin_family = AF_INET;
    serveraddr.sin_addr.s_addr = INADDR_ANY;
    serveraddr.sin_port = htons(80);


    if (bind(fd, (struct sockaddr *)&serveraddr, sizeof(serveraddr)) > 0)
    {
        perror("Bind failed on socket : ");
        exit(0);
    }

    // listen (max number of clients is 5 rn)
    if (listen(fd, 5) != 0)
    {
        perror("listen failed on socket : ");
    }

    // ****Create a new socket for file transfer****
    int fd_file_transfer = socket(AF_INET, SOCK_STREAM, 0);
    if (fd_file_transfer == -1)
    {
        perror("File Transfer Socket creation failed : ");
        exit(-1);
    }

    struct sockaddr_in s_addr_file_transfer;
    s_addr_file_transfer.sin_family = AF_INET;
    s_addr_file_transfer.sin_port = htons(8080); // Different port for file transfer
    inet_aton("127.0.0.1", &s_addr_file_transfer.sin_addr);

    if (bind(fd_file_transfer, (struct sockaddr *)&s_addr_file_transfer, sizeof(s_addr_file_transfer)) > 0)
    {
        perror("Bind failed on File Transfer Socket : ");
        exit(-1);
    }

    if (listen(fd_file_transfer, 5) != 0)
    {
        perror("listen failed on File Transfer Socket : ");
    }
    ////////////////////////////////////

    pthread_t send_thread;
    pthread_create(&send_thread, NULL, sendMsg, NULL);
    while (1)
    {
        //cout << "while";
        struct sockaddr_in c_addr;
        socklen_t cliaddr_len = sizeof(c_addr);

        // new client added - connfd is client socket
        int connfd = accept(fd, (struct sockaddr *)&c_addr, &cliaddr_len);
        if (connfd < 0)
        {
            perror("accept failed on socket : ");
            exit(-1);
        }


        //struct vector<string> files;

        cout << "Client # "
             << client_index + 1
             << " connected!\nIP: " << inet_ntoa(c_addr.sin_addr)
             << " | Port: " << c_addr.sin_port << endl
             << "Files: ";

        // Assume each file is transmitted as a null-terminated string
        char Files[1000]; // Adjust the buffer size as needed
        string filestream;

        recv(connfd, Files, 1000, 0);
        cout << Files << endl;
       

        struct sockaddr_in Fc_addr;
        socklen_t Fcliaddr_len = sizeof(Fc_addr);

        // Accept connection on the file transfer socket
        int connfd_file_transfer = accept(fd_file_transfer, (struct sockaddr *)&Fc_addr, &Fcliaddr_len);
        //cout << "dsafds";
        if (connfd_file_transfer < 0)
        {
            perror("accept failed on File Transfer Socket : ");
            exit(-1);
        }

        // send(connfd, Files, 1000, 0);
        // storing client info into the client structure
        client *clientData = new client;
        clientData->index = client_index + 1;
        clientData->sockfd = connfd;
        clientData->sockfd_file_transfer = connfd_file_transfer;
        clientData->addr = c_addr;

        // Add the client to the vector
        clients.push_back(clientData);
        client_index++;

        for (client *c : clients)
        {
            // if (c->index != client_index) {
            string message = "Files in client " + to_string(client_index) + ": " + Files;
            send(c->sockfd, message.c_str(), strlen(message.c_str()), 0);
            //}
        }

        /* // getting total size of the client vector and serializing it
        size_t totalSize = clients.size() * sizeof(client);
        // Allocate memory for serializedClients before the loop
        char *serializedClients = new char[totalSize];

        // Serialize the clients
        size_t offset = 0;
        for (const client *element : clients)
        {
            serializeMyStruct(*element, serializedClients + offset);
            offset += sizeof(client);
        }

        // sending clientTable to all clients
        for (client *c : clients)
        {
            // Send the total size and the serialized data
            if (c->index != client_index)
            {
                send(c->sockfd, &totalSize, sizeof(size_t), 0);
                send(c->sockfd, serializedClients, totalSize, 0);
            }
        }

        // Deallocate memory for serializedClients
        delete[] serializedClients; */

        // send all clients the struct client for new client
        /* client data = {clientData->index, clientData->sockfd, clientData->addr};
        //char buffer[sizeof(client)];
        //std::memcpy(buffer, &data, sizeof(client));

        for (client *c : clients)
        {
            // Send the total size and the serialized data
            // if (c->index != client_index)
            cout << c->index << " " << c->sockfd << endl;
            send(c->sockfd, &data, sizeof(data), 0);
        } */

        cout << "Client info broadcasted\n\n\n";

        // recv message thread
        pthread_t recv_thread;
        pthread_create(&recv_thread, NULL, recvMsg, (void *)clientData);
    }

    return 0;
}
