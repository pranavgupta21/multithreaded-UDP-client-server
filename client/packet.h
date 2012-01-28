#include <iostream>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <pthread.h>

#define PACK_SIZE 270			// maximum size of a packet string			
#define BUF_SIZE 256			// maximum size of the buffer in a packet
#define SEPARATOR ';'			// SEPARATOR separates the different fields of a packet in a packet string

// ennuneration for packet type
enum packet_type {REQUEST, REQUEST_ACK, DONE, DONE_ACK};

struct Packet{
	short connection_id;
	enum packet_type type;
	short status;
	char buffer[BUF_SIZE];
};

void parse_packet(char*, struct Packet&);
void ptos(short, enum packet_type, short, char*, char*);
void allzero(char*, int);
void print_packet(struct Packet);
void print_packet(char*);
