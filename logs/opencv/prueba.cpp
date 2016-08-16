#include <iostream>
#include <math.h>
#include <string>
#include "cv.h"
#include "ml.h"
#include "highgui.h"
#include <fstream>

using namespace cv;
using namespace std;

// Mat ReadMatFromTxt(string filename, int rows,int cols)
// {
//     double m;
//     Mat out = Mat::zeros(rows, cols, CV_64FC1);//Matrix to store values

//     ifstream fileStream(filename);
//     int cnt = 0;//index starts from 0
//     while (fileStream >> m)
//     {
//         int temprow = cnt / cols;
//         int tempcol = cnt % cols;
//         out.at<double>(temprow, tempcol) = m;
//         cnt++;
//     }
//     return out;
// }

// void printMat(CvMat* mat, char* filename)
// {
//     printf("(%dx%d)\n",mat->cols,mat->rows);
//     for(int i=0; i<mat->rows; i++)
//     {
//         if(i==0)
//         {
//             for(int j=0; j<mat->cols; j++)  fprintf(pf,"%10d",j+1);
//         }

//         fprintf(pf,"\n%4d: ",i+1);
//         for(int j=0; j<mat->cols; j++)
//         {

//             fprintf(pf,"%10.2f",cvGet2D(mat,i,j).val[0]);
//         }
//     }
//     fflush(pf);
//     fclose(pf);
// }

int main() {
	// cv::Mat trainingData = ReadMatFromTxt("opponentFile.dat",630,9);
	// cout << trainingData << endl;

    /* STEP 2. Opening the file */
    //1. Declare a structure to keep the data
    CvMLData cvml;
    //2. Read the file
    cvml.read_csv ("opponentFile.dat");
    //3. Indicate which column is the response
    cvml.set_response_idx (9);

    // std::cout << cv::Mat(cvml.get_values()) << '\n';

    CvDTreeParams params = CvDTreeParams();

    CvDTree* dtree = new CvDTree();
    dtree->train(&cvml,params);

    cv::Mat testData(100, 10, CV_32FC1);
    cv::randu(testData,0,5);
    const cv::Mat sample = testData.row(0);

    std::cout<< dtree->predict(sample)->value << '\n';
	return 0;
}