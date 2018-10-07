//离散傅里叶变换 DFT

#include "opencv2/core.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"

#include <iostream>

using namespace cv;
using namespace std;

static void help(void)
{
	cout << endl
		<< "This program demonstrated the use of the discrete Fourier transform (DFT). " << endl
		<< "The dft of an image is taken and it's power spectrum is displayed." << endl;
}

int main(int argc, char ** argv)
{
	help();

	const char* filename = argv[1];

	Mat I = imread(filename, IMREAD_GRAYSCALE);
	if (I.empty()){
		cout << "Error opening image" << endl;
		return -1;
	}

	//! [expand]  当图像的尺寸是2， 3，5的整数倍时，傅里叶变换的计算速度最快
	Mat padded; 
	int m = getOptimalDFTSize(I.rows); //获取最佳尺寸
	int n = getOptimalDFTSize(I.cols);
	copyMakeBorder(I, padded, 0, m - I.rows, 0, n - I.cols, BORDER_CONSTANT, Scalar::all(0)); //将边缘像素填充为0
	//! [expand]

	//! [complex_and_real] 为傅里叶变换的实部虚部分配内存
	Mat planes[] = { Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F) }; //在padded基础上加一个初始化为0的通道
	Mat complexI;
	merge(planes, 2, complexI);  //将单通道数组合并成一个多通道的数组，从而创建出一个多通道阵列 
	//! [complex_and_real]

	//! [dft]
	dft(complexI, complexI); 
	//! [dft]

	// compute the magnitude 
	//! [magnitude]
	split(complexI, planes);    // planes[0] = Re(DFT(I), planes[1] = Im(DFT(I))
	magnitude(planes[0], planes[1], planes[0]);// 计算实部和虚部的幅值，放在planes[0]
	Mat magI = planes[0];
	//! [magnitude]

	//! [log] switch to logarithmic scale
	// => log(1 + sqrt(Re(DFT(I))^2 + Im(DFT(I))^2))
	magI += Scalar::all(1); //傅立叶变换的幅度值范围大到不适合在屏幕上显示，需要做对数尺度缩放
	log(magI, magI);
	//! [log]

	//! [crop_rearrange]
	// crop the spectrum, if it has an odd number of rows or columns
	magI = magI(Rect(0, 0, magI.cols & -2, magI.rows & -2)); //剔除添加的像素(和-2按位与) ？？实在不懂是为什么啊

	// rearrange the quadrants of Fourier image  so that the origin is at the image center 重分布幅度图象限
	int cx = magI.cols / 2;
	int cy = magI.rows / 2;

	Mat q0(magI, Rect(0, 0, cx, cy));   // Top-Left - Create a ROI per quadrant
	Mat q1(magI, Rect(cx, 0, cx, cy));  // Top-Right
	Mat q2(magI, Rect(0, cy, cx, cy));  // Bottom-Left
	Mat q3(magI, Rect(cx, cy, cx, cy)); // Bottom-Right

	Mat tmp;                           // swap quadrants (Top-Left with Bottom-Right)
	q0.copyTo(tmp);
	q3.copyTo(q0);
	tmp.copyTo(q3);

	q1.copyTo(tmp);                    // swap quadrant (Top-Right with Bottom-Left)
	q2.copyTo(q1);
	tmp.copyTo(q2);
	//! [crop_rearrange]

	//! [normalize]
	normalize(magI, magI, 0, 1, NORM_MINMAX);//数将幅度归一化到可显示范围//（不能放到0-255）
	//! [normalize]

	imshow("Input Image", I);    
	imshow("spectrum magnitude", magI);
	waitKey();

	return 0;
}