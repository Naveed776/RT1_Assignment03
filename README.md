# RT1_Assignment03

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
 ###  topics and Node
 
 After running final.launch, seven nodes are active:

- the topic`\cmd_vel` and the node of`\teleop` eidted on `\prov_cmd_vel`. 
- form this the velocity imposed via keyboard isn't immediately imposed to the robot,
- the node `\final_task` is also connected to `\gazebo` and `\move_base `nodes. 
- It receives the robot's status by `\move_base\feedback `and publish the goal to reach on `\move_base\goal`
- This node also subscribe on `\move_base\goal` topic to have a goal feedback.
- It can cancel current goal using `\move_base\cancel `topic.
- `\final_task` node also receives the laser scanner output on `\scan` topic by `\gazebo` node and sends to this node the robot velocity on `\cmd_vel `topic.
![Screenshot from 2022-05-16 22-37-54](https://user-images.githubusercontent.com/91262613/168683169-2411b699-ca36-4270-af76-7b30ceaa172f.png)

### FLOW CHART



### Possible improvement
In this project the user can replace the goal to be achieved even when the robot is still reaching the previous one, deleting the later. A queue could be implemented where goals are reached sequentially.
Even when the map of the environment is complete, the robot is not able to understand a priori which points can be reached and which cannot. This could be improved.


