from sklearn import tree
import arff, numpy as np

dataset = arff.load(open('../opponentFile.arff'))
arffData = np.array(dataset['data'])

data = []
target = []
j = 0
for x in arffData :
	i = 0
	data.append([])
	while i < 10 :
		if (i <= 8) :
			data[j].append(x[i])
		elif (i == 9):
			target.append(x[i])
		i = i+1
	j = j+1

clf = tree.DecisionTreeClassifier()
clf = clf.fit(data, target)
# print(clf.predict([1,1,1,1,1,1,1,1,1]))