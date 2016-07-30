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
	print("TEAM: ",owner)
	if (owner != ""):
		typePlayer = extractTypeInfo(auxOwner[0], auxOwner[1], line)
		if (isOwner(ball_posX, ball_posY, ownerX, ownerY, radious + kick_rand[typePlayer])):
			return owner
	return ""

##------------------------------------------------------------##
#	This function determines if a pass has ocurred			   #
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
#		owner: Real owner according to our function.           #
##------------------------------------------------------------##
def isPass(ball_posX, ball_posY,ball_velXNew,ball_velYNew,ball_velXOld,ball_velYOld,ownerNew,ownerOld,owner):
	if ((ball_velXNew != ball_velXOld) or (ball_velYNew != ball_velYOld)) :
		if (ownerNew != ownerOld) and (ownerNew != "") and (ownerOld != "") :
			if (ownerNew[0] == ownerOld[0]) :
				return "PASS"
			elif (ownerNew[0] != ownerOld[0]) :
				return "INTERCEPT"
		if (ownerNew == ownerOld) and (ownerNew != "") and (ownerOld != "") :
			if (owner != "") :
				return "DRIBBLE"
	
	return ""

##---- Main function ----##
if __name__ == "__main__":

	ball_velXOld = 0;
	ball_velYOld = 0;
	ball_velXNew = 0;
	ball_velYNew = 0;
	ownerOld = "";
	ownerNew = "";
	kick_rand = []

	with open(sys.argv[1]) as file:
		for line in file:
			left_pPosX, left_pPosY = [], []
			right_pPosX, right_pPosY = [], []
			line = line.split()						# Split into a list
			if ((line[0] == "(show")):	# Is a (show) line

				##---- Extract ball info ----##
				ball_posX = extractBallInfo("ballpos.x", line)
				ball_posY = extractBallInfo("ballpos.y", line)
				#print("Ball_X: ", ball_posX)
				#print("Ball_Y: ", ball_posY)

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
				if not(owner == "") :
					##---- If there is an owner, store it in ownerNew ----##
					ownerNew = owner

				##---- Extract the current ball velocity ----##
				ball_velXNew = extractBallInfo("ballvel.x",line)
				ball_velYNew = extractBallInfo("ballvel.y",line)

				print(isPass(ball_posX,ball_posY,ball_velXNew,ball_velYNew,ball_velXOld,ball_velYOld,ownerNew,ownerOld,owner))

				##---- Assign the Old Ball Velocity and Owner ----##
				ballvelXOld = ball_velXNew
				ballvelYOld = ball_velYNew
				ownerOld = ownerNew

			elif (line[0] == "(player_type"):
				kick_rand.append(extractKickTypeInfo(line))



			


