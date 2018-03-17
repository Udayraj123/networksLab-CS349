/****************************************
 * UDP Client : client_udp.c
 ****************************************/

#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h> // Internet family of protocols
#include <arpa/inet.h>
#include <unistd.h>

#define SERVERIP "172.16.114.241"
// #define SERVERIP "127.0.0.1"
#define PORT     50000
#define BUFFSIZE 1024

int main(){
    int socDes ;
    struct sockaddr_in	thatAddr ;
    char buff[BUFFSIZE] = "IIT Kgp" ;
    int dataBytes = 8, dataSent ;

    if((socDes = socket(PF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("cannot create socket") ;
        return 0 ;
    }

    thatAddr.sin_family = PF_INET ;
    thatAddr.sin_addr.s_addr = inet_addr(SERVERIP) ; // Server IP
                                        // Converts to 32-bit number
    thatAddr.sin_port = htons(PORT) ;  // Port number - byte order

    if((dataSent = sendto(socDes, buff, dataBytes, 0,
                      (struct sockaddr *)&thatAddr, sizeof(thatAddr))) < 0) {
        perror("cannot send") ;
        return 0 ;
    }


    close(socDes) ;
    
    return 0 ;
}
