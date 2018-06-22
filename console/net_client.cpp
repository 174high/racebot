/*client.c*/

//#ifdef CLIENT

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#define SERVPORT 3333
#define MAXDATASIZE 1024
int main(int argc,char *argv[]){

	int sockfd,sendbytes;
	char buf[MAXDATASIZE]={0};
	struct hostent *host;
	struct sockaddr_in serv_addr;

	if(argc < 2){
	fprintf(stderr,"Please enter the server's hostname!\n");
	exit(1);
	}

	//µØÖ·œâÎöº¯Êý
	if((host=gethostbyname(argv[1]))==NULL){
	perror("gethostbyname");
	exit(1);
	}
	//ŽŽœšsocket
	if((sockfd=socket(AF_INET,SOCK_STREAM,0))== -1){
	perror("socket");
	exit(1);
	}

	//ÉèÖÃsockaddr_in œá¹¹ÌåÖÐÏà¹Ø²ÎÊý
	serv_addr.sin_family=AF_INET;
	serv_addr.sin_port=htons(SERVPORT);
	serv_addr.sin_addr=*((struct in_addr *)host->h_addr);
	bzero(&(serv_addr.sin_zero),8);
	//µ÷ÓÃconnectº¯ÊýÖ÷¶¯·¢Æð¶Ô·þÎñÆ÷¶ËµÄÁ¬œÓ
	if(connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(struct sockaddr))== -1)
	{
		perror("connect");
		exit(1);
	}

	while(1)
	{
		gets(buf);

		if(send(sockfd,buf,strlen(buf),0)== -1)
		{
			perror("send");
			exit(1);
		}

		//µ÷ÓÃrecvº¯ÊýœÓÊÕ¿Í»§¶ËµÄÇëÇó
		if((recv(sockfd,buf,100,0))== -1)
		{
			perror("recv");
			exit(1);
		}

		printf("received a connection :%s\n",buf);

	}

        //close(sockfd);

}

//#endif

