#include <stdio.h>
#include <iostream>
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#define SERVPORT 3333
#define MAXDATASIZE 1024


#define speed_level_1  0x02 
#define speed_level_2  0x03   
#define speed_level_3  0x04 
#define speed_level_4  0x05
#define speed_level_5  0x06 
#define speed_level_6  0x07
#define speed_level_7  0x08 
#define speed_level_8  0x09
#define speed_level_9  0x0a 
#define speed_level_10 0x0b

#define run_forward   103
#define run_backward  108
#define turn_left     105   
#define turn_right    106

#define botton_status_on  2 
#define botton_status_off 0  

int main(int argc,char** argv)
{
    	int keys_fd;
    	char ret[2];
    	struct input_event t;
    	int sockfd,sendbytes;
    	char buf[MAXDATASIZE]={0};
    	struct hostent *host;
    	struct sockaddr_in serv_addr;


	if((host=gethostbyname("192.168.0.108"))==NULL){
        	perror("gethostbyname");
        	exit(1);
    	}

        //ŽŽœšsocket
        if((sockfd=socket(AF_INET,SOCK_STREAM,0))== -1){
        perror("socket");
        exit(1);
        }

        serv_addr.sin_family=AF_INET;
        serv_addr.sin_port=htons(SERVPORT);
        serv_addr.sin_addr=*((struct in_addr *)host->h_addr);
        bzero(&(serv_addr.sin_zero),8);


        if(connect(sockfd,(struct sockaddr *)&serv_addr,sizeof(struct sockaddr))== -1)
        {
                perror("connect");
                exit(1);
        }

    	keys_fd=open("/dev/input/event2",O_RDONLY);

    	if(keys_fd<=0)
    	{
        	printf("error\n");
        	return -1;
    	}

    	while(1)
    	{
                read(keys_fd,&t,sizeof(struct input_event));
                if(t.type==1)
                        printf("key %i state %i \n",t.code,t.value);

                while(((run_forward==t.code)||(run_backward==t.code)||\
                (turn_left==t.code)||(turn_right==t.code))&&\
                (botton_status_on==t.value)) 
                {  
                    printf("test\n");

                    read(keys_fd,&t,sizeof(struct input_event));
                    if(t.type==1)
                    	printf("key %i state %i \n",t.code,t.value);   

                    while(((run_forward==t.code)||(run_backward==t.code)||\
                    (turn_left==t.code)||(turn_right==t.code))&&\
                    (botton_status_on==t.value))
                    {     


                    }
                                               
              //  if(send(sockfd,buf,strlen(buf),0)== -1)
              //  {
              //          perror("send");
             //           exit(1);
             //   }
    	}

    	close(keys_fd);
    	return 0;
}

