/*******************************************************************
Creator : 2015112154 김석윤
********************************************************************
<Problem>
- CCTV를 이용한 거동 수상자 인식

<Input>
- 현재 실시간 영상

<Output>
- 거동수상자의 존재 유무 판단
********************************************************************/

#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <thread>
#include <iostream>
#include <chrono>

using namespace cv;

bool TIME_ELAPSED; // 일정 시간이 지났는지 확인하는 bool 변수.
RNG rng(12345);
#define DIFF_THRESHOLD 0.03
bool REMEMBER = true;
bool DETECTED = false;




int main()
{



	Mat realtime_frame; // 실시간 카메라로 들어오는 영상을 담기 위한 Mat 객체 선언.
	Mat record_frame; // 실시간 카메라로 들어오는 영상을 기록하기 위한 Mat 객체 선언
	Mat realtime_frame_with_biggest_contour;
	Mat sub;
	Mat background;
	Mat gray_background;
	Mat thrsh_background;
	double diff = 0;
	Point center_of_rect_previous = Point(-10, -10);
	Point center_of_rect = Point(-10, -10);


	VideoCapture cap("C:/Users/K/NaverCloud/대학/개별연구/sample/example4_edit1.mp4");
	namedWindow("Video", WINDOW_NORMAL);
	if (!cap.isOpened()) { std::cout << "동영상을 읽을 수 없음" << std::endl; }

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
