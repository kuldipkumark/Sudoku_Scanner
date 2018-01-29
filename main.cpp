#include "sudoku.h"
#include <iostream>
#include <opencv2/opencv.hpp>
#include "opencv2/ml.hpp"

using namespace std;
using namespace cv;
using namespace ml;

int main()
{
	sudoku obj1;
	Mat image = imread("index.jpg", IMREAD_GRAYSCALE);
	if(image.empty())
	{
		cout << "Cannot read the image\n";
		return -1;
	}


	Mat box = obj1.preproces(image);

	vector<Mat> small_boxs = obj1.small_box(box);

	Ptr<KNearest> kclassifier = obj1.train_data();

	obj1.solve(small_boxs, kclassifier);


	return 0;
}