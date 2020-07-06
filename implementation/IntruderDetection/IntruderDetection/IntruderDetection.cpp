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

bool TIME_ELAPSED; // ���� �ð��� �������� Ȯ���ϴ� bool ����.
RNG rng(12345);
#define DIFF_THRESHOLD 0.01


// ������ ����ϱ� ���� �Լ�
void fun_record(Mat* realtime, Mat* record) {
	while (1) {
		*record = (*realtime).clone(); // ���� ������ cam �� ������ �����Ѵ�.
		GaussianBlur(*record, *record, Size(3, 3), 0, 0);// gray_realtime_frame �� ��ó��
		std::cout << "time interval" << std::endl;

		std::this_thread::sleep_for(std::chrono::milliseconds(100)); // N�и��� ��� time interval�� �д�.
		TIME_ELAPSED = true;
		std::this_thread::sleep_for(std::chrono::milliseconds(100)); // N�и��� ��� time interval�� �д�
	}
}

int main()
{
	VideoCapture cam(0); // ���� �̹����� �ҷ��´�.

	cam.set(CV_CAP_PROP_FRAME_WIDTH, 600); // CV_CAP_PROP_FRAME_WIDTH : Width of the frames in the video stream.
	cam.set(CV_CAP_PROP_FRAME_WIDTH, 400); // CV_CAP_PROP_FRAME_WIDTH : Height of the frames in the video stream.
	Mat realtime_frame; // �ǽð� ī�޶�� ������ ������ ��� ���� Mat ��ü ����.
	Mat record_frame; // �ǽð� ī�޶�� ������ ������ ����ϱ� ���� Mat ��ü ����

	std::thread thread_record(fun_record, &realtime_frame, &record_frame); // ������ ȭ���� ����ϱ� ���� ������ ����

	while (cam.read(realtime_frame)) { // ���� ������ �ݺ�
		if (realtime_frame.empty()) { // cv::Mat::empty �Լ�. ���� ��� element�� ������ true�� ��ȯ�Ѵ�. ��, ī�޶� ���� ���� ������ ������ if���� �����Ѵ�.
			printf(" --(!) No cam frame -- Break!");
			break;
		}

		resize(record_frame, record_frame, Size(realtime_frame.cols, realtime_frame.rows), 0, 0, CV_INTER_LINEAR);

		Mat gray_realtime_frame; // realtime_frame ������ �׷��� �����Ϸ� �����ϱ� ���� ��ü
		cvtColor(realtime_frame, gray_realtime_frame, COLOR_BGR2GRAY); // ������ �׷��̽����Ϸ� ��ȯ
		GaussianBlur(gray_realtime_frame, gray_realtime_frame, Size(3, 3), 0, 0);// gray_realtime_frame �� ��ó��

		Mat thrsh_realtime_frame; // �ǽð� ������ ����ȭ �������� �����ϱ� ���� Mat ��ü ����
		threshold(gray_realtime_frame, thrsh_realtime_frame, 0, 255, THRESH_BINARY | THRESH_OTSU); // ������ ����ȭ�Ѵ�.
		imshow("thrsh_realtime_frame", thrsh_realtime_frame);

		Mat	thrsh_record_frame; // �ǽð� ����� ��ϵ� ������ ����ȭ�Ͽ� �����ϱ� ���� ��ü ����

		if (TIME_ELAPSED) { // ��� ������ �ִ��� Ȯ��(=time interval�� �������� Ȯ��)

			Mat gray_record_frame; // record_frame ������ �׷��� �����Ϸ� �����ϱ� ���� ��ü
			cvtColor(record_frame, gray_record_frame, COLOR_BGR2GRAY); // ������ �׷��̽����Ϸ� ��ȯ

			// ����ȭ�� ���� ���� �����Ӱ� ���� �������� ������ ������ ����
			double errorL2 = cv::norm(gray_realtime_frame, gray_record_frame, CV_L2);
			double diff = errorL2 / (double)(gray_realtime_frame.rows * gray_record_frame.rows);

			if (diff >= DIFF_THRESHOLD) { // ħ���ڰ� �߻��ߴٰ� �Ǵ��� ���
				std::cout << "intruder~!!!" << std::endl;

				threshold(gray_record_frame, thrsh_record_frame, 0, 255, THRESH_BINARY | THRESH_OTSU); // ������ ����ȭ�Ѵ�. 

				imshow("thrsh_record_frame", thrsh_record_frame);



				Mat sub = thrsh_realtime_frame - thrsh_record_frame; // �ǽð� ȭ�鿡�� record ������ ����.
				imshow("sub", sub); // sub ���

				Mat CannyEdge; // sub ���󿡼� canny edge�� ������ ����� �����ϱ� ���� ��ü ����
				Canny(sub, CannyEdge, 0, 255);
				imshow("CannyEdge", CannyEdge); // sub ���

				std::vector<std::vector<Point> > contours;
				std::vector<Vec4i> hierarchy;
				findContours(CannyEdge, contours, RETR_LIST, CHAIN_APPROX_NONE);

				Mat drawing = Mat::zeros(CannyEdge.size(), CV_8UC3);
				Mat drawing2 = Mat::zeros(CannyEdge.size(), CV_8UC3);
				int largest_area = 0;
				int largest_contour_index = 0;
				Rect bounding_rect;
				for (int i = 0; i < contours.size(); i++)
				{
					Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
					drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());

					double area = contourArea(contours[i]);  //  Find the area of contour
					if (area > largest_area)
					{
						largest_area = area;
						largest_contour_index = i;               //Store the index of largest contour
						bounding_rect = boundingRect(contours[i]); // Find the bounding rectangle for biggest contour
					}
				}

				imshow("Contours", drawing);

				drawContours(drawing2, contours, largest_contour_index, Scalar(0, 255, 0), 2); // Draw the largest contour using previously stored index.

				imshow("biggest contours", drawing2);
			}
			TIME_ELAPSED = false;
		}
		else {

		}


		if (waitKey(10) == 27) { // 27 : ESC�� ������ ����. 27 == ESC key
			break;
		}
	}

	waitKey();
	return 0;
}
