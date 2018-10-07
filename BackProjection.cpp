/**
* @brief Sample code for backproject function usage ����ӳ��
 https://blog.csdn.net/qq_18343569/article/details/48028065
*/

#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

#include <iostream>

using namespace cv;
using namespace std;

/*
src ����ԭͼ
hsv ��ɫ�ռ�ռ䣬�ܹ��ǳ�ֱ�۵ı��ɫ�ʵ�������ɫ�����Լ����޳̶�
    ȱ���ǲ���ֱ�Ӱ���ɫֵ�������ʾ������ת������������ϵͳ��Դ
hue ��hsv�в�ֳ�����ͨ��hue
*/
Mat src; Mat hsv; Mat hue;
int bins = 25; //[0,255]�ܹ�256 ��ֵ, ���ǿ��Խ������Χ�ָ��������(���� bins)

void Hist_and_Backproj(int, void*);


int main(int, char** argv)
{
	src = imread(argv[1], IMREAD_COLOR);

	if (src.empty())
	{
		cout << "Usage: ./calcBackProject_Demo1 <path_to_image>" << endl;
		return -1;
	}

	// Transform it to HSV
	cvtColor(src, hsv, COLOR_BGR2HSV);

	//Use only the Hue value
	hue.create(hsv.size(), hsv.depth());
	int ch[] = { 0, 0 };
	mixChannels(&hsv, 1, &hue, 1, ch, 1); //ͨ����֣����Ƹ��������//�õ�HSVͼ���0ͨ��(Hue)

	// Create Trackbar to enter the number of bins
	const char* window_image = "Source image";
	namedWindow(window_image, WINDOW_AUTOSIZE);
	createTrackbar("* Hue  bins: ", window_image, &bins, 180, Hist_and_Backproj);
	Hist_and_Backproj(0, 0); //��ʼ��

	imshow(window_image, src);
	waitKey(0);
	return 0;
}


/**
* @function Hist_and_Backproj
* @brief Callback to Trackbar
*/
void Hist_and_Backproj(int, void*)
{
	MatND hist; //Mat��ָ2ά����MatND�Ƕ�ά����
	int histSize = MAX(bins, 2);  //ֱ��ͼ�����ĸ�������[0-255]���ֳɵ�������������ø����������1
	float hue_range[] = { 0, 180 }; //��Ҫͳ�Ƶ�ֱ��ͼ�ķ�Χ
	const float* ranges = { hue_range };

	/// Get the Histogram and normalize it �õ�hue��ֱ��ͼ
	calcHist(&hue, 1, 0, Mat(), hist, 1, &histSize, &ranges, true, false);//����ֱ��ͼ���������hist
	normalize(hist, hist, 0, 255, NORM_MINMAX, -1, Mat()); //��һ����[0,1]

	/// Get Backprojection �õ�hue�ķ���ͶӰ
	MatND backproj; //����ԭͼ��hue�ķ���ͶӰ
	calcBackProject(&hue, 1, 0, hist, backproj, &ranges, 1, true); //����ͶӰ//����histֱ��ͼѰ�ҷ��ϸ�ֱ��ͼ������

	/// Draw the backproj
	imshow("BackProj", backproj);

	/// Draw the histogram ��ֱ��ͼ
	int w = 400; int h = 400; //w�� h��
	int bin_w = cvRound((double)w / histSize); //ÿ��bin�Ŀ��
	Mat histImg = Mat::zeros(w, h, CV_8UC3); //400*400

	for (int i = 0; i < bins; i++)
	{
		rectangle(histImg, Point(i*bin_w, h), Point((i + 1)*bin_w, h - cvRound(hist.at<float>(i)*h / 255.0)), Scalar(0, 0, 255), -1);
	} //�̶�w���ϵ������㻭��

	imshow("Histogram", histImg);

}
/*
mixChannels��Ҫ���ǰ�����ľ��󣨻�������飩��ĳЩͨ����ָ��Ƹ���Ӧ��������󣨻�������飩��ĳЩͨ���У����еĶ�Ӧ��ϵ����fromTo�����ƶ�
void  mixChannels (const Mat*  src , int  nsrc , Mat*  dst , int  ndst , const int*  fromTo , size_t  npairs );
src�C��������󣬿���Ϊһ��Ҳ����Ϊ��������Ǿ����������ͬ�Ĵ�С�����
nsrcs�C �������ĸ���
dst�C ������󣬿���Ϊһ��Ҳ����Ϊ������������еľ���������ȷ���ռ䣨����create������С�����������������ͬ
ndsts�C �������ĸ���
------------------------------------
fromTo �C������������ͨ����Ӧ��������ͨ�����������£����������ֱ���������ĸ���ͨ��
��һ����������ͨ����Ƿ�ΧΪ��0 ~ src[0].channels()-1���ڶ�����������ͨ����Ƿ�ΧΪ��src[0].channels() ~ src[0].channels()+src[1].channels()-1,�Դ�����
����������Ҳ��ͬ���Ĺ����ǣ���һ����������ͨ����Ƿ�ΧΪ��0 ~ dst[0].channels()-1���ڶ�����������ͨ����Ƿ�ΧΪ��dst[0].channels()~ dst[0].channels()+dst[1].channels()-1,�Դ�����
�������fromTo�ĵ�һ��Ԫ�ؼ�fromTo[0]Ӧ��������������ĳ��ͨ����ǣ���fromTo�ĵڶ���Ԫ�ؼ�fromTo[1]Ӧ��������������ĳ��ͨ����ǣ������ͻ����������fromTo[0]ͨ����������ݸ��Ƹ���������fromTo[1]ͨ������֮һ����������ͨ����Ǻ�����������������ͨ����ǡ�
------------------------------------
npairs�C������fromTo�е��м����������ͨ����ϵ����ʵ���ǲ���fromTo������Ԫ�ظ�������2
*/

/*
void calcHist(const Mat* arrays, int narrays, const int* channels, InputArray mask, SparseMat& hist, int dims, const int* histSize, const float** ranges, bool uniform=true, bool accumulate=false )
arrays �����ͼ���ָ�룬�����Ƕ��ͼ�����е�ͼ�������ͬ������ȣ�CV_8U or CV_32F����ͬʱһ��ͼ������ж��channels
narrays �����ͼ��ĸ���
------------------------------------
channels ��������ֱ��ͼ��channes������
����������2��ͼ�񣬵�һ��ͼ����0��1��2������channels���ڶ���ͼ��ֻ��0һ��channel����ô�����һ����4��channels
���int channels[3] = {3, 2, 0}����ô�ͱ�ʾ��ʹ�õڶ���ͼ��ĵ�һ��ͨ���͵�һ��ͼ��ĵ�2�͵�0��ͨ��������ֱ��ͼ
------------------------------------
mask �������mask��Ϊ�գ���ô��������һ��8λ��CV_8U�������飬�������Ĵ�С�ĺ�arrays[i]�Ĵ�С��ͬ��ֵΪ1�ĵ㽫��������ֱ��ͼ��
hist ���������ֱ��ͼ
dims ���������ֱ��ͼ��ά��
histSize ��ÿһά��ֱ��ͼ�ĸ������򵥰�ֱ��ͼ����һ��һ���������Ļ�������ÿһά�������ĸ���
------------------------------------
ranges ��������ͳ�Ƶķ�Χ
float rang1[] = {0, 20};
float rang2[] = {30, 40};
const float *rangs[] = {rang1, rang2};��ô���Ƕ�0��20��30��40��Χ��ֵ����ͳ��
------------------------------------
uniform ÿһ�������Ŀ���Ƿ����
*/
