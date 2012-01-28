#include <iostream>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>
#include <stdio.h>
#include "server_func.h"

extern struct Ack *h_ack;				// list of acknowledgements

void error(const char *msg){
    perror(msg);
    exit(0);
}

/* insert_ack() : insert an acknowledgement at the head of the list of acknowledgements */
void insert_ack(short conn_id){
	struct Ack *new_ack = (Ack*)malloc(sizeof(Ack));
	new_ack->connection_id = conn_id;
	new_ack->next_ack = h_ack;
	h_ack = new_ack;
}

/* ack_received() : checks if DONE_ACK is received corresponding to conn_id
   if received : remove the ACK from the list of DONE_ACKs and return true
   else return false
*/
int ack_received(short conn_id){
	if(h_ack == NULL)
		return 0;
	else if(h_ack->next_ack == NULL){
		if(h_ack->connection_id == conn_id){
			delete(h_ack);
			h_ack = NULL;
			return 1;
		}
		else
			return 0;
	}
	struct Ack *lpt = h_ack;
	while(lpt->next_ack->next_ack != NULL && lpt->next_ack->connection_id != conn_id)
		lpt = lpt->next_ack;
	if(lpt->next_ack->next_ack != NULL){
		struct Ack *lpt2 = lpt->next_ack;
		lpt->next_ack = lpt2->next_ack;
		delete(lpt2);
		return 1;
	}
	else if(lpt->next_ack->connection_id == conn_id){
		delete(lpt->next_ack);
		lpt->next_ack = NULL;
		return 1;
	}
	return 0;
}
