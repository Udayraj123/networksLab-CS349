#include "socketConf.h"

typedef struct Message{
	int type;
	int len;
	char message[MSG_LEN];
} Message;
char * serialize_int(char *buffer, int value)
{
  /* Write big-endian(std method) int value into buffer; assumes 32-bit int and 8-bit char. */
  // Once this works, try replacing with htonl, it'd still work
	buffer[0] = value >> 24;
	buffer[1] = value >> 16;
	buffer[2] = value >> 8;
	buffer[3] = value;
	return buffer + 4;
}


char * serialize_char(char *buffer, char value)
{
	buffer[0] = value;
	return buffer + 1;
}

char * serialize_string(char *buffer, char* value)
{
	// TODO: make a test for empty strings
	int len =strlen(value);
	//copy with the (expected) '\0';
	for (int i = 0; i < len+1; ++i)
		buffer[i]=value[i];
	return buffer + len + 1;
}

char * serialize_msg(char *buffer, Message *value)
{
	buffer = serialize_int(buffer, value->type);
	buffer = serialize_int(buffer, value->len);
	buffer = serialize_string(buffer, value->message);
	return buffer;
}

char * deserialize_int(char *buffer, int *value){
  /* Read big-endian(std method) int value into buffer; assumes 32-bit int and 8-bit char. */
  // Once this works, try replacing with ntohl, it'd still work
	int ans = 0;
	for (int i = 0; i < 4; ++i)
	{
		if(i > 0)ans <<= 8;
		ans |= (int)(buffer[i]) & 0xFF;// take 8 bits only, others zero
		// printf("%d\n", (int)(buffer[i]) & 0xFF);
	}
	*value = ans;
	return buffer + 4;
}


Message* create_msg(int type, int len, char* buffer){
	
	Message* msg = (Message*)malloc(sizeof(Message));
	msg->type=type;
	msg->len=len;
	// loop used & strcpy not because we can't guarantee presence of a '\0'
	// +1 to include an expected '\0'
	if(len > MSG_LEN){
		printf("Error: Max Message Length exceeded");
        // exit(1);
	}
	for (int i = 0; i < len+1; ++i){
		msg->message[i]=buffer[i];
	}
	return msg;
}

Message* deserialize_msg(char* buffer){
	int type,len;
	buffer = deserialize_int(buffer,&type);
	buffer = deserialize_int(buffer,&len);
	return create_msg(type,len,buffer);
}


int send_msg_udp(char* msgTitle,int socket,struct sockaddr *cliaddr, socklen_t dlen, Message *msg)
{
	// getpid(), inet_ntoa(cliaddr.sin_addr)
	printf("Sending Type-%d UDP msg : |%d|%d|'%s'| (stage: %s)\n",msg->type,msg->type,msg->len,msg->message,msgTitle);
	char buffer[sizeof(Message)+1], *ptr;
	ptr = serialize_msg(buffer, msg);
	return sendto(socket, buffer, ptr - buffer, 0, cliaddr, dlen) == ptr - buffer;
}
int send_msg_tcp(char* msgTitle,int socket,Message *m)
{
	printf(" Sending Type-%d TCP msg : |%d|%d|'%s'| (stage: %s)\n",m->type,m->type,m->len,m->message,msgTitle);
	char buffer[sizeof(Message)+1], *ptr;
	ptr = serialize_msg(buffer, m);
	return send(socket, buffer, ptr - buffer, 0) == ptr - buffer;
}

Message* decodeCheckNPrint( char* msgTitle,int MSG_TYPE, char* buf){
	Message* m = deserialize_msg(buf);
	if(m->type!=MSG_TYPE){
		printf("Note: Unexpected message type, expected Type-%d, got Type-%d\n",MSG_TYPE,m->type);
	}
	if(m->len != strlen(m->message)){
		printf("Note: Length mismatch occurred for ");
		perror(msgTitle);
	}
	//printf("Received Type-%d message: %s (%s)\n",m->type,m->message,msgTitle);
	printf("Received Type-%d Message: |%d|%d|'%s'| (stage: %s)\n",m->type,m->type,m->len,m->message,msgTitle);

	return m;
}
