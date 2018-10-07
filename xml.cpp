//����д��xml
//http://www.cnblogs.com/skyfsm/p/7182313.html
#include<opencv2\opencv.hpp>   
#include<opencv2\highgui\highgui.hpp>

using namespace std;
using namespace cv;

//�Զ�������ݽṹ
typedef struct
{
	int x;
	int y;
	string s;
}test_t;


int main(int argc, char** argv)
{
	//XML/YAML�ļ���OpenCV�е����ݽṹΪFileStorage
	FileStorage fs("test.yml", FileStorage::WRITE); //����д����

	//��������
	int a1 = 1;
	char a2 = 'J';
	string str = "hello JZY!";
	int arr[10] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10 };
	test_t t = { 11, 12, "hi JZY" };
	map<string, int> m;
	m["kobe"] = 100;
	m["james"] = 99;
	m["curry"] = 98;

	//д���ļ�����,��д��ע��д����
	fs << "int_data" << a1;
	fs << "char_data" << a2;
	fs << "string_data" << str;

	//д������
	fs << "array_data" << "["; //���鿪ʼ
	for (int i = 0; i < 10; i++)
	{
		fs << arr[i];
	}
	fs << "]"; //�������

	//д���Զ���ṹ��
	fs << "struct_data" << "{"; //�ṹ�忪ʼ
	fs << "x" << t.x;
	fs << "y" << t.y;
	fs << "s" << t.s;
	fs << "}";  //�ṹ����


	//map��д�� 
	fs << "map_data" << "{";  //map�Ŀ�ʼд��
	map<string, int>::iterator it = m.begin();
	for (; it != m.end(); it++)
	{
		fs << it->first << it->second;
	}
	fs << "}";  //mapд�����

	return 0;
}