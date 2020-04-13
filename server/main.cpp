#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <string.h>
#include <cstring>

#define BUFLEN 1024
using namespace std;

class Server{
	int server_sockfd, client_sockfd;
	int server_len, client_len;
	sockaddr_in server_addr;
	sockaddr_in client_addr;
public:
	Server(int port = 8080){
		server_sockfd = socket(AF_INET, SOCK_STREAM, 0);
		server_addr.sin_family = AF_INET;
		server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
		server_addr.sin_port = htons(port);
		server_len = sizeof(server_addr);
		cout << "Port = " << port << endl;
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
	void Saccept(){
		client_sockfd = accept(server_sockfd, (struct sockaddr *)&client_addr, (socklen_t*)&client_len);
		if(client_sockfd < 0){
			cerr << "Can't accept (not fatal error)" << endl;
		}
	}
	void Srequest(){
		int len;
		char buf[BUFLEN];
		if((len = recv(client_sockfd, &buf, BUFLEN, 0)) < 0){
			shutdown(client_sockfd, 1);
			close(client_sockfd);
			cout << "Error with reading socket" << endl;
			exit(3);
		}
		if(strncmp(buf, "GET /", 5)){
			strcpy(buf, "HTTP/1.0 400 Bad Request\n");
			len = strlen(buf);
			send(client_sockfd, &buf, len, 0);
			shutdown(client_sockfd, 1);
			close(client_sockfd);
			cout << "Error: BadRequest" << endl;
			return;
		}
		int i = 5;
		while(buf[i] && (buf[i++] > ' '));
		buf[i-1] = 0;
		cout << "received = " << buf + 5 << endl;
		int fd;
		if((fd = open(buf+5, O_RDONLY)) < 0){
			strcpy(buf, "HTTP/1.0 404 PageNotFound\n\n\n");
			len = strlen(buf);
			send(client_sockfd, &buf, len, 0);
			fd = open("404.html", O_RDONLY);
			while((len = read(fd, buf, BUFLEN)) > 0){
				send(client_sockfd, &buf, len, 0);
			}
			shutdown(client_sockfd, 1);
			close(client_sockfd);
			cout << "Error: 404" << endl;
			return;
		}
		
		strcpy(buf, "HTTP/1.0 200 BikmetovDanilServer\nAllow: GET\n\n");
		len = strlen(buf);
		send(client_sockfd, &buf, len, 0);
		
		while((len = read(fd, buf, BUFLEN)) > 0){
			send(client_sockfd, &buf, len, 0);
		}
		close(fd);
		shutdown(client_sockfd, 1);
		close(client_sockfd);
		return;
	}
};

int main(){
	Server server(8080);
	server.Sbind();
	server.Slisten();
	while(1){
		server.Saccept();
		server.Srequest();
	}
	return 0;
}
