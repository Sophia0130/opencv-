/*使用形态学操作来提取水平和垂直线
使用不同的结构元素！！！结构元素的大小形状很关键
先腐蚀得到相应的水平或垂直方向的线，再膨胀将线加粗
用music.png 做测试
*/

#include <iostream>
#include <opencv2/opencv.hpp>
using namespace std;
using namespace cv;
int main(int, char** argv)
{
	Mat src = imread(argv[1]);
	if (!src.data)
		cerr << "Problem loading image!!!" << endl;

	//imshow("src", src);
	Mat gray;
	if (src.channels() == 3)
	{
		cvtColor(src, gray, CV_BGR2GRAY);
	}
	else
	{
		gray = src;
	}
	//imshow("gray", gray);
	// Apply adaptiveThreshold at the bitwise_not of gray 黑白互换
	Mat bw;
	adaptiveThreshold(~gray, bw, 255, CV_ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 15, -2);

	//imshow("binary", bw);

	Mat horizontal = bw.clone();
	Mat vertical = bw.clone();

	/*******水平线*******/ //得到水平线
	// Specify size on horizontal axis
	int horizontalsize = horizontal.cols / 30;
	// Create structure element for extracting horizontal lines through morphology operations
	Mat horizontalStructure = getStructuringElement(MORPH_RECT, Size(horizontalsize, 1)); //提取水平线的结构元素
	// Apply morphology operations
	erode(horizontal, horizontal, horizontalStructure, Point(-1, -1));
	dilate(horizontal, horizontal, horizontalStructure, Point(-1, -1));
	imshow("horizontal", horizontal);

	/*******竖直线*******/  //得到竖直线
	// Specify size on vertical axis
	int verticalsize = vertical.rows / 30;
	// Create structure element for extracting vertical lines through morphology operations
	Mat verticalStructure = getStructuringElement(MORPH_RECT, Size(1, verticalsize));
	// Apply morphology operations
	erode(vertical, vertical, verticalStructure, Point(-1, -1));
	dilate(vertical, vertical, verticalStructure, Point(-1, -1));
	imshow("vertical", vertical);

	/*******竖直方向的图像进行处理*******/
	// Inverse vertical image
	bitwise_not(vertical, vertical);
	imshow("vertical_bit", vertical);

	// Extract edges and smooth image according to the logic
	// 1. extract edges
	// 2. dilate(edges)
	// 3. vertical.copyTo(smooth)
	// 4. blur smooth img
	// 5. smooth.copyTo(src, edges)

	// Step 1
	Mat edges;
	adaptiveThreshold(vertical, edges, 255, CV_ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 3, -2); //获取边缘
	imshow("edges", edges);
	//Step 2
	Mat kernel = Mat::ones(2, 2, CV_8UC1);
	dilate(edges, edges, kernel); //膨胀边缘，边缘粗糙
	imshow("dilate", edges);
	// Step 3
	Mat smooth;
	vertical.copyTo(smooth); 
	// Step 4
	blur(smooth, smooth, Size(2, 2)); //平滑竖直方向的图像
	imshow("smooth1", smooth);
	// Step 5
	smooth.copyTo(vertical, edges); // copyTo( OutputArray m, InputArray mask) 
	//srcImage.copyto(dstImage, mask)，mask作为一个掩模板，如果在某个像素点(i, j)其值为1（只看第一通道，所以mask单通道即可）则把srcImage.at(i, j)处的值直接赋给dstImage.at(i, j)，如果其值为0则dstImage.at(i, j)处保留其原始像素值
	// Show final result
	imshow("smooth2", vertical);
	waitKey(0);
	return 0;
}