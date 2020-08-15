#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>

int main()
{
    cv::VideoCapture cap(0);

    if(!cap.isOpened())
    {
	std::cout << "cannot read camera" << std::endl;
    }

    cv::Mat frame;
    while(cap.read(frame))
    {
	cv::imshow("show img", frame);
	cv::waitKey(1);
    }
    cv::destroyAllWindows();
}
