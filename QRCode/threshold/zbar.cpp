#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include <zbar.h>

int main()
{
    // opencvのカメラの初期化
    cv::VideoCapture cap("./test4.png");

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

    //cv::threshold(frame, frame, 75, 255, cv::THRESH_BINARY);
    cv::adaptiveThreshold(frame,
                          frame,
                          125,
                          cv::ADAPTIVE_THRESH_MEAN_C, 
                          cv::THRESH_BINARY,
                          171,
                          2);

    zbar::Image image(frame.cols, frame.rows, "Y800", frame.data, frame.cols*frame.rows);
	
	int n = scanner.scan(image);
	
	  // Print results
	  for(zbar::Image::SymbolIterator symbol = image.symbol_begin(); symbol != image.symbol_end(); ++symbol)
	  {
	    std::cout << "Data : " << symbol->get_data() << std::endl;
	  }


    cv::resize(frame, frame, cv::Size(), 500.0/frame.cols, 500.0/frame.rows);
    cv::imshow("sample",frame);
    cv::waitKey(0);
    cv::destroyAllWindows();
}
