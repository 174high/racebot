#!/usr/bin/env python
import rospy

from ackermann_msgs.msg import AckermannDriveStamped

import sys
import select
import termios
import tty

banner = """
Reading from the keyboard  and Publishing to AckermannDriveStamped!
---------------------------
Moving around:
        w
   a    s    d
anything else : stop
CTRL-C to quit
"""

keyBindings = {
    'w': (1, 0),
    'd': (1, -1),
    'a': (1, 1),
    's': (-1, 0),
    "1": (1,0),
    "2": (2,0), 
    "3": (3,0),
    "4": (4,0),
    "5": (5,0),
    "6": (6,0),
    "7": (7,0),
    "8": (8,0),
    "9": (9,0),
    "0": (10,0),
}


def getKey():
    tty.setraw(sys.stdin.fileno())
    select.select([sys.stdin], [], [], 0)
    key = sys.stdin.read(1)
    termios.tcsetattr(sys.stdin, termios.TCSADRAIN, settings)
    return key


speed=0;
turn=1; 
speed_step = 0.2
turn_step = 0.25


def vels(speed, turn):
    return "currently:\tspeed %s\tturn %s " % (speed, turn)


if __name__ == "__main__":
    settings = termios.tcgetattr(sys.stdin)
    pub = rospy.Publisher(
        '/vesc/ackermann_cmd_mux/input/teleop', AckermannDriveStamped,queue_size=5)
    print banner
    rospy.init_node('keyop')

    x = 0
    th = 0
    status = 0

    try:
        while(1):
            key = getKey()
            if key in keyBindings.keys():
                x = keyBindings[key][0]
                th = keyBindings[key][1]
            else:
                x = 0
                th = 0
                if (key == '\x03'):
                    break

            if(key=='w'):
                turn=1 
            elif(key=='s'):
                turn=-1
            x=abs(x)

            msg = AckermannDriveStamped()
            msg.header.stamp = rospy.Time.now()
            msg.header.frame_id = "base_link"

            msg.drive.speed = x * speed_step * turn
            msg.drive.acceleration = 1
            msg.drive.jerk = 1
            msg.drive.steering_angle = th * turn_step
            msg.drive.steering_angle_velocity = 1

            pub.publish(msg)

    except:
        print 'error'

    finally:
        msg = AckermannDriveStamped()
        msg.header.stamp = rospy.Time.now()
        msg.header.frame_id = "base_link"

        msg.drive.speed = 0
        msg.drive.acceleration = 1
        msg.drive.jerk = 1
        msg.drive.steering_angle = 0
        msg.drive.steering_angle_velocity = 1
        pub.publish(msg)

        termios.tcsetattr(sys.stdin, termios.TCSADRAIN, settings)
