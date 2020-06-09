#include "block_ori.h"

#include <iostream>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <queue>

using namespace std;
using namespace cv;

void orientation(const Mat& inputImage, Mat& orientationMap, int blockSize)
{
	Mat fprintWithDirectionsSmoo = inputImage.clone();//copy
	Mat tmp(inputImage.size(), inputImage.type()); //make matrix equal size and equal type with Inputimage
	Mat coherence(inputImage.size(), inputImage.type());
	orientationMap = tmp.clone();

	//Gradiants x and y
	Mat grad_x, grad_y;

	Sobel(inputImage, grad_x, CV_32F, 1, 0);//grad_x
	Sobel(inputImage, grad_y, CV_32F, 0, 1);//grad_y

								  //Vector vield
	Mat Fx(inputImage.size(), inputImage.type()),
		Fy(inputImage.size(), inputImage.type()),
		Fx_gauss,
		Fy_gauss;
	Mat smoothed(inputImage.size(), inputImage.type());

	// Local orientation for each block
	int width = inputImage.cols;
	int height = inputImage.rows;
	int blockH;
	int blockW;

	//select block
	for (int i = 0; i < height; i += blockSize)
	{
		for (int j = 0; j < width; j += blockSize)
		{
			float Gsx = 0.0;
			float Gsy = 0.0;
			float Gxx = 0.0;
			float Gyy = 0.0;

			//for check bounds of img
			blockH = ((height - i) < blockSize) ? (height - i) : blockSize;
			blockW = ((width - j) < blockSize) ? (width - j) : blockSize;

			//average at block W¬çW
			for (int u = i; u < i + blockH; u++)
			{
				for (int v = j; v < j + blockW; v++)
				{
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
			for (int u = i; u < i + blockH; u++)
			{
				for (int v = j; v < j + blockW; v++)
				{
					orientationMap.at<float>(u, v) = fi;
					Fx.at<float>(u, v) = Fx.at<float>(i, j);
					Fy.at<float>(u, v) = Fy.at<float>(i, j);
					coherence.at<float>(u, v) = (coh < 0.85) ? 1 : 0;
				}
			}

		}
	} ///for

	//GaussianBlur
	GaussianBlur(Fx, Fx_gauss, Size(5, 5), 0);
	GaussianBlur(Fy, Fy_gauss, Size(5, 5), 0);

	for (int m = 0; m < height; m++)
	{
		for (int n = 0; n < width; n++)
		{
			smoothed.at<float>(m, n) = 0.5 * fastAtan2(Fy_gauss.at<float>(m, n), Fx_gauss.at<float>(m, n)) * CV_PI / 180;
			if ((m % blockSize) == 0 && (n % blockSize) == 0) {
				int x = n;
				int y = m;
				int ln = sqrt(2 * pow(blockSize, 2)) / 2;
				float dx = ln * cos(smoothed.at<float>(m, n) - CV_PI / 2);
				float dy = ln * sin(smoothed.at<float>(m, n) - CV_PI / 2);
				arrowedLine(fprintWithDirectionsSmoo, Point(x, y + blockH), Point(x + dx, y + blockW + dy), Scalar::all(255), 1, LINE_AA, 0, 0.06 * blockSize);
			}
		}
	}///for2
	orientationMap = smoothed.clone();
}


void enhancement(const Mat& inputImage, Mat& orientationMap, int blockSize, Mat& dst)
{

	Mat fprintWithDirectionsSmoo = inputImage.clone();   //copy
	Mat tmp(inputImage.size(), inputImage.type());   //make matrix equal size and equal type with Inputimage
	Mat coherence(inputImage.size(), inputImage.type());
	orientationMap = tmp.clone();   //will be stored orietationMap

							//Gradiants x and y
	Mat grad_x, grad_y;


	Scharr(inputImage, grad_x, CV_32F, 1, 0);   //find grad_x
	Scharr(inputImage, grad_y, CV_32F, 0, 1);   //find grad_y

									 //Vector vield
	Mat Fx(inputImage.size(), inputImage.type()),
		Fy(inputImage.size(), inputImage.type()),
		Fx_gauss,
		Fy_gauss;
	Mat smoothed(inputImage.size(), inputImage.type());   //will be stored smoothed orientationMap

											 // Local orientation for each block
	int width = inputImage.cols;
	int height = inputImage.rows;
	int blockH;
	int blockW;

	//select block
	for (int i = 0; i < height; i += blockSize)
	{
		for (int j = 0; j < width; j += blockSize)
		{
			float Gsx = 0.0;
			float Gsy = 0.0;
			float Gxx = 0.0;
			float Gyy = 0.0;

			//for check bounds of img
			blockH = ((height - i) < blockSize) ? (height - i) : blockSize;
			blockW = ((width - j) < blockSize) ? (width - j) : blockSize;

			//average at block W¬çW
			for (int u = i; u < i + blockH; u++)
			{
				for (int v = j; v < j + blockW; v++)
				{
					Gsx += (grad_x.at<float>(u, v) * grad_x.at<float>(u, v)) - (grad_y.at<float>(u, v) * grad_y.at<float>(u, v));
					Gsy += 2 * grad_x.at<float>(u, v) * grad_y.at<float>(u, v);
					Gxx += grad_x.at<float>(u, v) * grad_x.at<float>(u, v);
					Gyy += grad_y.at<float>(u, v) * grad_y.at<float>(u, v);
				}
			}

			float coh = sqrt(pow(Gsx, 2) + pow(Gsy, 2)) / (Gxx + Gyy);
			//theta of pixel
			float fi = 0.5 * fastAtan2(Gsy, Gsx) * CV_PI / 180;

			Fx.at<float>(i, j) = cos(2 * fi);
			Fy.at<float>(i, j) = sin(2 * fi);

			//fill blocks
			for (int u = i; u < i + blockH; u++)
			{
				for (int v = j; v < j + blockW; v++)
				{
					orientationMap.at<float>(u, v) = fi;
					Fx.at<float>(u, v) = Fx.at<float>(i, j);
					Fy.at<float>(u, v) = Fy.at<float>(i, j);
					coherence.at<float>(u, v) = (coh < 0.85) ? 1 : 0;
				}
			}

		}
	} ///for


	  //GAUSSIAN BLUR SMOOTHING Gaussianblur smoothing
	GaussianBlur(Fx, Fx_gauss, Size(5, 5), 0);
	GaussianBlur(Fy, Fy_gauss, Size(5, 5), 0);


	//smoothed Orientation
	for (int m = 0; m < height; m++)
	{
		for (int n = 0; n < width; n++)
		{
			smoothed.at<float>(m, n) = 0.5 * fastAtan2(Fy_gauss.at<float>(m, n), Fx_gauss.at<float>(m, n)) * CV_PI / 180;
			if ((m % blockSize) == 0 && (n % blockSize) == 0) {
				int x = n;
				int y = m;
				int ln = sqrt(2 * pow(blockSize, 2)) / 2;
				float dx = ln * cos(smoothed.at<float>(m, n) - CV_PI / 2);
				float dy = ln * sin(smoothed.at<float>(m, n) - CV_PI / 2);
				//draw line direction
				arrowedLine(fprintWithDirectionsSmoo, Point(x, y + blockH), Point(x + dx, y + blockW + dy), Scalar::all(255), 1, LINE_AA, 0, 0.06 * blockSize);
			}
		}
	}///for2
	orientationMap = smoothed.clone();

	//   Matrix for Gabor filter   //
	Mat dst2(inputImage.size(), inputImage.type());
	Mat kernel3;
	Mat src3 = inputImage.clone();

	int kernel_size = 7; //gabor filter kernel size
	int special = (kernel_size - 1) / 2;

	//padding source image
	Mat temp = inputImage.clone();
	copyMakeBorder(temp, temp, special, special, special, special, BORDER_REFLECT);
	Mat forgab(temp.size(), temp.type());

	//padding Orientaion Imge
	Mat stemp = smoothed.clone();
	copyMakeBorder(stemp, stemp, special, special, special, special, BORDER_REFLECT);

	// gabor filter parameter 
	double sig = 9, lm = 7.2, gm = 0.02, ps = 0;
	double theta;
	float ffi;

	/////Gabor filtering
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
	dst = src3;
	imshow("Smoothed orientation field", smoothed);
	imshow("Orientation", fprintWithDirectionsSmoo);
}