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
    cout << "Using " << (numSamples*(folds-1))/folds << " samples to train" << endl;
    cout << "and " << numSamples/folds << " samples to test" << endl;

    vector<int> trainingIndex;
    vector<int> testIndex;

    CvMLData cvml;                                  // Structure to keep the data
    cvml.read_csv(argv[2]);                         // Read the file
    //cvml.change_var_type(9, CV_VAR_CATEGORICAL);    // The output is categorical
    cvml.set_response_idx (9);
    /*cvml.change_var_type(24, CV_VAR_CATEGORICAL);    // The output is categorical
    cvml.set_response_idx (24);*/
    /*cvml.change_var_type(17, CV_VAR_CATEGORICAL);    // The output is categorical
    cvml.set_response_idx (17);*/
    /*cvml.change_var_type(23, CV_VAR_CATEGORICAL);    // The output is categorical
    cvml.set_response_idx (23);*/
    /*cvml.change_var_type(11, CV_VAR_CATEGORICAL);    // The output is categorical
    cvml.set_response_idx (11);*/
    /*cvml.change_var_type(7, CV_VAR_CATEGORICAL);    // The output is categorical
    cvml.set_response_idx (7);*/
    /*cvml.change_var_type(26, CV_VAR_CATEGORICAL);    // The output is categorical
    cvml.set_response_idx (26);*/

    const Mat aux(cvml.get_values(),true);
    const Mat values = aux(Range::all(), Range(0,9));
    //const Mat values = aux(Range::all(), Range(0,24));
    //const Mat values = aux(Range::all(), Range(0,23));
    //const Mat values = aux(Range::all(), Range(0,26));
    //const Mat values = aux(Range::all(), Range(0,11));
    //const Mat values = aux(Range::all(), Range(0,7));
    const Mat responses(cvml.get_responses(),true);
    //const Mat responsesT(responses.t());

    int* index = kfold(numSamples, folds);

    // Testing related stuff :
    vector<int>::iterator it; // Iterator for testIndex
    //CvDTreeNode* ans;        // Prediction obtained from the tree
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
    double predValue;

    //std::cout << responses << std::endl;

    //Define training data storage matrices (one for features samples and one for classifications)
    /*Mat trainingData = Mat(numSamples*(folds-1)/folds,9,CV_32FC1);
    Mat trainingClasses = Mat::zeros(numSamples*(folds-1)/folds,2,CV_32FC1);

    //Define testing data storage matrices
    Mat testData = Mat(numSamples/folds,9,CV_32FC1);
    Mat testClasses = Mat(numSamples/folds,2,CV_32FC1);*/

    for (int i = 0; i!=folds; i++){
        CvDTree dtree;

        trainingIndex = chooseIndex(index, 0, i, (numSamples*(folds-1))/folds, numSamples);
        testIndex = chooseIndex(index, 1, i, numSamples/folds, numSamples);

        //const Mat varIdx = Mat();
        //const Mat missing = Mat();
        const Mat training = Mat(trainingIndex);

        cv::Mat layers = cv::Mat(3, 1, CV_32SC1);

        layers.row(0) = cv::Scalar(9); //Neuronas de la capa de entrada
        layers.row(1) = cv::Scalar(18); //Neuronas de la capa oculta
        layers.row(2) = cv::Scalar(1); //Neuronas de la capa de salida

        CvANN_MLP mlp;
        CvANN_MLP_TrainParams params = CvANN_MLP_TrainParams(
          // terminate the training after either 1000
          // iterations or a very small change in the
          // network wieghts below the specified value
          cvTermCriteria(CV_TERMCRIT_ITER, 1000, 0.000001),

          // use backpropogation for training
          CvANN_MLP_TrainParams::BACKPROP,

          // co-efficents for backpropogation training
          // (refer to manual)
          0.1,
          0.1);

        mlp.create(layers,CvANN_MLP::SIGMOID_SYM, 0.6,1);

        // train
        mlp.train(values, responses, cv::Mat(), training, params);
        
        
        //Probar
        true0 = 0;           
        true1 = 0;           
        false0 = 0;          
        false1 = 0;         
        totalTrue = 0;       
        totalFalse = 0;    

        for (it=testIndex.begin() ; it < testIndex.end(); ++it) {
            cv::Mat predAux(1, 1, CV_32FC1);
            cv::Mat predicted(numSamples/folds, 1, CV_32F);

            mlp.predict(values.row(*it), predAux);
            //predicted.at<float>(i,0) = predAux.at<float>(0,0);

            //std::cout << predAux.at<float>(0,0) << std::endl;

            /*cout << "Accuracy_{MLP} = " << evaluate(predicted, testClasses) << endl;

            predict = ans->value;*/

            if (predAux.at<float>(0,0) >= 0){
                predValue = 1.0;
            } else {
                predValue = -1.0;
            }

            if (predValue == responses.at<float>(*it,0)) {
                if (predValue == -1.0) {
                    true0++; // The tree correctly predicted a 0 (unsuccessful action)
                }
                else {
                    true1++; // The tree correctly predicted a 1 (successful action)
                }
            }
            else if (predValue != responses.at<float>(*it,0)) {
                if (predValue == -1.0) {
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

        //std::cout << "Variables:" << std::endl;
        //std::cout << Mat(dtree.get_var_importance()) << std::endl;
        
    }


    //tp = tp / folds;
    //tn = tn / folds;
    //fp = fp / folds;
    //fn = fn / folds;
    //finalTrue = finalTrue / folds;
    //finalFalse = finalFalse / folds;

    cout << "----------*-------------" << endl; 
    cout << "True positives : " << tp  << endl;
    cout << "True negatives : " << tn  << endl;

    cout << "False positives : " << fp  << endl;
    cout << "False negatives : " << fn  << endl;

    //Sumar el error
    cout << "Total true : " << finalTrue << " - " << ((float)(finalTrue*100)/(float)(numSamples)) << '%' << endl;
    cout << "Total false : " << finalFalse << " - " << ((float)(finalFalse*100)/(float)(numSamples)) << '%' << endl;

    return 0;
}