/*******************************************************************
Creator : 2015112154 �輮��
********************************************************************
<Problem>
- ���� ó���� �̿��� �ձ� ����

<Input>
- �չٴ� ����
- ����ŷ �̹���

<Output>
- �ձ�
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
Mat& frame : �����߽��� ã�� ����
int* pX : �����߽��� x ��ǥ
int* pY : �����߽��� y ��ǥ

<function specification>
������ �����߽��� ����Ѵ�

<return>
void
*/
void Center_of_mass(Mat& frame, int* pX, int* pY) {
	int cnt = 0; // �ȼ� �� ī����
	int mx = 0;
	int my = 0;

	//���� �߽� ���ϱ�
	for (int i = 0; i < frame.size().height; i++) { // �� ���� �������� �ȼ����� ���� ��� ���Ѵ�
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

	if (cnt > 0) { // �ռ� �ݺ������� ���� �� ���� ���� ��ü �ȼ� ���� ������.
		*pX = mx / cnt; // �����߽��� x�� ��ǥ
		*pY = my / cnt; // �����߽��� y�� ��ǥ
	}
}

int main() {
	// ���� �̹����� �ҷ��´�.
	VideoCapture cam(0);

	// ������ �������� �̹���
	/*
	cv::VideoCapture::set �Լ�
	VideoCapture�� �Ӽ��� �����Ѵ�.
	ù��° ���� : Property identifier
	�ι�° ���� : �Ӽ��� ��
	*/
	cam.set(CV_CAP_PROP_FRAME_WIDTH, 600); // CV_CAP_PROP_FRAME_WIDTH : Width of the frames in the video stream.
	cam.set(CV_CAP_PROP_FRAME_WIDTH, 400); // CV_CAP_PROP_FRAME_WIDTH : Height of the frames in the video stream.
	Mat Origin_frame; // ī�޶�� ������ ������ ��� ���� Mat ��ü ����. �����̴�.
	Mat HAND_MASK = imread("C:/Users/K/Desktop/Develop/Project/2020_HCI_LAB7/2020_HCI_LAB7/1.png", 1); // ����ũ �̹����� �ҷ��´�.

	if (!cam.isOpened()) { // cam�� �� �� �ִ°��� Ȯ���ϴ� if��
		printf("--(!)Error opening video cam\n");
		return -1;
	}

	while (cam.read(Origin_frame)) { // ���� ������ �ݺ�
		if (Origin_frame.empty()) { // cv::Mat::empty �Լ�. ���� ��� element�� ������ true�� ��ȯ�Ѵ�. ��, ī�޶� ���� ���� ������ ������ if���� �����Ѵ�.
			printf(" --(!) No camd frame -- Break!");
			break;
		}
		//imshow("original_img", Origin_frame); // cam �� (����)���� ���
		//imshow("HAND_MASK", HAND_MASK); // ����ŷ �̹��� ���

		// ����ũ�� �̿��� ��ó��
		// HAND_MASK�� ũ�⸦ Origin_frame�� ũ��� ������ �����Ѵ�.
		/*
		cv::resize �Լ�
		ù��° ���� : source ��ü
		�ι�° ���� : destination ��ü
		����° ���� : output image size
		�׹�° ���� : ���� ���� scaling factor. 0�� ��� (double)dsize.width/src.cols �� ���� ���ȴ�.
		�ټ���° ���� : ���� ���� scaling factor. 0�� ��� (double)dsize.height/src.rows �� ���� ���ȴ�.
		������° ���� : interpolation method
		*/
		resize(HAND_MASK, HAND_MASK, Size(Origin_frame.cols, Origin_frame.rows), 0, 0, CV_INTER_LINEAR);

		/*
		���� ����� ����ŷ �̹����� ���ϴ� ������ �Ѵ�.
		����ũ �̹����� �Ͼ� �κ��� Ŭ���� ȿ���� ���� ������ �Ͼ��(255)�� ������,
		���� ����(0)�� ������ ������ ���´�.
		*/
		Mat Mask = Origin_frame + HAND_MASK;
		// masking ��� ���
		//imshow("Mask", Mask);

		Mat YCrCb; // YCrCb �÷��𵨷� ��ȯ�� ������ �����ϱ� ���� ��ü ����
		cvtColor(Mask, YCrCb, CV_BGR2YCrCb); // Mask ������ BGR �÷��𵨿��� YCrCb �÷��𵨷� ��ȯ
		//imshow("YCrCb", YCrCb);

		Mat Skin_Area; // �Ǻ� ������ �ش��ϴ� ������ �����ϱ� ���� ��ü ����
		// cr 133~77 cb 173~127
		// cr cb ���� �����ؼ� Ư�������� 0, Ư�� ������ 255�� ����� �����̴�.
		/*
		cv::inRange �Լ�
		ù��° ���� : src �̹���
		�ι�° ���� : inclusive lower boundary array or a scalar.
		����° ���� : inclusive upper boundary array or a scalar.
		�׹�° ���� : destination �̹���
		*/
		inRange(YCrCb, Scalar(0, 130, 90), Scalar(255, 168, 130), Skin_Area);
		// �Ǻ� ���� ���� ���. inRange �Լ� ������ �����, ������ ����ȭ �Ǿ�, �չٴ� ������ �Ͼ��, �̿��� ������ �������� ��Ÿ����.
		//imshow("range", Skin_Area);

		Mat Mask_gray; // Mask ������ �׷��̽����Ϸ� ��ȯ�� ����� �����ϱ� ���� ��ü ����
		cvtColor(Mask, Mask_gray, CV_BGR2GRAY); // Mask ������ �׷��̽����Ϸ� ��ȯ
		//imshow("Origin_gray", Mask_gray); // �׷��̽����Ϸ� ��ȯ�� Mask ���� ��� 
		Mat Skin_gray = Mask_gray + (~Skin_Area); // ~. Skin_Area ������ ��������, ��Ʈ ������ �Ͽ� Mask_gray�� ���Ѵ�. ���� �չٴ� ������ ���� �ȴ�.
		//imshow("Skin_gray", Skin_gray); // �׷��� ������ �չٴ� ���� ���

		// �չٴ� ���� �����س���
		Mat thrsh_skin_gray; // Skin_gray ������ ����ȭ �� ����� �����ϱ� ���� ��ü ����
		threshold(Skin_gray, thrsh_skin_gray, 200, 255, THRESH_BINARY); // Skin_gray ������ ����ȭ�Ѵ�. 200~255�� ���� �Ͼ������ ��Ÿ����, �̿��� ���� ���������� ��Ÿ����.
		//imshow("thrsh_skin_gray", thrsh_skin_gray); //����ȭ �� ��� ���
		Mat tild_thrsh_skin_gray = ~thrsh_skin_gray; // ��Ʈ ������ �Ͽ� ���� ������Ų��.
		//imshow("tild_thrsh_skin_gray", tild_thrsh_skin_gray); // ��Ʈ ������ �Ͽ� ���� ������Ų ��� ���

		// ������ �����ϱ�
		/*
		cv::erode �Լ�
		ù��° ���� : input �̹���
		�ι�° ���� : output �̹���
		����° ���� : kernel. 	structuring element used for erosion.
		�׹�° ���� : position of the anchor within the element. default ���� (-1, -1) �̰� �̴� anchor�� element center �� ������ �ǹ��Ѵ�
		�ټ���° ���� : number of times erosion is applied.
		*/
		erode(tild_thrsh_skin_gray, tild_thrsh_skin_gray, Mat(3, 3, CV_8U, Scalar(1)), Point(-1, -1), 2);
		//imshow("tild_thrsh_skin_gray", tild_thrsh_skin_gray); // ����� ���ŵ� ���� ���

		// ���� �߽����� �̿��� �չٴ� �߽��� ã��
		int cX, cY; // �����߽� ��ǥ ����
		Center_of_mass(tild_thrsh_skin_gray, &cX, &cY); // Center_of_mass �Լ��� ���� ���� �߽� ���
		Point center(cX, cY); // �����߽� ��ǥ�� Point ��ü ����

		// �չٴ� �߽��� �̿��� ���� ����� �������� �Ÿ� ã��
		int radius = 40;
		/*
		cv::circle �Լ�
		ù��° ���� : ���� �׷��� �̹���
		�ι�° ���� : ���� �߽�. (��ǥ)
		����° ���� : ���� ������
		�׹�° ���� : ���� ����
		�ټ���° ���� : ���� ����
		*/
		circle(Origin_frame, center, (int)(radius + 0.5), Scalar(130, 255, 100), 2);
		//imshow("Origin_frame_palm", Origin_frame); // ���� �׷��� �չٴ� ���� ���

		// �չٴ� ������ ����
		Mat Skin_gray_clone = Skin_gray.clone(); // ���� �׷��� ������ �����ϱ� ���� ��ü ����
		circle(Skin_gray_clone, center, (int)(radius + 0.5), Scalar(0, 0, 0), -1);
		//imshow("Skin_gray_clone", Skin_gray_clone); // �׷��� ������ �̹����� �չٴ� ���� ǥ��

		Mat Sub = Skin_gray - Skin_gray_clone; //sub ������ ���� �׷��� ������ �̹��� �չٴ� ���� ����
		//imshow("Sub", Sub); // �׷��� ������ �̹��� �չٴ� ���� ���

		// �ش� ������ ���� �����͸� ����ؼ� �ձݸ� ����
		Mat CannyEdge; // �ɴ� ������ ������ �̹����� �����ϱ� ���� ��ü ����
		Canny(Sub, CannyEdge, 40, 110);
		//imshow("CannyEdge", CannyEdge);
		// �ʿ� ���� �� ���� ����
		circle(CannyEdge, center, (int)(radius + 0.5), Scalar(0, 0, 0), 4);
		//imshow("Result", CannyEdge);

		// 27 : ESC�� ������ ����. 27 == ESC key
		if (waitKey(10) == 27) {
			break;
		}
	}

	waitKey(); // ����ڰ� Ű�� ���� �� ���� ��ٸ�

	return 0;
}

