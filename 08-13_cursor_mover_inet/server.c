#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include "cursor.h"
// -lX11
int main(int argc, char *argv[]){
	int sock;
	struct sockaddr_in server;
	int mysock;
	float buff[1024];
	int rval;
	int i;
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if(sock < 0){
		perror("Couldnt create socket");
		exit(1);
	}

	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons(5000);

	if(bind(sock, (struct sockaddr *)&server, sizeof(server))){
		perror("bind failed");
		exit(1);
	}

	listen(sock, 5);
	//Get system window
	Display *dpy;
	Window root_window;

	dpy = XOpenDisplay(0);
	root_window = XRootWindow(dpy, 0);

	while(1){
		mysock = accept(sock, (struct sockaddr *) 0, 0);
		if((rval = recv(mysock, buff, sizeof(buff), 0)) < 0){
			perror("accept failed");
		} else if (rval == 0){
			printf("Ending connection\n");
		} else {
			// for(i = 0; i < 3;i++)
			// 	printf("%i: %f\n",i, buff[i]);
			// printf("\n");
			int x = (int) buff[0];
			int y = (int) buff[1];
			printf("[%3d\t%3d]\n", x, y);
			XSelectInput(dpy, root_window, KeyReleaseMask);
			XWarpPointer(dpy, 				// display,
						 None,	 			// src_w,
						 root_window, 		// dest_w,
						 0, 0, 				// src_x, src_y,
						 0, 0, 				// src_width, src_height,
						 buff[0], buff[1] 	// dest_x, dest_y
						 );			

			XFlush(dpy);
		}

		// printf("Received message rval = %d\n", rval);
		close(mysock);
	} 

	return 0;
}