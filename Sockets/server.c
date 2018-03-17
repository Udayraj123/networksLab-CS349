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

int udp_communicate(int sent_port_no){
	int socUDP ;
	struct sockaddr_in	thisAddr, thatAddr;
	int dataBytes, thatAddrLen ;
	char buff[MAX_BUF_UDP] ;

	if((socUDP = socket(PF_INET, SOCK_DGRAM, 0)) == -1) {
		perror("cannot create socket") ;
		return 0 ;
	}   

	thisAddr.sin_family = PF_INET ;
    thisAddr.sin_addr.s_addr = INADDR_ANY ; // inet_addr("127.0.0.1") ; 
                                        	  // Converts to 32-bit number
    thisAddr.sin_port = htons(sent_port_no) ;  // UDP_PORT number - byte order

    if((bind(socUDP, (struct sockaddr *)&thisAddr, sizeof(thisAddr))) < 0) {
    	perror("cannot bind") ;
    	return 0 ;
    }


    thatAddrLen = sizeof(thatAddr) ;
    // server "listens" herre
    if((dataBytes=recvfrom(socUDP, buff, MAX_BUF_UDP-1, 0, 	(struct sockaddr *)&thatAddr, &thatAddrLen)) < 0) {
    	perror("cannot receive") ;
    	return 0 ;
    }

    buff[dataBytes] = '\0' ;
    // Decode the message obtained
    printf("Data received from Client: '%s'\n", buff) ;
    // send a type 4 message and close the connection.
    printf("send a type 4 message and close the connection.\n");
    close(socUDP) ;

    return 0 ;
}


int main()
{
	int			sockfd, newsockfd ; /* Socket descriptors */
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
	if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
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
	serv_addr.sin_port		= htons(TCP_PORT);

	/* With the information provided in serv_addr, we associate the server
	   with its port using the bind() system call. 
	*/
	if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) {
		printf("Unable to bind local address\n");
		exit(0);
	}

	listen(sockfd, MAX_CLIENTS); /* (Doesn't wait) This specifies that up to 5 concurrent client
			      requests will be queued up while the system is
			      executing the "accept" system call below.
			   */

	/* In this program we are illustrating a concurrent server -- one
	   which forks to accept multiple client connections concurrently.
	   As soon as the server accepts a connection from a client, it
	   forks a child which communicates with the client, while the
	   parent becomes free to accept a new connection. To facilitate
	   this, the accept() system call returns a new socket descriptor
	   which can be used by the child. The parent continues with the
	   original socket descriptor.
	*/
	while (1) {

		/* The accept() system call accepts a client connection.
		   It blocks the server until a client request comes.

		   The accept() system call fills up the client's details
		   in a struct sockaddr which is passed as a parameter.
		   The length of the structure is noted in clilen. Note
		   that the new socket descriptor returned by the accept()
		   system call is stored in "newsockfd".
		*/
		clilen = sizeof(cli_addr);
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr,
			&clilen) ;
		// above is a blocking statement, hence the process busy waits until a connection request is received. 
		// Then the accepted connection is processed in a new fork below.

		if (newsockfd < 0) {
			printf("Accept error\n");
			exit(0);
		}

		/* Having successfully accepted a client connection, the
		   server now forks. The parent closes the new socket
		   descriptor and loops back to accept the next connection.
		*/
		if (fork() == 0) {// memspace is duplicated immediately

			/* This child process will now communicate with the
			   client through the send() and recv() system calls.
			*/
			close(sockfd);	/* Close the old socket since all
					   communications will be through
					   the new socket.
					*/		  

			/* We initialize the buffer, and receive a 
			   message from the client. 
			*/
			// for(i=0; i < MAX_BUF_TCP; i++) buf[i] = '\0';
			memset(buf,'\0', MAX_BUF_TCP*sizeof(char));

			// Start of communication: receive Type 1 msg from a client
			recv(newsockfd, buf, MAX_BUF_TCP, 0);//receive the byte sequence into buffer
			//Decode and check for Type == 1
			Message* Phase1Message1 = deserialize_msg(buf);
			printf("received Phase1Message1: %d, %d, %s\n", Phase1Message1->type,Phase1Message1->len,Phase1Message1->message);


			// Generate a UPD port for this client
			int udp_no = UDP_PORT;
			memset(buf,'\0', MAX_BUF_TCP*sizeof(char));
			sprintf(buf,"%d",udp_no); // ref for this function: https://stackoverflow.com/questions/5050067/simple-int-to-char-conversion
		    Message* Phase1Message2 = create_msg(MSG_TYPE_1,strlen(buf),buf);

			//send this udp_no to client over TCP
		    if(send_msg_tcp(newsockfd, Phase1Message2) != 1){
		        perror("cannot send") ;
				exit(1);
			}
			printf("Sent UDP port to client: %d\n", udp_no);
		    
			close(newsockfd);

			//Now exchange over udp here. (No need to fork for UDP as we've already closed the TCP connection)
			// udp_communicate(udp_no);

			exit(0);
		}

		close(newsockfd);
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


