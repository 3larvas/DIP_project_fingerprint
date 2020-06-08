#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

using namespace std;
using namespace cv;


Mat gabor(Mat src, vector<pair<float, float>>& vec, int block_size);