#include <iostream>
#include <math.h>
#include <string>
#include "cv.h"
#include "ml.h"
#include "opencv2/core/core_c.h"
#include <vector>
 
using namespace cv;
using namespace std;

int* kfold( int size, int k )
{
    int* indices = new int[ size ];

    for (int i = 0; i < size; i++ )
        indices[i] = i%k;

    sort(indices, indices + size);

    return indices;
}

vector<int> chooseIndex(int* index, int flag, int iter, int size, int numSamples){
    vector<int> res(size);
    int actual = 0;

    if (flag == 0){
        for (int i = 0; i < numSamples; i++){
            if (index[i] != iter){
                res[actual] = i;
                actual++;
            }
        }

    } else {
        for (int i = 0; i < numSamples; i++){
            if (index[i] == iter){
                res[actual] = i;
                actual++;
            }
        }

    }
    return res;
}

int main(int argc, char* argv[]) {

    char* outFile;

    if (argc < 3){
        cout << "Hey, you forgot the log file or the type flag!" << endl;
        return 0;
    }

    if (strcmp(argv[1], "-d") == 0){
        outFile = (char*)malloc(15);
        strcpy(outFile,"dribbleTree.yml");
    } 
    else if (strcmp(argv[1], "-s") == 0){
        outFile = (char*)malloc(13);
        strcpy(outFile,"shootTree.yml");
    }
    else if (strcmp(argv[1], "-p") == 0){
        outFile = (char*)malloc(13);
        strcpy(outFile,"passTree.yml");
    }
    else if (strcmp(argv[1], "-o") == 0){
        outFile = (char*)malloc(17);
        strcpy(outFile,"opponentTree.yml");
    }
    else {
        cout << "Please enter a valid flag!" << endl;
        return 0;
    }

    int folds = 5;
    int numSamples = 144036;
    vector<int> trainingIndex;
    vector<int> testIndex;

    CvMLData cvml;                                  // Structure to keep the data
    cvml.read_csv(argv[2]);                         // Read the file
    cvml.set_response_idx (9);

    const Mat values(cvml.get_values(), true);
    const Mat responses(cvml.get_responses(), true);
    const Mat responsesT(responses.t());


    int* index = kfold(numSamples, folds);

    // Testing related stuff :
    vector<int>::iterator it; // Iterator for testIndex
    CvDTreeNode* ans;        // Prediction obtained from the tree
    int true0 = 0;           // Correctly classified 0s   
    int true1 = 0;           // Correctly classified 1s
    int false0 = 0;          // Incorrectly classified 0s
    int false1 = 0;          // Incorrectly classified 1s
    int totalTrue = 0;       // Total number of correctly classified actions
    int totalFalse = 0;      // Total number of incorrectly classified actions

    for (int i = 0; i!=folds; i++){
        CvDTree dtree;

        trainingIndex = chooseIndex(index, 0, i, (numSamples*(folds-1))/folds, numSamples);
        testIndex = chooseIndex(index, 1, i, numSamples/folds, numSamples);

        const Mat varIdx = Mat();
        const Mat training = Mat(trainingIndex);

        // Train the tree only with the training samples
        dtree.train(values, CV_ROW_SAMPLE, responsesT, varIdx, training);
        
        //Probar
        true0 = 0;           
        true1 = 0;           
        false0 = 0;          
        false1 = 0;         
        totalTrue = 0;       
        totalFalse = 0;      
        for (it=testIndex.begin() ; it < testIndex.end(); ++it) {
            ans = dtree.predict(values.row(*it));

            if (ans->value == responsesT.at<int>(0,*it)) {
                if (ans->value == 0) {
                    true0++; // The tree correctly predicted a 0 (unsuccessful action)
                }
                else {
                    true1++; // The tree correctly predicted a 1 (successful action)
                }
            }
            else if (ans->value != responsesT.at<int>(0,*it)) {
                if ((ans->value == 0) && (responsesT.at<int>(0,*it) == 1)) {
                    false0++; // The tree predicted a 0, but it was really a 1.
                }
                else {
                    false1++; // The tree predicted a 1, but it was really a 0.
                }
            }
        }

        totalTrue = true0+true1;
        totalFalse = false0+false1;

        //Sumar el error
        std::cout << "Correctos : " << totalTrue << std::endl;
        std::cout << "Incorrectos : " << totalFalse << std::endl;

    }

    /*

    // cout << dtree.calc_error(&cvml,CV_TEST_ERROR) << endl;
    dtree.save(outFile);

    free(outFile);*/

    return 0;
}