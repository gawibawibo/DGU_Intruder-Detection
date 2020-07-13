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

bool TIME_ELAPSED; // 일정 시간이 지났는지 확인하는 bool 변수.
RNG rng(12345);
#define DIFF_THRESHOLD 0.03
bool REMEMBER = true;
bool DETECTED = false;


// 영상을 기록하기 위한 함수
void fun_record(Mat* realtime, Mat* record) {
	while (1) {
		*record = (*realtime).clone(); // 원본 영상인 cam 의 영상을 복사한다.
		GaussianBlur(*record, *record, Size(3, 3), 0, 0);// gray_realtime_frame 을 블러처리
		//std::cout << "time interval" << std::endl;

		std::this_thread::sleep_for(std::chrono::milliseconds(100)); // N밀리초 라는 time interval을 둔다.
		TIME_ELAPSED = true;
		std::this_thread::sleep_for(std::chrono::milliseconds(100)); // N밀리초 라는 time interval을 둔다
	}
}

int main()
{
	VideoCapture cam(0); // 비디오 이미지를 불러온다.

	cam.set(CV_CAP_PROP_FRAME_WIDTH, 600); // CV_CAP_PROP_FRAME_WIDTH : Width of the frames in the video stream.
	cam.set(CV_CAP_PROP_FRAME_WIDTH, 400); // CV_CAP_PROP_FRAME_WIDTH : Height of the frames in the video stream.
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

	std::thread thread_record(fun_record, &realtime_frame, &record_frame); // 영상의 화면을 기록하기 위한 스레드 생성

	while (cam.read(realtime_frame)) { // 종료 전까지 반복
		if (realtime_frame.empty()) { // cv::Mat::empty 함수. 만약 어느 element도 없으면 true를 반환한다. 즉, 카메라를 통해 들어온 영상이 없으면 if문을 실행한다.
			printf(" --(!) No cam frame -- Break!");
			break;
		}

		resize(record_frame, record_frame, Size(realtime_frame.cols, realtime_frame.rows), 0, 0, CV_INTER_LINEAR);
		realtime_frame_with_biggest_contour = realtime_frame.clone();

		Mat gray_realtime_frame; // realtime_frame 영상을 그레이 스케일로 저장하기 위한 객체
		cvtColor(realtime_frame, gray_realtime_frame, COLOR_BGR2GRAY); // 영상을 그레이스케일로 변환
		GaussianBlur(gray_realtime_frame, gray_realtime_frame, Size(3, 3), 0, 0);// gray_realtime_frame 을 블러처리


		Mat thrsh_realtime_frame; // 실시간 영상을 이진화 영상으로 저장하기 위한 Mat 객체 선언
		//threshold(gray_realtime_frame, thrsh_realtime_frame, 0, 255, THRESH_BINARY | THRESH_OTSU); // 영상을 이진화한다.
		adaptiveThreshold(gray_realtime_frame, thrsh_realtime_frame, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 5, 10);

		//imshow("thrsh_realtime_frame", thrsh_realtime_frame);

		Mat	thrsh_record_frame; // 실시간 영상과 기록된 영상을 이진화하여 저장하기 위한 객체 선언

		if (REMEMBER && (diff < DIFF_THRESHOLD)) {
			printf("secured\n");
			background = thrsh_realtime_frame.clone();
			REMEMBER = false;
		}

		if (TIME_ELAPSED) { // 기록 영상이 있는지 확인(=time interval이 지났는지 확인)

			Mat gray_record_frame; // record_frame 영상을 그레이 스케일로 저장하기 위한 객체
			cvtColor(record_frame, gray_record_frame, COLOR_BGR2GRAY); // 영상을 그레이스케일로 변환

			// 정규화를 통해 이전 프레임과 현재 프레임의 차이의 정도를 구함
			double errorL2 = cv::norm(gray_realtime_frame, gray_record_frame, CV_L2);
			diff = errorL2 / (double)(gray_realtime_frame.rows * gray_record_frame.rows);



			if ((DETECTED || (diff >= DIFF_THRESHOLD))) { // 침입자가 발생했다고 판단한 경우

				//threshold(gray_record_frame, thrsh_record_frame, 0, 255, THRESH_BINARY | THRESH_OTSU); // 영상을 이진화한다. 
				adaptiveThreshold(gray_record_frame, thrsh_record_frame, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, 5, 10);
				//erode(thrsh_record_frame, thrsh_record_frame, Mat(3, 3, CV_8U, Scalar(1)), Point(-1, -1), 2);
				morphologyEx(thrsh_record_frame, thrsh_record_frame, MORPH_OPEN, Mat(3, 3, CV_8U, Scalar(1)), Point(-1, -1), 2);
				imshow("thrsh_record_frame", thrsh_record_frame);



				//erode(thrsh_realtime_frame, thrsh_realtime_frame, Mat(3, 3, CV_8U, Scalar(1)), Point(-1, -1), 2);
				morphologyEx(thrsh_realtime_frame, thrsh_realtime_frame, MORPH_OPEN, Mat(3, 3, CV_8U, Scalar(1)), Point(-1, -1), 2);
				morphologyEx(background, background, MORPH_OPEN, Mat(3, 3, CV_8U, Scalar(1)), Point(-1, -1), 2);

				imshow("thrsh_realtime_frame", thrsh_realtime_frame);



				sub = thrsh_realtime_frame - background; // 실시간 화면에서 record 영상을 뺀다.
				imshow("sub", sub); // sub 출력
				//GaussianBlur(sub, sub, Size(3, 3), 0, 0);// gray_realtime_frame 을 블러처리
				//imshow("edited sub", sub); // sub 출력

				Mat opening_sub;
				morphologyEx(sub, opening_sub, MORPH_OPEN, Mat(3, 3, CV_8U, Scalar(1)), Point(-1, -1), 2);
				imshow("opening_sub", opening_sub); // opening_sub 출력

				Mat CannyEdge; // sub 영상에서 canny edge를 검출한 결과를 저장하기 위한 객체 선언
				Canny(sub, CannyEdge, 0, 255);
				imshow("CannyEdge", CannyEdge); // sub 출력

				std::vector<std::vector<Point> > contours;
				std::vector<Vec4i> hierarchy;
				findContours(CannyEdge, contours, RETR_LIST, CHAIN_APPROX_NONE);

				Mat drawing = Mat::zeros(CannyEdge.size(), CV_8UC3);
				Mat drawing2 = Mat::zeros(CannyEdge.size(), CV_8UC3);
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

		if (waitKey(10) == 27) { // 27 : ESC를 누르면 종료. 27 == ESC key
			break;
		}
	}

	waitKey();
	return 0;
}
