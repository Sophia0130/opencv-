////opencv��ͼ-�ı�
//#include "opencv2/imgproc.hpp"
//#include "opencv2/highgui.hpp"
//
//using namespace std;
//using namespace cv;
//
//
//int main()
//{
//	//�����հ�ͼ���ڻ�������  
//	cv::Mat image = cv::Mat::zeros(cv::Size(640, 480), CV_8UC3);
//	//������ɫ����  
//	image.setTo(cv::Scalar(100, 0, 0));
//
//	//���û����ı�����ز���  
//	std::string text = "Hello World !";
//	int font_face = cv::FONT_HERSHEY_COMPLEX;
//	double font_scale = 2;
//	int thickness = 2;
//	int baseline;
//	//��ȡ�ı���ĳ���  
//	cv::Size text_size = cv::getTextSize(text, font_face, font_scale, thickness, &baseline);
//
//	//���ı�����л���  
//	cv::Point origin;
//	origin.x = image.cols / 2 - text_size.width / 2;
//	origin.y = image.rows / 2 + text_size.height / 2;
//	cv::putText(image, text, origin, font_face, font_scale, cv::Scalar(0, 255, 255), thickness, 8, 0);
//
//	//��ʾ���ƽ��  
//	cv::imshow("image", image);
//	cv::waitKey(0);
//	return 0;
//}