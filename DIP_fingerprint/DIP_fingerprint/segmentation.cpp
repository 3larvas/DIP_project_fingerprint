#include "segmentation.h"

Mat segmentation(Mat &ori_img) {
	Mat dst;
	ori_img.convertTo(dst, CV_8UC1);
	pyrUp(dst, dst, Size(1 / 4, 1 / 4));
	medianBlur(dst, dst, 7);

	Mat mask = getStructuringElement(1, Size(3, 3), Point(1, 1));
	adaptiveThreshold(dst, dst, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 15, 2);
	imshow("dst", dst);

	Mat seg;
	morphologyEx(dst, seg, MORPH_OPEN, mask, Point(-1, -1), 12);

	GaussianBlur(dst, dst, Size(9, 9), 7);
	adaptiveThreshold(dst, dst, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 9, 1);
	imshow("gaussian", dst);

	erode(dst, dst, mask, Point(-1, -1), 1);
	imshow("erode", dst);

	pyrDown(dst, dst, Size(1 / 4, 1 / 4));
	pyrDown(seg, seg, Size(1 / 4, 1 / 4));

	return seg;

}