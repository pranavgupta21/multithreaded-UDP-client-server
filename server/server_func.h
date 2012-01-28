#include <iostream>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>

#define SERVER_PORT 7747

// ennumeration for true/false variables
enum tf{f,t};

struct Ack{
	short connection_id;
	struct Ack *next_ack; 
};

void error(const char*);
void insert_ack(short);
int ack_received(short);
