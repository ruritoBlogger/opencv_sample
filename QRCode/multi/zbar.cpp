#include <iostream>
#include <string>
#include <chrono>
#include <opencv2/opencv.hpp>
#include <zbar.h>

int main()
{
    // opencvのカメラの初期化
    cv::VideoCapture cap("./test.png");

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

    int n = scanner.scan(image);

    // Print results
    for(zbar::Image::SymbolIterator symbol = image.symbol_begin(); symbol != image.symbol_end(); ++symbol)
    {
        std::cout << "Data : " << symbol->get_data() << std::endl;
    }
    cv::destroyAllWindows();
}
