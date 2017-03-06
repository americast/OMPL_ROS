-----------------
WORK IN PROGRESS
-----------------

Four turtles.
Issues: FLoating point exception, angles not being maintained.

Algo:  
  first random movement (save locations to prevent loop which will be followed again iff no other option is available) + saving obstacle locations.
  
  if food found, go back to the obstacles using a* iff 60% or more of the remaining items were discovered as obstacles, i.e. a good probability of finding food in the old obstacles.
  
  if no. of obs. + no. of food collected = 10, keep looping through the obstacles via a* and don't come out of it.
  
To run,  
first setup source files  
`cd catkin_ws` //migrate to the directory  
`source ./devel/setup.bash`

in one terminal  
`roscore &`  
`rosrun turtlesim turtlesim_node`  
  

and in another, run runner.sh or use  
`catkin_make`  
`rosservice call /kill turtle1`  
`rosservice call /spawn  1 1 0 "turtle1"`  
`rosservice call /spawn  9 1 0 "pumba"`  
`rosservice call /spawn  1 9 0 "timon"`  
`rosservice call /spawn  9 9 0 "segfault"`  
`rosrun turtlesim_cleaner robot_cleaner_node`  

Tested on ROS Kinetic on Ubuntu 16.04.1 LTS Linux 4.4.0
