#ifndef _CAR_INFO_
#define _CAR_INFO_

class car_info{

public: 
long long seq;
long long secs;
long long nsecs; 
ros::Time stamp; 
float speed; 
float distance; 
int cmd; 

};


#endif 
