
/*				THE CLIENT PROCESS

	Please read the file server.c before you read this file. To run this,
	you must first change the IP address specified in the line:

		serv_addr.sin_addr.s_addr = inet_addr("144.16.202.221");

	to the IP-address of the machine where you are running the server.
*/
#include "socketUtils.h"


int udp_communicate(int received_port_number){
    int socUDP,dataSent ;
    struct sockaddr_in	thatAddr ;

    if((socUDP = socket(PF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("cannot create socket") ;
        return 0 ;
    }

    thatAddr.sin_family = PF_INET ;
    thatAddr.sin_addr.s_addr = inet_addr(SERVER_ADDR) ; // Server IP // inet_addr Converts it to 32-bit number
    thatAddr.sin_port = htons(received_port_number) ;  // UDP_PORT number - byte order
	
    char* message =  "Type 3 Message from Client over UDP" ;
    Message* Phase2Message1 = create_msg(MSG_TYPE_3,strlen(message),message);
    // try sending struct directly instead of buffer and note the behaviour
    if(send_msg_udp(socUDP, (struct sockaddr *)&thatAddr, sizeof(thatAddr), Phase2Message1) != 1){
        perror("cannot send") ;
        return 0 ;
    }

    close(socUDP) ;
    
    return 0 ;
}

int main()
{
	int			sockfd ;
	struct sockaddr_in	serv_addr;

	int i;
	char buf[MAX_BUF_TCP];

	/* Opening a socket is exactly similar to the server process */
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Unable to create socket\n");
		exit(0);
	}else{
		printf("Socket successfully created with id : %d \n", sockfd);
	}

	/* Recall that we specified INADDR_ANY when we specified the server
	   address in the server. Since the client can run on a different
	   machine, we must specify the IP address of the server. 

	   TO RUN THIS CLIENT, YOU MUST CHANGE THE IP ADDRESS SPECIFIED
	   BELOW TO THE IP ADDRESS OF THE MACHINE WHERE YOU ARE RUNNING
	   THE SERVER. 

    	*/
	serv_addr.sin_family		= AF_INET;
	// serv_addr.sin_addr.s_addr	= inet_addr("144.16.202.221");
	serv_addr.sin_addr.s_addr	= inet_addr(SERVER_ADDR);
	// htons makes it support communication in bigendian and smallendian machines
	
	/* https://stackoverflow.com/questions/33985763/difference-between-htons-and-htonl
	htons host to network short
	ntohs network to host short
	htonl host to network long
	ntohl network to host long
	long is 32 bits. short is 16 bits.
	*/
	serv_addr.sin_port		= htons(TCP_PORT); 

	/* With the information specified in serv_addr, the connect()
	   system call establishes a connection with the server process.
	*/
	if ((connect(sockfd, (struct sockaddr *) &serv_addr,
						sizeof(serv_addr))) < 0) {
		printf("Unable to connect to server\n");
		exit(0);
	}

	/* After connection, the client can send or receive messages.
	   However, please note that recv() will block when the
	   server is not sending and vice versa. Similarly send() will
	   block when the server is not receiving and vice versa. For
	   non-blocking modes, refer to the online man pages.
	*/
	// strcpy(buf,"TCP Message from client");

	// Start of communication : send Type 1 to server
	char* message =  "Type 1 Request Message from Client over TCP" ;
    Message* Phase1Message1 = create_msg(MSG_TYPE_1,strlen(message),message);

    if(send_msg_tcp(sockfd, Phase1Message1) != 1){
        perror("cannot send") ;
		exit(1);
    }

	memset(buf,'\0', MAX_BUF_TCP*sizeof(char));
	// wait for receiving Type 2 from server.
	recv(sockfd, buf, MAX_BUF_TCP, 0);
	
	//decode here
	Message* Phase1Message2 = deserialize_msg(buf);
	printf("received Phase1Message2: %d, %d, %s\n", Phase1Message2->type,Phase1Message2->len,Phase1Message2->message);

	// int received_port_number = strtoull(buf, NULL, 10);
	// printf("Receieved port: %d\n", received_port_number);
	
	close(sockfd);// frees the resources as well
	

// UDP port obtained, now make a UDP connection
	// udp_communicate(received_port_number);

}

