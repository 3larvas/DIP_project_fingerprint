#include "detection.h"
//variable for making bin file 
int Num_minutiae;   //manutiae Number
int I_width;   //image width
int I_height;   //image height
float angle_;
vector<unsigned char> m_angle;
vector<unsigned char> m_type;
vector<int> x_point;
vector<int> y_point;
/*==============================================================================================*
function : detect minutiae
desc     : XOR 연산과 Minutiae의 특성을 이용하여 End Point와 Bifurcation Point를 구하고 이미지에 표시한다.
		   
parm
	- minuiae	:  세선화 된(thinning) 입력영상
	- mask		:  세그멘테이션 필터
	- OrientMap	:  Make bin file
*==============================================================================================*/
void detect(Mat& minutiae, Mat& mask, Mat& OrientMap)
{
	int end = 0;
	int endcheck = 0;
	int bif = 0;
	int bifcheck = 0;
	int CN = 0;
	Mat checked;
	cvtColor(minutiae, checked, COLOR_GRAY2BGR);

	// P9 P2 P3
	// P8 P1 P4
	// P7 P6 P5
	int P1, P8, P6, P4, P2;
	for (int x = 1; x < minutiae.rows - 1; x++) {
		for (int y = 1; y < minutiae.cols - 1; y++) {
			P1 = mask.at<uchar>(x, y);
			P8 = mask.at<uchar>(x - 1, y);
			P6 = mask.at<uchar>(x, y - 1);
			P4 = mask.at<uchar>(x + 1, y);
			P2 = mask.at<uchar>(x, y + 1);

			if (P1 != 0 && P8 != 0 && P6 != 0 && P4 != 0 && P2 != 0) {   //for not detecting point cut by mask because it is not ending point
				CN = 0;
				if (minutiae.at<uchar>(x, y) == 255) { // P1 == 1
					if ((minutiae.at<uchar>(x, y + 1)) != (minutiae.at<uchar>(x - 1, y + 1))) CN++; // P2 - P9
					if ((minutiae.at<uchar>(x - 1, y + 1)) != (minutiae.at<uchar>(x - 1, y))) CN++; // P9 - P8
					if ((minutiae.at<uchar>(x - 1, y)) != (minutiae.at<uchar>(x - 1, y - 1))) CN++; // P8 - P7
					if ((minutiae.at<uchar>(x - 1, y - 1)) != (minutiae.at<uchar>(x, y - 1))) CN++; // P7 - P6
					if ((minutiae.at<uchar>(x, y - 1)) != (minutiae.at<uchar>(x + 1, y - 1))) CN++; // P6 - P5
					if ((minutiae.at<uchar>(x + 1, y - 1)) != (minutiae.at<uchar>(x + 1, y))) CN++; // P5 - P4
					if ((minutiae.at<uchar>(x + 1, y)) != (minutiae.at<uchar>(x + 1, y + 1))) CN++; // P4 - P3
					if ((minutiae.at<uchar>(x + 1, y + 1)) != (minutiae.at<uchar>(x, y + 1))) CN++; // P3 - P2
				}

				if (CN == 2) end = 1;
				else if (CN == 6) bif = 1;

				//for make bin file
				if (end || bif) {
					x_point.push_back(y);
					y_point.push_back(x);
					angle_ = (OrientMap.at<float>(x, y) * (-1) + (CV_PI / 2)) * 180 * 255 / CV_PI / 360;
					m_angle.push_back((unsigned char)angle_);
					if (end) m_type.push_back(1);
					else m_type.push_back(3);
				}

				if ((end)) {
					Point pt1 = Point(y, x);
					circle(checked, pt1, 3, Scalar(51, 255, 255), 1, 8, 0);   //yellow circle
					end = 0;
					endcheck++;
				}
				if ((bif)) {
					Point pt2 = Point(y, x);
					circle(checked, pt2, 3, Scalar(0, 0, 255), 1, 8, 0);   //red circle
					bif = 0;
					bifcheck++;
				}
			}
		}
	}
	Mat show_checked = checked;
	resize(show_checked, show_checked, Size(300, 400));
	imshow("Check_Minutiae", show_checked);
	Num_minutiae = endcheck + bifcheck;   //all minutiae number
	cout << "End point Num : " << endcheck << endl;
	cout << "Bifufcation point Num : " << bifcheck << endl;
	cout << "minutiae of Number: " << Num_minutiae << endl;

	int limit = 0;

	for (int i = 0; i < Num_minutiae; i++) {
		if (i + limit >= 50) break;
		printf("X[%d]: %d Y[%d]: %d O[%d]: %d T[%d]: %d\n", i, x_point[i], i, y_point[i], i, m_angle[i], i, m_type[i]);
	}

}

