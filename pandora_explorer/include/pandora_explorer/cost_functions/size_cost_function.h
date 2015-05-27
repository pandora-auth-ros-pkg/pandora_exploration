/*********************************************************************
*
* Software License Agreement (BSD License)
*
*  Copyright (c) 2014 - 2015, P.A.N.D.O.R.A. Team.
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
* Author: Chris Zalidis <zalidis@gmail.com>,
          Dimitrios Kirtsios <dimkirts@gmail.com>
*********************************************************************/

#ifndef PANDORA_EXPLORER_COST_FUNCTIONS_SIZE_COST_FUNCTION_H
#define PANDORA_EXPLORER_COST_FUNCTIONS_SIZE_COST_FUNCTION_H

#include <boost/foreach.hpp>

#include "pandora_explorer/cost_functions/frontier_cost_function.h"

namespace pandora_explorer {

  /**
    * @class SizeCostFunction
    * @brief A class implementing a frontier cost function using the FrontierCostFunction
    * interface
    * 
    * This cost function adds costs to the frontiers based on the frontier size.
    * Frontiers bigger in size are added higher costs. Cost are normalized to a scale 
    * between 0 and 1. 
    */
  class SizeCostFunction : public FrontierCostFunction
  {
   public:

    /**
      * @brief Explicit constructor for the SizeCostFunction class
      * @param scale The weight we set on the cost function
      */
    explicit SizeCostFunction(double scale);

    /**
      * @brief Takes a list of frontiers and adds a cost to each one of them
      * @param frontier_list A pointer to a list of frontiers that are to be evaluated.
      * 
      * If a frontier has already assigned a negative cost then scoreFrontiers doesn't 
      * add any cost.
      */
    virtual void scoreFrontiers(const FrontierListPtr& frontier_list);

    /**
      * @brief Destructor for the SizeCostFunction class.
      */
    ~SizeCostFunction() {}
  };

}   // namespace pandora_explorer

#endif  // PANDORA_EXPLORER_COST_FUNCTIONS_SIZE_COST_FUNCTION_H
