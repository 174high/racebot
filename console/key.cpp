#include <stdio.h>  
#include <iostream>  
#include <linux/input.h>  
#include <sys/types.h>  
#include <sys/stat.h>  
#include <fcntl.h>  
#include <unistd.h> 
 
int main(int argc,char** argv)  
{  
    int keys_fd;  
    char ret[2];  
    struct input_event t;  

    printf("test1\n") ;
    keys_fd=open(argv[1],O_RDONLY);
    printf("test2\n"); 
    if(keys_fd<=0)  
    {     
        printf("error\n");  
        return -1;   
    }     
    printf("test3\n");
    while(1)  
    {     
        printf("test4 \n");
        read(keys_fd,&t,sizeof(struct input_event));  
        if(t.type==1)  
            printf("key %i state %i \n",t.code,t.value);  
    }     
    close(keys_fd);  
    return 0;  
}  

