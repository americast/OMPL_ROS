#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include "turtlesim/Pose.h"
#include <bits/stdc++.h>
#include<"turtlesim/SetPen.h">

using namespace std;

ros::Publisher velocity_publisher;


const double PI = 3.14159265359;

int cover=0, pos[]={4,4};
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
		//ROS_INFO("%d, %d\n",vec[i].x,vec[i].y);
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
	rotate(degrees2radians(30), degrees2radians(180), 1);
	dir+=2;
	if (dir>4) dir-=4;
	if (dir<1) dir+=4;
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
	int diff = to-::dir;
	if (diff==2 || diff==-2)
	  at();
	if (diff==1 || diff==-3)
	  lt();
	if (diff==-1 || diff==3)
	  rt();
	::dir=to;
}

vector <co_ordinates> uobs;  //obstacles later to be considered targets
vector <co_ordinates> foods;

void a_star()
{
	int loop_flag=0;
	for (int i=0;i<uobs.size();i++)
	{
		if (cover==9) break;
		int tarx=uobs[i].x, tary=uobs[i].y, flag=0, food[2];
		float dist1,dist2,dist3,dist4;
		
		vector <int> v;
		while(1)
		{
			if (cover==9) break;
			if (uobs.size()>=(10-cover)) loop_flag=1;
			ROS_INFO("\n*a_star*\nNow at %d,%d\nCollected: %d\nObstacles hit: %ld\n",pos[0],pos[1],cover,uobs.size());
			int obs[]={0,0,0,0};
			int count=0, foodflag=0, remflag;
			int tempx=1, tempy=0;
			dist1=dist2=dist3=dist4=500.0;
			if (pos[0]+tempx<10 && pos[1]+tempy<10 && pos[0]+tempx>=0 && pos[1]+tempy>=0)
			{
				if (!foodflag)
				{
					int dec=decode(pos[0]+tempx,pos[1]+tempy);
					ROS_INFO("1: %d",dec);
					if (dec==1)
					{
				  		obs[count++]=1;
				  		remflag=0;
						  for (int i=0; i<uobs.size();i++)
						  {
						  	if (uobs[i].x==pos[0]+tempx && uobs[i].y==pos[1]+tempy)
						  	{remflag=1;  break;}
						  }
						  for (int i=0; i<foods.size();i++)
						  {
						  	if (foods[i].x==pos[0]+tempx && foods[i].y==pos[1]+tempy)
						  	{remflag=1;  break;}
						  }
						  
						  if (!remflag)
						    {co_ordinates temp; temp.x=pos[0]+tempx; temp.y=pos[1]+tempy; uobs.push_back(temp);}
						  
						  if (pos[0]+tempx==tarx && pos[1]+tempy==tary) break;
					}
					else if (dec==2)
					  {food[0]=tempx; food[1]=tempy; foodflag=1;}
					else {count++;dist1=pow(tarx-(pos[0]+tempx),2)+pow(tary-(pos[1]+tempy),2);}
				}
			}
			else obs[count++]=1;
			
			tempx=0; tempy=1;
			if (pos[0]+tempx<10 && pos[1]+tempy<10 && pos[0]+tempx>=0 && pos[1]+tempy>=0)
			{
				if (!foodflag)
				{
					int dec=decode(pos[0]+tempx,pos[1]+tempy);
					ROS_INFO("2: %d",dec);
					if (dec==1)
					{
				  		obs[count++]=1;
				  		remflag=0;
						  for (int i=0; i<uobs.size();i++)
						  {
						  	if (uobs[i].x==pos[0]+tempx && uobs[i].y==pos[1]+tempy)
						  	{remflag=1;  break;}
						  }
						  for (int i=0; i<foods.size();i++)
						  {
						  	if (foods[i].x==pos[0]+tempx && foods[i].y==pos[1]+tempy)
						  	{remflag=1;  break;}
						  }
						  if (!remflag)
						    {co_ordinates temp; temp.x=pos[0]+tempx; temp.y=pos[1]+tempy; uobs.push_back(temp);}
						  
						  if (pos[0]+tempx==tarx && pos[1]+tempy==tary) break;
					}
					else if (dec==2)
					  {food[0]=tempx; food[1]=tempy; foodflag=1;}
					else {count++;dist2=pow(tarx-(pos[0]+tempx),2)+pow(tary-(pos[1]+tempy),2);}
				}
			}
			else obs[count++]=1;
		
			tempx=-1; tempy=0;
			if (pos[0]+tempx<10 && pos[1]+tempy<10 && pos[0]+tempx>=0 && pos[1]+tempy>=0)
			{
				if (!foodflag)
				{
					int dec=decode(pos[0]+tempx,pos[1]+tempy);
					ROS_INFO("3: %d",dec);
					if (dec==1)
					{
				  		obs[count++]=1;
				  		remflag=0;
						  for (int i=0; i<uobs.size();i++)
						  {
						  	if (uobs[i].x==pos[0]+tempx && uobs[i].y==pos[1]+tempy)
						  	{remflag=1;  break;}
						  }
						  for (int i=0; i<foods.size();i++)
						  {
						  	if (foods[i].x==pos[0]+tempx && foods[i].y==pos[1]+tempy)
						  	{remflag=1;  break;}
						  }
						  if (!remflag)
						    {co_ordinates temp; temp.x=pos[0]+tempx; temp.y=pos[1]+tempy; uobs.push_back(temp);}
						  
						  if (pos[0]+tempx==tarx && pos[1]+tempy==tary) break;
					}
					else if (dec==2)
					  {food[0]=tempx; food[1]=tempy; foodflag=1;}
					else {count++;dist3=pow(tarx-(pos[0]+tempx),2)+pow(tary-(pos[1]+tempy),2);}
				}
			}
			else obs[count++]=1;
		
			tempx=0; tempy=-1;
			if (pos[0]+tempx<10 && pos[1]+tempy<10 && pos[0]+tempx>=0 && pos[1]+tempy>=0)
			{
				if (!foodflag)
				{
					int dec=decode(pos[0]+tempx,pos[1]+tempy);
					ROS_INFO("4: %d",dec);
					if (dec==1)
					{
				  		obs[count++]=1;
				  		remflag=0;
						  for (int i=0; i<uobs.size();i++)
						  {
						  	if (uobs[i].x==pos[0]+tempx && uobs[i].y==pos[1]+tempy)
						  	{remflag=1;  break;}
						  }
						  for (int i=0; i<foods.size();i++)
						  {
						  	if (foods[i].x==pos[0]+tempx && foods[i].y==pos[1]+tempy)
						  	{remflag=1;  break;}
						  }
						  if (!remflag)
						    {co_ordinates temp; temp.x=pos[0]+tempx; temp.y=pos[1]+tempy; uobs.push_back(temp);}
						  
						  if (pos[0]+tempx==tarx && pos[1]+tempy==tary) break;
					}
					else if (dec==2)
					  {food[0]=tempx; food[1]=tempy; foodflag=1;}
					else {count++;dist4=pow(tarx-(pos[0]+tempx),2)+pow(tary-(pos[1]+tempy),2);}
				}
			}
			else obs[count++]=1;
			if (foodflag)
			{
				if (food[0]==1) {rot(1); fm(pos);}
				if (food[0]==-1) {rot(3); fm(pos);}
				if (food[1]==1) {rot(2); fm(pos);}
				if (food[1]==-1) {rot(4); fm(pos);}
				co_ordinates temp; temp.x=pos[0]; temp.y=pos[1];
				foods.push_back(temp);
				ROS_INFO("\n\n\n ********COLLECTED %d*********\n", cover+1);
				cover++;
				v.clear();
				for (int i=0;i<uobs.size();i++)
				  if (uobs[i].x==pos[0] && uobs[i].y==pos[1])  uobs.erase(uobs.begin()+i,uobs.begin()+i+1);
				if (pos[0]==tarx && pos[1]==tary)
					break;
			}
			else
			{
			
				ROS_INFO("d1: %f, d2: %f, d3 %f, d4: %f\n", dist1,dist2,dist3,dist4);
				if (dist1 <= dist2 && dist1 <= dist3 && dist1 <= dist4)
				{rot(1); fm(pos);}
				else if (dist2 <= dist1 && dist2 <= dist3 && dist2 <= dist4)
				{rot(2); fm(pos);}
				else if (dist3 <= dist2 && dist3 <= dist1 && dist3 <= dist4)
				{rot(3); fm(pos);}
				else if (dist4 <= dist2 && dist4 <= dist1 && dist4 <= dist3)
				{rot(4); fm(pos);}
			}
		}
		if (i>=uobs.size()-1 && loop_flag) i=0;
	}	
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

	ros::ServiceClient client = n.serviceClient<turtlesim::SetPen>("turtle2/set_pen");

	velocity_publisher = n.advertise<geometry_msgs::Twist>("/turtle1/cmd_vel", 1000);
	ros::Rate loop_rate(10);
	

	//	/turtle1/cmd_vel is the Topic name
	//	/geometry_msgs::Twist is the msg type 
	//move(2.5, 5, 0);
	//lt();
	//move(2.5, 5, 0);
	//rt();
	ROS_INFO("\n\n\n ********START*********\n");
	while(cover<10)
	{	
		ROS_INFO("\nNow at %d,%d\nCollected: %d\nObstacles hit: %ld\n",pos[0],pos[1],cover,uobs.size());
		int obs[4]={0,0,0,0}, food[2], flag=0;
		int tempx, tempy, count=0;

		tempx=1; tempy=0;
		if (pos[0]+tempx<10 && pos[1]+tempy<10 && pos[0]+tempx>=0 && pos[1]+tempy>=0)
		{
			if (!flag)
			{
				
				int dec=decode(pos[0]+tempx,pos[1]+tempy);
				ROS_INFO("1: %d",dec);
				if (dec==1)
				{
				  obs[count++]=1;
				  int remflag=0;
				  for (int i=0; i<uobs.size();i++)
				  {
				  	if (uobs[i].x==pos[0]+tempx && uobs[i].y==pos[1]+tempy)
				  	{remflag=1;  break;}
				  }
				  for (int i=0; i<foods.size();i++)
				  {
				  	if (foods[i].x==pos[0]+tempx && foods[i].y==pos[1]+tempy)
				  	{remflag=1;  break;}
				  }
				  if (!remflag)
				    {co_ordinates temp; temp.x=pos[0]+tempx; temp.y=pos[1]+tempy; uobs.push_back(temp);}
				}
				else if (dec==2)
				  {food[0]=tempx; food[1]=tempy; flag=1;}
				else count++;
			}
		}
		else obs[count++]=1;
		
		tempx=0; tempy=1;
		if (pos[0]+tempx<10 && pos[1]+tempy<10 && pos[0]+tempx>=0 && pos[1]+tempy>=0)
		{
			if (!flag)
			{
				int dec=decode(pos[0]+tempx,pos[1]+tempy);
				ROS_INFO("2: %d",dec);
				if (dec==1)
				{
				  obs[count++]=1;
				  int remflag=0;
				  for (int i=0; i<uobs.size();i++)
				  {
				  	if (uobs[i].x==pos[0]+tempx && uobs[i].y==pos[1]+tempy)
				  	{remflag=1;  break;}
				  }
				  for (int i=0; i<foods.size();i++)
				  {
				  	if (foods[i].x==pos[0]+tempx && foods[i].y==pos[1]+tempy)
				  	{remflag=1;  break;}
				  }
				  if (!remflag)
				    {co_ordinates temp; temp.x=pos[0]+tempx; temp.y=pos[1]+tempy; uobs.push_back(temp);}
				}
				else if (dec==2)
				  {food[0]=tempx; food[1]=tempy; flag=1;}
				else count++;
			}
		}
		else obs[count++]=1;
		
		tempx=-1; tempy=0;
		if (pos[0]+tempx<10 && pos[1]+tempy<10 && pos[0]+tempx>=0 && pos[1]+tempy>=0)
		{
			if (!flag)
			{
				int dec=decode(pos[0]+tempx,pos[1]+tempy);
				ROS_INFO("3: %d",dec);
				if (dec==1)
				{
				  obs[count++]=1;
				  int remflag=0;
				  for (int i=0; i<uobs.size();i++)
				  {
				  	if (uobs[i].x==pos[0]+tempx && uobs[i].y==pos[1]+tempy)
				  	{remflag=1;  break;}
				  }
				  for (int i=0; i<foods.size();i++)
				  {
				  	if (foods[i].x==pos[0]+tempx && foods[i].y==pos[1]+tempy)
				  	{remflag=1;  break;}
				  }
				  if (!remflag)
				    {co_ordinates temp; temp.x=pos[0]+tempx; temp.y=pos[1]+tempy; uobs.push_back(temp);}
				}
				else if (dec==2)
				  {food[0]=tempx; food[1]=tempy; flag=1;}
				else count++;
			}
			
		}
		else obs[count++]=1;
		
		tempx=0; tempy=-1;
		if (pos[0]+tempx<10 && pos[1]+tempy<10 && pos[0]+tempx>=0 && pos[1]+tempy>=0)
		{
			if (!flag)
			{
				int dec=decode(pos[0]+tempx,pos[1]+tempy);
				ROS_INFO("4: %d",dec);
				if (dec==1)
				{
				  obs[count++]=1;
				  int remflag=0;
				  for (int i=0; i<uobs.size();i++)
				  {
				  	if (uobs[i].x==pos[0]+tempx && uobs[i].y==pos[1]+tempy)
				  	{remflag=1;  break;}
				  }
				  for (int i=0; i<foods.size();i++)
				  {
				  	if (foods[i].x==pos[0]+tempx && foods[i].y==pos[1]+tempy)
				  	{remflag=1;  break;}
				  }
				  if (!remflag)
				    {co_ordinates temp; temp.x=pos[0]+tempx; temp.y=pos[1]+tempy; uobs.push_back(temp);}
				}
				else if (dec==2)
				  {food[0]=tempx; food[1]=tempy; flag=1;}
				else count++;
			}
		}
		else obs[count++]=1;
		//for (int i=0; i<v.size();i++)
		//{ 
		//  int here=v[i]; here/=10;
		//  if (here==pos[0]*10+pos[1])
		//    obs[v[i]%10]=1;
		//}
		vector <int> vnow;
		for (int i=0;i<4;i++)
		{
		  ROS_INFO("%d: %d\n",i+1,obs[i]);
		  if (obs[i]==0) vnow.push_back(i);
		}
		if (flag)
		{
			if (food[0]==1) {rot(1); fm(pos);}
			if (food[0]==-1) {rot(3); fm(pos);}
			if (food[1]==1) {rot(2); fm(pos);}
			if (food[1]==-1) {rot(4); fm(pos);}
			ROS_INFO("\n\n\n ********COLLECTED %d*********\n", cover+1);
			cover++;
			v.clear();
			for (int i=0;i<uobs.size();i++)
			{
				if (uobs[i].x==pos[0] && uobs[i].y==pos[1])
			 	      uobs.erase(uobs.begin()+i, uobs.begin()+i+1);
			}
			co_ordinates temp; temp.x=pos[0]; temp.y=pos[1];
			foods.push_back(temp);
			if (uobs.size()>0.6*(9-cover))
			  {a_star(); uobs.clear();}
		}
		else
		{
			int flag=1;
			if (obs[dir-1]==0)
			{
				int count=0;
				for (int i=0; i<v.size();i++)
				{ 
					int here=v[i]; here/=10;
					if (here==pos[0]*10+pos[1])
					  if (v[i]%10==dir) flag=0;
				}
				for (int i=0;i<4;i++)
				  if (i==dir-1) continue;
				  else if (obs[i]==1) count++;
				if (count>=3) flag=0;
				if (flag)
				{v.push_back(pos[0]*100+pos[1]*10+dir-1); fm(pos); flag=0;}
				else flag=1;
			}
			if (flag)
			{
				int posn=(int)rand() % (int)vnow.size();
				posn=vnow[posn];
				if 	(posn==0) {rot(1); v.push_back(pos[0]*100+pos[1]*10+1); fm(pos);}
				else if (posn==1) {rot(2); v.push_back(pos[0]*100+pos[1]*10+2); fm(pos);}
				else if (posn==2) {rot(3); v.push_back(pos[0]*100+pos[1]*10+3); fm(pos);}
				else if (posn==3) {rot(4); v.push_back(pos[0]*100+pos[1]*10+4); fm(pos);}
			}
			
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

