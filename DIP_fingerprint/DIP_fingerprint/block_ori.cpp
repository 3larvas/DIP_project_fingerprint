#include "block_ori.h"

void block_orientation(Mat& ori_img, Mat& out_img, int block_size) {
	Mat fprintWithDirectionsSmoo = ori_img.clone();//copy
	Mat tmp(ori_img.size(), ori_img.type()); //make matrix equal size and equal type with ori_img
	Mat coherence(ori_img.size(), ori_img.type());
	out_img = tmp.clone();

	//Gradiants x and y
	Mat grad_x, grad_y;

	Sobel(ori_img, grad_x, CV_32F, 1, 0);//grad_x
	Sobel(ori_img, grad_y, CV_32F, 0, 1);//grad_y

								  //Vector vield
	Mat Fx(ori_img.size(), ori_img.type()),
		Fy(ori_img.size(), ori_img.type()),
		Fx_gauss,
		Fy_gauss;
	Mat smoothed(ori_img.size(), ori_img.type());

	// Local orientation for each block
	int width = ori_img.cols;
	int height = ori_img.rows;
	int blockH;
	int blockW;

	//select block
	for (int i = 0; i < height; i += block_size)
	{
		for (int j = 0; j < width; j += block_size)
		{
			float Gsx = 0.0;
			float Gsy = 0.0;
			float Gxx = 0.0;
			float Gyy = 0.0;

			//for check bounds of img
			blockH = ((height - i) < block_size) ? (height - i) : block_size;
			blockW = ((width - j) < block_size) ? (width - j) : block_size;

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
					out_img.at<float>(u, v) = fi;
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
			if ((m % block_size) == 0 && (n % block_size) == 0) {
				int x = n;
				int y = m;
				int ln = sqrt(2 * pow(block_size, 2)) / 2;
				float dx = ln * cos(smoothed.at<float>(m, n) - CV_PI / 2);
				float dy = ln * sin(smoothed.at<float>(m, n) - CV_PI / 2);
				arrowedLine(fprintWithDirectionsSmoo, Point(x, y + blockH), Point(x + dx, y + blockW + dy), Scalar::all(255), 1, LINE_AA, 0, 0.06 * block_size);
			}
		}
	}///for2
	out_img = smoothed.clone();
}

