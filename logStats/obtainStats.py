from math import sqrt
import re
import sys
from operator import itemgetter
import glob
import os


if __name__ == "__main__":

	fileName = sys.argv[1]

	controlTotal = 50;
	controlGoalDif = 0;
	controlOurPos = 0;
	controlPosProm = 0;
	controlOurGoals = 0;
	controlOppGoals = 0;

	exp1Total = 50;
	exp1OurPos = 0;
	exp1GoalDif = 0;
	exp1PosProm = 0;
	exp1OurGoals = 0;
	exp1OppGoals = 0;

	with open(fileName) as file:
		for line in file:
			line = line.split()	
			# print(line)
			if (line[0] != "#") :
				if (line[2] == "control") :
					controlGoalDif += int(line[5])
					controlOurPos += float(line[3])
					controlOurGoals += int(line[6])
					controlOppGoals += int(line[7])
				elif (line[2] == "exp1") :
					exp1GoalDif += int(line[5])
					exp1OurPos += float(line[3])
					exp1OurGoals += int(line[6])
					exp1OppGoals += int(line[7])

	controlPosProm += controlOurPos
	controlPosProm = controlPosProm / controlTotal

	exp1PosProm += exp1OurPos
	exp1PosProm = exp1PosProm / exp1Total


	print(fileName)
	print("Control stuff \n")
	print("Promedio de posesion : " + str(controlPosProm))
	print("Total de diferencia de goles : " + str(controlGoalDif))
	print("Nuestros goles : " + str(controlOurGoals))
	print("Goles oponentes : " + str(controlOppGoals))
	print("\n")

	print("Exp1 stuff \n")
	print("Promedio de posesion : " + str(exp1PosProm))
	print("Total de diferencia de goles : " + str(exp1GoalDif))
	print("Nuestros goles : " + str(exp1OurGoals))
	print("Goles oponentes : " + str(exp1OppGoals))
	print("\n")


