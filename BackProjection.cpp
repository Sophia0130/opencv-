/**
* @brief Sample code for backproject function usage 反向映射
 https://blog.csdn.net/qq_18343569/article/details/48028065
*/

#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

#include <iostream>

using namespace cv;
using namespace std;

/*
src 输入原图
hsv 颜色空间空间，能够非常直观的表达色彩的明暗，色调，以及鲜艳程度
    缺点是不能直接把颜色值传达给显示器，在转换过程中消耗系统资源
hue 从hsv中拆分出来的通道hue
*/
Mat src; Mat hsv; Mat hue;
int bins = 25; //[0,255]总共256 个值, 我们可以将这个范围分割成子区域(称作 bins)

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
	mixChannels(&hsv, 1, &hue, 1, ch, 1); //通道拆分，复制给输出矩阵//得到HSV图像的0通道(Hue)

	// Create Trackbar to enter the number of bins
	const char* window_image = "Source image";
	namedWindow(window_image, WINDOW_AUTOSIZE);
	createTrackbar("* Hue  bins: ", window_image, &bins, 180, Hist_and_Backproj);
	Hist_and_Backproj(0, 0); //初始化

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
	MatND hist; //Mat特指2维矩阵，MatND是多维矩阵
	int histSize = MAX(bins, 2);  //直方图竖条的个数，将[0-255]划分成的子区域个数，该个数必须大于1
	float hue_range[] = { 0, 180 }; //需要统计的直方图的范围
	const float* ranges = { hue_range };

	/// Get the Histogram and normalize it 得到hue的直方图
	calcHist(&hue, 1, 0, Mat(), hist, 1, &histSize, &ranges, true, false);//计算直方图，结果放在hist
	normalize(hist, hist, 0, 255, NORM_MINMAX, -1, Mat()); //归一化到[0,1]

	/// Get Backprojection 得到hue的反向投影
	MatND backproj; //储存原图像hue的反向投影
	calcBackProject(&hue, 1, 0, hist, backproj, &ranges, 1, true); //反向投影//根据hist直方图寻找符合该直方图的特征

	/// Draw the backproj
	imshow("BackProj", backproj);

	/// Draw the histogram 画直方图
	int w = 400; int h = 400; //w宽 h高
	int bin_w = cvRound((double)w / histSize); //每个bin的宽度
	Mat histImg = Mat::zeros(w, h, CV_8UC3); //400*400

	for (int i = 0; i < bins; i++)
	{
		rectangle(histImg, Point(i*bin_w, h), Point((i + 1)*bin_w, h - cvRound(hist.at<float>(i)*h / 255.0)), Scalar(0, 0, 255), -1);
	} //固定w轴上的两个点画高

	imshow("Histogram", histImg);

}
/*
mixChannels主要就是把输入的矩阵（或矩阵数组）的某些通道拆分复制给对应的输出矩阵（或矩阵数组）的某些通道中，其中的对应关系就由fromTo参数制定
void  mixChannels (const Mat*  src , int  nsrc , Mat*  dst , int  ndst , const int*  fromTo , size_t  npairs );
src–　输入矩阵，可以为一个也可以为多个，但是矩阵必须有相同的大小和深度
nsrcs– 输入矩阵的个数
dst– 输出矩阵，可以为一个也可以为多个，但是所有的矩阵必须事先分配空间（如用create），大小和深度须与输入矩阵等同
ndsts– 输出矩阵的个数
------------------------------------
fromTo –设置输入矩阵的通道对应输出矩阵的通道，规则如下：首先用数字标记输入矩阵的各个通道
第一个输入矩阵的通道标记范围为：0 ~ src[0].channels()-1，第二个输入矩阵的通道标记范围为：src[0].channels() ~ src[0].channels()+src[1].channels()-1,以此类推
其次输出矩阵也用同样的规则标记，第一个输出矩阵的通道标记范围为：0 ~ dst[0].channels()-1，第二个输入矩阵的通道标记范围为：dst[0].channels()~ dst[0].channels()+dst[1].channels()-1,以此类推
最后，数组fromTo的第一个元素即fromTo[0]应该填入输入矩阵的某个通道标记，而fromTo的第二个元素即fromTo[1]应该填入输出矩阵的某个通道标记，函数就会把输入矩阵的fromTo[0]通道里面的数据复制给输出矩阵的fromTo[1]通道，总之一个输入矩阵的通道标记后面必须跟着输出矩阵的通道标记。
------------------------------------
npairs–即参数fromTo中的有几组输入输出通道关系，其实就是参数fromTo的数组元素个数除以2
*/

/*
void calcHist(const Mat* arrays, int narrays, const int* channels, InputArray mask, SparseMat& hist, int dims, const int* histSize, const float** ranges, bool uniform=true, bool accumulate=false )
arrays 输入的图像的指针，可以是多幅图像，所有的图像必须有同样的深度（CV_8U or CV_32F），同时一副图像可以有多个channels
narrays 输入的图像的个数
------------------------------------
channels 用来计算直方图的channes的数组
比如输入是2张图像，第一副图像有0，1，2共三个channels，第二幅图像只有0一个channel，那么输入就一共有4个channels
如果int channels[3] = {3, 2, 0}，那么就表示是使用第二副图像的第一个通道和第一副图像的第2和第0个通道来计算直方图
------------------------------------
mask 掩码如果mask不为空，那么它必须是一个8位（CV_8U）的数组，并且它的大小的和arrays[i]的大小相同，值为1的点将用来计算直方图。
hist 计算出来的直方图
dims 计算出来的直方图的维数
histSize 在每一维上直方图的个数。简单把直方图看作一个一个的竖条的话，就是每一维上竖条的个数
------------------------------------
ranges 用来进行统计的范围
float rang1[] = {0, 20};
float rang2[] = {30, 40};
const float *rangs[] = {rang1, rang2};那么就是对0，20和30，40范围的值进行统计
------------------------------------
uniform 每一个竖条的宽度是否相等
*/
