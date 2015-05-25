/*********************************************************************
*
* Software License Agreement (BSD License)
*
*  Copyright (c) 2014, P.A.N.D.O.R.A. Team.
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
*   * Neither the name of the P.A.N.D.O.R.A. Team nor the names of its
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
*
* Author: Chris Zalidis <zalidis@gmail.com>
*********************************************************************/

#ifndef PANDORA_EXPLORER_NAVFN_SERVICE_FRONTIER_PATH_GENERATOR_H
#define PANDORA_EXPLORER_NAVFN_SERVICE_FRONTIER_PATH_GENERATOR_H

#include <string>
#include <ros/ros.h>
#include <nav_msgs/GetPlan.h>
#include <boost/foreach.hpp>

#include "pandora_explorer/frontier_path_generator.h"

namespace pandora_explorer {

class NavfnServiceFrontierPathGenerator : public FrontierPathGenerator {
 public:
  NavfnServiceFrontierPathGenerator(const std::string& name,
                                    const std::string& frontier_representation,
                                    ros::Duration max_duration = ros::Duration(5.0));

  virtual bool findPaths(const geometry_msgs::PoseStamped& start,
                         const FrontierListPtr& frontier_list);

  inline void setExpiration(const ros::Duration& max_duration)
  {
    max_duration_ = max_duration;
  }

  ~NavfnServiceFrontierPathGenerator()
  {
  }

 private:
  ros::NodeHandle nh_;
  ros::NodeHandle pnh_;

  ros::Duration max_duration_;

  ros::ServiceClient path_client_;
};

}  // namespace pandora_explorer

#endif  // PANDORA_EXPLORER_NAVFN_SERVICE_FRONTIER_PATH_GENERATOR_H
