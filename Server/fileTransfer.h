#include <iostream>
#include <fstream>
#include <string>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

using namespace std;

struct client
{
    int index;               // client number (index + 1)
    int sockfd;              // client socket
    struct sockaddr_in addr; // client address
    int sockfd_file_transfer;
    // pthread_t thread;
};

void sendFile(const string &filePath, int clientSocket)
{
    // Send file metadata (filename)
    send(clientSocket, filePath.c_str(), filePath.size() + 1, 0);

    // Open and send file content
    ifstream file(filePath, ios::binary);
    if (!file.is_open())
    {
        cout << "Error opening file: " << filePath << endl;
        return;
    }

    char buffer[1000];
    while (!file.eof())
    {
        file.read(buffer, sizeof(buffer));
        send(clientSocket, buffer, file.gcount(), 0);
    }

    cout << "File sent successfully: " << filePath << endl;
    // close(clientSocket);
    // shutdown(clientSocket, 1);
}

string receiveFile(int clientSocket)
{
    // Receive file metadata
    char fileNameBuffer[100];
    if (recv(clientSocket, fileNameBuffer, sizeof(fileNameBuffer), 0) <= 0)
    {
        cerr << "Error receiving file name" << endl;
        return "";
    }

    string fileName(fileNameBuffer);

    ofstream file(fileName, ios::binary);
    if (!file.is_open())
    {
        cerr << "Error opening file for writing: " << fileName << endl;
        return "";
    }

    // Receive file content
    char buffer[1000];
    int bytesRead;

    // cout << "\n-> about to run the loop\n";
    /* while ((bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0)) > 0)
    {
        cout << bytesRead << ' '; // Indicate progress - you may remove this in production
        file.write(buffer, bytesRead);
    } */

    do
    {
        bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);
        // cout << bytesRead << ' '; 
        file.write(buffer, bytesRead);
    } while (bytesRead == 1000);

    // close(clientSocket);

    if (bytesRead < 0)
    {
        cerr << "Error receiving file content" << endl;
        return "";
    }

    cout << "File received successfully: " << fileName << endl;
    return fileName;
}

void forwardFile(int senderSocket, int recieverSocket)
{
    sendFile(receiveFile(recieverSocket), senderSocket);
}
