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
#include <thread>
#include <iostream>
#include <chrono>

using namespace cv;

bool TIME_ELAPSED = false; // 일정 시간이 지났는지 확인하는 bool 변수.
RNG rng(12345);
#define DIFF_THRESHOLD 0.03
bool REMEMBER = true;
bool DETECTED = false;

using namespace cv;

Mat realtime_frame;
Mat record_frame;

// 영상을 기록하기 위한 함수
void fun_record() {
	while (1) {
		realtime_frame = record_frame.clone(); // 원본 영상인 cam 의 영상을 복사한다.
		printf("interval\n");
		std::this_thread::sleep_for(std::chrono::milliseconds(100)); // N밀리초 라는 time interval을 둔다.
		TIME_ELAPSED = true;
		std::this_thread::sleep_for(std::chrono::milliseconds(100)); // N밀리초 라는 time interval을 둔다
	}
}


bool isGrayImage(Mat img) // returns true if the given 3 channel image is B = G = R
{
	Mat dst;
	Mat bgr[3];
	split(img, bgr);
	absdiff(bgr[0], bgr[1], dst);

	if (countNonZero(dst))
		return false;

	absdiff(bgr[0], bgr[2], dst);
	return !countNonZero(dst);
}


int main()
{
	// 실시간 카메라로 들어오는 영상을 담기 위한 Mat 객체 선언.
	Mat result;
	Ptr<BackgroundSubtractor> pMOG2;
	int keyboard;

	// 실시간 카메라로 들어오는 영상을 기록하기 위한 Mat 객체 선언
	Mat realtime_frame_with_biggest_contour;
	Mat gray_record_frame;
	Mat gray_realtime_frame;
	Mat sub;
	Mat background;
	Mat gray_background;
	Mat thrsh_background;
	double diff = 0;
	Point center_of_rect_previous = Point(-10, -10);
	Point center_of_rect = Point(-10, -10);

	pMOG2 = createBackgroundSubtractorMOG2();
	VideoCapture cap("C:/Users/K/NaverCloud/대학/개별연구/sample/example2.mp4");

	cap.read(realtime_frame);
	imshow("realtime_frame_Start", realtime_frame);
	record_frame = realtime_frame.clone();

	std::thread thread_record(fun_record); // 영상의 화면을 기록하기 위한 스레드 생성

	imshow("record_frame", record_frame);


	if (!cap.isOpened()) { std::cout << "동영상을 읽을 수 없음" << std::endl; }

	while (true) {
		cap.read(realtime_frame);
		//imshow("realtime_frame", realtime_frame);

		if (TIME_ELAPSED) { // 기록 영상이 있는지 확인(=time interval이 지났는지 확인)

			if (isGrayImage(realtime_frame)) {
				cvtColor(realtime_frame, gray_realtime_frame, COLOR_BGR2GRAY); // 영상을 그레이스케일로 변환
				cvtColor(record_frame, gray_record_frame, COLOR_BGR2GRAY); // 영상을 그레이스케일로 변환
			}
			else {
				gray_realtime_frame = realtime_frame;
				gray_record_frame = record_frame;
			}


			// 정규화를 통해 이전 프레임과 현재 프레임의 차이의 정도를 구함
			double errorL2 = cv::norm(gray_realtime_frame, gray_record_frame, CV_L2);
			diff = errorL2 / (double)(gray_realtime_frame.rows * gray_record_frame.rows);

			if (diff >= DIFF_THRESHOLD) {
				printf("intruder!!\n");

				pMOG2->apply(realtime_frame, result);
				imshow("realtime_frame", realtime_frame);
				imshow("result", result);

			}
			TIME_ELAPSED = false;
		}




		if (waitKey(30) >= 0) break;
	}
	cap.release();


	return 0;
}
