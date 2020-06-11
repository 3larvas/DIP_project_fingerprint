#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <fstream>
#include <string.h>
#include <vector>

#include "block_ori.h"
#include "segmentation.h"
#include "thinning.h"
#include "detection.h"

//variable for making bin file 
static int n_minutiae;   //manutiae Number
static int i_width;   //image width
static int i_height;   //image height
static float angle_;
static vector<unsigned char> m_angle;
static vector<unsigned char> m_type;
static vector<int> x_point;
static vector<int> y_point;

String file_nm_list[40] = {
	"[1]_2019_8_4_L_I_1",
	"[2]_2019_8_1_L_I_1",
	"[3]_2019_8_3_L_I_1",
	"[4]_2019_8_2_L_I_1",
	"[5]_2019_2_1_L_검_1",
	"[6]_2019_2_1_L_검_1",
	"[7]_2019_2_1_L_검_1",
	"[8]_2019_2_1_L_검_1",
	"[9]_2019_5_4_L_I_1",
	"[10]_2019_5_1_L_I#1",
	"[11]_2019_2_1_L_색_1",
	"[12]_2019_2_1_L_약_1",
	"[13]_2019_2_1_L_엄_1",
	"[14]_2019_2_1_L_중_1",
	"[15]_2019_8_1_L_I_1",
	"[16]_ 2019_8_1_L_M_1",
	"[17]_2019_8_1_L_P_1",
	"[18]_2019_8_1_L_R_1",
	"[19]_2019_8_1_L_T_1",
	"[20]_2019_8_4_L_M_1",
	"[21]_2019_8_4_L_P_1",
	"[22]_2019_8_4_L_R_1",
	"[23]_2019_8_4_L_T_1",
	"[24]_2019_8_3_L_M_1",
	"[25]_2019_8_3_L_P_1",
	"[26]_2019_8_3_L_T_1",
	"[27]_2019_5_2_L_I_1",
	"[28]_2019_5_2_L_M_1",
	"[29]_2019_5_2_L_P_1",
	"[30]_2019_5_2_L_T_1",
	"[31]_2019_5_3_L_I_1",
	"[32]_2019_5_3_L_M_1",
	"[33]_2019_5_3_L_P_1",
	"[34]_2019_5_3_L_R_1",
	"[35]_2019_5_3_L_T_1",
	"[36]_2019_5_1_L_I#1",
	"[37]_2019_5_1_L_M#1",
	"[38]_2019_5_1_L_P#1",
	"[39]_2019_5_1_L_R#1",
	"[40]_2019_5_1_L_T#1"
};
