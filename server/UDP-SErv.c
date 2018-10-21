#include"netinet/in.h"
#include"sys/socket.h"
#include"stdio.h"
#include"string.h"
#include"time.h"
main( )
{
	struct sockaddr_in sa;
	struct sockaddr_in cli;
	int sockfd,conntfd;
	int len,ch;
	char str[100];
	time_t tick;

//socket function

	sockfd=socket(AF_INET,SOCK_DGRAM,0);
	if(sockfd<0)
	{
		printf("error in socket\n");
		exit(0);
	}
	else 
		printf("Socket opened");
	
	bzero(&sa,sizeof(sa));
	sa.sin_port=htons(9924);
	sa.sin_addr.s_addr=htonl(0);
//Binding of socket to ip address and port no.	
	if(bind(sockfd,(struct sockaddr*)&sa,sizeof(sa))<0)
	{
		printf("Error in binding\n");
	}
	else
		printf("Binded Successfully");

//listens the clients requests andd max 50	
	listen(sockfd,50);
	
	for(;;)
	{
		len=sizeof(ch);
		//accept client request
		conntfd=accept(sockfd,(struct sockaddr*)&cli,&len);
		printf("Accepted");
		//prepare data to be sent to connected client
		tick=time(NULL);
		snprintf(str,sizeof(str),"%s",ctime(&tick));
		printf("%s",str);
		write(conntfd,str,100); //send data to client
	}
}
