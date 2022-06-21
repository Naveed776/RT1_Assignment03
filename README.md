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
 
 
 # Environment of Assignemnt
 The robot moves in the environment in the figure (Gazebo view):
 ![Screenshot from 2022-06-21 16-01-54](https://user-images.githubusercontent.com/91262613/174819361-4154f467-50e2-43d0-b63f-ed77aedb1d9c.png)

 
initially the robot does not have a map of the surrounding environment, but can build it thanks to the laser scanners it is equipped with and thanks to the gmapping package. The final map, visible on Rviz is as follows:

![Screenshot from 2022-06-21 16-02-11](https://user-images.githubusercontent.com/91262613/174819383-b9b14b97-f23d-4b88-bbc0-e4a0be29099e.png)

 
 ### ASSIGNEMNT GOALS
 
 There is a three goal of assigment that i achieved it from very efficent way
 
 `case 0 `
 
 
 used for quit goal
 break.
 
 `case 1`
 
 
 take co ordinates from the user and move the robot and achievd the target within a time 
 
 `case 2 `
 
 if there is a goal set cancel the actual goal and print the message 
 else print warninng

`case 3`

 manually driving
a input enable/disable driving manuallty
x is used for the input, stop the robot disable driving assistences disable manually driving.

break print error message.


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

![WhatsApp Image 2022-05-20 at 1 39 26 AM (1)](https://user-images.githubusercontent.com/91262613/174805415-d00cdeff-1c03-46fc-90f8-aff9c34f3e02.jpeg)


### Possible improvement
In this project the user can replace the goal to be achieved even when the robot is still reaching the previous one, deleting the later. A queue could be implemented where goals are reached sequentially.
Even when the map of the environment is complete, the robot is not able to understand a priori which points can be reached and which cannot. This could be improved.


