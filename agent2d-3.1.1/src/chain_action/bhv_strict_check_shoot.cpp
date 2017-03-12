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

//Values obtained from the normalization before the games
//Genius
// float maxBxShotS = 10.68678, maxByShotS = 5.35688, minBxShotS = -4.23784, minByShotS = -5.4228, maxShotS = 123.6549;

//Helios
// float maxBxShotS = 10.60262, maxByShotS = 4.33662, minBxShotS = -10.39796, minByShotS = -4.68052, maxShotS = 120.5484;

//Hermes
// float maxBxShotS = 10.55944, maxByShotS = 6.73782, minBxShotS = -2.76568, minByShotS = -6.42094, maxShotS = 126.6197;

//Jaeger
float maxBxShotS = 10.67964, maxByShotS = 6.65094, minBxShotS = 0.84936, minByShotS = -6.11404, maxShotS = 128.5996;

//WrightEagle
//float maxBxShotS = 10.6253, maxByShotS = 6.8, minBxShotS = -10.53776, minByShotS = -6.6, maxShotS = 128.045;

double distFromLineShoot(Vector2D p0, Vector2D p1, Vector2D p2){
  float a,b,c;
  float num, denom;

  if (p0.x != p1.x){
    a = -(p1.y - p0.y)/(p1.x - p0.x);
    c = (((p1.y - p0.y)*p0.x)/(p1.x - p0.x)) - p0.y;
    b = 1;
  } else {
    a = 1;
    b = 0;
    c = p0.x;
  }

  num = abs(a*p2.x + p2.y + c);
  denom = sqrt(pow(a,2) + b);

  return num/denom;

}

cv::Mat
extractFeaturesShoot(PlayerAgent* agent, Vector2D targetPoint){
    PlayerCont allOpps;
    PlayerCont allTeammts;
    PlayerCont::iterator iter;
    Mat features = Mat::zeros(1, 24, CV_32F);

    // Ball position
    Vector2D ballPos = agent->world().ball().pos();

    features.at<float>(0,0) = ((ballPos.x/5) - minBxShotS)/(maxBxShotS-minBxShotS);
    features.at<float>(0,1) = ((ballPos.y/5) - minByShotS)/(maxByShotS-minByShotS);

    // Calculating Teammates
    int i = 2;

    features.at<float>(0,agent->world().self().unum()+1) = distFromLineShoot(ballPos, targetPoint, agent->world().self().pos())/maxShotS;

    allTeammts = agent->world().teammates();
    for (iter = allTeammts.begin(); iter != allTeammts.end(); iter++) {
        if (i-1 == agent->world().self().unum()){
          i++;
        }

        features.at<float>(0,i) = distFromLineShoot(ballPos, targetPoint, iter->pos())/maxShotS;
        i++;
    }

    // Calculating Opponents.
    allOpps = agent->world().opponents();
    for (iter = allOpps.begin(); iter != allOpps.end(); iter++) {
        features.at<float>(0,i) = distFromLineShoot(ballPos, targetPoint, iter->pos())/maxShotS;
        i++;
    }

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

    CvDTree shootTree;
    //CAMBIAR POR EL PATH DEL ARBOL DEL EQUIPO CORRESPONDIENTE
    shootTree.load("/home/vicky/Documents/Repositorio/robocup-coach/agent2d-3.1.1/src/trainedTrees/Genius/shotTree.yml");
    // shootTree.load("/home/jemd/Documents/USB/Tesis/robocup-coach/agent2d-3.1.1/src/trainedTrees/Jaeger/shotTree.yml");

    cv::Mat testSample(extractFeaturesShoot(agent, best_shoot->target_point_));
    
    if ( one_step_speed > best_shoot->first_ball_speed_ * 0.99 )
    { 
        // It will be a successful shoot.
      if (shootTree.predict(testSample)->value == 1){
          if ( Body_SmartKick( best_shoot->target_point_,
                               one_step_speed,
                               one_step_speed * 0.99 - 0.0001,
                               1 ).execute( agent ) )
          {
               agent->setNeckAction( new Neck_TurnToGoalieOrScan( -1 ) );
               agent->debugClient().addMessage( "Force1Step" );
               //std::cout << "Shoot " << std::endl;
               return true;
          }
      }
    }

    // It will be a successful shoot.
    if (shootTree.predict(testSample)->value == 1){
      if ( Body_SmartKick( best_shoot->target_point_,
                           best_shoot->first_ball_speed_,
                           best_shoot->first_ball_speed_ * 0.99,
                           3 ).execute( agent ) )
      {
          if ( ! doTurnNeckToShootPoint( agent, best_shoot->target_point_ ) )
          {
              agent->setNeckAction( new Neck_TurnToGoalieOrScan( -1 ) );
          }
          //std::cout << "Shoot " << std::endl;
          return true;
      }
    }

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
