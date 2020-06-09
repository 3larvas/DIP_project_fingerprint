#include "segmentation.h"

#include <opencv2/core.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <iostream>

using namespace std;
using namespace cv;
/*==============================================================================================*
function : segmentation
desc     : �̹����� ���׶���� ��׶��带 �����Ͽ�, ���׶��常 ����ϰ��ϴ� ����ũ ����
		   mean, variance�� ����Ͽ� mask ����
parm
	- input		: �Է¿���
	- bockSize	: mean, variance�� ����� ��� ������ ����
	- blackT	: ��׶��� ���� �Ӱ�ġ
	- whiteT	: ���׶��� �Ӱ�ġ
	- meanMask	: mean �������� ������ mask
	- varMask	: variance �������� ������ mask
*==============================================================================================*/
void segmentation(Mat input, int blockSize, int blackT, int whiteT, Mat& meanMask, Mat& varMask) {
	Mat ori_img = input.clone();
	int sg_blockSize = blockSize;   //segment blocksize
	Mat mask_mean, mask_var;		//Mean mask , Variance mask

	//zero-padding
	copyMakeBorder(ori_img, mask_mean, 0, sg_blockSize, 0, sg_blockSize, BORDER_CONSTANT, 0);
	copyMakeBorder(ori_img, mask_var, 0, sg_blockSize, 0, sg_blockSize, BORDER_CONSTANT, 0);

	Mat segmented_m, segmented_v;

	// ��� �ȼ��� ��������� ������ ��ȸ�ϸ� mean, variance �� ���
	for (int img_x = 0; img_x < ori_img.cols; img_x += sg_blockSize) {
		for (int img_y = 0; img_y < ori_img.rows; img_y += sg_blockSize) {
			Mat sg_block = mask_mean(Rect(img_x, img_y, sg_blockSize, sg_blockSize));

			//calculate mean
			int pixelCount = 0;
			int mask_meanean = 0;
			for (int x = 0; (x < sg_blockSize) && (img_y + x < ori_img.rows); x++) {
				for (int y = 0; (y < sg_blockSize) && (img_x + y < ori_img.cols); y++) {
					mask_meanean += (int)mask_mean.at<uchar>(img_y + x, img_x + y);
					pixelCount++;
				}
			}
			mask_meanean = mask_meanean / pixelCount;

			//calculate variance
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

			//Mean fill in Matrix
			//black (0)
			if (mask_meanean > whiteT /*|| mask_meanean < blackT*/){
				for (int x = 0; (x < sg_blockSize) && (img_y + x < ori_img.rows); x++) 
					for (int y = 0; (y < sg_blockSize) && (img_x + y < ori_img.cols); y++) 
						mask_mean.at<uchar>(img_y + x, img_x + y) = 0;
			}
			//white (255)
			else {
				for (int x = 0; (x < sg_blockSize) && (img_y + x < ori_img.rows); x++) 
					for (int y = 0; (y < sg_blockSize) && (img_x + y < ori_img.cols); y++) 
						mask_mean.at<uchar>(img_y + x, img_x + y) = 255;
			}

			//variance fill in image
			//black (0)
			if (mask_varariance < blackT){
				for (int x = 0; (x < sg_blockSize) && (img_y + x < ori_img.rows); x++) {
					for (int y = 0; (y < sg_blockSize) && (img_x + y < ori_img.cols); y++) {
						mask_var.at<uchar>(img_y + x, img_x + y) = 0;
					}
				}
			}
			//white (255)
			else {
				for (int x = 0; (x < sg_blockSize) && (img_y + x < ori_img.rows); x++) {
					for (int y = 0; (y < sg_blockSize) && (img_x + y < ori_img.cols); y++) {
						mask_var.at<uchar>(img_y + x, img_x + y) = 255;
					}
				}
			}

		}
		segmented_m = mask_mean(Rect(0, 0, ori_img.cols, ori_img.rows)); // Mean mask
		segmented_v = mask_var(Rect(0, 0, ori_img.cols, ori_img.rows)); // Variance mask

		meanMask = segmented_m;
		varMask = segmented_v;
	}
}

