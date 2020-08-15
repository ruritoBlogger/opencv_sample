#include <iostream>
#include <string>
#include <zbar.h>
#include <opencv2/opencv.hpp>

int main()
{
    cv::VideoCapture cap(0);

    if(!cap.isOpened())
    {
	std::cout << "cannot read camera" << std::endl;
    }

    // zbarの初期設定
    zbar::ImageScanner scanner;
    // disable all
    scanner.set_config(zbar::ZBAR_NONE, zbar::ZBAR_CFG_ENABLE, 0);

    // enable qr
    scanner.set_config(zbar::ZBAR_QRCODE, zbar::ZBAR_CFG_ENABLE, 1);
    cv::Mat frame;
    while(cap.read(frame))
    {
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
    cv::destroyAllWindows();
}
