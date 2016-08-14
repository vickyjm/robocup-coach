from math import sqrt
import sys


def nearestTeammate(px, py, teammates):
	minDist = sqrt(pow(px - teammates[0][0], 2) + pow(py - teammates[0][1], 2))
	for i in range(1,3):
		dist = sqrt(pow(px - teammates[i][0], 2) + pow(py - teammates[i][1], 2))
		if (minDist > dist):
			minDist = dist

	return minDist

##---- Main function ----##
if __name__ == "__main__":

	##---- Outputs files ----##
	shotFile = open("shotFile.dat", "w") 
	dribbleFile = open("dribbleFile.dat", "w")	
	passFile = open("passFile.dat", "w")
	opponentFile = open("opponentFile.dat", "w")

	with open(sys.argv[1]) as file:
		for line in file:
			teammates = []
			opponents = []
			originalDist = []

			line = line.split()						# Split into a list

			##---- Normalize ball's position ----##
			old_bx = float(line[0])
			old_by = float(line[1])
			bx = old_bx/5
			by = old_by/5

			i = 2
			while (i <= 6):
				originalDist.append([float(line[i]), float(line[i+1])])
				dist = sqrt(pow(old_bx - float(line[i]), 2) + pow(old_by - float(line[i+1]), 2))
				teammates.append(dist)
				i = i + 2
			while (i <= 15):
				nearestPlayer = nearestTeammate(float(line[i]), float(line[i+1]), originalDist)
				opponents.append(nearestPlayer)
				i = i + 2

			##---- Actions ----##
			if (line[i] == "GOAL"):
				shotFile.write(str(bx) + " " + str(by) + " ")
				opponentFile.write(str(bx) + " " + str(by) + " ")
				for j in range(0,3):
					shotFile.write(str(teammates[j]) + " ")
					opponentFile.write(str(teammates[j]) + " ")
				for j in range(0,4):
					shotFile.write(str(opponents[j]) + " ")
					opponentFile.write(str(opponents[j]) + " ")
				shotFile.write(line[i] + "\n")
				opponentFile.write("SHOOT\n")
			elif (line[i] == "PASS"):
				passFile.write(str(bx) + " " + str(by) + " ")
				opponentFile.write(str(bx) + " " + str(by) + " ")
				for j in range(0,3):
					passFile.write(str(teammates[j]) + " ")
					opponentFile.write(str(teammates[j]) + " ")
				for j in range(0,4):
					passFile.write(str(opponents[j]) + " ")
					opponentFile.write(str(opponents[j]) + " ")
				passFile.write(line[i] + "\n")
				opponentFile.write(line[i] + "\n")
			elif (line[i] == "DRIBBLE"):
				dribbleFile.write(str(bx) + " " + str(by) + " ")
				opponentFile.write(str(bx) + " " + str(by) + " ")
				for j in range(0,3):
					dribbleFile.write(str(teammates[j]) + " ")
					opponentFile.write(str(teammates[j]) + " ")
				for j in range(0,4):
					dribbleFile.write(str(opponents[j]) + " ")
					opponentFile.write(str(opponents[j]) + " ")
				dribbleFile.write(line[i] + "\n")
				opponentFile.write(line[i] + "\n")
			elif (line[i] == "UNSUCCESSFULSHOOT"):
				shotFile.write(str(bx) + " " + str(by) + " ")
				opponentFile.write(str(bx) + " " + str(by) + " ")
				for j in range(0,3):
					shotFile.write(str(teammates[j]) + " ")
					opponentFile.write(str(teammates[j]) + " ")
				for j in range(0,4):
					shotFile.write(str(opponents[j]) + " ")
					opponentFile.write(str(opponents[j]) + " ")
				shotFile.write(line[i] + "\n")
				opponentFile.write("SHOOT\n")
			elif (line[i] == "UNSUCCESSFULDRIBBLE"):
				dribbleFile.write(str(bx) + " " + str(by) + " ")
				opponentFile.write(str(bx) + " " + str(by) + " ")
				for j in range(0,3):
					dribbleFile.write(str(teammates[j]) + " ")
					opponentFile.write(str(teammates[j]) + " ")
				for j in range(0,4):
					dribbleFile.write(str(opponents[j]) + " ")
					opponentFile.write(str(opponents[j]) + " ")
				dribbleFile.write(line[i] + "\n")
				opponentFile.write("DRIBBLE\n")
			elif (line[i] == "UNSUCCESSFULPASS"):
				passFile.write(str(bx) + " " + str(by) + " ")
				opponentFile.write(str(bx) + " " + str(by) + " ")
				for j in range(0,3):
					passFile.write(str(teammates[j]) + " ")
					opponentFile.write(str(teammates[j]) + " ")
				for j in range(0,4):
					passFile.write(str(opponents[j]) + " ")
					opponentFile.write(str(opponents[j]) + " ")
				passFile.write(line[i] + "\n")
				opponentFile.write("PASS\n")

	shotFile.close()
	passFile.close()
	dribbleFile.close()
	opponentFile.close()		
