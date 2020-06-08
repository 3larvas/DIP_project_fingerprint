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
	// ############### STEP 2. SEGMENTATION      ###############
	// ############### STEP 3. NORMALIZATION     ###############
	// ############### STEP 4. BLOCK ORIENTATION ###############
	// ############### STEP 5. GABOR FILTER      ###############
	// ############### STEP 6. BINARYIZATION     ###############
	// ############### STEP 7. THINNING          ###############
	// ############### STEP 7. DETECT MINUTIAE   ###############
	// orientation block size
	// rows, cols must be devided by block size
	int block_size = 7;

	Mat src = imread("img/[1]_2019_8_4_L_I_1.bmp");
	Size size = { 152,200 };
	cvtColor(src, src, COLOR_RGB2GRAY);

	// resize image
	resize(src, src, size);

	Mat segmented;
	// segmantation image
	Mat segmented2 = segmentation(src, segmented);
	// normalize image
	equalizeHist(src, src);


	// block oriented
	pair<Mat, vector<pair<float, float>>> returned = orientation(src, block_size, false);
	Mat show = returned.first;
	vector<pair<float, float>> vec = returned.second;

	pair<Mat, vector<pair<float, float>>> returned2 = orientation(src, 7, true);
	Mat coredelta = returned2.first;

	// gabor filter
	Mat gabored = gabor(src, vec, block_size) + segmented2;

	Mat gabored_end;
	// binarization
	threshold(gabored, gabored_end, 1, 255, THRESH_BINARY_INV);

	// thinning
	Mat imgt = thinning(gabored_end);

	// find minutiae and visual them
	Mat result = printMinutiae(imgt, segmented2, vec, block_size, size, src);
	// measure distance between ridge
	//calculate(imgt, segmented2);

	pyrUp(src, src);
	imshow("src", src);

	pyrUp(show, show);
	imshow("show", show);

	pyrUp(segmented2, segmented2);
	imshow("segmented area", segmented2);

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

	cout << "³¡!" << endl;

	waitKey(0);
	return 0;
}