#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

using namespace std;
using namespace cv;

void Segmentation(Mat input, int blockSize, int blackT, int whiteT, Mat& meanMask, Mat& varMask);
void Normalize(Mat& image);