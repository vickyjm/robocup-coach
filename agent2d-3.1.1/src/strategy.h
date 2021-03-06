// -*-c++-*-

/*!
  \file strategy.h
  \brief team strategy manager Header File
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

#ifndef STRATEGY_H
#define STRATEGY_H

#include "soccer_role.h"

#include <rcsc/formation/formation.h>
#include <rcsc/geom/vector_2d.h>

#include <boost/shared_ptr.hpp>
#include <map>
#include <vector>
#include <string>

// # define USE_GENERIC_FACTORY 1

namespace rcsc {
class CmdLineParser;
class WorldModel;
}

enum PositionType {
    Position_Left = -1,
    Position_Center = 0,
    Position_Right = 1,
};

enum SituationType {
    Normal_Situation,
    Offense_Situation,
    Defense_Situation,
    OurSetPlay_Situation,
    OppSetPlay_Situation,
    PenaltyKick_Situation,
};


class Strategy {
public:
    static const std::string BEFORE_KICK_OFF_CONF;
    static const std::string NORMAL_FORMATION_CONF;
    static const std::string DEFENSE_FORMATION_CONF;
    static const std::string OFFENSE_FORMATION_CONF;
    static const std::string GOAL_KICK_OPP_FORMATION_CONF;
    static const std::string GOAL_KICK_OUR_FORMATION_CONF;
    static const std::string GOALIE_CATCH_OPP_FORMATION_CONF;
    static const std::string GOALIE_CATCH_OUR_FORMATION_CONF;
    static const std::string KICKIN_OUR_FORMATION_CONF;
    static const std::string SETPLAY_OPP_FORMATION_CONF;
    static const std::string SETPLAY_OUR_FORMATION_CONF;
    static const std::string INDIRECT_FREEKICK_OPP_FORMATION_CONF;
    static const std::string INDIRECT_FREEKICK_OUR_FORMATION_CONF;

    static const std::string DEFENSE_4231_CONF;
    static const std::string DEFENSE_442_CONF;
    static const std::string DEFENSE_433_CONF;
    static const std::string OFFENSE_4231_CONF;
    static const std::string OFFENSE_442_CONF;
    static const std::string OFFENSE_433_CONF;

    // Our formations
    static const std::string FORM_352_CONF;
    static const std::string FORM_352_OFF_CONF;
    static const std::string FORM_352_DEF_CONF;
    static const std::string FORM_4231_CONF;
    static const std::string FORM_4231_OFF_CONF;
    static const std::string FORM_4231_DEF_CONF;
    static const std::string FORM_433_CONF;
    static const std::string FORM_433_OFF_CONF;
    static const std::string FORM_433_DEF_CONF;
    static const std::string FORM_442_CONF;
    static const std::string FORM_442_OFF_CONF;
    static const std::string FORM_442_DEF_CONF;
    static const std::string FORM_451_CONF;
    static const std::string FORM_451_OFF_CONF;
    static const std::string FORM_451_DEF_CONF;
    static const std::string FORM_532_CONF;

    static const std::string FORM_BK_352_CONF;
    static const std::string FORM_BK_4231_CONF;
    static const std::string FORM_BK_433_CONF;
    static const std::string FORM_BK_442_CONF;
    static const std::string FORM_BK_451_CONF;
    static const std::string FORM_BK_532_CONF;

    // Genius formations
    static const std::string DEF_FORM_433_CONF;
    static const std::string OFF_FORM_433_CONF;
    static const std::string NORM_FORM_433_CONF;
    static const std::string DEF_FORM_442_CONF;
    static const std::string OFF_FORM_442_CONF;
    static const std::string NORM_FORM_442_CONF;
    static const std::string DEF_FORM_541_CONF;
    static const std::string OFF_FORM_541_CONF;
    static const std::string NORM_FORM_541_CONF;
    static const std::string DEF_FORM_4123_CONF;
    static const std::string OFF_FORM_4123_CONF;
    static const std::string NORM_FORM_4123_CONF;
    static const std::string DEF_FORM_4213_CONF;
    static const std::string OFF_FORM_4213_CONF;
    static const std::string NORM_FORM_4213_CONF;

    // Genius other formations
    static const std::string BK_FORM_433_CONF;
    static const std::string GOAL_KICK_OPP_FORM_433_CONF;
    static const std::string GOAL_KICK_OUR_FORM_433_CONF;
    static const std::string GOALIE_CATCH_OPP_FORM_433_CONF;
    static const std::string GOALIE_CATCH_OUR_FORM_433_CONF;
    static const std::string INDIRECT_FREEKICK_OPP_FORM_433_CONF;
    static const std::string INDIRECT_FREEKICK_OUR_FORM_433_CONF;
    static const std::string KICKIN_OUR_FORM_433_CONF;
    static const std::string SETPLAY_OPP_FORM_433_CONF;
    static const std::string SETPLAY_OUR_FORM_433_CONF;

    static const std::string BK_FORM_442_CONF;
    static const std::string GOAL_KICK_OPP_FORM_442_CONF;
    static const std::string GOAL_KICK_OUR_FORM_442_CONF;
    static const std::string GOALIE_CATCH_OPP_FORM_442_CONF;
    static const std::string GOALIE_CATCH_OUR_FORM_442_CONF;
    static const std::string INDIRECT_FREEKICK_OPP_FORM_442_CONF;
    static const std::string INDIRECT_FREEKICK_OUR_FORM_442_CONF;
    static const std::string KICKIN_OUR_FORM_442_CONF;
    static const std::string SETPLAY_OPP_FORM_442_CONF;
    static const std::string SETPLAY_OUR_FORM_442_CONF;

    static const std::string BK_FORM_541_CONF;
    static const std::string GOAL_KICK_OPP_FORM_541_CONF;
    static const std::string GOAL_KICK_OUR_FORM_541_CONF;
    static const std::string GOALIE_CATCH_OPP_FORM_541_CONF;
    static const std::string GOALIE_CATCH_OUR_FORM_541_CONF;
    static const std::string INDIRECT_FREEKICK_OPP_FORM_541_CONF;
    static const std::string INDIRECT_FREEKICK_OUR_FORM_541_CONF;
    static const std::string KICKIN_OUR_FORM_541_CONF;
    static const std::string SETPLAY_OPP_FORM_541_CONF;
    static const std::string SETPLAY_OUR_FORM_541_CONF;

    static const std::string BK_FORM_4123_CONF;
    static const std::string GOAL_KICK_OPP_FORM_4123_CONF;
    static const std::string GOAL_KICK_OUR_FORM_4123_CONF;
    static const std::string GOALIE_CATCH_OPP_FORM_4123_CONF;
    static const std::string GOALIE_CATCH_OUR_FORM_4123_CONF;
    static const std::string INDIRECT_FREEKICK_OPP_FORM_4123_CONF;
    static const std::string INDIRECT_FREEKICK_OUR_FORM_4123_CONF;
    static const std::string KICKIN_OUR_FORM_4123_CONF;
    static const std::string SETPLAY_OPP_FORM_4123_CONF;
    static const std::string SETPLAY_OUR_FORM_4123_CONF;

    static const std::string BK_FORM_4213_CONF;
    static const std::string GOAL_KICK_OPP_FORM_4213_CONF;
    static const std::string GOAL_KICK_OUR_FORM_4213_CONF;
    static const std::string GOALIE_CATCH_OPP_FORM_4213_CONF;
    static const std::string GOALIE_CATCH_OUR_FORM_4213_CONF;
    static const std::string INDIRECT_FREEKICK_OPP_FORM_4213_CONF;
    static const std::string INDIRECT_FREEKICK_OUR_FORM_4213_CONF;
    static const std::string KICKIN_OUR_FORM_4213_CONF;
    static const std::string SETPLAY_OPP_FORM_4213_CONF;
    static const std::string SETPLAY_OUR_FORM_4213_CONF;

    // Formaciones nuevas 
    static const std::string DEF_FORM_532_CONF;
    static const std::string OFF_FORM_532_CONF;
    static const std::string NORM_FORM_532_CONF;
    static const std::string BK_FORM_532_CONF;





    enum BallArea {
        BA_CrossBlock, BA_DribbleBlock, BA_DribbleAttack, BA_Cross,
        BA_Stopper,    BA_DefMidField,  BA_OffMidField,   BA_ShootChance,
        BA_Danger,

        BA_None
    };

private:
    //
    // factories
    //
#ifndef USE_GENERIC_FACTORY
    typedef std::map< std::string, SoccerRole::Creator > RoleFactory;
    typedef std::map< std::string, rcsc::Formation::Creator > FormationFactory;

    RoleFactory M_role_factory;
    FormationFactory M_formation_factory;
#endif


    //
    // formations
    //

    rcsc::Formation::Ptr M_before_kick_off_formation;

    rcsc::Formation::Ptr M_normal_formation;
    rcsc::Formation::Ptr M_defense_formation;
    rcsc::Formation::Ptr M_offense_formation;

    rcsc::Formation::Ptr M_goal_kick_opp_formation;
    rcsc::Formation::Ptr M_goal_kick_our_formation;
    rcsc::Formation::Ptr M_goalie_catch_opp_formation;
    rcsc::Formation::Ptr M_goalie_catch_our_formation;
    rcsc::Formation::Ptr M_kickin_our_formation;
    rcsc::Formation::Ptr M_setplay_opp_formation;
    rcsc::Formation::Ptr M_setplay_our_formation;
    rcsc::Formation::Ptr M_indirect_freekick_opp_formation;
    rcsc::Formation::Ptr M_indirect_freekick_our_formation;

    rcsc::Formation::Ptr M_offense_442_formation;

    // Our formations
    rcsc::Formation::Ptr M_form_352_formation;
    rcsc::Formation::Ptr M_form_352_off_formation;
    rcsc::Formation::Ptr M_form_352_def_formation;
    rcsc::Formation::Ptr M_form_4231_formation;
    rcsc::Formation::Ptr M_form_4231_off_formation;
    rcsc::Formation::Ptr M_form_4231_def_formation;
    rcsc::Formation::Ptr M_form_433_formation;
    rcsc::Formation::Ptr M_form_433_off_formation;
    rcsc::Formation::Ptr M_form_433_def_formation;
    rcsc::Formation::Ptr M_form_442_formation;
    rcsc::Formation::Ptr M_form_442_off_formation;
    rcsc::Formation::Ptr M_form_442_def_formation;
    rcsc::Formation::Ptr M_form_451_formation;
    rcsc::Formation::Ptr M_form_451_off_formation;
    rcsc::Formation::Ptr M_form_451_def_formation;
    rcsc::Formation::Ptr M_form_532_formation;

    rcsc::Formation::Ptr M_form_BK_352_formation;
    rcsc::Formation::Ptr M_form_BK_4231_formation;
    rcsc::Formation::Ptr M_form_BK_433_formation;
    rcsc::Formation::Ptr M_form_BK_442_formation;
    rcsc::Formation::Ptr M_form_BK_451_formation;
    rcsc::Formation::Ptr M_form_BK_532_formation;

    // Genius formations
    rcsc::Formation::Ptr M_form_def_433_formation;
    rcsc::Formation::Ptr M_form_off_433_formation;
    rcsc::Formation::Ptr M_form_norm_433_formation; 
    rcsc::Formation::Ptr M_form_def_442_formation;
    rcsc::Formation::Ptr M_form_off_442_formation;
    rcsc::Formation::Ptr M_form_norm_442_formation;
    rcsc::Formation::Ptr M_form_def_541_formation;
    rcsc::Formation::Ptr M_form_off_541_formation;
    rcsc::Formation::Ptr M_form_norm_541_formation;
    rcsc::Formation::Ptr M_form_def_4123_formation;
    rcsc::Formation::Ptr M_form_off_4123_formation;
    rcsc::Formation::Ptr M_form_norm_4123_formation;
    rcsc::Formation::Ptr M_form_def_4213_formation;
    rcsc::Formation::Ptr M_form_off_4213_formation;
    rcsc::Formation::Ptr M_form_norm_4213_formation;

    // Other Genius formations
    rcsc::Formation::Ptr M_form_BKO_433_formation;
    rcsc::Formation::Ptr M_form_goal_kick_opp_433_formation;
    rcsc::Formation::Ptr M_form_goal_kick_our_433_formation;
    rcsc::Formation::Ptr M_form_goalie_catch_opp_433_formation;
    rcsc::Formation::Ptr M_form_goalie_catch_our_433_formation;
    rcsc::Formation::Ptr M_form_indirect_freekick_opp_433_formation;
    rcsc::Formation::Ptr M_form_indirect_freekick_our_433_formation;
    rcsc::Formation::Ptr M_form_kickin_our_433_formation;
    rcsc::Formation::Ptr M_form_setplay_opp_433_formation;
    rcsc::Formation::Ptr M_form_setplay_our_433_formation;

    rcsc::Formation::Ptr M_form_BKO_442_formation;
    rcsc::Formation::Ptr M_form_goal_kick_opp_442_formation;
    rcsc::Formation::Ptr M_form_goal_kick_our_442_formation;
    rcsc::Formation::Ptr M_form_goalie_catch_opp_442_formation;
    rcsc::Formation::Ptr M_form_goalie_catch_our_442_formation;
    rcsc::Formation::Ptr M_form_indirect_freekick_opp_442_formation;
    rcsc::Formation::Ptr M_form_indirect_freekick_our_442_formation;
    rcsc::Formation::Ptr M_form_kickin_our_442_formation;
    rcsc::Formation::Ptr M_form_setplay_opp_442_formation;
    rcsc::Formation::Ptr M_form_setplay_our_442_formation;

    rcsc::Formation::Ptr M_form_BKO_541_formation;
    rcsc::Formation::Ptr M_form_goal_kick_opp_541_formation;
    rcsc::Formation::Ptr M_form_goal_kick_our_541_formation;
    rcsc::Formation::Ptr M_form_goalie_catch_opp_541_formation;
    rcsc::Formation::Ptr M_form_goalie_catch_our_541_formation;
    rcsc::Formation::Ptr M_form_indirect_freekick_opp_541_formation;
    rcsc::Formation::Ptr M_form_indirect_freekick_our_541_formation;
    rcsc::Formation::Ptr M_form_kickin_our_541_formation;
    rcsc::Formation::Ptr M_form_setplay_opp_541_formation;
    rcsc::Formation::Ptr M_form_setplay_our_541_formation;

    rcsc::Formation::Ptr M_form_BKO_4123_formation;
    rcsc::Formation::Ptr M_form_goal_kick_opp_4123_formation;
    rcsc::Formation::Ptr M_form_goal_kick_our_4123_formation;
    rcsc::Formation::Ptr M_form_goalie_catch_opp_4123_formation;
    rcsc::Formation::Ptr M_form_goalie_catch_our_4123_formation;
    rcsc::Formation::Ptr M_form_indirect_freekick_opp_4123_formation;
    rcsc::Formation::Ptr M_form_indirect_freekick_our_4123_formation;
    rcsc::Formation::Ptr M_form_kickin_our_4123_formation;
    rcsc::Formation::Ptr M_form_setplay_opp_4123_formation;
    rcsc::Formation::Ptr M_form_setplay_our_4123_formation;

    rcsc::Formation::Ptr M_form_BKO_4213_formation;
    rcsc::Formation::Ptr M_form_goal_kick_opp_4213_formation;
    rcsc::Formation::Ptr M_form_goal_kick_our_4213_formation;
    rcsc::Formation::Ptr M_form_goalie_catch_opp_4213_formation;
    rcsc::Formation::Ptr M_form_goalie_catch_our_4213_formation;
    rcsc::Formation::Ptr M_form_indirect_freekick_opp_4213_formation;
    rcsc::Formation::Ptr M_form_indirect_freekick_our_4213_formation;
    rcsc::Formation::Ptr M_form_kickin_our_4213_formation;
    rcsc::Formation::Ptr M_form_setplay_opp_4213_formation;
    rcsc::Formation::Ptr M_form_setplay_our_4213_formation;

    // Formaciones nuevas
    rcsc::Formation::Ptr M_form_def_532_formation;
    rcsc::Formation::Ptr M_form_off_532_formation;
    rcsc::Formation::Ptr M_form_norm_532_formation; 
    rcsc::Formation::Ptr M_form_BKO_532_formation;

    int M_goalie_unum;


    // situation type
    SituationType M_current_situation;

    // role assignment
    std::vector< int > M_role_number;

    // current home positions
    std::vector< PositionType > M_position_types;
    std::vector< rcsc::Vector2D > M_positions;

    // private for singleton
    Strategy();

    // not used
    Strategy( const Strategy & );
    const Strategy & operator=( const Strategy & );
public:

    static
    Strategy & instance();

    static
    const
    Strategy & i()
      {
          return instance();
      }

    //
    // initialization
    //

    bool init( rcsc::CmdLineParser & cmd_parser );
    bool read( const std::string & config_dir );


    //
    // update
    //

    void update( const rcsc::WorldModel & wm );


    void exchangeRole( const int unum0,
                       const int unum1 );

    //
    // accessor to the current information
    //

    int goalieUnum() const { return M_goalie_unum; }

    int roleNumber( const int unum ) const
      {
          if ( unum < 1 || 11 < unum ) return unum;
          return M_role_number[unum - 1];
      }

    bool isMarkerType( const int unum ) const;


    SoccerRole::Ptr createRole( const int unum,
                                const rcsc::WorldModel & wm ) const;
    PositionType getPositionType( const int unum ) const;
    rcsc::Vector2D getPosition( const int unum ) const;


private:
    void updateSituation( const rcsc::WorldModel & wm );
    // update the current position table
    void updatePosition( const rcsc::WorldModel & wm );

    rcsc::Formation::Ptr readFormation( const std::string & filepath );
    rcsc::Formation::Ptr createFormation( const std::string & type_name ) const;

    rcsc::Formation::Ptr getFormation( const rcsc::WorldModel & wm ) const;

public:
    static
    BallArea get_ball_area( const rcsc::WorldModel & wm );
    static
    BallArea get_ball_area( const rcsc::Vector2D & ball_pos );

    static
    double get_normal_dash_power( const rcsc::WorldModel & wm );
};

#endif
