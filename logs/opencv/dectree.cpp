#include <iostream>
#include <math.h>
#include <string>
#include "cv.h"
#include "ml.h"
#include "opencv2/core/core_c.h"

using namespace cv;
using namespace std;

// accuracy
float evaluate(cv::Mat& predicted, cv::Mat& actual) {
    assert(predicted.rows == actual.rows);
    int t = 0;
    int f = 0;
    for(int i = 0; i < actual.rows; i++) {
        float p = predicted.at<float>(i,0);
        float a = actual.at<float>(i,0);
        if((p >= 0.0 && a >= 0.0) || (p <= 0.0 &&  a <= 0.0)) {
            t++;
        } else {
            f++;
        }
    }
    return (t * 1.0) / (t + f);
}

void decisiontree(cv::Mat& trainingData, cv::Mat& trainingClasses) {

    CvDTree dtree;

    cv::Mat var_type(3, 1, CV_8U);

    // define attributes as numerical
    var_type.at<unsigned int>(0,0) = CV_VAR_NUMERICAL;
    var_type.at<unsigned int>(0,1) = CV_VAR_NUMERICAL;
    // define output node as numerical
    var_type.at<unsigned int>(0,2) = CV_VAR_NUMERICAL;

    dtree.train(trainingData,CV_ROW_SAMPLE, trainingClasses, cv::Mat(), cv::Mat(), var_type, cv::Mat(), CvDTreeParams());

    dtree.save("arbol.yml");
}


int main(int argc, char* argv[]) {

    int numTrainingLogs = 1;
    float trainPortion = 0.7;   

    if (argc < 3){
        cout << "Hey, you forgot the log file or the type flag!" << endl;
        return 0;
    }

    if ((strcmp(argv[1], "-d") != 0) && (strcmp(argv[1], "-s") != 0)
            && (strcmp(argv[1], "-p") != 0) && (strcmp(argv[1], "-o") != 0)){
        
        cout << "Please enter a valid flag!" << endl;
        return 0;
    }

    CvMLData cvml;                          // Structure to keep the data
    cvml.read_csv(argv[2]);                 // Read the file
    cvml.set_response_idx (9);              // Indicate which column corresponds to the class

    CvTrainTestSplit spl(trainPortion);
    cvml.set_train_test_split(&spl); // The mix flag is set to true by default. Used to mix test and training samples
                                     // so it doesn't use them in the same order that's given.

    cv::Mat trainingSet = cvml.get_train_sample_idx();
    cv::Mat testSet = cvml.get_test_sample_idx();

    cout << trainingSet << endl;
    // To access a column of a cv::Mat use trainingSet.col(n) where n is the column
    // To access a row use .row(n)
    cout << testSet << endl;

    cv::Mat trainingData = Mat(trainingSet.rows,10,CV_32FC1);
    cv::Mat trainingClasses = Mat(1,trainingSet.rows,CV_32FC1);
    // cv::Mat cvmlMat = Mat(cvml.get_values(),true); // Transforming cvml into a Mat

    // int i = 0;
    // while (i < trainingSet.rows) { // The idea is to create the TrainingData and TrainingClasses using the ids from TrainingSet
    //     trainingData.row(i) = cvmlMat.row(trainingSet.at<int>(0,i));
    //     trainingClasses.at<int>(0,i) = cvmlMat.at(cvmlMat.row(trainingSet.at<int>(0,i)),9);
    //     i++;
    // }
    //cout << cv::Mat(cvml.get_values()) << '\n';


    // decisiontree(trainingSet, trainingClasses);

    return 0;
}