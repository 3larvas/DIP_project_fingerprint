#include <vector>
#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <math.h>

#define THR 10

using namespace std;
using namespace cv;


struct Minutiae {
	int x;
	int y;
	int angle;
	int type; //ending:1  bifurcation:2
};

vector<Minutiae> findMinutiae(Mat& img, Mat& seg);
float angle(Mat& dst, vector<pair<float, float>>& vec, int& u, int& v, int& block_size, Size size, int& type);
Mat printMinutiae(Mat src, Mat& srcc, vector<pair<float, float>>& vec, int& block_size, Size size, Mat& original);