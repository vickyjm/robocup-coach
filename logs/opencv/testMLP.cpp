#include <iostream>
#include <math.h>
#include <string>
#include "cv.h"
#include "ml.h"
#include "opencv2/core/core_c.h"
#include <vector>
 
using namespace cv;
using namespace std;


int main(int argc, char* argv[]) {

    char* outFile;

    if (argc < 5){
        cout << "Hey, you forgot the log file or the type flag!" << endl;
        return 0;
    }

    if (strcmp(argv[1], "-d") == 0){
        outFile = (char*)malloc(15);
        strcpy(outFile,"dribbleMLP.yml");
    } 
    else if (strcmp(argv[1], "-s") == 0){
        outFile = (char*)malloc(13);
        strcpy(outFile,"shotMLP.yml");
    }
    else if (strcmp(argv[1], "-p") == 0){
        outFile = (char*)malloc(13);
        strcpy(outFile,"passMLP.yml");
    }
    else {
        cout << "Please enter a valid flag!" << endl;
        return 0;
    }

    int folds = atoi(argv[3]);
    int numSamples = atoi(argv[4]);
    int nHidden = atoi(argv[5]);
    int epochs = atoi(argv[6]);
    int features = atoi(argv[7]);
    cout << "Total samples : " << numSamples << endl;

    CvMLData cvml;                                  // Structure to keep the data
    cvml.read_csv(argv[2]);                         // Read the file
    cvml.set_response_idx (features);

    const Mat aux(cvml.get_values(),true);
    const Mat values = aux(Range::all(), Range(0,features));
    const Mat responses(cvml.get_responses(),true);


    // Testing related stuff :
    int tp = 0;
    int tn = 0;
    int fp = 0;
    int fn = 0;
    int finalTrue = 0;
    int finalFalse = 0;
    float accuracy = 0;
    double predValue;

    CvANN_MLP mlp;

    mlp.load(argv[8]);
    
    //Probar

    for (int it = 0; it < numSamples; it++) {
        cv::Mat predAux(1, 1, CV_32FC1);
        cv::Mat predicted(numSamples, 1, CV_32F);

        mlp.predict(values.row(it), predAux);

        if (predAux.at<float>(0,0) >= 0){
            predValue = 1.0;
        } else {
            predValue = -1.0;
        }

        if (predValue == responses.at<float>(it,0)) {
            if (predValue == -1.0) {
                tn++; // The tree correctly predicted a 0 (unsuccessful action)
            }
            else {
                tp++; // The tree correctly predicted a 1 (successful action)
            }
        }
        else if (predValue != responses.at<float>(it,0)) {
            if (predValue == -1.0) {
                fn++; // The tree predicted a 0, but it was really a 1.
            }
            else {
                fp++; // The tree predicted a 1, but it was really a 0.
            }
        }
    }

    finalTrue = tn+tp;
    finalFalse = fn+fp;


    cout << "----------*-------------" << endl; 
    cout << "True positives : " << tp  << endl;
    cout << "True negatives : " << tn  << endl;

    cout << "False positives : " << fp  << endl;
    cout << "False negatives : " << fn  << endl;

    //Sumar el error
    cout << "Total true : " << finalTrue << " - " << ((float)(finalTrue*100)/(float)(numSamples)) << '%' << endl;
    cout << "Total false : " << finalFalse << " - " << ((float)(finalFalse*100)/(float)(numSamples)) << '%' << endl;

    free(outFile);

    return 0;
}