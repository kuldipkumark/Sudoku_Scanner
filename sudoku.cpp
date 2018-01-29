#include "opencv2/ml.hpp"
#include <opencv2/opencv.hpp>
#include <iostream>
#include <dirent.h>
#include "sudokusolver.h"
#include "sudoku.h"

using namespace cv;  
using namespace std;
using namespace ml;

sudoku::sudoku()
{

}

Mat sudoku::preproces(Mat image)
{
	GaussianBlur(image, image, Size(9, 9), 0, 0); // to smooth image before thresholding

	Mat box = Mat::zeros(image.size(), CV_8UC1);
	adaptiveThreshold(image, box, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 3, 2);// adaptive thresholding to create binary image
	
	Mat structEle = getStructuringElement(MORPH_CROSS, Size(3,3), Point(-1, -1));// structing element for dilating
	dilate(box, box, structEle);// to get proper lines
	
	return box;
}

Mat sudoku::outer_box(Mat box, Mat image)
{
	vector< vector<Point> > contours;// contan all the countours with their points
	vector<Vec4i> hierarchy;//

	findContours(box, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_SIMPLE);
	//namedWindow("temp", CV_WINDOW_NORMAL);
	int largest_area = 0;
	int largest_contour_index = 0;
	Rect bounding_rect;

	for(int i = 0; i < contours.size(); ++i)
	{
		double area = contourArea(contours[i], false);
		if(area > 100)
		{
		if(area > largest_area){
			largest_area = area;
			largest_contour_index = i;
		}
		}
	}
	//double perimeter = arcLength(contours[largest_contour_index], true);
	//approxPolyDP(contours[largest_contour_index], contours[largest_contour_index], 0.01*perimeter, true);
	Scalar color(255, 255, 255);
	Mat dst = Mat::zeros(box.size(), CV_8UC1);
	drawContours(dst, contours, largest_contour_index, color, CV_FILLED, 8, hierarchy);
	//drawContours(image, contours, largest_contour_index, Scalar(0, 25, 225), CV_FILLED, 8, hierarchy);
	//imshow("temp", image);
	Mat structEle = getStructuringElement(MORPH_CROSS, Size(3,3), Point(-1, -1));
	erode(dst, dst, structEle);

	return dst;
}
/*void extractouter()
{
		vector<Vec4i> lines;
    HoughLinesP(outerbox, lines, 1, CV_PI/180, 200);

    for(size_t i = 0; i <lines.size(); ++i)
    {
    	Vec4i l = lines[i];
        double theta1,theta2, hyp, result;

        theta1 = (l[3]-l[1]);
        theta2 = (l[2]-l[0]);
        hyp = hypot(theta1,theta2);

        line( image, Point(l[0], l[1]), Point(l[2], l[3]), Scalar(255,0,0), 3, CV_AA);
    }
    imshow("temp", image);
}*/
vector<Mat> sudoku::small_box(Mat box)
{
	vector<Mat> small_boxs;
	double small = box.rows/9.0;
	Mat small_img;
	for(double row = 0; row < box.rows; row += small)
	{
		for(double col = 0; col < box.cols; col += small)
		{
			small_img = Mat(box, Rect(col+15, row+15, small-20, small-20));
			resize(small_img, small_img, Size(16,16), 0, 0, INTER_NEAREST); 
			small_boxs.push_back(small_img);
		}
	}
	return small_boxs;
}

Ptr<KNearest> sudoku::train_data()
{
	int tot_image = 775, counter = 0;
	Mat trainData = Mat(Size(256, tot_image), CV_32FC1);
	Mat responses = Mat(Size(1, tot_image), CV_32FC1);
	
	for(int i = 0; i < 10; ++i)
	{
		DIR *dir;
		dirent *ent;
		char images_dir[] = "digit_pics/";
		char path[255];

		sprintf(path, "%s/%d", images_dir, i);
		namedWindow("temp", CV_WINDOW_NORMAL);

		dir = opendir(path);
		if(dir != NULL)
		{
			while((ent = readdir(dir)) != NULL)
			{
				if (strcmp(ent->d_name, ".") != 0 && strcmp(ent->d_name, "..") != 0 )
				{
					char image_name[255];
					sprintf(image_name, "/%s", ent->d_name);
					string temp(image_name);
					temp = path + temp;

					Mat Ds_img = imread(temp);

					cvtColor(Ds_img, Ds_img, CV_BGR2GRAY);
					threshold(Ds_img, Ds_img, 200, 255, THRESH_OTSU);
					Ds_img.convertTo(Ds_img, CV_32FC1,1.0/255.0);
					resize(Ds_img, Ds_img, Size(16, 16), 0, 0, INTER_NEAREST);
					Ds_img.reshape(1, 1);
					for(int k = 0; k < 256; ++k)
					{
						trainData.at<float>(counter*256 + k) = Ds_img.at<float>(k);
					}
					responses.at<float>(counter) = i;
					counter++;

				}
				
			}
		}
		closedir(dir);
	}
	 
	 Ptr<TrainData> trainingData;
	 Ptr<KNearest> kclassifier = KNearest::create();

	 trainingData = TrainData::create(trainData, SampleTypes::ROW_SAMPLE, responses);
	 kclassifier->train(trainingData);

	 return kclassifier;
}

void sudoku::solve(vector<Mat> small_boxs, Ptr<KNearest> kclassifier)
{
	Mat temp_image =Mat(Size(256, 1), CV_32FC1);
	int grid[9][9];
	for(int i = 0; i < small_boxs.size(); ++i)
	{
		Mat image1 = small_boxs[i];
		image1.convertTo(image1, CV_32FC1, 1.0/255.0);
		image1.reshape(1, 1);

		for(int k = 0; k < 256; ++k)
			temp_image.at<float>(k) = image1.at<float>(k);

		Mat output(0,0,CV_32F);
		float val = kclassifier->findNearest(temp_image, kclassifier->getDefaultK(),output);
		grid[i/9][i%9] = val;
	}

	for(int i = 0; i < 9; ++i)
	{
		for(int j = 0; j < 9; ++j)
			cout << grid[i][j] << " ";
		cout << endl;
	}

	sudokusolver *obj;
	if(obj->cansolve(grid))
	obj->showBoard(grid);
	else
	cout << "invalid sudoku\n";
}
