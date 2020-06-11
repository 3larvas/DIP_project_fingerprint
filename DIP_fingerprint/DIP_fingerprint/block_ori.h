#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

using namespace std;
using namespace cv;

void enhancement(const Mat& ori_img, Mat& orient_map, int block_size, Mat& dst_img, bool show_yn);