#include <iostream>
#include <sys/types.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <pthread.h>
#include "packet.h"

#define MAX_THREADS 32767			// Maximum number of threads that can be served at a time

struct Request{
	socklen_t fromlen;
	struct sockaddr_in from;
	char buf[PACK_SIZE];
};

struct Thread{
	int connection_id;
	struct Thread *next_thread;
};

//void get_timestamp(char*, char[]);
void* handle_request(void*);
short insert_thread();
void remove_thread(short);
