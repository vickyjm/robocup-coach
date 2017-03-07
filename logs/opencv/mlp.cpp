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
    else if (strcmp(argv[1], "-o") == 0){
        outFile = (char*)malloc(17);
        strcpy(outFile,"opponentMLP.yml");
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
    int isSigmoid = atoi(argv[8]);
    //int numTest = atoi(argv[8]);
    /*cout << "Total samples : " << numSamples << endl;
    cout << "Folds : " << folds << endl;
    cout << "Using " << (numSamples*(folds-1))/folds << " samples to train" << endl;
    cout << "and " << numSamples/folds << " samples to test" << endl;
    //cout << "Validating with " << numTest << endl;*/

    vector<int> trainingIndex;
    vector<int> testIndex;

    CvMLData cvml;                                  // Structure to keep the data
    cvml.read_csv(argv[2]);                         // Read the file
    cvml.set_response_idx (features);

    const Mat aux(cvml.get_values(),true);
    const Mat values = aux(Range::all(), Range(0,features));
    const Mat responses(cvml.get_responses(),true);

    int* index = kfold(numSamples, folds); //Esto se comenta si no se usa cross-validation

    // Testing related stuff :
    vector<int>::iterator
    it; // Iterator for testIndex
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

    //Este for se comenta para las redes finales, lo de adentro no
    for (int i = 0; i!=folds; i++){

        trainingIndex = chooseIndex(index, 0, 0, (numSamples*(folds-1))/folds, numSamples); // Solo cuando usamos cross-validation
        testIndex = chooseIndex(index, 1, i, numSamples/folds, numSamples);                 // Solo cuando usamos cross-validation

        const Mat training = Mat(trainingIndex);          // Solo cuando usamos cross-validation

        cv::Mat layers = cv::Mat(3, 1, CV_32SC1);

        layers.row(0) = cv::Scalar(features); //Neuronas de la capa de entrada
        layers.row(1) = cv::Scalar(nHidden); //Neuronas de la capa oculta
        layers.row(2) = cv::Scalar(1); //Neuronas de la capa de salida

        CvANN_MLP mlp;
        CvANN_MLP_TrainParams params = CvANN_MLP_TrainParams(
          // terminate the training after either 1000
          // iterations or a very small change in the
          // network wieghts below the specified value
          cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, epochs, 0.0000001),

          // use backpropogation for training
          CvANN_MLP_TrainParams::BACKPROP,

          // co-efficents for backpropogation training
          // (refer to manual)
          0.1,
          0.1);


        if (isSigmoid == 1){
            mlp.create(layers,CvANN_MLP::SIGMOID_SYM, 0.6,1);  
        }
        else {
            mlp.create(layers,CvANN_MLP::GAUSSIAN, 0.6,1);    
        }
        

        // train
        mlp.train(values, responses, cv::Mat(), training, params, CvANN_MLP::NO_INPUT_SCALE);      // Esta se usa cuando estamos usando cross-validation
        //mlp.train(values, responses, cv::Mat(), cv::Mat(), params, CvANN_MLP::NO_INPUT_SCALE);   // Esta se usa cuando queremos generar la red final para probar los de control
        
        
        //Probar
        true0 = 0;           
        true1 = 0;           
        false0 = 0;          
        false1 = 0;         
        totalTrue = 0;       
        totalFalse = 0;   

        // Este ciclo debe comentarse cuando se quieren generar las redes finales
        for (it=testIndex.begin() ; it < testIndex.end(); ++it) {
            cv::Mat predAux(1, 1, CV_32FC1);
            cv::Mat predicted(numSamples/folds, 1, CV_32F);
            mlp.predict(values.row(*it), predAux);
            //predicted.at<float>(i,0) = predAux.at<float>(0,0);
            //std::cout << predAux.at<float>(0,0) << std::endl;
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
        
    }

    cout << "----------*-------------" << endl; 
    cout << "True positives : " << tp  << endl;
    cout << "True negatives : " << tn  << endl;

    cout << "False positives : " << fp  << endl;
    cout << "False negatives : " << fn  << endl;

    //Sumar el error
    cout << "Total true : " << finalTrue << " - " << ((float)((finalTrue)*100)/(float)(numSamples)) << '%' << endl;
    cout << "Total false : " << finalFalse << " - " << ((float)((finalFalse)*100)/(float)(numSamples)) << '%' << endl;


    //Para generar la red para luego usarla con control
    cv::Mat layers = cv::Mat(3, 1, CV_32SC1);

    layers.row(0) = cv::Scalar(features); //Neuronas de la capa de entrada
    layers.row(1) = cv::Scalar(nHidden); //Neuronas de la capa oculta
    layers.row(2) = cv::Scalar(1); //Neuronas de la capa de salida

    CvANN_MLP mlp;
    CvANN_MLP_TrainParams params = CvANN_MLP_TrainParams(
      // terminate the training after either 1000
      // iterations or a very small change in the
      // network wieghts below the specified value
      cvTermCriteria(CV_TERMCRIT_ITER|CV_TERMCRIT_EPS, epochs, 0.0000001),

      // use backpropogation for training
      CvANN_MLP_TrainParams::BACKPROP,

      // co-efficents for backpropogation training
      // (refer to manual)
      0.1,
      0.1);


    if (isSigmoid == 1){
        mlp.create(layers,CvANN_MLP::SIGMOID_SYM, 0.6,1);  
    }
    else {
        mlp.create(layers,CvANN_MLP::GAUSSIAN, 0.6,1);    
    }
    

    // train
    mlp.train(values, responses, cv::Mat(), cv::Mat(), params, CvANN_MLP::NO_INPUT_SCALE);   // Esta se usa cuando queremos generar la red final para probar los de control

    mlp.save(outFile);  // Descomentar este save cuando se quieren generar las redes finales

    free(outFile);

    return 0;
}