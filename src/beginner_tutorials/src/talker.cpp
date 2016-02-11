#include "ros/ros.h"  
#include "std_msgs/String.h"  
#include  <std_msgs/Float64.h>

#include <sstream>  

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

#define kill_car      37

int main(int argc, char **argv)  
{  
        int command=0;         
        int direction=0;
        float position=0.43;
        int reuse = 1;

	ros::init(argc, argv, "talker");  

	ros::NodeHandle n;  

         /** 
         * This is a message object. You stuff it with data, and then publish it. 
         */

        std_msgs::Float64::Ptr  msg_s(new std_msgs::Float64);
        std_msgs::Float64::Ptr  msg_p(new std_msgs::Float64);

	ros::Publisher speed_pub = n.advertise<std_msgs::Float64>("/vesc/commands/motor/speed", 10);  
        ros::Publisher position_pub = n.advertise<std_msgs::Float64>("/vesc/commands/servo/position", 10);


	ros::Rate loop_rate(10);  

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

       if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse)) < 0)
       {
                perror("setsockopet error\n");
                return -1;
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

        while (ros::ok())
        {
                if((recvbytes=recv(client_fd,buf,100,0))== -1)
                {
                        perror("recv");
                        exit(1);
                }

                ROS_INFO("received a connection :%s number=%d \n",buf,recvbytes);

                if(recvbytes==0)
                {
                       	if((client_fd=accept(sockfd,(struct sockaddr *)&client_sockaddr,&sin_size))==-1)
                 	{
                    		perror("accept");
                		exit(1);
        		} 
                }

//                if(send(client_fd,buf,strlen(buf),0)== -1)
//                {
//                        perror("send");
//                        exit(1);
//                }

                command=atoi(buf);
            
                if(command==run_forward)
                {
                    position=0.43; 
                    direction=1; 
                }
                else if(command==run_backward)
                { 
                    position=0.43;
                    direction=-1; 
                } 
                  
               if(command==turn_right)
                {
                    if(position<=0.8)
                    position+=0.15;
                }
                else if(command==turn_left)
                {
                    if(position>=0.1)
                    position-=0.15;
                }              

                if((command>=speed_level_1)&&(command<=speed_level_10))
                {
                      ROS_INFO("command=%d",command);
                      msg_s->data=(1000+(2250/10)*command); 

                }

                if(command==kill_car)
                {
                	close(sockfd);
                        exit(1);
                }

                if(msg_s->data*direction<0)
                msg_s->data=msg_s->data*-1;                

        	msg_p->data = position;

		ROS_INFO("%g", msg_s->data);  
        	ROS_INFO("%g", msg_p->data);  

		/** 
	 	* The publish() function is how you send messages. The parameter 
	 	* is the message object. The type of this object must agree with the type 
	 	* given as a template parameter to the advertise<>() call, as was done 
	 	* in the constructor above. 
	 	*/  
                
                if(command!=0)
                {
                        
			speed_pub.publish(msg_s);  

        		position_pub.publish(msg_p);
                } 
              
                command=0;

		ros::spinOnce();  

		loop_rate.sleep();  
 
                memset(buf,0,sizeof(buf));
	}  

	close(sockfd);

	return 0;  
}  





