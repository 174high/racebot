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


int main(int argc, char **argv)  
{  
	ros::init(argc, argv, "talker");  

	ros::NodeHandle n;  

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
                //µ÷ÓÃrecvº¯ÊýœÓÊÕ¿Í»§¶ËµÄÇëÇó
                if((recvbytes=recv(client_fd,buf,100,0))== -1)
                {
                        perror("recv");
                        exit(1);
                }

                ROS_INFO("received a connection :%s\n",buf);

//                if(send(client_fd,buf,strlen(buf),0)== -1)
//                {
//                        perror("send");
//                        exit(1);
//                }

		/** 
		 * This is a message object. You stuff it with data, and then publish it. 
		 */  
		std_msgs::Float64::Ptr  msg_s(new std_msgs::Float64);  
        	std_msgs::Float64::Ptr  msg_p(new std_msgs::Float64);

		msg_s->data = 1200;  
        	msg_p->data = 0.8;

		ROS_INFO("%g", msg_s->data);  
        	ROS_INFO("%g", msg_p->data);  

		/** 
	 	* The publish() function is how you send messages. The parameter 
	 	* is the message object. The type of this object must agree with the type 
	 	* given as a template parameter to the advertise<>() call, as was done 
	 	* in the constructor above. 
	 	*/  
		speed_pub.publish(msg_s);  
        	position_pub.publish(msg_p);

		ros::spinOnce();  

		loop_rate.sleep();  
 
                memset(buf,0,sizeof(buf));
	}  

	close(sockfd);

	return 0;  
}  





