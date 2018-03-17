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
        perror("Error: Max Message Length exceeded");
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


int send_msg_udp(int socket,struct sockaddr *dest, socklen_t dlen, Message *msg)
{
	char buffer[sizeof(Message)+1], *ptr;
	ptr = serialize_msg(buffer, msg);
	return sendto(socket, buffer, ptr - buffer, 0, dest, dlen) == ptr - buffer;
}
int send_msg_tcp(int socket,Message *msg)
{
	char buffer[sizeof(Message)+1], *ptr;
	ptr = serialize_msg(buffer, msg);
	return send(socket, buffer, ptr - buffer, 0) == ptr - buffer;
}