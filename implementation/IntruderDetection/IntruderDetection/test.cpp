#include "opencv2/opencv.hpp"  
#include <iostream>  

using namespace cv;
using namespace std;



int main(int, char**)
{
    //동영상 파일로부터 부터 데이터 읽어오기 위해 준비  
    VideoCapture cap1("example1_edit2.mp4");
    if (!cap1.isOpened())
    {
        printf("동영상 파일을 열수 없습니다. \n");
    }

    //동영상 플레이시 크기를  320x240으로 지정  
    cap1.set(CAP_PROP_FRAME_WIDTH, 320);
    cap1.set(CAP_PROP_FRAME_HEIGHT, 240);


    Mat frame1;
    namedWindow("video", 1);



    for (;;)
    {

        //웹캡으로부터 한 프레임을 읽어옴  
        cap1 >> frame1;

        imshow("video", frame1);


        //30ms 정도 대기하도록 해야 동영상이 너무 빨리 재생되지 않음.  
        if (waitKey(20) == 27) break; //ESC키 누르면 종료  
    }


    return 0;
}