#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h> 

int main(int argc, char *argv[]) {
    int sockfd = 0;
    int n = 0;
    char recvBuff[1024];
    int i;

    // struct sockaddr_in {
    //     short            sin_family;   // e.g. AF_INET
    //     unsigned short   sin_port;     // e.g. htons(3490)
    //     struct in_addr   sin_addr;     // ip
    //     char             sin_zero[8];  // zero this if you want to
    // };

    // struct in_addr {
    //     unsigned long s_addr;  // load with inet_aton()
    // };    
    struct sockaddr_in serv_addr; 

    // make sure an ip was passed... kind of
    if(argc != 2) {
        printf("\n Usage: %s <ip of server> \n",argv[0]);
        return 1;
    } 

    // set recv_buff to repeating '0'
    memset(recvBuff, '0',sizeof(recvBuff));
    // open an internet socket (AF_INET) 
    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Error : Could not create socket \n");
        return 1;
    } 

    // blank out address to use as server socket
    memset(&serv_addr, '0', sizeof(serv_addr)); 

    serv_addr.sin_family = AF_INET;
    
    // convert connect port from x86 to network spec
    serv_addr.sin_port = htons(5000); 

    // inet_pton: ip char to bin
    if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0) {
        printf("\n inet_pton error occured\n");
        return 1;
    } 

    // connect: initiate a connection on a socket
    // int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
    if( connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
       printf("\n Error : Connect Failed \n");
       return 1;
    } 

    // use normal read once connection has succeeded
    for (i = 0; i < 3; i++){
        while ( (n = read(sockfd, recvBuff, sizeof(recvBuff)-1)) > 0) {
            recvBuff[n] = 0;
            if(fputs(recvBuff, stdout) == EOF) {
                printf("\n Error : Fputs error\n");
            }
        } 

        if(n < 0) {
            printf("\n Read error \n");
        } 
        sleep(1);
    }

    return 0;
}