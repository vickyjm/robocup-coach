from math import sqrt
import re
import sys
from matplotlib import pyplot as plt
import numpy as np
from PIL import Image
import glob
from matplotlib.colors import colorConverter
import matplotlib as mpl



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


# Maps the x and y positions of a player inside a rectangle with
# origin in (originX,originY) and width and height rectWidth and
# rectHeight respectively. Opponent side is either "l" or "r"
def mapToField(originX,originY,rectWidth,rectHeight,x,y,opponentSide) :
	fieldWidth = 104
	fieldHeight = 68

	finalPoint = [];

	if opponentSide == "r" :
		side = -1
	else : 
		side = 1

	finalPoint.append(side*((((x - originX)/rectWidth)*fieldWidth) - 52))
	finalPoint.append((((y - originY)/rectHeight)*fieldHeight) + 34)

	if (finalPoint[0] < -52) :
		print(str(x) + " " + str(originX) + " " + str(rectWidth))
		print("1")
		exit(1)
	elif (finalPoint[0] > 52) :
		print("2")
		exit(2)
	if (finalPoint[1] < -34) :
		print("3")
		exit(3)
	elif (finalPoint[1] > 34):
		print("4")
		exit(4)
	return finalPoint

# Obtain the smallest rectangle possible that covers
# all the players of the opponent team.
def obtainRectangle(opponentPosX,opponentPosY) :
	rectangle = []

	minX = 70
	maxX = -70
	minY = 70
	maxY = -70

	for i in range(1,11) :
		if (opponentPosX[i] > maxX) :
			maxX = opponentPosX[i]
		if (opponentPosX[i] < minX) :
			minX = opponentPosX[i]
		if (opponentPosY[i] > maxY) :
			maxY = opponentPosY[i]
		if (opponentPosY[i] < minY) :
			minY = opponentPosY[i]

	rectangle.append([minX,maxY])
	rectangle.append([maxX,maxY])
	rectangle.append([minX,minY])
	rectangle.append([maxX,minY])

	return rectangle

# Add 1 to each area of the field and its neighbors 
# for each opponent standing in it.
def sumPositions(field,areaWidth,areaHeight,mappedPos) :
	posX = 0
	posY = 0
	i = 0
	for pos in mappedPos :
		posX = round(pos[0] / areaWidth)
		posY = -1*round(pos[1] / areaHeight)
		if (posX < 0) :
			posX += 18
		else :
			posX += 16
		if (posY > 0) :
			posY += 16
		else :
			posY += 17
		# print("Test")
		# print(str(pos[0]) + " " + str(pos[1]))
		# print(str(posX) + " " + str(posY))
		field[posY][posX][i] += 1
		if (posY < 33) :
			field[posY+1][posX][i] += 1
		if (posY > 0) :
			field[posY-1][posX][i] += 1
		if (posX < 34) :
			field[posY][posX+1][i] += 1
		if (posX > 0) :
			field[posY][posX-1][i] += 1

		if (posY < 33) and (posX < 34) :
			field[posY+1][posX+1][i] += 1
		if (posY > 0) and (posX > 0) :
			field[posY-1][posX-1][i] += 1
		if (posY > 0) and (posX < 34) :
			field[posY-1][posX+1][i] += 1
		if (posY < 33) and (posX > 0) :
			field[posY+1][posX-1][i] += 1
		i+=1

# Normalizes the values of the field.
def normalizeField(field) :
	maxElems = []
	for i in range(0,10) :
		maxElems.append(0)

	for i in range(0,34) :
		for j in range (0,35) :
			for k in range(0,10) :
				if (field[i][j][k] > maxElems[k]) :
					maxElems[k] = field[i][j][k]
	num = 0

	checks = []
	for x in range(0,10) :
		checks.append(False)

	for i in range(0,34) :
		for j in range (0,35) :
			for k in range(0,10): 
				field[i][j][k] = float('%.3f'%(field[i][j][k] / maxElems[k]))  # Truncating to 3 decimal places
				if (field[i][j][k] == 1) and not(checks[k]) :
					checks[k] = True
					field[i][j][k] = 2
					num += 1

def initializeFormField() :	
	formField = []
	for i in range(0,34) :
		formField.append([])
		for j in range(0,35) :
			formField[i].append(0)

	return formField

# Print the field
def printField(field) :
	print("------- PRINTING FIELD -----------")
	for i in range(0,34) :
		print(field[i])

	print("------- DONE PRINTING FIELD -----------")

# Calculates the formation.
def calculateFormation(opponentSide,matrizFinal) :
	offense = 0
	center = 0
	defense = 0
	opponentSide = "l"
	if (opponentSide == "l") :
		for k in range(0,10):
			for i in range(0,34) :
				for j in range(0,12):
					if (matrizFinal[k][i][j] == 2):
						defense += 1
				for j in range(12,24):
					if (matrizFinal[k][i][j] == 2):
						center += 1
				for j in range(24,35):
					if (matrizFinal[k][i][j] == 2):
						offense += 1

	elif (opponentSide == "r") :
		for k in range(0,10) :
			for i in range(0,34) :
				for j in range(0,12):
					if (matrizFinal[k][i][j] == 2):
						offense += 1
				for j in range(12,24):
					if (matrizFinal[k][i][j] == 2):
						center += 1
				for j in range(24,35):
					if (matrizFinal[k][i][j] == 2):
						defense += 1

	print("Opponent side : "+opponentSide)
	print(str(defense)+str(center)+str(offense))

# Checks the side of the opponent team in the given file
def checkSide(filename,teamname) :
	aux = re.split("-|vs|self.|_|\d+",filename)
	newAux = []
	for item in aux :
		if item != '' :
			newAux.append(item)

	i = 0
	while i < len(newAux) :
		if newAux[i] == teamname :
			if newAux[i+1] == '.rcg' :
				return "r"
			else :
				return "l"
		i+=1
	return "l"


##---- Main function ----##
# python3 logsCampo.py path teamname
if __name__ == "__main__":
	ball_velXOld = 0
	ball_velYOld = 0
	ball_velXNew = 0
	ball_velYNew = 0
	ball_posXOld = 0
	ball_posYOld = 0
	isInitialPos = True
	ownerOld = ""
	ownerNew = ""
	kick_rand = []

	field = []
	for i in range(0,34) :
		field.append([])

	for i in range(0,34) :
		for j in range(0,35) :
			field[i].append([])
			for k in range(0,10) :
				field[i][j].append(0)

	teamname = sys.argv[2]
	cycle = 1

	fieldAreaWidth = 2.971428571
	fieldAreaHeight = 2

	for filename in glob.glob(sys.argv[1]+'*.rcg') :
		opponentSide = checkSide(filename,teamname)
		with open(filename) as file:
			for line in file:
				line = line.split()						# Split into a list
		
				if ((line[0] == "(show")):	# Is a (show) line
					opponentPosX, opponentPosY = [], []

					for i in range(11):
						unum = str(i+1) 

						# Extract opponent positions
						opponentPosX.append(extractPosInfo(opponentSide, unum, "pos.x", line))
						opponentPosY.append(extractPosInfo(opponentSide, unum, "pos.y", line))

					# Obtain the rectangle that contains all players (except goalie)
					rectangle = obtainRectangle(opponentPosX,opponentPosY) # [upLeftC,upRightC,botLeftC,botRightC]
					
					mappedPos = []
					# Obtain the rectangle's width and height
					rectWidth =  rectangle[1][0] - rectangle[0][0]
					rectHeight = rectangle[0][1] - rectangle[2][1]
					if (rectHeight == 0) or (rectWidth == 0) :
						print(filename)
						exit(1)
					# Map every opponent position inside the rectangle to the real field
					for i in range(1,11) :
						mappedPos.append(mapToField(rectangle[0][0],rectangle[0][1],
							rectWidth,rectHeight,opponentPosX[i],opponentPosY[i],opponentSide))
					
					sumPositions(field,fieldAreaWidth,fieldAreaHeight,mappedPos)

			file.close()

	normalizeField(field)
	matrizFinal = []
	for k in range(0,10):
		matrizFinal.append([])
		for i in range(0,34):
			matrizFinal[k].append([])
			for j in range(0,35):
				matrizFinal[k][i].append(field[i][j][k])

	calculateFormation(opponentSide,matrizFinal)

	# generate the colors for your colormap
	color1 = colorConverter.to_rgba('white')
	color2 = colorConverter.to_rgba('black')

	# make the colormaps
	cmap1 = mpl.colors.LinearSegmentedColormap.from_list('my_cmap',['green','blue'],256)
	cmap2 = mpl.colors.LinearSegmentedColormap.from_list('my_cmap2',[color1,color2],256)

	cmap2._init() # create the _lut array, with rgba values

	# create your alpha array and fill the colormap with them.
	# here it is progressive, but you can create whathever you want
	alphas = np.linspace(0, 0.8, cmap2.N+3)
	cmap2._lut[:,-1] = alphas
	img1 = plt.imshow(matrizFinal[0],cmap=cmap2,interpolation='nearest',origin='lower')
	img2 = plt.imshow(matrizFinal[1],cmap=cmap2,interpolation='nearest',origin='lower')
	img3 = plt.imshow(matrizFinal[2],cmap=cmap2,interpolation='nearest',origin='lower')
	img4 = plt.imshow(matrizFinal[3],cmap=cmap2,interpolation='nearest',origin='lower')
	img5 = plt.imshow(matrizFinal[4],cmap=cmap2,interpolation='nearest',origin='lower')
	img6 = plt.imshow(matrizFinal[5],cmap=cmap2,interpolation='nearest',origin='lower')
	img7 = plt.imshow(matrizFinal[6],cmap=cmap2,interpolation='nearest',origin='lower')
	img8 = plt.imshow(matrizFinal[7],cmap=cmap2,interpolation='nearest',origin='lower')
	img9 = plt.imshow(matrizFinal[8],cmap=cmap2,interpolation='nearest',origin='lower')
	img10 = plt.imshow(matrizFinal[9],cmap=cmap2,interpolation='nearest',origin='lower')
	# img11 = plt.imshow(matrizFinal[10],cmap=cmap2,interpolation='nearest',origin='lower')
	plt.show()	

			

		



			


