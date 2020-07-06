#include <opencv2/opencv.hpp>

using namespace cv;

int main()
{
	Mat image = Mat(Size(300, 400), CV_8UC1, Scalar(255));
	image(Rect(0, 0, 300, 200)) = Scalar(1);

	putText(image, "HELLO", Point(50, 100), FONT_HERSHEY_PLAIN, 2, Scalar(255, 255, 255));
	putText(image, "WORLD", Point(150, 300), FONT_HERSHEY_PLAIN, 2, Scalar(0, 0, 0));

	std::cout << "HELLO WORLD" << std::endl;
	imshow("image", image);
	Mat image2 = image.clone();

	image2.release();
	if (image2.empty()) {
		std::cout << "empty!!" << std::endl;
		image2 = image.clone();
	}
	imshow("image again", image); imshow("image2", image2);

	image = Scalar(0);
	imshow("image scalra 0", image);



	waitKey();
	return 0;
}
