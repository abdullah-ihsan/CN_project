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

struct client
{
    int index;
    int sockfd;
    struct sockaddr_in addr;
    pthread_t thread;
};

int client_index = 0;

vector<client *> clients;

void *recvMsg(void *data)
{
    client *clientData = (client *)data;
    int connfd = clientData->sockfd;
    char select_client;
    char buffer[1000];

    while (recv(connfd, buffer, sizeof(buffer), 0) > 0)
    {
        cout << "\nMessage from Client # " << clientData->index << ": " << buffer << " | ";
        select_client = buffer[0] - '0';
        string message = buffer;
        cout << message << endl;

        for (client *c : clients)
        {
            if (c->index == select_client)
            {
                cout << c->index << " | message sent!\n";
                send(c->sockfd, message.c_str(), strlen(message.c_str()), 0);
            }
        }
        memset(buffer, 0, sizeof(buffer));

        // if message received is "file",
        if (message.substr(1).compare("file") == 0)
        {
            //string filename = receiveFile(connfd);
            //cout << "File incoming > " << filename << endl;
            for (client *c : clients)
            {
                if (c->index == select_client)
                {
                    // send(c->sockfd, message.c_str(), strlen(message.c_str()), 0);
                    // cout << c->index << " | message sent!\n";
                    // sendFile(receiveFile(connfd), c->sockfd);
                    forwardFile(c->sockfd, connfd);
                }
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

    // bind socket
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

    pthread_t send_thread;
    pthread_create(&send_thread, NULL, sendMsg, NULL);
    while (1)
    {
        struct sockaddr_in c_addr;
        socklen_t cliaddr_len = sizeof(c_addr);

        // new client added - connfd is client socket
        int connfd = accept(fd, (struct sockaddr *)&c_addr, &cliaddr_len);
        if (connfd < 0)
        {
            perror("accept failed on socket : ");
            exit(-1);
        }

        cout << "Client # "
             << client_index + 1
             << "connected!\nIP: " << inet_ntoa(c_addr.sin_addr) << " | Port: " << c_addr.sin_port << endl;

        // storing client info into the client structure
        client *clientData = new client;
        clientData->index = client_index + 1;
        clientData->sockfd = connfd;
        clientData->addr = c_addr;

        // Add the client to the vector
        clients.push_back(clientData);
        client_index++;

        // recv message thread
        pthread_t recv_thread;
        pthread_create(&recv_thread, NULL, recvMsg, (void *)clientData);
    }

    return 0;
}
