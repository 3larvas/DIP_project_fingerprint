#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

using namespace std;
using namespace cv;

void orientation(const Mat& inputImage, Mat& orientationMap, int blockSize);
void enhancement(const Mat& inputImage, Mat& orientationMap, int blockSize, Mat& dst);