#include <iostream>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <sys/select.h>
#include <sys/socket.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <cstring>
#include <set>
#include <vector>
#include <algorithm>

#define BUFLEN 1024
using namespace std;

char* IntToString(int);

struct PidSet{
	int size;
	int* arr;
	PidSet(){
		size = 0;
		arr = new int[0];
	}
	void add(int pid){
		int* newArr = new int[size+1];
		for(int i = 0; i < size; i++){
			newArr[i] = arr[i];
		}
		newArr[size] = pid;
		size++;
		delete [] arr;
		arr = newArr;
	}
	void erase(int pid){
		int* newArr = new int[size-1];
		int i = 0;
		for(i = 0; arr[i] != pid; i++) newArr[i] = arr[i];
		for(int j = i+1; j < size; j++) newArr[j-1] = arr[i];
		size--;
		delete [] arr;
		arr = newArr;
	}
	int & operator[](int index){
		return arr[index];
	}
};

PidSet pids;

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
		set_port(port);
		server_len = sizeof(server_addr);
	}
	void set_port(int port){
		this->port = port;
		server_addr.sin_port = htons(port);
	}
	int get_port() const{
		return port;
	}
	int get_socket() const{
		return server_sockfd;
	}
	void server_bind(){
		if(bind(server_sockfd, (struct sockaddr *)&server_addr, server_len) < 0){
			cerr << "Can't bind socket" << endl;
			exit(1);
		}
	}
	void server_listen(){
		if(listen(server_sockfd, 5) < 0){
			cerr << "Can't listen" << endl;
			exit(2);
		}
	}
};

class Client{
	int client_sockfd, len, fd;
	socklen_t client_len;
	sockaddr_in client_addr;
	char buf[BUFLEN];
	int getLength(int fd){
		char c;
		int len = 0;
		while(read(fd, &c, 1)) len++;
		lseek(fd, 0, 0);
		return len;
	}
	bool is_cgi(){
		char* last = get_last();
		for(unsigned int i = 0; i < strlen(last); i++){
			if(last[i] == '?'){
				delete [] last;
				return true;
			}
		}
		delete [] last;
		return false;
	}
	int get_amps(){
		char* last = get_last();
		int i = 0, n = 0;
		while(last[i] != '?') i++;
		while(last[i] != 0){
			if(last[i] == '&') n++;
			i++;
		}
		delete [] last;
		return n;
	}
	char* get_argv0(){
		char* last = get_last();
		int i = 0;
		while(last[i] != '?') i++;
		last[i] = 0;
		return last;
	}
	char* get_all_env(){
		char* all_env = new char[BUFLEN];
		int i = strlen(buf) - 1, k = 0;
		while(buf[i] != '?'){
			if(buf[i--] == 0){
				all_env[0] = 0;
				return all_env;
			}
		}
		for(unsigned int j = i+1; j < strlen(buf); j++) all_env[k++] = buf[j];
		return all_env;
	}
	char* get_first() const{
		char* first = new char[BUFLEN];
		strcpy(first, buf + 5);
		int i = 0;
		while(first[i++] != '?');
		first[i-1] = 0;
		return first;
	}
	char* get_last() const{
		char* last = new char[BUFLEN];
		int i = 5, j = 0;
		while(buf[i] != 0){
			if(buf[i] != '/') last[j++] = buf[i];
			else j = 0;
			i++;
		}
		last[j] = 0;
		return last;
	}
	void str_put(char* dest, const char* src){
		dest = new char[strlen(src) + 1];
		strcpy(dest, src);
		dest[strlen(src)] = 0;
	}
	void str_put2(char* dest, const char* src1, const char* src2){
		dest = new char[strlen(src1) + strlen(src2) + 1];
		strcpy(dest, src1);
		strcat(dest, src2);
		dest[strlen(src1) + strlen(src2)] = 0;
	}
public:
	void client_accept(Server & server){
		client_len = sizeof(client_addr);
		client_sockfd = accept(server.get_socket(), (struct sockaddr *)&client_addr, &client_len);
		if(client_sockfd < 0){
			cout << "Can't accept (not fatal error)" << endl;
		}
	}
	int client_read(){
		if((len = recv(client_sockfd, &buf, BUFLEN, 0)) < 0){
			shutdown(client_sockfd, 1);
			close(client_sockfd);
			cerr << "Error with reading socket" << endl;
			exit(3);
		}
		if(!isalpha(buf[0])) return -1;
		if(strncmp(buf, "GET /", 5)){
			client_send("501.html", "HTTP/1.0 501 NotImplemented");
			shutdown(client_sockfd, 1);
			close(client_sockfd);
			cerr << "Error: BadRequest" << endl;
			return 501;
		}
		int i = 5;
		while(buf[i] && (buf[i++] > ' '));
		buf[i-1] = 0;
		return 0;
	}
	int client_request(Server & server){
		if(client_read() == -1) return 0;
		if(is_cgi()){
			int pid = fork();
			if(pid < 0){
				cerr << "Не удалось создать процесс" << endl;
				exit(6);
			}else if(pid == 0){
				char** argv = new char*[2];
				argv[0] = get_argv0();
				argv[1] = NULL;
				char** env = new char*[7];
				str_put(env[0], "SERVER_ADDR=127.0.0.1");
				str_put(env[1], "CONTENT_TYPE=text/plain");
				str_put(env[2], "SERVER_PROTOCOL=HTTP/1.0");
				char* str_first = get_first();
				str_put2(env[3], "SCRIPT_NAME=", str_first);
				delete [] str_first;
				char* str_port = IntToString(server.get_port());
				str_put2(env[4], "SERVER_PORT=", str_port);
				delete [] str_port;
				char* str_all_env = get_all_env();
				str_put2(env[5], "QUERRY_STRING=", str_all_env);
				delete [] str_all_env;
				env[6] = NULL;
				char* str_pid = IntToString(getpid());
				char* filename = new char[strlen(str_pid) + 8];
				strcpy(filename, "tmp");
				strcat(filename, str_pid);
				strcat(filename, ".txt");
				filename[strlen(str_pid) + 7] = 0;
				creat(filename, 0666);
				int fd = open(filename, O_TRUNC | O_WRONLY);
				if(fd < 0){
					cout << "Ошибка при создании файла" << endl;
					exit(9);
				}
				dup2(fd, 1);
				close(fd);
				execvpe(argv[0], argv, env);
				cerr << "Не удалось запустить cgi" << endl;
				delete [] str_pid;
				delete [] filename;
				delete [] argv[0];
				delete [] argv[1];
				for(int i = 0; i < 7; i++) delete [] env[i];
				delete [] argv;
				delete [] env;
				exit(7);
			}else{
				pids.add(pid);
				return 1;
			}
		}else{
			if(strlen(buf) > 5 && (fd = open(buf+5, O_RDONLY)) < 0){
				client_send("404.html", "HTTP/1.0 404 PageNotFound");
				shutdown(client_sockfd, 1);
				close(client_sockfd);
				cerr << "Error: 404" << endl;
				return 404;
			}
			if(!strcmp(buf+5, "")) strcpy(buf+5, "index.html");
			client_send(buf+5, "HTTP/1.0 200 BikmetovDanilServer");
			shutdown(client_sockfd, 1);
			close(client_sockfd);
		}
		return 0;
	}
	void client_send(const char* file, const char* header){
		int fileLength;
		char* strFileLength;
		fd = open(file, O_RDONLY);
		fileLength = getLength(fd);
		strcpy(buf, header);
		strcat(buf, "\nAllow: GET\nServer: BikmetovDanilServer/0.1\nConnection: keep-alive\nContet-length: ");
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
	}
	char* get_buf(){
		return buf;
	}
	int get_socket() const{
		return client_sockfd;
	}
};

int main(int argc, char* argv[]){
	Server server;
	Client client;
	if(argc > 1) server.set_port(atoi(argv[1]));
	cout << "Server started " << "127.0.0.1" << ":" << server.get_port() << endl;
	server.server_bind();
	server.server_listen();
	
	fd_set readset;
	timeval timeout;
	int activity, status;
	
	while(1){
		FD_ZERO(&readset);
		FD_SET(server.get_socket(), &readset);
		
		if(pids.size == 0){
			activity = select(server.get_socket() + 1, &readset, NULL, NULL, NULL);
		}else{
			timeout.tv_sec = 0;
			timeout.tv_usec = 500000;
			activity = select(server.get_socket() + 1, &readset, NULL, NULL, &timeout);
		}
		
		if(activity < 0){
			cerr << "Error with select" << endl;
			exit(4);
		}else if(activity > 0){
			if(FD_ISSET(server.get_socket(), &readset)){
				fcntl(client.get_socket(), F_SETFL, O_NONBLOCK);
				client.client_accept(server);
				int req = client.client_request(server);
				if(req == 1) continue;
			}
		}
		
		for(int i = 0; i < pids.size; i++){
			if(waitpid(pids[i], &status, WNOHANG)){
				if(WIFEXITED(status)){
					char* str_pid = IntToString(pids[i]);
					char* filename = new char[strlen(str_pid) + 8];
					strcpy(filename, "tmp");
					strcat(filename, str_pid);
					strcat(filename, ".txt");
					client.client_send(filename, "HTTP/1.0 200 BikmetovDanilServer");
					remove(filename);
					delete[] str_pid;
					delete[] filename;
				}else{
					client.client_send("cgierror.html", "HTTP/1.0 500 BikmetovDanilServer");
				}
				pids.erase(pids[i]);
				shutdown(client.get_socket(), 1);
				close(client.get_socket());
			}
		}
	}
	return 0;
}

char* IntToString(int a){
	char* s = new char[10];
	sprintf(s, "%d", a);
	strcat(s, "\0");
	return s;
}
