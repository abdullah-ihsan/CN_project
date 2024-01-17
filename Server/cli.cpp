#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include <unordered_map>

#include "fileTransfer.h"
#include <vector>

using namespace std;

vector<client *> clientTable;

int fd_file_transfer; 

void *recvMsg(void *socket)
{
	int64_t fd = (long)socket;
	char buffer[1000];
	while (1)
	{
		while (recv(fd, buffer, 1000, 0) > 0)
		{
			string message = buffer;
			if (message[0] == '0' || message[0] == '1' || message[0] == '2' || message[0] == '3' || message[0] == '4' || message[0] == '5')
			{
				char cliID = message[0];
				message = message.substr(1, message.size() - 1);
				if (message == "file")
				{
					receiveFile(fd_file_transfer);
					cout << "File recieved from client " << cliID << endl;
				}
				else
					cout << "\nMessage from Client " << cliID << " : " << message << endl;
			}
			else if (message[0] == '~')
			{
			}
			else
				cout << "\nMessage from Server: " << message << endl;
			memset(&buffer, 0, sizeof(buffer));
		}
	}
}

// p2p stuff
/* void *getClientInfo(void *socket)
{
	cout << "Getting client info\n";
	int64_t fd = (long)socket;
	client adata;

	while (1)
	{
		while (recv(fd, &adata, sizeof(adata), 0) > 0)
		{
			// Dynamically allocate memory for each client
			client *receivedData = new client;

			// Copy the received data into the newly allocated memory
			memcpy(receivedData, &adata, sizeof(client));

			// Push a copy of the client data into the vector
			clientTable.push_back(receivedData);

			for (client *c : clientTable)
			{
				cout << "Client Number " << c->index << endl
					 << "Client fd: " << c->sockfd << endl;
			}
		}
	}
} */

vector<string> getFilesInDirectory()
{
	vector<string> files;

	DIR *dir;
	struct dirent *ent;

	if ((dir = opendir(".")) != NULL)
	{
		while ((ent = readdir(dir)) != NULL)
		{
			if (ent->d_type == DT_REG)
			{
				if (strcmp(ent->d_name, "client") == 0)
					continue;
				files.push_back(ent->d_name);
			}
		}
		closedir(dir);
	}
	cout << endl;
	return files;
}

int main()
{

	int fd = socket(AF_INET, SOCK_STREAM, 0);
	if (fd == -1)
	{
		perror("Socket creation failed : ");
		exit(-1);
	}

	struct sockaddr_in s_addr;
	s_addr.sin_family = AF_INET;
	s_addr.sin_port = htons(80);
	inet_aton("127.0.0.1", &s_addr.sin_addr);

	if (connect(fd, (struct sockaddr *)&s_addr, sizeof(s_addr)) == -1)
	{
		perror("Connect failed on socket : ");
		exit(-1);
	} else cout << "Connected\n";

	// sending file info to server.
	vector<string> files = getFilesInDirectory();
	string Files = "";
	for (string f : files)
	{
		// cout << f << " ";
		Files += f + ' ';
	}
	// Files += '*';
	// cout << Files << endl;

	// file transfer soceketing

	fd_file_transfer = socket(AF_INET, SOCK_STREAM, 0);
	if (fd_file_transfer == -1)
	{
		perror("File Transfer Socket creation failed : ");
		exit(-1);
	}

	struct sockaddr_in s_addr_file_transfer;
	s_addr_file_transfer.sin_family = AF_INET;
	s_addr_file_transfer.sin_port = htons(8080); // Different port for file transfer
	inet_aton("127.0.0.1", &s_addr_file_transfer.sin_addr);
	//cout << "dsagsdg";
	if (connect(fd_file_transfer, (struct sockaddr *)&s_addr_file_transfer, sizeof(s_addr_file_transfer)) == -1)
	{
		perror("Connect failed on File Transfer Socket : ");
		exit(-1);
	} else cout << "File connected\n";

	// Create a thread for file transfer
	/* pthread_t fileTransferThread;
	pthread_create(&fileTransferThread, NULL, fileTransferThread, (void *)fd_file_transfer); */
	///////////////////////////

	// make this client's data

	// client will send the client info to the server

	// pthread_t clientTableThread;
	// pthread_create(&clientTableThread, NULL, getClientInfo, (void *)fd);
	send(fd, Files.c_str(), 1000, 0);

	char fileinfo[1000];
	recv(fd, fileinfo, 1000, 0);
	cout << fileinfo << endl;

	char buffer[999];

	pthread_t recvThread;
	pthread_create(&recvThread, NULL, recvMsg, (void *)fd);

	// loop for sending message
	while (1)
	{
		// select client
		cout << "Select a client 1-5 (0 to send the whole community): ";
		char sel;
		cin >> sel;
		while (sel != '0' && sel != '1' && sel != '2' && sel != '3' && sel != '4' && sel != '5')
		{
			cout << "Wrong input, only 1 to 5";
			char temp;
			cin.ignore();
			cin >> temp;
			sel = temp;
		}

		cout << "Enter Message :  ";
		cin.ignore();
		cin.getline(buffer, 999);
		string message = string(1, sel) + buffer; 
		// cout << "test: " << message << endl;

		// file sending mechanism

		// 1. check if the input is file  // 2file
		if (strcmp(buffer, "file") == 0)
		{
			send(fd, message.c_str(), strlen(message.c_str()), 0);

			// ask for file name
			cout << "Enter file name > ";
			char filename[100];
			cin.getline(filename, 100);
			sendFile(filename, fd_file_transfer);
			// pass it into the send file function

			// shutdown(fd, 1);
			// close(fd);
		}
		else
		////////////////////////
		{
			// cout << "sending " << buffer << endl;
			send(fd, message.c_str(), strlen(message.c_str()), 0);
		}
	}

	return 0;
}
