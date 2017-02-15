from math import sqrt
import re
import sys
from operator import itemgetter
import glob
import os


##------------------------------------------------------------##
# 	This function determines if a player is the ball owner.    #
#															   #
#	Param:													   #
#		bx, by: ball's position on the X and Y axes.		   #
#		px, py: player's position on the X and Y axes.  	   #
#		radious												   #
##------------------------------------------------------------## 												
def isOwner(bx, by, px, py, radious):
	distance = sqrt(pow(bx - px, 2) + pow(by - py, 2))/10
	return distance < radious

##------------------------------------------------------------##
#	Function used to extract some info about the ball from	   #
#	the logfile. 											   #
#															   #
#	Param:													   #
#		info: string that indicates the required info. 		   #
#		line: (show) line from the logfile. 				   #
##------------------------------------------------------------##
def extractBallInfo(info, line):
	i = 0

	##---- Move through the line until the 'b' char is found ----##
	while (i < len(line)):
		if (line[i]=="((b)"):
			break
		i = i + 1

	##---- Extract the info required ----##
	if (info == "ballpos.x"):
		return float(line[i+1])
	elif (info == "ballpos.y"):
		return float(line[i+2])
	elif (info == "ballvel.x"):
		return float(line[i+3])
	elif (info == "ballvel.y"):
		tmp = re.sub(r'[()]', '', line[i+4]) # Remove ")"
		return float(tmp)

##------------------------------------------------------------##
#	Function used to extract the type of a certain player from #
#	the logfile. 											   #
#															   #
#	Param:													   #
#		side: side of the field where his team is playing 	   #
#			  (l or r).     								   #
#		unum: player number. 						           #
#		line: (show) line from the logfile. 				   #
##------------------------------------------------------------##
def extractTypeInfo(side, unum, line):
	i = 0

	##---- Move through the line until the player is found ----##
	while (i < len(line) - 2):
		if ((line[i] == "((" + side) and (line[i+1] == unum + ")")):
			i = i + 2 		# Move to the first value of the selected player
			break
		i = i + 1

	return int(line[i])


##------------------------------------------------------------##
#	Function used to extract the position of a certain player  #
#   from the logfile. 										   #
#															   #
#	Param:													   #
#		side: side of the field where his team is playing 	   #
#			  (l or r).     								   #
#		unum: player number. 						           #
#		info: string that indicates the required info. 		   #
#		line: (show) line from the logfile. 				   #
##------------------------------------------------------------##
def extractPosInfo(side, unum, info, line):
	i = 0

	##---- Move through the line until the player is found ----##
	while (i < len(line) - 1):
		if ((line[i] == "((" + side) and (line[i+1] == unum + ")")):
			i = i + 4  		# Move to the first value of the selected player
			break
		i = i + 1

	if (info == "pos.x"):
		return float(line[i])
	elif (info == "pos.y"):
		return float(line[i+1])
	elif (info == "vel.x"):
		return float(line[i+2])
	elif (info == "vel.y"):
		return float(line[i+3])
	elif (info == "body"):
		return float(line[i+4])
	elif (info == "neck"):
		return float(line[i+5])


##------------------------------------------------------------##
#	Function used to extract the kick_rand value of a certain  #
#	type of player from the logfile. 						   #
#															   #
#	Param:													   #
#		line: (show) line from the logfile. 				   #
##------------------------------------------------------------##
def extractKickTypeInfo(line):
	i = 0
	newLine = []

	##---- Remove parenthesis and separates	fields ----##
	for elem in line:
		tmp = re.sub(r'[()]', ' ', elem) 
		newLine += tmp.split()

	##---- Move through the line until the kick_rand is found ----##
	while (i < len(newLine) - 1):
		if (newLine[i] == "kick_rand"):
			return float(newLine[i+1])
		i = i + 1

##------------------------------------------------------------##
#	This function determines the ball owner. 				   #
#															   #
#	Param:													   #
#		ball_posX, ball_posY: ball's position on the X and     #
#							  Y axes.		   				   #
#		left_pPosX, left_pPosY: positions of the left team.    #
#		right_pPosX, right_pPosY: positions of the right team. #
#		line: (show) line from the logfile. 				   #
#		kick_rand: random value given for the server.		   #
##------------------------------------------------------------##
def ownerPlayer(ball_posX, ball_posY, left_pPosX, left_pPosY, right_pPosX, right_pPosY, line, kick_rand):
	radious = 0.05
	owner = ""
	minDist = 1000
	ownerX = 0
	ownerY = 0

	for i in range(11):
		if (pow(ball_posX - left_pPosX[i], 2) + pow(ball_posY - left_pPosY[i], 2) < minDist):
			ownerX = left_pPosX[i]
			ownerY = left_pPosY[i]
			owner = "l " + str(i+1)
			minDist = pow(ball_posX - left_pPosX[i], 2) + pow(ball_posY - left_pPosY[i], 2)

		if (pow(ball_posX - right_pPosX[i], 2) + pow(ball_posY - right_pPosY[i], 2) < minDist):
			ownerX = right_pPosX[i]
			ownerY = right_pPosY[i]
			owner = "r " + str(i+1)
			minDist = pow(ball_posX - right_pPosX[i], 2) + pow(ball_posY - right_pPosY[i], 2)

	auxOwner = owner.split() 	# Convert into a list for extracting info
	if (owner != ""):
		typePlayer = extractTypeInfo(auxOwner[0], auxOwner[1], line)
		if (isOwner(ball_posX, ball_posY, ownerX, ownerY, radious + kick_rand[typePlayer])):
			return owner
	return ""

##------------------------------------------------------------##
#	This function returns the nearest teammate 				   #
#															   #
#	Param:													   #
#		px,py: player's position.	   				   		   #
#		teammates: position of all teammates.				   #
##------------------------------------------------------------##
def nearestTeammate(px, py, teammates):
	minDist = sqrt(pow(px - teammates[0][0], 2) + pow(py - teammates[0][1], 2))
	index = 0
	for i in range(1,3):
		dist = sqrt(pow(px - teammates[i][0], 2) + pow(py - teammates[i][1], 2))
		if (minDist > dist):
			minDist = dist
			index = i

	return minDist

def distFromLine(x0,y0,x1,y1,x2,y2):

	if (x0!=x1):
		a = -(y1-y0)/(x1-x0)
		c = (((y1-y0)*x0)/(x1-x0)) - y0
		b = 1
	else:
		a = 1
		c = x0
		b = 0

	num = abs(a*x2 + y2 + c)
	denom = sqrt(pow(a,2) + b)

	return num/denom




def chooseNearestPlayers(p_pos, bx, by, old_bx, old_by, numP):
	auxPlayers = []
	players = []

	i = 0
	for p in p_pos:
		auxPlayers.append([distFromLine(old_bx, old_by, bx, by, p[0], p[1]),i])
		i = i + 1

	auxPlayers = sorted(auxPlayers, key=itemgetter(0))

	for p in range(numP):
		index = auxPlayers[p][1]
		players.append(p_pos[index])

	return players

##---- Main function ----##
# python3 logStatsGen folderPath opponentTeam type outputFile
# where folderPath is the folder path where the logs are (make sure all the logs that will be used end in .rcg)
# opponent team is the name of the opponent Team as it appears in the log titles.
# type is one of the following : control, exp1 or exp2. Control means our team is the base one, exp1 is the base one with decision
# trees and exp2 is the base one with formation detection
# outputFile is where the information will be outputted

# Numbers for teams in output files (as well as correct team names): 
# JEMV - 1
# HELIOS2015 - 2
# HERMES - 3
# Genius2015 - 4
# WrightEagle - 5
# Jaeger - 6

if __name__ == "__main__":

	ball_velXOld = 0
	ball_velYOld = 0
	ball_velXNew = 0
	ball_velYNew = 0
	ball_posXOld = 0
	ball_posYOld = 0
	firstVel = [0,0]
	isInitialPos = True
	ourSide = ""
	theirSide = ""
	ownerOld = ""
	ownerNew = ""
	kick_rand = []
	left_pPosX0, left_pPosY0, right_pPosX0, right_pPosY0 = [], [], [], []

	opponentPos = 0 # Ball posession for opponent and for our team
	ourPos = 0
	ourGoals = 0	# Goals for our team and the opponent
	opponentGoals = 0

	folderPath = sys.argv[1]
	opponentTeam = sys.argv[2]
	ourTeamType = sys.argv[3]
	outputFile = open(sys.argv[4],"a")

	if (os.stat(sys.argv[4]).st_size == 0) :
		outputFile.write("# OurTeam OppTeam Type OurPos OppPos GoalDiff OurGoals OppGoals \n")

	for filename in glob.glob(folderPath+'*.rcg') :
		nameSplit = filename.split("vs")
		if (nameSplit[0].find("JEMV") != -1) :
			ourSide = "l"
			theirSide = "r"
		else :
			ourSide = "r"
			theirSide = "l"

		cycleTotal = 1
		opponentPos = 0 # Ball posession for opponent and for our team
		ourPos = 0
		ourGoals = 0	# Goals for our team and the opponent
		opponentGoals = 0
		with open(filename) as file:
			for line in file:
				line = line.split()						# Split into a list
		
				if ((line[0] == "(show")):	# Is a (show) line
					left_pPosX, left_pPosY = [], []
					right_pPosX, right_pPosY = [], []

					##---- Extract ball info ----##
					ball_posX = extractBallInfo("ballpos.x", line)
					ball_posY = extractBallInfo("ballpos.y", line)


					for i in range(11):
						unum = str(i+1) 				# Uniform number

						##---- Extract player's position (Left Team) ----##
						left_pPosX.append(extractPosInfo("l", unum, "pos.x", line))
						left_pPosY.append(extractPosInfo("l", unum, "pos.y", line))
						
						##---- Extract player's position (Right Team) ----##
						right_pPosX.append(extractPosInfo("r", unum, "pos.x", line))
						right_pPosY.append(extractPosInfo("r", unum, "pos.y", line))
						

					if (isInitialPos): 			# First line of the action
						ball_posXOld = ball_posX
						ball_posYOld = ball_posY

						isInitialPos = False

					#---- Obtaining the current owner of the ball ----##
					owner = ownerPlayer(ball_posX,ball_posY,left_pPosX,left_pPosY,right_pPosX,right_pPosY,line,kick_rand)

					if (owner != "") :
						##---- If there is an owner, store it in ownerNew ----##
						ownerNew = owner

					if (ownerNew != "") :
						if (ownerNew[0] == ourSide) :
							# if (opponentTeam == "Jaeger") :
							# 	opponentPos += 1
							# else :
								ourPos += 1
						else :
							# if (opponentTeam == "Jaeger") :
							# 	ourPos += 1
							# else : 
								opponentPos += 1
					cycleTotal = cycleTotal+1


				elif (line[0] == "(player_type"):
					kick_rand.append(extractKickTypeInfo(line))
				elif (line[0] == "(playmode" and (line[2] == "goal_l)")) : # Check if a goal happened
					if (ourSide == "l") :
						ourGoals += 1
					else :
						opponentGoals += 1
				elif (line[0] == "(playmode" and (line[2] == "goal_r)")) :
					if (ourSide == "r") :
						ourGoals += 1
					else :
						opponentGoals += 1

		finalString = ""
		finalString += "1 "
		if (opponentTeam == "HELIOS2015") :
			finalString += "2 "
		elif (opponentTeam == "HERMES") :
			finalString += "3 "
		elif (opponentTeam == "Genius2015") :
			finalString += "4 "
		elif (opponentTeam == "WrightEagle") :
			finalString += "5 "
		elif (opponentTeam == "Jaeger") :
			finalString += "6 "

		finalString += ourTeamType+" "
		finalString += str("{0:.3f}".format(ourPos / cycleTotal)) + " "
		finalString += str("{0:.3f}".format(opponentPos / cycleTotal)) + " "
		# if (opponentTeam == "Jaeger") :
		# 	finalString += str(-1*(ourGoals - opponentGoals)) + " "
		# else :
		finalString += str(ourGoals - opponentGoals) + " "

		# if (opponentTeam == "Jaeger") :
		# 	finalString += str(opponentGoals) + " " + str(ourGoals) + "\n"
		# else :
		finalString += str(ourGoals) + " " + str(opponentGoals) + "\n"

		outputFile.write(finalString)
		print("listo")