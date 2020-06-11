
#include "main.h"

using namespace std;
using namespace cv;

bool show_yn = false;

int main() {
	for (int t = 0; t < 40; t++) {
		printf("[%d] image start process!!\n", t);
		String file_nm = file_nm_list[t];
		String file_img = "img/" + file_nm + ".bmp";
		String file_bin = "bin/" + file_nm + ".bin";

		// ############### STEP 1. READ FILE         ###############
		Mat mat = imread(file_img, IMREAD_GRAYSCALE); // READ INPUT IMAGE IN GRAY SCALE
		Mat img_original = mat;
		if (show_yn) {
			resize(img_original, img_original, Size(300, 400));
			imshow("img_original", img_original);
		}
		i_width = mat.cols;
		i_height = mat.rows;

		// ############### STEP 2. SEGMENTATION      ###############
		Mat meanMask, varianceMask;
		Segmentation(mat, 5, 5, 230, meanMask, varianceMask);   //block size is 5
		Mat mask = meanMask & varianceMask;
		dilate(mask, mask, Mat(), Point(-1, -1), 3, 0, BORDER_CONSTANT);
		erode(mask, mask, Mat(), Point(-1, -1), 3, 0, BORDER_CONSTANT);
		if (show_yn) {
			Mat mask_original = mask;
			resize(mask_original, mask_original, Size(300, 400));
			imshow("mask_original", mask_original);
		}

		// ############### STEP 3. NORMALIZATION     ###############
		equalizeHist(mat, mat);
		Mat img_Equalize = mat;
		resize(img_Equalize, img_Equalize, Size(300, 400));
		if (show_yn) imshow("EqualizeHistogram", img_Equalize);
		mat.convertTo(mat, CV_32F, 1.0 / 255, 0);   //type : uchar -> float / scale 0~1
		Normalize(mat);
		if (show_yn) {
			Mat img_Normailze = mat;
			resize(img_Normailze, img_Normailze, Size(300, 400));
			imshow("Normalize", img_Normailze);
		}

		// ############### STEP 4. BLOCK ORIENTATION & GABOR FILTER###############
		int blockSize = 30; // SPECIFY THE BLOCKSIZE;
		int height = mat.rows;
		int width = mat.cols;
		Mat orientationMap;
		Mat dst;
		enhancement(mat, orientationMap, blockSize, dst, show_yn);
		if (show_yn) {
			Mat img_enhancement = dst;
			resize(img_enhancement, img_enhancement, Size(300, 400));
			imshow("Enhancement", img_enhancement);
		}
		Mat orien, coreMap, deltaMap;

		// ############### STEP 5. BINARYIZATION     ###############
		adaptiveThreshold(dst, dst, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 7, 0);
		if (show_yn) {
			Mat show_bin = dst;
			resize(show_bin, show_bin, Size(300, 400));
			imshow("Binary", show_bin);
		}

		// ############### STEP 6. APPLY MASK     ###############
		Mat masked = dst.clone();
		masked |= ~mask;
		if (show_yn) {
			Mat show_mask = masked;
			resize(show_mask, show_mask, Size(300, 400));
			imshow("masked", show_mask);
		}

		// ############### STEP 7. THINNING          ###############
		masked = ~masked;
		thinning(masked);
		if (show_yn) {
			Mat show_thin = masked;
			resize(show_thin, show_thin, Size(300, 400));
			imshow("thinning", show_thin);
		}
		erode(mask, mask, Mat(), Point(-1, -1), 5, 0, BORDER_CONSTANT);

		// ############### STEP 8. DETECT MINUTIAE   ###############
		detect(masked, mask, orientationMap, show_yn, file_bin, i_width, i_height);
		if (show_yn) {
			Mat show_dest = masked;
			resize(show_dest, show_dest, Size(300, 400));
			imshow("Destination", show_dest);
		}
	}
	waitKey(0);
	return 0;
}