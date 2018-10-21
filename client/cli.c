#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <netinet/in.h>
#include <netdb.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
int main(int argc, char **argv)
{
	 struct hostent *hname;
	 struct in_addr **addr_list;
	 int i;
struct sockaddr_in sa,cli;
int n,sockfd;
int len;char buff[100];
sockfd=socket(AF_INET,SOCK_STREAM,0);
if(sockfd<0){ printf("\nError in Socket");
exit(0);
}
else printf("\nSocket is Opened");
bzero(&sa,sizeof(sa));
sa.sin_family=AF_INET;
sa.sin_port=htons(9924);
inet_pton(AF_INET, argv[1], &sa.sin_addr);
if(connect(sockfd,(struct sockaddr*)&sa,sizeof(sa))<0)
{
printf("\nError in connection failed");
exit(0);
}
else
printf("\nconnected successfully");
if(n=read(sockfd,buff,sizeof(buff))<0)
{
printf("\nError in Reading");
exit(0);
}
else
{printf("\nMessage Read %s",buff);
}
// printf("%s\n", inet_ntoa(sa.sin_addr));

        if ((hname = gethostbyname(argv[1])) == NULL) {
            perror("hostname error");
            exit(2);
        }
        printf("IP address of %s : ",argv[1]);
        addr_list = (struct in_addr **)hname->h_addr_list;
        for(i = 0; addr_list[i] != NULL; i++) {
            printf("%s ", inet_ntoa(*addr_list[i]));
        }
        printf("\n");
	printf("%d\n", ntohs(sa.sin_port));
}
