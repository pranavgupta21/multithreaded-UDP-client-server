/* UDP client in the internet domain */
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "client_func.h"
#include "packet.h"

#define SERVER_PORT 7747

int main(int argc, char *argv[])
{
	/* Socket Variables */
	int sock, n;
	unsigned int length;
	struct sockaddr_in server, from;
	struct hostent *hp;

	/* check if sufficient arguments are provided */
	if(argc < 2){
		printf("Usage: server name or address\n");
		exit(1);
	}
	
	/* creating socket */
	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0)
		error("socket");

	server.sin_family = AF_INET;
	hp = gethostbyname(argv[1]);
	if(!hp)
		error("Unknown host");

	bcopy((char *)hp->h_addr, (char *)&server.sin_addr, hp->h_length);
	server.sin_port = htons(SERVER_PORT);
	length = sizeof(struct sockaddr_in);
	
	struct Packet packet;

	while(1){
		char filename[BUF_SIZE], packet_str[PACK_SIZE], buffer[BUF_SIZE];
		bzero(filename, BUF_SIZE);
		
		// check whether filename is provided as an argument, otherwise prompt for filename //
		if(argc < 3){
			printf("\nPlease enter the name of the file: ");
			scanf("%s", filename);
		}
		else{
			strcpy(filename, argv[2]);
		}
		bzero(buffer, BUF_SIZE);
		bzero(packet_str, PACK_SIZE);
		
		strcpy(buffer, filename);
		// prepare packet to be sent //
		strcpy(packet_str, "");
		ptos(-1, REQUEST, 0, buffer, packet_str);
		
		printf("Trying %s ... for File : %s\n", argv[1], filename);
		
		// Send REQUEST //
		n = sendto(sock,packet_str, strlen(packet_str),0,(const struct sockaddr *)&server,length);
		if(n < 0)
			error("Sendto");
		
		// Receive REQUEST_ACK //
		strcpy(packet_str, "");
		n = recvfrom(sock,packet_str,PACK_SIZE,0,(struct sockaddr *)&from, &length);
		if(n < 0)
			error("recvfrom");
		parse_packet(packet_str, packet);
		
		// Try Again if the server was too busy //
		if(packet.connection_id == -1){
			printf("The server is too busy !!\n");
			continue;
		}
		short conn_id = packet.connection_id;
		printf("The server %s is querying the timestamp of the file %s ... \n", argv[1], filename);
		
		// Receive DONE //
		strcpy(packet_str, "");
		char done_str[PACK_SIZE];
		n = recvfrom(sock,done_str,PACK_SIZE,0,(struct sockaddr *)&from, &length);
		if(n < 0)
			error("recvfrom");
		parse_packet(done_str, packet);
		
		// report to the user in case the file does not exist //
		if(packet.status == 1){
			printf("The file %s DOES NOT EXIST !!\n", filename);
		}
		else{
			printf("Timestamp of %s is %s\n", filename, packet.buffer);
				
			// Send DONE_ACK //
			strcpy(packet_str, "");
			allzero(buffer, BUF_SIZE - 1);
			ptos(conn_id, DONE_ACK, 0, buffer, packet_str);
			n = sendto(sock,packet_str, strlen(packet_str),0,(const struct sockaddr *)&server,length);
			if(n < 0)
				error("Sendto");
		}
		// close program if filename was provided as an argument //
		if(argc > 2)
			break;
	}

	close(sock);
	return 0;
}

