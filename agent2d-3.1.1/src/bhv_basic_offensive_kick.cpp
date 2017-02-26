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


double distFromLine(Vector2D p0, Vector2D p1, Vector2D p2){
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
extractFeaturesKick(PlayerAgent* agent, Vector2D targetPoint){
    PlayerCont allOpps;
    PlayerCont allTeammts;
    PlayerCont::iterator iter;
    Mat features(1,24);

    // Ball position
    Vector2D ballPos = agent->world().ball().pos();

    features.push_back(ballPos.x/5);
    features.push_back(ballPos.y/5);

    // Calculating Teammates.
    allTeammts = agent->world().teammates();
    for (iter = allTeammts.begin(); iter != allTeammts.end(); iter++) {
      features.push_back(distFromLine(ballPos, targetPoint, iter->pos()));
        
    }

    // Calculating Opponents.
    allOpps = agent->world().opponents();
    for (iter = allOpps.begin(); iter != allOpps.end(); iter++) {
      features.push_back(distFromLine(ballPos, targetPoint, iter->pos()));
    }


    //ballPos.assign(ballPos.x/5, ballPos.y/5);

    //Mat features = (Mat_<float>(1,10) << ballPos.x, ballPos.y, distT1, distT2, distT3, distO1, distO2, distO3, distO4);
    
    return features;
}


/*-------------------------------------------------------------------*/
/*!

 */
bool
Bhv_BasicOffensiveKick::execute( PlayerAgent * agent )
{
   /* CvDTree dribbleTree;
    dribbleTree.load("trainedTrees/Genius/dribbleTree.yml");

    CvDTree passTree;
    passTree.load("trainedTrees/Genius/passTree.yml");*/

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
        //cv::Mat bestPass(extractFeaturesKick(agent, pass_point));

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
                //if (passTree.predict(bestPass)->value >= 0.5){
                  dlog.addText( Logger::TEAM,
                                __FILE__": (execute) do best pass" );
                  agent->debugClient().addMessage( "OffKickPass(1)" );
                  // Execute a pass.
                  Body_Pass().execute( agent );
                  agent->setNeckAction( new Neck_TurnToLowConfTeammate() );
                  return true;
                //}
            }
        }
        // If the distance to the nearest opponent is less than 7.0
        if ( nearest_opp_dist < 7.0 )
        {
            // It will be a successful pass
            //if (passTree.predict(bestPass)->value >= 0.5){
              // If the pass is executed.
              if ( Body_Pass().execute( agent ) )
              {
                  dlog.addText( Logger::TEAM,
                                __FILE__": (execute) do best pass" );
                  agent->debugClient().addMessage( "OffKickPass(2)" );
                  agent->setNeckAction( new Neck_TurnToLowConfTeammate() );
                  return true;
              }
            //}
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

            //cv::Mat dribbleSample1(extractFeaturesKick(agent, body_dir_drib_target));

            // opponent check with goalie
          //if (dribbleTree.predict(dribbleSample1)->value >= 0.5){
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
          //}
        }
    }

    Vector2D drib_target( 50.0, wm.self().pos().absY() );
    if ( drib_target.y < 20.0 ) drib_target.y = 20.0;
    if ( drib_target.y > 29.0 ) drib_target.y = 27.0;
    if ( wm.self().pos().y < 0.0 ) drib_target.y *= -1.0;
    const AngleDeg drib_angle = ( drib_target - wm.self().pos() ).th();

    //cv::Mat dribbleSample(extractFeaturesKick(agent, drib_target));

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
            //if (dribbleTree.predict(dribbleSample)->value >= 0.5){
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
            //}
        }
        else
        {
            // It will be a successful dribble
            //if (dribbleTree.predict(dribbleSample)->value >= 0.5){
              dlog.addText( Logger::TEAM,
                            __FILE__": (execute) slow dribble to (%.1f, %.1f)",
                            drib_target.x, drib_target.y );
              agent->debugClient().addMessage( "OffKickDrib(3)" );
              Body_Dribble( drib_target,
                            1.0,
                            ServerParam::i().maxDashPower(),
                            2
                            ).execute( agent );
            //}

        }
        agent->setNeckAction( new Neck_TurnToLowConfTeammate() );
        return true;
    }

    // opp is far from me
    if ( nearest_opp_dist > 5.0 )
    {
        // It will be a successful dribble
        //if (dribbleTree.predict(dribbleSample)->value >= 0.5){
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
        //}
    }

    // opp is near

    //if (getBestPass){
      cv::Mat bestPass(extractFeaturesKick(agent, pass_point));
      // It will be a successful pass
      //if (passTree.predict(bestPass)->value >= 0.5){
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
      //}
    //}

    // opp is far from me
    if ( nearest_opp_dist > 3.0 )
    { 
        // It will be a successful dribble
        //if (dribbleTree.predict(dribbleSample)->value >= 0.5){
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
        //}
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
