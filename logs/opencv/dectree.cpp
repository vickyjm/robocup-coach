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
        cout << "Hey, you forget the log file or the type flag!" << endl;
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
    cvml.set_train_test_split(&spl);

    cv::Mat trainingSet = cvml.get_train_sample_idx();
    cv::Mat testSet = cvml.get_test_sample_idx();

    cout << trainingSet << endl;

    cout << testSet << endl;

    //cout << cv::Mat(cvml.get_values()) << '\n';


    //decisiontree(trainingData, trainingClasses);

    return 0;
}