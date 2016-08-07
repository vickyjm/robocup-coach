from math import sqrt
import re
import sys


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
						return "UNSUCCESSFUL SHOT"
				elif (ownerOld[0] == "r") :
					if (ball_posX <= -42.5) and (ball_posY > -10) and (ball_posY < 10) and (ball_velXOld < 0):
						return "UNSUCCESSFUL SHOT"
				distance = sqrt(pow(ball_posX - oldOwner_X, 2) + pow(ball_posY - oldOwner_Y, 2))/10
				if (distance < 5) : 							# If the ball was lost near the old owner
					return "UNSUCCESSFUL DRIBBLE"
				else :
					return "UNSUCCESSFUL PASS"
		elif (ownerNew == ownerOld) and (ownerNew != "") and (ownerOld != "") and (owner != "") :
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

	teammates = [[ownerX, ownerY]] # Add the info about the owner

	##---- The second teammate correspond to the player who is closer to the owner ----##
	minDistSecond = sqrt(pow(ownerX - team_posX[0], 2) + pow(ownerY - team_posY[0], 2))
	auxSecondP = 0

	##---- The third teammate correspond to the player who is closer to the ball's trajectory ----##
	minDistThird = sqrt(pow(bx - team_posX[0], 2) + pow(by - team_posY[0], 2))
	auxThirdP = 0

	for i in range(1,11):
		if (str(i + 1) != auxOwner[1]): 	# Discard the owner
			actualSecondDist = sqrt(pow(ownerX - team_posX[i], 2) + pow(ownerY - team_posY[i], 2))
			if (minDistSecond > actualSecondDist):
				minDistSecond = actualSecondDist
				auxSecondP = i

			actualThirdDist = sqrt(pow(bx - team_posX[i], 2) + pow(by - team_posY[i], 2))
			if (minDistThird > actualThirdDist):
				minDistThird = actualThirdDist
				auxThirdP = i

	teammates.append([team_posX[auxSecondP], team_posY[auxSecondP]]) 	# Add the info about the second teammate
	teammates.append([team_posX[auxThirdP], team_posY[auxThirdP]]) 		# Add the info about the third teammate

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

	##---- The first opponent correspond to the player who is closer to the owner ----##
	minDist = sqrt(pow(ownerX - team_posX[0], 2) + pow(ownerY - team_posY[0], 2))
	auxPlayer = 0

	##---- The second opponent correspond to the player who is closer to last position of the ball ----##
	minDistSecond = sqrt(pow(bx - team_posX[0], 2) + pow(by - team_posY[0], 2))
	auxSecondP = 0
	
	for i in range(1,11):
		actualDist = sqrt(pow(ownerX - team_posX[i], 2) + pow(ownerY - team_posY[i], 2))
		if (minDist > actualDist):
			minDist = actualDist
			auxPlayer = i

		actualDistSecond = sqrt(pow(bx - team_posX[i], 2) + pow(by - team_posY[i], 2))
		if (minDistSecond > actualDistSecond):
			minDistSecond = actualDistSecond
			auxSecondP = i

	opponents.append([team_posX[auxPlayer], team_posY[auxPlayer]]) 		# Add the info about the first opponent
	opponents.append([team_posX[auxSecondP], team_posY[auxSecondP]])	# Add the info about the second opponent

	##---- The third and the fourth opponents corresponds to the players near to the center of action path ----##
	auxPlayer = 0
	auxSecondP = 1
	middlePoint = [(bx + old_bx)/2, (by + old_by)/2]
	minDist = sqrt(pow(middlePoint[0] - team_posX[0], 2) + pow(middlePoint[1] - team_posY[0], 2))
	minDistSecond = sqrt(pow(middlePoint[0] - team_posX[1], 2) + pow(middlePoint[1] - team_posY[1], 2))
	
	for i in range(2,11):
		actualDist = sqrt(pow(middlePoint[0] - team_posX[i], 2) + pow(middlePoint[1] - team_posY[i], 2))
		if (minDist > actualDist):
			minDist = actualDist
			auxPlayer = i
		elif (minDistSecond > actualDist):
			minDistSecond = actualDist
			auxSecondP = i

	opponents.append([team_posX[auxPlayer], team_posY[auxPlayer]]) 		# Add the info about the third player
	opponents.append([team_posX[auxSecondP], team_posY[auxSecondP]])	# Add the info about the fourth player

	return opponents



##---- Main function ----##
if __name__ == "__main__":

	ball_velXOld = 0
	ball_velYOld = 0
	ball_velXNew = 0
	ball_velYNew = 0
	ball_posXOld = None
	ball_posYOld = None
	ownerOld = ""
	ownerNew = ""
	kick_rand = []
	outputFile = open("logActions.dat","w") # Output file

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

				if (ball_posXOld == None): 			# First line of the action
					ball_posXOld = ball_posX
					ball_posYOld = ball_posY

				for i in range(11):
					unum = str(i+1) 				# Uniform number

					##---- Extract player's position (Left Team) ----##
					left_pPosX.append(extractPosInfo("l", unum, "pos.x", line))
					left_pPosY.append(extractPosInfo("l", unum, "pos.y", line))


					##---- Extract player's position (Right Team) ----##
					right_pPosX.append(extractPosInfo("r", unum, "pos.x", line))
					right_pPosY.append(extractPosInfo("r", unum, "pos.y", line))

				# print("Team L-X", left_pPosX)
				# print("Team L-Y", left_pPosY)
				# print("Team R-X", right_pPosX)
				# print("Team R-Y", right_pPosY)

				#---- Obtaining the current owner of the ball ----##
				owner = ownerPlayer(ball_posX,ball_posY,left_pPosX,left_pPosY,right_pPosX,right_pPosY,line,kick_rand)
				#print("Cycle "+str(cycle)+" : "+owner)
				if (owner != "") :
					##---- If there is an owner, store it in ownerNew ----##
					ownerNew = owner

				##---- Extract the current ball velocity ----##
				ball_velXNew = extractBallInfo("ballvel.x",line)
				ball_velYNew = extractBallInfo("ballvel.y",line)

				##---- Extract the old owner's position ----##
				oldOwner_X = None;
				oldOwner_Y = None;
				if (ownerOld != "") :
					if (len(ownerOld) == 4) :
						ownerONum = ownerOld[2]+ownerOld[3]
					else :
						ownerONum = ownerOld[2]

					oldOwner_X = extractPosInfo(ownerOld[0], ownerONum, "pos.x", line)
					oldOwner_Y = extractPosInfo(ownerOld[0], ownerONum, "pos.y", line)

				action = actionClassifier(ball_posX,ball_posY,ball_velXNew,ball_velYNew,ball_velXOld,ball_velYOld,ownerNew,ownerOld,oldOwner_X,oldOwner_Y,owner)
				
				if (action != ""):
					##---- Add the action and the values to the output file ----##
					auxOwner = ownerNew.split()

					if (auxOwner[0] == "l"):
						ownerX = left_pPosX[int(auxOwner[1]) - 1]
						ownerY = left_pPosY[int(auxOwner[1]) - 1]
						teammates = chooseTeammates(ownerNew, left_pPosX, left_pPosY, ball_posX, ball_posY)
						opponents = chooseOpponents(ownerX, ownerY, right_pPosX, right_pPosY, ball_posX, ball_posY, ball_posXOld, ball_posYOld)
					else:
						ownerX = right_pPosX[int(auxOwner[1]) - 1]
						ownerY = right_pPosY[int(auxOwner[1]) - 1]
						teammates = chooseTeammates(ownerNew, right_pPosX, right_pPosY, ball_posX, ball_posY)
						opponents = chooseOpponents(ownerX, ownerY, left_pPosX, left_pPosY, ball_posX, ball_posY, ball_posXOld, ball_posYOld)

					outputFile.write(str(ball_posX) + " " + str(ball_posY) + " ") # Ball position
					for player in teammates:
						outputFile.write(str(player[0]) + " " + str(player[1]) + " ") # X,Y position of the teammate
					for player in opponents:
						outputFile.write(str(player[0]) + " " + str(player[1]) + " ")
					outputFile.write(action + "\n")

					ball_posXOld = None		# Restart the init values of the next action
					ball_posYOld = None

				##---- Assign the Old Ball Velocity and Owner ----##
				ballvelXOld = ball_velXNew
				ballvelYOld = ball_velYNew
				ownerOld = ownerNew

				cycle = cycle+1

			elif (line[0] == "(player_type"):
				kick_rand.append(extractKickTypeInfo(line))
			elif (line[0] == "(playmode" and (line[2] == "goal_l)" or line[2]=="goal_r)")) : # Check if a goal happened
				##---- Add the action and the values to the output file ----##
				auxOwner = ownerNew.split()

				if (auxOwner[0] == "l"):
					ownerX = left_pPosX[int(auxOwner[1]) - 1]
					ownerY = left_pPosY[int(auxOwner[1]) - 1]
					teammates = chooseTeammates(ownerNew, left_pPosX, left_pPosY, ball_posX, ball_posY)
					opponents = chooseOpponents(ownerX, ownerY, right_pPosX, right_pPosY, ball_posX, ball_posY, ball_posXOld, ball_posYOld)
				else:
					ownerX = right_pPosX[int(auxOwner[1]) - 1]
					ownerY = right_pPosY[int(auxOwner[1]) - 1]
					teammates = chooseTeammates(ownerNew, right_pPosX, right_pPosY, ball_posX, ball_posY)
					opponents = chooseOpponents(ownerX, ownerY, left_pPosX, left_pPosY, ball_posX, ball_posY, ball_posXOld, ball_posYOld)

				outputFile.write(str(ball_posX) + " " + str(ball_posY) + " ") # Ball position
				for player in teammates:
					outputFile.write(str(player[0]) + " " + str(player[1]) + " ") # X,Y position of the teammate
				for player in opponents:
					outputFile.write(str(player[0]) + " " + str(player[1]) + " ")
				outputFile.write("GOAL" + "\n")

				ball_posXOld = None		# Restart the init values of the next action
				ball_posYOld = None

	outputFile.close()


			


