//#ifdef HOST

#include <sys/types.h>
#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
#include <netinet/in.h>
#define SERVPORT 3333
#define BACKLOG 10
#define MAX_CONNECTED_NO 10
#define MAXDATASIZE 1024

int main()
{
	struct sockaddr_in server_sockaddr,client_sockaddr;
	socklen_t sin_size ;
	//socklen_t sin_size= sizeof(struct sockaddr);
	int recvbytes;
	int sockfd,client_fd;
	char buf[MAXDATASIZE]={0};
	//œšÁ¢socketÁ¬œÓ
                          //IPV4 xieyi
	if((sockfd = socket(AF_INET,SOCK_STREAM,0))== -1)
	{
		perror("socket");
		exit(1);
	}

	printf("socket success!,sockfd=%d\n",sockfd);
	//ÉèÖÃsockaddr_in œá¹¹ÌåÖÐÏà¹Ø²ÎÊý
	server_sockaddr.sin_family=AF_INET;
	                               //duankou
	server_sockaddr.sin_port=htons(SERVPORT);
	server_sockaddr.sin_addr.s_addr=INADDR_ANY;
	bzero(&(server_sockaddr.sin_zero),8);

	//°ó¶šº¯Êýbind
	if(bind(sockfd,(struct sockaddr *)&server_sockaddr,sizeof(struct
	sockaddr))== -1){
		perror("bind");
		exit(1);
	}
	printf("bind success!\n");
	//µ÷ÓÃlistenº¯Êý
	if(listen(sockfd,BACKLOG)== -1){
		perror("listen");
		exit(1);
	}

	printf("listening....\n");
	//µ÷ÓÃacceptº¯Êý£¬µÈŽý¿Í»§¶ËµÄÁ¬œÓ

	//memset(&client_sockaddr, 0, sizeof(struct sockaddr_in));

	sin_size = 1;
        printf("sin_size=%d\n",sin_size);

	if((client_fd=accept(sockfd,(struct sockaddr *)&client_sockaddr,&sin_size))==-1)
	{
		perror("accept");
		exit(1);
	}


	while(1)
	{
		//µ÷ÓÃrecvº¯ÊýœÓÊÕ¿Í»§¶ËµÄÇëÇó
		if((recvbytes=recv(client_fd,buf,100,0))== -1)
		{
			perror("recv");
			exit(1);
		}

		printf("received a connection :%s\n",buf);

		if(send(client_fd,buf,strlen(buf),0)== -1)
		{
			perror("send");
			exit(1);
		}

	}


	close(sockfd);
}

//#endif
