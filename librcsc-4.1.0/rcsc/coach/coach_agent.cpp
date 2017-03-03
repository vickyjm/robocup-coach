// -*-c++-*-

/*!
  \file coach_agent.cpp
  \brief basic coach agent Header File
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

#include "coach_agent.h"

#include "coach_audio_sensor.h"
#include "coach_config.h"
#include "coach_command.h"
#include "global_visual_sensor.h"
#include "global_world_model.h"

#include <rcsc/common/basic_client.h>
#include <rcsc/common/logger.h>
#include <rcsc/common/server_param.h>
#include <rcsc/common/player_param.h>
#include <rcsc/common/player_type.h>
#include <rcsc/common/team_graphic.h>
#include <rcsc/common/audio_memory.h>

#include <rcsc/param/param_map.h>
#include <rcsc/param/conf_file_parser.h>
#include <rcsc/param/cmd_line_parser.h>
#include <rcsc/version.h>

#include <sstream>
#include <cstring>
#include <vector>
#include <fstream>
#include <math.h>
#include <string> 

#include "cv.h"
#include "ml.h"


int opp_score = 0;
int old_opp_score = 0;
int our_score = 0;
int lastReset = 0;

namespace patch
{
    template < typename T > std::string to_string( const T& n )
    {
        std::ostringstream stm ;
        stm << n ;
        return stm.str() ;
    }
}

namespace rcsc {


/*
  This function is used for training the existing decision trees
  using the old log information and the actions extracted from the
  current game.

  IMPORTANT NOTE: The paths to the .dats files and the trees must
  be changed when the opponent changes.

*/
void trainTrees(){
  // Structures to keep the data from the logs
  CvMLData shotData;
  CvMLData dribbleData;
  CvMLData passData;

  // Read the files with the current info
  shotData.read_csv ("shotFile.dat");
  dribbleData.read_csv ("dribbleFile.dat");
  passData.read_csv ("passFile.dat");

  // Indicate which column is the response
  shotData.set_response_idx (9);
  dribbleData.set_response_idx (9);
  passData.set_response_idx (9);

  // Create the new decision trees for every type of action
  CvDTree* shotTree = new CvDTree();
  CvDTree* dribbleTree = new CvDTree();
  CvDTree* passTree = new CvDTree();

  // Train the new trees with the old logs and the actions from the current game
  CvDTreeParams params = CvDTreeParams();

  shotTree->train(&shotData,params);
  dribbleTree->train(&dribbleData,params);
  passTree->train(&passData,params);

  // Save the new trees
  // NOTE: These paths MUST be changed every time you play with a different team
  // If the logs are separated by teams you should put here the tree that you trained
  // for the current opponent team.
  shotTree->save("./trainedTrees/shootTree.yml");
  dribbleTree->save("./trainedTrees/dribbleTree.yml");
  passTree->save("./trainedTrees/passTree.yml");

}

/*
  This function determines if a player is a possible ball owner.

  Parameters:
    bx, by: ball's position on the X and Y axes.
    px, py: player's position on the X and Y axes.
    radious: some value referent to the size of the player and the kick_rand associated to his type.

*/
bool isOwner(float bx, float by, float px, float py, float radious){
  float distance = sqrt(pow(bx - px, 2) + pow(by - py, 2));
  return distance < radious;
}


/*
  Returns the nearest teammate to a certain player.

  Parameters:
    teammate1, teammate2, teammate3: vectors that contains the position of the three selected teammates.
    opponent: vector with the position of the opponent player.
*/
double nearestTeammate(Vector2D teammate1, Vector2D teammate2, Vector2D teammate3, Vector2D opponnent){
  double dists[3];
  dists[0] = opponnent.dist(teammate1);
  dists[1] = opponnent.dist(teammate2);
  dists[2] = opponnent.dist(teammate3);

  return *std::min_element(dists,dists+3);

}

/*
  This function is used for the feature extraction of the actions obtained from a game.
  The features will be used for the training of the corresponding decision tree.

  Parameters:
    firstAction: contains the info about the first cycle of the current action. 
    lastAction: contains the info about the last cycle of the current action.
    currentAction: indicates which action is happening at this cycle.
*/
void CoachAgent::extractFeatures(actionInfo firstAction, actionInfo lastAction, std::string currentAction){
  std::ofstream outfile;
  const std::vector<const GlobalPlayerObject*> myPlayers = world().teammates();
  const std::vector<const GlobalPlayerObject*> myOpponents = world().opponents();
  Vector2D posTeammate2, posTeammate3;
  int unumTeammate2 = -1;
  Vector2D posOpponent1, posOpponent2, posOpponent3, posOpponent4;
  int unumOpponent1 = -1, unumOpponent2 = -1, unumOpponent3 = -1;
  int valAction;

  // The output file depends on the type of the action.
  if (currentAction == "PASS"){
    outfile.open("passFile.dat", std::ios_base::app);
    valAction = 1;
  } else if (currentAction == "UNSUCCESFULPASS"){
    outfile.open("passFile.dat", std::ios_base::app);
    valAction = 0;
  } else if (currentAction == "DRIBBLE"){
    outfile.open("dribbleFile.dat", std::ios_base::app);
    valAction = 1;
  } else if (currentAction == "UNSUCCESFULDRIBBLE"){
    outfile.open("dribbleFile.dat", std::ios_base::app);
    valAction = 0;
  } else if (currentAction == "GOAL"){
    outfile.open("shotFile.dat", std::ios_base::app);
    valAction = 1;
  } else if (currentAction == "UNSUCCESFULSHOOT"){
    outfile.open("shotFile.dat", std::ios_base::app);
    valAction = 0;
  }


  //Normalized ball position
  outfile << lastAction.ballPos.x/5 << " " << lastAction.ballPos.y/5 << " ";

  //Distance of the owner from the ball
  outfile << lastAction.ballPos.dist(lastAction.ownerPos) << " ";

  // The second teammate corresponds to the player who is closer to the owner
  float minDist = 10000000000;
  float distAux = 0; 
  for (unsigned int i = 0; i < myPlayers.size(); i++){
    if (myPlayers[i]->unum() != lastAction.ownerUnum){
      distAux = lastAction.ownerPos.dist(myPlayers[i]->pos());
      if (distAux < minDist) {
          minDist = distAux;  
          posTeammate2 = myPlayers[i]->pos();     
          unumTeammate2 = myPlayers[i]->unum();
      }
    }
  }

  outfile << lastAction.ballPos.dist(posTeammate2) << " ";

  // The third teammate corresponds to the player who is closer to the ball's trajectory
  minDist = 10000000000;
  distAux = 0; 
  for (unsigned int i = 0; i < myPlayers.size(); i++){
    if ((myPlayers[i]->unum() != lastAction.ownerUnum) && (myPlayers[i]->unum() != unumTeammate2)){
      distAux = lastAction.ballPos.dist(myPlayers[i]->pos());
      if (distAux < minDist){
          minDist = distAux;   
          posTeammate3 = myPlayers[i]->pos();   
      }
    }
  }

  outfile << lastAction.ballPos.dist(posTeammate3) << " ";

  // The first opponent corresponds to the player who is closer to the owner
  minDist = 10000000000;
  distAux = 0; 
  for (unsigned int i = 0; i < myOpponents.size(); i++){
    if (myOpponents[i]->unum() != lastAction.ownerUnum){
      distAux = lastAction.ownerPos.dist(myOpponents[i]->pos());
      if (distAux < minDist) {
          minDist = distAux;  
          posOpponent1 = myOpponents[i]->pos();     
          unumOpponent1 = myOpponents[i]->unum();
      }
    }
  }
  
  outfile << nearestTeammate(lastAction.ownerPos, posTeammate2, posTeammate3, posOpponent1) << " ";

  // The second opponent corresponds to the player who is closer to last position of the ball
  minDist = 10000000000;
  distAux = 0; 
  for (unsigned int i = 0; i < myOpponents.size(); i++){
    if ((myOpponents[i]->unum() != lastAction.ownerUnum) && (myOpponents[i]->unum() != unumOpponent1)){
      distAux = lastAction.ballPos.dist(myOpponents[i]->pos());
      if (distAux < minDist){
          minDist = distAux;   
          posOpponent2 = myOpponents[i]->pos();
          unumOpponent2 = myOpponents[i]->unum();     
      }
    }
  }

  outfile << nearestTeammate(lastAction.ownerPos, posTeammate2, posTeammate3, posOpponent2) << " ";

  //The third and the fourth opponents corresponds to the players near to the center of action path
  Vector2D middlePoint = Vector2D((lastAction.ballPos.x + firstAction.ballPos.x)/2, (lastAction.ballPos.y + firstAction.ballPos.y)/2);
  minDist = 10000000000;
  distAux = 0; 
  for (int i = 0; i < 2; i++){
    for (unsigned int j = 0; j < myOpponents.size(); j++){
      if ((myOpponents[j]->unum() != unumOpponent1) && (myOpponents[j]->unum() != unumOpponent2) && (myOpponents[j]->unum() != unumOpponent3)){
        distAux = middlePoint.dist(myOpponents[i]->pos());
        if (distAux < minDist){
          minDist = distAux;
          posOpponent3 = myOpponents[i]->pos();
          unumOpponent3 = myOpponents[i]->unum();
        }
      }
    }
    outfile << nearestTeammate(lastAction.ownerPos, posTeammate2, posTeammate3, posOpponent3) << " ";
  }

  // Value associated to the type of the action
  // 1 if it's a positive action
  // 0 if it's negative
  outfile << valAction << std::endl;

  outfile.flush(); 

  outfile.close();

}

/*
  This function determines the ball owner.

*/
actionInfo CoachAgent::ownerPlayer(){
  float radious = 0.8;
  float minDist = 1000;
  float aux;
  actionInfo newAction;
  const std::vector<const GlobalPlayerObject*> myPlayers = world().teammates();
  const std::vector<const GlobalPlayerObject*> myOpponents = world().opponents();
  Vector2D ballPos = world().ball().pos();
  Vector2D ballVel = world().ball().vel();

  newAction.ownerUnum = -1;

  for (unsigned int i = 0; i < myPlayers.size(); i++) {
    aux = pow(ballPos.x - myPlayers[i]->pos().x,2) + pow(ballPos.y - myPlayers[i]->pos().y,2);
    if (aux < minDist){
      newAction.isTeammate = true;
      newAction.ownerUnum = myPlayers[i]->unum();
      minDist = aux;
    }
  }

  for (unsigned int i = 0; i < myOpponents.size();i++) {
    aux = pow(ballPos.x - myOpponents[i]->pos().x,2) + pow(ballPos.y - myOpponents[i]->pos().y,2);
    if (aux < minDist){
      newAction.isTeammate = false;
      newAction.ownerUnum = myOpponents[i]->unum();
      minDist = aux;
    }
  }

  newAction.ballPos.x = ballPos.x;
  newAction.ballPos.y = ballPos.y;
  newAction.ballVel.x = ballVel.x;
  newAction.ballVel.y = ballVel.y;

  if (newAction.ownerUnum != -1){
    if (newAction.isTeammate){
      if (isOwner(ballPos.x, ballPos.y, world().teammate(newAction.ownerUnum)->pos().x, world().teammate(newAction.ownerUnum)->pos().y, radious)){
        return newAction;
      }  
    } else {
      if (isOwner(ballPos.x, ballPos.y, world().opponent(newAction.ownerUnum)->pos().x, world().opponent(newAction.ownerUnum)->pos().y, radious)){
        return newAction;
      }    
    }    
  }
  newAction.ownerUnum = -1;

  return newAction;
}


/*
  This function classifies the actions that have occurred during the match.

  Parameters:
    oldAction: contains the info from the first cycle of the action.
    currentAction: contains the info from the last cycle of the action.
*/
std::string CoachAgent::actionClassifier(actionInfo oldAction, actionInfo currentAction){
  float distance = sqrt(pow(currentAction.ballPos.x - oldAction.ownerPos.x, 2) + pow(currentAction.ballPos.y - oldAction.ownerPos.y, 2));

  // If the ball velocity has changed it means it has moved
  if ((oldAction.ballVel.x != currentAction.ballVel.x) || (oldAction.ballVel.y != currentAction.ballVel.y)) {
    // A different player ownes the ball
    if ((oldAction.ownerUnum != currentAction.ownerUnum) && (currentAction.ownerUnum != -1) && (oldAction.ownerUnum != -1)){
      // Both players are on the same team
      if (oldAction.isTeammate == currentAction.isTeammate){  
        std::cout << "PASS" << std::endl;
        return "PASS";
      }
      // The ball changed teams
      else {    
        // The owner is from our team
        if (oldAction.isTeammate){
          // We're playing on the left side of the field
          if ((world().ourSide() == LEFT) && (currentAction.ballPos.x >= 42.5) && (currentAction.ballPos.y > -10) && (currentAction.ballPos.y < 10) && (oldAction.ballVel.x > 0)){
            std::cout << "UNSUCCESFULSHOOT" << std::endl;
            return "UNSUCCESFULSHOOT";
          } 
          // We're playing on the right side of the field
          else if ((world().ourSide() == RIGHT ) && (currentAction.ballPos.x <= -42.5) && (currentAction.ballPos.y > -10) && (currentAction.ballPos.y < 10) && (oldAction.ballVel.x < 0)){
            std::cout << "UNSUCCESFULSHOOT" << std::endl;
            return "UNSUCCESFULSHOOT";
          }
        }
        // The owner is from the other team 
        else {
          // They're playing on the left side of the field
          if ((world().theirSide() == LEFT) && (currentAction.ballPos.x >= 42.5) && (currentAction.ballPos.y > -10) && (currentAction.ballPos.y < 10) && (oldAction.ballVel.x > 0)){
            std::cout << "UNSUCCESFULSHOOT" << std::endl;
            return "UNSUCCESFULSHOOT";
          } 
          // They're playing on the right side of the field
          else if ((world().theirSide() == RIGHT ) && (currentAction.ballPos.x <= -42.5) && (currentAction.ballPos.y > -10) && (currentAction.ballPos.y < 10) && (oldAction.ballVel.x < 0)){
            std::cout << "UNSUCCESFULSHOOT" << std::endl;
            return "UNSUCCESFULSHOOT";
          }
        }
  
        if (distance < 30){
          std::cout << "UNSUCCESFULDRIBBLE" << std::endl;
          return "UNSUCCESFULDRIBBLE";
        }
        else {
          std::cout << "UNSUCCESFULPASS" << std::endl;
          return "UNSUCCESFULPASS";
        }
      }
    }
    else if ((oldAction.ownerUnum == currentAction.ownerUnum) && (oldAction.isTeammate == currentAction.isTeammate)
                  && (oldAction.ownerUnum != -1)){
      if (distance > 0.5){
        std::cout << "DRIBBLE" << std::endl;
        return "DRIBBLE";
      }
    }
  }
  
  return "";
}

/*
  This function is used to obtain the smallest rectangle that can contain every agent of the opponent 
  team and returns it in a 2x2 matrix.

  Parameters:
    myOpponents : Vector containing the information of each opponent.
*/
float **obtainRectangle(const std::vector<const GlobalPlayerObject*> myOpponents) {
  // Create the rectangle
  float **rectangle = new float*[4];
  for (int row = 0;row < 4;row++){
    rectangle[row] = new float[2];
  }

  float minX = 70;
  float maxX = -70;
  float minY = 70;
  float maxY = -70;

  for (int i = 0;i < myOpponents.size(); i++){
    if (myOpponents[i]->unum() != 1) {
      if (myOpponents[i]->pos().x > maxX) {
        maxX = myOpponents[i]->pos().x;
      }
      if (myOpponents[i]->pos().x < minX) {
        minX = myOpponents[i]->pos().x;
      }
      if (myOpponents[i]->pos().y > maxY) {
        maxY = myOpponents[i]->pos().y;
      }
      if (myOpponents[i]->pos().y < minY) {
        minY = myOpponents[i]->pos().y;
      }
    }
  }

  rectangle[0][0] = minX;
  rectangle[0][1] = maxY;

  rectangle[1][0] = maxX;
  rectangle[1][1] = maxY;

  rectangle[2][0] = minX;
  rectangle[2][1] = minY;

  rectangle[3][0] = maxX;
  rectangle[3][1] = minY;

  return rectangle;


}

/*
  This function is used to map the position of a player from the initial rectangle to the field. 

  Parameters:
    originX,OriginY : Top left corner of the rectangle.
    rectWidth : Width of the rectangle.
    rectHeight : Height of the rectangle.
    x : X coordinate of the position of the agent in the rectangle.
    y : Y coordinate of the position of the agent in the rectangle.
    opponentSide : RIGHT or LEFT depending on the side of the opponent team.
*/
float* mapToField(float originX,float originY,float rectWidth,float rectHeight,float x,float y,SideID opponentSide) {
  float* finalPoint = new float[2];
  float fieldWidth = 104;
  float fieldHeight = 68;
  int side = 1;
  if (opponentSide == RIGHT) {
    side = -1;
  }

  finalPoint[0] = side*((((x - originX)/rectWidth)*fieldWidth) - 52);
  finalPoint[1] = -1*((((y - originY)/rectHeight)*fieldHeight) + 34);

  return finalPoint;
}

/*
  This function uses the mapped positions of each player to the field and adds 1 to the respective area
  to which that player is asociated in the field.

  Parameters:
    field : Matrices for each player that represent the field and its rectangular divisions.
    areaWidth : Width of the rectangular areas in which the field is divided.
    areaHeight : Height of the rectangular areas in which the field is divided.
    mappedPos : Positions of the players in the field (after being mapped from the rectangle).
*/
void sumPositions(float field[10][34][35],float areaWidth,float areaHeight,float** mappedPos) {
  int posX = 0;
  int posY = 0;
  int i = 0;

  for (int j = 0;j < 10;j++){
    if (mappedPos[j][0] != -1) {
      posX = floor((mappedPos[j][0] / areaWidth)+0.5);
      posY = floor((mappedPos[j][1] / areaHeight)+0.5);

      if (posX < 0) {
        posX = posX+18;
      }
      else {
        posX = posX+16;
      }
      if (posY > 0) {
        posY = posY+16;
      }
      else {
        posY = posY+17;
      }

      field[j][posY][posX] = field[j][posY][posX]+1;
      if (posY < 33) {
        field[j][posY+1][posX] = field[j][posY+1][posX]+1;
      }
      if (posY > 0) {
        field[j][posY-1][posX] = field[j][posY-1][posX]+1;
      }
      if (posX < 34) {
        field[j][posY][posX+1] = field[j][posY][posX+1]+1;
      }
      if (posX > 0) {
        field[j][posY][posX-1] = field[j][posY][posX-1]+1;
      }

      if ((posY < 33) and (posX < 34)) {
        field[j][posY+1][posX+1] = field[j][posY+1][posX+1]+1;
      }
      if ((posY > 0) and (posX > 0)) {
        field[j][posY-1][posX-1] = field[j][posY-1][posX-1]+1;
      }
      if ((posY > 0) and (posX < 34)) {
        field[j][posY-1][posX+1] = field[j][posY-1][posX+1]+1;
      }
      if ((posY < 33) and (posX > 0)) {
        field[j][posY+1][posX-1] = field[j][posY+1][posX-1]+1;
      }
    }
  }
}

/*
  This function normalizes the field so that each element is between 0 and 1.

  Parameters:
    field : Matrices for each player that represent the field and its rectangular divisions.
*/
void normalizeField(float field[10][34][35]){
  float maxElems[10];

  for (int i = 0;i<10;i++){
    maxElems[i] = 0;
  }

  for (int k = 0;k < 10;k++){
    for (int i = 0;i < 34;i++){
      for (int j = 0;j<35;j++){
        if (field[k][i][j] > maxElems[k]) {
          maxElems[k] = field[k][i][j];
        }
      }
    }
  }

  bool checks[10];
  for (int i = 0;i < 10;i++){
    checks[i] = false;
  }

  for (int k = 0;k < 10;k++){
    for (int i = 0;i < 34;i++){
      for (int j = 0;j<35;j++){
        field[k][i][j] = (field[k][i][j] / maxElems[k]);
        if ((field[k][i][j] == 1) and (!(checks[k]))) {
          checks[k] = true;
          field[k][i][j] = 2;
        }
      }
    }
  }
}

/*
  This function is used calculate the formation of the enemy team.

  Parameters:
    field : Matrices for each player that represent the field and its rectangular divisions.
    
*/
void calculateFormation(float field[10][34][35],const std::string& opponentName) {
  int offense = 0;
  int center = 0;
  int defense = 0;


  // for (int k = 0; k < 10;k++){
  //   for (int i = 0;i < 34;i++) {
  //     for (int j = 0;j < 35;j++){
  //       std::cout << field[k][i][j] << " ";
  //     }
  //     std::cout << " " << std::endl;
  //   }
  //   std::cout << " " << std::endl;
  // }

  for (int k = 0; k < 10;k++){
    for (int i = 0;i < 34;i++) {
      for (int j = 0;j < 12;j++){
        if (field[k][i][j] == 2){
            defense = defense+1;
        }
      }
      for (int j = 12;j < 24;j++) {
        if (field[k][i][j] == 2){
          center = center+1;
        }
      }
      for (int j = 24;j < 35;j++) {
        if (field[k][i][j] == 2){
          offense = offense+1;
        }
      }
    }
  }

  //std::cout << "FORMATION : " << defense << " " << center << " " << offense << std::endl;
  std::ofstream formationOutFile;
  // std::string fileName = "../../agent2d-3.1.1/src/enemyFormations/formation" + opponentName + ".txt";
  // std::string fileName = "/home/jemd/Documents/USB/Tesis/robocup-coach/agent2d-3.1.1/src/enemyFormations/formation" + opponentName + ".txt";
  // VICKY : Ponle el path completo antes de robocup-coach segun tu computadora
  std::string fileName = "/home/jemd/Documents/USB/Tesis/robocup-coach/agent2d-3.1.1/src/enemyFormations/formation" + opponentName + ".txt";
  formationOutFile.open(fileName.c_str());

  // formationOutFile.open("../../agent2d-3.1.1/src/enemyFormations/formationJaeger.txt");

  if (formationOutFile.is_open()) {
    std::cout << "ESCRIBIENDO" << std::endl;
    formationOutFile << patch::to_string(defense);
    formationOutFile << patch::to_string(center);
    formationOutFile << patch::to_string(offense);
    formationOutFile.close();
  }
}

/*
  This function resets the field after each goal.

  Parameters:
    field : Matrices for each player that represent the field and its rectangular divisions.
    
*/
void resetField(float field[10][34][35]) {
  for (int k = 0; k < 10;k++){
    for (int i = 0;i < 34;i++) {
      for (int j = 0;j < 35;j++){
        field[k][i][j] = 0;
      }
    }
  }  
}

/*
  This function checks if the field  has any elements different from 0.

  Parameters:
    field : Matrices for each player that represent the field and its rectangular divisions.
    
*/
bool checkField(float field[10][34][35]) {
   for (int k = 0; k < 10;k++){
    for (int i = 0;i < 34;i++) {
      for (int j = 0;j < 35;j++){
        if (field[k][i][j] != 0) {
          return false;
        }
      }
    }
  }  
  return true;
}



///////////////////////////////////////////////////////////////////////
/*!
  \struct Impl
  \brief coach agent implementation
*/
struct CoachAgent::Impl {

    //! reference to the agent instance
    CoachAgent & agent_;

    //! flag to check if (think) message was received or not.
    bool think_received_;

    //! flag to check if server cycle is stopped or not.
    bool server_cycle_stopped_;

    //! last action decision game time
    GameTime last_decision_time_;

    //! current game time
    GameTime current_time_;

    //! referee info
    GameMode game_mode_;

    //! visual sensor data
    GlobalVisualSensor visual_;

    //! audio sensor
    CoachAudioSensor audio_;

    /*!
      \brief initialize all members
    */
    Impl( CoachAgent & agent )
        : agent_( agent ),
          think_received_( false ),
          server_cycle_stopped_( true ),
          last_decision_time_( -1, 0 ),
          current_time_( 0, 0 )
      { }

    /*!
      \brief initialize debug resources
     */
    void initDebug();

    /*!
      \brief open offline client log file.
    */
    bool openOfflineLog();

    /*!
      \brief open debug log file.
    */
    bool openDebugLog();

    /*!
      \brief set debug output flags to logger
     */
    void setDebugFlags();

    /*!
      \brief send init or reconnect command to server

      init commad is sent in BasicClient's run() method
      Do not call this method yourself!
     */
    void sendInitCommand();

    /*!
      \brief send client setting commands(compression...) to server
     */
    void sendSettingCommands();


    /*!
      \brief send disconnection command message to server
      and set the server status to end.
     */
    void sendByeCommand();

    /*!
      \brief analyze init replay message
      \param msg raw server message
    */
    void analyzeInit( const char * msg );

    /*!
      \brief analyze cycle info in server message
      \param msg raw server message
      \param by_see_global if message type is see_global, this value becomes true
      \return parsing result status
     */
    bool analyzeCycle( const char * msg,
                       const bool by_see_global );

    /*!
      \brief analyze see_global message
      \param msg raw server message
     */
    void analyzeSeeGlobal( const char * msg );

    /*!
      \brief analyze hear message
      \param msg raw server message
     */
    void analyzeHear( const char * msg );

    /*!
      \brief analyze referee message
      \param msg raw server message
     */
    void analyzeHearReferee( const char * msg );

    /*!
      \brief analyze audio message from player
      \param msg raw server message
    */
    void analyzeHearPlayer( const char * msg );

    /*!
      \brief analyze change_player_type message
      \param msg raw server message
    */
    void analyzeChangePlayerType( const char * msg );

    /*!
      \brief analyze player_type parameter message
      \param msg raw server message
    */
    void analyzePlayerType( const char * msg );

    /*!
      \brief analyze player_param parameter message
      \param msg raw server message
    */
    void analyzePlayerParam( const char * msg );

    /*!
      \brief analyze server_param parameter message
      \param msg raw server message
    */
    void analyzeServerParam( const char * msg );

    /*!
      \brief analyze clang version message
      \param msg raw server message
    */
    void analyzeCLangVer( const char * msg );

    /*!
      \brief analyze score message
      \param msg raw server message
    */
    void analyzeScore( const char * msg );

    /*!
      \brief analyze ok message
      \param msg raw server message
    */
    void analyzeOK( const char * msg );

    /*!
      \brief analyze ok team_graphic_? message
      \param msg raw server message
    */
    void analyzeOKTeamGraphic( const char * msg );

    /*!
      \brief analyze ok teamnames message
      \param msg raw server message
     */
    void analyzeTeamNames( const char * msg );

    /*!
      \brief analyze error message
      \param msg raw server message
    */
    void analyzeError( const char * msg );

    /*!
      \brief analyze warning message
      \param msg raw server message
    */
    void analyzeWarning( const char * msg );

    /*!
      \brief analyze include message
      \param msg raw server message
    */
    void analyzeInclude( const char * msg );

    /*!
      \brief update current time using analyzed time value
      \param new_time analyzed time value
      \param by_see_global true if called after see_global message
    */
    void updateCurrentTime( const long & new_time,
                            const bool by_see_global );

    /*!
      \brief update server game cycle status.

      This method must be called just after referee message
    */
    void updateServerStatus();


    /*!
      \brief output debug messages to disk/server.
     */
    void printDebug();
};


/*-------------------------------------------------------------------*/
/*!

*/
void
CoachAgent::Impl::updateCurrentTime( const long & new_time,
                                     const bool by_see_global )
{
    if ( server_cycle_stopped_ )
    {
        if ( new_time != current_time_.cycle() )
        {
            dlog.addText( Logger::LEVEL_ANY,
                          "CYCLE %ld-0 --------------------"
                          " return from cycle stop",
                          new_time );
            if ( new_time - 1 != current_time_.cycle() )
            {
                std::cerr << "coach: server cycle stopped mode:"
                          << " previous server time is incorrect?? "
                          << current_time_ << " -> " << new_time
                          << std::endl;
                dlog.addText( Logger::SYSTEM,
                              "server cycle stopped mode:"
                              " previous server time is incorrect?? "
                              " (%ld, %ld) -> %ld",
                              current_time_.cycle(), current_time_.stopped(),
                              new_time );
            }

            current_time_.assign( new_time, 0 );
        }
        else
        {
            if ( by_see_global )
            {
                dlog.addText( Logger::LEVEL_ANY,
                              "CYCLE %ld-%ld --------------------"
                              " stopped time was updated by see_global",
                              current_time_.cycle(), current_time_.stopped() + 1 );
                current_time_.assign( current_time_.cycle(),
                                      current_time_.stopped() + 1 );
            }
        }
    }
    // normal case
    else
    {
        if ( current_time_.cycle() != new_time )
        {
            dlog.addText( Logger::LEVEL_ANY,
                          "CYCLE %ld-0  -------------------------------------------------",
                          new_time );
        }
        current_time_.assign( new_time, 0 );
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
CoachAgent::Impl::updateServerStatus()
{
    // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    //  called just after referee.parse()

    // if current mode is stopped mode,
    // stopped flag is cleared.
    if ( server_cycle_stopped_ )
    {
        server_cycle_stopped_ = false;
    }

    if ( game_mode_.isServerCycleStoppedMode() )
    {
        server_cycle_stopped_ = true;
    }
}

///////////////////////////////////////////////////////////////////////

/*-------------------------------------------------------------------*/
/*!

*/
CoachAgent::CoachAgent()
    : SoccerAgent()
    , M_impl( new Impl( *this ) )
    , M_debug_client()
    , M_worldmodel()
{
    boost::shared_ptr< AudioMemory > audio_memory( new AudioMemory );

    M_worldmodel.setAudioMemory( audio_memory );
}

/*-------------------------------------------------------------------*/
/*!

*/
CoachAgent::~CoachAgent()
{

}

/*-------------------------------------------------------------------*/
/*!

*/
const
GlobalVisualSensor &
CoachAgent::visualSensor() const
{
    return M_impl->visual_;
}

/*-------------------------------------------------------------------*/
/*!

*/
const
CoachAudioSensor &
CoachAgent::audioSensor() const
{
    return M_impl->audio_;
}

/*-------------------------------------------------------------------*/
/*!

*/
bool
CoachAgent::initImpl( CmdLineParser & cmd_parser )
{
    bool help = false;
    std::string coach_config_file;

    ParamMap system_param_map( "System options" );
    system_param_map.add()
        ( "help" , "", BoolSwitch( &help ), "print help message.")
        ( "coach-config", "", &coach_config_file, "specifies coach config file." );

    ParamMap coach_param_map( "Coach options" );
    M_config.createParamMap( coach_param_map );

    // analyze command line for system options
    cmd_parser.parse( system_param_map );
    if ( help )
    {
        std::cout << copyright() << std::endl;
        system_param_map.printHelp( std::cout );
        coach_param_map.printHelp( std::cout );
        return false;
    }

    // analyze config file for coach config options
    if ( ! coach_config_file.empty() )
    {
        ConfFileParser conf_parser( coach_config_file.c_str() );
        conf_parser.parse( coach_param_map );
    }

    // analyze command line for coach options
    cmd_parser.parse( coach_param_map );

    if ( config().version() < 1.0
         || 15.0 <= config().version() )
    {
        std::cerr << "Unsupported client version: " << config().version()
                  << std::endl;
        return false;
    }

    M_impl->setDebugFlags();

    if ( config().offlineClientMode() )
    {
        M_client->setClientMode( BasicClient::OFFLINE );
    }

    return true;
}

/*-------------------------------------------------------------------*/
/*!

*/
bool
CoachAgent::handleStart()
{
    if ( ! M_client )
    {
        return false;
    }

    if ( config().host().empty() )
    {
        std::cerr << config().teamName()
                  << " coach: ***ERROR*** coach: server host name is empty"
                  << std::endl;
        M_client->setServerAlive( false );
        return false;
    }

    // just create a connection. init command is automaticaly sent
    // by BasicClient's run() method.
    if ( ! M_client->connectTo( config().host().c_str(),
                                config().port(),
                                static_cast< long >( config().intervalMSec() ) ) )
    {
        std::cerr << config().teamName()
                  << " coach: ***ERROR*** Failed to connect." << std::endl;
        M_client->setServerAlive( false );
        return false;
    }

    if ( config().offlineLogging() )
    {
        if ( ! M_impl->openOfflineLog() )
        {
            return false;
        }
    }

    M_impl->sendInitCommand();
    return true;
}

/*-------------------------------------------------------------------*/
/*!

*/
bool
CoachAgent::handleStartOffline()
{
    if ( ! M_client )
    {
        return false;
    }

    if ( ! M_impl->openOfflineLog() )
    {
        return false;
    }

    M_client->setServerAlive( true );
    return true;
}

/*-------------------------------------------------------------------*/
/*!

*/
void
// CoachAgent::handleMessage(actionInfo* firstAction, actionInfo* lastAction)
CoachAgent::handleMessage(actionInfo* firstAction, actionInfo* lastAction,float field[10][34][35])
{
    if ( ! M_client )
    {
        std::cerr << "CoachAgent::handleMessage(). Client is not registered."
                  << std::endl;
        return;
    }
    
    int counter = 0;
    GameTime start_time = M_impl->current_time_;

    old_opp_score = opp_score;
    our_score = ( world().ourSide() == LEFT
                      ? world().gameMode().scoreLeft()
                      : world().gameMode().scoreRight() );
    opp_score = ( world().ourSide() == LEFT
                      ? world().gameMode().scoreRight()
                      : world().gameMode().scoreLeft() );
    
    /*actionInfo newAction = ownerPlayer();

    if (newAction.ownerUnum != -1){
      lastAction->ownerUnum = newAction.ownerUnum;
      lastAction->isTeammate = newAction.isTeammate;
      lastAction->ownerPos = newAction.ownerPos;
    }

    lastAction->ballPos = newAction.ballPos;
    lastAction->ballVel = newAction.ballVel;

    std::string currentAction = actionClassifier(*firstAction, *lastAction);

    
    // std::cout << &field[9][33][34] << std::endl;
    if ((world().gameMode().type() == GameMode::AfterGoal_) && (firstAction->goalChecked == false)){
      std::cout << "GOAL" << std::endl;
      currentAction = "GOAL";
      firstAction->goalChecked = true;
    }

    if ((world().gameMode().type() != GameMode::AfterGoal_) && firstAction->goalChecked){
      firstAction->goalChecked = false;
    }

    if (currentAction != ""){
      extractFeatures(*firstAction, *lastAction, currentAction);
    }

    firstAction->ownerUnum = lastAction->ownerUnum;
    firstAction->isTeammate = lastAction->isTeammate;
    firstAction->ownerPos = lastAction->ownerPos;
    firstAction->ballPos = lastAction->ballPos;
    firstAction->ballVel = lastAction->ballVel;

    if (world().gameMode().type() == GameMode::KickIn_){
      std::cout << "Estoy reentrenando" << std::endl;
      trainTrees();
    }*/


    // Formation Stuff
    //if ((world().gameMode().type() == GameMode::AfterGoal_)){
    if ((old_opp_score + 1 == opp_score) && (world().time().cycle() > 1750)) {
      if (!(checkField(field))) {
        std::cout << "Reseteando y normalizando!" << std::endl;
        normalizeField(field);
        calculateFormation(field,M_worldmodel.theirTeamName());
        resetField(field);
        lastReset = world().time().cycle();
      }
    }

    if (world().gameMode().type() == GameMode::PlayOn) {
      float fieldAreaWidth = 2.971428571;
      float fieldAreaHeight = 2;
      const std::vector<const GlobalPlayerObject*> myOpponents = world().opponents();
      float **rectangle = obtainRectangle(myOpponents);

      // std::cout << rectangle[0][0] << "    " << rectangle[0][1] << std::endl;
      // std::cout << rectangle[1][0] << "    " << rectangle[1][1] << std::endl;

      float rectWidth = rectangle[1][0] - rectangle[0][0];
      float rectHeight = rectangle[0][1] - rectangle[2][1];

      float **mappedPos = new float*[10];

      // Initialize mappedPos
      for (int row = 0;row < 10;row++){
        mappedPos[row] = new float[2];
        for (int col = 0; col < 2;col++){
          mappedPos[row][col] = -1;
        }
      }


      for (int i = 0; i < myOpponents.size(); i++){
        if (myOpponents[i]->unum() >= 2) {
          mappedPos[myOpponents[i]->unum()-2] = mapToField(rectangle[0][0],rectangle[0][1],rectWidth,rectHeight,
            myOpponents[i]->pos().x,myOpponents[i]->pos().y,world().theirSide());
          // std::cout << mappedPos[myOpponents[i]->unum()-2][0] << " " << mappedPos[myOpponents[i]->unum()-2][1] << std::endl;
        }
      }


      sumPositions(field,fieldAreaWidth,fieldAreaHeight,mappedPos);

      // std::cout << "Estoy funcionando!" << std::endl;
      free(rectangle);
      free(mappedPos);
    }


    // receive and analyze message
    while ( M_client->recvMessage() > 0 )
    {
        ++counter;
        parse( M_client->message() );
    }

    if ( M_impl->current_time_.cycle() > start_time.cycle() + 1
         && start_time.stopped() == 0
         && M_impl->current_time_.stopped() == 0 )
    {
        std::cerr << config().teamName()
                  << " coach: parser used several steps -- missed an action!  received"
                  << counter << " messages     start time=" << start_time
                  << " end time=" << M_impl->current_time_
                  << std::endl;
    }


    if ( M_impl->think_received_ )
    {
        action();
    }
#if 0
    else if ( ! ServerParam::i().synchMode() )
    {
        if ( M_impl->last_decision_time_ != M_impl->current_time_
             && M_impl->visual_.time() == M_impl->current_time_ )
        {
            action();
        }
    }
#endif
}

/*-------------------------------------------------------------------*/
/*!

*/
void
CoachAgent::handleMessageOffline()
{
    if ( ! M_client )
    {
        std::cerr << "CoachAgent::handleMessageOffline(). Client is not registered."
                  << std::endl;
        return;
    }

    if ( M_client->recvMessage() > 0 )
    {
        parse( M_client->message() );
    }

    if ( M_impl->think_received_ )
    {
        dlog.addText( Logger::SYSTEM,
                      __FILE__": Got think message: decide action" );
#if 0
        std::cout << config().teamName() << " coach: "
                  << world().time()
                  << " action" << std::endl;
#endif
        action();
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
CoachAgent::handleTimeout( const int /*timeout_count*/,
                           const int waited_msec )
{
    if ( ! M_client )
    {
        std::cerr << "CoachAgent::handleTimeout(). Client is not registered."
                  << std::endl;
        return;
    }

    if ( waited_msec > config().serverWaitSeconds() * 1000 )
    {
        if ( config().useEye() )
        {
            std::cout << config().teamName()
                      << " coach: waited "
                      << waited_msec / 1000
                      << " seconds. server down??" << std::endl;
            M_client->setServerAlive( false );
            return;
        }

        if ( waited_msec > config().serverWaitSeconds() * 2 * 1000 )
        {
            std::cout << config().teamName()
                      << " coach: waited "
                      << waited_msec / 1000
                      << " seconds. server down??" << std::endl;
            M_client->setServerAlive( false );
            return;
        }

        //std::cout << config().teamName() << ": coach requests server response..."
        //<< std::endl;
        doCheckBall();
    }


    if ( M_impl->last_decision_time_ != M_impl->current_time_ )
    {
        if ( M_impl->visual_.time() == M_impl->current_time_
             || waited_msec >= 20 * ServerParam::i().slowDownFactor() )
        {
            dlog.addText( Logger::SYSTEM,
                          "----- TIMEOUT DECISION !! [%d]ms from last sensory",
                          waited_msec );
            action();
        }
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
CoachAgent::handleExit()
{
    finalize();
}

/*-------------------------------------------------------------------*/
/*!

*/
void
CoachAgent::addSayMessageParser( boost::shared_ptr< SayMessageParser > parser )
{
    M_impl->audio_.addParser( parser );
}

/*-------------------------------------------------------------------*/
/*!

*/
void
CoachAgent::removeSayMessageParser( const char header )
{
    M_impl->audio_.removeParser( header );
}

/*-------------------------------------------------------------------*/
/*!

*/
void
CoachAgent::finalize()
{
    if ( M_client->isServerAlive() )
    {
        M_impl->sendByeCommand();
    }
    std::cout << config().teamName() << " coach: finished."
              << std::endl;
}

/*-------------------------------------------------------------------*/
/*!

 */
void
CoachAgent::Impl::initDebug()
{
    if ( agent_.M_client->clientMode() == BasicClient::ONLINE )
    {
        if ( agent_.config().debugServerConnect() )
        {
            agent_.M_debug_client.connect( agent_.config().debugServerHost(),
                                           agent_.config().debugServerPort() );
        }

        if ( agent_.config().offlineLogging() )
        {
            openOfflineLog();
        }
    }

    if ( agent_.config().debug() )
    {
        openDebugLog();
    }

    if ( agent_.config().debugServerLogging() )
    {
        agent_.M_debug_client.open( agent_.config().logDir(),
                                    agent_.config().teamName() );
    }
}

/*-------------------------------------------------------------------*/
/*!

 */
bool
CoachAgent::Impl::openOfflineLog()
{
    std::string filepath = agent_.config().logDir();

    if ( ! filepath.empty() )
    {
        if ( *filepath.rbegin() != '/' )
        {
            filepath += '/';
        }
    }

    filepath += agent_.config().teamName();
    filepath += "-coach";
    filepath += agent_.config().offlineLogExt();

    if ( ! agent_.M_client->openOfflineLog( filepath ) )
    {
        std::cerr << "Failed to open the offline client log file ["
                  << filepath
                  << "]" << std::endl;
        agent_.M_client->setServerAlive( false );
        return false;
    }

    return true;
}

/*-------------------------------------------------------------------*/
/*!

 */
bool
CoachAgent::Impl::openDebugLog()
{
    std::string filepath = agent_.config().logDir();

    if ( ! filepath.empty() )
    {
        if ( *filepath.rbegin() != '/' )
        {
            filepath += '/';
        }
    }

    filepath += agent_.config().teamName();
    filepath += "-coach";
    filepath += agent_.config().debugLogExt();

    dlog.open( filepath );

    if ( ! dlog.isOpen() )
    {
        std::cerr << agent_.config().teamName() << " coach: "
                  << " Failed to open the debug log file [" << filepath << "]"
                  << std::endl;
        agent_.M_client->setServerAlive( false );
        return false;
    }

    return true;
}

/*-------------------------------------------------------------------*/
/*!

 */
void
CoachAgent::Impl::setDebugFlags()
{
    const CoachConfig & c = agent_.config();

    if ( ! c.debug() )
    {
        return;
    }

    dlog.setLogFlag( &current_time_, Logger::SYSTEM, c.debugSystem() );
    dlog.setLogFlag( &current_time_, Logger::SENSOR, c.debugSensor() );
    dlog.setLogFlag( &current_time_, Logger::WORLD, c.debugWorld() );
    dlog.setLogFlag( &current_time_, Logger::ACTION, c.debugAction() );
    dlog.setLogFlag( &current_time_, Logger::INTERCEPT, c.debugIntercept() );
    dlog.setLogFlag( &current_time_, Logger::KICK, c.debugKick() );
    dlog.setLogFlag( &current_time_, Logger::HOLD, c.debugHold() );
    dlog.setLogFlag( &current_time_, Logger::DRIBBLE, c.debugDribble() );
    dlog.setLogFlag( &current_time_, Logger::PASS, c.debugPass() );
    dlog.setLogFlag( &current_time_, Logger::CROSS, c.debugCross() );
    dlog.setLogFlag( &current_time_, Logger::SHOOT, c.debugShoot() );
    dlog.setLogFlag( &current_time_, Logger::CLEAR, c.debugClear() );
    dlog.setLogFlag( &current_time_, Logger::BLOCK, c.debugBlock() );
    dlog.setLogFlag( &current_time_, Logger::MARK, c.debugMark() );
    dlog.setLogFlag( &current_time_, Logger::POSITIONING, c.debugPositioning() );
    dlog.setLogFlag( &current_time_, Logger::ROLE, c.debugRole() );
    dlog.setLogFlag( &current_time_, Logger::PLAN, c.debugPlan() );
    dlog.setLogFlag( &current_time_, Logger::TEAM, c.debugTeam() );
    dlog.setLogFlag( &current_time_, Logger::COMMUNICATION, c.debugCommunication() );
    dlog.setLogFlag( &current_time_, Logger::ANALYZER, c.debugAnalyzer() );
    dlog.setLogFlag( &current_time_, Logger::ACTION_CHAIN, c.debugActionChain() );
}

/*-------------------------------------------------------------------*/
/*!

*/
void
CoachAgent::action()
{
    if ( config().offlineLogging()
         && ! ServerParam::i().synchMode() )
    {
        M_client->printOfflineThink();
    }

    M_worldmodel.updateJustBeforeDecision( M_impl->current_time_ );

    if ( M_impl->last_decision_time_ != M_impl->current_time_ )
    {
        actionImpl();
        M_impl->last_decision_time_ = M_impl->current_time_;
    }

    if ( M_impl->think_received_ )
    {
        CoachDoneCommand com;
        sendCommand( com );
        M_impl->think_received_ = false;
    }


    M_impl->printDebug();
}

/*-------------------------------------------------------------------*/
/*!

*/
void
CoachAgent::parse( const char * msg )
{
    if ( ! std::strncmp( msg, "(see_global ", 12 ) )
    {
        M_impl->analyzeSeeGlobal( msg );
    }
#if 0
    else if ( ! std::strncmp( msg, "(see ", 5 ) )
    {
        M_impl->analyzeSeeGlobal( msg );
    }
#endif
    else if ( ! std::strncmp( msg, "(hear ", 6 ) )
    {
        M_impl->analyzeHear( msg );
    }
    else if ( ! std::strncmp( msg, "(think)", 7 ) )
    {
        M_impl->think_received_ = true;
    }
    else if ( ! std::strncmp( msg, "(change_player_type ", 20 ) )
    {
        M_impl->analyzeChangePlayerType( msg );
    }
    else if ( ! std::strncmp( msg, "(clang ", 7 ) )
    {
        M_impl->analyzeCLangVer( msg );
    }
    else if ( ! std::strncmp( msg, "(player_type ", 13 ) )  // hetero param
    {
        M_impl->analyzePlayerType( msg );
    }
    else if ( ! std::strncmp( msg, "(player_param ", 14 ) ) // tradeoff param
    {
        M_impl->analyzePlayerParam( msg );
    }
    else if ( ! std::strncmp( msg, "(server_param ", 14 ) )
    {
        M_impl->analyzeServerParam( msg );
    }
    else if ( ! std::strncmp( msg, "(ok ", 4 ) )
    {
        M_impl->analyzeOK( msg );
    }
    else if ( ! std::strncmp( msg, "(error ", 7 ) )
    {
        M_impl->analyzeError( msg );
    }
    else if ( ! std::strncmp( msg, "(warning ", 9 ) )
    {
        M_impl->analyzeWarning( msg );
    }
    else if ( ! std::strncmp( msg, "(score ", 7 ) )
    {
        M_impl->analyzeScore( msg );
    }
    else if ( ! std::strncmp( msg, "(init ", 6 ) )
    {
        M_impl->analyzeInit( msg );
    }
    else if ( ! std::strncmp( msg, "(include ", 9 ) )
    {
        M_impl->analyzeInclude( msg );
    }
    else
    {
        std::cerr << config().teamName()
                  << " coach: "
                  << world().time()
                  << " received unsupported Message : [" << msg << "]"
                  << std::endl;
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
CoachAgent::Impl::analyzeInit( const char * msg )
{
    // "(init l ok)" | "(init r ok)"

    const CoachConfig & c = agent_.config();

    char side;

    if ( std::sscanf( msg, "(init %c ok)", &side ) != 1 )
    {
        agent_.M_client->setServerAlive( false );
        return;
    }

    if ( side != 'l' && side != 'r' )
    {
        std::cerr << c.teamName()
                  << " coach: "
                  << agent_.world().time()
                  << " received unexpected init message. " << msg
                  << std::endl;
        agent_.M_client->setServerAlive( false );
        return;
    }

    // inititalize member variables
    SideID side_id = ( side == 'l'
                       ? LEFT
                       : RIGHT );
    agent_.M_worldmodel.init( side_id, c.version() );

    // send specific settings
    // if we intend to advise to the team,
    // set visual sensory
    if ( c.useEye() )
    {
        agent_.doEye( true );
    }

    if ( c.hearSay() )
    {
        audio_.setTeamName( c.teamName() );
    }


    //
    // initialize debug resources
    //

    initDebug();

    //
    // send client setting commands
    //

    sendSettingCommands();
}

/*-------------------------------------------------------------------*/
/*!

*/
bool
CoachAgent::Impl::analyzeCycle( const char * msg,
                          const bool by_see_global )
{
    char id[16];
    long cycle = 0;
    if ( std::sscanf( msg, "(%15s %ld ",
                      id, &cycle ) != 2 )
    {
        std::cerr << agent_.config().teamName()
                  << " coach:"
                  << agent_.world().time()
                  << " ***ERROR*** failed to parse time msg=["
                  << msg << "]"
                  << std::endl;
        return false;
    }

    updateCurrentTime( cycle, by_see_global );
    return true;
}

/*-------------------------------------------------------------------*/
/*!

*/
void
CoachAgent::Impl::analyzeSeeGlobal( const char * msg )
{
    if ( ! analyzeCycle( msg, true ) )
    {
        return;
    }

    // analyze message
    visual_.parse( msg,
                   agent_.config().version(),
                   current_time_ );

    // update world model
    if ( visual_.time() == current_time_ )
    {
        agent_.M_worldmodel.updateAfterSeeGlobal( visual_,
                                                  current_time_ );
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
CoachAgent::Impl::analyzeHear( const char * msg )
{
    if ( ! analyzeCycle( msg, false ) )
    {
        return;
    }

    long cycle;
    char sender[128];

    if ( std::sscanf( msg, "(hear %ld %127s ",
                      &cycle, sender ) != 2 )
    {
        std::cerr << agent_.config().teamName()
                  << " coach: "
                  << agent_.world().time()
                  << " Error. failed to parse audio sender. ["
                  << msg
                  << std::endl;
        return;
    }

    // check sender name
    if ( ! std::strcmp( sender, "referee" ) )
    {
        analyzeHearReferee( msg );
    }
    else if ( ! std::strncmp( sender, "(p", 2 ) )
    {
        // (hear <time> (player "<teamname>" <unum>) "<message>")
        // (hear <time> (p "<teamname>" <unum>) "<message>")
        analyzeHearPlayer( msg );
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
CoachAgent::Impl::analyzeHearReferee( const char * msg )
{
    const CoachConfig & c = agent_.config();
    const GlobalWorldModel & wm = agent_.world();

    long cycle;
    char mode[512];

    if ( std::sscanf( msg, "(hear %ld referee %511[^)]", &cycle, mode ) != 2 )
    {
        std::cerr << c.teamName()
                  << " coach: "
                  << wm.time()
                  << " ***ERROR*** Failed to scan playmode." << msg
                  << std::endl;
        return;
    }

    if ( ! game_mode_.update( mode, current_time_ ) )
    {
        if ( ! std::strncmp( mode, "yellow_card", std::strlen( "yellow_card" ) ) )
        {
            char side = '?';
            int unum = Unum_Unknown;
            if ( std::sscanf( mode, "yellow_card_%c_%d", &side, &unum ) != 2 )
            {
                std::cerr << c.teamName()
                          << " coach: "
                          << wm.time()
                          << " could not parse the yellow card message [" << msg << ']'
                          << std::endl;
            }

            agent_.M_worldmodel.setYellowCard( ( side == 'l'
                                                 ? LEFT
                                                 : side == 'r'
                                                 ? RIGHT
                                                 : NEUTRAL ),
                                               unum );
        }
        else if ( ! std::strncmp( mode, "red_card", std::strlen( "red_card" ) ) )
        {
            char side = '?';
            int unum = Unum_Unknown;
            if ( std::sscanf( mode, "red_card_%c_%d", &side, &unum ) != 2 )
            {
                std::cerr << c.teamName()
                          << " coach: "
                          << wm.time()
                          << " could not parse the red card message [" << msg << ']'
                          << std::endl;
            }

            agent_.M_worldmodel.setRedCard( ( side == 'l'
                                              ? LEFT
                                              : side == 'r'
                                              ? RIGHT
                                              : NEUTRAL ),
                                            unum );
        }
        else if ( ! std::strncmp( mode, "training", std::strlen( "training" ) ) )
        {
            // end keepaway (or some training) episode
            agent_.M_worldmodel.setTrainingTime( current_time_ );
            return;
        }
        else
        {
            std::cerr << c.teamName() << " coach: "
                      << wm.time()
                      << " Unknown playmode string." << mode
                      << std::endl;
        }

        return;
    }

    updateServerStatus();

    if ( game_mode_.isGameEndMode() )
    {
        sendByeCommand();
        return;
    }

    agent_.M_worldmodel.updateGameMode( game_mode_, current_time_ );
}

/*-------------------------------------------------------------------*/
/*!

*/
void
CoachAgent::Impl::analyzeHearPlayer( const char * msg )
{
    // (hear <time> (player "<teamname>" <unum>) "<message>")
    // (hear <time> (p "<teamname>" <unum>) "<message>")

    if ( agent_.config().hearSay() )
    {
        audio_.parsePlayerMessage( msg, current_time_ );
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
CoachAgent::Impl::analyzeChangePlayerType( const char * msg )
{
    // teammate: "(change_player_type <unum> <type>)\n"
    //           "(ok change_player_type <unum> <type>)\n"
    // opponent: "(change_player_type <unum>)\n"

    int unum = -1, type = -1;

    if ( std::sscanf( msg, " ( ok change_player_type %d %d ) ",
                      &unum, &type ) == 2 )
    {
        // do nothing
    }
    else if ( std::sscanf( msg, " ( change_player_type %d %d ) ",
                           &unum, &type ) == 2 )
    {
        // teammate
        agent_.M_worldmodel.setPlayerType( agent_.world().ourSide(),
                                           unum,
                                           type );
    }
    else if ( std::sscanf( msg, " ( change_player_type %d ) ",
                           &unum ) == 1 )
    {
        // opponent
        agent_.M_worldmodel.setPlayerType( agent_.world().theirSide(),
                                           unum,
                                           Hetero_Unknown );
    }
    else
    {
        std::cerr << " ***ERROR*** parse error. "
                  << msg
                  << std::endl;
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
CoachAgent::Impl::analyzePlayerType( const char * msg )
{
    PlayerType player_type( msg, agent_.config().version() );
    PlayerTypeSet::instance().insert( player_type );

    agent_.handlePlayerType();
}

/*-------------------------------------------------------------------*/
/*!

*/
void
CoachAgent::Impl::analyzePlayerParam( const char * msg )
{
    PlayerParam::instance().parse( msg, agent_.config().version() );
    //PlayerParam::i().print( std::cout );

    agent_.M_worldmodel.setPlayerParam();
    agent_.handlePlayerParam();
}

/*-------------------------------------------------------------------*/
/*!

*/
void
CoachAgent::Impl::analyzeServerParam( const char * msg )
{
    ServerParam::instance().parse( msg, agent_.config().version() );
    PlayerTypeSet::instance().resetDefaultType();

    if ( ! ServerParam::i().synchMode()
         && ServerParam::i().slowDownFactor() > 1 )
    {
        long interval = ( agent_.config().intervalMSec()
                          * ServerParam::i().slowDownFactor() );
        agent_.M_client->setIntervalMSec( interval );
    }

    agent_.M_worldmodel.setServerParam();
    agent_.handleServerParam();
}

/*-------------------------------------------------------------------*/
/*!

*/
void
CoachAgent::Impl::analyzeCLangVer( const char * )
{
    //     std::cerr << config().teamName()
    //               << " coach: "
    //               << world().time()
    //               << " recv: " << msg << std::endl;
}

/*-------------------------------------------------------------------*/
/*!

*/
void
CoachAgent::Impl::analyzeOK( const char * msg )
{
    //std::cout << M_impl->current_time_
    //<< " Coach received ok " << std::endl;

    if ( ! std::strncmp( msg, "(ok say)",
                         std::strlen( "(ok say)" ) ) )
    {
        // nothing to do
    }
    else if ( ! std::strncmp( msg, "(ok team_graphic ",
                              std::strlen( "(ok team_graphic " ) ) )
    {
        analyzeOKTeamGraphic( msg );
    }
    else if ( ! std::strncmp( msg, "(ok look ",
                              std::strlen( "(ok look " ) ) )
    {
        std::cout << agent_.config().teamName()
                  << " coach: "
                  << agent_.world().time()
                  << "recv (ok look ..." << std::endl;
    }
    else if ( ! std::strncmp( msg, "(ok check_ball ",
                              std::strlen( "(ok check_ball " ) ) )
    {
        std::cout << agent_.config().teamName()
                  << " coach: "
                  << agent_.world().time()
                  << " recv (ok check_ball ..." << std::endl;;
    }
    else if ( ! std::strncmp( msg, "(ok change_player_type ",
                              std::strlen( "(ok change_player_type " ) ) )
    {
        analyzeChangePlayerType( msg );
    }
    else if ( ! std::strncmp( msg, "(ok compression ",
                              std::strlen( "(ok compression " ) ) )
    {
        int level = 0;
        if ( std::sscanf( msg, "(ok compression %d)", &level ) == 1 )
        {
            std::cout << agent_.config().teamName()
                      << " coach: "
                      << agent_.world().time()
                      << " set compression level " << level<< std::endl;
            agent_.M_client->setCompressionLevel( level );
        }
    }
    else if ( ! std::strncmp( msg, "(ok eye ", std::strlen( "(ok eye " ) ) )
    {
        std::cout << agent_.config().teamName()
                  << " coach: "
                  << agent_.world().time()
                  << " recv " << msg << std::endl;
    }
    else if ( ! std::strncmp( msg, "(ok team_names ",
                              std::strlen( "(ok team_names " ) ) )
    {
        std::cout << agent_.config().teamName()
                  << " coach: "
                  << agent_.world().time()
                  << " recv " << msg << std::endl;
        analyzeTeamNames( msg );
    }
    else
    {
        std::cout << agent_.config().teamName()
                  << " coach: "
                  << agent_.world().time()
                  << " recv " << msg << std::endl;
    }
}

/*-------------------------------------------------------------------*/
/*!

*/
void
CoachAgent::Impl::analyzeOKTeamGraphic( const char * msg )
{
    // "(ok team_graphic <x> <y>)"
    // "(ok team_graphic <x> <y>)"

    int x = -1, y = -1;

    if ( std::sscanf( msg,
                      "(ok team_graphic %d %d)",
                      &x, &y ) != 2
         || x < 0
         || y < 0 )
    {
        std::cout << agent_.config().teamName()
                  << " coach: "
                  << agent_.world().time()
                  << " recv illegal message. " << msg
                  << std::endl;
        return;
    }

    agent_.M_team_graphic_ok_set.insert( TeamGraphic::Index( x, y ) );
}

/*-------------------------------------------------------------------*/
/*!

*/
void
CoachAgent::Impl::analyzeTeamNames( const char * msg )
{
    // "(ok team_names (team l <name>)[ (team r <name>)])"

    char left[32], right[32];

    int n = std::sscanf( msg,
                         "(ok team_names (team l %31[^)]) (team r %31[^)]))",
                         left, right );
    if ( n == 2 )
    {
        agent_.M_worldmodel.setTeamName( LEFT, left );
        agent_.M_worldmodel.setTeamName( RIGHT, right );
    }
    else if ( n == 1 )
    {
        agent_.M_worldmodel.setTeamName( LEFT, left );
    }
}


/*-------------------------------------------------------------------*/
/*!

*/
void
CoachAgent::Impl::analyzeScore( const char * msg )
{
    std::cerr << agent_.config().teamName()
              << " coach: "
              << agent_.world().time()
              << " recv " << msg << std::endl;
}

/*-------------------------------------------------------------------*/
/*!

*/
void
CoachAgent::Impl::analyzeError( const char * msg )
{
    std::cerr << agent_.config().teamName()
              << " coach: "
              << agent_.world().time()
              << " recv " << msg << std::endl;
}

/*-------------------------------------------------------------------*/
/*!

*/
void
CoachAgent::Impl::analyzeWarning( const char * msg )
{
    std::cerr << agent_.config().teamName()
              << " coach: "
              << agent_.world().time()
              << " recv " << msg
              << std::endl;
}

/*-------------------------------------------------------------------*/
/*!

*/
void
CoachAgent::Impl::analyzeInclude( const char * msg )
{
    std::cerr << agent_.config().teamName()
              << " coach: "
              << agent_.world().time()
              << " recv " << msg << std::endl;
}

/*-------------------------------------------------------------------*/
/*!

*/
bool
CoachAgent::sendCommand( const CoachCommand & com )
{
    std::ostringstream os;
    com.toStr( os );

    std::string str = os.str();
    if ( str.length() == 0 )
    {
        return false;
    }

    return ( M_client->sendMessage( str.c_str() ) > 0 );
}

/*-------------------------------------------------------------------*/
/*!

*/
void
CoachAgent::Impl::sendInitCommand()
{
    if ( ! agent_.M_client->isServerAlive() )
    {
        std::cerr << agent_.config().teamName()
                  << " coach: server is not alive" << std::endl;
        return;
    }

    // make command string
    bool success = true;
    if ( agent_.config().useCoachName()
         && ! agent_.config().coachName().empty() )
    {
        CoachInitCommand com( agent_.config().teamName(),
                              agent_.config().version(),
                              agent_.config().coachName() );
        success = agent_.sendCommand( com );
    }
    else
    {
        CoachInitCommand com( agent_.config().teamName(),
                              agent_.config().version() );
        success = agent_.sendCommand( com );
    }

    if ( ! success )
    {
        std::cerr << agent_.config().teamName()
                  << " coach: Failed to init coach...\nExit ..."
                  << std::endl;
        agent_.M_client->setServerAlive( false );
    }
}

/*-------------------------------------------------------------------*/
/*!

 */
void
CoachAgent::Impl::sendSettingCommands()
{
    std::ostringstream ostr;

    // set compression level
    if ( 0 < agent_.config().compression()
         && agent_.config().compression() <= 9 )
    {
        CoachCompressionCommand com( agent_.config().compression() );
        com.toStr( ostr );
    }

    if ( ! ostr.str().empty() )
    {
        dlog.addText( Logger::SYSTEM,
                      "---- send[%s]",
                      ostr.str().c_str() );
        agent_.M_client->sendMessage( ostr.str().c_str() );
    }
}

/*-------------------------------------------------------------------*/
/*!

 */
void
CoachAgent::Impl::printDebug()
{
    if ( agent_.config().debugServerConnect()
         || agent_.config().debugServerLogging() )
    {
        // compose worldmodel & some additional debug messages
        // send to debug server or write to disk
        agent_.M_debug_client.writeAll( agent_.world() );
    }
    else
    {
        agent_.M_debug_client.clear();
    }

    dlog.flush();
}

/*-------------------------------------------------------------------*/
/*!

*/
void
CoachAgent::Impl::sendByeCommand()
{
    CoachByeCommand com;
    agent_.sendCommand( com );
    agent_.M_client->setServerAlive( false );
}

/*-------------------------------------------------------------------*/
/*!

*/
bool
CoachAgent::doCheckBall()
{
    CoachCheckBallCommand com;
    return sendCommand( com );
}

/*-------------------------------------------------------------------*/
/*!

*/
bool
CoachAgent::doLook()
{
    CoachLookCommand com;
    return sendCommand( com );
}

/*-------------------------------------------------------------------*/
/*!

*/
bool
CoachAgent::doTeamNames()
{
    CoachTeamNamesCommand com;
    return sendCommand( com );
}

/*-------------------------------------------------------------------*/
/*!

*/
bool
CoachAgent::doEye( bool on )
{
    CoachEyeCommand com( on );
    return sendCommand( com );
}

/*-------------------------------------------------------------------*/
/*!

*/
bool
CoachAgent::doChangePlayerType( const int unum,
                                const int type )
{
    if ( unum < 1 || 11 < unum )
    {
        std::cerr << config().teamName()
                  << "coach: "
                  << world().time()
                  << " (doChangePlayerType) Illegal player number = "
                  << unum
                  << std::endl;
        return false;
    }

    if ( type < Hetero_Default
         || PlayerParam::i().playerTypes() <= type )
    {
        std::cerr << config().teamName()
                  << " coach: "
                  << world().time()
                  << "(doChangePlayerType) Illegal player type = "
                  << type
                  << std::endl;
        return false;
    }

    CoachChangePlayerTypeCommand com( unum, type );
    return sendCommand( com );
}

/*-------------------------------------------------------------------*/
/*!

*/
bool
CoachAgent::doChangePlayerTypes( const std::vector< std::pair< int, int > > & types )
{
    if ( types.empty() )
    {
        return false;
    }

    //CoachChangePlayerTypesCommand com( types );
    //return sendCommand( com );

    bool result = true;
    for ( std::vector< std::pair< int, int > >::const_iterator it = types.begin();
          it != types.end();
          ++it )
    {
        result = doChangePlayerType( it->first, it->second );
    }

    return result;
}

/*-------------------------------------------------------------------*/
/*!

*/
bool
CoachAgent::doSayFreeform( const std::string & msg )
{
    if ( msg.empty()
         || static_cast< int >( msg.length() ) > ServerParam::i().coachSayMsgSize() )
    {
        // over the message size limitation
        std::cerr << config().teamName()
                  << " coach: "
                  << world().time()
                  << " ***WARNING** invalid free form message length = "
                  << msg.length()
                  << std::endl;
        return false;
    }

    if ( config().version() < 7.0 )
    {
        if ( world().gameMode().type() == GameMode::PlayOn )
        {
            // coach cannot send freeform while playon
            std::cerr << config().teamName()
                      << " coach: "
                      << world().time()
                      << " ***WARNING*** cannot send message while playon. "
                      << std::endl;
            return false;
        }

        // raw message string is sent.
        // (say <message>)

        M_worldmodel.incFreeformSendCount();

        CoachSayCommand com( msg );
        return sendCommand( com );
    }

    // check if coach is allowd to send the freeform message or not.
    if ( ! world().canSendFreeform() )
    {
        std::cerr << config().teamName()
                  << " coach: "
                  << world().time()
                  << " ***WARNING*** cannot send freeform now. "
                  << std::endl;
        return false;
    }

    // send clang format message
    // (say (freeform "<message>"))

    M_worldmodel.incFreeformSendCount();

    //std::ostringstream ostr;
    //ostr << "(say (freeform \"" << msg << "\"))";
    //return ( M_client->sendMessage( ostr.str().c_str() ) > 0 );

    std::string freeform_msg;
    freeform_msg.reserve( msg.length() + 32 );

    freeform_msg = "(say (freeform \"";
    freeform_msg += msg;
    freeform_msg += "\"))";

    return ( M_client->sendMessage( freeform_msg.c_str() ) > 0 );
}

/*-------------------------------------------------------------------*/
/*!

*/
bool
CoachAgent::doTeamGraphic( const int x,
                           const int y,
                           const TeamGraphic & team_graphic )
{
    static int send_count = 0;
    static GameTime send_time( -1, 0 );

    if ( send_time != M_impl->current_time_ )
    {
        send_count = 0;
    }

    send_time = M_impl->current_time_;
    ++send_count;

    if ( send_count > config().maxTeamGraphicPerCycle() ) // XXX Magic Number
    {
        return false;
    }

    TeamGraphic::Index index( x, y );
    TeamGraphic::Map::const_iterator tile = team_graphic.tiles().find( index );

    if ( tile == team_graphic.tiles().end() )
    {
        std::cerr << config().teamName()
                  << " coach: "
                  << world().time()
                  << " ***WARNING*** The xpm tile ("
                  << x << ',' << y << ") does not found in the team graphic."
                  << std::endl;
        return false;
    }

    std::ostringstream ostr;

    ostr << "(team_graphic (" << x << ' ' << y << ' ';
    tile->second->print( ostr );
    ostr << "))";

    return ( M_client->sendMessage( ostr.str().c_str() ) > 0 );
}

}
