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

bool TIME_ELAPSED = false; // ���� �ð��� �������� Ȯ���ϴ� bool ����.
RNG rng(12345);
#define DIFF_THRESHOLD 0.03
bool REMEMBER = true;
bool DETECTED = false;

using namespace cv;

Mat realtime_frame;
Mat record_frame;

// ������ ����ϱ� ���� �Լ�
void fun_record() {
	while (1) {
		realtime_frame = record_frame.clone(); // ���� ������ cam �� ������ �����Ѵ�.
		printf("interval\n");
		std::this_thread::sleep_for(std::chrono::milliseconds(100)); // N�и��� ��� time interval�� �д�.
		TIME_ELAPSED = true;
		std::this_thread::sleep_for(std::chrono::milliseconds(100)); // N�и��� ��� time interval�� �д�
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
	// �ǽð� ī�޶�� ������ ������ ��� ���� Mat ��ü ����.
	Mat result;
	Ptr<BackgroundSubtractor> pMOG2;
	int keyboard;

	// �ǽð� ī�޶�� ������ ������ ����ϱ� ���� Mat ��ü ����
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
	VideoCapture cap("C:/Users/K/NaverCloud/����/��������/sample/example2.mp4");

	cap.read(realtime_frame);
	imshow("realtime_frame_Start", realtime_frame);
	record_frame = realtime_frame.clone();

	std::thread thread_record(fun_record); // ������ ȭ���� ����ϱ� ���� ������ ����

	imshow("record_frame", record_frame);


	if (!cap.isOpened()) { std::cout << "�������� ���� �� ����" << std::endl; }

	while (true) {
		cap.read(realtime_frame);
		//imshow("realtime_frame", realtime_frame);

		if (TIME_ELAPSED) { // ��� ������ �ִ��� Ȯ��(=time interval�� �������� Ȯ��)

			if (isGrayImage(realtime_frame)) {
				cvtColor(realtime_frame, gray_realtime_frame, COLOR_BGR2GRAY); // ������ �׷��̽����Ϸ� ��ȯ
				cvtColor(record_frame, gray_record_frame, COLOR_BGR2GRAY); // ������ �׷��̽����Ϸ� ��ȯ
			}
			else {
				gray_realtime_frame = realtime_frame;
				gray_record_frame = record_frame;
			}


			// ����ȭ�� ���� ���� �����Ӱ� ���� �������� ������ ������ ����
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
