#include <GClasses/GMatrix.h>
#include <GClasses/GDecisionTree.h>

using namespace GClasses;
using std::cerr;
using std::cout;
using std::vector;
using std::endl;

int main(){
	GMatrix data;
	data.loadArff("../passFile.arff");				// Read the dataset
	data.swapColumns(1, data.cols() - 1);		// Swap the first column with the last one
	GDataColSplitter splitter(data, 2);			// Split the matrix into features and labels
	GMatrix& features = splitter.features();	// Extract the features
	GMatrix& labels = splitter.labels();		// Extract the labels

	GDecisionTree model;
	model.train(features, labels);				// Training the Decision Tree model

	GVec out(2);								// Predicted label vector
	model.predict(features[10], out);

}
	