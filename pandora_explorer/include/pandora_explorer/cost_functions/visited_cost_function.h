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

#ifndef PANDORA_EXPLORER_COST_FUNCTIONS_VISITED_COST_FUNCTION_H
#define PANDORA_EXPLORER_COST_FUNCTIONS_VISITED_COST_FUNCTION_H

#include <vector>
#include <boost/foreach.hpp>
#include <tf/tf.h>
#include <angles/angles.h>

#include "pandora_explorer/cost_functions/frontier_cost_function.h"

namespace pandora_explorer {

  class VisitedCostFunction : public FrontierCostFunction
  {
   public:

    VisitedCostFunction(double scale, const std::vector<geometry_msgs::PoseStamped>& selected_goals);

    virtual void scoreFrontiers(const FrontierListPtr& frontier_list);

    ~VisitedCostFunction() {}

   private:

    const std::vector<geometry_msgs::PoseStamped>& selected_goals_;
  };

} // namespace pandora_explorer

#endif  // PANDORA_EXPLORER_COST_FUNCTIONS_VISITED_COST_FUNCTION_H
