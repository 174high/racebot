#include "ros/ros.h"  
#include "std_msgs/String.h"  
#include  <std_msgs/Float64.h>

#include <sstream>  
int main(int argc, char **argv)  
{  
	ros::init(argc, argv, "talker");  

	ros::NodeHandle n;  

	ros::Publisher speed_pub = n.advertise<std_msgs::Float64>("/vesc/commands/motor/speed", 10);  
        ros::Publisher position_pub = n.advertise<std_msgs::Float64>("/vesc/commands/servo/position", 10);


	ros::Rate loop_rate(10);  

	/** 
	* A count of how many messages we have sent. This is used to create 
	* a unique string for each message. 
	*/  
	int count = 0;  
	while (ros::ok())  
	{  
	/** 
	 * This is a message object. You stuff it with data, and then publish it. 
	 */  
	std_msgs::Float64::Ptr  msg_s(new std_msgs::Float64);  
        std_msgs::Float64::Ptr  msg_p(new std_msgs::Float64);

//	std::stringstream ss;  
//	ss << 100 << count;  
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
	++count;  
	}  


	return 0;  
}  





