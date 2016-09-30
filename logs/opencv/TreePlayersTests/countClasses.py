import sys

total0 = 0
total1 = 0

with open(sys.argv[1]) as file:
	for line in file:
		if (line[len(line)-2]=="0"):
			total0 += 1
		else:
			total1 += 1

print("Archivo: ", sys.argv[1])
print("Total 1's: ", total1)
print("Total 0's: ", total0)
