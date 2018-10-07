//图像金字塔实现图像尺寸大放大和缩小

#include "opencv2/imgproc.hpp"
#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui.hpp"
using namespace cv;
Mat src, dst, tmp;
const char* window_name = "Pyramids Demo";
int main(int argc, char** argv)
{
	printf("\n Zoom In-Out demo  \n ");
	printf("------------------ \n");
	printf(" * [u] -> Zoom in  \n");
	printf(" * [d] -> Zoom out \n");
	printf(" * [ESC] -> Close program \n \n");
	Mat src = imread(argv[1]);
	if (src.empty())
	{
		printf(" No data! -- Exiting the program \n");
		return -1;
	}
	tmp = src;
	dst = tmp;
	imshow(window_name, dst);
	for (;;)
	{
		char c = (char)waitKey(0);
		if (c == 27)
		{
			break;
		}
		if (c == 'u')
		{
			pyrUp(tmp, dst, Size(tmp.cols * 2, tmp.rows * 2));
			printf("** UP: Image x 2 \n");
		}
		else if (c == 'd')
		{
			pyrDown(tmp, dst, Size(tmp.cols / 2, tmp.rows / 2));
			printf("** DOWN: Image / 2 \n");
		}
		imshow(window_name, dst);
		tmp = dst;
	}
	waitKey(0);
	return 0;
}