#include <iostream>
#include <string>
#include <thread>
#include <opencv2/opencv.hpp>
#include "queue.cpp"
#include <zbar.h>

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
    // zbarの初期設定
    zbar::ImageScanner scanner;
    // disable all
    scanner.set_config(zbar::ZBAR_NONE, zbar::ZBAR_CFG_ENABLE, 0);

    // enable qr
    scanner.set_config(zbar::ZBAR_QRCODE, zbar::ZBAR_CFG_ENABLE, 1);
    while(true)
    {
	if( !que.empty() )
	{
	    frame = *que.pop().get();
	    cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
	    zbar::Image image(frame.cols, frame.rows, "Y800", frame.data, frame.cols*frame.rows);
	
	    int n = scanner.scan(image);

	    // Print results
	    for(zbar::Image::SymbolIterator symbol = image.symbol_begin(); symbol != image.symbol_end(); ++symbol)
	    {
	        std::cout << "Data : " << symbol->get_data() << std::endl;
	    }
	    cv::imshow("show img", frame);
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
