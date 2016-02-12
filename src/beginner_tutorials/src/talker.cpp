#include "ros/ros.h"  
#include "std_msgs/String.h"  
#include  <std_msgs/Float64.h>
#include <nav_msgs/Odometry.h>
#include "car_info.h"

#include <db_cxx.h>
#include <db.h>

#include <iostream>
#include <fstream>
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

std::vector<car_info*> train_car_seq;
std::vector<std::vector<car_info*> > train_car_info;   
 

struct car_ps_info
{
	bool real_data; 
	long long time_nsecs; 
	float speed; 
	float position; 

};

std::vector<car_ps_info> train_ps_one_chain ;  
std::vector<std::vector<car_ps_info> > train_ps_all_chain;  

//ros::init(argc, argv, "talker");
//ros::Rate loop_rate(10);

std_msgs::Float64::Ptr  msg_s(new std_msgs::Float64);
std_msgs::Float64::Ptr  msg_p(new std_msgs::Float64);
nav_msgs::Odometry      distance; 

ros::Publisher   speed_pub ;
ros::Publisher   position_pub;
ros::Subscriber  distance_sub;

char buf[MAXDATASIZE]={0};

int command=0;
int direction=0;
float position=0.43;

float beginning_distance=0;
float beginning_speed=0;
long long beginning_seq=0;
long long   beginning_secs=0;
long long   beginning_nsecs=0; 

long long   train_beginning_secs=0;
long long  train_beginning_nsecs=0; 

ros::Time beginning_stampp ;


float current_speed=0; 
float current_distance=0; 
long long current_seq=0;
long  current_secs=0;
long  current_nsecs=0; 
ros::Time current_stamp; 

bool is_training_mode=false ; 
bool is_testing_mode=false; 

std::ofstream outFile; 

void store_data(void)
{	
		for(int i=0;i<5;i++)
		{  
           		std::cout<<"s: how many train ps now ="<<train_ps_one_chain.size()<<std::endl;
                	car_ps_info one_ps_info;
                	one_ps_info.real_data=false;
                	one_ps_info.speed=0;
                	one_ps_info.position=0;
                	train_ps_one_chain.push_back(one_ps_info);
                	std::cout<<"e: how many train ps now ="<<train_ps_one_chain.size()<<std::endl;
		}

/*

 	       car_ps_info one_ps_info;
               one_ps_info.real_data=false;
               one_ps_info.speed=900;
               one_ps_info.position=0.5;
*/

                 /* 1: create the database handle */
                Db db (0, 0);

                /* 2: open the database using the handle */
                db.open (NULL, "./apollo_db", NULL, DB_BTREE, DB_CREATE, 0644);

                /* 3: create the key and value Dbts */
                char *first_key = "test 1";
                u_int32_t key_len = (u_int32_t) strlen (first_key);


                //car_ps_info* first_value=&one_ps_info; 
                //u_int32_t value_len= (u_int32_t)sizeof(one_ps_info);
//                char * first_value = "Hello World - Berkeley DB style!!";		
//                u_int32_t value_len = (u_int32_t) strlen (first_value);
		std::vector<car_ps_info> * first_value=&train_ps_one_chain;
                u_int32_t value_len= (u_int32_t)sizeof(train_ps_one_chain);


                Dbt key (first_key, key_len + 1);
                Dbt value (first_value, value_len + 1);

                /* 4: insert the key-value pair into the database */
                int ret;
                ret = db.put (0, &key, &value, DB_NOOVERWRITE);

                if (ret == DB_KEYEXIST)
                {
                    std::cout << "hello_world: " << first_key << " already exists in db" <<std::endl;
                }

                /* 5: read the value stored earlier in a Dbt object */
                Dbt stored_value;
                ret = db.get (0, &key, &stored_value, 0);

		std::cout<<" size="<<((std::vector<car_ps_info>*)(stored_value.get_data()))->size()<<std::endl; 

//		std::cout<<"speed="<<((std::vector<car_ps_info>*)(stored_value.get_data()))<<std::endl; 
//                std::cout<<"position="<<((std::vector<car_ps_info>*)(stored_value.get_data()))<<std::endl;

                /* 6: print the value read from the database */
               // std::cout << (char *) stored_value.get_data () << std::endl;

                /* 7: close the database handle */
                db.close (0);
}


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

			if(is_training_mode==true)
                        {
		   		car_ps_info one_ps_info;
                        	one_ps_info.real_data=true;
                        	one_ps_info.speed=msg_s->data;
                        	one_ps_info.position=msg_p->data;
                        	train_ps_one_chain.push_back(one_ps_info);

                        }

                }

 		command=0; 
               // ros::spinOnce();

                //loop_rate.sleep();

                //memset(buf,0,sizeof(buf));
}

void training_car(int & command)
{
        car_info *my_car=new car_info() ;  
        my_car->distance=current_distance; 
        my_car->speed=current_speed;
        my_car->seq=current_seq; 
        my_car->stamp=current_stamp; 
        my_car->secs=current_secs; 
        my_car->nsecs=current_nsecs; 
        my_car->cmd=command; 
        train_car_seq.push_back(my_car);        

        std::cout<<"training car num="<<train_car_seq.size()<<std::endl ; 
        std::cout<<"distance="<<my_car->distance<<std::endl; 
        std::cout<<"speed="<<my_car->speed<<std::endl;      
        std::cout<<"seq="<<my_car->seq<<std::endl; 
        std::cout<<"stamp="<<my_car->stamp<<std::endl; 
        std::cout<<"secs="<<my_car->secs<<std::endl; 
        std::cout<<"nsecs="<<my_car->nsecs<<std::endl; 
        std::cout<<"command="<<my_car->cmd<<std::endl; 

	control_car(command);

}

void get_beginning_info(void)
{
	beginning_distance=current_distance;
        beginning_speed=current_speed;
        beginning_seq=current_seq;
        //beginning_stamp=current_stamp; 
        beginning_secs=current_secs; 
        beginning_nsecs=current_nsecs; 
}

void test_train_data(void)
{
	std::cout<<"train_ps_one chain size="<<train_ps_one_chain.size()<<std::endl ;

        std::vector<car_ps_info >::iterator it;

       	for(it=train_ps_one_chain.begin();it!=train_ps_one_chain.end();it++)
        {
                if((*it).real_data==true)
		{
      			ros::Duration(0.1).sleep();
         		msg_s->data=(*it).speed ;
                	msg_p->data=(*it).position ;
                        speed_pub.publish(msg_s);
                        position_pub.publish(msg_p);
			std::cout<<" publish !!"<<std::endl ;
		}
 	}

} 


void testing_car(void)
{
        long long   duration=0;
        int command=0; 

 	std::cout<<"testing car num="<<train_car_seq.size()<<std::endl ;
        ros::spinOnce();
        get_beginning_info(); 

       	std::vector<car_info *>::iterator it;

        train_beginning_secs=train_car_seq[0]->secs;
        train_beginning_nsecs=train_car_seq[0]->nsecs;
        std::cout<<"get train beginning data"<<std::endl; 

       	for(it=train_car_seq.begin()+1;it!=train_car_seq.end();it++)
       	{
                
//		std::cout<<"testing car num="<<train_car_seq.size()<<std::endl ;
        	std::cout<<"distance="<<(*it)->distance<<std::endl;
        	std::cout<<"speed="<<(*it)->speed<<std::endl;
        	std::cout<<"seq="<<(*it)->seq<<std::endl;
         	std::cout<<"stamp="<<(*it)->stamp<<std::endl;
        	std::cout<<"secs="<<(*it)->secs<<std::endl;
        	std::cout<<"nsecs="<<(*it)->nsecs<<std::endl;
        	std::cout<<"command="<<(*it)->cmd<<std::endl;   
                std::cout<<" "<<std::endl; 

                duration=(*it)->nsecs-train_beginning_nsecs ;

                std::cout<<"duration="<<duration<<std::endl; 
  		std::cout<<"current_nsecs-beginning_nsecs="<<(current_nsecs-beginning_nsecs)<<std::endl;

                while(current_nsecs-beginning_nsecs<duration)
                {
                           ros::spinOnce();
                }
   
                command=(*it)->cmd; 
                control_car(command);
                ros::spinOnce();


    	 }

         std::cout<<"###### END ######"<<std::endl; 

}

void smooth_control(int &command)
{
       	float final_speed=0;  
       	msg_s->data=0 ;

        std::cout<<" smooth control !!!!!"<<std::endl; 

       	if((command>=speed_level_1)&&(command<=speed_level_10))
       	{
                      ROS_INFO("command=%d",command);
                      final_speed=(1000+(2250/10)*command);

       	}

        ROS_INFO("%g", final_speed);


       	while(final_speed>msg_s->data)
       	{
       		msg_s->data+=300;
                usleep(100000); 
       		speed_pub.publish(msg_s);
                ros::spinOnce();
      	}

}

void distanceCallback(const nav_msgs::OdometryConstPtr &msg)
{
	distance=*msg; 
//        std::cout<<" get odom data!!!"<<std::endl; 
//        std::cout<<" distance x="<<distance.pose.pose.position.x<<std::endl ;  
//        std::cout<<" speed= "<<distance.twist.twist.linear.x<<std::endl ;

        current_seq=distance.header.seq; 
        current_secs=distance.header.stamp.toSec();; 
        current_nsecs=distance.header.stamp.toNSec(); 
        current_stamp=distance.header.stamp;
        current_distance=distance.pose.pose.position.x; 
        current_speed=distance.twist.twist.linear.x; 


}


void timerCallback(const ros::TimerEvent &event)
{
        static  unsigned int last_seq=0; 
        if(is_training_mode==true)
        {
        	if(last_seq==train_ps_one_chain.size())
        	{
        		std::cout<<"s: how many train ps now ="<<train_ps_one_chain.size()<<std::endl; 
			car_ps_info one_ps_info; 
        		one_ps_info.real_data=false; 
        		one_ps_info.speed=0;   
        		one_ps_info.position=0;
			train_ps_one_chain.push_back(one_ps_info);
        		std::cout<<"e: how many train ps now ="<<train_ps_one_chain.size()<<std::endl;
        	}
 	} 
        else
        {
		last_seq=0;
        }

        last_seq=train_ps_one_chain.size(); 
}


int main(int argc, char **argv)  
{  
        int reuse = 1;
	int ret;

//	Db db (0, 0);

	outFile.open("/home/ubuntu/racecar-ws/data.csv",std::ios::out);
	outFile << "name" << ',' << "age" << ',' << "hobby" << std::endl;
	outFile.close();


	ros::init(argc, argv, "talker");  

	ros::NodeHandle n;  

         /** 
         * This is a message object. You stuff it with data, and then publish it. 
         */

	speed_pub = n.advertise<std_msgs::Float64>("/vesc/commands/motor/speed", 10);  
        position_pub = n.advertise<std_msgs::Float64>("/vesc/commands/servo/position", 10);
        distance_sub = n.subscribe("/odom", 1, distanceCallback);        

	ros::Timer timer = n.createTimer(ros::Duration(0.1), timerCallback);

        struct sockaddr_in server_sockaddr,client_sockaddr;
        socklen_t sin_size ;
        //socklen_t sin_size= sizeof(struct sockaddr);
        int recvbytes=0;
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

 //      if((client_fd=accept(sockfd,(struct sockaddr *)&client_sockaddr,&sin_size))==-1)
 //       {
 //               perror("accept");
 //               exit(1);
//        }

       fd_set fdsr; 
       int maxsock;

       struct timeval tv;

       maxsock = sockfd;

        printf("before loop \n");
        while (ros::ok())
        {

               	// initialize file descriptor set
               	FD_ZERO(&fdsr);
               	FD_SET(sockfd, &fdsr);

              	// timeout setting
               	tv.tv_sec = 0;
               	tv.tv_usec = 0;

                if(client_fd!=0)
                FD_SET(client_fd, &fdsr);

               // printf("test max=%d \n",maxsock);
               	ret = select(maxsock+1, &fdsr, NULL, NULL, &tv);


                if (ret < 0) 
                {
               		perror("select");
            		break;
        	} 
                else if (ret == 0) 
                {
            //		printf("timeout\n");
           // 		continue;
        	}

           //     printf("test0\n");

                memset(buf,0,sizeof(buf));
              

                if (FD_ISSET((client_fd), &fdsr)) 
		{
                        if(client_fd>0)                       
                        {
	              		if((recvbytes=recv(client_fd,buf,100,0))== -1)
        	        	{
                	        	perror("recv");
                        		exit(1);
                		}
               		}
                }

              // printf("test1 buf0==%d \n",buf[0]);

               if (FD_ISSET(sockfd, &fdsr)) {

               		client_fd = accept(sockfd, (struct sockaddr *)&client_sockaddr, &sin_size);

               		if (client_fd <= 0) {
               			perror("accept");
                		continue;
              		}
 
                	maxsock = client_fd;            
                }
 
             

//                ROS_INFO("received a connection :%s number=%d \n",buf,recvbytes);

                command=atoi(buf);

                ros::spinOnce();

                if(command==0) 
                continue; 
    
                if(command==train_car)
                {
                	is_training_mode=true; 
 			is_testing_mode= false;
                        std::cout<<" training mode"<<std::endl; 
                }
                else if (command==test_car)
                { 
                        is_testing_mode=true;
                        is_training_mode=false;                       
                        std::cout<<"testing mode" <<std::endl; 
                }
                else if(command==stop_do) 
                {
                	is_training_mode=false;
                        is_testing_mode= false;
                        std::cout<<"stop all mode!!!"<<std::endl; 
                        std::cout<<"############ store data of training   ##########################"<<std::endl; 
		        std::cout<<"testing car num="<<train_car_seq.size()<<std::endl ;

			store_data();
	
                        train_car_info.push_back(train_car_seq); 
                        train_car_seq.clear(); 
                        std::cout<<"how many times we trained "<<train_car_info.size()<<std::endl;

			train_ps_all_chain.push_back(train_ps_one_chain);
			train_ps_one_chain.clear();
                }
                else
                {
                        if(is_training_mode==true)
                        {
                        	//training_car(command);
				control_car(command);
                        }
                        else if(is_testing_mode==true)                        
                        {
				
		//		testing_car();
		           	test_train_data();
			}
                        else
                        { 
                	//      smooth_control(command);
                     		control_car(command);
			}
                }

	}  

	close(sockfd);

	return 0;  
}  





