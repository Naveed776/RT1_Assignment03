// add libaries


#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include "sensor_msgs/LaserScan.h"

#include "actionlib_msgs/GoalID.h"
#include "move_base_msgs/MoveBaseActionGoal.h"
#include "move_base_msgs/MoveBaseActionFeedback.h"


#include <iostream>
#include <string>
#include <chrono>

#define Goal_th 0.5 //The goal is considered to be reached if the distance between it and the robot is < 0.5
#define D_th 0.5 //A wall is considered to be too close if the distance between it and the robot is < 0.5
#define Timeout 180000000 // A goal is considered not reachable if the robot doesn't reach it within 3 minutes

// Goal to cancel message
actionlib_msgs::GoalID cancel_target;

// Declare the publishers
ros::Publisher pub_velocity;
ros::Publisher pub_target;
ros::Publisher pub_off;

float x_target; // Goal coordinate x
float y_target; // Goal coordinate y


bool target = false;
bool drive = false;
bool instructor = false;

geometry_msgs::Twist stop; // Null velocity to stop the robot
geometry_msgs::Twist new_velocity; // Velocity to be checked by the driving assistence before being published

char m = 'a'; // Variable used to store the user commands
std::string id = ""; // Goal ID
// Time variable to measure the time spent reaching the goal
std::chrono::high_resolution_clock::time_point time_start;  
std::chrono::high_resolution_clock::time_point time_end;

// Initialize a raw string literal to print the menu
const char * menu = R"(
   Read instruction for control Robot
   -----------------------------------
   - press 0 Quit for the system 
   - prees 1 Insert your new value of the target 
   - press 2 Cancel your current driving target
   - press 3 for the Manual driving
   -------------------------------------
)";


//This function print on the screen the actual goal if a goal has been set

void displaytarget()
{
	if(target)
	{
		printf("----------------------------\n");
		printf("Provide your actual target\n");
	    	printf("x = %f\n",x_target);
	    	printf("y = %f\n",y_target);
	    	printf("----------------------------\n");
    	}
    	else
    	{
    		printf("----------------------------\n");
    		printf("Provide your actual target\n");
	    	printf("x = NaN\n");
	    	printf("y = NaN\n");
	    	printf("----------------------------\n");
    	}
}

//This function ask the user to enter the goal's coordinates
//Then it sends a message to the goal topic and takes the starting time

void settarget()
{
	move_base_msgs::MoveBaseActionGoal target_msg;
	std::string s;
	float x,y;
	// Take new goal from the user
        printf("Enter Your Target coordinates\n");
	printf("x: ");
	std::cin >> s;
	x = atof(s.c_str());
	printf("y: ");
	std::cin >> s;
	y = atof(s.c_str());
	
	// Set new coordinates to reach
	target_msg.goal.target_pose.header.frame_id = "map";
	target_msg.goal.target_pose.pose.orientation.w = 1;
		    
	target_msg.goal.target_pose.pose.position.x = x;
	target_msg.goal.target_pose.pose.position.y = y;

	// Publish new goal 
	pub_target.publish(target_msg);
	// Set the starting time
	time_start = std::chrono::high_resolution_clock::now();
	// There is a goal
	target = true;
}


//This function return false if there isn't a goal to cancel,
//or cancel the goal by its id and return true

bool canceltarget()
{
	if(!target)
		return false;
	cancel_target.id = id;
	pub_off.publish(cancel_target);
	target = false;
	return true;
	
}


//This function is called whenever something is published on move_base/feedback topic
//It checks if the goal is been reached, updates the actual goal id and cancel the actual goal if the timeout is reached

void currentSituation(const move_base_msgs::MoveBaseActionFeedback::ConstPtr& msg) {

    // Update the goal ID if there is a new goal
    if (id != msg->status.goal_id.id) {
        id = msg->status.goal_id.id;
    }
    
    // If there is a goal check if the robot has reached the goal or is trying to reach the goal for a too long time
    if(target)
    {
    	// Take the current robot position
        float distance_x;
	float distance_y;
	float robotposition_x = msg->feedback.base_position.pose.position.x;
	float robotposition_y = msg->feedback.base_position.pose.position.y;
    
	// Compute the error from the actual position and the goal position
	distance_x = robotposition_x - x_target;
	distance_y = robotposition_y - y_target;

	// The robot is on the goal position
	if (abs(distance_x) <= Goal_th&& abs(distance_y) <= Goal_th)
	{
	     printf("target achieved\n");
	     canceltarget();
	}

    	time_end = std::chrono::high_resolution_clock::now();
        auto time = std::chrono::duration_cast<std::chrono::microseconds>(time_end - time_start).count();
        if(time > Timeout)
        {
        	printf("Sorry The Target Can not be reached Try Next time!\n");
        	canceltarget(); // Cancel the goal if it can't be reached
        }
    }
}

//This function saves in global variables the actual goal

void Maintarget(const move_base_msgs::MoveBaseActionGoal::ConstPtr& msg) {
    x_target = msg->goal.target_pose.pose.position.x;
    y_target = msg->goal.target_pose.pose.position.y;
}


//This function is called whenever the teleop twist keyboard want to change the robot velocity
//If the robot isn't in manual driving, the message is ignored.
//If the robot is in manual driving, but driving assistence is disabled, the message is sent to the cmd_vel topic
//If the robot is in manual driving and driving assistence is enabled, the message is saved in a global variable 
//and it will be corrected by the driving assistence before being published

void Velocity(const geometry_msgs::Twist::ConstPtr& msg) {
    if(!drive )
    	return;
    if(!instructor)
    {
    	pub_velocity.publish(msg);
    	return;
    }
    new_velocity.linear.x = msg->linear.x;
    new_velocity.angular.z = msg->angular.z;
}


//This function receives as input an array of distances and return the smallest

double checking_distance(double range[], int size)
{
	double min = 100;
	int i;
	for( i=0; i < size; i++)
	{
		if(range[i] < min)
		{
			min = range[i];
		}
	}
	return min;
}


//This function is called whenever a message from LaserScan topic arrives.
//If driving assistence is disabled the function does nothing.
//If the driving assistence is enabled:
	//- divides the distances in input in 5 sectors
	//- if there is a close wall in the direction where the user wants the robot to go, 
	 //it prints a warning on the console and stops the robot

void drivinginstructor(const sensor_msgs::LaserScan::ConstPtr &msg)
{
	if(!instructor) // If driving assistence is disabled, do nothing
		return;
	// get array by laserscan
	float range[msg->ranges.size()];
	
	for(int m = 0; m < msg->ranges.size(); m++)
	{
		range[m] = msg->ranges[m];
	}
	
	//divide the laserscan values in 5 sectors
	int location = (msg->ranges.size())/5;
	double left[location];
	double front_left[location];
	double front[location];
	double front_right[location];
	double right[location];
	
	//fill sector arrays
	for(int i = 0; i < location; i++)
	{
		left[i] = range[i + 4*location];
	}

	for(int i = 0; i < location; i++)
	{
		front_left[i] = range[i + 3*location];
	}
	
	for(int i = 0; i < location; i++)
	{
		front[i] = range[i + 2*location];
	}
	
	for(int i = 0; i < location; i++)
	{
		front_right[i] = range[i + location];
	}

	for(int i = 0; i < location; i++)
	{
		right[i] = range[i];
	}
	// check if there is a too close wall in the direction of the robot
	if(checking_distance(front, location) < D_th) // Check if there is a wall in front of the robot
	{
		if(new_velocity.linear.x > 0 && new_velocity.angular.z == 0) // Check if the robot has to go straight on
		{
			new_velocity.linear.x = 0; // Stop the robot
			printf("you are close to the wall move other side\n"); // Print an error
		}
	}
		
	if(checking_distance(front_left, location) < D_th)
	{
		if(new_velocity.linear.x > 0 && new_velocity.angular.z > 0)
		{
			new_velocity.linear.x = 0;
			new_velocity.angular.z = 0;
			printf("you are close to the wall move other side\n");
		}
	}
	
	if(checking_distance(front_right, location) < D_th)
	{
		if(new_velocity.linear.x > 0 && new_velocity.angular.z < 0)
		{
			new_velocity.linear.x = 0;
			new_velocity.angular.z = 0;
			printf("you are close to the wall move other side\n");
		}
	}
		
	if(checking_distance(left, location) < D_th)
	{
		if(new_velocity.linear.x == 0 && new_velocity.angular.z > 0)
		{
			new_velocity.angular.z = 0;
			printf("you are close to the wall move other side\n");
		}
	}
		
	if(checking_distance(right, location) < D_th)
	{
		if(new_velocity.linear.x == 0 && new_velocity.angular.z < 0)
		{
			new_velocity.angular.z = 0;
			printf("you are close to the wall move other side\n");
		}
	}

    	pub_velocity.publish(new_velocity); // Publish the correct velocity 
		
}


//This function is called when the user choose the manual driving.
//It changes the driving mode and enable/disable driving assistence if the user press 'a'.

void drivingmanually()
{
	drive = true; // Change driving mode
	while(m != 'x')
	{
		// Print manual driving menu
		printf("----------------------------\n");
		printf("Start driving manually\n");
		if(instructor)
			printf("Driving assistence enabled\n");
		else
			printf("Driving assistence disabled\n");
		printf("----------------------------\n");
		printf("Press a for enable/disable driving assistence\n");
		printf("Press x for exiting\n");
		scanf(" %c", &m); // Read user command
		system("clear");
		switch(m)
		{
			case 'a':
				if(!instructor) // Change driving assistence's state
					instructor = true;
				else
					instructor = false;
				break;
			case 'x':
				drive = false; // Change driving mode
				instructor = false; // Disable driving assistence
				pub_velocity.publish(stop); // Set velocity to zero before exiting
				break;
			default:
				printf("Command invalid please define correct command!!\n");
		}
	}
}


//This function print the main menu on the screen
//and calls the functions needed for the selected operation


void choice()
{
	while(m != '0')
	{
		displaytarget(); // Print the actual goal on the console
	    	printf("%s\n", menu); // Print menu
	    	scanf(" %c", &m); // Read the input from the user
	    	system("clear"); // Clear the terminal for order
	    	switch(m) // Select the right action
	    	{
	    		case '0':
	    			// Quit
	    			break;
	    		case '1':
	    			settarget(); 
	    			system("clear"); // Clear the terminal to rewrite the menu
	    			break;
	    		case '2':
	    			if(canceltarget()) // If the goal cancel succeded
	    				printf("Previous target cancelled\n");
	    			else
	    				printf("There is no target to remove\n");
	    			break;
	    		case '3':
	    			drivingmanually(); // Enter in manual driving mode
	    			break;
	    		default:
	    			printf("***Command invalid please define correct command ***");
	    			break;
	    	
	    	}
    	}
}

int main(int argc, char **argv) {
    // Initialize the node
    ros::init(argc, argv, "final_task");
    ros::NodeHandle nh;
    
    // Define the publishers
    pub_target = nh.advertise<move_base_msgs::MoveBaseActionGoal>("move_base/goal", 1000); // Update goal
    pub_off = nh.advertise<actionlib_msgs::GoalID>("move_base/cancel", 1000); // Cancel actual goal
    pub_velocity = nh.advertise<geometry_msgs::Twist>("/cmd_vel", 1000); // Update robot velocity
    
    // Define the subscribers
    ros::Subscriber sub_pos = nh.subscribe("/move_base/feedback", 1000, currentSituation); // Current  Status
    ros::Subscriber sub_goal = nh.subscribe("/move_base/goal", 1000, Maintarget); // Actual Goal feedback
    ros::Subscriber sub_key = nh.subscribe("/prov_cmd_vel", 1000, Velocity); // Get velocity from teleop twist keyboard
    ros::Subscriber sub_laser = nh.subscribe("/scan", 1000, drivinginstructor); // Laser scanner
    
    
    // Multi-threading
    ros::AsyncSpinner spinner(4);
    spinner.start();
    choice(); // Enter in the main menu
    spinner.stop();
    ros::shutdown();
    ros::waitForShutdown();

    return 0;
}

