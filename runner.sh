catkin_make
rosservice call /kill turtle1
rosservice call /spawn  1 1 0 "turtle1"
rosservice call /spawn  9 1 0 "pumba"
rosservice call /spawn  1 9 0 "timon"  
rosservice call /spawn  9 9 0 "segfault"
rosrun turtlesim_cleaner robot_cleaner_node
