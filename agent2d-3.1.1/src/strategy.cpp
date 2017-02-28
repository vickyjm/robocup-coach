// -*-c++-*-

/*!
  \file strategy.cpp
  \brief team strategh Source File
*/

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

#include "strategy.h"

#include "soccer_role.h"


#ifndef USE_GENERIC_FACTORY
#include "role_sample.h"

#include "role_center_back.h"
#include "role_center_forward.h"
#include "role_defensive_half.h"
#include "role_goalie.h"
#include "role_offensive_half.h"
#include "role_side_back.h"
#include "role_side_forward.h"
#include "role_side_half.h"

#include "role_keepaway_keeper.h"
#include "role_keepaway_taker.h"

#include <rcsc/formation/formation_static.h>
#include <rcsc/formation/formation_dt.h>
#endif

#include <rcsc/player/intercept_table.h>
#include <rcsc/player/world_model.h>

#include <rcsc/common/logger.h>
#include <rcsc/common/server_param.h>
#include <rcsc/param/cmd_line_parser.h>
#include <rcsc/param/param_map.h>
#include <rcsc/game_mode.h>

#include <set>
#include <fstream>
#include <iostream>
#include <cstdio>
#include <string>

int ourFormationChange = 0;
int ourFormationMode = 0;
namespace patch
{
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }
}

void writeToFormChangeFile(int unum,int form,int mode) {
    std::ofstream formationChangeFile;
    formationChangeFile.open("/home/jemd/Documents/USB/Tesis/robocup-coach/formationChanges/changes.txt",std::ios::app);
    if (unum == 8) {
        if (formationChangeFile.is_open()) {
            if (ourFormationChange != form) {
                ourFormationChange = form;
                ourFormationMode = mode;
            }
            else {
                formationChangeFile.clear();
                formationChangeFile.close();  
                return;
            }
            if (form == -1) // our goal
                formationChangeFile << "OurGoal";
            if (form == -2) // their goal
                formationChangeFile << "TheirGoal";
            else
                formationChangeFile << form;
            formationChangeFile << " ";
            if (mode == 0)
                formationChangeFile << "N";
            if (mode == 1)
                formationChangeFile << "D";
            if (mode == 2)
                formationChangeFile << "O";
            formationChangeFile << std::endl;
            formationChangeFile.flush();
        }
    }
    formationChangeFile.clear();
    formationChangeFile.close();
    return;
}

using namespace rcsc;

const std::string Strategy::BEFORE_KICK_OFF_CONF = "before-kick-off.conf";
const std::string Strategy::NORMAL_FORMATION_CONF = "normal-formation.conf";
const std::string Strategy::DEFENSE_FORMATION_CONF = "defense-formation.conf";
const std::string Strategy::OFFENSE_FORMATION_CONF = "offense-formation.conf";
const std::string Strategy::GOAL_KICK_OPP_FORMATION_CONF = "goal-kick-opp.conf";
const std::string Strategy::GOAL_KICK_OUR_FORMATION_CONF = "goal-kick-our.conf";
const std::string Strategy::GOALIE_CATCH_OPP_FORMATION_CONF = "goalie-catch-opp.conf";
const std::string Strategy::GOALIE_CATCH_OUR_FORMATION_CONF = "goalie-catch-our.conf";
const std::string Strategy::KICKIN_OUR_FORMATION_CONF = "kickin-our-formation.conf";
const std::string Strategy::SETPLAY_OPP_FORMATION_CONF = "setplay-opp-formation.conf";
const std::string Strategy::SETPLAY_OUR_FORMATION_CONF = "setplay-our-formation.conf";
const std::string Strategy::INDIRECT_FREEKICK_OPP_FORMATION_CONF = "indirect-freekick-opp-formation.conf";
const std::string Strategy::INDIRECT_FREEKICK_OUR_FORMATION_CONF = "indirect-freekick-our-formation.conf";

const std::string Strategy::DEFENSE_4231_CONF = "defense-4231.conf";
const std::string Strategy::DEFENSE_442_CONF = "defense-442.conf";
const std::string Strategy::DEFENSE_433_CONF = "defense-433.conf";
const std::string Strategy::OFFENSE_4231_CONF = "offense-4231.conf";
const std::string Strategy::OFFENSE_442_CONF = "offense-442.conf";
const std::string Strategy::OFFENSE_433_CONF = "offense-433.conf";

// Our formations
const std::string Strategy::FORM_352_CONF = "3-5-2.conf";
const std::string Strategy::FORM_352_OFF_CONF = "3-5-2-offense.conf";
const std::string Strategy::FORM_352_DEF_CONF = "3-5-2-defense.conf";
const std::string Strategy::FORM_4231_CONF = "4-2-3-1.conf";
const std::string Strategy::FORM_4231_OFF_CONF = "4-2-3-1-offense.conf";
const std::string Strategy::FORM_4231_DEF_CONF = "4-2-3-1-defense.conf";
const std::string Strategy::FORM_433_CONF = "4-3-3.conf";
const std::string Strategy::FORM_433_OFF_CONF = "4-3-3-offense.conf";
const std::string Strategy::FORM_433_DEF_CONF = "4-3-3-defense.conf";
const std::string Strategy::FORM_442_CONF = "4-4-2.conf";
const std::string Strategy::FORM_442_OFF_CONF = "4-4-2-offense.conf";
const std::string Strategy::FORM_442_DEF_CONF = "4-4-2-defense.conf";
const std::string Strategy::FORM_451_CONF = "4-5-1.conf";
const std::string Strategy::FORM_451_OFF_CONF = "4-5-1-offense.conf";
const std::string Strategy::FORM_451_DEF_CONF = "4-5-1-defense.conf";
const std::string Strategy::FORM_532_CONF = "5-3-2.conf";

const std::string Strategy::FORM_BK_352_CONF = "BK-3-5-2.conf";
const std::string Strategy::FORM_BK_4231_CONF = "BK-4-2-3-1.conf";
const std::string Strategy::FORM_BK_433_CONF = "BK-4-3-3.conf";
const std::string Strategy::FORM_BK_442_CONF = "BK-4-4-2.conf";
const std::string Strategy::FORM_BK_451_CONF = "BK-4-5-1.conf";
const std::string Strategy::FORM_BK_532_CONF = "BK-5-3-2.conf";

// Genius formations
const std::string Strategy::DEF_FORM_433_CONF = "F-433-defense-formation.conf";
const std::string Strategy::OFF_FORM_433_CONF = "F-433-offense-formation.conf";
const std::string Strategy::NORM_FORM_433_CONF = "F-433-normal-formation.conf";
const std::string Strategy::DEF_FORM_442_CONF = "F-442-defense-formation.conf";
const std::string Strategy::OFF_FORM_442_CONF = "F-442-offense-formation.conf";
const std::string Strategy::NORM_FORM_442_CONF = "F-442-normal-formation.conf";
const std::string Strategy::DEF_FORM_541_CONF = "F-541-defense-formation.conf";
const std::string Strategy::OFF_FORM_541_CONF = "F-541-offense-formation.conf";
const std::string Strategy::NORM_FORM_541_CONF = "F-541-normal-formation.conf";
const std::string Strategy::DEF_FORM_4123_CONF = "F-4123-defense-formation.conf";
const std::string Strategy::OFF_FORM_4123_CONF = "F-4123-offense-formation.conf";
const std::string Strategy::NORM_FORM_4123_CONF = "F-4123-normal-formation.conf";
const std::string Strategy::DEF_FORM_4213_CONF = "F-4213-defense-formation.conf";
const std::string Strategy::OFF_FORM_4213_CONF = "F-4213-offense-formation.conf";
const std::string Strategy::NORM_FORM_4213_CONF = "F-4213-normal-formation.conf";

// Other Genius Formations
const std::string Strategy::BK_FORM_433_CONF = "F-433-before-kick-off.conf";
const std::string Strategy::GOAL_KICK_OPP_FORM_433_CONF = "F-433-goal-kick-opp.conf";
const std::string Strategy::GOAL_KICK_OUR_FORM_433_CONF = "F-433-goal-kick-our.conf";
const std::string Strategy::GOALIE_CATCH_OPP_FORM_433_CONF = "F-433-goalie-catch-opp.conf";
const std::string Strategy::GOALIE_CATCH_OUR_FORM_433_CONF = "F-433-goalie-catch-our.conf";
const std::string Strategy::INDIRECT_FREEKICK_OPP_FORM_433_CONF = "F-433-indirect-freekick-opp-formation.conf";
const std::string Strategy::INDIRECT_FREEKICK_OUR_FORM_433_CONF = "F-433-indirect-freekick-our-formation.conf";
const std::string Strategy::KICKIN_OUR_FORM_433_CONF = "F-433-kickin-our-formation.conf";
const std::string Strategy::SETPLAY_OPP_FORM_433_CONF = "F-433-setplay-opp-formation.conf";
const std::string Strategy::SETPLAY_OUR_FORM_433_CONF = "F-433-setplay-our-formation.conf";

const std::string Strategy::BK_FORM_442_CONF = "F-442-before-kick-off.conf";
const std::string Strategy::GOAL_KICK_OPP_FORM_442_CONF = "F-442-goal-kick-opp.conf";
const std::string Strategy::GOAL_KICK_OUR_FORM_442_CONF = "F-442-goal-kick-our.conf";
const std::string Strategy::GOALIE_CATCH_OPP_FORM_442_CONF = "F-442-goalie-catch-opp.conf";
const std::string Strategy::GOALIE_CATCH_OUR_FORM_442_CONF = "F-442-goalie-catch-our.conf";
const std::string Strategy::INDIRECT_FREEKICK_OPP_FORM_442_CONF = "F-442-indirect-freekick-opp-formation.conf";
const std::string Strategy::INDIRECT_FREEKICK_OUR_FORM_442_CONF = "F-442-indirect-freekick-our-formation.conf";
const std::string Strategy::KICKIN_OUR_FORM_442_CONF = "F-442-kickin-our-formation.conf";
const std::string Strategy::SETPLAY_OPP_FORM_442_CONF = "F-442-setplay-opp-formation.conf";
const std::string Strategy::SETPLAY_OUR_FORM_442_CONF = "F-442-setplay-our-formation.conf";

const std::string Strategy::BK_FORM_541_CONF = "F-541-before-kick-off.conf";
const std::string Strategy::GOAL_KICK_OPP_FORM_541_CONF = "F-541-goal-kick-opp.conf";
const std::string Strategy::GOAL_KICK_OUR_FORM_541_CONF = "F-541-goal-kick-our.conf";
const std::string Strategy::GOALIE_CATCH_OPP_FORM_541_CONF = "F-541-goalie-catch-opp.conf";
const std::string Strategy::GOALIE_CATCH_OUR_FORM_541_CONF = "F-541-goalie-catch-our.conf";
const std::string Strategy::INDIRECT_FREEKICK_OPP_FORM_541_CONF = "F-541-indirect-freekick-opp-formation.conf";
const std::string Strategy::INDIRECT_FREEKICK_OUR_FORM_541_CONF = "F-541-indirect-freekick-our-formation.conf";
const std::string Strategy::KICKIN_OUR_FORM_541_CONF = "F-541-kickin-our-formation.conf";
const std::string Strategy::SETPLAY_OPP_FORM_541_CONF = "F-541-setplay-opp-formation.conf";
const std::string Strategy::SETPLAY_OUR_FORM_541_CONF = "F-541-setplay-our-formation.conf";

const std::string Strategy::BK_FORM_4123_CONF = "F-4123-before-kick-off.conf";
const std::string Strategy::GOAL_KICK_OPP_FORM_4123_CONF = "F-4123-goal-kick-opp.conf";
const std::string Strategy::GOAL_KICK_OUR_FORM_4123_CONF = "F-4123-goal-kick-our.conf";
const std::string Strategy::GOALIE_CATCH_OPP_FORM_4123_CONF = "F-4123-goalie-catch-opp.conf";
const std::string Strategy::GOALIE_CATCH_OUR_FORM_4123_CONF = "F-4123-goalie-catch-our.conf";
const std::string Strategy::INDIRECT_FREEKICK_OPP_FORM_4123_CONF = "F-4123-indirect-freekick-opp-formation.conf";
const std::string Strategy::INDIRECT_FREEKICK_OUR_FORM_4123_CONF = "F-4123-indirect-freekick-our-formation.conf";
const std::string Strategy::KICKIN_OUR_FORM_4123_CONF = "F-4123-kickin-our-formation.conf";
const std::string Strategy::SETPLAY_OPP_FORM_4123_CONF = "F-4123-setplay-opp-formation.conf";
const std::string Strategy::SETPLAY_OUR_FORM_4123_CONF = "F-4123-setplay-our-formation.conf";

const std::string Strategy::BK_FORM_4213_CONF = "F-4213-before-kick-off.conf";
const std::string Strategy::GOAL_KICK_OPP_FORM_4213_CONF = "F-4213-goal-kick-opp.conf";
const std::string Strategy::GOAL_KICK_OUR_FORM_4213_CONF = "F-4213-goal-kick-our.conf";
const std::string Strategy::GOALIE_CATCH_OPP_FORM_4213_CONF = "F-4213-goalie-catch-opp.conf";
const std::string Strategy::GOALIE_CATCH_OUR_FORM_4213_CONF = "F-4213-goalie-catch-our.conf";
const std::string Strategy::INDIRECT_FREEKICK_OPP_FORM_4213_CONF = "F-4213-indirect-freekick-opp-formation.conf";
const std::string Strategy::INDIRECT_FREEKICK_OUR_FORM_4213_CONF = "F-4213-indirect-freekick-our-formation.conf";
const std::string Strategy::KICKIN_OUR_FORM_4213_CONF = "F-4213-kickin-our-formation.conf";
const std::string Strategy::SETPLAY_OPP_FORM_4213_CONF = "F-4213-setplay-opp-formation.conf";
const std::string Strategy::SETPLAY_OUR_FORM_4213_CONF = "F-4213-setplay-our-formation.conf";

// Formaciones nuevas
const std::string Strategy::DEF_FORM_532_CONF = "F-532-defense-formation.conf";
const std::string Strategy::OFF_FORM_532_CONF = "F-532-offense-formation.conf";
const std::string Strategy::NORM_FORM_532_CONF = "F-532-normal-formation.conf";
const std::string Strategy::BK_FORM_532_CONF = "F-532-before-kick-off.conf";


/*-------------------------------------------------------------------*/
/*!

 */
Strategy::Strategy()
    : M_goalie_unum( Unum_Unknown ),
      M_current_situation( Normal_Situation ),
      M_role_number( 11, 0 ),
      M_position_types( 11, Position_Center ),
      M_positions( 11 )
{
#ifndef USE_GENERIC_FACTORY
    //
    // roles
    //

    M_role_factory[RoleSample::name()] = &RoleSample::create;

    M_role_factory[RoleGoalie::name()] = &RoleGoalie::create;
    M_role_factory[RoleCenterBack::name()] = &RoleCenterBack::create;
    M_role_factory[RoleSideBack::name()] = &RoleSideBack::create;
    M_role_factory[RoleDefensiveHalf::name()] = &RoleDefensiveHalf::create;
    M_role_factory[RoleOffensiveHalf::name()] = &RoleOffensiveHalf::create;
    M_role_factory[RoleSideHalf::name()] = &RoleSideHalf::create;
    M_role_factory[RoleSideForward::name()] = &RoleSideForward::create;
    M_role_factory[RoleCenterForward::name()] = &RoleCenterForward::create;

    // keepaway
    M_role_factory[RoleKeepawayKeeper::name()] = &RoleKeepawayKeeper::create;
    M_role_factory[RoleKeepawayTaker::name()] = &RoleKeepawayTaker::create;

    //
    // formations
    //

    M_formation_factory[FormationStatic::name()] = &FormationStatic::create;
    M_formation_factory[FormationDT::name()] = &FormationDT::create;
#endif

    for ( size_t i = 0; i < M_role_number.size(); ++i )
    {
        M_role_number[i] = i + 1;
    }
}

/*-------------------------------------------------------------------*/
/*!

 */
Strategy &
Strategy::instance()
{
    static Strategy s_instance;
    return s_instance;
}

/*-------------------------------------------------------------------*/
/*!

 */
bool
Strategy::init( CmdLineParser & cmd_parser )
{
    ParamMap param_map( "HELIOS_base options" );

    // std::string fconf;
    //param_map.add()
    //    ( "fconf", "", &fconf, "another formation file." );

    //
    //
    //

    if ( cmd_parser.count( "help" ) > 0 )
    {
        param_map.printHelp( std::cout );
        return false;
    }

    //
    //
    //

    cmd_parser.parse( param_map );

    return true;
}

/*-------------------------------------------------------------------*/
/*!

 */
bool
Strategy::read( const std::string & formation_dir )
{
    static bool s_initialized = false;

    if ( s_initialized )
    {
        std::cerr << __FILE__ << ' ' << __LINE__ << ": already initialized."
                  << std::endl;
        return false;
    }

    std::string configpath = formation_dir;
    if ( ! configpath.empty()
         && configpath[ configpath.length() - 1 ] != '/' )
    {
        configpath += '/';
    }


    // Testing formation stuff
    M_offense_442_formation = readFormation( configpath + OFFENSE_442_CONF);
    if (! M_offense_442_formation )
    {
        std::cerr << "Failed to read offense-442 formation" << std::endl;
        return false;
    }

    // Our formations
    M_form_352_formation = readFormation( configpath + FORM_352_CONF);
    if (! M_form_352_formation)
    {
        std::cerr << "Failed to read 352 formation" << std::endl;
        return false;
    }

    M_form_352_off_formation = readFormation( configpath + FORM_352_OFF_CONF);
    if (! M_form_352_off_formation)
    {
        std::cerr << "Failed to read 352_off formation" << std::endl;
        return false;
    }

    M_form_352_def_formation = readFormation( configpath + FORM_352_DEF_CONF);
    if (! M_form_352_def_formation)
    {
        std::cerr << "Failed to read 352_def formation" << std::endl;
        return false;
    }


    M_form_4231_formation = readFormation( configpath + FORM_4231_CONF);
    if (! M_form_4231_formation)
    {
        std::cerr << "Failed to read 4231 formation" << std::endl;
        return false;
    }

    M_form_4231_off_formation = readFormation( configpath + FORM_4231_OFF_CONF);
    if (! M_form_4231_off_formation)
    {
        std::cerr << "Failed to read 4231_off formation" << std::endl;
        return false;
    }

    M_form_4231_def_formation = readFormation( configpath + FORM_4231_DEF_CONF);
    if (! M_form_4231_def_formation)
    {
        std::cerr << "Failed to read 4231_def formation" << std::endl;
        return false;
    }


    M_form_433_formation = readFormation( configpath + FORM_433_CONF);
    if (! M_form_433_formation)
    {
        std::cerr << "Failed to read 433 formation" << std::endl;
        return false;
    }

    M_form_433_off_formation = readFormation( configpath + FORM_433_OFF_CONF);
    if (! M_form_433_off_formation)
    {
        std::cerr << "Failed to read 433_off formation" << std::endl;
        return false;
    }

    M_form_433_def_formation = readFormation( configpath + FORM_433_DEF_CONF);
    if (! M_form_433_def_formation)
    {
        std::cerr << "Failed to read 433_def formation" << std::endl;
        return false;
    }

    M_form_442_formation = readFormation( configpath + FORM_442_CONF);
    if (! M_form_442_formation)
    {
        std::cerr << "Failed to read 442 formation" << std::endl;
        return false;
    }

    M_form_442_off_formation = readFormation( configpath + FORM_442_OFF_CONF);
    if (! M_form_442_off_formation)
    {
        std::cerr << "Failed to read 442_off formation" << std::endl;
        return false;
    }

    M_form_442_def_formation = readFormation( configpath + FORM_442_DEF_CONF);
    if (! M_form_442_def_formation)
    {
        std::cerr << "Failed to read 442_def formation" << std::endl;
        return false;
    }


    M_form_451_formation = readFormation( configpath + FORM_451_CONF);
    if (! M_form_451_formation)
    {
        std::cerr << "Failed to read 451 formation" << std::endl;
        return false;
    }

    M_form_451_off_formation = readFormation( configpath + FORM_451_OFF_CONF);
    if (! M_form_451_off_formation)
    {
        std::cerr << "Failed to read 451_off formation" << std::endl;
        return false;
    }

    M_form_451_def_formation = readFormation( configpath + FORM_451_DEF_CONF);
    if (! M_form_451_def_formation)
    {
        std::cerr << "Failed to read 451_def formation" << std::endl;
        return false;
    }


    M_form_532_formation = readFormation( configpath + FORM_532_CONF);
    if (! M_form_532_formation)
    {
        std::cerr << "Failed to read 532 formation" << std::endl;
        return false;
    }

    M_form_BK_352_formation = readFormation( configpath + FORM_BK_352_CONF);
    if (! M_form_BK_352_formation)
    {
        std::cerr << "Failed to read BK352 formation" << std::endl;
        return false;       
    }

    M_form_BK_4231_formation = readFormation( configpath + FORM_BK_4231_CONF);
    if (! M_form_BK_4231_formation)
    {
        std::cerr << "Failed to read BK4231 formation" << std::endl;
        return false;       
    }

    M_form_BK_433_formation = readFormation( configpath + FORM_BK_433_CONF);
    if (! M_form_BK_433_formation)
    {
        std::cerr << "Failed to read BK433 formation" << std::endl;
        return false;       
    }

    M_form_BK_442_formation = readFormation( configpath + FORM_BK_442_CONF);
    if (! M_form_BK_442_formation)
    {
        std::cerr << "Failed to read BK442 formation" << std::endl;
        return false;       
    }

    M_form_BK_451_formation = readFormation( configpath + FORM_BK_451_CONF);
    if (! M_form_BK_451_formation)
    {
        std::cerr << "Failed to read BK451 formation" << std::endl;
        return false;       
    }

    M_form_BK_532_formation = readFormation( configpath + FORM_BK_532_CONF);
    if (! M_form_BK_532_formation)
    {
        std::cerr << "Failed to read BK532 formation" << std::endl;
        return false;       
    }

    // End of our formations

    // Genius formations

    M_form_def_433_formation = readFormation( configpath + DEF_FORM_433_CONF);
    if (! M_form_def_433_formation)
    {
        std::cerr << "Failed to read M_form_def_433 formation" << std::endl;
        return false;       
    }

    M_form_off_433_formation = readFormation( configpath + OFF_FORM_433_CONF);
    if (! M_form_off_433_formation)
    {
        std::cerr << "Failed to read M_form_off_433 formation" << std::endl;
        return false;       
    }  

    M_form_norm_433_formation = readFormation( configpath + NORM_FORM_433_CONF);
    if (! M_form_norm_433_formation)
    {
        std::cerr << "Failed to read M_form_norm_433 formation" << std::endl;
        return false;       
    }

    M_form_def_442_formation = readFormation( configpath + DEF_FORM_442_CONF);
    if (! M_form_def_442_formation)
    {
        std::cerr << "Failed to read M_form_def_442 formation" << std::endl;
        return false;       
    }

    M_form_off_442_formation = readFormation( configpath + OFF_FORM_442_CONF);
    if (! M_form_off_442_formation)
    {
        std::cerr << "Failed to read M_form_off_442 formation" << std::endl;
        return false;       
    }

    M_form_norm_442_formation = readFormation( configpath + NORM_FORM_442_CONF);
    if (! M_form_norm_442_formation)
    {
        std::cerr << "Failed to read M_form_norm_442 formation" << std::endl;
        return false;       
    }

    M_form_def_541_formation = readFormation( configpath + DEF_FORM_541_CONF);
    if (! M_form_def_541_formation)
    {
        std::cerr << "Failed to read M_form_def_541 formation" << std::endl;
        return false;       
    }

    M_form_off_541_formation = readFormation( configpath + OFF_FORM_541_CONF);
    if (! M_form_off_541_formation)
    {
        std::cerr << "Failed to read M_form_off_541 formation" << std::endl;
        return false;       
    }

    M_form_norm_541_formation = readFormation( configpath + NORM_FORM_541_CONF);
    if (! M_form_norm_541_formation)
    {
        std::cerr << "Failed to read M_form_norm_541 formation" << std::endl;
        return false;       
    }

    M_form_def_4123_formation = readFormation( configpath + DEF_FORM_4123_CONF);
    if (! M_form_def_4123_formation)
    {
        std::cerr << "Failed to read M_form_def_4123 formation" << std::endl;
        return false;       
    }

    M_form_off_4123_formation = readFormation( configpath + OFF_FORM_4123_CONF);
    if (! M_form_off_4123_formation)
    {
        std::cerr << "Failed to read M_form_off_4123 formation" << std::endl;
        return false;       
    }

    M_form_norm_4123_formation = readFormation( configpath + NORM_FORM_4123_CONF);
    if (! M_form_norm_4123_formation)
    {
        std::cerr << "Failed to read M_form_norm_4123 formation" << std::endl;
        return false;       
    }

    M_form_def_4213_formation = readFormation( configpath + DEF_FORM_4213_CONF);
    if (!  M_form_def_4213_formation)
    {
        std::cerr << "Failed to read  M_form_def_4213 formation" << std::endl;
        return false;       
    }

    M_form_off_4213_formation = readFormation( configpath + OFF_FORM_4213_CONF);
    if (!  M_form_off_4213_formation)
    {
        std::cerr << "Failed to read  M_form_off_4213 formation" << std::endl;
        return false;       
    }

    M_form_norm_4213_formation = readFormation( configpath + NORM_FORM_4213_CONF);
    if (! M_form_norm_4213_formation)
    {
        std::cerr << "Failed to read  M_form_norm_4213 formation" << std::endl;
        return false;       
    }

    // Other Genius formations
    M_form_BKO_433_formation = readFormation( configpath + BK_FORM_433_CONF);
    if (! M_form_BKO_433_formation)
    {
        std::cerr << "Failed to read  M_form_BKO_433 formation" << std::endl;
        return false;       
    }

    M_form_goal_kick_opp_433_formation = readFormation( configpath + GOAL_KICK_OPP_FORM_433_CONF);
    if (! M_form_goal_kick_opp_433_formation)
    {
        std::cerr << "Failed to read  M_form_goal_kick_opp_433 formation" << std::endl;
        return false;       
    }

    M_form_goal_kick_our_433_formation = readFormation( configpath + GOAL_KICK_OUR_FORM_433_CONF);
    if (! M_form_goal_kick_our_433_formation)
    {
        std::cerr << "Failed to read  M_form_goal_kick_our_433 formation" << std::endl;
        return false;       
    }

    M_form_goalie_catch_opp_433_formation = readFormation( configpath + GOALIE_CATCH_OPP_FORM_433_CONF);
    if (! M_form_goalie_catch_opp_433_formation)
    {
        std::cerr << "Failed to read  M_form_goalie_catch_opp_433 formation" << std::endl;
        return false;       
    }

    M_form_goalie_catch_our_433_formation = readFormation( configpath + GOALIE_CATCH_OUR_FORM_433_CONF);
    if (! M_form_goalie_catch_our_433_formation)
    {
        std::cerr << "Failed to read M_form_goalie_catch_our_433 formation" << std::endl;
        return false;       
    }

    M_form_indirect_freekick_opp_433_formation = readFormation( configpath + INDIRECT_FREEKICK_OPP_FORM_433_CONF);
    if (! M_form_indirect_freekick_opp_433_formation)
    {
        std::cerr << "Failed to read M_form_indirect_freekick_opp_433 formation" << std::endl;
        return false;       
    }

    M_form_indirect_freekick_our_433_formation = readFormation( configpath + INDIRECT_FREEKICK_OUR_FORM_433_CONF);
    if (! M_form_indirect_freekick_our_433_formation)
    {
        std::cerr << "Failed to read M_form_indirect_freekick_our_433 formation" << std::endl;
        return false;       
    }

    M_form_kickin_our_433_formation = readFormation( configpath + KICKIN_OUR_FORM_433_CONF);
    if (! M_form_kickin_our_433_formation)
    {
        std::cerr << "Failed to read M_form_kickin_our_433 formation" << std::endl;
        return false;       
    }

    M_form_setplay_opp_433_formation = readFormation( configpath + SETPLAY_OPP_FORM_433_CONF);
    if (! M_form_setplay_opp_433_formation)
    {
        std::cerr << "Failed to read M_form_setplay_opp_433 formation" << std::endl;
        return false;       
    }

    M_form_setplay_our_433_formation = readFormation( configpath + SETPLAY_OUR_FORM_433_CONF);
    if (! M_form_setplay_our_433_formation)
    {
        std::cerr << "Failed to read M_form_setplay_our_433 formation" << std::endl;
        return false;       
    }

    // // 442
    M_form_BKO_442_formation = readFormation( configpath + BK_FORM_442_CONF);
    if (! M_form_BKO_442_formation)
    {
        std::cerr << "Failed to read  M_form_BKO_442 formation" << std::endl;
        return false;       
    }

    M_form_goal_kick_opp_442_formation = readFormation( configpath + GOAL_KICK_OPP_FORM_442_CONF);
    if (! M_form_goal_kick_opp_442_formation)
    {
        std::cerr << "Failed to read  M_form_goal_kick_opp_442 formation" << std::endl;
        return false;       
    }

    M_form_goal_kick_our_442_formation = readFormation( configpath + GOAL_KICK_OUR_FORM_442_CONF);
    if (! M_form_goal_kick_our_442_formation)
    {
        std::cerr << "Failed to read  M_form_goal_kick_our_442 formation" << std::endl;
        return false;       
    }

    M_form_goalie_catch_opp_442_formation = readFormation( configpath + GOALIE_CATCH_OPP_FORM_442_CONF);
    if (! M_form_goalie_catch_opp_442_formation)
    {
        std::cerr << "Failed to read  M_form_goalie_catch_opp_442 formation" << std::endl;
        return false;       
    }

    M_form_goalie_catch_our_442_formation = readFormation( configpath + GOALIE_CATCH_OUR_FORM_442_CONF);
    if (! M_form_goalie_catch_our_442_formation)
    {
        std::cerr << "Failed to read M_form_goalie_catch_our_442 formation" << std::endl;
        return false;       
    }

    M_form_indirect_freekick_opp_442_formation = readFormation( configpath + INDIRECT_FREEKICK_OPP_FORM_442_CONF);
    if (! M_form_indirect_freekick_opp_442_formation)
    {
        std::cerr << "Failed to read M_form_indirect_freekick_opp_442 formation" << std::endl;
        return false;       
    }

    M_form_indirect_freekick_our_442_formation = readFormation( configpath + INDIRECT_FREEKICK_OUR_FORM_442_CONF);
    if (! M_form_indirect_freekick_our_442_formation)
    {
        std::cerr << "Failed to read M_form_indirect_freekick_our_442 formation" << std::endl;
        return false;       
    }

    M_form_kickin_our_442_formation = readFormation( configpath + KICKIN_OUR_FORM_442_CONF);
    if (! M_form_kickin_our_442_formation)
    {
        std::cerr << "Failed to read M_form_kickin_our_442 formation" << std::endl;
        return false;       
    }

    M_form_setplay_opp_442_formation = readFormation( configpath + SETPLAY_OPP_FORM_442_CONF);
    if (! M_form_setplay_opp_442_formation)
    {
        std::cerr << "Failed to read M_form_setplay_opp_442 formation" << std::endl;
        return false;       
    }

    M_form_setplay_our_442_formation = readFormation( configpath + SETPLAY_OUR_FORM_442_CONF);
    if (! M_form_setplay_our_442_formation)
    {
        std::cerr << "Failed to read M_form_setplay_our_442 formation" << std::endl;
        return false;       
    }

    // // 541
    M_form_BKO_541_formation = readFormation( configpath + BK_FORM_541_CONF);
    if (! M_form_BKO_541_formation)
    {
        std::cerr << "Failed to read  M_form_BKO_541 formation" << std::endl;
        return false;       
    }

    M_form_goal_kick_opp_541_formation = readFormation( configpath + GOAL_KICK_OPP_FORM_541_CONF);
    if (! M_form_goal_kick_opp_541_formation)
    {
        std::cerr << "Failed to read  M_form_goal_kick_opp_541 formation" << std::endl;
        return false;       
    }

    M_form_goal_kick_our_541_formation = readFormation( configpath + GOAL_KICK_OUR_FORM_541_CONF);
    if (! M_form_goal_kick_our_541_formation)
    {
        std::cerr << "Failed to read  M_form_goal_kick_our_541 formation" << std::endl;
        return false;       
    }

    M_form_goalie_catch_opp_541_formation = readFormation( configpath + GOALIE_CATCH_OPP_FORM_541_CONF);
    if (! M_form_goalie_catch_opp_541_formation)
    {
        std::cerr << "Failed to read  M_form_goalie_catch_opp_541 formation" << std::endl;
        return false;       
    }

    M_form_goalie_catch_our_541_formation = readFormation( configpath + GOALIE_CATCH_OUR_FORM_541_CONF);
    if (! M_form_goalie_catch_our_541_formation)
    {
        std::cerr << "Failed to read M_form_goalie_catch_our_541 formation" << std::endl;
        return false;       
    }

    M_form_indirect_freekick_opp_541_formation = readFormation( configpath + INDIRECT_FREEKICK_OPP_FORM_541_CONF);
    if (! M_form_indirect_freekick_opp_541_formation)
    {
        std::cerr << "Failed to read M_form_indirect_freekick_opp_541 formation" << std::endl;
        return false;       
    }

    M_form_indirect_freekick_our_541_formation = readFormation( configpath + INDIRECT_FREEKICK_OUR_FORM_541_CONF);
    if (! M_form_indirect_freekick_our_541_formation)
    {
        std::cerr << "Failed to read M_form_indirect_freekick_our_541 formation" << std::endl;
        return false;       
    }

    M_form_kickin_our_541_formation = readFormation( configpath + KICKIN_OUR_FORM_541_CONF);
    if (! M_form_kickin_our_541_formation)
    {
        std::cerr << "Failed to read M_form_kickin_our_541 formation" << std::endl;
        return false;       
    }

    M_form_setplay_opp_541_formation = readFormation( configpath + SETPLAY_OPP_FORM_541_CONF);
    if (! M_form_setplay_opp_541_formation)
    {
        std::cerr << "Failed to read M_form_setplay_opp_541 formation" << std::endl;
        return false;       
    }

    M_form_setplay_our_541_formation = readFormation( configpath + SETPLAY_OUR_FORM_541_CONF);
    if (! M_form_setplay_our_541_formation)
    {
        std::cerr << "Failed to read M_form_setplay_our_541 formation" << std::endl;
        return false;       
    }  

    // // 4123
    M_form_BKO_4123_formation = readFormation( configpath + BK_FORM_4123_CONF);
    if (! M_form_BKO_4123_formation)
    {
        std::cerr << "Failed to read  M_form_BKO_4123 formation" << std::endl;
        return false;       
    }

    M_form_goal_kick_opp_4123_formation = readFormation( configpath + GOAL_KICK_OPP_FORM_4123_CONF);
    if (! M_form_goal_kick_opp_4123_formation)
    {
        std::cerr << "Failed to read  M_form_goal_kick_opp_4123 formation" << std::endl;
        return false;       
    }

    M_form_goal_kick_our_4123_formation = readFormation( configpath + GOAL_KICK_OUR_FORM_4123_CONF);
    if (! M_form_goal_kick_our_4123_formation)
    {
        std::cerr << "Failed to read  M_form_goal_kick_our_4123 formation" << std::endl;
        return false;       
    }

    M_form_goalie_catch_opp_4123_formation = readFormation( configpath + GOALIE_CATCH_OPP_FORM_4123_CONF);
    if (! M_form_goalie_catch_opp_4123_formation)
    {
        std::cerr << "Failed to read  M_form_goalie_catch_opp_4123 formation" << std::endl;
        return false;       
    }

    M_form_goalie_catch_our_4123_formation = readFormation( configpath + GOALIE_CATCH_OUR_FORM_4123_CONF);
    if (! M_form_goalie_catch_our_4123_formation)
    {
        std::cerr << "Failed to read M_form_goalie_catch_our_4123 formation" << std::endl;
        return false;       
    }

    M_form_indirect_freekick_opp_4123_formation = readFormation( configpath + INDIRECT_FREEKICK_OPP_FORM_4123_CONF);
    if (! M_form_indirect_freekick_opp_4123_formation)
    {
        std::cerr << "Failed to read M_form_indirect_freekick_opp_4123 formation" << std::endl;
        return false;       
    }

    M_form_indirect_freekick_our_4123_formation = readFormation( configpath + INDIRECT_FREEKICK_OUR_FORM_4123_CONF);
    if (! M_form_indirect_freekick_our_4123_formation)
    {
        std::cerr << "Failed to read M_form_indirect_freekick_our_4123 formation" << std::endl;
        return false;       
    }

    M_form_kickin_our_4123_formation = readFormation( configpath + KICKIN_OUR_FORM_4123_CONF);
    if (! M_form_kickin_our_4123_formation)
    {
        std::cerr << "Failed to read M_form_kickin_our_4123 formation" << std::endl;
        return false;       
    }

    M_form_setplay_opp_4123_formation = readFormation( configpath + SETPLAY_OPP_FORM_4123_CONF);
    if (! M_form_setplay_opp_4123_formation)
    {
        std::cerr << "Failed to read M_form_setplay_opp_4123 formation" << std::endl;
        return false;       
    }

    M_form_setplay_our_4123_formation = readFormation( configpath + SETPLAY_OUR_FORM_4123_CONF);
    if (! M_form_setplay_our_4123_formation)
    {
        std::cerr << "Failed to read M_form_setplay_our_4123 formation" << std::endl;
        return false;       
    } 

    // // 4213
    M_form_BKO_4213_formation = readFormation( configpath + BK_FORM_4213_CONF);
    if (! M_form_BKO_4213_formation)
    {
        std::cerr << "Failed to read  M_form_BKO_4213 formation" << std::endl;
        return false;       
    }

    M_form_goal_kick_opp_4213_formation = readFormation( configpath + GOAL_KICK_OPP_FORM_4213_CONF);
    if (! M_form_goal_kick_opp_4213_formation)
    {
        std::cerr << "Failed to read  M_form_goal_kick_opp_4213 formation" << std::endl;
        return false;       
    }

    M_form_goal_kick_our_4213_formation = readFormation( configpath + GOAL_KICK_OUR_FORM_4213_CONF);
    if (! M_form_goal_kick_our_4213_formation)
    {
        std::cerr << "Failed to read  M_form_goal_kick_our_4213 formation" << std::endl;
        return false;       
    }

    M_form_goalie_catch_opp_4213_formation = readFormation( configpath + GOALIE_CATCH_OPP_FORM_4213_CONF);
    if (! M_form_goalie_catch_opp_4213_formation)
    {
        std::cerr << "Failed to read  M_form_goalie_catch_opp_4213 formation" << std::endl;
        return false;       
    }

    M_form_goalie_catch_our_4213_formation = readFormation( configpath + GOALIE_CATCH_OUR_FORM_4213_CONF);
    if (! M_form_goalie_catch_our_4213_formation)
    {
        std::cerr << "Failed to read M_form_goalie_catch_our_4213 formation" << std::endl;
        return false;       
    }

    M_form_indirect_freekick_opp_4213_formation = readFormation( configpath + INDIRECT_FREEKICK_OPP_FORM_4213_CONF);
    if (! M_form_indirect_freekick_opp_4213_formation)
    {
        std::cerr << "Failed to read M_form_indirect_freekick_opp_4213 formation" << std::endl;
        return false;       
    }

    M_form_indirect_freekick_our_4213_formation = readFormation( configpath + INDIRECT_FREEKICK_OUR_FORM_4213_CONF);
    if (! M_form_indirect_freekick_our_4213_formation)
    {
        std::cerr << "Failed to read M_form_indirect_freekick_our_4213 formation" << std::endl;
        return false;       
    }

    M_form_kickin_our_4213_formation = readFormation( configpath + KICKIN_OUR_FORM_4213_CONF);
    if (! M_form_kickin_our_4213_formation)
    {
        std::cerr << "Failed to read M_form_kickin_our_4213 formation" << std::endl;
        return false;       
    }

    M_form_setplay_opp_4213_formation = readFormation( configpath + SETPLAY_OPP_FORM_4213_CONF);
    if (! M_form_setplay_opp_4213_formation)
    {
        std::cerr << "Failed to read M_form_setplay_opp_4213 formation" << std::endl;
        return false;       
    }

    M_form_setplay_our_4213_formation = readFormation( configpath + SETPLAY_OUR_FORM_4213_CONF);
    if (! M_form_setplay_our_4213_formation)
    {
        std::cerr << "Failed to read M_form_setplay_our_4213 formation" << std::endl;
        return false;       
    } 

    // End of Genius formations


    // before kick off
    M_before_kick_off_formation = readFormation( configpath + BEFORE_KICK_OFF_CONF );
    if ( ! M_before_kick_off_formation )
    {
        std::cerr << "Failed to read before_kick_off formation" << std::endl;
        return false;
    }

    ///////////////////////////////////////////////////////////
    M_normal_formation = readFormation( configpath + NORMAL_FORMATION_CONF );
    if ( ! M_normal_formation )
    {
        std::cerr << "Failed to read normal formation" << std::endl;
        return false;
    }

    M_defense_formation = readFormation( configpath + DEFENSE_FORMATION_CONF );
    if ( ! M_defense_formation )
    {
        std::cerr << "Failed to read defense formation" << std::endl;
        return false;
    }

    M_offense_formation = readFormation( configpath + OFFENSE_FORMATION_CONF );
    if ( ! M_offense_formation )
    {
        std::cerr << "Failed to read offense formation" << std::endl;
        return false;
    }

    M_goal_kick_opp_formation = readFormation( configpath + GOAL_KICK_OPP_FORMATION_CONF );
    if ( ! M_goal_kick_opp_formation )
    {
        return false;
    }

    M_goal_kick_our_formation = readFormation( configpath + GOAL_KICK_OUR_FORMATION_CONF );
    if ( ! M_goal_kick_our_formation )
    {
        return false;
    }

    M_goalie_catch_opp_formation = readFormation( configpath + GOALIE_CATCH_OPP_FORMATION_CONF );
    if ( ! M_goalie_catch_opp_formation )
    {
        return false;
    }

    M_goalie_catch_our_formation = readFormation( configpath + GOALIE_CATCH_OUR_FORMATION_CONF );
    if ( ! M_goalie_catch_our_formation )
    {
        return false;
    }

    M_kickin_our_formation = readFormation( configpath + KICKIN_OUR_FORMATION_CONF );
    if ( ! M_kickin_our_formation )
    {
        std::cerr << "Failed to read kickin our formation" << std::endl;
        return false;
    }

    M_setplay_opp_formation = readFormation( configpath + SETPLAY_OPP_FORMATION_CONF );
    if ( ! M_setplay_opp_formation )
    {
        std::cerr << "Failed to read setplay opp formation" << std::endl;
        return false;
    }

    M_setplay_our_formation = readFormation( configpath + SETPLAY_OUR_FORMATION_CONF );
    if ( ! M_setplay_our_formation )
    {
        std::cerr << "Failed to read setplay our formation" << std::endl;
        return false;
    }

    M_indirect_freekick_opp_formation = readFormation( configpath + INDIRECT_FREEKICK_OPP_FORMATION_CONF );
    if ( ! M_indirect_freekick_opp_formation )
    {
        std::cerr << "Failed to read indirect freekick opp formation" << std::endl;
        return false;
    }

    M_indirect_freekick_our_formation = readFormation( configpath + INDIRECT_FREEKICK_OUR_FORMATION_CONF );
    if ( ! M_indirect_freekick_our_formation )
    {
        std::cerr << "Failed to read indirect freekick our formation" << std::endl;
        return false;
    }

    // Nuevas formaciones
    M_form_def_532_formation = readFormation( configpath + DEF_FORM_532_CONF);
    if (! M_form_def_532_formation)
    {
        std::cerr << "Failed to read M_form_def_532 formation" << std::endl;
        return false;       
    }

    M_form_off_532_formation = readFormation( configpath + OFF_FORM_532_CONF);
    if (! M_form_off_532_formation)
    {
        std::cerr << "Failed to read M_form_off_532 formation" << std::endl;
        return false;       
    }

    M_form_norm_532_formation = readFormation( configpath + NORM_FORM_532_CONF);
    if (! M_form_norm_532_formation)
    {
        std::cerr << "Failed to read M_form_norm_532 formation" << std::endl;
        return false;       
    }

    M_form_BKO_532_formation = readFormation( configpath + BK_FORM_532_CONF);
    if (! M_form_BKO_532_formation)
    {
        std::cerr << "Failed to read  M_form_BKO_532 formation" << std::endl;
        return false;       
    }

    s_initialized = true;
    return true;
}

/*-------------------------------------------------------------------*/
/*!

 */
Formation::Ptr
Strategy::readFormation( const std::string & filepath )
{
    Formation::Ptr f;

    std::ifstream fin( filepath.c_str() );
    if ( ! fin.is_open() )
    {
        std::cerr << __FILE__ << ':' << __LINE__ << ':'
                  << " ***ERROR*** failed to open file [" << filepath << "]"
                  << std::endl;
        return f;
    }

    std::string temp, type;
    fin >> temp >> type; // read training method type name
    fin.seekg( 0 );

    f = createFormation( type );

    if ( ! f )
    {
        std::cerr << __FILE__ << ':' << __LINE__ << ':'
                  << " ***ERROR*** failed to create formation [" << filepath << "]"
                  << std::endl;
        return f;
    }

    //
    // read data from file
    //
    if ( ! f->read( fin ) )
    {
        std::cerr << __FILE__ << ':' << __LINE__ << ':'
                  << " ***ERROR*** failed to read formation [" << filepath << "]"
                  << std::endl;
        f.reset();
        return f;
    }


    //
    // check role names
    //
    for ( int unum = 1; unum <= 11; ++unum )
    {
        const std::string role_name = f->getRoleName( unum );
        if ( role_name == "Savior"
             || role_name == "Goalie" )
        {
            if ( M_goalie_unum == Unum_Unknown )
            {
                M_goalie_unum = unum;
            }

            if ( M_goalie_unum != unum )
            {
                std::cerr << __FILE__ << ':' << __LINE__ << ':'
                          << " ***ERROR*** Illegal goalie's uniform number"
                          << " read unum=" << unum
                          << " expected=" << M_goalie_unum
                          << std::endl;
                f.reset();
                return f;
            }
        }


#ifdef USE_GENERIC_FACTORY
        SoccerRole::Ptr role = SoccerRole::create( role_name );
        if ( ! role )
        {
            std::cerr << __FILE__ << ':' << __LINE__ << ':'
                      << " ***ERROR*** Unsupported role name ["
                      << role_name << "] is appered in ["
                      << filepath << "]" << std::endl;
            f.reset();
            return f;
        }
#else
        if ( M_role_factory.find( role_name ) == M_role_factory.end() )
        {
            std::cerr << __FILE__ << ':' << __LINE__ << ':'
                      << " ***ERROR*** Unsupported role name ["
                      << role_name << "] is appered in ["
                      << filepath << "]" << std::endl;
            f.reset();
            return f;
        }
#endif
    }

    return f;
}

/*-------------------------------------------------------------------*/
/*!

 */
Formation::Ptr
Strategy::createFormation( const std::string & type_name ) const
{
    Formation::Ptr f;

#ifdef USE_GENERIC_FACTORY
    f = Formation::create( type_name );
#else
    FormationFactory::const_iterator creator = M_formation_factory.find( type_name );
    if ( creator == M_formation_factory.end() )
    {
        std::cerr << __FILE__ << ": " << __LINE__
                  << " ***ERROR*** unsupported formation type ["
                  << type_name << "]"
                  << std::endl;
        return f;
    }
    f = creator->second();
#endif

    if ( ! f )
    {
        std::cerr << __FILE__ << ": " << __LINE__
                  << " ***ERROR*** unsupported formation type ["
                  << type_name << "]"
                  << std::endl;
    }

    return f;
}

/*-------------------------------------------------------------------*/
/*!

 */
void
Strategy::update( const WorldModel & wm )
{
    static GameTime s_update_time( -1, 0 );

    if ( s_update_time == wm.time() )
    {
        return;
    }
    s_update_time = wm.time();

    updateSituation( wm );
    updatePosition( wm );
}

/*-------------------------------------------------------------------*/
/*!
    Parameters:
        const int unum0: uniform number 
        const int unum1: uniform number
    Description:
        Exchange roles between two players.
 */
void
Strategy::exchangeRole( const int unum0,
                        const int unum1 )
{
    if ( unum0 < 1 || 11 < unum0
         || unum1 < 1 || 11 < unum1 )
    {
        std::cerr << __FILE__ << ':' << __LINE__ << ':'
                  << "(exchangeRole) Illegal uniform number. "
                  << unum0 << ' ' << unum1
                  << std::endl;
        dlog.addText( Logger::TEAM,
                      __FILE__":(exchangeRole) Illegal unum. %d %d",
                      unum0, unum1 );
        return;
    }

    if ( unum0 == unum1 )
    {
        std::cerr << __FILE__ << ':' << __LINE__ << ':'
                  << "(exchangeRole) same uniform number. "
                  << unum0 << ' ' << unum1
                  << std::endl;
        dlog.addText( Logger::TEAM,
                      __FILE__":(exchangeRole) same unum. %d %d",
                      unum0, unum1 );
        return;
    }

    int role0 = M_role_number[unum0 - 1];
    int role1 = M_role_number[unum1 - 1];

    dlog.addText( Logger::TEAM,
                  __FILE__":(exchangeRole) unum=%d(role=%d) <-> unum=%d(role=%d)",
                  unum0, role0,
                  unum1, role1 );

    M_role_number[unum0 - 1] = role1;
    M_role_number[unum1 - 1] = role0;
}

/*-------------------------------------------------------------------*/
/*!
    Parameters:
        const int unum: uniform number.
    Description: check if it is a defensive player.

*/
bool
Strategy::isMarkerType( const int unum ) const
{
    int number = roleNumber( unum );

    if ( number == 2
         || number == 3
         || number == 4
         || number == 5 )
    {
        return true;
    }

    return false;
}

/*-------------------------------------------------------------------*/
/*!

 */
SoccerRole::Ptr
Strategy::createRole( const int unum,
                      const WorldModel & world ) const
{
    const int number = roleNumber( unum );

    SoccerRole::Ptr role;

    if ( number < 1 || 11 < number )
    {
        std::cerr << __FILE__ << ": " << __LINE__
                  << " ***ERROR*** Invalid player number " << number
                  << std::endl;
        return role;
    }

    Formation::Ptr f = getFormation( world );
    if ( ! f )
    {
        std::cerr << __FILE__ << ": " << __LINE__
                  << " ***ERROR*** faled to create role. Null formation" << std::endl;
        return role;
    }

    const std::string role_name = f->getRoleName( number );

#ifdef USE_GENERIC_FACTORY
    role = SoccerRole::create( role_name );
#else
    RoleFactory::const_iterator factory = M_role_factory.find( role_name );
    if ( factory != M_role_factory.end() )
    {
        role = factory->second();
    }
#endif

    if ( ! role )
    {
        std::cerr << __FILE__ << ": " << __LINE__
                  << " ***ERROR*** unsupported role name ["
                  << role_name << "]"
                  << std::endl;
    }
    return role;
}

/*-------------------------------------------------------------------*/
/*!

 */
void
Strategy::updateSituation( const WorldModel & wm )
{
    M_current_situation = Normal_Situation;

    if ( wm.gameMode().type() != GameMode::PlayOn )
    {
        if ( wm.gameMode().isPenaltyKickMode() )
        {
            dlog.addText( Logger::TEAM,
                          __FILE__": Situation PenaltyKick" );
            M_current_situation = PenaltyKick_Situation;
        }
        else if ( wm.gameMode().isPenaltyKickMode() )
        {
            dlog.addText( Logger::TEAM,
                          __FILE__": Situation OurSetPlay" );
            M_current_situation = OurSetPlay_Situation;
        }
        else
        {
            dlog.addText( Logger::TEAM,
                          __FILE__": Situation OppSetPlay" );
            M_current_situation = OppSetPlay_Situation;
        }
        return;
    }

    int self_min = wm.interceptTable()->selfReachCycle();
    int mate_min = wm.interceptTable()->teammateReachCycle();
    int opp_min = wm.interceptTable()->opponentReachCycle();
    int our_min = std::min( self_min, mate_min );

    if ( opp_min <= our_min - 2 )
    {
        dlog.addText( Logger::TEAM,
                      __FILE__": Situation Defense" );
        M_current_situation = Defense_Situation;
        return;
    }

    if ( our_min <= opp_min - 2 )
    {
        dlog.addText( Logger::TEAM,
                      __FILE__": Situation Offense" );
        M_current_situation = Offense_Situation;
        return;
    }

    dlog.addText( Logger::TEAM,
                  __FILE__": Situation Normal" );
}

/*-------------------------------------------------------------------*/
/*!

 */
void
Strategy::updatePosition( const WorldModel & wm )
{
    static GameTime s_update_time( 0, 0 );
    if ( s_update_time == wm.time() )
    {
        return;
    }
    s_update_time = wm.time();

    Formation::Ptr f = getFormation( wm );
    if ( ! f )
    {
        std::cerr << wm.teamName() << ':' << wm.self().unum() << ": "
                  << wm.time()
                  << " ***ERROR*** could not get the current formation" << std::endl;
        return;
    }

    int ball_step = 0;
    if ( wm.gameMode().type() == GameMode::PlayOn
         || wm.gameMode().type() == GameMode::GoalKick_ )
    {
        ball_step = std::min( 1000, wm.interceptTable()->teammateReachCycle() );
        ball_step = std::min( ball_step, wm.interceptTable()->opponentReachCycle() );
        ball_step = std::min( ball_step, wm.interceptTable()->selfReachCycle() );
    }

    Vector2D ball_pos = wm.ball().inertiaPoint( ball_step );

    dlog.addText( Logger::TEAM,
                  __FILE__": HOME POSITION: ball pos=(%.1f %.1f) step=%d",
                  ball_pos.x, ball_pos.y,
                  ball_step );

    M_positions.clear();
    f->getPositions( ball_pos, M_positions );

    if ( ServerParam::i().useOffside() )
    {
        double max_x = wm.offsideLineX();
        if ( ServerParam::i().kickoffOffside()
             && ( wm.gameMode().type() == GameMode::BeforeKickOff
                  || wm.gameMode().type() == GameMode::AfterGoal_ ) )
        {
            max_x = 0.0;
        }
        else
        {
            int mate_step = wm.interceptTable()->teammateReachCycle();
            if ( mate_step < 50 )
            {
                Vector2D trap_pos = wm.ball().inertiaPoint( mate_step );
                if ( trap_pos.x > max_x ) max_x = trap_pos.x;
            }

            max_x -= 1.0;
        }

        for ( int unum = 1; unum <= 11; ++unum )
        {
            if ( M_positions[unum-1].x > max_x )
            {
                dlog.addText( Logger::TEAM,
                              "____ %d offside. home_pos_x %.2f -> %.2f",
                              unum,
                              M_positions[unum-1].x, max_x );
                M_positions[unum-1].x = max_x;
            }
        }
    }

    M_position_types.clear();
    for ( int unum = 1; unum <= 11; ++unum )
    {
        PositionType type = Position_Center;
        if ( f->isSideType( unum ) )
        {
            type = Position_Left;
        }
        else if ( f->isSymmetryType( unum ) )
        {
            type = Position_Right;
        }

        M_position_types.push_back( type );

        dlog.addText( Logger::TEAM,
                      "__ %d home pos (%.2f %.2f) type=%d",
                      unum,
                      M_positions[unum-1].x, M_positions[unum-1].y,
                      type );
        dlog.addCircle( Logger::TEAM,
                        M_positions[unum-1], 0.5,
                        "#000000" );
    }
}


/*-------------------------------------------------------------------*/
/*!

 */
PositionType
Strategy::getPositionType( const int unum ) const
{
    const int number = roleNumber( unum );

    if ( number < 1 || 11 < number )
    {
        std::cerr << __FILE__ << ' ' << __LINE__
                  << ": Illegal number : " << number
                  << std::endl;
        return Position_Center;
    }

    try
    {
        return M_position_types.at( number - 1 );
    }
    catch ( std::exception & e )
    {
        std::cerr<< __FILE__ << ':' << __LINE__ << ':'
                 << " Exception caught! " << e.what()
                 << std::endl;
        return Position_Center;
    }
}

/*-------------------------------------------------------------------*/
/*!

 */
Vector2D
Strategy::getPosition( const int unum ) const
{
    const int number = roleNumber( unum );

    if ( number < 1 || 11 < number )
    {
        std::cerr << __FILE__ << ' ' << __LINE__
                  << ": Illegal number : " << number
                  << std::endl;
        return Vector2D::INVALIDATED;
    }

    try
    {
        return M_positions.at( number - 1 );
    }
    catch ( std::exception & e )
    {
        std::cerr<< __FILE__ << ':' << __LINE__ << ':'
                 << " Exception caught! " << e.what()
                 << std::endl;
        return Vector2D::INVALIDATED;
    }
}


/*-------------------------------------------------------------------*/
/*!

 */
Formation::Ptr
Strategy::getFormation( const WorldModel & wm ) const
{
    std::string line;
    std::string opponentName = wm.opponentTeamName();
    // std::string fileName = "./enemyFormations/formation" + opponentName + ".txt";
    std::string fileName = "/home/jemd/Documents/USB/Tesis/robocup-coach/agent2d-3.1.1/src/enemyFormations/formation" + opponentName + ".txt";
    std::ifstream oppFormationFile(fileName.c_str());
    int offense = 0;
    int defense = 0;
    int center = 0;
    // writeToFormChangeFile(wm.self().unum(),stuff);
   // M_role_number[10]  role_number es solo eso, los numeros.
   // M_position_types.at(1) Esto indica si es un tipo simetrico o no.
   // M_positions es literal un x y y donde estan los jugadores segun la formacion si no me equivoco.

    // for ( AbstractPlayerCont::const_iterator t = wm.ourPlayers().begin();
    //     t != wm.ourPlayers().end(); ++t )
    // {
    //     std::cout <<  (*t)->unum() << std::endl;
    // }

    // (*t)->unum()
    // (*t)->type()

    int our_score = ( wm.ourSide() == LEFT
                      ? wm.gameMode().scoreLeft()
                      : wm.gameMode().scoreRight() );
    int opp_score = ( wm.ourSide() == LEFT
                      ? wm.gameMode().scoreRight()
                      : wm.gameMode().scoreLeft() );


    // Read the formation file and obtain the number of defense, centers and offense players.
    if (oppFormationFile.is_open()) {
        while (getline(oppFormationFile,line)) {
            defense = line[0] - '0';
            center = line[1] - '0';
            offense = line[2] - '0';
        }
        oppFormationFile.close();
    }

    if (defense == 0) {
        if (center >  1) 
            center = center - 1;
        else 
            offense = offense - 1;
        defense = defense + 1;
    }
    if (offense == 0) {
        if (center > 1)
            center = center - 1;
        else 
            defense = defense - 1;
        offense = offense + 1;
    }
    if (center == 0) {
        if (defense >= offense) 
            defense = defense - 1;
        else 
            offense = offense - 1;
        center = center + 1;
    }

    // Done reading formation

    // Define our formation :
    int ourFormation = 0;

    // Condiciones mias :
    // if (wm.time().cycle() >= 4200) {
    //         if (our_score <= opp_score) {
    //             if (opp_score - our_score < 2) {
    //                 ourFormation = 433;
    //             }
    //             else if (opp_score - our_score >= 2) {
    //                 ourFormation = 442;
    //             }
    //         }
    //         else if (our_score > opp_score) {
    //             if (offense > center)
    //                 ourFormation = 541;
    //             else
    //                 ourFormation = 442;
    //         }
    // }
    // else {
    //     if ((defense == 4) && (center == 3) && (offense == 3)) {
    //         ourFormation = 4123;
    //     }
    //     else if ((offense > center)) {
    //         ourFormation = 442;
    //     }
    //     else if (center >= 5) {
    //         ourFormation = 541;
    //     }
    //     else if (center < 5){
    //        ourFormation = 4213;
    //     }
    // }




    
    //
    // play on
    //
    if ( wm.gameMode().type() == GameMode::PlayOn )
    {

        switch ( M_current_situation ) {
        case Defense_Situation:
            if (ourFormation == 433) {
                writeToFormChangeFile(wm.self().unum(),433,1);
                return M_form_def_433_formation;
                // return M_form_433_def_formation; // Nuestra
            }
            else if (ourFormation == 442) {
                writeToFormChangeFile(wm.self().unum(),442,1);
                return M_form_def_442_formation;
                // return M_form_442_def_formation; // Nuestra
            }
            else if (ourFormation == 541) {
                writeToFormChangeFile(wm.self().unum(),541,1);
                return M_form_def_541_formation;
            }
            else if (ourFormation == 4123){
                writeToFormChangeFile(wm.self().unum(),4123,1);
                return M_form_def_4123_formation;
            }
            else if (ourFormation == 4213){
                writeToFormChangeFile(wm.self().unum(),4213,1);
                return M_form_def_4213_formation;
            }
            else if (ourFormation == 532) {
                writeToFormChangeFile(wm.self().unum(),532,1);
                return M_form_def_532_formation;            
            }
            else if (ourFormation == 4231) {
                writeToFormChangeFile(wm.self().unum(),4231,1);
                return M_form_4231_def_formation;
            }   
            else if (ourFormation == 352) {
                writeToFormChangeFile(wm.self().unum(),352,1);
                return M_form_352_def_formation;
            }
            else if (ourFormation == 451) {
                writeToFormChangeFile(wm.self().unum(),451,1);
                return M_form_451_def_formation;
            }
            return M_defense_formation;
        case Offense_Situation:
            if (ourFormation == 433) {
                writeToFormChangeFile(wm.self().unum(),433,2);
                return M_form_off_433_formation;
                // return M_form_433_off_formation; // Nuestra
            }
            else if (ourFormation == 442) {
                writeToFormChangeFile(wm.self().unum(),442,2);
                return M_form_off_442_formation;
                // return M_form_442_off_formation; // Nuestra
            }
            else if (ourFormation == 541) {
                writeToFormChangeFile(wm.self().unum(),541,2);
                return M_form_off_541_formation;
            }
            else if (ourFormation == 4123){
                writeToFormChangeFile(wm.self().unum(),4123,2);
                return M_form_off_4123_formation;
            }
            else if (ourFormation == 4213){
                writeToFormChangeFile(wm.self().unum(),4213,2);
                return M_form_off_4213_formation;
            }
            else if (ourFormation == 532){
                writeToFormChangeFile(wm.self().unum(),532,2);
                return M_form_off_532_formation;
            }
            else if (ourFormation == 4231) {
                writeToFormChangeFile(wm.self().unum(),4231,2);
                return M_form_4231_off_formation;
            } 
            else if (ourFormation == 352) {
                writeToFormChangeFile(wm.self().unum(),352,2);
                return M_form_352_off_formation;
            }
            else if (ourFormation == 451) {
                writeToFormChangeFile(wm.self().unum(),451,2);
                return M_form_451_off_formation;
            }
            return M_offense_formation;
        default:
            break;
        }
        if (ourFormation == 433) {
            writeToFormChangeFile(wm.self().unum(),433,0);
            return M_form_norm_433_formation;
            // return M_form_433_formation; // Nuestra
        }
        else if (ourFormation == 442) {
            writeToFormChangeFile(wm.self().unum(),442,0);
            // return M_form_norm_442_formation;
            return M_form_442_formation; // Nuestra
        }
        else if (ourFormation == 541) {
            writeToFormChangeFile(wm.self().unum(),541,0);
            return M_form_norm_541_formation;
        }
        else if (ourFormation == 4123){
            writeToFormChangeFile(wm.self().unum(),4123,0);
            return M_form_norm_4123_formation;
        }
        else if (ourFormation == 4213){
            writeToFormChangeFile(wm.self().unum(),4213,0);
            return M_form_norm_4213_formation;
        }
        else if (ourFormation == 532){
            writeToFormChangeFile(wm.self().unum(),532,0);
            return M_form_norm_532_formation;
        }
        else if (ourFormation == 4231) {
            writeToFormChangeFile(wm.self().unum(),4231,0);
            return M_form_4231_formation;
        } 
        else if (ourFormation == 352) {
            writeToFormChangeFile(wm.self().unum(),352,0);
            return M_form_352_formation;
        }
        else if (ourFormation == 451) {
            writeToFormChangeFile(wm.self().unum(),451,0);
            return M_form_451_formation;
        }
        return M_normal_formation;
    }
    else {
        if (!( wm.gameMode().type() == GameMode::BeforeKickOff
                 || wm.gameMode().type() == GameMode::AfterGoal_ )) {
            if (ourFormation == 532) {
                ourFormation = 541;
            }
            if (ourFormation == 352) {
                ourFormation = 442;
            }
            if (ourFormation == 4231) {
                ourFormation = 4123;
            }
            if (ourFormation == 451) {
                ourFormation = 541;
            }
        }
    }

    //
    // kick in, corner kick
    //
    if ( wm.gameMode().type() == GameMode::KickIn_
         || wm.gameMode().type() == GameMode::CornerKick_ )
    {

        if ( wm.ourSide() == wm.gameMode().side() )
        {
            // our kick-in or corner-kick
            if (ourFormation == 433) 
                return M_form_kickin_our_433_formation;
            else if (ourFormation == 442) 
                return M_form_kickin_our_442_formation;
            else if (ourFormation == 541) 
                return M_form_kickin_our_541_formation;
            else if (ourFormation == 4123)
                return M_form_kickin_our_4123_formation;
            else if (ourFormation == 4213)
                return M_form_kickin_our_4213_formation;
            return M_kickin_our_formation;


        }
        else
        {
            if (ourFormation == 433) 
                return M_form_setplay_opp_433_formation;
            else if (ourFormation == 442) 
                return M_form_setplay_opp_442_formation;
            else if (ourFormation == 541) 
                return M_form_setplay_opp_541_formation;
            else if (ourFormation == 4123)
                return M_form_setplay_opp_4123_formation;
            else if (ourFormation == 4213)
                return M_form_setplay_opp_4213_formation;
            return M_setplay_opp_formation;
        }
    }

    //
    // our indirect free kick
    //
    if ( ( wm.gameMode().type() == GameMode::BackPass_
           && wm.gameMode().side() == wm.theirSide() )
         || ( wm.gameMode().type() == GameMode::IndFreeKick_
              && wm.gameMode().side() == wm.ourSide() ) )
    {
        if (ourFormation == 433) 
            return M_form_indirect_freekick_our_433_formation;
        else if (ourFormation == 442) 
            return M_form_indirect_freekick_our_442_formation;
        else if (ourFormation == 541) 
            return M_form_indirect_freekick_our_541_formation;
        else if (ourFormation == 4123)
            return M_form_indirect_freekick_our_4123_formation;
        else if (ourFormation == 4213)
            return M_form_indirect_freekick_our_4213_formation;
        return M_indirect_freekick_our_formation;
    }

    //
    // opponent indirect free kick
    //
    if ( ( wm.gameMode().type() == GameMode::BackPass_
           && wm.gameMode().side() == wm.ourSide() )
         || ( wm.gameMode().type() == GameMode::IndFreeKick_
              && wm.gameMode().side() == wm.theirSide() ) )
    {

        if (ourFormation == 433) 
            return M_form_indirect_freekick_opp_433_formation;
        else if (ourFormation == 442) 
            return M_form_indirect_freekick_opp_442_formation;
        else if (ourFormation == 541) 
            return M_form_indirect_freekick_opp_541_formation;
        else if (ourFormation == 4123)
            return M_form_indirect_freekick_opp_4123_formation;
        else if (ourFormation == 4213)
            return M_form_indirect_freekick_opp_4213_formation;

        return M_indirect_freekick_opp_formation;
    }

    //
    // after foul
    //
    if ( wm.gameMode().type() == GameMode::FoulCharge_
         || wm.gameMode().type() == GameMode::FoulPush_ )
    {

        if ( wm.gameMode().side() == wm.ourSide() )
        {
            //
            // opponent (indirect) free kick
            //
            if ( wm.ball().pos().x < ServerParam::i().ourPenaltyAreaLineX() + 1.0
                 && wm.ball().pos().absY() < ServerParam::i().penaltyAreaHalfWidth() + 1.0 )
            {
                if (ourFormation == 433) 
                    return M_form_indirect_freekick_opp_433_formation;
                else if (ourFormation == 442) 
                    return M_form_indirect_freekick_opp_442_formation;
                else if (ourFormation == 541) 
                    return M_form_indirect_freekick_opp_541_formation;
                else if (ourFormation == 4123)
                    return M_form_indirect_freekick_opp_4123_formation;
                else if (ourFormation == 4213)
                    return M_form_indirect_freekick_opp_4213_formation;
                return M_indirect_freekick_opp_formation;
            }
            else
            {
                if (ourFormation == 433) 
                    return M_form_setplay_opp_433_formation;
                else if (ourFormation == 442) 
                    return M_form_setplay_opp_442_formation;
                else if (ourFormation == 541) 
                    return M_form_setplay_opp_541_formation;
                else if (ourFormation == 4123)
                    return M_form_setplay_opp_4123_formation;
                else if (ourFormation == 4213)
                    return M_form_setplay_opp_4213_formation;
                return M_setplay_opp_formation;
            }
        }
        else
        {
            //
            // our (indirect) free kick
            //
            if ( wm.ball().pos().x > ServerParam::i().theirPenaltyAreaLineX()
                 && wm.ball().pos().absY() < ServerParam::i().penaltyAreaHalfWidth() )
            {
                if (ourFormation == 433) 
                    return M_form_indirect_freekick_our_433_formation;
                else if (ourFormation == 442) 
                    return M_form_indirect_freekick_our_442_formation;
                else if (ourFormation == 541) 
                    return M_form_indirect_freekick_our_541_formation;
                else if (ourFormation == 4123)
                    return M_form_indirect_freekick_our_4123_formation;
                else if (ourFormation == 4213)
                    return M_form_indirect_freekick_our_4213_formation;
                return M_indirect_freekick_our_formation;
            }
            else
            {
                if (ourFormation == 433) 
                    return M_form_setplay_our_433_formation;
                else if (ourFormation == 442) 
                    return M_form_setplay_our_442_formation;
                else if (ourFormation == 541) 
                    return M_form_setplay_our_541_formation;
                else if (ourFormation == 4123)
                    return M_form_setplay_our_4123_formation;
                else if (ourFormation == 4213)
                    return M_form_setplay_our_4213_formation;
                return M_setplay_our_formation;
            }
        }
    }

    //
    // goal kick
    //
    if ( wm.gameMode().type() == GameMode::GoalKick_ )
    {

        if ( wm.gameMode().side() == wm.ourSide() )
        {  
            if (ourFormation == 433) 
                return M_form_goal_kick_our_433_formation;
            else if (ourFormation == 442) 
                return M_form_goal_kick_our_442_formation;
            else if (ourFormation == 541) 
                return M_form_goal_kick_our_541_formation;
            else if (ourFormation == 4123)
                return M_form_goal_kick_our_4123_formation;
            else if (ourFormation == 4213)
                return M_form_goal_kick_our_4213_formation;

            return M_goal_kick_our_formation;
        }
        else
        {
            if (ourFormation == 433) 
                return M_form_goal_kick_opp_433_formation;
            else if (ourFormation == 442) 
                return M_form_goal_kick_opp_442_formation;
            else if (ourFormation == 541) 
                return M_form_goal_kick_opp_541_formation;
            else if (ourFormation == 4123)
                return M_form_goal_kick_opp_4123_formation;
            else if (ourFormation == 4213)
                return M_form_goal_kick_opp_4213_formation;

            return M_goal_kick_opp_formation;
        }
    }

    //
    // goalie catch
    //
    if ( wm.gameMode().type() == GameMode::GoalieCatch_ )
    {

        if ( wm.gameMode().side() == wm.ourSide() )
        {
            if (ourFormation == 433) 
                return M_form_goalie_catch_our_433_formation;
            else if (ourFormation == 442) 
                return M_form_goalie_catch_our_442_formation;
            else if (ourFormation == 541) 
                return M_form_goalie_catch_our_541_formation;
            else if (ourFormation == 4123)
                return M_form_goalie_catch_our_4123_formation;
            else if (ourFormation == 4213)
                return M_form_goalie_catch_our_4213_formation;

            return M_goalie_catch_our_formation;
        }
        else
        {
            if (ourFormation == 433) 
                return M_form_goalie_catch_opp_433_formation;
            else if (ourFormation == 442) 
                return M_form_goalie_catch_opp_442_formation;
            else if (ourFormation == 541) 
                return M_form_goalie_catch_opp_541_formation;
            else if (ourFormation == 4123)
                return M_form_goalie_catch_opp_4123_formation;
            else if (ourFormation == 4213)
                return M_form_goalie_catch_opp_4213_formation;
            return M_goalie_catch_opp_formation;
        }
    }

    //
    // before kick off
    //
    if ( wm.gameMode().type() == GameMode::BeforeKickOff
         || wm.gameMode().type() == GameMode::AfterGoal_ )
    {

        if (wm.gameMode().type() == GameMode::AfterGoal_) { 
            if (wm.gameMode().side() == wm.ourSide()) {
                writeToFormChangeFile(wm.self().unum(),-1,5);
            }
            else {
                writeToFormChangeFile(wm.self().unum(),-2,5);
            }
        }

        if (ourFormation == 433) 
            return M_form_BKO_433_formation;
        else if (ourFormation == 442) 
            return M_form_BKO_442_formation;
        else if (ourFormation == 541) 
            return M_form_BKO_541_formation;
        else if (ourFormation == 4123)
            return M_form_BKO_4123_formation;
        else if (ourFormation == 4213)
            return M_form_BKO_4213_formation;
        else if (ourFormation == 532)
            return M_form_BKO_532_formation;
        else if (ourFormation == 352) 
            return M_form_BK_352_formation;
        else if (ourFormation == 4231) 
            return M_form_BK_4231_formation;
        else if (ourFormation == 451) 
            return M_form_BK_451_formation;

        return M_before_kick_off_formation;

        // New Before kickoff formations (COMMENT IF NEEDED) //
        // if ((defense == 4) && (center == 3) && (offense == 3)) 
        //     return M_form_BK_4231_formation;
        // else if (offense > center) 
        //     return M_form_BK_532_formation;
        // else if (center >= 5)
        //     return M_form_BK_451_formation;
        // else if (center < 5)
        //     return M_form_BK_4231_formation;
    }

    //
    // other set play
    //
    if ( wm.gameMode().isOurSetPlay( wm.ourSide() ) )
    {

        if (ourFormation == 433) 
            return M_form_setplay_our_433_formation;
        else if (ourFormation == 442) 
            return M_form_setplay_our_442_formation;
        else if (ourFormation == 541) 
            return M_form_setplay_our_541_formation;
        else if (ourFormation == 4123)
            return M_form_setplay_our_4123_formation;
        else if (ourFormation == 4213)
            return M_form_setplay_our_4213_formation;

        return M_setplay_our_formation;
    }

    if ( wm.gameMode().type() != GameMode::PlayOn )
    {

        if (ourFormation == 433) 
            return M_form_setplay_opp_433_formation;
        else if (ourFormation == 442) 
            return M_form_setplay_opp_442_formation;
        else if (ourFormation == 541) 
            return M_form_setplay_opp_541_formation;
        else if (ourFormation == 4123)
            return M_form_setplay_opp_4123_formation;
        else if (ourFormation == 4213)
            return M_form_setplay_opp_4213_formation;

        return M_setplay_opp_formation;
    }

    //
    // unknown
    //
    switch ( M_current_situation ) {
        case Defense_Situation:
            if (ourFormation == 433) {
                writeToFormChangeFile(wm.self().unum(),433,1);
                return M_form_def_433_formation;
            }
            else if (ourFormation == 442) {
                writeToFormChangeFile(wm.self().unum(),442,1);
                return M_form_def_442_formation;
            }
            else if (ourFormation == 541) {
                writeToFormChangeFile(wm.self().unum(),541,1);
                return M_form_def_541_formation;
            }
            else if (ourFormation == 4123){
                writeToFormChangeFile(wm.self().unum(),4123,1);
                return M_form_def_4123_formation;
            }
            else if (ourFormation == 4213){
                writeToFormChangeFile(wm.self().unum(),4213,1);
                return M_form_def_4213_formation;
            }
            else if (ourFormation == 532){
                writeToFormChangeFile(wm.self().unum(),532,1);
                return M_form_def_532_formation;
            }
            return M_defense_formation;
        case Offense_Situation:
            if (ourFormation == 433) {
                writeToFormChangeFile(wm.self().unum(),433,2);
                return M_form_off_433_formation;
            }
            else if (ourFormation == 442) {
                writeToFormChangeFile(wm.self().unum(),442,2);
                return M_form_off_442_formation;
            }
            else if (ourFormation == 541) {
                writeToFormChangeFile(wm.self().unum(),541,2);
                return M_form_off_541_formation;
            }
            else if (ourFormation == 4123){
                writeToFormChangeFile(wm.self().unum(),4123,2);
                return M_form_off_4123_formation;
            }
            else if (ourFormation == 4213){
                writeToFormChangeFile(wm.self().unum(),4213,2);
                return M_form_off_4213_formation;
            }
            else if (ourFormation == 532){
                writeToFormChangeFile(wm.self().unum(),532,2);
                return M_form_off_532_formation;
            }
            return M_offense_formation;
        default:
            break;
    }
    if (ourFormation == 433) {
        writeToFormChangeFile(wm.self().unum(),433,0);
        return M_form_norm_433_formation;
    }
    else if (ourFormation == 442) {
        writeToFormChangeFile(wm.self().unum(),442,0);
        return M_form_norm_442_formation;
    }
    else if (ourFormation == 541) {
        writeToFormChangeFile(wm.self().unum(),541,0);
        return M_form_norm_541_formation;
    }
    else if (ourFormation == 4123){
        writeToFormChangeFile(wm.self().unum(),4123,0);
        return M_form_norm_4123_formation;
    }
    else if (ourFormation == 4213){
        writeToFormChangeFile(wm.self().unum(),4213,0);
        return M_form_norm_4213_formation;
    }
    else if (ourFormation == 532){
        writeToFormChangeFile(wm.self().unum(),532,0);
        return M_form_norm_532_formation;
    }
    return M_normal_formation;
}

/*-------------------------------------------------------------------*/
/*!

 */
Strategy::BallArea
Strategy::get_ball_area( const WorldModel & wm )
{
    int ball_step = 1000;
    ball_step = std::min( ball_step, wm.interceptTable()->teammateReachCycle() );
    ball_step = std::min( ball_step, wm.interceptTable()->opponentReachCycle() );
    ball_step = std::min( ball_step, wm.interceptTable()->selfReachCycle() );

    return get_ball_area( wm.ball().inertiaPoint( ball_step ) );
}

/*-------------------------------------------------------------------*/
/*!

 */
/* 
/    Parameters : 
/       Vector2D & ball_pos : Position of the ball.
/    Description : 
/       Given the ball position, this function will return the area
/       in which the ball is currently located in the world. Possible return values are :
/       BA_CrossBlock:    x > -35.5 and y > 17.0 or x <= 35.5 and y > 17.0.
/       BA_Stopper:       x > -35.5 and y <= 17.0
/       BA_Danger:        x <= -35.5 and y <= 17.0
/       BA_DribbleBlock:  x > -30.0 and y > 17.0.
/       BA_DefMidField:   x > -30.0 and y <= 17.0.
/       BA_DribbleAttack: x > -1.0 and y > 17.0.
/       BA_OffMidField:   x > -1.0 and y <= 17.0.
/       BA_Cross:         x > 36.0 and y > 17.0.
/       BA_ShootChance:   x > 36.0 and y <= 17.0.
/       BA_None :         The ball position isn't in any of these ranges.
*/
Strategy::BallArea
Strategy::get_ball_area( const Vector2D & ball_pos )
{
    dlog.addLine( Logger::TEAM,
                  52.5, -17.0, -52.5, -17.0,
                  "#999999" );
    dlog.addLine( Logger::TEAM,
                  52.5, 17.0, -52.5, 17.0,
                  "#999999" );
    dlog.addLine( Logger::TEAM,
                  36.0, -34.0, 36.0, 34.0,
                  "#999999" );
    dlog.addLine( Logger::TEAM,
                  -1.0, -34.0, -1.0, 34.0,
                  "#999999" );
    dlog.addLine( Logger::TEAM,
                  -30.0, -17.0, -30.0, 17.0,
                  "#999999" );
    dlog.addLine( Logger::TEAM,
                  //-36.5, -34.0, -36.5, 34.0,
                  -35.5, -34.0, -35.5, 34.0,
                  "#999999" );

    if ( ball_pos.x > 36.0 )
    {
        if ( ball_pos.absY() > 17.0 )
        {
            dlog.addText( Logger::TEAM,
                          __FILE__": get_ball_area: Cross" );
            dlog.addRect( Logger::TEAM,
                          36.0, -34.0, 52.5 - 36.0, 34.0 - 17.0,
                          "#00ff00" );
            dlog.addRect( Logger::TEAM,
                          36.0, 17.0, 52.5 - 36.0, 34.0 - 17.0,
                          "#00ff00" );
            return BA_Cross;
        }
        else
        {
            dlog.addText( Logger::TEAM,
                          __FILE__": get_ball_area: ShootChance" );
            dlog.addRect( Logger::TEAM,
                          36.0, -17.0, 52.5 - 36.0, 34.0,
                          "#00ff00" );
            return BA_ShootChance;
        }
    }
    else if ( ball_pos.x > -1.0 )
    {
        if ( ball_pos.absY() > 17.0 )
        {
            dlog.addText( Logger::TEAM,
                          __FILE__": get_ball_area: DribbleAttack" );
            dlog.addRect( Logger::TEAM,
                          -1.0, -34.0, 36.0 + 1.0, 34.0 - 17.0,
                          "#00ff00" );
            dlog.addRect( Logger::TEAM,
                          -1.0, 17.0, 36.0 + 1.0, 34.0 - 17.0,
                          "#00ff00" );
            return BA_DribbleAttack;
        }
        else
        {
            dlog.addText( Logger::TEAM,
                          __FILE__": get_ball_area: OffMidField" );
            dlog.addRect( Logger::TEAM,
                          -1.0, -17.0, 36.0 + 1.0, 34.0,
                          "#00ff00" );
            return BA_OffMidField;
        }
    }
    else if ( ball_pos.x > -30.0 )
    {
        if ( ball_pos.absY() > 17.0 )
        {
            dlog.addText( Logger::TEAM,
                          __FILE__": get_ball_area: DribbleBlock" );
            dlog.addRect( Logger::TEAM,
                          -30.0, -34.0, -1.0 + 30.0, 34.0 - 17.0,
                          "#00ff00" );
            dlog.addRect( Logger::TEAM,
                          -30.0, 17.0, -1.0 + 30.0, 34.0 - 17.0,
                          "#00ff00" );
            return BA_DribbleBlock;
        }
        else
        {
            dlog.addText( Logger::TEAM,
                          __FILE__": get_ball_area: DefMidField" );
            dlog.addRect( Logger::TEAM,
                          -30.0, -17.0, -1.0 + 30.0, 34.0,
                          "#00ff00" );
            return BA_DefMidField;
        }
    }
    // 2009-06-17 akiyama: -36.5 -> -35.5
    //else if ( ball_pos.x > -36.5 )
    else if ( ball_pos.x > -35.5 )
    {
        if ( ball_pos.absY() > 17.0 )
        {
            dlog.addText( Logger::TEAM,
                          __FILE__": get_ball_area: CrossBlock" );
            dlog.addRect( Logger::TEAM,
                          //-36.5, -34.0, 36.5 - 30.0, 34.0 - 17.0,
                          -35.5, -34.0, 35.5 - 30.0, 34.0 - 17.0,
                          "#00ff00" );
            dlog.addRect( Logger::TEAM,
                          -35.5, 17.0, 35.5 - 30.0, 34.0 - 17.0,
                          "#00ff00" );
            return BA_CrossBlock;
        }
        else
        {
            dlog.addText( Logger::TEAM,
                          __FILE__": get_ball_area: Stopper" );
            dlog.addRect( Logger::TEAM,
                          //-36.5, -17.0, 36.5 - 30.0, 34.0,
                          -35.5, -17.0, 35.5 - 30.0, 34.0,
                          "#00ff00" );
            // 2009-06-17 akiyama: Stopper -> DefMidField
            //return BA_Stopper;
            return BA_DefMidField;
        }
    }
    else
    {
        if ( ball_pos.absY() > 17.0 )
        {
            dlog.addText( Logger::TEAM,
                          __FILE__": get_ball_area: CrossBlock" );
            dlog.addRect( Logger::TEAM,
                          -52.5, -34.0, 52.5 - 36.5, 34.0 - 17.0,
                          "#00ff00" );
            dlog.addRect( Logger::TEAM,
                          -52.5, 17.0, 52.5 - 36.5, 34.0 - 17.0,
                          "#00ff00" );
            return BA_CrossBlock;
        }
        else
        {
            dlog.addText( Logger::TEAM,
                          __FILE__": get_ball_area: Danger" );
            dlog.addRect( Logger::TEAM,
                          -52.5, -17.0, 52.5 - 36.5, 34.0,
                          "#00ff00" );
            return BA_Danger;
        }
    }

    dlog.addText( Logger::TEAM,
                  __FILE__": get_ball_area: unknown area" );
    return BA_None;
}

/*-------------------------------------------------------------------*/
/*!

 */
double
Strategy::get_normal_dash_power( const WorldModel & wm )
{
    static bool s_recover_mode = false;

    if ( wm.self().staminaModel().capacityIsEmpty() )
    {
        return std::min( ServerParam::i().maxDashPower(),
                         wm.self().stamina() + wm.self().playerType().extraStamina() );
    }

    const int self_min = wm.interceptTable()->selfReachCycle();
    const int mate_min = wm.interceptTable()->teammateReachCycle();
    const int opp_min = wm.interceptTable()->opponentReachCycle();

    // check recover
    if ( wm.self().staminaModel().capacityIsEmpty() )
    {
        s_recover_mode = false;
    }
    else if ( wm.self().stamina() < ServerParam::i().staminaMax() * 0.5 )
    {
        s_recover_mode = true;
    }
    else if ( wm.self().stamina() > ServerParam::i().staminaMax() * 0.7 )
    {
        s_recover_mode = false;
    }

    /*--------------------------------------------------------*/
    double dash_power = ServerParam::i().maxDashPower();
    const double my_inc
        = wm.self().playerType().staminaIncMax()
        * wm.self().recovery();

    if ( wm.ourDefenseLineX() > wm.self().pos().x
         && wm.ball().pos().x < wm.ourDefenseLineX() + 20.0 )
    {
        dlog.addText( Logger::TEAM,
                      __FILE__": (get_normal_dash_power) correct DF line. keep max power" );
        // keep max power
        dash_power = ServerParam::i().maxDashPower();
    }
    else if ( s_recover_mode )
    {
        dash_power = my_inc - 25.0; // preffered recover value
        if ( dash_power < 0.0 ) dash_power = 0.0;

        dlog.addText( Logger::TEAM,
                      __FILE__": (get_normal_dash_power) recovering" );
    }
    // exist kickable teammate
    else if ( wm.existKickableTeammate()
              && wm.ball().distFromSelf() < 20.0 )
    {
        dash_power = std::min( my_inc * 1.1,
                               ServerParam::i().maxDashPower() );
        dlog.addText( Logger::TEAM,
                      __FILE__": (get_normal_dash_power) exist kickable teammate. dash_power=%.1f",
                      dash_power );
    }
    // in offside area
    else if ( wm.self().pos().x > wm.offsideLineX() )
    {
        dash_power = ServerParam::i().maxDashPower();
        dlog.addText( Logger::TEAM,
                      __FILE__": in offside area. dash_power=%.1f",
                      dash_power );
    }
    else if ( wm.ball().pos().x > 25.0
              && wm.ball().pos().x > wm.self().pos().x + 10.0
              && self_min < opp_min - 6
              && mate_min < opp_min - 6 )
    {
        dash_power = bound( ServerParam::i().maxDashPower() * 0.1,
                            my_inc * 0.5,
                            ServerParam::i().maxDashPower() );
        dlog.addText( Logger::TEAM,
                      __FILE__": (get_normal_dash_power) opponent ball dash_power=%.1f",
                      dash_power );
    }
    // normal
    else
    {
        dash_power = std::min( my_inc * 1.7,
                               ServerParam::i().maxDashPower() );
        dlog.addText( Logger::TEAM,
                      __FILE__": (get_normal_dash_power) normal mode dash_power=%.1f",
                      dash_power );
    }

    return dash_power;
}
