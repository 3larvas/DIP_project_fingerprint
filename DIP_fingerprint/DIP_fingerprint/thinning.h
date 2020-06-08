#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

using namespace std;
using namespace cv;

void thinningIteration(cv::Mat& img, int iter);
Mat thinning(const cv::Mat& src);