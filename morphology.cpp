/*ʹ����̬ѧ��������ȡˮƽ�ʹ�ֱ��
ʹ�ò�ͬ�ĽṹԪ�أ������ṹԪ�صĴ�С��״�ܹؼ�
�ȸ�ʴ�õ���Ӧ��ˮƽ��ֱ������ߣ������ͽ��߼Ӵ�
��music.png ������
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
	// Apply adaptiveThreshold at the bitwise_not of gray �ڰ׻���
	Mat bw;
	adaptiveThreshold(~gray, bw, 255, CV_ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 15, -2);

	//imshow("binary", bw);

	Mat horizontal = bw.clone();
	Mat vertical = bw.clone();

	/*******ˮƽ��*******/ //�õ�ˮƽ��
	// Specify size on horizontal axis
	int horizontalsize = horizontal.cols / 30;
	// Create structure element for extracting horizontal lines through morphology operations
	Mat horizontalStructure = getStructuringElement(MORPH_RECT, Size(horizontalsize, 1)); //��ȡˮƽ�ߵĽṹԪ��
	// Apply morphology operations
	erode(horizontal, horizontal, horizontalStructure, Point(-1, -1));
	dilate(horizontal, horizontal, horizontalStructure, Point(-1, -1));
	imshow("horizontal", horizontal);

	/*******��ֱ��*******/  //�õ���ֱ��
	// Specify size on vertical axis
	int verticalsize = vertical.rows / 30;
	// Create structure element for extracting vertical lines through morphology operations
	Mat verticalStructure = getStructuringElement(MORPH_RECT, Size(1, verticalsize));
	// Apply morphology operations
	erode(vertical, vertical, verticalStructure, Point(-1, -1));
	dilate(vertical, vertical, verticalStructure, Point(-1, -1));
	imshow("vertical", vertical);

	/*******��ֱ�����ͼ����д���*******/
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
	adaptiveThreshold(vertical, edges, 255, CV_ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 3, -2); //��ȡ��Ե
	imshow("edges", edges);
	//Step 2
	Mat kernel = Mat::ones(2, 2, CV_8UC1);
	dilate(edges, edges, kernel); //���ͱ�Ե����Ե�ֲ�
	imshow("dilate", edges);
	// Step 3
	Mat smooth;
	vertical.copyTo(smooth); 
	// Step 4
	blur(smooth, smooth, Size(2, 2)); //ƽ����ֱ�����ͼ��
	imshow("smooth1", smooth);
	// Step 5
	smooth.copyTo(vertical, edges); // copyTo( OutputArray m, InputArray mask) 
	//srcImage.copyto(dstImage, mask)��mask��Ϊһ����ģ�壬�����ĳ�����ص�(i, j)��ֵΪ1��ֻ����һͨ��������mask��ͨ�����ɣ����srcImage.at(i, j)����ֱֵ�Ӹ���dstImage.at(i, j)�������ֵΪ0��dstImage.at(i, j)��������ԭʼ����ֵ
	// Show final result
	imshow("smooth2", vertical);
	waitKey(0);
	return 0;
}