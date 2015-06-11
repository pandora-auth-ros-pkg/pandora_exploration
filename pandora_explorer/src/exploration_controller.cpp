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
* Author: Chris Zalidis <zalidis@gmail.com>
*********************************************************************/

#include "pandora_explorer/exploration_controller.h"

namespace pandora_explorer {

ExplorationController::ExplorationController()
  : private_nh_("~"),
    goal_searches_count_(0),
    abort_count_(0),
    aborted_(false),
    do_exploration_server_(nh_, "do_exploration",
                           boost::bind(&ExplorationController::executeCb, this, _1), false),
    move_base_client_("move_base", true),
    first_time_(true),
    goal_reached_(false),
    goal_expired_count_(0)
{
  // create explore frontier goal selector
  explore_goal_selector_.reset(new FrontierGoalSelector("explore"));

  // load use_coverage
  bool use_coverage;
  private_nh_.param<bool>("use_coverage", use_coverage, false);

  // if use_coverage is enabled we create a frontier goal selector with the name coverage
  if (use_coverage)
    coverage_goal_selector_.reset(new FrontierGoalSelector("coverage"));

  // register preempt callback
  do_exploration_server_.registerPreemptCallback(
      boost::bind(&ExplorationController::preemptCb, this));

  // load max goal searches
  private_nh_.param<int>("max_goal_searches", max_goal_searches_, 5);
  private_nh_.param<int>("max_abortions", max_abortions_, 5);

  // proporsional to number of frontiers?
  private_nh_.param<double>("goal_reached_dist", goal_reached_dist_, 1.0);

  // robot has that many seconds to reach a goal
  // TODO(czalidis): it could be proportional to path's length
  double goal_timeout;
  private_nh_.param<double>("goal_timeout", goal_timeout, 20.0);
  goal_timeout_ = ros::Duration(goal_timeout);

  // start exploration server
  do_exploration_server_.start();
}

void ExplorationController::executeCb(
    const pandora_exploration_msgs::DoExplorationGoalConstPtr& goal)
{
  // wait for move_base to set-up
  if (!move_base_client_.waitForServer(ros::Duration(1.0)))
  {
    do_exploration_server_.setAborted(pandora_exploration_msgs::DoExplorationResult(),
                                      "Could not connect to move_base action");
    return;
  }

  ROS_INFO("[%s] Received request to explore the world!", ros::this_node::getName().c_str());

  // reset aborts from previous time, this shouldn't really happen
  abort_count_ = 0;
  goal_searches_count_ = 0;

  // while we didn't receive a preempt request
  while (ros::ok() && do_exploration_server_.isActive()) {
    // we reached maximum goal searches retries
    // if we reach maximum goal searches but there are frontiers found
    // should not set succeed the exploration
    // Maybe we should wait move_base to finish
    if (goal_searches_count_ >= max_goal_searches_ && goal_reached_) {
      do_exploration_server_.setSucceeded(pandora_exploration_msgs::DoExplorationResult(),
      "[pandora_explorer] Max retries reached, we could not find more goals - exploration completed");
      return;
    }
    // Check for timeouts
    /*int max_goal_timeouts = 5;
    if (goal_searches_count_ >= max_goal_searches_ && goal_expired_count_ >= max_goal_timeouts) {
      do_exploration_server_.setAborted(pandora_exploration_msgs::DoExplorationResult(),
      "[pandora_explorer] Max retries reached, goal is not reached, move_base appears to be stucked");
      return;
    }
*/
    if (abort_count_ >= max_abortions_) {
      do_exploration_server_.setAborted(pandora_exploration_msgs::DoExplorationResult(),
                                        "Robot refuses to move, aborting...");
      return;
    }

    bool success = false;

    if (goal->exploration_type == pandora_exploration_msgs::DoExplorationGoal::TYPE_DEEP &&
        coverage_goal_selector_) {
      // to current goal gemizetai me to stoxo pou tha vrei o goal selector
      // If success is false that means we cant find more frontiers
      success = coverage_goal_selector_->findNextGoal(&current_goal_);
    } else {
      success = explore_goal_selector_->findNextGoal(&current_goal_);
    }

    // if succes is false, dld an den exei vrei stoxo, auksanoume ta goal searches
    if (!success) {
      goal_searches_count_++;
      // wait a little
      ros::Duration(0.2).sleep();
      continue;
    }
    // we have a valid goal
    else {
      // reset failures
      goal_searches_count_ = 0;
      aborted_ = false;

      // prepare a MoveBaseGoal
      move_base_msgs::MoveBaseGoal move_base_goal;
      move_base_goal.target_pose = current_goal_;

      // send new goal to move_base
      // cancel goals before?
      move_base_client_.sendGoal(
          move_base_goal, boost::bind(&ExplorationController::doneMovingCb, this, _1, _2), NULL,
          boost::bind(&ExplorationController::feedbackMovingCb, this, _1));

      // set selected goal to all goal selectors
      explore_goal_selector_->setSelectedGoal(current_goal_);
      if (coverage_goal_selector_)
        coverage_goal_selector_->setSelectedGoal(current_goal_);
    }

    while (ros::ok() && do_exploration_server_.isActive() && goal_reached_ != true && !isTimeReached() &&
           !aborted_)
      ros::Duration(0.1).sleep();
    goal_reached_ = false;
  }  // end of outer while

  // goal should never be active at this point
  ROS_ASSERT(!do_exploration_server_.isActive());
}

void ExplorationController::feedbackMovingCb(
    const move_base_msgs::MoveBaseFeedbackConstPtr& feedback)
{
  feedback_.base_position = feedback->base_position;
  do_exploration_server_.publishFeedback(feedback_);
}

void ExplorationController::doneMovingCb(const actionlib::SimpleClientGoalState& state,
                                         const move_base_msgs::MoveBaseResultConstPtr& result)
{
  if (state == actionlib::SimpleClientGoalState::ABORTED) {
    abort_count_++;
    aborted_ = true;
  }
  if (state == actionlib::SimpleClientGoalState::SUCCEEDED)
  {
    ROS_WARN("[pandora_explorer] How much fire tell me");
    goal_reached_ = true;
  }
}

void ExplorationController::preemptCb()
{
  move_base_client_.cancelGoalsAtAndBeforeTime(ros::Time::now());
  ROS_WARN("[%s] Preempt request from up-stairs!", ros::this_node::getName().c_str());

  if (do_exploration_server_.isActive()) do_exploration_server_.setPreempted();
}

bool ExplorationController::isGoalReached()
{
  // check if we are close to target
  // TODO(czalidis): check for race condition
  // Auto ousiastika klanei to navi gt den perimenei na tou pei an eftase, alla
  // to apofasizei mono tou, omws den douleuei swsta gt otan ftanei to navi,
  // o explorer perimenei na faei timeout gia na vgalei epomeno stoxo.
  double dx = current_goal_.pose.position.x - feedback_.base_position.pose.position.x;
  double dy = current_goal_.pose.position.y - feedback_.base_position.pose.position.y;

  double dist = goal_reached_dist_;

  if (first_time_) {
    dist = 0.1;
  }

  if (::hypot(dx, dy) < dist) {
    abort_count_ = 0;
    first_time_ = false;
    ROS_INFO("[pandora_explorer] goal is reached");
    return true;
  }

  return false;
}

bool ExplorationController::isTimeReached()
{
  // something is wrong here
  if (ros::Time::now() - current_goal_.header.stamp < goal_timeout_) {
    return false;
  }
 // goal_expired_count_ ++;
 // if (goal_expired_count_ >= 5)
   // goal_expired_count_ = 0;
  ROS_INFO("[%s] Time for goal expired!", ros::this_node::getName().c_str());
  return true;
}

}  // namespace pandora_explorer
