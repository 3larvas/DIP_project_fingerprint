#include "detection.h"

//variable for making bin file 
int N_minutiae;   //manutiae Number
int I_width;   //image width
int I_height;   //image height
float angle_;
vector<unsigned char> m_angle;
vector<unsigned char> m_type;
vector<int> x_point;
vector<int> y_point;

void detect(Mat& minutiae, Mat& mask, Mat& OrientMap)
{
	int end = 0, endcheck = 0;
	int bif = 0, bifcheck = 0;
	int found = 0;
	Mat color2;
	cvtColor(minutiae, color2, COLOR_GRAY2BGR);


	int a, b, c, d, e;
	for (int x = 1; x < minutiae.rows - 1; x++) {
		for (int y = 1; y < minutiae.cols - 1; y++) {
			a = mask.at<uchar>(x, y);
			b = mask.at<uchar>(x - 1, y);
			c = mask.at<uchar>(x, y - 1);
			d = mask.at<uchar>(x + 1, y);
			e = mask.at<uchar>(x, y + 1);

			if (a != 0 && b != 0 && c != 0 && d != 0 && e != 0) {   //for not detecting point cut by mask because it is not ending point
				found = 0;
				if (minutiae.at<uchar>(x, y) == 255) {

					if ((minutiae.at<uchar>(x - 1, y - 1)) != (minutiae.at<uchar>(x, y - 1))) found++;
					if ((minutiae.at<uchar>(x, y - 1)) != (minutiae.at<uchar>(x + 1, y - 1))) found++;
					if ((minutiae.at<uchar>(x + 1, y - 1)) != (minutiae.at<uchar>(x + 1, y))) found++;
					if ((minutiae.at<uchar>(x + 1, y)) != (minutiae.at<uchar>(x + 1, y + 1))) found++;
					if ((minutiae.at<uchar>(x + 1, y + 1)) != (minutiae.at<uchar>(x, y + 1))) found++;
					if ((minutiae.at<uchar>(x, y + 1)) != (minutiae.at<uchar>(x - 1, y + 1))) found++;
					if ((minutiae.at<uchar>(x - 1, y + 1)) != (minutiae.at<uchar>(x - 1, y))) found++;
					if ((minutiae.at<uchar>(x - 1, y)) != (minutiae.at<uchar>(x - 1, y - 1))) found++;
				}

				if (found == 2) end = 1;
				else if (found == 6) bif = 1;

				//for make bin file
				if (end || bif) {
					x_point.push_back(y);
					y_point.push_back(x);
					angle_ = (OrientMap.at<float>(x, y) * (-1) + (CV_PI / 2)) * 180 * 255 / CV_PI / 360;
					m_angle.push_back((unsigned char)angle_);
					if (end) m_type.push_back(1);
					else m_type.push_back(3);
				}
				//

				if ((end)) {
					Point pt1 = Point(y, x);
					circle(color2, pt1, 3, Scalar(255, 0, 0), 1, 8, 0);   //blue circle
					end = 0;
					endcheck++;
				}
				if ((bif)) {
					Point pt2 = Point(y, x);
					circle(color2, pt2, 3, Scalar(0, 0, 255), 1, 8, 0);   //red circle
					bif = 0;
					bifcheck++;
				}
			}
		}
	}
	imshow("minutiae_color", color2);
	N_minutiae = endcheck + bifcheck;   //all minutiae number
	cout << "end point Num : " << endcheck << endl;
	cout << "bif point Num : " << bifcheck << endl;
	cout << "minutiae of Number: " << N_minutiae << endl;

	int lim = 0;

	for (int i = 0; i < N_minutiae; i++) {
		if (i + lim >= 50) break;
		printf("X[%d]: %d Y[%d]: %d O[%d]: %d T[%d]: %d\n", i, x_point[i], i, y_point[i], i, m_angle[i], i, m_type[i]);
	}



}

