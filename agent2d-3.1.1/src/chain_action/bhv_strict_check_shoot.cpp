// -*-c++-*-

/*!
  \file bhv_strict_check_shoot.cpp
  \brief strict checked shoot behavior using ShootGenerator
*/

/*
 *Copyright:

 Copyright (C) Hidehisa AKIYAMA

 This code is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 3 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

 *EndCopyright:
 */

/////////////////////////////////////////////////////////////////////

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "bhv_strict_check_shoot.h"

#include "shoot_generator.h"

#include <rcsc/action/neck_turn_to_goalie_or_scan.h>
#include <rcsc/action/neck_turn_to_point.h>
#include <rcsc/action/body_smart_kick.h>
#include <rcsc/player/player_agent.h>
#include <rcsc/player/debug_client.h>
#include <rcsc/common/logger.h>

#include "cv.h"
#include "ml.h"

#include <algorithm>

using namespace cv;
using namespace rcsc;

double nearestTeammateShoot(Vector2D teammate1, Vector2D teammate2, Vector2D teammate3, Vector2D opponnent){
    double dists[3];
    dists[0] = opponnent.dist(teammate1);
    dists[1] = opponnent.dist(teammate2);
    dists[2] = opponnent.dist(teammate3);

    return *std::min_element(dists,dists+3);

}

cv::Mat
extractFeaturesShoot(PlayerAgent* agent, Vector2D targetPoint){
    PlayerCont allOpps;
    PlayerCont allTeammts;
    PlayerCont::iterator iter;
    double distOpp;
    double distAux;

    // Ball position
    Vector2D ballPos = agent->world().ball().pos();

    // Calculating Teammate2.
    allTeammts = agent->world().teammates();
    distOpp = 10000000000;
    distAux = 0; 
    PlayerObject teammate2;
    for (iter = allTeammts.begin(); iter != allTeammts.end(); iter++) {
        distAux = agent->world().self().pos().dist(iter->pos());
        if ((distAux < distOpp) and (iter->unum() != agent->world().self().unum())) {
            distOpp = distAux;  
            teammate2 = *iter;     
        }
    }

    // Calculating teammate3
    allTeammts = agent->world().teammates();
    distOpp = std::numeric_limits<double>::max();
    distAux = 0; 
    PlayerObject teammate3;
    for (iter = allTeammts.begin(); iter != allTeammts.end(); iter++) {
        distAux = targetPoint.dist(iter->pos());

        if ((distAux < distOpp) and (teammate2.unum() != iter->unum()) and (iter->unum() != agent->world().self().unum())){
            distOpp = distAux;   
            teammate3 = *iter;     
        }
    }

    // Calculating Opponent1.
    allOpps = agent->world().opponents();
    distOpp = 10000000000;
    distAux = 0; 
    PlayerObject opponent1;
    for (iter = allOpps.begin(); iter != allOpps.end(); iter++) {
        distAux = agent->world().self().pos().dist(iter->pos());
        if (distAux < distOpp) {
            distOpp = distAux;  
            opponent1 = *iter;     
        }
    }

    // Calculating Opponent2.
    allOpps = agent->world().opponents();
    distOpp = std::numeric_limits<double>::max();
    distAux = 0; 
    PlayerObject opponent2;
    for (iter = allOpps.begin(); iter != allOpps.end(); iter++) {
        distAux = targetPoint.dist(iter->pos());

        if ((distAux < distOpp) and (opponent1.unum() != iter->unum())){
            distOpp = distAux;   
            opponent2 = *iter;     
        }
    }

    // Opponents nearest to the middle point of action path
    const double midPointX = (ballPos.x + targetPoint.x) / 2;
    const double midPointY = (ballPos.y + targetPoint.y) / 2;
    const Vector2D midPoint = Vector2D(midPointX,midPointY);

    // Calculating Opponent3.
    allOpps = agent->world().opponents();
    distOpp = std::numeric_limits<double>::max();
    distAux = 0; 
    PlayerObject opponent3;
    allOpps = agent->world().opponents(); // Grab all opponents from the world model
    for (iter = allOpps.begin(); iter != allOpps.end(); iter++) {
        distAux = midPoint.dist(iter->pos());

        if ((iter->unum() != opponent2.unum()) and (iter->unum() != opponent1.unum()) and (distAux < distOpp)) {
            distOpp = distAux;   
            opponent3 = *iter;     
        }
    }

    // Calculating Opponent4'
    allOpps = agent->world().opponents();
    distOpp = std::numeric_limits<double>::max();
    distAux = 0; 
    PlayerObject opponent4;
    allOpps = agent->world().opponents(); // Grab all opponents from the world model
    double distOpp3Aux = midPoint.dist(opponent3.pos());
    for (iter = allOpps.begin(); iter != allOpps.end(); iter++) {
        distAux = midPoint.dist(iter->pos());

        if ((iter->unum() != opponent3.unum()) and (iter->unum() != opponent2.unum()) and 
            (iter->unum() != opponent1.unum()) and (distAux < distOpp) and (distAux >= distOpp3Aux)) {
            distOpp = distAux;   
            opponent4 = *iter;     
        }
    }

    // Second preprocessing
    ballPos.assign(ballPos.x/5, ballPos.y/5);
    double distT1 = ballPos.dist(agent->world().self().pos());
    double distT2 = ballPos.dist(teammate2.pos());
    double distT3 = ballPos.dist(teammate3.pos());

    double distO1 = nearestTeammateShoot(agent->world().self().pos(), teammate2.pos(), teammate3.pos(), opponent1.pos());
    double distO2 = nearestTeammateShoot(agent->world().self().pos(), teammate2.pos(), teammate3.pos(), opponent2.pos());
    double distO3 = nearestTeammateShoot(agent->world().self().pos(), teammate2.pos(), teammate3.pos(), opponent3.pos());
    double distO4 = nearestTeammateShoot(agent->world().self().pos(), teammate2.pos(), teammate3.pos(), opponent4.pos());

    Mat features = (Mat_<float>(1,10) << ballPos.x, ballPos.y, distT1, distT2, distT3, distO1, distO2, distO3, distO4);
    
    return features;
}


/*-------------------------------------------------------------------*/
/*!

*/
bool
Bhv_StrictCheckShoot::execute( PlayerAgent * agent )
{
    const WorldModel & wm = agent->world();

    if ( ! wm.self().isKickable() )
    {
        std::cerr << __FILE__ << ": " << __LINE__
                  << " not ball kickable!"
                  << std::endl;
        dlog.addText( Logger::ACTION,
                      __FILE__":  not kickable" );
        return false;
    }

    const ShootGenerator::Container & cont = ShootGenerator::instance().courses( wm );

    // update
    if ( cont.empty() )
    {
        dlog.addText( Logger::SHOOT,
                      __FILE__": no shoot course" );
        return false;
    }

    ShootGenerator::Container::const_iterator best_shoot
        = std::min_element( cont.begin(),
                            cont.end(),
                            ShootGenerator::ScoreCmp() );

    if ( best_shoot == cont.end() )
    {
        dlog.addText( Logger::SHOOT,
                      __FILE__": no best shoot" );
        return false;
    }

    // it is necessary to evaluate shoot courses

    agent->debugClient().addMessage( "Shoot" );
    agent->debugClient().setTarget( best_shoot->target_point_ );

    Vector2D one_step_vel
        = KickTable::calc_max_velocity( ( best_shoot->target_point_ - wm.ball().pos() ).th(),
                                        wm.self().kickRate(),
                                        wm.ball().vel() );
    double one_step_speed = one_step_vel.r();

    dlog.addText( Logger::SHOOT,
                  __FILE__": shoot[%d] target=(%.2f, %.2f) first_speed=%f one_kick_max_speed=%f",
                  best_shoot->index_,
                  best_shoot->target_point_.x,
                  best_shoot->target_point_.y,
                  best_shoot->first_ball_speed_,
                  one_step_speed );

   /* CvDTree shootTree;
    shootTree.load("trainedTrees/shootTree.yml");

    cv::Mat testSample(extractFeaturesShoot(agent, best_shoot->target_point_));*/

    if ( one_step_speed > best_shoot->first_ball_speed_ * 0.99 )
    { 
        // It will be a successful shoot.
        //if (shootTree.predict(testSample)->value >= 0.5){
          if ( Body_SmartKick( best_shoot->target_point_,
                               one_step_speed,
                               one_step_speed * 0.99 - 0.0001,
                               1 ).execute( agent ) )
          {
               agent->setNeckAction( new Neck_TurnToGoalieOrScan( -1 ) );
               agent->debugClient().addMessage( "Force1Step" );
               std::cout << "Shoot " << std::endl;
               return true;
          }
        //}
    }

    // It will be a successful shoot.
    //if (shootTree.predict(testSample)->value >= 0.5){
      if ( Body_SmartKick( best_shoot->target_point_,
                           best_shoot->first_ball_speed_,
                           best_shoot->first_ball_speed_ * 0.99,
                           3 ).execute( agent ) )
      {
          if ( ! doTurnNeckToShootPoint( agent, best_shoot->target_point_ ) )
          {
              agent->setNeckAction( new Neck_TurnToGoalieOrScan( -1 ) );
          }
          std::cout << "Shoot " << std::endl;
          return true;
      }
    //}

    dlog.addText( Logger::SHOOT,
                  __FILE__": failed" );
    return false;
}


/*-------------------------------------------------------------------*/
/*!

 */
bool
Bhv_StrictCheckShoot::doTurnNeckToShootPoint( PlayerAgent * agent,
                                              const Vector2D & shoot_point )
{
    const double angle_buf = 10.0; // Magic Number

    if ( ! agent->effector().queuedNextCanSeeWithTurnNeck( shoot_point, angle_buf ) )
    {
        dlog.addText( Logger::TEAM,
                      __FILE__": cannot look the shoot point(%.2f %.2f)",
                      shoot_point.x,
                      shoot_point.y );
        return false;
    }

#if 0
    const WorldModel & wm = agent->world();
    if ( wm.seeTime() == wm.time() )
    {
        double current_width = wm.self().viewWidth().width();
        AngleDeg target_angle = ( shoot_point - wm.self().pos() ).th();
        double angle_diff = ( target_angle - wm.self().face() ).abs();

        if ( angle_diff < current_width*0.5 - angle_buf )
        {
            dlog.addText( Logger::TEAM,
                          __FILE__": already seen. width=%.1f, diff=%.1f. shoot point(%.2f %.2f)",
                          current_width,
                          angle_diff,
                          shoot_point.x,
                          shoot_point.y );
            return false;
        }
    }
#endif

    dlog.addText( Logger::TEAM,
                  __FILE__": turn_neck to the shoot point(%.2f %.2f)",
                  shoot_point.x,
                  shoot_point.y );
    agent->debugClient().addMessage( "Shoot:NeckToTarget" );

    agent->setNeckAction( new Neck_TurnToPoint( shoot_point ) );

    return true;
}
