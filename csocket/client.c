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

#define CONNECT_PORT 6789

int main(int argc, char *argv[]) {
    // socket file descriptor
    int socket_file_desc = 0;
    int read_return_code = 0;
    char recv_buff[1024];
    
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
    memset(recv_buff, '0', sizeof(recv_buff));
    // open an internet socket (AF_INET) 
    if((socket_file_desc = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        printf("\n Error : Could not create socket \n");
        return 1;
    } 

    // blank out address to use as server socket
    memset(&serv_addr, '0', sizeof(serv_addr)); 

    serv_addr.sin_family = AF_INET;

    // convert connect port from x86 to network spec
    serv_addr.sin_port = htons(CONNECT_PORT); 

    // inet_pton: ip char to bin
    if(inet_pton(AF_INET, argv[1], &serv_addr.sin_addr)<=0) {
        printf("\n inet_pton error occured\n");
        return 1;
    } 

    // connect: initiate a connection on a socket
    // int connect(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
    if( connect(socket_file_desc, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
       printf("\n Error : Connect Failed \n");
       return 1;
    } 

    // use normal read once connection has succeeded
    while ( (read_return_code = read(socket_file_desc, recv_buff, sizeof(recv_buff)-1)) > 0) {
        printf("%d\n", read_return_code);
        recv_buff[read_return_code] = 0;
        if(fputs(recv_buff, stdout) == EOF) {
            printf("\n Error : Fputs error\n");
        }
    } 

    if(read_return_code < 0) {
        printf("\n Read error \n");
    } 

    return 0;
}