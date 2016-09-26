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

int filterValue(float ans){
    if (ans >= 0.5)
        return 1;
    else
        return 0;
}

int main(int argc, char* argv[]) {

    char* outFile;

    if (argc < 5){
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

    int folds = atoi(argv[3]);
    int numSamples = atoi(argv[4]);
    cout << "Total samples : " << numSamples << endl;
    cout << "Folds : " << folds << endl;
    cout << "Using " << (numSamples/folds)*(folds-1) << " samples to train" << endl;
    cout << "and " << (numSamples/folds) << " samples to test" << endl;

    vector<int> trainingIndex;
    vector<int> testIndex;

    CvMLData cvml;                                  // Structure to keep the data
    cvml.read_csv(argv[2]);                         // Read the file
    cvml.set_response_idx (9);

    const Mat aux(cvml.get_values(),true);
    const Mat values = aux(Range::all(), Range(0,9));
    const Mat responses(cvml.get_responses(),true);
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
    int predict;

    int tp = 0;
    int tn = 0;
    int fp = 0;
    int fn = 0;
    int finalTrue = 0;
    int finalFalse = 0;
    float accuracy = 0;
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
            predict = filterValue(ans->value);
            if (predict == responsesT.at<float>(0,*it)) {
                if (predict == 0) {
                    true0++; // The tree correctly predicted a 0 (unsuccessful action)
                }
                else {
                    true1++; // The tree correctly predicted a 1 (successful action)
                }
            }
            else if (predict != responsesT.at<float>(0,*it)) {
                if ((predict == 0) && (responsesT.at<float>(0,*it) == 1)) {
                    false0++; // The tree predicted a 0, but it was really a 1.
                }
                else {
                    false1++; // The tree predicted a 1, but it was really a 0.
                }
            }
        }

        totalTrue = true0+true1;
        totalFalse = false0+false1;

        tp = tp + true1;
        tn = tn + true0;
        fp = fp + false1;
        fn = fn + false0;
        finalTrue = finalTrue + totalTrue;
        finalFalse = finalFalse + totalFalse;

    }

    tp = tp / folds;
    tn = tn / folds;
    fp = fp / folds;
    fn = fn / folds;
    finalTrue = finalTrue / folds;
    finalFalse = finalFalse / folds;

    cout << "----------*-------------" << endl; 
    cout << "True positives : " << tp << " - " << ((float)(tp*100)/(float)(finalTrue+finalFalse)) << '%' << endl;
    cout << "True negatives : " << tn << " - " << ((float)(tn*100)/(float)(finalTrue+finalFalse)) << '%' << endl;

    cout << "False positives : " << fp << " - " << ((float)(fp*100)/(float)(finalTrue+finalFalse)) << '%' << endl;
    cout << "False negatives : " << fn << " - " << ((float)(fn*100)/(float)(finalTrue+finalFalse)) << '%' << endl;

    //Sumar el error
    cout << "Total true : " << finalTrue << " - " << ((float)(finalTrue*100)/(float)(finalTrue+finalFalse)) << '%' << endl;
    cout << "Total false : " << finalFalse << " - " << ((float)(finalFalse*100)/(float)(finalTrue+finalFalse)) << '%' << endl;

    // CvDTree dtree;
    // CvDTreeParams params = CvDTreeParams();
    // dtree.train(&cvml,params);

    // for (int i = 0; i < numSamples; i++) {
    //     ans = dtree.predict(values.row(i));
    //     cout << ans->value << endl;
    //     if (ans->value == responsesT.at<int>(0,i)) {
    //         if (ans->value == 0) {
    //             true0++; // The tree correctly predicted a 0 (unsuccessful action)
    //         }
    //         else {
    //             true1++; // The tree correctly predicted a 1 (successful action)
    //         }
    //     }
    //     else if (ans->value != responsesT.at<int>(0,i)) {
    //         if ((ans->value == 0) && (responsesT.at<int>(0,i) == 1)) {
    //             false0++; // The tree predicted a 0, but it was really a 1.
    //         }
    //         else {
    //             false1++; // The tree predicted a 1, but it was really a 0.
    //         }
    //     }
    // }
    // totalTrue = true0+true1;
    // totalFalse = false0+false1;
    // std::cout << "Correctos : " << totalTrue << std::endl;
    // std::cout << "Incorrectos : " << totalFalse << std::endl;
    // std::cout << "Error : " << dtree.calc_error(&cvml,CV_TEST_ERROR) << std::endl;
    /*

    // cout << dtree.calc_error(&cvml,CV_TEST_ERROR) << endl;
    dtree.save(outFile);

    free(outFile);*/

    return 0;
}