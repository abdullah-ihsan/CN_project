#include <iostream>
#include <fstream>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

struct client
{
    int index;               // client number (index + 1)
    int sockfd;              // client socket
    struct sockaddr_in addr; // client address
    // pthread_t thread;
};

const char* receiveFile(const char *filename, int serverSocket)
{
    std::ofstream file(filename, std::ios::binary);

    if (!file.is_open())
    {
        std::cerr << "Error creating file: " << filename << std::endl;
        return NULL;
    }

    // Receive the file size
    std::streampos fileSize;
    recv(serverSocket, reinterpret_cast<char *>(&fileSize), sizeof(fileSize), 0);

    // Receive and write the file content
    char buffer[1024];
    std::streampos receivedSize = 0;

    while (receivedSize < fileSize)
    {
        size_t bytesRead = recv(serverSocket, buffer, sizeof(buffer), 0);
        file.write(buffer, bytesRead);
        receivedSize += bytesRead;
    }

    file.close();
    std::cout << "File received successfully: " << filename << std::endl;
    return filename;
}

void sendFile(const char* filename, int clientSocket) {
    std::ifstream file(filename, std::ios::binary);

    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
        return;
    }

    // Get the file size
    file.seekg(0, std::ios::end);
    std::streampos fileSize = file.tellg();
    file.seekg(0, std::ios::beg);

    // Send the file size to the client
    send(clientSocket, reinterpret_cast<const char*>(&fileSize), sizeof(fileSize), 0);

    // Send the file content
    char buffer[1024];
    while (!file.eof()) {
        file.read(buffer, sizeof(buffer));
        send(clientSocket, buffer, file.gcount(), 0);
    }

    file.close();
    std::cout << "File sent successfully: " << filename << std::endl;
}

