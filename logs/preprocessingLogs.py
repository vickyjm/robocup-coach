from math import sqrt
import re
import sys
from operator import itemgetter


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
#	This function classifies the actions that have occurred    #
#   during the match.                            			   #
#															   #
#	Param:													   #
#		ball_posX, ball_posY: ball's position on the X and     #
#							  Y axes.		   				   #
#		ball_velXNew,ball_velYNew: ball's velocity on the X    #
#								   and Y axes for the          #
#								   current cycle.		   	   #
#		ball_velXOld,ball_velYOld: ball's velocity on the X    #
#								   and Y axes for the          #
#								   last cycle.		   	       #
#		ownerNew: current owner of the ball.                   #
#		ownerOld: owner of the ball for the last cycle.        #
#       oldOwner_X, oldOwner_Y: Position of the old owner in   #
#                               the X and Y axes.              #
# 		owner : 				Real owner of the ball in      #
#								the current cycle.             #
##------------------------------------------------------------##
def actionClassifier(ball_posX, ball_posY,ball_velXNew,ball_velYNew,ball_velXOld,ball_velYOld,ownerNew,ownerOld,oldOwner_X,oldOwner_Y,owner):
	# -- Checking how many digits the owner's ids have -- #
	ownerO = ""
	ownerN = ""
	if (len(ownerOld) == 4) :
		ownerO = ownerOld[2]+ownerOld[3]
	elif (len(ownerOld) == 3) :
		ownerO = ownerOld[2]

	if (len(ownerNew) == 4) :
		ownerN = ownerNew[2]+ownerNew[3]
	elif (len(ownerNew) == 3) :
		ownerN = ownerNew[2]

	# If the ball velocity has changed it means it has moved
	if ((ball_velXNew != ball_velXOld) or (ball_velYNew != ball_velYOld)) :
		if (ownerNew != ownerOld) and (ownerNew != "") and (ownerOld != "") :
			if (ownerNew[0] == ownerOld[0]) : 					# If the ball stayed in the same team
				if (ownerN != ownerO) : 						# If the ball changed owners
					return "PASS"
			elif (ownerNew[0] != ownerOld[0]) :				    # If the ball changed teams
				if (ownerOld[0] == "l") :						# Check for unsuccessful shots to the goal
					if (ball_posX >= 42.5) and (ball_posY > -10) and (ball_posY < 10) and (ball_velXOld > 0):
						return "UNSUCCESSFULSHOOT"
				elif (ownerOld[0] == "r") :
					if (ball_posX <= -42.5) and (ball_posY > -10) and (ball_posY < 10) and (ball_velXOld < 0):
						return "UNSUCCESSFULSHOOT"
				distance = sqrt(pow(ball_posX - oldOwner_X, 2) + pow(ball_posY - oldOwner_Y, 2))
				if (distance < 5) : 							# If the ball was lost near the old owner
					return "UNSUCCESSFULDRIBBLE"
				else :
					return "UNSUCCESSFULPASS"
		elif (ownerNew == ownerOld) and (ownerNew != "") and (ownerOld != "") and (owner != "") :
			distance = sqrt(pow(ball_posX - oldOwner_X, 2) + pow(ball_posY - oldOwner_Y, 2))
			if (distance > 0.5) : # If the ball and the owner have moved
				return "DRIBBLE"
	
	return ""


##------------------------------------------------------------##
#	This function returns the teammates involved in the action.#
#															   #
#	Param:													   #
#		owner: the owner of the ball.		   				   #
#		team_posX, team_posY: positions of the team that owns  #
#							  the ball.   					   #
#		bx, by: ball position. 								   #
##------------------------------------------------------------##
def chooseTeammates(owner, team_posX, team_posY, bx, by):
	auxOwner = owner.split()
	ownerX = team_posX[int(auxOwner[1]) - 1]
	ownerY = team_posY[int(auxOwner[1]) - 1]
	selected = [int(auxOwner[1]) - 1] # List with the players that are already selected

	teammates = [[ownerX, ownerY]] # Add the info about the owner

	##---- The second teammate correspond to the player who is closer to the owner ----##
	minDist = 10000

	for i in range(0,11):
		if not(i in selected): 	#Discard the selected players
			actualDist = sqrt(pow(ownerX - team_posX[i], 2) + pow(ownerY - team_posY[i], 2))
			if (minDist > actualDist):
				minDist = actualDist
				minPlayer = i
	selected.append(minPlayer)
	teammates.append([team_posX[minPlayer], team_posY[minPlayer]]) 	# Add the info about the second teammate
	
	##---- The third teammate correspond to the player who is closer to the ball's trajectory ----##
	minDist = 10000

	for i in range(0,11):
		if not(i in selected): 	#Discard the selected players
			actualDist = sqrt(pow(bx - team_posX[i], 2) + pow(by - team_posY[i], 2))
			if (minDist > actualDist):
				minDist = actualDist
				minPlayer = i

	teammates.append([team_posX[minPlayer], team_posY[minPlayer]]) 		# Add the info about the third teammate

	return teammates

##------------------------------------------------------------##
#	This function returns the opponents involved in the action.#
#															   #
#	Param:													   #
#		ownerX, ownerY: owner's position.	   				   #
#		team_posX, team_posY: positions of the opponent team.  #
#		bx, by: ball position. 								   #
#		old_bx, old_by: position of the ball at the start of   #
#						the action.							   #
##------------------------------------------------------------##
def chooseOpponents(ownerX, ownerY, team_posX, team_posY, bx, by, old_bx, old_by):
	opponents = []
	selected = [] # List with the players that are already selected

	##---- The first opponent correspond to the player who is closer to the owner ----##
	minDist = sqrt(pow(ownerX - team_posX[0], 2) + pow(ownerY - team_posY[0], 2))
	minPlayer = 0

	for i in range(1,11):
		actualDist = sqrt(pow(ownerX - team_posX[i], 2) + pow(ownerY - team_posY[i], 2))
		if (minDist > actualDist):
			minDist = actualDist
			minPlayer = i
	selected.append(minPlayer)
	opponents.append([team_posX[minPlayer], team_posY[minPlayer]]) 		# Add the info about the first opponent

	##---- The second opponent correspond to the player who is closer to last position of the ball ----##
	minDist = 10000
	
	for i in range(0,11):
		if not(i in selected): 	#Discard the selected players
			actualDist = sqrt(pow(bx - team_posX[i], 2) + pow(by - team_posY[i], 2))
			if (minDist > actualDist):
				minDist = actualDist
				minPlayer = i
	selected.append(minPlayer)
	opponents.append([team_posX[minPlayer], team_posY[minPlayer]])	# Add the info about the second opponent

	##---- The third and the fourth opponents corresponds to the players near to the center of action path ----##
	middlePoint = [(bx + old_bx)/2, (by + old_by)/2]
	
	for j in range(0,2):
		minDist = 10000
		for i in range(0,11):
			if not(i in selected):
				actualDist = sqrt(pow(middlePoint[0] - team_posX[i], 2) + pow(middlePoint[1] - team_posY[i], 2))
				if (minDist > actualDist):
					minDist = actualDist
					minPlayer = i
		selected.append(minPlayer)
		opponents.append([team_posX[minPlayer], team_posY[minPlayer]])

	return opponents


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


##------------------------------------------------------------##
#	This function generate the files for the second 		   #
#	preprocessing described by Maryam.						   #
#	Param:													   #
#		px,py: player's position.	   				   		   #
#		teammates: position of all teammates.				   #
##------------------------------------------------------------##
def generateNormalizedLogs(logFile):
	##---- Outputs files ----##
	shotFile = open("shotFile.dat", "a") 
	dribbleFile = open("dribbleFile.dat", "a")	
	passFile = open("passFile.dat", "a")
	#opponentFile = open("opponentFile.dat", "a")

	with open(logFile) as file:
		cont = 0
		for line in file:
			cont += 1
			teammates = []
			teammatesPass = []
			opponents = []
			opponentsPass = []
			originalDist = []

			line = line.split()						# Split into a list

			##---- Normalize ball's position ----##

			first_bx = float(line[0])
			first_by = float(line[1])
			last_bx = float(line[2])
			last_by = float(line[3])

			#bbbb|45|67|89|1011|1213|1415|1617|1819|2021|2223|2425|
			#2627|2829|3031|3233|3435|3637|3839|4041|4243|4445|4647

			i = 4
			while (i <= 24):
				#originalDist.append([float(line[i]), float(line[i+1])])
				#dist = sqrt(pow(first_bx - float(line[i]), 2) + pow(first_by - float(line[i+1]), 2))
				dist = distFromLine(first_bx,first_by,last_bx,last_by,float(line[i]),float(line[i+1]))
				#teammatesPass.append(distPass)
				teammates.append(dist)
				i = i + 2
			while (i <= 46):
				#nearestPlayer = nearestTeammate(float(line[i]), float(line[i+1]), originalDist)
				nearestPlayer = distFromLine(first_bx,first_by,last_bx,last_by,float(line[i]),float(line[i+1]))
				opponents.append(nearestPlayer)
				#opponentsPass.append(dist)
				i = i + 2

			ball_velX = float(line[i])
			ball_velY = float(line[i+1])

			i = i + 2

			##---- Actions ----##
			if (line[i] == "GOAL"):
				shotFile.write(str(first_bx/5) + "," + str(first_by/5) + ",")
				#shotFile.write(str(ball_velX) + "," + str(ball_velY) + ",")
				#opponentFile.write(str(first_bx) + "," + str(first_by) + ",")
				for p in teammates:
					shotFile.write(str(p) + ",")
					#opponentFile.write(str(teammates[j]) + ",")
				for p in opponents:
					shotFile.write(str(p) + ",")
					#opponentFile.write(str(opponents[j]) + ",")
				#shotFile.write(line[i] + "\n")
				shotFile.write("1" + "\n")
				#opponentFile.write("SHOOT\n")
			elif (line[i] == "PASS"):
				passFile.write(str(first_bx/5) + "," + str(first_by/5) + ",")
				#passFile.write(str(ball_velX) + "," + str(ball_velY) + ",")
				#opponentFile.write(str(first_bx) + "," + str(first_by) + ",")
				for p in teammates:
					#passFile.write(str(teammatesPass[j]) + ",")
					passFile.write(str(p) + ",")
					#opponentFile.write(str(teammates[j]) + ",")
				for p in opponents:
					#passFile.write(str(opponentsPass[j]) + ",")
					passFile.write(str(p) + ",")
					#opponentFile.write(str(opponents[j]) + ",")
				#passFile.write(line[i] + "\n")
				passFile.write("1" + "\n")
				#opponentFile.write(line[i] + "\n")
			elif (line[i] == "DRIBBLE"):
				dribbleFile.write(str(first_bx/5) + "," + str(first_by/5) + ",")
				#dribbleFile.write(str(ball_velX) + "," + str(ball_velY) + ",")
				#opponentFile.write(str(first_bx/5) + "," + str(first_by/5) + ",")
				for p in teammates:
					dribbleFile.write(str(p) + ",")
					#opponentFile.write(str(teammates[j]) + ",")
				for p in opponents:
					dribbleFile.write(str(p) + ",")
					#opponentFile.write(str(opponents[j]) + ",")
				#dribbleFile.write(line[i] + "\n")
				dribbleFile.write("1" + "\n")
				#opponentFile.write(line[i] + "\n")
			elif (line[i] == "UNSUCCESSFULSHOOT"):
				shotFile.write(str(first_bx/5) + "," + str(first_by/5) + ",")
				#shotFile.write(str(ball_velX) + "," + str(ball_velY) + ",")
				#opponentFile.write(str(first_bx/5) + "," + str(first_by/5) + ",")
				for p in teammates:
					shotFile.write(str(p) + ",")
					#opponentFile.write(str(teammates[j]) + ",")
				for p in opponents:
					shotFile.write(str(p) + ",")
					#opponentFile.write(str(opponents[j]) + ",")
				#shotFile.write(line[i] + "\n")
				shotFile.write("0" + "\n")
				#opponentFile.write("SHOOT\n")
			elif (line[i] == "UNSUCCESSFULDRIBBLE"):
				dribbleFile.write(str(first_bx/5) + "," + str(first_by/5) + ",")
				#dribbleFile.write(str(ball_velX) + "," + str(ball_velY) + ",")
				#opponentFile.write(str(first_bx/5) + "," + str(first_by/5) + ",")
				for p in teammates:
					dribbleFile.write(str(p) + ",")
					#opponentFile.write(str(teammates[j]) + ",")
				for p in opponents:
					dribbleFile.write(str(p) + ",")
					#opponentFile.write(str(opponents[j]) + ",")
				#dribbleFile.write(line[i] + "\n")
				dribbleFile.write("0" + "\n")
				#opponentFile.write("DRIBBLE\n")
			elif (line[i] == "UNSUCCESSFULPASS"):
				passFile.write(str(first_bx/5) + "," + str(first_by/5) + ",")
				#passFile.write(str(ball_velX) + "," + str(ball_velY) + ",")
				#opponentFile.write(str(first_bx/5) + "," + str(first_by) + ",")
				for p in teammates:
					#passFile.write(str(teammatesPass[j]) + ",")
					passFile.write(str(p) + ",")
					#opponentFile.write(str(teammates[j]) + ",")
				for p in opponents:
					#passFile.write(str(opponentsPass[j]) + ",")
					passFile.write(str(p) + ",")
					#opponentFile.write(str(opponents[j]) + ",")
				#passFile.write(line[i] + "\n")
				passFile.write("0" + "\n")
				#opponentFile.write("PASS\n")
		file.close()
	
	shotFile.close()
	passFile.close()
	dribbleFile.close()
	#opponentFile.close()

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
	ownerOld = ""
	ownerNew = ""
	kick_rand = []
	outputFile = open(sys.argv[2],"w") # Output file
	left_pPosX0, left_pPosY0, right_pPosX0, right_pPosY0 = [], [], [], []

	cycle = 1
	with open(sys.argv[1]) as file:
		for line in file:
			line = line.split()						# Split into a list
	
			if ((line[0] == "(show")):	# Is a (show) line
				left_pPosX, left_pPosY = [], []
				right_pPosX, right_pPosY = [], []

				##---- Extract ball info ----##
				ball_posX = extractBallInfo("ballpos.x", line)
				ball_posY = extractBallInfo("ballpos.y", line)

				##---- Extract the current ball velocity ----##
				ball_velXNew = extractBallInfo("ballvel.x",line)
				ball_velYNew = extractBallInfo("ballvel.y",line)

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
					firstVel[0] = ball_velXNew
					firstVel[1] = ball_velYNew

					# Keep the player's position at the begining of the action
					left_pPosX0 = left_pPosX 	
					left_pPosY0 = left_pPosY
					right_pPosX0 = right_pPosX
					right_pPosY0 = right_pPosY

					isInitialPos = False

				#---- Obtaining the current owner of the ball ----##
				owner = ownerPlayer(ball_posX,ball_posY,left_pPosX,left_pPosY,right_pPosX,right_pPosY,line,kick_rand)
				
				if (owner != "") :
					##---- If there is an owner, store it in ownerNew ----##
					ownerNew = owner

				##---- Extract the old owner's position ----##
				oldOwner_X = None
				oldOwner_Y = None
				if (ownerOld != ""):
					if (len(ownerOld) == 4) :
						ownerONum = ownerOld[2]+ownerOld[3]
					else :
						ownerONum = ownerOld[2]

					oldOwner_X = extractPosInfo(ownerOld[0], ownerONum, "pos.x", line)
					oldOwner_Y = extractPosInfo(ownerOld[0], ownerONum, "pos.y", line)

				action = actionClassifier(ball_posX,ball_posY,ball_velXNew,ball_velYNew,ball_velXOld,ball_velYOld,ownerNew,ownerOld,oldOwner_X,oldOwner_Y,owner)
	
				if (action != ""):
					##---- Add the action and the values to the output file ----##
					auxOwner = ownerOld.split()

					#if (auxOwner[0] == ourSide):
					ownerX = oldOwner_X
					ownerY = oldOwner_Y

					if (auxOwner[0] == "l"):
						#ownerX = left_pPosX[int(auxOwner[1]) - 1]
						#ownerY = left_pPosY[int(auxOwner[1]) - 1]
						#auxTeam = list(zip(left_pPosX0,left_pPosY0))
						teammates = list(zip(left_pPosX0,left_pPosY0))
						#auxOpp = list(zip(right_pPosX0,right_pPosY0))
						opponents = list(zip(right_pPosX0,right_pPosY0))
						#teammates = chooseTeammates(ownerOld, left_pPosX0, left_pPosY0, ball_posX, ball_posY)
						#opponents = chooseOpponents(ownerX, ownerY, right_pPosX0, right_pPosY0, ball_posX, ball_posY, ball_posXOld, ball_posYOld)
					else:
						#ownerX = right_pPosX[int(auxOwner[1]) - 1]
						#ownerY = right_pPosY[int(auxOwner[1]) - 1]
						#auxTeam = list(zip(right_pPosX0, right_pPosY0))
						teammates = list(zip(right_pPosX0, right_pPosY0))
						#auxOpp = list(zip(left_pPosX0, left_pPosY0))
						opponents = list(zip(left_pPosX0, left_pPosY0))
						#teammates = chooseTeammates(ownerOld, right_pPosX0, right_pPosY0, ball_posX, ball_posY)
						#opponents = chooseOpponents(ownerX, ownerY, left_pPosX0, left_pPosY0, ball_posX, ball_posY, ball_posXOld, ball_posYOld)

					#teammates = chooseNearestPlayers(auxTeam, ball_posX, ball_posY, ball_posXOld, ball_posYOld,7)
					#opponents = chooseNearestPlayers(auxOpp, ball_posX, ball_posY, ball_posXOld, ball_posYOld,7)

					outputFile.write(str(ball_posXOld) + " " + str(ball_posYOld) + " ") # First ball position
					outputFile.write(str(ball_posX) + " " + str(ball_posY) + " ") # Last ball position
					for player in teammates:
						outputFile.write(str(player[0]) + " " + str(player[1]) + " ") # X,Y position of the teammate
					for player in opponents:
						outputFile.write(str(player[0]) + " " + str(player[1]) + " ")

					outputFile.write(str(firstVel[0]) + " " + str(firstVel[1]) + " ")
					outputFile.write(action + "\n")

					isInitialPos = True		# Restart the init values of the next action

				##---- Assign the Old Ball Velocity and Owner ----##
				ball_velXOld = ball_velXNew
				ball_velYOld = ball_velYNew
				ownerOld = ownerNew

				cycle = cycle+1

			elif (line[0] == "(player_type"):
				kick_rand.append(extractKickTypeInfo(line))
			#elif (line[0] == "(playmode" and (line[2] == "goal_l)" or line[2]=="goal_r)") and (ourSide == line[2][5])) : # Check if a goal happened
			elif (line[0] == "(playmode" and (line[2] == "goal_l)" or line[2]=="goal_r)")) : # Check if a goal happened
				##---- Add the action and the values to the output file ----##
				auxOwner = ownerOld.split()

				if (auxOwner[0] == "l"):
					#ownerX = left_pPosX[int(auxOwner[1]) - 1]
					#ownerY = left_pPosY[int(auxOwner[1]) - 1]
					#auxTeam = list(zip(left_pPosX0,left_pPosY0))
					teammates = list(zip(left_pPosX0,left_pPosY0))
					#auxOpp = list(zip(right_pPosX0,right_pPosY0))
					opponents = list(zip(right_pPosX0,right_pPosY0))
					#teammates = chooseTeammates(ownerOld, left_pPosX0, left_pPosY0, ball_posX, ball_posY)
					#opponents = chooseOpponents(ownerX, ownerY, right_pPosX0, right_pPosY0, ball_posX, ball_posY, ball_posXOld, ball_posYOld)
				elif (auxOwner[0] == "r"):
					#ownerX = right_pPosX[int(auxOwner[1]) - 1]
					#ownerY = right_pPosY[int(auxOwner[1]) - 1]
					#auxTeam = list(zip(right_pPosX0, right_pPosY0))
					teammates = list(zip(right_pPosX0, right_pPosY0))
					#auxOpp = list(zip(left_pPosX0, left_pPosY0))
					opponents = list(zip(left_pPosX0, left_pPosY0))
					#teammates = chooseTeammates(ownerOld, right_pPosX0, right_pPosY0, ball_posX, ball_posY)
					#opponents = chooseOpponents(ownerX, ownerY, left_pPosX, left_pPosY, ball_posX, ball_posY, ball_posXOld, ball_posYOld)

				#teammates = chooseNearestPlayers(auxTeam, ball_posX, ball_posY, ball_posXOld, ball_posYOld,7)
				#opponents = chooseNearestPlayers(auxOpp, ball_posX, ball_posY, ball_posXOld, ball_posYOld,7)

				outputFile.write(str(ball_posXOld) + " " + str(ball_posYOld) + " ") # First ball position
				outputFile.write(str(ball_posX) + " " + str(ball_posY) + " ") # Last ball position
				for player in teammates:
					outputFile.write(str(player[0]) + " " + str(player[1]) + " ") # X,Y position of the teammate
				for player in opponents:
					outputFile.write(str(player[0]) + " " + str(player[1]) + " ")
				outputFile.write(str(firstVel[0]) + " " + str(firstVel[1]) + " ")
				outputFile.write("GOAL" + "\n")

				isInitialPos = True		# Restart the init values of the next action

			#elif ((line[0] == "(team") and (line[1] == "1")):
			#	if ((line[2] == "JEMV") or (line[2] == "HELIOS_BASE") or (line[2] == "HELIOS_base")):
			#		ourSide = "l"
			#	else:
			#		ourSide = "r"

		file.close()
			

	outputFile.close()
	generateNormalizedLogs(sys.argv[2])


			


