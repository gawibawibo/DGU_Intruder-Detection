/*******************************************************************
Creator : 2015112154 김석윤
********************************************************************
<Problem>
- 영상 처리를 이용한 손금 추출

<Input>
- 손바닥 영상
- 마스킹 이미지

<Output>
- 손금
********************************************************************/

#include <opencv2/opencv.hpp>
#include "opencv2/objdetect.hpp"
#include "opencv2/videoio.hpp"
#include "opencv2/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include <iostream>
#include <stdio.h>
#include <stdlib.h>

using namespace cv;
using namespace std;

/*
<function name>
Center_of_mass

<Parameter>
Mat& frame : 무게중심을 찾을 영상
int* pX : 무게중심의 x 좌표
int* pY : 무게중심의 y 좌표

<function specification>
영상의 무게중심을 계산한다

<return>
void
*/
void Center_of_mass(Mat& frame, int* pX, int* pY) {
	int cnt = 0; // 픽셀 수 카운팅
	int mx = 0;
	int my = 0;

	//면적 중심 구하기
	for (int i = 0; i < frame.size().height; i++) { // 각 축을 기준으로 픽셀들의 값을 모두 더한다
		for (int j = 0; j < frame.size().width; j++) {
			if (frame.at<uchar>(i, j) > 200) {
				cnt++;
				mx += j;
				my += i;
			}
		}
	}
	if (cnt == 0)
		cnt = 1;

	if (cnt > 0) { // 앞서 반복문에서 구한 각 축의 값을 전체 픽셀 수로 나눈다.
		*pX = mx / cnt; // 무게중심의 x축 좌표
		*pY = my / cnt; // 무게중심의 y축 좌표
	}
}

int main() {
	// 비디오 이미지를 불러온다.
	VideoCapture cam(0);

	// 원래의 오리지널 이미지
	/*
	cv::VideoCapture::set 함수
	VideoCapture의 속성을 설정한다.
	첫번째 인자 : Property identifier
	두번째 인자 : 속성의 값
	*/
	cam.set(CV_CAP_PROP_FRAME_WIDTH, 600); // CV_CAP_PROP_FRAME_WIDTH : Width of the frames in the video stream.
	cam.set(CV_CAP_PROP_FRAME_WIDTH, 400); // CV_CAP_PROP_FRAME_WIDTH : Height of the frames in the video stream.
	Mat Origin_frame; // 카메라로 들어오는 영상을 담기 위한 Mat 객체 선언. 원본이다.
	Mat HAND_MASK = imread("C:/Users/K/Desktop/Develop/Project/2020_HCI_LAB7/2020_HCI_LAB7/1.png", 1); // 마스크 이미지를 불러온다.

	if (!cam.isOpened()) { // cam을 열 수 있는가를 확인하는 if문
		printf("--(!)Error opening video cam\n");
		return -1;
	}

	while (cam.read(Origin_frame)) { // 종료 전까지 반복
		if (Origin_frame.empty()) { // cv::Mat::empty 함수. 만약 어느 element도 없으면 true를 반환한다. 즉, 카메라를 통해 들어온 영상이 없으면 if문을 실행한다.
			printf(" --(!) No camd frame -- Break!");
			break;
		}
		//imshow("original_img", Origin_frame); // cam 의 (원본)영상 출력
		//imshow("HAND_MASK", HAND_MASK); // 마스킹 이미지 출력

		// 마스크를 이용한 전처리
		// HAND_MASK의 크기를 Origin_frame의 크기와 같도록 조정한다.
		/*
		cv::resize 함수
		첫번째 인자 : source 객체
		두번째 인자 : destination 객체
		세번째 인자 : output image size
		네번째 인자 : 수평 축의 scaling factor. 0인 경우 (double)dsize.width/src.cols 와 같이 계산된다.
		다섯번째 인자 : 수직 축의 scaling factor. 0인 경우 (double)dsize.height/src.rows 와 같이 계산된다.
		여섯번째 인자 : interpolation method
		*/
		resize(HAND_MASK, HAND_MASK, Size(Origin_frame.cols, Origin_frame.rows), 0, 0, CV_INTER_LINEAR);

		/*
		원본 영상과 마스킹 이미지를 더하는 연산을 한다.
		마스크 이미지의 하얀 부분은 클램핑 효과로 인해 무조건 하얀색(255)로 나오고,
		검은 영역(0)은 영상이 더해져 나온다.
		*/
		Mat Mask = Origin_frame + HAND_MASK;
		// masking 결과 출력
		//imshow("Mask", Mask);

		Mat YCrCb; // YCrCb 컬러모델로 변환한 영상을 저장하기 위한 객체 선언
		cvtColor(Mask, YCrCb, CV_BGR2YCrCb); // Mask 영상을 BGR 컬러모델에서 YCrCb 컬러모델로 변환
		//imshow("YCrCb", YCrCb);

		Mat Skin_Area; // 피부 영역에 해당하는 영상을 저장하기 위한 객체 선언
		// cr 133~77 cb 173~127
		// cr cb 값을 조정해서 특정영역은 0, 특정 영역은 255로 만드는 과정이다.
		/*
		cv::inRange 함수
		첫번째 인자 : src 이미지
		두번째 인자 : inclusive lower boundary array or a scalar.
		세번째 인자 : inclusive upper boundary array or a scalar.
		네번째 인자 : destination 이미지
		*/
		inRange(YCrCb, Scalar(0, 130, 90), Scalar(255, 168, 130), Skin_Area);
		// 피부 영역 영상 출력. inRange 함수 연산의 결과로, 영상이 이진화 되어, 손바닥 영역은 하얀색, 이외의 영역은 검은색을 나타낸다.
		//imshow("range", Skin_Area);

		Mat Mask_gray; // Mask 영상을 그레이스케일로 변환한 결과를 저장하기 위한 객체 선언
		cvtColor(Mask, Mask_gray, CV_BGR2GRAY); // Mask 영상을 그레이스케일로 변환
		//imshow("Origin_gray", Mask_gray); // 그레이스케일로 변환된 Mask 영상 출력 
		Mat Skin_gray = Mask_gray + (~Skin_Area); // ~. Skin_Area 영상을 보수연산, 비트 반전을 하여 Mask_gray와 더한다. 따라서 손바닥 영역만 남게 된다.
		//imshow("Skin_gray", Skin_gray); // 그레이 스케일 손바닥 영역 출력

		// 손바닥 영역 추출해내기
		Mat thrsh_skin_gray; // Skin_gray 영상을 이진화 한 결과를 저장하기 위한 객체 선언
		threshold(Skin_gray, thrsh_skin_gray, 200, 255, THRESH_BINARY); // Skin_gray 영상을 이진화한다. 200~255의 값은 하얀색으로 나타나고, 이외의 값은 검은색으로 나타난다.
		//imshow("thrsh_skin_gray", thrsh_skin_gray); //이진화 한 결과 출력
		Mat tild_thrsh_skin_gray = ~thrsh_skin_gray; // 비트 반전을 하여 색을 반전시킨다.
		//imshow("tild_thrsh_skin_gray", tild_thrsh_skin_gray); // 비트 반전을 하여 색을 반전시킨 결과 출력

		// 노이즈 제거하기
		/*
		cv::erode 함수
		첫번째 인자 : input 이미지
		두번째 인자 : output 이미지
		세번째 인자 : kernel. 	structuring element used for erosion.
		네번째 인자 : position of the anchor within the element. default 값은 (-1, -1) 이고 이는 anchor가 element center 에 있음을 의미한다
		다섯번째 인자 : number of times erosion is applied.
		*/
		erode(tild_thrsh_skin_gray, tild_thrsh_skin_gray, Mat(3, 3, CV_8U, Scalar(1)), Point(-1, -1), 2);
		//imshow("tild_thrsh_skin_gray", tild_thrsh_skin_gray); // 노이즈가 제거된 영상 출력

		// 무게 중심점을 이용한 손바닥 중심점 찾기
		int cX, cY; // 무게중심 좌표 선언
		Center_of_mass(tild_thrsh_skin_gray, &cX, &cY); // Center_of_mass 함수를 통해 무게 중심 계산
		Point center(cX, cY); // 무게중심 좌표인 Point 객체 선언

		// 손바닥 중심을 이용한 가장 가까운 배경까지의 거리 찾기
		int radius = 40;
		/*
		cv::circle 함수
		첫번째 인자 : 원이 그려질 이미지
		두번째 인자 : 원의 중심. (좌표)
		세번째 인자 : 원의 반지름
		네번째 인자 : 원의 색깔
		다섯번째 인자 : 원의 굵기
		*/
		circle(Origin_frame, center, (int)(radius + 0.5), Scalar(130, 255, 100), 2);
		//imshow("Origin_frame_palm", Origin_frame); // 원이 그려진 손바닥 영상 출력

		// 손바닥 영역만 추출
		Mat Skin_gray_clone = Skin_gray.clone(); // 원이 그려진 영상을 저장하기 위한 객체 선언
		circle(Skin_gray_clone, center, (int)(radius + 0.5), Scalar(0, 0, 0), -1);
		//imshow("Skin_gray_clone", Skin_gray_clone); // 그레이 스케일 이미지에 손바닥 영역 표기

		Mat Sub = Skin_gray - Skin_gray_clone; //sub 연산을 통해 그레이 스케일 이미지 손바닥 영역 추출
		//imshow("Sub", Sub); // 그레이 스케일 이미지 손바닥 영역 출력

		// 해당 영역에 에지 디텍터를 사용해서 손금만 추출
		Mat CannyEdge; // 케니 에지를 적용한 이미지를 저장하기 위한 객체 선언
		Canny(Sub, CannyEdge, 40, 110);
		//imshow("CannyEdge", CannyEdge);
		// 필요 없는 원 영역 제거
		circle(CannyEdge, center, (int)(radius + 0.5), Scalar(0, 0, 0), 4);
		//imshow("Result", CannyEdge);

		// 27 : ESC를 누르면 종료. 27 == ESC key
		if (waitKey(10) == 27) {
			break;
		}
	}

	waitKey(); // 사용자가 키를 누를 때 까지 기다림

	return 0;
}

