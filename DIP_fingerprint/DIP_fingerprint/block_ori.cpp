#include "block_ori.h"

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <queue>

using namespace std;
using namespace cv;
/*==============================================================================================*
function : orientation
desc     : gabor 필터에 입력될 이미지의 단위 블럭 당 패턴의 방향각을 계산, 그리고 gabor filter를
		   이용해 패턴 방향으로 이미지를 강조
parm
	- ori_img		: 입력영상
	- orient_map	: 방향 매트릭스로 block 단위 각각의 방향값이 저장되어있다. gabor filter의
					  입력값과 지문의 방향을 통해 minutiae를 구분할때 사용된다.
	- block_size	: 방향값을 계산하기위한 블럭의 단위 픽셀
	- dst_img		: 연산결과 이미지로 gabor filter가 적용된 모습이다.
*==============================================================================================*/
void enhancement(const Mat& ori_img, Mat& orient_map, int block_size, Mat& dst_img, bool show_yn){
	Mat dir_smo = ori_img.clone();//copy
	Mat tmp(ori_img.size(), ori_img.type()); //make matrix equal size and equal type with ori_img
	Mat coherence(ori_img.size(), ori_img.type());
	orient_map = tmp.clone();

	// 1. 방향 매트릭스 계산 -----------------------------------
	// x축, y축 방향의 변화량
	Mat grad_x, grad_y;
	Sobel(ori_img, grad_x, CV_32F, 1, 0);
	Sobel(ori_img, grad_y, CV_32F, 0, 1);

	Mat Fx(ori_img.size(), ori_img.type()),	Fy(ori_img.size(), ori_img.type()),	Fx_gauss, Fy_gauss;
	Mat smoothed(ori_img.size(), ori_img.type());

	// 블럭의 변수
	int width = ori_img.cols;
	int height = ori_img.rows;
	int blockH, blockW;

	//픽셀을 block size 단위로 조회하여 기울기 계싼
	for (int i = 0; i < height; i += block_size) {
		for (int j = 0; j < width; j += block_size) {
			float Gsx = 0.0, Gsy = 0.0, Gxx = 0.0, Gyy = 0.0;

			//for check bounds of img
			blockH = ((height - i) < block_size) ? (height - i) : block_size;
			blockW = ((width - j) < block_size) ? (width - j) : block_size;
			//average at block WхW
			for (int u = i; u < i + blockH; u++) {
				for (int v = j; v < j + blockW; v++) {
					Gsx += (grad_x.at<float>(u, v) * grad_x.at<float>(u, v)) - (grad_y.at<float>(u, v) * grad_y.at<float>(u, v));
					Gsy += 2 * grad_x.at<float>(u, v) * grad_y.at<float>(u, v);
					Gxx += grad_x.at<float>(u, v) * grad_x.at<float>(u, v);
					Gyy += grad_y.at<float>(u, v) * grad_y.at<float>(u, v);
				}
			}

			float coh = sqrt(pow(Gsx, 2) + pow(Gsy, 2)) / (Gxx + Gyy);
			//smoothed
			float fi = 0.5 * fastAtan2(Gsy, Gsx) * CV_PI / 180;

			Fx.at<float>(i, j) = cos(2 * fi);
			Fy.at<float>(i, j) = sin(2 * fi);

			//fill blocks
			for (int u = i; u < i + blockH; u++) {
				for (int v = j; v < j + blockW; v++) {
					orient_map.at<float>(u, v) = fi;
					Fx.at<float>(u, v) = Fx.at<float>(i, j);
					Fy.at<float>(u, v) = Fy.at<float>(i, j);
					coherence.at<float>(u, v) = (coh < 0.85) ? 1 : 0;
				}
			}
		}
	}

	//GaussianBlur
	GaussianBlur(Fx, Fx_gauss, Size(5, 5), 0);
	GaussianBlur(Fy, Fy_gauss, Size(5, 5), 0);

	for (int m = 0; m < height; m++) {
		for (int n = 0; n < width; n++) {
			smoothed.at<float>(m, n) = 0.5 * fastAtan2(Fy_gauss.at<float>(m, n), Fx_gauss.at<float>(m, n)) * CV_PI / 180;
			if ((m % block_size) == 0 && (n % block_size) == 0) {
				int x = n, y = m;
				int ln = sqrt(2 * pow(block_size, 2)) / 2;
				float dx = ln * cos(smoothed.at<float>(m, n) - CV_PI / 2);
				float dy = ln * sin(smoothed.at<float>(m, n) - CV_PI / 2);
				arrowedLine(dir_smo, Point(x, y + blockH), Point(x + dx, y + blockW + dy), Scalar::all(255), 1, LINE_AA, 0, 0.06 * block_size);
			}
		}
	}
	orient_map = smoothed.clone();

	// 2. gabor filter 적용 -----------------------------------

	//   Matrix for Gabor filter   //
	Mat dst_img2(ori_img.size(), ori_img.type());
	Mat kernel3;
	Mat src3 = ori_img.clone();

	int kernel_size = 7; //gabor filter kernel size
	int special = (kernel_size - 1) / 2;

	//padding source image
	Mat temp = ori_img.clone();
	copyMakeBorder(temp, temp, special, special, special, special, BORDER_REFLECT);
	Mat forgab(temp.size(), temp.type());

	//padding Orientaion Imge
	Mat stemp = smoothed.clone();
	copyMakeBorder(stemp, stemp, special, special, special, special, BORDER_REFLECT);

	// gabor filter parameter 
	double sig = 9, lm = 7.2, gm = 0.02, ps = 0;
	double theta;
	float ffi;

	//Gabor filtering
	for (int m = special; m < temp.rows - special; m++) {
		for (int n = special; n < temp.cols - special; n++) {
			theta = stemp.at<float>(m, n);
			kernel3 = getGaborKernel(Size(kernel_size, kernel_size), sig, theta, lm, gm, ps, CV_32F);
			ffi = 0;
			for (int k = 0; k < kernel_size; k++) {
				for (int l = 0; l < kernel_size; l++) {
					ffi += temp.at<float>(m - special + k, n - special + l) * kernel3.at<float>(kernel_size - 1 - k, kernel_size - 1 - l);
				}
			}
			forgab.at<float>(m, n) = ffi / (kernel_size * kernel_size);
		}
	}

	//Roi of original image size
	Rect roi(special, special, src3.cols, src3.rows);
	Mat forgab_roi = forgab(roi);
	src3 = forgab_roi;

	src3.convertTo(src3, CV_8U, 255, 0);   //0~255 scale
	dst_img = src3;
	if (show_yn) {
		Mat show_smoo = smoothed;
		resize(show_smoo, show_smoo, Size(300, 400));
		imshow("show_smoo", show_smoo);
	}
	if (show_yn) {
		Mat show_dir = dir_smo;
		resize(show_dir, show_dir, Size(300, 400));
		imshow("show_dir", show_dir);
	}
}