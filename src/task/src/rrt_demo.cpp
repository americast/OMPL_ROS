/*********************************************************************
* Software License Agreement (BSD License)
*
*  Copyright (c) 2013, Rice University
*  All rights reserved.
*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*   * Redistributions of source code must retain the above copyright
*     notice, this list of conditions and the following disclaimer.
*   * Redistributions in binary form must reproduce the above
*     copyright notice, this list of conditions and the following
*     disclaimer in the documentation and/or other materials provided
*     with the distribution.
*   * Neither the name of the Rice University nor the names of its
*     contributors may be used to endorse or promote products derived
*     from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
*  FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
*  COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
*  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
*  BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
*  LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
*  CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
*  LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
*  ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
*  POSSIBILITY OF SUCH DAMAGE.
*********************************************************************/

/* Author: Ioan Sucan */
#include "ros/ros.h"
#include "geometry_msgs/Point.h"
#include <sstream>
#include <ompl/base/spaces/RealVectorStateSpace.h>
#include <ompl/geometric/SimpleSetup.h>
#include <ompl/geometric/planners/rrt/RRT.h>
#include <ompl/geometric/planners/rrt/RRTstar.h>
#include <ompl/geometric/planners/rrt/RRTConnect.h>
#include <ompl/util/PPM.h>
#include <string>
#include <ompl/config.h>

#include <boost/filesystem.hpp>
#include <iostream>
#include <functional>
using namespace std ;
using namespace boost;

namespace ob = ompl::base;
namespace og = ompl::geometric;

class Plane2DEnvironment
{
public:
     ompl::PPM ppm_;
    

    Plane2DEnvironment(const char *ppm_file)
    {
        bool ok ;
         
            ppm_.loadFile(ppm_file);
            cout<< "sdsd";
            ok = true;
        
        if (ok)
        {
          auto ob::StateSpacePtr *space = new ob::RealVectorStateSpace();
           //auto space(make_shared<ob::RealVectorStateSpace>());
            space->addDimension(0.0, ppm_.getWidth());
            space->addDimension(0.0, ppm_.getHeight());
            maxWidth_ = ppm_.getWidth() - 1;
            maxHeight_ = ppm_.getHeight() - 1;
           ss_.reset(new og::SimpleSetup(ob::StateSpacePtr(space)));
           // ss_ = make_shared<og::SimpleSetup>(space);

            //set state validity checking for this space
              ss_->setStateValidityChecker(boost::bind(&Plane2DEnvironment::isStateValid, this, _1));
            //ss_->setStateValidityChecker(bind(&Plane2DEnvironment::isStateValid, this, std::placeholders::_1));
           // ss_->setStateValidityChecker([this](const ob::State *state) { return isStateValid(state); });
            space->setup();
            ss_->getSpaceInformation()->setStateValidityCheckingResolution(1.0 / space->getMaximumExtent());
              ss_->setPlanner(ob::PlannerPtr(new og::RRTConnect(ss_->getSpaceInformation())));
           // ss_->setPlanner(make_shared<og::RRTConnect>(ss_->getSpaceInformation()));
        }
    }

    bool plan(unsigned int start_row, unsigned int start_col, unsigned int goal_row, unsigned int goal_col)
    {
        if (!ss_)
            return false;
        ob::ScopedState<> start(ss_->getStateSpace());
        start[0] = start_row;
        start[1] = start_col;
        ob::ScopedState<> goal(ss_->getStateSpace());
        goal[0] = goal_row;
        goal[1] = goal_col;
        ss_->setStartAndGoalStates(start, goal);
    
            if (ss_->getPlanner())
                ss_->getPlanner()->clear();
            ss_->solve();
       
        const std::size_t ns = ss_->getProblemDefinition()->getSolutionCount();
        OMPL_INFORM("Found %d solutions", (int)ns);
        if (ss_->haveSolutionPath())
        {
            ss_->simplifySolution();
            og::PathGeometric &p = ss_->getSolutionPath();
            ss_->getPathSimplifier()->simplifyMax(p);
            ss_->getPathSimplifier()->smoothBSpline(p);
            return true;
        }
        
            return false;
    }

    void recordSolution()
    {
        if (!ss_ || !ss_->haveSolutionPath())
            return;
        og::PathGeometric &p = ss_->getSolutionPath();
        p.interpolate();
        for (std::size_t i = 0 ; i < p.getStateCount() ; ++i)
        {
            const int w = std::min(maxWidth_, (int)p.getState(i)->as<ob::RealVectorStateSpace::StateType>()->values[0]);
            const int h = std::min(maxHeight_, (int)p.getState(i)->as<ob::RealVectorStateSpace::StateType>()->values[1]);
            ompl::PPM::Color &c = ppm_.getPixel(h, w);
            c.red = 255;
            c.green = 0;
            c.blue = 0;
        }
    }

    void save(const char *filename)
    { 
        if (!ss_)
            return;
        ppm_.saveFile(filename);
    }

private:

    bool isStateValid(const ob::State *state) const
    {
        const int w = std::min((int)state->as<ob::RealVectorStateSpace::StateType>()->values[0], maxWidth_);
        const int h = std::min((int)state->as<ob::RealVectorStateSpace::StateType>()->values[1], maxHeight_);

        const ompl::PPM::Color &c = ppm_.getPixel(h, w);
        return c.red >=200 && c.green >=200  && c.blue >=200 ;
    }

    og::SimpleSetupPtr ss_;
    int maxWidth_;
    int maxHeight_;
   

};

int main(int argc,char **argv)
{
     ros::init(argc,argv,"publisher");
     ros::NodeHandle nh;
    srand ( time(NULL) );          
     ros::Publisher pub=nh.advertise<geometry_msgs::Point>("points",2000);
    std::cout << "OMPL version: " << OMPL_VERSION << std::endl;

   // boost::filesystem::path path(TEST_RESOURCES_DIR);
    const char *str="../floor.ppm";
    Plane2DEnvironment env(str);
    if (env.plan(0, 0, 1200,1500))
    {
        env.recordSolution();
        int x= rand()%100;
        stringstream ss;
        ss<<"result";
        ss << x;
        string str = ss.str();
        cout<<str<<endl;
        const char *res =str.c_str();
        cout << x << endl;
        cout<< res <<" kfndf\nidid\\dwwd\n";
        env.save(res);
    }

    return 0;
}
