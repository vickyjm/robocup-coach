from math import sqrt
import re
import sys
from matplotlib import pyplot as plt
import numpy as np
from PIL import Image
import glob

features = 0

def generateMatrix(filename) :
	matrix = []
	with open(filename) as file:
		for line in file:
			line = line.split(",")
			for i in range(len(line)) :
				line[i] = float(line[i])
			matrix.append(line)
		file.close()

	return matrix

def generateLists(featureMatrix,subsets) :
	maxList = []
	minList = []
	stepList = []
	for i in range(features) :
		maxList.append(-1000)
		minList.append(1000)
		stepList.append(0)

	for row in featureMatrix :
		for i in range(features) :
			if (row[i] > maxList[i]) :
				maxList[i] = row[i]
			if (row[i] < minList[i]) :
				minList[i] = row[i]


	for i in range(features) :
		stepList[i] = ((maxList[i] - minList[i]) / subsets)

	return maxList,minList,stepList

def initializeCounter(subsetCounter) :
	for i in range(5) :
		subsetCounter.append(0)

def obtainRowSubsets(row,minList,maxList,stepList,subsets) :
	newRow = []
	for i in range(features) :
		low = minList[i]
		high = minList[i] + stepList[i] + stepList[i]/10
		for j in range(subsets) :
			if (j != 0) :
				low = high
				high = high + stepList[i] + stepList[i]/10

			if (row[i] >= low) and (row[i] <= high) :
				newRow.append(j)
				break;

	newRow.append(row[features])
	return newRow

def checkEqualRows(subsetMatrix) :
	difRows1 = []
	difRows2 = []
	newMatrix = [row[:features] for row in subsetMatrix]
	rowsDribbles = [] # Para los dribbles distintos.
	i = 0

	for row in subsetMatrix :
		check = False
		if (row[features] == -1.0) :
			rowsDribbles.append(i)
		for row2 in difRows1 :
			if (row2 == row) :
				check = True
				break
		if not(check) :
			difRows1.append(row)
			if (row[features] == 1.0) :
				rowsDribbles.append(i)
		i += 1

	for row in newMatrix :
		check = False
		for row2 in difRows2 :
			if (row2 == row) :
				check = True
				break
		if not(check) :
			difRows2.append(row)

	finalMatrix = []
	for i in range(len(difRows2)) :
		finalMatrix.append([0,0])

	for i in range(len(newMatrix)) :
		for j in range(len(difRows2)) :
			if newMatrix[i] == difRows2[j] :
				if (subsetMatrix[i][features] == -1.0) :
					finalMatrix[j][0] += 1
				else :
					finalMatrix[j][1] += 1

	contradict = 0
	contradictTotal = 0
	for row in finalMatrix :
		if (row[0] > 0) and (row[1] > 0) :
			contradict += 1
			contradictTotal += row[0] + row[1]

	print("Numero de filas distintas contando clasificacion : " + str(len(difRows1)))
	print("Numero de filas distintas sin clasificacion : " + str(len(difRows2)))
	print("Numero de contradicciones : " + str(contradict))
	print("Contradicciones totales : " + str(contradictTotal)) 
	return finalMatrix, difRows2, rowsDribbles

def findContrads(finalMatrix) :
	contradMatrix = []
	for i in range(len(finalMatrix)) :
		if (finalMatrix[i][0] > 0) and (finalMatrix[i][1] > 0) :
			contradMatrix.append(i)
	return contradMatrix

def generateNewDatRows(difRows,subsetMatrix,contrads) :
	newDatRows = []

	auxSubsetM = [row[:features] for row in subsetMatrix]

	for i in range(len(subsetMatrix)) :
		check = False
		for c in contrads :
			if (auxSubsetM[i] == difRows[c]) :
				check = True
				break
		if not(check) :
			newDatRows.append(i)
	return newDatRows

def generateNewDat(featureMatrix,newDatRows):
	newDat = []
	for i in range(len(featureMatrix)) :
		if (i in newDatRows) :
			newDat.append(featureMatrix[i])

	return newDat

# python3 passChecking.py filename subsets features outputFile outputFile2
# filname es el archivo .dat
# subsets recomiendo 5. 10 es mucho y termina por haber muy pocas contradicciones.
# features es el  numero de features
# outputFile es el nuevo .dat con las contradicciones eliminadas.
# outputFile2 es el nuevo .dat con solo 1 caso para cada elemento successful (para los dribles)
# El output en pantalla (numero total de filas,contradicciones,etc) si se quiere en un archivo se puede
# usar >> despues del comando.
if __name__ == "__main__":

	filename = sys.argv[1]
	subsets = int(sys.argv[2])
	features = int(sys.argv[3])
	outputFile = sys.argv[4]
	outputFile2 = sys.argv[5]

	print("Team : " + filename)
	print("Subsets : " + str(subsets))

	featureMatrix = generateMatrix(filename)
	maxList,minList,stepList = generateLists(featureMatrix,subsets)

	subsetCounter = []
	subsetMatrix = []
	for row in featureMatrix :
		initializeCounter(subsetCounter)
		subsetMatrix.append(obtainRowSubsets(row,minList,maxList,stepList,subsets))

	finalMatrix,difRows,rowsDribbles = checkEqualRows(subsetMatrix)
	print("Numero total de filas : " + str(len(subsetMatrix)))
	
	contrads = findContrads(finalMatrix)

	newDatRows = generateNewDatRows(difRows,subsetMatrix,contrads)

	newDat = generateNewDat(featureMatrix,newDatRows)
	newDat2 = generateNewDat(featureMatrix,rowsDribbles)

	outFile = open(outputFile,'w+')
	j = 0
	for row in newDat :
		j = 0
		for elem in row :
			j += 1
			outFile.write(str(elem))
			if (j != features+1) :
				outFile.write(',')
		outFile.write('\n')

	outFile2 = open(outputFile2,'w+')
	j = 0
	for row in newDat2 :
		j = 0
		for elem in row :
			j += 1
			outFile2.write(str(elem))
			if (j != features+1) :
				outFile2.write(',')
		outFile2.write('\n')


	



