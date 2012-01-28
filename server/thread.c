#include <iostream>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <time.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include <pthread.h>
#include "thread.h"
#include "server_func.h"

/* Process Variables */
extern int sock, length, n;
extern struct sockaddr_in server;

extern int thread_no;
extern pthread_mutex_t mutex1;
extern pthread_mutex_t mutex2;
extern struct Thread *h_thread;		// list of threads - sorted on connection id

/* 
handle_request() :
1. if new request, finds timestamp of the specified file 
2. if done_ack received, then takes sets the corresponding shared variable
*/
void* handle_request(void *req){
	Request *request = (Request*)req;
	char packet_str[PACK_SIZE];
	strcpy(packet_str, request->buf);
	
	char buffer[BUF_SIZE], filename[BUF_SIZE];
	struct Packet packet;
	parse_packet(packet_str, packet);
	
	//printf("Received Packet Contents :\n");
	//print_packet(packet);
	
	// NEW CONNECTION //
	if(packet.connection_id < 0 ){
		//printf("\nNEW REQUEST/CONNECTION\n");
		pthread_mutex_lock(&mutex1);
		packet.connection_id = insert_thread();
		pthread_mutex_unlock(&mutex1);
		//printf("Connection ID Alloted: %d\n", packet.connection_id);
		packet.type = REQUEST_ACK;
		strcpy(filename, packet.buffer);
		allzero(packet.buffer, BUF_SIZE-1);
		
		strcpy(packet_str, "");
		ptos(packet.connection_id, packet.type, packet.status, packet.buffer, packet_str);
		//print_packet(packet_str);
		
		// Send REQUEST_ACK/REQUEST_REGRET //
		n = sendto(sock,packet_str, strlen(packet_str),0,(const struct sockaddr *)&request->from,request->fromlen);
		
		// exit if the server is too busy //
		if(packet.connection_id == -1){
			//write(1, "REQUEST_REGRET SENT\n", 20);
			pthread_exit(NULL);
		}
		else{
			//write(1, "REQUEST_ACK SENT\n", 17);
		}
		
		char *timestamp;
		//get_timestamp(filename, timestamp);
		struct stat fstat;
		if(stat(filename, &fstat) != 0){
			packet.status = 1;
		}
		timestamp = asctime(localtime(&fstat.st_ctime));
			
		//if(!strcmp(timestamp, "error"))
			//packet.status = 1;
		//printf("Last Modification Time\t:\t%s\n",timestamp);
		
		packet.type = DONE;
		strcpy(packet.buffer, timestamp);
		strcpy(packet_str, "");
		ptos(packet.connection_id, packet.type, packet.status, timestamp, packet_str);
		//print_packet(packet_str);
		//printf("Packet Length : %d\n", strlen(packet_str));

		// Send DONE //
		n = sendto(sock,packet_str,strlen(packet_str),0,(const struct sockaddr *)&request->from,request->fromlen);
		if(n < 0)
			error("sendto");
		//write(1, "DONE SENT\n", 10);

		// Wait to check if DONE_ACK is received //
		int done = 0;
		while(!done){
			pthread_mutex_lock(&mutex2);
			if(ack_received(packet.connection_id)){
				pthread_mutex_lock(&mutex1);
				remove_thread(packet.connection_id);
				pthread_mutex_unlock(&mutex1);
				//write(1, "DONE_ACK RECEIVED!\n", 19);
				done++;
			}
			pthread_mutex_unlock(&mutex2);
		}
		printf("REQUEST COMPLETED : %d\n", packet.connection_id);
	}
	
	// DONE ACK FOR AN EXISTING CONNECTION //
	else{
		//printf("\nEXISTING REQUEST/CONNECTION\n");
		//write(1,"NOTIFYING DONE_ACK\n",19);
		pthread_mutex_lock(&mutex2);
		insert_ack(packet.connection_id);
		pthread_mutex_unlock(&mutex2);
		//write(1, "ACK THREAD KILLING SELF!\n", 25);
	}
	delete(request);
	pthread_exit(NULL);
}

/* get_timestamp : returns timestamp (string) of the file fpath by using stat library function */
/*
void get_timestamp(char *fpath, char timestamp[]){
	struct stat fstat;
	char *error;
	strcpy(error, "error");
	bzero(&fstat, sizeof(fstat));
	if(stat(fpath, &fstat) != 0){
		timestamp = error;
	}
	//char time[BUF_SIZE];
	timestamp = asctime(localtime(&fstat.st_ctime));
	printf("TIME : %s\n", timestamp); 
	//return timestamp;
}
*/

/*insert_thread : determine an available connection id and insert a thread with that id in the list of threads */
short insert_thread(){
	struct Thread *new_thread = (Thread*)malloc(sizeof(Thread));	
	if(h_thread == NULL || h_thread->connection_id > 0){
		new_thread->next_thread = h_thread;
		h_thread = new_thread;
		return (new_thread->connection_id = 0);
	}
	struct Thread *lpt = h_thread;
	while(lpt->next_thread != NULL && lpt->next_thread->connection_id == (lpt->connection_id + 1))
		lpt = lpt->next_thread;
	if(lpt->connection_id == MAX_THREADS){
		delete(new_thread);
		return -1;
	}
	new_thread->next_thread = lpt->next_thread;
	lpt->next_thread = new_thread;
	return (new_thread->connection_id = (lpt->connection_id + 1));	
}

/* remove_thread() : remove the thread with the specified connection id from the list of threads */
void remove_thread(short conn_id){
	struct Thread *lpt = h_thread;
	if(h_thread->connection_id == conn_id){
		h_thread = h_thread->next_thread;
		delete(lpt);
		return;
	}
	while(lpt->next_thread->next_thread != NULL && lpt->next_thread->connection_id != conn_id)
		lpt = lpt->next_thread;
	if(lpt->next_thread->next_thread == NULL){
		delete(lpt->next_thread);
		lpt->next_thread = NULL;
		return;
	}
	struct Thread *lpt2 = lpt->next_thread;
	lpt->next_thread = lpt2->next_thread;
	delete(lpt2);
}
