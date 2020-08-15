#include <iostream>
#include <string>
#include <thread>
#include <opencv2/opencv.hpp>
#include "queue.cpp"

void add_que(safe_queue<cv::Mat> &que)
{
    cv::VideoCapture cap(0);
    cv::Mat frame;
    if(!cap.isOpened())
    {
	std::cout << "cannot read camera" << std::endl;
    }

    while(true)
    {
	while(cap.read(frame))
	{
	    que.push(frame);
	}
    }
}

void pop_que(safe_queue<cv::Mat> &que)
{
    cv::Mat frame;
    while(true)
    {
	if( !que.empty() )
	{
	    cv::imshow("show img", *que.pop().get());
	    cv::waitKey(1);
	}
    }
}

int main()
{
    safe_queue<cv::Mat> que;

    std::thread t1(add_que, std::ref(que));
    std::thread t2(pop_que, std::ref(que));

    t1.join();
    t2.join();
}
