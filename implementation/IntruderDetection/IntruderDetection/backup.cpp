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
#define DIFF_THRESHOLD 0.03
bool REMEMBER = true;
bool DETECTED = false;


// ������ ����ϱ� ���� �Լ�
void fun_record(Mat* realtime, Mat* record) {
	while (1) {
		*record = (*realtime).clone(); // ���� ������ cam �� ������ �����Ѵ�.
		GaussianBlur(*record, *record, Size(3, 3), 0, 0);// gray_realtime_frame �� ��ó��
		//std::cout << "time interval" << std::endl;

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
	Mat realtime_frame_with_biggest_contour;
	Mat sub;
	Mat background;
	Mat gray_background;
	Mat thrsh_background;
	double diff = 0;
	Point center_of_rect_previous = Point(-10, -10);
	Point center_of_rect = Point(-10, -10);

	std::thread thread_record(fun_record, &realtime_frame, &record_frame); // ������ ȭ���� ����ϱ� ���� ������ ����

	while (cam.read(realtime_frame)) { // ���� ������ �ݺ�
		if (realtime_frame.empty()) { // cv::Mat::empty �Լ�. ���� ��� element�� ������ true�� ��ȯ�Ѵ�. ��, ī�޶� ���� ���� ������ ������ if���� �����Ѵ�.
			printf(" --(!) No cam frame -- Break!");
			break;
		}

		resize(record_frame, record_frame, Size(realtime_frame.cols, realtime_frame.rows), 0, 0, CV_INTER_LINEAR);
		realtime_frame_with_biggest_contour = realtime_frame.clone();

		Mat gray_realtime_frame; // realtime_frame ������ �׷��� �����Ϸ� �����ϱ� ���� ��ü
		cvtColor(realtime_frame, gray_realtime_frame, COLOR_BGR2GRAY); // ������ �׷��̽����Ϸ� ��ȯ
		GaussianBlur(gray_realtime_frame, gray_realtime_frame, Size(3, 3), 0, 0);// gray_realtime_frame �� ��ó��


		Mat thrsh_realtime_frame; // �ǽð� ������ ����ȭ �������� �����ϱ� ���� Mat ��ü ����
		//threshold(gray_realtime_frame, thrsh_realtime_frame, 0, 255, THRESH_BINARY | THRESH_OTSU); // ������ ����ȭ�Ѵ�.
		adaptiveThreshold(gray_realtime_frame, thrsh_realtime_frame, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 5, 10);

		//imshow("thrsh_realtime_frame", thrsh_realtime_frame);

		Mat	thrsh_record_frame; // �ǽð� ����� ��ϵ� ������ ����ȭ�Ͽ� �����ϱ� ���� ��ü ����

		if (REMEMBER && (diff < DIFF_THRESHOLD)) {
			printf("secured\n");
			background = thrsh_realtime_frame.clone();
			REMEMBER = false;
		}

		if (TIME_ELAPSED) { // ��� ������ �ִ��� Ȯ��(=time interval�� �������� Ȯ��)

			Mat gray_record_frame; // record_frame ������ �׷��� �����Ϸ� �����ϱ� ���� ��ü
			cvtColor(record_frame, gray_record_frame, COLOR_BGR2GRAY); // ������ �׷��̽����Ϸ� ��ȯ

			// ����ȭ�� ���� ���� �����Ӱ� ���� �������� ������ ������ ����
			double errorL2 = cv::norm(gray_realtime_frame, gray_record_frame, CV_L2);
			diff = errorL2 / (double)(gray_realtime_frame.rows * gray_record_frame.rows);



			if ((DETECTED || (diff >= DIFF_THRESHOLD))) { // ħ���ڰ� �߻��ߴٰ� �Ǵ��� ���

				//threshold(gray_record_frame, thrsh_record_frame, 0, 255, THRESH_BINARY | THRESH_OTSU); // ������ ����ȭ�Ѵ�. 
				adaptiveThreshold(gray_record_frame, thrsh_record_frame, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 5, 10);
				//erode(thrsh_record_frame, thrsh_record_frame, Mat(3, 3, CV_8U, Scalar(1)), Point(-1, -1), 2);
				morphologyEx(thrsh_record_frame, thrsh_record_frame, MORPH_OPEN, Mat(3, 3, CV_8U, Scalar(1)), Point(-1, -1), 2);
				imshow("thrsh_record_frame", thrsh_record_frame);



				//erode(thrsh_realtime_frame, thrsh_realtime_frame, Mat(3, 3, CV_8U, Scalar(1)), Point(-1, -1), 2);
				morphologyEx(thrsh_realtime_frame, thrsh_realtime_frame, MORPH_OPEN, Mat(3, 3, CV_8U, Scalar(1)), Point(-1, -1), 2);
				morphologyEx(background, background, MORPH_OPEN, Mat(3, 3, CV_8U, Scalar(1)), Point(-1, -1), 2);

				imshow("thrsh_realtime_frame", thrsh_realtime_frame);



				sub = thrsh_realtime_frame - background; // �ǽð� ȭ�鿡�� record ������ ����.
				imshow("sub", sub); // sub ���
				//GaussianBlur(sub, sub, Size(3, 3), 0, 0);// gray_realtime_frame �� ��ó��
				//imshow("edited sub", sub); // sub ���

				Mat opening_sub;
				morphologyEx(sub, opening_sub, MORPH_OPEN, Mat(3, 3, CV_8U, Scalar(1)), Point(-1, -1), 2);
				imshow("opening_sub", opening_sub); // opening_sub ���

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

				///////////////////////////////////////////////////////////////////////////////// bound rect�� �׸��� ����
				// contour�� ���� outbound rect�� �׸��� ���� ���� ����
				std::vector<std::vector<Point> > contours_poly(contours.size());
				std::vector<Rect> boundRect(contours.size());
				///////////////////////////////////////////////////////////////////////////// bound rect�� �׸��� ����

				for (int i = 0; i < contours.size(); i++)
				{
					approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
					boundRect[i] = boundingRect(Mat(contours_poly[i]));

					Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
					drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());
					Scalar color2 = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255)); // rect�� �׸��� ��
					rectangle(drawing, boundRect[i].tl(), boundRect[i].br(), color2, 2, 8, 0);

					double area = contourArea(contours[i]);  //  Find the area of contour
					if (area > largest_area)
					{
						largest_area = area;
						largest_contour_index = i;               //Store the index of largest contour
						bounding_rect = boundingRect(contours[i]); // Find the bounding rectangle for biggest contour
					}
				}


				//imshow("Contours", drawing);

				drawContours(drawing2, contours, largest_contour_index, Scalar(0, 255, 0), 2); // Draw the largest contour using previously stored index.

				//imshow("biggest contours", drawing2);

				center_of_rect_previous = center_of_rect;
				center_of_rect = (bounding_rect.br() + bounding_rect.tl()) * 0.5;
				circle(realtime_frame_with_biggest_contour, center_of_rect, 3, Scalar(255, 0, 0), 2);

				drawContours(realtime_frame_with_biggest_contour, contours, largest_contour_index, Scalar(0, 255, 0), 2);
				rectangle(realtime_frame_with_biggest_contour, bounding_rect.tl(), bounding_rect.br(), Scalar(0, 255, 255), 2, 8, 0);
				imshow("realtime with biggest contour", realtime_frame_with_biggest_contour);
				diff = 0;





			}

			TIME_ELAPSED = false;
		}

		if ((!DETECTED) && ((center_of_rect_previous == Point(-10, -10)) || (center_of_rect_previous == center_of_rect))) {
			printf("Initial state\n");
			DETECTED = false;
		}
		else {
			printf("intruder!!!\n");
			DETECTED = true;
		}

		imshow("background", background);

		if (waitKey(10) == 27) { // 27 : ESC�� ������ ����. 27 == ESC key
			break;
		}
	}

	waitKey();
	return 0;
}
