#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
//hello~~~~
using namespace std;
using namespace cv;

int main() {
    Mat image = imread("img/[1]_2019_8_4_L_I_1.bmp", 1);
    imshow("original", image);
    waitKey(0);
    return 0;
}