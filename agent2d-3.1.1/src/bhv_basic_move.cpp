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

#include "bhv_basic_move.h"

#include "strategy.h"

#include "bhv_basic_tackle.h"

#include <rcsc/action/basic_actions.h>
#include <rcsc/action/body_go_to_point.h>
#include <rcsc/action/body_intercept.h>
#include <rcsc/action/neck_turn_to_ball_or_scan.h>
#include <rcsc/action/neck_turn_to_low_conf_teammate.h>

#include <rcsc/player/player_agent.h>
#include <rcsc/player/debug_client.h>
#include <rcsc/player/intercept_table.h>

#include <rcsc/common/logger.h>
#include <rcsc/common/server_param.h>

#include "neck_offensive_intercept_neck.h"

using namespace rcsc;

/*-------------------------------------------------------------------*/
/*!

 */
/* 
/    Parameters : 
/       PlayerAgent * agent : Player agent that'll execute this code.
/    Description : 
/      The agent will tackle, intercept or move to a target location 
/      depending on certain conditions.
*/
bool
Bhv_BasicMove::execute( PlayerAgent * agent )
{
    dlog.addText( Logger::TEAM,
                  __FILE__": Bhv_BasicMove" );

    //-----------------------------------------------
    // tackle
    if ( Bhv_BasicTackle( 0.8, 80.0 ).execute( agent ) )
    {
        return true;
    }

    // Get a pointer to the world model.
    const WorldModel & wm = agent->world();
    /*--------------------------------------------------------*/
    // chase ball
    // Get minimal ball gettable cycle for self without stamina exhaust.
    const int self_min = wm.interceptTable()->selfReachCycle();
    // Get minimal ball gettable cycle for teammate.
    const int mate_min = wm.interceptTable()->teammateReachCycle();
    // Get minimal ball gettable cycle for opponent.
    const int opp_min = wm.interceptTable()->opponentReachCycle();

    // Checks if there is another teammate that can kick the ball.
    // If there isn't one and the minimal ball gettable cycle for this agent
    // is less than 3 or lower than the teammates and the oponents, then execute
    // an intercept.
    if ( ! wm.existKickableTeammate()
         && ( self_min <= 3
              || ( self_min <= mate_min
                   && self_min < opp_min + 3 )
              )
         )
    {
        dlog.addText( Logger::TEAM,
                      __FILE__": intercept" );
        // Execute the intercept. (Ball chasing action including smart planning).
        Body_Intercept().execute( agent );
        // Reserve turn neck action. 
        // (See neck_offensive_intercept_neck.cpp)
        agent->setNeckAction( new Neck_OffensiveInterceptNeck() );

        return true;
    }

    // X and Y coordinates of the position where the player will move.
    const Vector2D target_point = Strategy::i().getPosition( wm.self().unum() );
    // Maximum dash power to use when moving.
    const double dash_power = Strategy::get_normal_dash_power( wm );

    // Calculates the distance from the ball to the player.
    double dist_thr = wm.ball().distFromSelf() * 0.1;
    if ( dist_thr < 1.0 ) dist_thr = 1.0;

    dlog.addText( Logger::TEAM,
                  __FILE__": Bhv_BasicMove target=(%.1f %.1f) dist_thr=%.2f",
                  target_point.x, target_point.y,
                  dist_thr );

    agent->debugClient().addMessage( "BasicMove%.0f", dash_power );
    agent->debugClient().setTarget( target_point );
    agent->debugClient().addCircle( target_point, dist_thr );

    // If its not possible to move to the target point, then turn the player's
    // body to face the ball.
    if ( ! Body_GoToPoint( target_point, dist_thr, dash_power
                           ).execute( agent ) )
    {
        Body_TurnToBall().execute( agent );
    }

    // If there is an oponent that can kick the ball and the distance from the ball
    // to the player is < 18.0, turn the player's neck to face the ball.
    // Else turn to the ball or scan the field. Check the ball or scan field with neck evenly. 
    // if next ball position is NOT over view width ,scan field else face to ball
    if ( wm.existKickableOpponent()
         && wm.ball().distFromSelf() < 18.0 )
    {
        agent->setNeckAction( new Neck_TurnToBall() );
    }
    else
    {
        agent->setNeckAction( new Neck_TurnToBallOrScan() );
    }

    return true;
}
