#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <netdb.h>

int main(int argc, char *argv[]){
	int sock;
	struct sockaddr_in server;
	struct hostent *hp;
	float buff[1024];
	
	buff[0] = 666.0;
	buff[1] = 666.0;
	buff[2] = 666.0;
	int i = 0;
	int ms = 0;
	for(i = 0; i < 1000; i++) {
		sock = socket(AF_INET, SOCK_STREAM, 0);
		if(sock < 0){
			perror("Couldnt create socket");
			close(sock);
			exit(1);
		}

		server.sin_family = AF_INET;

		hp = gethostbyname(argv[1]);
		if(hp == 0){
			perror("gethostbyname failed");
			close(sock);
			exit(1);
		}

		memcpy(&server.sin_addr, hp->h_addr, hp->h_length);
		server.sin_port = htons(5000);

		if(connect(sock, (struct sockaddr *) &server, sizeof(server)) < 0){
			perror("connect failed");
			close(sock);
			exit(1);
		}
		if(send(sock, buff, sizeof(buff), 0) < 0){
			perror("gethostbyname failed");
			close(sock);
			exit(1);
		}
		buff[0]++;
		buff[1]++;
		buff[2]++;
		usleep(1000 * ms++);
		close(sock);
	}


	return 0;
}