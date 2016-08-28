#include <iostream>
#include <math.h>
#include <string>
#include "cv.h"
#include "ml.h"
#include "opencv2/core/core_c.h"

using namespace cv;
using namespace std;

int main(int argc, char* argv[]) {

    float trainPortion = 0.7;   
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
        strcpy(outFile,"shotTree.yml");
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


    CvMLData cvml;                                  // Structure to keep the data
    cvml.read_csv(argv[2]);                         // Read the file
    cvml.set_response_idx (9);                      // Indicate which column corresponds to the class
    cvml.change_var_type(9, CV_VAR_CATEGORICAL);    // The output is categorical 

    CvTrainTestSplit spl(trainPortion);
    cvml.set_train_test_split(&spl); // The mix flag is set to true by default. Used to mix test and training samples
                                     // so it doesn't use them in the same order that's given.

    CvDTree dtree;

    dtree.train(&cvml,CvDTreeParams());    // Train the tree only using the test set

    // cout << dtree.calc_error(&cvml,CV_TEST_ERROR) << endl;
    dtree.save(outFile);

    free(outFile);

    return 0;
}