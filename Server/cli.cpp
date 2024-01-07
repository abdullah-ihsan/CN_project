#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>

#include "fileTransfer.h"

using namespace std;

void *recvMsg(void *socket)
{
	int64_t fd = (long)socket;
	char buffer[1000];
	while (1)
	{
		while (recv(fd, buffer, 1000, 0) > 0)
		{
			string message = buffer;
			if (message[0] == '1' || message[0] == '2' || message[0] == '3' || message[0] == '4' || message[0] == '5')
			{
				char cliID = message[0];
				message = message.substr(1, message.size() - 1);
				if (message == "file")
				{
					receiveFile(fd);
					cout << "File recieved from client " << cliID << endl;
				}
				else
					cout << "\nMessage from Client " << cliID << " : " << message << endl;
			}
			else
				cout << "\nMessage from Server: " << message << endl;
			memset(&buffer, 0, sizeof(buffer));
		}
	}
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
	}

	char buffer[999];

	pthread_t thread;
	pthread_create(&thread, NULL, recvMsg, (void *)fd);

	// loop for sending message
	while (1)
	{
		// select client
		cout << "Select a client 1-5: ";
		char sel;
		cin >> sel;
		while (sel != '1' && sel != '2' && sel != '3' && sel != '4' && sel != '5')
		{
			cout << "Wrong input, only 1 to 5";
			char temp;
			cin >> temp;
			sel = temp;
		}

		cout << "Enter Message :  ";
		cin.ignore();
		cin.getline(buffer, 999);
		string message = string(1, sel) + buffer;
		// cout << "test: " << message << endl;

		// file sending mechanism

		// 1. check if the input is file
		if (strcmp(buffer, "file") == 0)
		{
			send(fd, message.c_str(), strlen(message.c_str()), 0);

			// ask for file name
			cout << "Enter file name > ";
			char filename[100];
			cin.getline(filename, 100);

			sendFile(filename, fd);
			// pass it into the send file function

			close(fd);
		}
		else
		////////////////////////
		{
			send(fd, message.c_str(), strlen(message.c_str()), 0);
		}
	}

	return 0;
}
