//扫描、读取图像的四种方法的比较
//命令行输入的三个参数  imageName.jpg  divideWith  intValueToReduce [G](可省略)


#include <opencv2/core.hpp>
#include <opencv2/core/utility.hpp>
#include "opencv2/imgcodecs.hpp"
#include <opencv2/highgui.hpp>
#include <iostream>
#include <sstream>

using namespace std;
using namespace cv;

static void help()
{
	cout
		<< "\n--------------------------------------------------------------------------" << endl
		<< "This program shows how to scan image objects in OpenCV (cv::Mat)."
		<< " we take an input image and divide the native color palette (255) with the input. " << endl
		<< "Shows C operator[] method, iterators and at function for on-the-fly item address calculation." << endl
		<< "./输入三个参数 <imageNameToUse> <divideWith> [G]" << endl
		<< "if you add a G parameter the image is processed in gray scale" << endl
		<< "--------------------------------------------------------------------------" << endl
		<< endl;
}

Mat& ScanImageAndReduceC(Mat& I, const uchar* table);
Mat& ScanImageAndReduceIterator(Mat& I, const uchar* table);
Mat& ScanImageAndReduceRandomAccess(Mat& I, const uchar * table);

int main(int argc, char* argv[])
{
	help();
	if (argc < 3)
	{
		cout << "Not enough parameters" << endl;
		return -1;
	}

	Mat I, J;
	if (argc == 4 && !strcmp(argv[3], "G"))
		I = imread(argv[1], IMREAD_GRAYSCALE);
	else
		I = imread(argv[1], IMREAD_COLOR);

	if (I.empty())
	{
		cout << "The image" << argv[1] << " could not be loaded." << endl;
		return -1;
	}

	//用命令行参数argv[2]给出的整数进行颜色缩减
	int divideWith = 0; // convert our input string to number - C++ style
	stringstream s;
	s << argv[2];
	s >> divideWith;
	if (!s || !divideWith)
	{
		cout << "Invalid number entered for dividing. " << endl;
		return -1;
	}

	//查找表
	uchar table[256]; //查找表
	for (int i = 0; i < 256; ++i)
		table[i] = (uchar)(divideWith * (i / divideWith));
	//divideWith=10时，0到9取为0，10到19取为10

	// 运行时间-单位毫秒
	// getTickCount() 返回CPU自某个事件以来走过的时钟周期数
	// getTickFrequency()  返回CPU一秒钟所走的时钟周期数
	const int times = 100;
	double t;

	t = (double)getTickCount();

	for (int i = 0; i < times; ++i)
	{
		cv::Mat clone_i = I.clone();
		J = ScanImageAndReduceC(clone_i, table);
	}

	t = 1000 * ((double)getTickCount() - t) / getTickFrequency();
	t /= times;

	cout << "Time of reducing with the C operator [] (averaged for "
		<< times << " runs): " << t << " milliseconds." << endl;

	t = (double)getTickCount();

	for (int i = 0; i < times; ++i)
	{
		cv::Mat clone_i = I.clone();
		J = ScanImageAndReduceIterator(clone_i, table);
	}

	t = 1000 * ((double)getTickCount() - t) / getTickFrequency();
	t /= times;

	cout << "Time of reducing with the iterator (averaged for "
		<< times << " runs): " << t << " milliseconds." << endl;

	t = (double)getTickCount();

	for (int i = 0; i < times; ++i)
	{
		cv::Mat clone_i = I.clone();
		ScanImageAndReduceRandomAccess(clone_i, table);
	}

	t = 1000 * ((double)getTickCount() - t) / getTickFrequency();
	t /= times;

	cout << "Time of reducing with the on-the-fly address generation - at function (averaged for "
		<< times << " runs): " << t << " milliseconds." << endl;

	//
	//! [table-init] 最被推荐的用于实现批量图像元素查找和更该操作图像方法，在图像处理中，对于一个给定的值，将其替换成其他的值是一个很常见的操作
	Mat lookUpTable(1, 256, CV_8U);
	uchar* p = lookUpTable.ptr();
	for (int i = 0; i < 256; ++i)
		p[i] = table[i]; //table数组的值赋给lookUpTable Mat
	//! [table-init]

	t = (double)getTickCount();

	for (int i = 0; i < times; ++i)
		//! [table-use]
		LUT(I, lookUpTable, J);
	//! [table-use]

	t = 1000 * ((double)getTickCount() - t) / getTickFrequency();
	t /= times;

	cout << "Time of reducing with the LUT function (averaged for "
		<< times << " runs): " << t << " milliseconds." << endl;
	system("pause");
	return 0;
}

//! [scan-c] c语言风格，用指针
Mat& ScanImageAndReduceC(Mat& I, const uchar* const table)
{
	// accept only char type matrices
	CV_Assert(I.depth() == CV_8U); //CV_Assert()若括号中的表达式值为false，则返回一个错误信息

	int channels = I.channels();

	int nRows = I.rows;
	int nCols = I.cols * channels;

	if (I.isContinuous()) //判断图像在内存中是否连续存储
	{
		nCols *= nRows;   //若图像连续存储将图像作为一维数组
		nRows = 1;
	}

	//若图像连续存储，则可以将图像的数据看成是一个一维数组
	//p（uchar*）就是指向图像数据的第一个字节的，因此可以用data指针访问图像的数据,从而加速Mat图像的访问速度
	//一般经过裁剪的Mat图像，都不再连续了
	int i, j;
	uchar* p;
	for (i = 0; i < nRows; ++i)
	{
		p = I.ptr<uchar>(i);
		for (j = 0; j < nCols; ++j)
		{
			p[j] = table[p[j]];
		}
	}
	return I;
}
//! [scan-c]

//迭代法
//! [scan-iterator] 获得图像矩阵的begin和end
Mat& ScanImageAndReduceIterator(Mat& I, const uchar* const table)
{
	// accept only char type matrices
	CV_Assert(I.depth() == CV_8U);

	const int channels = I.channels();
	switch (channels)
	{
	case 1:
	{
			  MatIterator_<uchar> it, end;
			  for (it = I.begin<uchar>(), end = I.end<uchar>(); it != end; ++it)
				  *it = table[*it];
			  break;
	}
	case 3:
	{
			  MatIterator_<Vec3b> it, end;
			  for (it = I.begin<Vec3b>(), end = I.end<Vec3b>(); it != end; ++it)
			  {
				  (*it)[0] = table[(*it)[0]];
				  (*it)[1] = table[(*it)[1]];
				  (*it)[2] = table[(*it)[2]];
			  }
	}
	}

	return I;
}
//! [scan-iterator]

// On-the-fly 地址计算
//! [scan-random] 这个方法并不推荐被用来进行图像扫描，需要知道元素的所在行数与列数还有数据类型
Mat& ScanImageAndReduceRandomAccess(Mat& I, const uchar* const table)
{
	// accept only char type matrices
	CV_Assert(I.depth() == CV_8U);

	const int channels = I.channels();
	switch (channels)
	{
	case 1:
	{
			  for (int i = 0; i < I.rows; ++i)
			  for (int j = 0; j < I.cols; ++j)
				  I.at<uchar>(i, j) = table[I.at<uchar>(i, j)];
			  break;
	}
	case 3:
	{
			  Mat_<Vec3b> _I = I;

			  for (int i = 0; i < I.rows; ++i)
			  for (int j = 0; j < I.cols; ++j)
			  {
				  _I(i, j)[0] = table[_I(i, j)[0]];
				  _I(i, j)[1] = table[_I(i, j)[1]];
				  _I(i, j)[2] = table[_I(i, j)[2]];
			  }
			  I = _I;
			  break;
	}
	}

	return I;
}