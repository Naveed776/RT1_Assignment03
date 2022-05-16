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

- the topic`\cmd_vel` and the node of`\teleop` eidted on `\prov_cmd_vel`. 
- form this the velocity imposed via keyboard isn't immediately imposed to the robot,
-  it's controlled by \final_UI node.
### final_UI

- the node `\final_UI` is also connected to `\gazebo` and `\move_base `nodes. 
- It receives the robot's status by `\move_base\feedback `and publish the goal to reach on `\move_base\goal`
- This node also subscribe on `\move_base\goal` topic to have a goal feedback.
- It can cancel current goal using `\move_base\cancel `topic.
- `\final_UI` node also receives the laser scanner output on `\scan` topic by `\gazebo` node and sends to this node the robot velocity on `\cmd_vel `topic.
