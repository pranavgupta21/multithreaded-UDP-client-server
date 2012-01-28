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
#include "packet.h"

/* parse_packet() : takes the raw packet string, parses it and stores its 4 fields into a packet (struct Packet) */
void parse_packet(char *packet_str, struct Packet& packet){
	char ch = 'c', sign = '+';
	short byte_no = 0;
	
	// connection id //
	packet.connection_id = 0;
	sign = packet_str[byte_no];
	if(sign == '-'){
		byte_no++;	
	}
	while((ch = packet_str[byte_no++]) != SEPARATOR){
		packet.connection_id += ch - '0';
	}
	if(sign == '-'){
		packet.connection_id *= -1; 
	}
	
	// packet type //
	packet.type = (enum packet_type)(packet_str[byte_no++] - '0');
	byte_no++;
	// packet status //
	packet.status = packet_str[byte_no++] - '0';
	byte_no++;
	
	// buffer //
	short buf_byte = 0;
	while((packet.buffer[buf_byte++] = packet_str[byte_no++]) != '\0');
}

/* ptos() : takes the 4 fields of a packet and generates a raw string directly deliverable over the network. The fields in the packet are separated by a ';' */
void ptos(short conn_id, enum packet_type type, short status, char *fpath, char *packet_str){
	char type_str[2], conn_id_str[7], status_str[2];
	sprintf(conn_id_str, "%d", conn_id);
	sprintf(type_str, "%d", (int)type);
	sprintf(status_str, "%d", status);

	strcat(packet_str, conn_id_str);
	strcat(packet_str, ";");
	strcat(packet_str, type_str);
	strcat(packet_str, ";");
	strcat(packet_str, status_str);
	strcat(packet_str, ";");
	strcat(packet_str, fpath);
	//printf("Packet Length : %d\n", strlen(packet_str));
}

/* allzero - prepares a string of length size containing all 0s. Such a string with size BUF_SIZE (256) is used as buffer for REQUEST_ACK and DONE_ACK */
void allzero(char *str, int size){
	int buf_byte = 0;
	while(buf_byte < size){
		str[buf_byte++] = '0';
	}
	str[buf_byte] = '\0';
}

/* print_packet() : takes an instance of struct Packet and prints its indevidual fields. This function is generally used for debugging an tracking purposes */
void print_packet(struct Packet packet){
	printf("Conn_id : %d\nType : %d\nStatus : %d\nBuffer : %s\n", packet.connection_id, packet.type, packet.status, packet.buffer);
}

/* print_packet() : takes a packet string and prints it. */
void print_packet(char *packet_str){
	printf("\nPacket Contents (String Form): %s\n", packet_str);
}
