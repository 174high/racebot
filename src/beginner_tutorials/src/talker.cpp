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
#define stop_car      25

#define train_car    44
#define test_car     50
#define stop_do      0xff 

int sockfd;

//ros::init(argc, argv, "talker");
//ros::Rate loop_rate(10);

std_msgs::Float64::Ptr  msg_s(new std_msgs::Float64);
std_msgs::Float64::Ptr  msg_p(new std_msgs::Float64);

ros::Publisher speed_pub ;
ros::Publisher position_pub;

char buf[MAXDATASIZE]={0};

int command=0;
int direction=0;
float position=0.43;

bool is_training_mode=false ; 
bool is_testing_mode=false; 
/**
the control_car（）function is used to control  motion of car 
command=run_forward is used to control the direction of car to move forward 
command=run_backward is used to control to move backward 
command=turn_right  is used to control to turn right 
command=turn_left   is used to control to turn left 
command=kill_car    is used to stop control car 
command=stop_car    is used to stop car (speed=0)
command=speed_level_1~10 is used to control the speed of car
*/

void control_car(int & command)
{


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
                else  if(command==turn_right)
                {
                    if(position<=0.8)
                    position+=0.15;
                    }
                else if(command==turn_left)
                {
                    if(position>=0.1)
                    position-=0.15;
                }
                else if((command>=speed_level_1)&&(command<=speed_level_10))
                {
                      ROS_INFO("command=%d",command);
                      msg_s->data=(1000+(2250/10)*command);

                }
                else if(command==kill_car)
                {
                        close(sockfd);
                        exit(1);
                }

                if(msg_s->data*direction<0)
                msg_s->data=msg_s->data*-1;

                msg_p->data = position;

                ROS_INFO("%g", msg_s->data);
                ROS_INFO("%g", msg_p->data);

                if(command==stop_car)
                {
                        msg_s->data=0;
                }

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

                //loop_rate.sleep();

                //memset(buf,0,sizeof(buf));

}


int main(int argc, char **argv)  
{  
        int reuse = 1;

	ros::init(argc, argv, "talker");  

	ros::NodeHandle n;  

         /** 
         * This is a message object. You stuff it with data, and then publish it. 
         */

	speed_pub = n.advertise<std_msgs::Float64>("/vesc/commands/motor/speed", 10);  
        position_pub = n.advertise<std_msgs::Float64>("/vesc/commands/servo/position", 10);

        struct sockaddr_in server_sockaddr,client_sockaddr;
        socklen_t sin_size ;
        //socklen_t sin_size= sizeof(struct sockaddr);
        int recvbytes;
        int client_fd;
        char buf[MAXDATASIZE]={0};
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
        server_sockaddr.sin_family=AF_INET;
                                       //duankou
        server_sockaddr.sin_port=htons(SERVPORT);
        server_sockaddr.sin_addr.s_addr=INADDR_ANY;
        bzero(&(server_sockaddr.sin_zero),8);

        if(bind(sockfd,(struct sockaddr *)&server_sockaddr,sizeof(struct
        sockaddr))== -1){
                perror("bind");
                exit(1);
        }
        printf("bind success!\n");
        if(listen(sockfd,BACKLOG)== -1){
                perror("listen");
                exit(1);
        }

        printf("listening....\n");

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
                memset(buf,0,sizeof(buf));

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

                command=atoi(buf);

    
                if(command==train_car)
                {
                	is_training_mode=true; 
                }
                else if (command==test_car)
                { 
                        is_testing_mode=true; 
                }
                else if(command==stop_do) 
                {
                	is_training_mode=false;
                        is_testing_mode= false;
                }
            
                control_car(command);
                
	}  

	close(sockfd);

	return 0;  
}  





