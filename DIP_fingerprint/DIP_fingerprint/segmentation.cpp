#include "segmentation.h"
#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

using namespace std;
using namespace cv;
/*==============================================================================================*
function : segmentation
desc     : 이미지의 포그라운드와 백그라운드를 구분하여, 포그라운드만 출력하게하는 마스크 생성
		   mean, variance를 계산하여 mask 생성
parm
	- input		: 입력영상
	- bockSize	: mean, variance를 계산할 블록 사이즈 정의
	- blackT	: 백그라운드 영역 임계치
	- whiteT	: 포그라운드 임계치
	- meanMask	: mean 연산으로 생성된 mask
	- varMask	: variance 연산으로 생성된 mask
*==============================================================================================*/
void Segmentation(Mat input, int blockSize, int blackT, int whiteT, Mat& meanMask, Mat& varMask) {
	Mat ori_img = input.clone();
	int sg_blockSize = blockSize;  
	Mat mask_mean, mask_var;		//Mean mask , Variance mask

	//zero-padding
	copyMakeBorder(ori_img, mask_mean, 0, sg_blockSize, 0, sg_blockSize, BORDER_CONSTANT, 0);
	copyMakeBorder(ori_img, mask_var, 0, sg_blockSize, 0, sg_blockSize, BORDER_CONSTANT, 0);

	Mat segmented_m, segmented_v;

	// 모든 픽셀을 블락사이즈 단위로 조회하며 mean, variance 값 계산
	for (int img_x = 0; img_x < ori_img.cols; img_x += sg_blockSize) {
		for (int img_y = 0; img_y < ori_img.rows; img_y += sg_blockSize) {
			Mat sg_block = mask_mean(Rect(img_x, img_y, sg_blockSize, sg_blockSize));

			//mean 값 계산
			int pixelCount = 0;
			int mask_meanean = 0;
			for (int x = 0; (x < sg_blockSize) && (img_y + x < ori_img.rows); x++) {
				for (int y = 0; (y < sg_blockSize) && (img_x + y < ori_img.cols); y++) {
					mask_meanean += (int)mask_mean.at<uchar>(img_y + x, img_x + y);
					pixelCount++;
				}
			}
			mask_meanean = mask_meanean / pixelCount;

			//variance 값 계산
			int mask_varariance = 0;
			int sg_dev = 0;
			for (int x = 0; (x < sg_blockSize) && (img_y + x < ori_img.rows); x++) {
				for (int y = 0; (y < sg_blockSize) && (img_x + y < ori_img.cols); y++) {
					sg_dev = (int)mask_mean.at<uchar>(img_y + x, img_x + y);
					sg_dev -= mask_meanean;
					mask_varariance += sg_dev * sg_dev;
					pixelCount++;
				}
			}
			mask_varariance = mask_varariance / pixelCount;

			//Mean 값을 임계치와 비교
			if (mask_meanean > whiteT){
				for (int x = 0; (x < sg_blockSize) && (img_y + x < ori_img.rows); x++) 
					for (int y = 0; (y < sg_blockSize) && (img_x + y < ori_img.cols); y++) 
						mask_mean.at<uchar>(img_y + x, img_x + y) = 0; // 검은색으로 채움
			}
			else {
				for (int x = 0; (x < sg_blockSize) && (img_y + x < ori_img.rows); x++) 
					for (int y = 0; (y < sg_blockSize) && (img_x + y < ori_img.cols); y++) 
						mask_mean.at<uchar>(img_y + x, img_x + y) = 255; // 흰색으로 채움
			}

			//Mean 값을 임계치와 비교
			if (mask_varariance < blackT){
				for (int x = 0; (x < sg_blockSize) && (img_y + x < ori_img.rows); x++) {
					for (int y = 0; (y < sg_blockSize) && (img_x + y < ori_img.cols); y++) {
						mask_var.at<uchar>(img_y + x, img_x + y) = 0;// 검은색으로 채움
					}
				}
			}
			//white (255)
			else {
				for (int x = 0; (x < sg_blockSize) && (img_y + x < ori_img.rows); x++) 
					for (int y = 0; (y < sg_blockSize) && (img_x + y < ori_img.cols); y++) 
						mask_var.at<uchar>(img_y + x, img_x + y) = 255;// 흰색으로 채움
			}
		}
		// 좌측 상단 이미지 모서리부분(높이의 15%, 너비의 20% 부분을 마스크로 설정)
		for (int img_x = 0; img_x < ori_img.cols*0.2; img_x++) {
			for (int img_y = 0; img_y < ori_img.rows * 0.15 - img_x; img_y++) {
				mask_mean.at<uchar>(img_y, img_x) = 0; // 검은색으로 채움
				mask_var.at<uchar>(img_y, img_x) = 0;// 검은색으로 채움
			}
		}
		// 우측 상단 이미지 모서리부분(높이의 15%, 너비의 20% 부분을 마스크로 설정)
		for (int img_x = ori_img.cols * 0.8 ; img_x < ori_img.cols; img_x++) {
			for (int img_y = 0; img_y < ori_img.rows * 0.15 - (ori_img.cols - img_x); img_y++) {
				mask_mean.at<uchar>(img_y, img_x) = 0; // 검은색으로 채움
				mask_var.at<uchar>(img_y, img_x) = 0;// 검은색으로 채움
			}
		}

		segmented_m = mask_mean(Rect(0, 0, ori_img.cols, ori_img.rows)); // Mean mask
		segmented_v = mask_var(Rect(0, 0, ori_img.cols, ori_img.rows));  // Variance mask

		meanMask = segmented_m;
		varMask = segmented_v;
	}
}

void Normalize(Mat& image)
{
	Scalar mean, dev;

	meanStdDev(image, mean, dev, noArray());   //calculate mean and StdDev

	double M = mean.val[0];
	double D = dev.val[0];

	//Normalize image
	for (int i(0); i < image.rows; i++){
		for (int j(0); j < image.cols; j++){
			if (image.at<float>(i, j) > M)
				image.at<float>(i, j) = 100.0 / 255 + sqrt(100.0 / 255 * pow(image.at<float>(i, j) - M, 2) / D);
			else
				image.at<float>(i, j) = 100.0 / 255 - sqrt(100.0 / 255 * pow(image.at<float>(i, j) - M, 2) / D);
		}
	}
}

