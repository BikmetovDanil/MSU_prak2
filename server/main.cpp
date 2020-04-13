#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cstring>

#define BUFLEN 1024
using namespace std;

char* IntToString(int);

class Server{
	int server_sockfd;
	socklen_t server_len;
	sockaddr_in server_addr;
	int port;
public:
	Server(int port = 8080){
		server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
		server_addr.sin_family = AF_INET;
		server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
		server_addr.sin_port = htons(port);
		this->port = port;
		server_len = sizeof(server_addr);
	}
	void setPort(int port){
		server_addr.sin_port = htons(port);
	}
	int getPort() const{
		return port;
	}
	int getSocket() const{
		return server_sockfd;
	}
	void Sbind(){
		if(bind(server_sockfd, (struct sockaddr *)&server_addr, server_len) < 0){
			cerr << "Can't bind socket" << endl;
			exit(1);
		}
	}
	void Slisten(){
		if(listen(server_sockfd, 5) < 0){
			cerr << "Can't listen" << endl;
			exit(2);
		}
	}
};

class Client{
	int client_sockfd;
	socklen_t client_len;
	sockaddr_in client_addr;
	int getLength(int fd){
		char c;
		int len = 0;
		while(read(fd, &c, 1)) len++;
		lseek(fd, 0, 0);
		return len;
	}
public:
	void Caccept(int server_sockfd){
		client_len = sizeof(client_addr);
		client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_addr, &client_len);
		if(client_sockfd < 0){
			cerr << "Can't accept (not fatal error)" << endl;
		}
	}
	void Crequest(){
		int len, fileLength;
		char* strFileLength;
		char buf[BUFLEN];
		if((len = recv(client_sockfd, &buf, BUFLEN, 0)) < 0){
			shutdown(client_sockfd, 1);
			close(client_sockfd);
			cout << "Error with reading socket" << endl;
			exit(3);
		}
		int fd;
		if(strncmp(buf, "GET /", 5)){
			fd = open("501.html", O_RDONLY);
			fileLength = getLength(fd);
			strcpy(buf, "HTTP/1.0 501 NotImplemented\nAllow: GET\nServer: BikmetovDanilServer/0.1\nConnection: keep-alive\nContent-type: text/html\nContet-length: ");
			strFileLength = IntToString(fileLength);
			strcat(buf, strFileLength);
			strcat(buf, "\n\n");
			len = strlen(buf);
			send(client_sockfd, &buf, len, 0);
			while((len = read(fd, buf, BUFLEN)) > 0){
				send(client_sockfd, &buf, len, 0);
			}
			delete [] strFileLength;
			shutdown(client_sockfd, 1);
			close(client_sockfd);
			cout << "Error: BadRequest" << endl;
			return;
		}
		int i = 5;
		while(buf[i] && (buf[i++] > ' '));
		buf[i-1] = 0;
		cout << "received = " << buf + 5 << endl;
		if((fd = open(buf+5, O_RDONLY)) < 0){
			fd = open("404.html", O_RDONLY);
			fileLength = getLength(fd);
			strcpy(buf, "HTTP/1.0 404 PageNotFound\nAllow: GET\nServer: BikmetovDanilServer/0.1\nConnection: keep-alive\nContent-type:text/html\nContent-length: ");
			strFileLength = IntToString(fileLength);
			strcat(buf, strFileLength);
			strcat(buf, "\n\n");
			len = strlen(buf);
			send(client_sockfd, &buf, len, 0);
			while((len = read(fd, buf, BUFLEN)) > 0){
				send(client_sockfd, &buf, len, 0);
			}
			delete [] strFileLength;
			shutdown(client_sockfd, 1);
			close(client_sockfd);
			cout << "Error: 404" << endl;
			return;
		}
		
		fileLength = getLength(fd);
		strcpy(buf, "HTTP/1.0 200 BikmetovDanilServer\nAllow: GET\nServer: BikmetovDanilServer/0.1\nConnection: keep-alive\nContent-type:text/html\nContent-length: ");
		strFileLength = IntToString(fileLength);
		strcat(buf, strFileLength);
		strcat(buf, "\n\n");
		len = strlen(buf);
		send(client_sockfd, &buf, len, 0);
		
		while((len = read(fd, buf, BUFLEN)) > 0){
			send(client_sockfd, &buf, len, 0);
		}
		delete [] strFileLength;
		close(fd);
		shutdown(client_sockfd, 1);
		close(client_sockfd);
		return;
	}
};

char* IntToString(int a){
	char* s = new char[11];
	sprintf(s, "%d", a);
	strcat(s, "\0");
	return s;
}

int main(int argc, char* argv[]){
	Server server;
	Client client;
	if(argc > 1){
		server.setPort(atoi(argv[1]));
	}
	cout << "Port = " << server.getPort() << endl;
	server.Sbind();
	server.Slisten();
	while(1){
		client.Caccept(server.getSocket());
		client.Crequest();
	}
	return 0;
}
