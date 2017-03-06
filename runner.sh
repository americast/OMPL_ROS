catkin_make
rosservice call /kill turtle1
rosservice call /spawn  1 1 0 "turtle1"
rosservice call /spawn  10 1 0 "pumba"
rosservice call /spawn  1 10 0 "timon"  
rosservice call /spawn  10 10 0 "segfault"
rosrun turtlesim_cleaner robot_cleaner_node
