//세선화(thinning) 알고리즘 
//http://blog.daum.net/pg365/107 3종류의 세선화 알고리즘이 있다.

//두팀 모두 3가지의 알고리즘 중 Zhang Suen(장 쑤엔) 세선화 알고리즘을 사용하였다.
//https://rosettacode.org/wiki/Zhang-Suen_thinning_algorithm
#include "thinning.h"

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

using namespace std;
using namespace cv;

void thinningIteration(cv::Mat& img, int iter)
{	// CV_Assert(조건) : 조건식이 거짓이면 예외를 발생시킨다.
	CV_Assert(img.channels() == 1);
	CV_Assert(img.depth() != sizeof(uchar)); // uchar 1byte 0~255
	CV_Assert(img.rows > 3 && img.cols > 3);

	cv::Mat marker = cv::Mat::zeros(img.size(), CV_8UC1); // 8UC1 : 8bit unsigned integer : uchar(0...255), C1 : 1 Channel 

	int nRows = img.rows;
	int nCols = img.cols;
	// isContinuous : 각 행의 마지막에 공백없이 연속적으로 데이터가 저장되어있는지 확인
	if (img.isContinuous()) {  // 데이터가 저장되어있으면 한 열로 표현 ex) [0][152 * 200]
		nCols *= nRows;
		nRows = 1;
	}

	int x, y;
	uchar* pAbove; // 윗 행
	uchar* pCurr; // 중간 행
	uchar* pBelow; // 아랫 행
	uchar* nw, * no, * ne;    // north (pAbove) // 9배열 표현
	uchar* we, * me, * ea;
	uchar* sw, * so, * se;    // south (pBelow)

	uchar* pDst;

	// initialize row pointers
	pAbove = NULL;
	pCurr = img.ptr<uchar>(0);
	pBelow = img.ptr<uchar>(1);

	for (y = 1; y < img.rows - 1; ++y) {
		// shift the rows up by one
		pAbove = pCurr;
		pCurr = pBelow;
		pBelow = img.ptr<uchar>(y + 1);

		pDst = marker.ptr<uchar>(y);

		// initialize col pointers
		no = &(pAbove[0]);
		ne = &(pAbove[1]);
		me = &(pCurr[0]);
		ea = &(pCurr[1]);
		so = &(pBelow[0]);
		se = &(pBelow[1]);

		for (x = 1; x < img.cols - 1; ++x) {
			// shift col pointers left by one (scan left to right)
			nw = no;
			no = ne;
			ne = &(pAbove[x + 1]);
			we = me;
			me = ea;
			ea = &(pCurr[x + 1]);
			sw = so;
			so = se;
			se = &(pBelow[x + 1]);

			int A = (*no == 0 && *ne == 1) + (*ne == 0 && *ea == 1) +
				(*ea == 0 && *se == 1) + (*se == 0 && *so == 1) +
				(*so == 0 && *sw == 1) + (*sw == 0 && *we == 1) +
				(*we == 0 && *nw == 1) + (*nw == 0 && *no == 1);
			int B = *no + *ne + *ea + *se + *so + *sw + *we + *nw;
			int m1 = iter == 0 ? (*no * *ea * *so) : (*no * *ea * *we);
			int m2 = iter == 0 ? (*ea * *so * *we) : (*no * *so * *we);

			if (A == 1 && (B >= 2 && B <= 6) && m1 == 0 && m2 == 0)
				pDst[x] = 1;
		}
	}

	img &= ~marker;
}


Mat thinning(const cv::Mat& src)
{
	Mat dst;
	dst = src.clone();
	dst /= 255;         // convert to binary image

	cv::Mat prev = cv::Mat::zeros(dst.size(), CV_8UC1);
	cv::Mat diff;

	do {
		thinningIteration(dst, 0);
		thinningIteration(dst, 1);
		cv::absdiff(dst, prev, diff);
		dst.copyTo(prev);
	} while (cv::countNonZero(diff) > 0);


	dst *= 255;

	return dst;
}

