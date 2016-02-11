#include "ros/ros.h"  
#include "std_msgs/String.h"  
#include  <std_msgs/Float64.h>

#include <sstream>  
int main(int argc, char **argv)  
{  
	ros::init(argc, argv, "talker");  

	ros::NodeHandle n;  

	ros::Publisher chatter_pub = n.advertise<std_msgs::Float64>("/vesc/commands/motor/speed", 10);  

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
	std_msgs::Float64::Ptr  msg(new std_msgs::Float64);  

//	std::stringstream ss;  
//	ss << 100 << count;  
	msg->data = 100;  

	ROS_INFO("%g", msg->data);  

	/** 
	 * The publish() function is how you send messages. The parameter 
	 * is the message object. The type of this object must agree with the type 
	 * given as a template parameter to the advertise<>() call, as was done 
	 * in the constructor above. 
	 */  
	chatter_pub.publish(msg);  

	ros::spinOnce();  

	loop_rate.sleep();  
	++count;  
	}  


	return 0;  
}  





