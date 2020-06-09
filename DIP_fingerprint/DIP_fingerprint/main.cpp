#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <string.h>
#include <vector>

#include "block_ori.h"
#include "segmentation.h"
#include "thinning.h"
#include "detection.h"

using namespace std;
using namespace cv;

//variable for making bin file 
static int N_minutiae;   //manutiae Number
static int I_width;   //image width
static int I_height;   //image height
static float angle_;
static vector<unsigned char> m_angle;
static vector<unsigned char> m_type;
static vector<int> x_point;
static vector<int> y_point;


int main() {
	// ############### STEP 1. READ FILE         ###############
	Mat mat = imread("img/[1]_2019_8_4_L_I_1.bmp", IMREAD_GRAYSCALE); // READ INPUT IMAGE IN GRAY SCALE
	imshow("Original", mat);
	I_width = mat.cols;
	I_height = mat.rows;

	// ############### STEP 3. SEGMENTATION      ###############
	Mat meanMask, varianceMask;
	Segmentation(mat, 5, 5, 230, meanMask, varianceMask);   //block size is 5
	Mat mask = meanMask & varianceMask;
	dilate(mask, mask, Mat(), Point(-1, -1), 3, 0, BORDER_CONSTANT);
	erode(mask, mask, Mat(), Point(-1, -1), 3, 0, BORDER_CONSTANT);
	imshow("mask", mask);

	// ############### STEP 2. NORMALIZATION     ###############
	equalizeHist(mat, mat);
	imshow("EqualizeHistogram", mat);
	mat.convertTo(mat, CV_32F, 1.0 / 255, 0);   //type : uchar -> float / scale 0~1
	Normalize(mat);
	imshow("Normalize", mat);

	// ############### STEP 4. BLOCK ORIENTATION & GABOR FILTER###############
	int blockSize = 15; // SPECIFY THE BLOCKSIZE;
	int height = mat.rows;
	int width = mat.cols;
	Mat orientationMap;
	Mat dst;
	enhancement(mat, orientationMap, blockSize, dst);
	imshow("Enhancement", dst);
	Mat orien, coreMap, deltaMap;
	
	// ############### STEP 6. BINARYIZATION     ###############
	adaptiveThreshold(dst, dst, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 7, 0);
	Mat show_bin = dst;
	resize(show_bin, show_bin, Size(300, 400));
	imshow("Binary", show_bin);

	// ############### STEP 6. APPLY MASK     ###############
	Mat masked = dst.clone();
	masked |= ~mask;	
	Mat show_mask = masked;
	resize(show_mask, show_mask, Size(300, 400));
	imshow("masked", show_mask);

	// ############### STEP 7. THINNING          ###############
	masked = ~masked;
	thinning(masked);
	Mat show_thin = masked;
	resize(show_thin, show_thin, Size(300, 400));
	imshow("thinning", show_thin);
	erode(mask, mask, Mat(), Point(-1, -1), 5, 0, BORDER_CONSTANT);

	// ############### STEP 7. DETECT MINUTIAE   ###############
	detect(masked, mask, orientationMap);
	Mat show_dest = masked;
	resize(show_dest, show_dest, Size(300, 400));
	imshow("Destination", show_dest);
	
	waitKey(0);
	return 0;
}