/**
* @brief Sample code to use the function MatchTemplate �û�����ʵ��ģ��ƥ������ַ���
* https://docs.opencv.org/3.2.0/de/da9/tutorial_template_matching.html
* ���Խ������2�ر�׼
** ���ԸĽ��ĵط������Ըı�ģ��Ĵ�С����ƥ�䣬�ҳ����ƥ��ߴ�����ƥ��λ��
*/

#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>

using namespace std;
using namespace cv;

//ȫ�ֱ���
/*
img    �����ͼƬ
templ  ��ƥ���ģ��
mask   ������ģ��ƥ��ķ�����Ҫ�õ���Ĥ����Ĥ��ģ���С��ͬ����Ĥ��ģ���Ӧ��ֵ���
result R ���metric�ľ��󣬼�ƥ��û��̶ȵ�ֵ R����Ĵ�С��result_rows = img.rows - templ.rows + 1��result_rows = img.rows - templ.rows + 1;
*/
bool use_mask;
Mat img; Mat templ; Mat mask; Mat result;
const char* image_window = "Source Image";
const char* result_window = "Result window";

int match_method;  //ģ��ƥ��ķ���
int max_Trackbar = 5;  //ѡ��ģ��ƥ��ķ�����6��

void MatchingMethod(int, void*);

int main(int argc, char** argv)
{
	if (argc < 3)
	{
		cout << "Not enough parameters" << endl;
		return -1;
	}

	img = imread(argv[1], IMREAD_COLOR);  //ԭͼ
	templ = imread(argv[2], IMREAD_COLOR);  //��ƥ���ģ��

	if (argc > 3) 
	{
		use_mask = true;
		mask = imread(argv[3], IMREAD_COLOR);  //��Ĥ
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
* @function MatchingMethod �������ص�����
*/
void MatchingMethod(int, void*)
{

	Mat img_display;
	img.copyTo(img_display);

	int result_cols = img.cols - templ.cols + 1;
	int result_rows = img.rows - templ.rows + 1;

	result.create(result_rows, result_cols, CV_32FC1);

	// Do the Matching and Normalizeƥ��͹�һ��
	bool method_accepts_mask = (TM_SQDIFF == match_method || match_method == TM_CCORR_NORMED); //��Ҫ��Ĥ��ƥ�䷽��
	if (use_mask && method_accepts_mask)
	{
		matchTemplate(img, templ, result, match_method, mask);
	}
	else
	{
		matchTemplate(img, templ, result, match_method);
	}

	normalize(result, result, 0, 1, NORM_MINMAX, -1, Mat()); //����Ĺ�һ��//ͼƬ��ͨ��ֵ[0-255]��unsigned char���ͣ�����ת����[0,1]���������

	//localizing the best match with minMaxLoc ��λ��ƥ���λ��
	double minVal; double maxVal; //ȷ��R �е�������Сֵ 
	Point minLoc; Point maxLoc; //ȷ��������Сֵ��λ��
	Point matchLoc; //���ƥ��ĵ�

	//ȷ��������� R �����ֵ����Сֵ��λ��
	minMaxLoc(result, &minVal, &maxVal, &minLoc, &maxLoc, Mat());


	// For SQDIFF and SQDIFF_NORMED, the best matches are lower values. For all the other methods, the higher the better
	// ����ƥ�䷽�� SQDIFF �� SQDIFF_NORMED, ԽС����ֵ������ߵ�ƥ��������������������, ��ֵԽ��ƥ��Խ��
	if (match_method == TM_SQDIFF || match_method == TM_SQDIFF_NORMED)
	{
		matchLoc = minLoc; //��ֵԽС��Ӧ�ĵ�
	}
	else
	{
		matchLoc = maxLoc; //��ֵԽ���Ӧ�ĵ�
	}

	rectangle(img_display, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar::all(0), 2, 8, 0); //ԭͼ��ʾ������Ľ��
	rectangle(result, matchLoc, Point(matchLoc.x + templ.cols, matchLoc.y + templ.rows), Scalar::all(0), 2, 8, 0);  //R ����

	imshow(image_window, img_display);
	imshow(result_window, result);


	return;
}
/*

void matchTemplate(InputArray image,InputArray templ,OutputArray result,int method )
��һ��������Դͼ�񣬵ڶ���������ģ��ͼ�񣬵�����������ƥ��Ľ��ͼ�񣬵��ĸ�����������ָ���Ƚϵķ���
TM_SQDIFF���÷���ʹ��ƽ�������ƥ�䣬�����ѵ�ƥ�����ڽ��Ϊ0����ֵԽ��ƥ����Խ��
TM_SQDIFF_NORMED���÷���ʹ�ù�һ����ƽ�������ƥ�䣬���ƥ��Ҳ�ڽ��Ϊ0��
TM_CCORR�������ƥ�䷽�����÷���ʹ��Դͼ����ģ��ͼ��ľ���������ƥ�䣬��ˣ����ƥ��λ����ֵ��󴦣�ֵԽСƥ����Խ��
TM_CCORR_NORMED����һ���������ƥ�䷽�����������ƥ�䷽�����ƣ����ƥ��λ��Ҳ����ֵ���
TM_CCOEFF�������ϵ��ƥ�䷽�����÷���ʹ��Դͼ�������ֵ�Ĳģ�������ֵ�Ĳ����֮�������Խ���ƥ�䣬���ƥ������ֵ����1�������ƥ������ֵ����-1����ֵ����0ֱ�ӱ�ʾ���߲����
TM_CCOEFF_NORMED����һ���������ϵ��ƥ�䷽������ֵ��ʾƥ��Ľ���Ϻã���ֵ���ʾƥ���Ч���ϲҲ��ֵԽ��ƥ��Ч��Ҳ��
"Method: \n 0: SQDIFF \n 1: SQDIFF_NORMED \n 2: CCORR \n 3: CCORR_NORMED \n 4: COEFF \n 5: COEFF_NORMED";
*/