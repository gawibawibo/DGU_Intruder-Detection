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
#include "opencv2/opencv.hpp"
#include <iostream>

using namespace cv;
using namespace std;

int main()
{
	VideoCapture cap("KakaoTalk_20200712_180345359.mp4");

	if (!cap.isOpened()) {
		cerr << "Video open failed!" << endl;
		return -1;

	}

	HOGDescriptor hog;
	hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());

	Mat frame;
	while (true) {
		cap >> frame;
		if (frame.empty())
			break;

		vector<Rect> detected;
		hog.detectMultiScale(frame, detected);

		for (Rect r : detected) {
			Scalar c = Scalar(rand() % 256, rand() % 256, rand() % 256);
			rectangle(frame, r, c, 3);

		}

		imshow("frame", frame);

		if (waitKey(10) == 27)
			break;

	}

	return 0;
}