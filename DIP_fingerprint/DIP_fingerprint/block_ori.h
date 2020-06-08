#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

using namespace std;
using namespace cv;

Mat block_orientation(Mat& ori_img);
pair<Mat, vector<pair<float, float>>> orientation(Mat src, int size = 8, bool coredelta = false);