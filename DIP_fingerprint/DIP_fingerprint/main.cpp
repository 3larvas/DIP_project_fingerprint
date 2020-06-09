#include <iostream>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "gabor_filter.h"
#include "block_ori.h"
#include "segmentation.h"
#include "thinning.h"
#include "minutiae.h"
#include "distance.h"

using namespace std;
using namespace cv;

int main() {
	// ############### STEP 1. READ FILE         ###############
	Mat src = imread("img/[1]_2019_8_4_L_I_1.bmp");
	Size size = { 152,200 };
	int block_size = 7;
	cvtColor(src, src, COLOR_RGB2GRAY);
	
	// ############### STEP 2. NORMALIZATION     ###############
	equalizeHist(src, src);

	// ############### STEP 3. SEGMENTATION      ###############
	Mat segmented;
	//Mat segmented2 = segmentation(src, segmented);	
	Mat meanMask, varianceMask;
	segmentation(src, 5, 5, 230, meanMask, varianceMask);   //block size is 5
	Mat mask = meanMask & varianceMask;
	dilate(mask, mask, Mat(), Point(-1, -1), 3, 0, BORDER_CONSTANT);
	erode(mask, mask, Mat(), Point(-1, -1), 3, 0, BORDER_CONSTANT);
	imshow("mask", mask);

	// ############### STEP 4. BLOCK ORIENTATION ###############
	pair<Mat, vector<pair<float, float>>> returned = orientation(src, block_size, false);
	Mat show = returned.first;
	vector<pair<float, float>> vec = returned.second;
	pair<Mat, vector<pair<float, float>>> returned2 = orientation(src, 7, true);
	Mat coredelta = returned2.first;

	// ############### STEP 5. GABOR FILTER      ###############
	Mat gabored = gabor(src, vec, block_size);//+ segmented2;

	// ############### STEP 6. BINARYIZATION     ###############
	Mat gabored_end;
	threshold(gabored, gabored_end, 1, 255, THRESH_BINARY_INV);

	// ############### STEP 7. THINNING          ###############
	Mat imgt = thinning(gabored_end);

	// ############### STEP 7. DETECT MINUTIAE   ###############
	Mat result = printMinutiae(imgt, src/*segmented2*/, vec, block_size, size, src);
	// measure distance between ridge
	//calculate(imgt, segmented2);

	pyrUp(src, src);
	imshow("src", src);

	pyrUp(show, show);
	imshow("show", show);

	//pyrUp(segmented2, segmented2);
	//imshow("segmented area", segmented2);

	gabored.convertTo(gabored, CV_8U);
	pyrUp(gabored, gabored);
	imshow("gabored", gabored);

	imgt.convertTo(imgt, CV_8U);
	pyrUp(imgt, imgt);
	imshow("thinned", imgt);

	pyrUp(result, result);
	imshow("check", result);

	pyrUp(coredelta, coredelta);
	imshow("coredelta", coredelta);

	waitKey(0);
	return 0;
}