/******************************************
 * UDP Iterative server: server_udp.c
 ******************************************/
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> // Internet family of protocols
#include <arpa/inet.h>
#include <unistd.h>

#define PORT 50000
#define BUFFSIZE 1024

int main(){
    int socDes ;
    struct sockaddr_in	thisAddr, thatAddr;
    int dataBytes, thatAddrLen ;
    char buff[BUFFSIZE] ;

    if((socDes = socket(PF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("cannot create socket") ;
        return 0 ;
    }   

    thisAddr.sin_family = PF_INET ;
    thisAddr.sin_addr.s_addr = INADDR_ANY ; // inet_addr("127.0.0.1") ; 
                                        	  // Converts to 32-bit number
    thisAddr.sin_port = htons(PORT) ;  // Port number - byte order

    if((bind(socDes, (struct sockaddr *)&thisAddr, sizeof(thisAddr))) < 0) {
        perror("cannot bind") ;
        return 0 ;
    }


    thatAddrLen = sizeof(thatAddr) ;

    if((dataBytes=recvfrom(socDes, buff, BUFFSIZE-1, 0,
                           (struct sockaddr *)&thatAddr, &thatAddrLen)) < 0) {
                 perror("cannot receive") ;
                 return 0 ;
             }

    buff[dataBytes] = '\0' ;
    printf("Data received: %s\n", buff) ;

    return 0 ;
}
