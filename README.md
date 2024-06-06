# Multi-Party Chat and File Sharing using C++

The project uses TCP sockets to handle communication between different clients connected to the server. The project file contains cli.cpp and ser.cpp, along with the fileTransfer.h header file for functions responsible for file transferring. The messages are sent in a P2P model and the files through a server. Every client has its own directory containing files. Files transfer has been handled using fstream. Duplex chat is being handled by pthread_t.


## How to use
1. First, clone the repository to your system and change directory:
```zsh
git clone https://github.com/abdullah-ihsan/Multi-Party-Chat-and-File-Sharing-using-C-
```
```zsh
cd Multi-Party-Chat-and-File-Sharing-using-C-
cd Server
```

2. Open at least two instances of the terminal, one for the server and the rest for clients (max 4 clients)
3. First start the server using the following command

  ```zsh
  sudo ./server
  ```
  and the client with this command
  ```zsh
  sudo ./client
  ```

  
![image](https://github.com/abdullah-ihsan/Multi-Party-Chat-and-File-Sharing-using-C-/assets/65601738/b82da6c5-70c7-48bc-bcee-96ce4aa6b912)
Every client contains an executable and other files available to transfer.


![image](https://github.com/abdullah-ihsan/Multi-Party-Chat-and-File-Sharing-using-C-/assets/65601738/e9f9c405-f654-4bdb-b364-99f6ca9c077b)
One console for server and four consoles for clients in different directories.

![image](https://github.com/abdullah-ihsan/Multi-Party-Chat-and-File-Sharing-using-C-/assets/65601738/53c4405c-5bae-4ce9-b58e-ac16e9e5d743)

Four clients have been connected. (MAX 5). Clients receive file information of rest of the clients.


![image](https://github.com/abdullah-ihsan/Multi-Party-Chat-and-File-Sharing-using-C-/assets/65601738/eb5caed8-8bf8-4893-90d7-f1953fbfa9a7)
![image](https://github.com/abdullah-ihsan/Multi-Party-Chat-and-File-Sharing-using-C-/assets/65601738/ecdbc7f3-909c-4b34-af3f-8f60fcf6283f)

Messaging between clients and server broadcasting its message.



![image](https://github.com/abdullah-ihsan/Multi-Party-Chat-and-File-Sharing-using-C-/assets/65601738/b124eda4-6353-4810-8b73-d726c23a9779)
![image](https://github.com/abdullah-ihsan/Multi-Party-Chat-and-File-Sharing-using-C-/assets/65601738/6c3fad7e-eb85-4f9b-8272-7c58e6e5189a)

File transferring between clients through the server.
  
