#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/opencv.hpp"
#include <iostream>
#include <vector>
#include <math.h>

using namespace cv;
using namespace std;

void detect(Mat& minutiae, Mat& mask, Mat& OrientMap, bool show_yn, String file_bin, String file_img, int img_w, int img_h);
