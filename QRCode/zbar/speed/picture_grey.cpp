#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include <zbar.h>
#include <chrono>

int main()
{
    std::chrono::system_clock::time_point start, end;
    start = std::chrono::system_clock::now();
    int key = 0;
    while(true)
    {
        // opencvのカメラの初期化
        cv::VideoCapture cap("./qr.png");

        if(!cap.isOpened())
        {
          std::cout << "cannot read camera" << std::endl;
          return -1;
        }

        zbar::ImageScanner scanner;
        // disable all
        scanner.set_config(zbar::ZBAR_NONE, zbar::ZBAR_CFG_ENABLE, 0);

        // enable qr
        scanner.set_config(zbar::ZBAR_QRCODE, zbar::ZBAR_CFG_ENABLE, 1);
        cv::Mat frame;
        cap.read(frame);
        cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
        zbar::Image image(frame.cols, frame.rows, "Y800", frame.data, frame.cols*frame.rows);
        int n; 

        if( key > 1000 ) break;
	    n = scanner.scan(image);
        key++;
    }
    end = std::chrono::system_clock::now();
    double result = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << result << std::endl;
}
