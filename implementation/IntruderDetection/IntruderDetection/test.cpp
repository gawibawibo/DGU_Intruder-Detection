/*******************************************************************
Creator : 2015112154 �輮��
********************************************************************
<Problem>
- CCTV�� �̿��� �ŵ� ������ �ν�

<Input>
- ���� �ǽð� ����

<Output>
- �ŵ��������� ���� ���� �Ǵ�
********************************************************************/

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <thread>
#include <iostream>
#include <chrono>

using namespace cv;

bool TIME_ELAPSED; // ���� �ð��� �������� Ȯ���ϴ� bool ����.
RNG rng(12345);
#define DIFF_THRESHOLD 0.03
bool REMEMBER = true;
bool DETECTED = false;




int main()
{



	Mat realtime_frame; // �ǽð� ī�޶�� ������ ������ ��� ���� Mat ��ü ����.
	Mat record_frame; // �ǽð� ī�޶�� ������ ������ ����ϱ� ���� Mat ��ü ����
	Mat realtime_frame_with_biggest_contour;
	Mat sub;
	Mat background;
	Mat gray_background;
	Mat thrsh_background;
	double diff = 0;
	Point center_of_rect_previous = Point(-10, -10);
	Point center_of_rect = Point(-10, -10);


	VideoCapture cap("C:/Users/K/NaverCloud/����/��������/sample/example4_edit1.mp4");
	namedWindow("Video", WINDOW_NORMAL);
	if (!cap.isOpened()) { std::cout << "�������� ���� �� ����" << std::endl; }

	while (true) {
		cap >> realtime_frame;
		cvtColor(realtime_frame, realtime_frame, COLOR_BGR2GRAY);
		cvtColor(realtime_frame, realtime_frame, COLOR_BGR2GRAY);
		//cvtColor(realtime_frame, realtime_frame, COLOR_BGR2GRAY);*/
		imshow("Video", realtime_frame);

		if (waitKey(30) >= 0) break;
	}

	return 0;
}
