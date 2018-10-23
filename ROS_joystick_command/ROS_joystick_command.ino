/*
  Steering ROS robot with an Arduino
  
  In this code, the analog values of a joystick module are read and transformed to velocity commands
  which are published as a ROS message of type "geometry_msgs/twist" in your ROS network.
  
  List of components:
  * Arduino board + serial cabel
  * Joystick module (e.g. KY-023)
  * 4 x Jumper Wires M/F
  
  Last modified 23/10/18
  By Jan-Felix Klein
  For the full project description, please refer to:
  https://github.com/JaFeKl/steering_ROS_robot_with_arduino
*/

#include <ros.h>
#include <geometry_msgs/Twist.h>

ros::NodeHandle nh;                               // create node handle
geometry_msgs::Twist twist_msg;                   // create empty twist message
ros::Publisher joystick("cmd_vel", &twist_msg);   // initialize publisher "joystick"

const int yAxisPin = A0;          //VRy pin is connected to analog pin A1
const int xAxisPin = A1;          //VRx pin is connected to analog pin A0
int x_nominal;  // variables to store nominal joystick reading
int y_nominal;
int x_read;     // variables to store analog readings
int y_read;


// function to linear map analog readings to a desired velocity scale
float mapf(int val, float out_min, float out_max) {
    if (val<= max(x_nominal,y_nominal) && val >= min(x_nominal,y_nominal)) {
      return 0;   // return 0 for nominal joystick position
    } else {  
      return val * (out_max - out_min) /1023 + out_min;
    }
}


void setup() {
  pinMode(xAxisPin, INPUT);         //Set the xAxisPin to be an input
  pinMode(yAxisPin, INPUT);         //Set the yAxisPin to be an input
  nh.initNode();
  nh.advertise(joystick);

  x_nominal = analogRead(xAxisPin); //receive nominal value for 
  y_nominal = analogRead(yAxisPin);
}


void loop() {
  // For the purpose of this project, we only need to read the x and y value from the joystick
  x_read = analogRead(xAxisPin);    // receive analog signal value from joystick (range 0-1023)
  y_read = analogRead(yAxisPin);

  twist_msg.linear.x = -mapf(y_read, -1, 1);          //map y-axis reading and fill linear x value (changed sign --> adjust movement direction to match)
  twist_msg.angular.z = - mapf(x_read, -1.5, 1.5);    //move joystick to the left = positive angular velocity = robot drives a left curve
  joystick.publish ( &twist_msg);                     //publish message
  nh.spinOnce();
  
  delay(50); //50 ms delay --> publishing with ~20Hz
}
