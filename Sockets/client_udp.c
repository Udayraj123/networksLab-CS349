/****************************************
 * UDP Client : client_udp.c
 ****************************************/

#include "socketConf.h"
#include <unistd.h>


int main(){
    int socDes ;
    struct sockaddr_in	thatAddr ;
    char buff[MAX_BUF_UDP] = "Client UDP Message" ;
    int dataBytes = 8, dataSent ;

    if((socDes = socket(PF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("cannot create socket") ;
        return 0 ;
    }

    thatAddr.sin_family = PF_INET ;
    thatAddr.sin_addr.s_addr = inet_addr(SERVER_ADDR) ; // Server IP
                                        // Converts to 32-bit number
    thatAddr.sin_port = htons(UDP_PORT) ;  // UDP_PORT number - byte order

    if((dataSent = sendto(socDes, buff, dataBytes, 0,
                      (struct sockaddr *)&thatAddr, sizeof(thatAddr))) < 0) {
        perror("cannot send") ;
        return 0 ;
    }


    close(socDes) ;
    
    return 0 ;
}
