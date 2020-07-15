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
		record_frame = realtime_frame.clone(); // 원본 영상인 cam 의 영상을 복사한다.
		//printf("interval\n");
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
	Mat CannyEdge; // sub 영상에서 canny edge를 검출한 결과를 저장하기 위한 객체 선언
	double diff = 0;
	Point center_of_rect_previous = Point(-10, -10);
	Point center_of_rect = Point(-10, -10);
	Mat drawing;
	Mat drawing2;


	// 두개 비교
	//pMOG2 = createBackgroundSubtractorMOG2(500, 16, false);
	pMOG2 = createBackgroundSubtractorKNN(500, 400.0, false);


	VideoCapture cap("C:/Users/K/NaverCloud/대학/개별연구/sample/example2.mp4");

	cap.read(realtime_frame);
	//imshow("realtime_frame_Start", realtime_frame);
	record_frame = realtime_frame.clone();

	//std::thread thread_record(fun_record); // 영상의 화면을 기록하기 위한 스레드 생성

	//imshow("record_frame", record_frame);


	if (!cap.isOpened()) { std::cout << "동영상을 읽을 수 없음" << std::endl; }

	while (true) {
		cap.read(realtime_frame);
		//imshow("realtime_frame", realtime_frame);


		if (isGrayImage(realtime_frame)) { // !!!!!!!!!!!!!!!!!!! 플래그 해서 다시 또 계산 안하게 하자
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


		if (REMEMBER && diff < DIFF_THRESHOLD) {
			printf("secured\n");
			background = record_frame.clone();
			record_frame = realtime_frame.clone();
		}

		if (DETECTED || diff >= DIFF_THRESHOLD) {
			//imshow("record_frame", record_frame);
			REMEMBER = false;
			DETECTED = true;
			printf("intruder!!\n");

			pMOG2->apply(realtime_frame, result);



			erode(result, result, Mat(3, 3, CV_8U, Scalar(1)), Point(-1, -1), 2);
			dilate(result, result, Mat(3, 3, CV_8U, Scalar(1)), Point(-1, -1), 2);
			dilate(result, result, Mat(3, 3, CV_8U, Scalar(1)), Point(-1, -1), 2);
			dilate(result, result, Mat(3, 3, CV_8U, Scalar(1)), Point(-1, -1), 2);

			imshow("result", result);

			Canny(result, CannyEdge, 0, 255);
			//imshow("CannyEdge", CannyEdge);




			std::vector<std::vector<Point> > contours;
			std::vector<Vec4i> hierarchy;
			findContours(CannyEdge, contours, RETR_LIST, CHAIN_APPROX_NONE);

			drawing = Mat::zeros(CannyEdge.size(), CV_8UC3);
			drawing2 = Mat::zeros(CannyEdge.size(), CV_8UC3);
			int largest_area = 0;
			int largest_contour_index = 0;
			Rect bounding_rect;

			///////////////////////////////////////////////////////////////////////////////// bound rect를 그리기 위함
			// contour에 대한 outbound rect를 그리기 위한 변수 설정
			std::vector<std::vector<Point> > contours_poly(contours.size());
			std::vector<Rect> boundRect(contours.size());
			///////////////////////////////////////////////////////////////////////////// bound rect를 그리기 위함

			for (int i = 0; i < contours.size(); i++)
			{
				approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
				boundRect[i] = boundingRect(Mat(contours_poly[i]));

				Scalar color = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255));
				drawContours(drawing, contours, i, color, 2, 8, hierarchy, 0, Point());
				Scalar color2 = Scalar(rng.uniform(0, 255), rng.uniform(0, 255), rng.uniform(0, 255)); // rect를 그리는 색
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

			//drawContours(drawing2, contours, largest_contour_index, Scalar(0, 255, 0), 2); // Draw the largest contour using previously stored index.

			//imshow("biggest contours", drawing2);

			//center_of_rect_previous = center_of_rect;
			//center_of_rect = (bounding_rect.br() + bounding_rect.tl()) * 0.5;
			//circle(realtime_frame_with_biggest_contour, center_of_rect, 3, Scalar(255, 0, 0), 2);

			//drawContours(realtime_frame_with_biggest_contour, contours, largest_contour_index, Scalar(0, 255, 0), 2);
			//rectangle(realtime_frame_with_biggest_contour, bounding_rect.tl(), bounding_rect.br(), Scalar(0, 255, 255), 2, 8, 0);
			//imshow("realtime with biggest contour", realtime_frame_with_biggest_contour);

		}





		if (waitKey(30) >= 0) break;
	}
	cap.release();


	return 0;
}
