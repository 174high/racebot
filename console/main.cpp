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
#define MAXDATASIZE 20


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

#define botton_status_on     2 
#define botton_status_click  1
#define botton_status_off    0  

#define kill_car  37
#define stop_car  25

#define train_car 44
#define test_car  50

int main(int argc,char** argv)
{
    	int keys_fd;
    	char ret[2];
    	struct input_event t;
    	int sockfd,sendbytes;
    	struct hostent *host;
    	struct sockaddr_in serv_addr;
        int last_code=0; 

        bool is_training=false;
        bool is_testing=false;  

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

                if((t.type==1)&&(botton_status_off!=t.value))
                {
		        printf("key %i state %i \n",t.code,t.value);

                        if(last_code==t.code)
                        {	
                            t.code=0; 
                            last_code=0; 
                        }

                        printf("key %i state %i \n",t.code,t.value);

		        if(run_forward==t.code)
		        {
		                char buf[MAXDATASIZE]={0};
		                sprintf(buf,"%d\n",t.code);
		        	if(send(sockfd,buf,strlen(buf),0)== -1)
		      		{
		     	        	perror("send");
		     		        exit(1);
		     		}

		                printf("send forward\n");
		        }
		        else if(run_backward==t.code)                                                                {  
		                char buf[MAXDATASIZE]={0};
		                sprintf(buf,"%d\0",t.code);
		                if(send(sockfd,buf,strlen(buf),0)== -1)
		                {
		                        perror("send");
		                        exit(1);
		                }

		                printf("send backward\n");

		        }
		        else if(turn_left==t.code) 
                        {
	  
		                char buf[MAXDATASIZE]={0};
		                sprintf(buf,"%d\0",t.code);
		                if(send(sockfd,buf,strlen(buf),0)== -1)
		                {
		                        perror("send");
		                        exit(1);
		                }

		                printf("send turn left \n");
		        }
		        else if(turn_right==t.code)                                                                  {
		                char buf[MAXDATASIZE]={0};
		                sprintf(buf,"%d\0",t.code);
		                if(send(sockfd,buf,strlen(buf),0)== -1)
		                {
		                        perror("send");
		                        exit(1);
		                }

		                printf("send turn right \n");

		        }
		        else if((t.code>=speed_level_1)&&(t.code<=speed_level_10))
		        {
		                printf("speed %d \n",t.code);      
		                char buf[MAXDATASIZE]={0} ;
		                sprintf(buf,"%d\0",t.code);
		                if(send(sockfd,buf,strlen(buf),0)== -1)
		                {
		                        perror("send");
		                        exit(1);
		                }

		                printf("send speed %d \n",t.code);
		        }
                        else if(t.code==kill_car)
                        {
                                char buf[MAXDATASIZE]={0} ;
                                sprintf(buf,"%d\0",t.code);
                                if(send(sockfd,buf,strlen(buf),0)== -1)
                                {
                                        perror("send");
                                        exit(1);
                                }

                                printf("kill car %d \n",t.code);
                                exit(1);
                        }     
                        else if(t.code==stop_car)       
                        {
                              char buf[MAXDATASIZE]={0} ;
                                sprintf(buf,"%d\0",t.code);
                                if(send(sockfd,buf,strlen(buf),0)== -1)
                                {
                                        perror("send");
                                        exit(1);
                                }

                                printf("stop car %d \n",t.code);
                        }    
                        else if(t.code==train_car)
                        {
   				if(is_training==false)
                                {
                                	is_training=true;
                                }
                                else
                                {
                                        is_training=false;
                                }

                                  
                                char buf[MAXDATASIZE]={0} ;
                                if(is_training==true)
                                sprintf(buf,"%d\0",t.code);
                                else                                 
                                sprintf(buf,"%d\0",0xff);                                

                                if(send(sockfd,buf,strlen(buf),0)== -1)
                                {
                                        perror("send");
                                        exit(1);
                                }

                                printf("start training car %s %d \n",buf,is_training);                                                           }
                         else if(t.code==test_car)
                         {
                               if(is_testing==false)
                                {
                                        is_testing=true;
                                }
                                else
                                {
                                        is_testing=false;
                                }
                                char buf[MAXDATASIZE]={0} ;
                                if(is_testing==true)
                                sprintf(buf,"%d\0",t.code);
                                else
                                sprintf(buf,"%d\0",0xff);
                                if(send(sockfd,buf,strlen(buf),0)== -1)
                                {
                                        perror("send");
                                        exit(1);
                                }

                                printf("start testing car %s  %d\n",buf,is_testing);

                         }
                       
                        last_code=t.code;
                }

    	}

    	close(keys_fd);
    	return 0;
}























