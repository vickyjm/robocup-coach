// -*-c++-*-

/*
 *Copyright:

 Copyright (C) Hidehisa AKIYAMA

 This code is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 3, or (at your option)
 any later version.

 This code is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this code; see the file COPYING.  If not, write to
 the Free Software Foundation, 675 Mass Ave, Cambridge, MA 02139, USA.

 *EndCopyright:
 */

/////////////////////////////////////////////////////////////////////

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "bhv_basic_offensive_kick.h"

#include <rcsc/action/body_advance_ball.h>
#include <rcsc/action/body_dribble.h>
#include <rcsc/action/body_hold_ball.h>
#include <rcsc/action/body_pass.h>
#include <rcsc/action/neck_scan_field.h>
#include <rcsc/action/neck_turn_to_low_conf_teammate.h>

#include <rcsc/player/player_agent.h>
#include <rcsc/player/debug_client.h>

#include <rcsc/common/logger.h>
#include <rcsc/common/server_param.h>
#include <rcsc/geom/sector_2d.h>

#include "cv.h"
#include "ml.h"

#include <algorithm>

using namespace cv;
using namespace rcsc;

//Values obtained from the normalization before the games
//Genius
// float maxBxDribbleO = 10.50302, maxByDribbleO = 6.81262, minBxDribbleO = -10.52726, minByDribbleO = -6.73452, maxDribbleO = 136.0912;
// float maxBxPassO = 10.51674, maxByPassO = 6.81434, minBxPassO = -11.05596, minByPassO = -6.81646, maxPassO = 127.3224;

//Helios
// float maxBxDribbleO = 10.48934, maxByDribbleO = 6.80904, minBxDribbleO = -10.42648, minByDribbleO = -6.79716, maxDribbleO = 134.2811;
// float maxBxPassO = 10.63988, maxByPassO = 6.81484, minBxPassO = -10.74836, minByPassO = -6.80434, maxPassO = 117.5729;


//Hermes
// float maxBxDribbleO = 10.50934, maxByDribbleO = 6.79544, minBxDribbleO = -10.21012, minByDribbleO = -6.80346, maxDribbleO = 134.9107;
// float maxBxPassO = 10.51576, maxByPassO = 6.8, minBxPassO = -10.63878, minByPassO = -6.8141, maxPassO = 135.6462;


//Jaeger
float maxBxDribbleO = 10.5067, maxByDribbleO = 6.8, minBxDribbleO = -10.54852, minByDribbleO = -6.81356, maxDribbleO = 137.1277;
float maxBxPassO = 10.57736, maxByPassO = 6.813, minBxPassO = -10.7262, minByPassO = -6.81652, maxPassO = 124.1222;


//WrightEagle
//float maxBxDribbleO = 10.51572, maxByDribbleO = 6.80018, minBxDribbleO = -10.47292, minByDribbleO = -6.8, maxDribbleO = 135.3868;
//float maxBxPassO = 10.66324, maxByPassO = 6.81448, minBxPassO = -10.76028, minByPassO = -6.81686, maxPassO = 135.8845;



double distFromLineKick(Vector2D p0, Vector2D p1, Vector2D p2){
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
extractFeaturesKick(PlayerAgent* agent, Vector2D targetPoint, int isPass){
    PlayerCont allOpps;
    PlayerCont allTeammts;
    PlayerCont::iterator iter;
    Mat features = Mat::zeros(1, 24, CV_32F);
    float maxBx, maxBy, minBx, minBy, maxPlays;

    if (isPass == 1){
      maxBx = maxBxPassO, maxBy = maxByPassO;
      minBx = minBxPassO, minBy = minByPassO;
      maxPlays = maxPassO;
    } else {
      maxBx = maxBxDribbleO, maxBy = maxByDribbleO;
      minBx = minBxDribbleO, minBy = minByDribbleO;
      maxPlays = maxDribbleO;
    }

    // Ball position
    Vector2D ballPos = agent->world().ball().pos();

    features.at<float>(0,0) = ((ballPos.x/5)-minBx)/(maxBx-minBx);
    features.at<float>(0,1) = ((ballPos.y/5)-minBy)/(maxBy-minBy);

    //Owner
    features.at<float>(0,agent->world().self().unum()+1) = distFromLineKick(ballPos, targetPoint, agent->world().self().pos())/maxPlays;

    // Calculating Teammates
    int i = 2;
    allTeammts = agent->world().teammates();
    for (iter = allTeammts.begin(); iter != allTeammts.end(); iter++) {
        if (i-1 == agent->world().self().unum()){
            i++;
        }
        features.at<float>(0,i) = distFromLineKick(ballPos, targetPoint, iter->pos())/maxPlays;
        i++;
    }

    // Calculating Opponents.
    allOpps = agent->world().opponents();
    for (iter = allOpps.begin(); iter != allOpps.end(); iter++) {
        features.at<float>(0,i) = distFromLineKick(ballPos, targetPoint, iter->pos())/maxPlays;
        i++;
    }
    
    return features;
}


/*-------------------------------------------------------------------*/
/*!

 */
bool
Bhv_BasicOffensiveKick::execute( PlayerAgent * agent )
{

    //CAMBIAR POR EL PATH DE LOS ARBOLES DEL EQUIPO CORRESPONDIENTE
    CvDTree dribbleTree;
    CvDTree passTree;
    dribbleTree.load("/home/vicky/Documents/Repositorio/robocup-coach/agent2d-3.1.1/src/trainedTrees/Helios/dribbleTree.yml");
    passTree.load("/home/vicky/Documents/Repositorio/robocup-coach/agent2d-3.1.1/src/trainedTrees/Helios/passTree.yml");
    // dribbleTree.load("/home/jemd/Documents/USB/Tesis/robocup-coach/agent2d-3.1.1/src/trainedTrees/Jaeger/dribbleTree.yml");
    // passTree.load("/home/jemd/Documents/USB/Tesis/robocup-coach/agent2d-3.1.1/src/trainedTrees/Jaeger/passTree.yml");


    dlog.addText( Logger::TEAM,
                  __FILE__": Bhv_BasicOffensiveKick" );

    bool getBestPass = false;

    // Get a pointer to the world model.
    const WorldModel & wm = agent->world();
    // Get opponents sorted by distance from self.
    const PlayerPtrCont & opps = wm.opponentsFromSelf();
    // Nearest opponent to the player.
    const PlayerObject * nearest_opp
        = ( opps.empty()
            ? static_cast< PlayerObject * >( 0 )
            : opps.front() );
    // Distance to the nearest opponent.
    const double nearest_opp_dist = ( nearest_opp
                                      ? nearest_opp->distFromSelf()
                                      : 1000.0 );
    // X and Y coordinates of the nearest opponent in the map.
    const Vector2D nearest_opp_pos = ( nearest_opp
                                       ? nearest_opp->pos()
                                       : Vector2D( -1000.0, 0.0 ) );

    // X and Y coordinates of the point in the map where the ball will be passed.
    Vector2D pass_point;
    // Calculates the best pass route. (True if the pass route is found).
    if ( Body_Pass::get_best_pass( wm, &pass_point, NULL, NULL ) )
    {
        getBestPass = true;
        cv::Mat bestPass(extractFeaturesKick(agent, pass_point,1));

        // If the X coordinate of the pass point is greater than
        // the X coordinate of the player.
        if ( pass_point.x > wm.self().pos().x - 1.0 )
        {
            bool safety = true;
            // Checks if there is any opponent close to the pass point.
            const PlayerPtrCont::const_iterator opps_end = opps.end();
            for ( PlayerPtrCont::const_iterator it = opps.begin();
                  it != opps_end;
                  ++it )
            {
                if ( (*it)->pos().dist( pass_point ) < 4.0 )
                {
                    safety = false;
                }
            }

            // If its still safe to make the pass
            if ( safety )
            {
                // It will be a successful pass
                if (passTree.predict(bestPass)->value == 1){
                  dlog.addText( Logger::TEAM,
                                __FILE__": (execute) do best pass" );
                  agent->debugClient().addMessage( "OffKickPass(1)" );
                  // Execute a pass.
                  Body_Pass().execute( agent );
                  agent->setNeckAction( new Neck_TurnToLowConfTeammate() );
                  return true;
                }
                else {
                  if (Body_HoldBall().execute( agent )) {
                      //std::cout << "Hold " << agent->world().self().unum() << std::endl;
                      agent->setNeckAction( new Neck_ScanField() );
                      return true;
                  }
                }
            }
        }
        // If the distance to the nearest opponent is less than 7.0
        if ( nearest_opp_dist < 7.0 )
        {
            // It will be a successful pass
            if (passTree.predict(bestPass)->value == 1){
              // If the pass is executed.
              if ( Body_Pass().execute( agent ) )
              {
                  dlog.addText( Logger::TEAM,
                                __FILE__": (execute) do best pass" );
                  agent->debugClient().addMessage( "OffKickPass(2)" );
                  agent->setNeckAction( new Neck_TurnToLowConfTeammate() );
                  return true;
              }
            }
            else {
              if (Body_HoldBall().execute( agent )) {
                  //std::cout << "Hold " << agent->world().self().unum() << std::endl;
                  agent->setNeckAction( new Neck_ScanField() );
                  return true;
              }
            }
        }
    }

    // dribble to my body dir
    if ( nearest_opp_dist < 5.0
         && nearest_opp_dist > ( ServerParam::i().tackleDist()
                                 + ServerParam::i().defaultPlayerSpeedMax() * 1.5 )
         && wm.self().body().abs() < 70.0 )
    {
        const Vector2D body_dir_drib_target
            = wm.self().pos()
            + Vector2D::polar2vector(5.0, wm.self().body());

        int max_dir_count = 0;
        wm.dirRangeCount( wm.self().body(), 20.0, &max_dir_count, NULL, NULL );

        if ( body_dir_drib_target.x < ServerParam::i().pitchHalfLength() - 1.0
             && body_dir_drib_target.absY() < ServerParam::i().pitchHalfWidth() - 1.0
             && max_dir_count < 3
             )
        {
            // check opponents
            // 10m, +-30 degree
            const Sector2D sector( wm.self().pos(),
                                   0.5, 10.0,
                                   wm.self().body() - 30.0,
                                   wm.self().body() + 30.0 );

            cv::Mat dribbleSample1(extractFeaturesKick(agent, body_dir_drib_target, 0));

            // opponent check with goalie
            if (dribbleTree.predict(dribbleSample1)->value == 1){
              if ( ! wm.existOpponentIn( sector, 10, true ) )
              {
                  dlog.addText( Logger::TEAM,
                                __FILE__": (execute) dribble to my body dir" );
                  agent->debugClient().addMessage( "OffKickDrib(1)" );
                  Body_Dribble( body_dir_drib_target,
                                1.0,
                                ServerParam::i().maxDashPower(),
                                2
                                ).execute( agent );
                  agent->setNeckAction( new Neck_TurnToLowConfTeammate() );
                  return true;
              }
            }
            else {
                if (Body_HoldBall().execute( agent )) {
                    //std::cout << "Hold " << agent->world().self().unum() << std::endl;
                    agent->setNeckAction( new Neck_ScanField() );
                    return true;
                }
            }
        }
    }

    Vector2D drib_target( 50.0, wm.self().pos().absY() );
    if ( drib_target.y < 20.0 ) drib_target.y = 20.0;
    if ( drib_target.y > 29.0 ) drib_target.y = 27.0;
    if ( wm.self().pos().y < 0.0 ) drib_target.y *= -1.0;
    const AngleDeg drib_angle = ( drib_target - wm.self().pos() ).th();

    cv::Mat dribbleSample(extractFeaturesKick(agent, drib_target, 0));

    // opponent is behind of me
    if ( nearest_opp_pos.x < wm.self().pos().x + 1.0 )
    {
        // check opponents
        // 15m, +-30 degree
        const Sector2D sector( wm.self().pos(),
                               0.5, 15.0,
                               drib_angle - 30.0,
                               drib_angle + 30.0 );
        // opponent check with goalie
        if ( ! wm.existOpponentIn( sector, 10, true ) )
        {
            const int max_dash_step
                = wm.self().playerType()
                .cyclesToReachDistance( wm.self().pos().dist( drib_target ) );
            if ( wm.self().pos().x > 35.0 )
            {
                drib_target.y *= ( 10.0 / drib_target.absY() );
            }

            // It will be a successful dribble
            if (dribbleTree.predict(dribbleSample)->value == 1){
              dlog.addText( Logger::TEAM,
                            __FILE__": (execute) fast dribble to (%.1f, %.1f) max_step=%d",
                            drib_target.x, drib_target.y,
                            max_dash_step );
              agent->debugClient().addMessage( "OffKickDrib(2)" );
              Body_Dribble( drib_target,
                            1.0,
                            ServerParam::i().maxDashPower(),
                            std::min( 5, max_dash_step )
                            ).execute( agent );
            }
            else {
              if (Body_HoldBall().execute( agent )) {
                  //std::cout << "Hold " << agent->world().self().unum() << std::endl;
                  agent->setNeckAction( new Neck_ScanField() );
                  return true;
              }
            }
        }
        else
        {
            // It will be a successful dribble
          if (dribbleTree.predict(dribbleSample)->value == 1){
              dlog.addText( Logger::TEAM,
                            __FILE__": (execute) slow dribble to (%.1f, %.1f)",
                            drib_target.x, drib_target.y );
              agent->debugClient().addMessage( "OffKickDrib(3)" );
              Body_Dribble( drib_target,
                            1.0,
                            ServerParam::i().maxDashPower(),
                            2
                            ).execute( agent );
          }
          else {
            if (Body_HoldBall().execute( agent )) {
                //std::cout << "Hold " << agent->world().self().unum() << std::endl;
                agent->setNeckAction( new Neck_ScanField() );
                return true;
            }
          }

        }
        agent->setNeckAction( new Neck_TurnToLowConfTeammate() );
        return true;
    }

    // opp is far from me
    if ( nearest_opp_dist > 5.0 )
    {
        // It will be a successful dribble
      if (dribbleTree.predict(dribbleSample)->value == 1){
          dlog.addText( Logger::TEAM,
                        __FILE__": opp far. dribble(%.1f, %.1f)",
                        drib_target.x, drib_target.y );
          agent->debugClient().addMessage( "OffKickDrib(4)" );
          Body_Dribble( drib_target,
                        1.0,
                        ServerParam::i().maxDashPower() * 0.4,
                        1
                        ).execute( agent );
          agent->setNeckAction( new Neck_TurnToLowConfTeammate() );
          return true;
      }
      else {
        if (Body_HoldBall().execute( agent )) {
            //std::cout << "Hold " << agent->world().self().unum() << std::endl;
            agent->setNeckAction( new Neck_ScanField() );
            return true;
        }
      }
    }

    // opp is near

    if (getBestPass){
      cv::Mat bestPass(extractFeaturesKick(agent, pass_point, 0));
      // It will be a successful pass
      if (passTree.predict(bestPass)->value == 1){
      // can pass
        if ( Body_Pass().execute( agent ) )
        {
            dlog.addText( Logger::TEAM,
                          __FILE__": (execute) pass",
                          __LINE__ );
            agent->debugClient().addMessage( "OffKickPass(3)" );
            agent->setNeckAction( new Neck_TurnToLowConfTeammate() );
            return true;
        }
      }
      else {
        if (Body_HoldBall().execute( agent )) {
            //std::cout << "Hold " << agent->world().self().unum() << std::endl;
            agent->setNeckAction( new Neck_ScanField() );
            return true;
        }
      }
    }

    // opp is far from me
    if ( nearest_opp_dist > 3.0 )
    { 
        // It will be a successful dribble
      if (dribbleTree.predict(dribbleSample)->value == 1){
          dlog.addText( Logger::TEAM,
                        __FILE__": (execute) opp far. dribble(%f, %f)",
                        drib_target.x, drib_target.y );
          agent->debugClient().addMessage( "OffKickDrib(5)" );
          Body_Dribble( drib_target,
                        1.0,
                        ServerParam::i().maxDashPower() * 0.2,
                        1
                        ).execute( agent );
          agent->setNeckAction( new Neck_TurnToLowConfTeammate() );
          return true;
      }
      else {
        if (Body_HoldBall().execute( agent )) {
            //std::cout << "Hold " << agent->world().self().unum() << std::endl;
            agent->setNeckAction( new Neck_ScanField() );
            return true;
        }
      }
    }

    // Hold the ball if the nearest opponent is between 2,5 and 3.0 distance.
    if ( nearest_opp_dist > 2.5 )
    {
        dlog.addText( Logger::TEAM,
                      __FILE__": hold" );
        agent->debugClient().addMessage( "OffKickHold" );
        Body_HoldBall().execute( agent );
        agent->setNeckAction( new Neck_TurnToLowConfTeammate() );
        return true;
    }

    {
        dlog.addText( Logger::TEAM,
                      __FILE__": clear" );
        agent->debugClient().addMessage( "OffKickAdvance" );
        Body_AdvanceBall().execute( agent );
        agent->setNeckAction( new Neck_ScanField() );
    }

    return true;

}
