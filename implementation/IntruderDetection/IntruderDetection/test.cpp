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
#include <thread>
#include <iostream>
#include <chrono>

using namespace cv;

RNG rng(12345);




int main()
{
	VideoCapture cam(0); // ���� �̹����� �ҷ��´�.

	cam.set(CV_CAP_PROP_FRAME_WIDTH, 600); // CV_CAP_PROP_FRAME_WIDTH : Width of the frames in the video stream.
	cam.set(CV_CAP_PROP_FRAME_WIDTH, 400); // CV_CAP_PROP_FRAME_WIDTH : Height of the frames in the video stream.
	Mat realtime_frame; // �ǽð� ī�޶�� ������ ������ ��� ���� Mat ��ü ����.


	HOGDescriptor hog;
	hog.setSVMDetector(HOGDescriptor::getDefaultPeopleDetector());
	Mat frame;

	while (cam.read(realtime_frame)) { // ���� ������ �ݺ�
		if (realtime_frame.empty()) { // cv::Mat::empty �Լ�. ���� ��� element�� ������ true�� ��ȯ�Ѵ�. ��, ī�޶� ���� ���� ������ ������ if���� �����Ѵ�.
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

		if (waitKey(10) == 27) { // 27 : ESC�� ������ ����. 27 == ESC key
			break;
		}
	}

	waitKey();
	return 0;
}
