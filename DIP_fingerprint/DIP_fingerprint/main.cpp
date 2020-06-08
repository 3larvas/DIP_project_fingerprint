#include <iostream>
#include <vector>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

#include "segmentation.h"

//hello~~~~
using namespace std;
using namespace cv;

int main() {
    // ############### STEP 1. READ FILE         ###############
    Mat ori_img = imread("img/[1]_2019_8_4_L_I_1.bmp", 1);
    cvtColor(ori_img, ori_img, COLOR_RGB2GRAY);
    imshow("ori_img", ori_img);

    // ############### STEP 2. SEGMENTATION      ###############
    Mat seg_img = segmentation(ori_img);
    imshow("seg_img", seg_img);
    // ############### STEP 3. NORMALIZATION     ###############
    Mat nor_img;
    // ############### STEP 4. BLOCK ORIENTATION ###############
    // ############### STEP 5. GABOR FILTER      ###############
    // ############### STEP 6. BINARYIZATION     ###############
    // ############### STEP 7. THINNING          ###############
    // ############### STEP 7. DETECT MINUTIAE   ###############
    imshow("seg_img", seg_img);
    waitKey(0);
    return 0;
}
