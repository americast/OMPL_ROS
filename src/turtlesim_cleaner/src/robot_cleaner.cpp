#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include "turtlesim/Pose.h"
#include <bits/stdc++.h>

using namespace std;

ros::Publisher velocity_publisher;


const double PI = 3.14159265359;

void move(double speed, double distance, bool isForward);
void rotate(double angular_speed, double angle, bool cloclwise);
double degrees2radians(double angle_in_degrees);

#define TOTAL_POINTS 10
int dir=1;

struct co_ordinates {
	int x;
	int y;
};


int decode(int x, int y){		

/*
Input  :	x: x co-ordinate
  			y: y co-ordinate
 
Returns: 	0: if the (x,y) co-ordinate doesn't have an obstacle.
		1: if the (x,y) co-ordinate has an obstacle and that's not what you are 
 			searching for.
		2: if the (x,y) co-ordinate has an obstacle and that's what you are 
			searching for.
*/

	static int seq_count=0, redo=0;
	int rand_x, rand_y, count = 0;
	static vector<co_ordinates> vec;
	co_ordinates temp;

	if(!redo){
		while (count != TOTAL_POINTS){

			// generate a random number between [1, 10]
			while(1){
				int flag=0;
				rand_x = rand()%10;
				rand_y = rand()%10;
				if((rand_x == 0 || rand_x == 9) && (rand_y == 0 || rand_y == 9))
					continue;
				else{
					for(int i=0;i<vec.size(); ++i){
						if(((rand_x == vec[i].x   ) && (rand_y == vec[i].y   )) ||
						   ((rand_x == vec[i].x -1) && (rand_y == vec[i].y   )) ||
						   ((rand_x == vec[i].x +1) && (rand_y == vec[i].y   )) ||
						   ((rand_x == vec[i].x   ) && (rand_y == vec[i].y -1)) ||
						   ((rand_x == vec[i].x   ) && (rand_y == vec[i].y +1)) ||
						   ((rand_x == vec[i].x -1) && (rand_y == vec[i].y -1)) ||
						   ((rand_x == vec[i].x +1) && (rand_y == vec[i].y +1)) ||
						   ((rand_x == vec[i].x -1) && (rand_y == vec[i].y +1)) ||
						   ((rand_x == vec[i].x +1) && (rand_y == vec[i].y -1))){
							flag=1;
							break;
						}
					}
					if(!flag){
						temp.x = rand_x;
						temp.y = rand_y;
						vec.push_back(temp);
						break;
					}
					else{
						continue;
					}
				}
			} // generating one random co-ordinate
			++count;
		} // generate 'TOTAL_POINTS' random numbers
		++redo;
	}

	for(int i=0;i<vec.size();++i){
		if(vec[i].x==x && vec[i].y==y){
			if(i==seq_count){
				++seq_count;
				return 2;
			}
			else
				return 1;
		}
	}
	return 0;
}

void rt(int a=1)	//right turn
{
	if (a) ROS_INFO("\n\n\n********Right turn*********\n");
	rotate(degrees2radians(30), degrees2radians(90), 1);
	dir-=1;
	if (dir<=0) dir=4;
}
void lt()	//left turn
{
	ROS_INFO("\n\n\n ********Left turn*********\n");
	rotate(degrees2radians(30), degrees2radians(90), 0);
	dir+=1;
	if (dir>=5) dir=1;
	
}
void at()	//about turn
{
	ROS_INFO("\n\n\n********About turn*********\n");
	rt(0);rt(0);
}
void fm(int pos[2])	//forward march
{
	ROS_INFO("\n\n\n ********Forward march*********\n");
	move(2.5, 1, 1);
	if (dir==1)
	  pos[0]+=1;
	if (dir==2)
	  pos[1]+=1;
	if (dir==3)
	  pos[0]-=1;
	if (dir==4)
	  pos[1]-=1;
}
void rot(int to)
{
	int diff = to-dir;
	if (diff==2 || diff==-2)
	  at();
	if (diff==1 || diff==-3)
	  lt();
	if (diff==-1 || diff==3)
	  rt();
	dir=to;
}


int main(int argc, char **argv)
{
	// Initiate new ROS node named "talker"
	ros::init(argc, argv, "turtlesim_cleaner");
	ros::NodeHandle n;
	double speed, angular_speed;
	double distance, angle;
	bool isForward, clockwise;
	vector <int>v;

	velocity_publisher = n.advertise<geometry_msgs::Twist>("/turtle1/cmd_vel", 1000);
	ros::Rate loop_rate(10);
	

	//	/turtle1/cmd_vel is the Topic name
	//	/geometry_msgs::Twist is the msg type 
	move(2.5, 5, 0);
	rotate(degrees2radians(30), degrees2radians(90), 0);
	move(2.5, 5, 0);
	rotate(degrees2radians(30), degrees2radians(90), 1);
	ROS_INFO("\n\n\n ********START*********\n");
	int cover=1, pos[]={1,1};
	while(cover<=10)
	{	
		ROS_INFO("\nNow at %d,%d\n",pos[0],pos[1]);
		int obs[4]={0,0,0,0}, food[2], flag=0;
		int tempx, tempy, count=0;

		tempx=1; tempy=0;
		if (pos[0]+tempx<=10 && pos[1]+tempy<=10 && pos[0]+tempx>0 && pos[1]+tempy>0)
		{
			if (!flag)
			{
				
				int dec=decode(pos[0]+tempx,pos[1]+tempy);
				ROS_INFO("%d",dec);
				if (dec==1)
				  obs[count++]=1;
				else if (dec==2)
				  {food[0]=tempx; food[1]=tempy; flag=1;}
				else count++;
			}
		}
		else obs[count++]=1;
		
		tempx=0; tempy=1;
		if (pos[0]+tempx<=10 && pos[1]+tempy<=10 && pos[0]+tempx>0 && pos[1]+tempy>0)
		{
			if (!flag)
			{
				int dec=decode(pos[0]+tempx,pos[1]+tempy);
				ROS_INFO("%d",dec);
				if (dec==1)
				  obs[count++]=1;
				else if (dec==2)
				  {food[0]=tempx; food[1]=tempy; flag=1;}
				else count++;
			}
		}
		else obs[count++]=1;
		
		tempx=-1; tempy=0;
		if (pos[0]+tempx<=10 && pos[1]+tempy<=10 && pos[0]+tempx>0 && pos[1]+tempy>0)
		{
			if (!flag)
			{
				int dec=decode(pos[0]+tempx,pos[1]+tempy);
				ROS_INFO("%d",dec);
				if (dec==1)
				  obs[count++]=1;
				else if (dec==2)
				  {food[0]=tempx; food[1]=tempy; flag=1;}
				else count++;
			}
			
		}
		else obs[count++]=1;
		
		tempx=0; tempy=-1;
		if (pos[0]+tempx<=10 && pos[1]+tempy<=10 && pos[0]+tempx>0 && pos[1]+tempy>0)
		{
			if (!flag)
			{
				int dec=decode(pos[0]+tempx,pos[1]+tempy);
				ROS_INFO("%d",dec);
				if (dec==1)
				  obs[count++]=1;
				else if (dec==2)
				  {food[0]=tempx; food[1]=tempy; flag=1;}
				else count++;
			}
		}
		else obs[count++]=1;
		for (int i=0; i<v.size();i++)
		{ 
		  int here=v[i]; here/=10;
		  if (here==pos[0]*10+pos[1])
		    obs[v[i]%10]=1;
		}
		  
		if (flag)
		{
			if (food[0]==1) {rot(1); fm(pos);}
			if (food[0]==-1) {rot(3); fm(pos);}
			if (food[1]==1) {rot(2); fm(pos);}
			if (food[1]==-1) {rot(4); fm(pos);}
			ROS_INFO("\n\n\n ********COLLECTED %d*********\n", cover);
			cover++;
		}
		else
		{
			int count=0;
			if (!obs[count++])
				{v.push_back(pos[0]*100+pos[1]*10+count-1); rot(1); fm(pos);}
			else if (!obs[count++])
				{v.push_back(pos[0]*100+pos[1]*10+count-1); rot(2); fm(pos);}
			else if (!obs[count++])
				{v.push_back(pos[0]*100+pos[1]*10+count-1); rot(3); fm(pos);}
			else if (!obs[count++])
				{v.push_back(pos[0]*100+pos[1]*10+count-1); rot(4); fm(pos);}
		}
			
		//ros::spin();
	}	
	
	return 0;
}

void move(double speed, double distance, bool isForward){
   geometry_msgs::Twist vel_msg;
   //set a random linear velocity in the x-axis
   if (isForward)
	   vel_msg.linear.x =abs(speed);
   else
	   vel_msg.linear.x =-abs(speed);
   vel_msg.linear.y =0;
   vel_msg.linear.z =0;
   //set a random angular velocity in the y-axis
   vel_msg.angular.x = 0;
   vel_msg.angular.y = 0;
   vel_msg.angular.z =0;

   double t0 = ros::Time::now().toSec(),t2;
   double current_distance = 0.0, curr;
   ros::Rate loop_rate(100);
   do{   
   	   int rateflag=0;
   	   if (distance-current_distance<0.2)
   	   { 	   speed=0.5;
	   	   if (isForward)
		   	vel_msg.linear.x =0.5;
	   	   else
		   	vel_msg.linear.x =-0.5;
		   rateflag=1;
	   }
	   velocity_publisher.publish(vel_msg);
	   double t1 = ros::Time::now().toSec();
	   if (!rateflag)
	   {
	   	current_distance = speed * (t1-t0);
	   	curr=current_distance;
	   	t2=ros::Time::now().toSec();
	   }
	   else
	   	current_distance=curr+speed * (t1-t2);
	   ros::spinOnce();
	   loop_rate.sleep();
	   //cout<<(t1-t0)<<", "<<current_distance <<", "<<distance<<endl;
   }while(current_distance<distance);
   vel_msg.linear.x =0;
   velocity_publisher.publish(vel_msg);

}

void rotate (double angular_speed, double relative_angle, bool clockwise){

	geometry_msgs::Twist vel_msg;
	   //set a random linear velocity in the x-axis
	   vel_msg.linear.x =0;
	   vel_msg.linear.y =0;
	   vel_msg.linear.z =0;
	   //set a random angular velocity in the y-axis
	   vel_msg.angular.x = 0;
	   vel_msg.angular.y = 0;
	   if (clockwise)
	   	vel_msg.angular.z =-abs(angular_speed);
	   else
	   	vel_msg.angular.z =abs(angular_speed);

	   double t0 = ros::Time::now().toSec(), t2;
	   double current_angle = 0.0, curr=0.0;
	   ros::Rate loop_rate(1000);
	   do{
	   	   int rateflag=0;
		   if (relative_angle-current_angle<degrees2radians(10))
		   {
		      angular_speed=degrees2radians(5);
		      if (clockwise)
	   		vel_msg.angular.z =-degrees2radians(5);
	   	      else
	   		vel_msg.angular.z =degrees2radians(5);
	   	      rateflag=1;
	   	   }
		   velocity_publisher.publish(vel_msg);
		   double t1 = ros::Time::now().toSec();
		   if (!rateflag)
		   {
			   current_angle = angular_speed * (t1-t0);
			   curr=current_angle;
			   t2 = ros::Time::now().toSec();
		   }
		   else
		   	current_angle = curr+ angular_speed * (t1-t2);
		   ros::spinOnce();
		   loop_rate.sleep();
		   //cout<<(t1-t0)<<", "<<current_angle <<", "<<relative_angle<<endl;
	   }while(current_angle<relative_angle);
	   vel_msg.angular.z =0;
	   velocity_publisher.publish(vel_msg);
}

/**
 *  converts angles from degree to radians  
 */

double degrees2radians(double angle_in_degrees){
	return angle_in_degrees *PI /180.0;
}

