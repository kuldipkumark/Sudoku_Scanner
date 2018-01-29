#ifndef SUDOKU
#define SUDOKU

#include "opencv2/ml.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
using namespace std;
using namespace cv;
using namespace ml;


class sudoku
{
public:
	sudoku();
	Mat preproces(Mat image);
	Mat outer_box(Mat box, Mat image);
	vector<Mat> small_box(Mat box);
	Ptr<KNearest> train_data();
	void solve(vector<Mat> small_boxs, Ptr<KNearest> kclassifier);

};

#endif