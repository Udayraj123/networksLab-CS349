#include <stdio.h>
#include <string.h> //for memset
#include <stdlib.h> //for strtoull // contains exit()
#include <sys/types.h>

/* The following three files must be included for network programming */
#include <sys/socket.h> 
#include <netinet/in.h> // Internet family of protocols
#include <arpa/inet.h> //for close
//extra for UDP
#include <unistd.h> //for close

enum {NA,MSG_TYPE_1,MSG_TYPE_2,MSG_TYPE_3,MSG_TYPE_4};
#define MAX_CLIENTS 5
#define MSG_LEN 100
#define MAX_BUF_TCP 100
#define MAX_BUF_UDP 1024

//enabler for custom user input for message content (else default values used)
#define USER_INPUT 0

#define DEBUG_RUN_DEF 1
#define dprintf(...) if(DEBUG_RUN_DEF)printf(__VA_ARGS__);
