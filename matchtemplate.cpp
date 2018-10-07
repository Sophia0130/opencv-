/**
* @brief Sample code to use the function MatchTemplate 用滑动条实现模板匹配的六种方法
* https://docs.opencv.org/3.2.0/de/da9/tutorial_template_matching.html
* 测试结果方法2特别不准
** 可以改进的地方，可以改变模板的大小进行匹配，找出最佳匹配尺寸和最佳匹配位置
*/

#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>

using namespace std;
using namespace cv;

//全局变量
/*
img    输入的图片
templ  待匹配的模板
mask   有两种模板匹配的方法需要用到掩膜，掩膜和模板大小相同，掩膜和模板对应的值相乘
result R 存放metric的矩阵，即匹配好坏程度的值 R矩阵的大小：result_rows = img.rows - templ.rows + 1、result_rows = img.rows - templ.rows + 1;
*/
bool use_mask;
Mat img; Mat templ; Mat mask; Mat result;
const char* image_window = "Source Image";
const char* result_window = "Result window";

int match_method;  //模板匹配的方法
int max_Trackbar = 5;  //选择模板匹配的方法有6种

void MatchingMethod(int, void*);

int main(int argc, char** argv)
{
	if (argc < 3)
	{
		cout << "Not enough parameters" << endl;
		return -1;
	}

	img = imread(argv[1], IMREAD_COLOR);  //原图
	templ = imread(argv[2], IMREAD_COLOR);  //待匹配的模板

	if (argc > 3) 
	{
		use_mask = true;
		mask = imread(argv[3], IMREAD_COLOR);  //掩膜
	}

	if (img.empty() || templ.empty() || (use_mask && mask.empty()))
	{
		cout << "Can't read one of the images" << endl;
		return -1;
	}

	namedWindow(image_window, WINDOW_AUTOSIZE);
	namedWindow(result_window, WINDOW_AUTOSIZE);

	const char* trackbar_label = "Method: \n 0: SQDIFF \n 1: SQDIFF_NORMED \n 2: CCORR \n 3: CCORR_NORMED \n 4: COEFF \n 5: COEFF_NORMED";
	createTrackbar(trackbar_label, image_window, &match_method, max_Trackbar, MatchingMethod);
	//! [create_trackbar]

	MatchingMethod(0, 0);

	waitKey(0);
	return 0;

}

/**
* @function MatchingMethod 滑动条回掉函数
*/
void MatchingMethod(int, void*)
{

	Mat img_display;
	img.copyTo(img_display);

	int result_cols = img.cols - templ.cols + 1;
	int result_rows = img.rows - templ.rows + 1;

	result.create(result_rows, result_cols, CV_32FC1);

	// Do the Matching and Normalize匹配和归一化
	bool method_accepts_mask = (TM_SQDIFF == match_method || match_method == TM_CCORR_NORMED); //需要掩膜的匹配方法
	if (use_mask && method_accepts_mask)
	{
		matchTemplate(img, templ, result, match_method, mask);
	}
	else
	{
		matchTemplate(img, templ, result, match_method);
	}

	normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat()); //矩阵的归一化//图片单通道值[0-255]的unsigned char类型，将其转化到[0,1]，方便计算

	//localizing the best match with minMaxLoc 定位最匹配的位置
	double minVal; double maxVal; //确定R 中的最大和最小值 
	Point minLoc; Point maxLoc; //确定最大和最小值的位置
	Point matchLoc; //最佳匹配的点

	//确定结果矩阵 R 的最大值和最小值的位置
	minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());


	// For SQDIFF and SQDIFF_NORMED, the best matches are lower values. For all the other methods, the higher the better
	// 对于匹配方法 SQDIFF 和 SQDIFF_NORMED, 越小的数值代表更高的匹配结果，而对于其他方法, 数值越大匹配越好
	if (match_method == TM_SQDIFF || match_method == TM_SQDIFF_NORMED)
	{
		matchLoc = minLoc; //数值越小对应的点
	}
	else
	{
		matchLoc = maxLoc; //数值越大对应的点
	}

	rectangle(img_display, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar::all(0), 2, 8, 0); //原图显示框出来的结果
	rectangle(result, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar::all(0), 2, 8, 0);  //R 矩阵

	imshow(image_window, img_display);
	imshow(result_window, result);


	return;
}
/*

void matchTemplate(InputArray image,InputArray templ,OutputArray result,int method )
第一个参数是源图像，第二个参数是模板图像，第三个参数是匹配的结果图像，第四个参数是用于指定比较的方法
TM_SQDIFF：该方法使用平方差进行匹配，因此最佳的匹配结果在结果为0处，值越大匹配结果越差
TM_SQDIFF_NORMED：该方法使用归一化的平方差进行匹配，最佳匹配也在结果为0处
TM_CCORR：相关性匹配方法，该方法使用源图像与模板图像的卷积结果进行匹配，因此，最佳匹配位置在值最大处，值越小匹配结果越差
TM_CCORR_NORMED：归一化的相关性匹配方法，与相关性匹配方法类似，最佳匹配位置也是在值最大处
TM_CCOEFF：相关性系数匹配方法，该方法使用源图像与其均值的差、模板与其均值的差二者之间的相关性进行匹配，最佳匹配结果在值等于1处，最差匹配结果在值等于-1处，值等于0直接表示二者不相关
TM_CCOEFF_NORMED：归一化的相关性系数匹配方法，正值表示匹配的结果较好，负值则表示匹配的效果较差，也是值越大，匹配效果也好
"Method: \n 0: SQDIFF \n 1: SQDIFF_NORMED \n 2: CCORR \n 3: CCORR_NORMED \n 4: COEFF \n 5: COEFF_NORMED";
*/