
/*				THE CLIENT PROCESS

	Please read the file server.c before you read this file. To run this,
	you must first change the IP address specified in the line:

		serv_addr.sin_addr.s_addr = inet_addr("144.16.202.221");

	to the IP-address of the machine where you are running the server.
*/
#include "socketUtils.h"


int udp_communicate(int received_port_number){
    int sock_udp,dataSent,dataBytes,srvAddrLen ;
    struct sockaddr_in	srvAddr ;

    if((sock_udp = socket(PF_INET, SOCK_DGRAM, 0)) == -1) {
        perror("cannot create socket") ;
        return 0 ;
    }
    else{
		dprintf("Created UDP socket with id : %d\n",sock_udp);
    }

    srvAddr.sin_family = PF_INET ;
    srvAddr.sin_addr.s_addr = inet_addr(SERVER_ADDR) ; // Server IP // inet_addr Converts it to 32-bit number
    srvAddr.sin_port = htons(received_port_number) ;  // UDP_PORT number - byte order
    srvAddrLen = sizeof(srvAddr) ;
	
    char* message =  "Data Message from Client over UDP" ;
    Message* Phase2Message1 = create_msg(MSG_TYPE_3,strlen(message),message);
    // try sending struct directly instead of buffer and note the behaviour
    if(send_msg_udp("Phase2Message1",sock_udp, (struct sockaddr *)&srvAddr, srvAddrLen, Phase2Message1) != 1){
        perror("cannot send") ;
        return 0 ;
    }

	char buff[MAX_BUF_UDP] ;
	dprintf("Waiting to receive the Type 4 msg...\n");
    //receive the Type 4 msg from Server and close the connection
    if((dataBytes=recvfrom(sock_udp, buff, MAX_BUF_UDP-1, 0, 	(struct sockaddr *)&srvAddr, &srvAddrLen)) < 0) {
    	perror("cannot receive") ;
    	return 0 ;
    }
	Message* Phase2Message2 = decodeCheckNPrint("Phase2Message2",MSG_TYPE_4,buff);

    close(sock_udp) ;
    return 0 ;
}

int main()
{
	int			sock_tcp ;
	struct sockaddr_in	serv_addr;

	int i;
	char buf[MAX_BUF_TCP];

	/* Opening a socket is exactly similar to the server process */
	if ((sock_tcp = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Unable to create socket\n");
		exit(0);
	}else{
		printf("Created a Socket with id : %d \n", sock_tcp);
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
	if ((connect(sock_tcp, (struct sockaddr *) &serv_addr,
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
	char* message =  "Request Message from Client over TCP" ;
    Message* Phase1Message1 = create_msg(MSG_TYPE_1,strlen(message),message);

    if(send_msg_tcp("Phase1Message1",sock_tcp, Phase1Message1) != 1){
        perror("cannot send") ;
		exit(1);
    }

	memset(buf,'\0', MAX_BUF_TCP*sizeof(char));
	// wait for receiving Type 2 from server.
	recv(sock_tcp, buf, MAX_BUF_TCP, 0);
	
	//decode here
	Message* Phase1Message2 = decodeCheckNPrint("Phase1Message2",MSG_TYPE_2,buf);	

	int received_port_number;// = strtoull(Phase1Message2, NULL, 10);
	sscanf(Phase1Message2->message,"%d",&received_port_number);// string to integer
	// printf("Receieved port: %d\n", received_port_number);
	
	close(sock_tcp);// frees the resources as well

// UDP port obtained, now make a UDP connection
	dprintf("Creating UDP socket \n");
	udp_communicate(received_port_number);

}

