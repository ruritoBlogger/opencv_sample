#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include <queue>

int main()
{
    cv::VideoCapture cap(0);

    if(!cap.isOpened())
    {
	std::cout << "cannot read camera" << std::endl;
    }

    cv::Mat frame;
    std::queue<cv::Mat> que;
    while(cap.read(frame))
    {
	que.push(frame);
	if( que.size() > 100 ) break;
    }
    while(!que.empty())
    {
	cv::imshow("show img", que.front());
	cv::waitKey(10);
	que.pop();
	std::cout << que.size() << std::endl;
    }
    cv::destroyAllWindows();
}
