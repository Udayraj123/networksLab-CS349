/*
			NETWORK PROGRAMMING WITH SOCKETS

In this program we illustrate the use of Berkeley sockets for interprocess
communication across the network. We show the communication between a server
process and a client process.

Since many server processes may be running in a system, we identify the
desired server process by a "port number". Standard server processes have
a worldwide unique port number associated with it. For example, the port
number of SMTP (the sendmail process) is 25. To see a list of server
processes and their port numbers see the file /etc/services

In this program, we choose port number 6000 for our server process. Here we
shall demonstrate TCP connections only. For details and for other types of
connections see:

	 Unix Network Programming
		-- W. Richard Stevens, Prentice Hall India.

To create a TCP server process, we first need to open a "socket" using the
socket() system call. This is similar to opening a file, and returns a socket
descriptor. The socket is then bound to the desired port number. After this
the process waits to "accept" client connections.

*/
#include "socketUtils.h"

			/* THE SERVER PROCESS */

	/* Compile this program with cc server.c -o server
	   and then execute it as ./server &
	*/

int udp_communicate(int sent_port_no,int sock_udp_listen){
	
	int dataBytes, thatAddrLen ;

	char buff[MAX_BUF_UDP] ;
	struct sockaddr_in thatAddr;
	thatAddrLen = sizeof(thatAddr) ;
	dprintf("%s","Waiting to receive Phase2Message1...\n");
    // server "listens" here
	if((dataBytes=recvfrom(sock_udp_listen, buff, MAX_BUF_UDP-1, 0, 	(struct sockaddr *)&thatAddr, &thatAddrLen)) < 0) {
		perror("cannot receive") ;
		return 0 ;
	}

	buff[dataBytes] = '\0' ;
    // Decode the message obtained
	Message* Phase2Message1 = decodeCheckNPrint("Phase2Message1",MSG_TYPE_3,buff);

    // send a type 4 message and close the connection.
	char* msg="ACK message from server";
	Message* Phase2Message2 = create_msg(MSG_TYPE_4,strlen(msg),msg);

	//send this udp_no to client over TCP
	if(send_msg_udp("Phase2Message2",sock_udp_listen, (struct sockaddr *)&thatAddr, sizeof(thatAddr), Phase2Message2) != 1){
		perror("cannot send") ;
		exit(1);
	}
	close(sock_udp_listen) ;
	dprintf("Closed UDP socket %d (child-id %d)\n", sock_udp_listen, getpid());
	return 0 ;
}

int generateUDPport(int * sock_udp){

	int sock_udp_listen ;
	struct sockaddr_in	thisAddr;
	if((sock_udp_listen = socket(PF_INET, SOCK_DGRAM, 0)) == -1) {
		perror("cannot create udp listener socket") ;
		return 0 ;
	} else{
		dprintf("Created UDP socket %d (child-id %d)\n", sock_udp_listen, getpid());
	}

	thisAddr.sin_family = PF_INET ;
    thisAddr.sin_addr.s_addr = INADDR_ANY ; // inet_addr("127.0.0.1") ; // Converts to 32-bit number
    thisAddr.sin_port = htons(0) ;  // UDP_PORT number - byte order - 0 does the generation of free port 

    if((bind(sock_udp_listen, (struct sockaddr *)&thisAddr, sizeof(thisAddr))) < 0) {
    	perror("cannot bind to udp socket") ;
    	exit(1);
    }

    struct sockaddr_in localSocket;
    socklen_t addressLength = sizeof localSocket;
    // loads local socket to find assigned port
    getsockname(sock_udp_listen, (struct sockaddr*)&localSocket,&addressLength);
    *sock_udp = sock_udp_listen;
    // print assigned ports
    return (int) ntohs(localSocket.sin_port);
}

int main (int argc, char **argv)
{
	int serv_port;
	if (argc !=2)
	{
		perror("Usage: <Server TCP Port>\n");
		exit(1);
	}
	else{
		serv_port = atoi(argv[1]);
	}

	int			sock_tcp_listen, child_sock_tcp ; /* Socket descriptors */
	int			clilen;
	struct sockaddr_in	cli_addr, serv_addr;
	int i;
	char buf[MAX_BUF_TCP];		/* We will use this buffer for communication */

	/* The following system call opens a socket. The first parameter
	   indicates the family of the protocol to be followed. For internet
	   protocols we use AF_INET. For TCP sockets the second parameter
	   is SOCK_STREAM. The third parameter is set to 0 for user
	   applications.
	*/
	if ((sock_tcp_listen = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		printf("Cannot create socket\n");
		exit(0);
	}

	/* The structure "sockaddr_in" is defined in <netinet/in.h> for the
	   internet family of protocols. This has three main fields. The
 	   field "sin_family" specifies the family and is therefore AF_INET
	   for the internet family. The field "sin_addr" specifies the
	   internet address of the server. This field is set to INADDR_ANY
	   for machines having a single IP address. The field "sin_port"
	   specifies the port number of the server.
	*/
	serv_addr.sin_family		= AF_INET;
	serv_addr.sin_addr.s_addr	= INADDR_ANY;
	serv_addr.sin_port		= htons(serv_port);

	/* With the information provided in serv_addr, we associate the server
	   with its port using the bind() system call. 
	*/
	if (bind(sock_tcp_listen, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
		// https://stackoverflow.com/questions/5106674/error-address-already-in-use-while-binding-socket-with-address-but-the-port-num
		printf("Unable to bind local address, Maybe kernel didn't clear the socket yet. Wait max 4 minutes!\n");
		exit(0);
	}
	//try to specify maximum pending connections for the master socket sock_tcp_listen
	if (listen(sock_tcp_listen, MAX_CLIENTS) < 0)  
	{  
        perror("Listen error"); /* (Doesn't wait) This specifies that up to 5 concurrent client
			      requests will be queued up while the system is
			      executing the "accept" system call below.
			   */
		exit(1);  
	}  

	/* In this program we are illustrating a concurrent server -- one
	   which forks to accept multiple client connections concurrently.
	   As soon as the server accepts a connection from a client, it
	   forks a child which communicates with the client, while the
	   parent becomes free to accept a new connection. To facilitate
	   this, the accept() system call returns a new socket descriptor
	   which can be used by the child. The parent continues with the
	   original socket descriptor.
	*/
	dprintf("%s","Waiting for connection...\n");

	while (1) {

		/* The accept() system call accepts a client connection.
		   It blocks the server until a client request comes.

		   The accept() system call fills up the client's details
		   in a struct sockaddr which is passed as a parameter.
		   The length of the structure is noted in clilen. Note
		   that the new socket descriptor returned by the accept()
		   system call is stored in "child_sock_tcp".
		*/
		clilen = sizeof(cli_addr);
		// accept creates and returns a new socket
		
		child_sock_tcp = accept(sock_tcp_listen, (struct sockaddr *) &cli_addr, &clilen) ; // loads Client info into cli_addr 
		char * client_ip = inet_ntoa(cli_addr.sin_addr);
		int client_port = ntohs(cli_addr.sin_port);
		dprintf("Connection established: Client IP: %s  Client Port: %d\n",client_ip,client_port ); 
		// above is a blocking statement, hence the process busy waits until a connection request is received. 
		// Then the accepted connection is processed in a new fork below.

		if (child_sock_tcp < 0) {
			printf("Accept error\n");
			exit(0);
		}

		/* Having successfully accepted a client connection, the
		   server now forks. The parent closes the new socket
		   descriptor and loops back to accept the next connection.
		*/
		// ONLY CHILD GOES INTO THIS LOOP-
		if (fork() == 0) {// memspace is duplicated immediately

			/* This child process will now communicate with the
			   client through the send() and recv() system calls.
			*/
			close(sock_tcp_listen);	/* Close the old socket since all
					   communications will be through
					   the new socket.
					*/		  

			/* We initialize the buffer, and receive a 
			   message from the client. 
			*/
			// for(i=0; i < MAX_BUF_TCP; i++) buf[i] = '\0';
			memset(buf,'\0', MAX_BUF_TCP*sizeof(char));

			dprintf("%s","Waiting to receive Phase1Message1...\n");
			// Start of communication: receive Type 1 msg from a client
            if(recv(child_sock_tcp, buf, MAX_BUF_TCP, 0) == 0)//receive the byte sequence into buffer
            {
            	perror("The client terminated prematurely\n");
            	exit(1);
            }
			//Decode and check for Type == 1
            Message* Phase1Message1 = decodeCheckNPrint("Phase1Message1",MSG_TYPE_1,buf);

			// Generate a UPD port for this client <- Use the system feature i.e. pass port 0
            int sock_udp_listen;
            int udp_no = generateUDPport(&sock_udp_listen);
            
            memset(buf,'\0', MAX_BUF_TCP*sizeof(char));
			sprintf(buf,"%d",udp_no); // ref for this function: https://stackoverflow.com/questions/5050067/simple-int-to-char-conversion
			Message* Phase1Message2 = create_msg(MSG_TYPE_2,strlen(buf),buf);

			//send this udp_no to client over TCP
			if(send_msg_tcp("Phase1Message2",child_sock_tcp, Phase1Message2) != 1){
				perror("cannot send") ;
				exit(1);
			}

			dprintf("Closing TCP socket %d at port %d with client (%s)\n", child_sock_tcp,serv_port,client_ip);
			close(child_sock_tcp);

			//Now exchange over udp here. (No need to fork for UDP as we've already closed the TCP connection)
			udp_communicate(udp_no,sock_udp_listen);

			dprintf("Communication Done. Killed child connection process (%d).\n",getpid());
			fflush(stdout);
			exit(0);//kill the child
		}
		//PARENT CONTINUES THIS LOOP
		// parent closes this socket as it's only to be used by child
		close(child_sock_tcp); 
	}

	/*

    // Simple way to finish both processes
    do {
        int status;
        pid_t finished_pid = waitpid(-1, &status, 0);

        printf("Process %d finished\n", finished_pid);

        if (finished_pid == tcp_pid)
            tcp_pid = -1;
        if (finished_pid == udp_pid)
            udp_pid = -1;

    } while (tcp_pid != -1 || udp_pid != -1);

    */
}


