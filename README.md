# RT_Assignment03

**Introduction**

The assignment consists in writing two ROS nodes. The user can choose one of three methods to command the robot (autonomously, manually, or in assisted driving mode).

# How to Run this assignment

There is a simple way to run this assignment. first intall some packages 

`Install xterm:`

`$ sudo apt install xterm`

Install ros navigation stack:

`$ sudo apt-get install ros-<your_ros_distro>-navigation`

`Install teleop twist keyboard package:`

`$ sudo apt-get install ros-hydro-teleop-twist-keyboard`

 and then launch the simple file 
 
 `$ roslaunch final_assignment final.launch`
 # Brief Discussion of the Project
 
 After running final.launch, seven nodes are active:

- 'cmd_vel/'
