#include <stdio.h>
#include <string.h> //for memset
#include <stdlib.h> //for strtoull // contains exit()
#include <sys/types.h>

/* The following three files must be included for network programming */
#include <sys/socket.h> 
#include <netinet/in.h> // Internet family of protocols
#include <arpa/inet.h>
//extra for UDP
#include <unistd.h>

#define SERVER_ADDR "127.0.0.1"
//#define SERVER_ADDR "172.16.114.241"
enum {MSG_TYPE_1,MSG_TYPE_2,MSG_TYPE_3};
#define MAX_CLIENTS 5
#define MSG_LEN 100
#define MAX_BUF_TCP 100
#define MAX_BUF_UDP 1024
#define TCP_PORT 8001
#define UDP_PORT 50000
