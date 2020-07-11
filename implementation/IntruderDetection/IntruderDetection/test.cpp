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

RNG rng(12345);




int main()
{
	VideoCapture cam(0); // 비디오 이미지를 불러온다.

	cam.set(CV_CAP_PROP_FRAME_WIDTH, 600); // CV_CAP_PROP_FRAME_WIDTH : Width of the frames in the video stream.
	cam.set(CV_CAP_PROP_FRAME_WIDTH, 400); // CV_CAP_PROP_FRAME_WIDTH : Height of the frames in the video stream.
	Mat realtime_frame; // 실시간 카메라로 들어오는 영상을 담기 위한 Mat 객체 선언.


	HOGDescriptor hog;
	hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());
	Mat frame;

	while (cam.read(realtime_frame)) { // 종료 전까지 반복
		if (realtime_frame.empty()) { // cv::Mat::empty 함수. 만약 어느 element도 없으면 true를 반환한다. 즉, 카메라를 통해 들어온 영상이 없으면 if문을 실행한다.
			printf(" --(!) No cam frame -- Break!");
			break;
		}

		



		std::vector<Rect> detected;
		hog.detectMultiScale(realtime_frame, detected);

		for (Rect r : detected) {
			Scalar c = Scalar(rand() % 256, rand() % 256, rand() % 256);
			rectangle(realtime_frame, r, c, 3);

		}

		imshow("realtime_frame", realtime_frame);

		if (waitKey(10) == 27) { // 27 : ESC를 누르면 종료. 27 == ESC key
			break;
		}
	}

	waitKey();
	return 0;
}
