#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include <ZXing/ReadBarcode.h>
#include <ZXing/TextUtfEncoding.h>
#include <ZXing/BarcodeFormat.h>
#include <ZXing/DecodeStatus.h>
#define STB_IMAGE_IMPLEMENTATION
#include "./thirdparty/stb/stb_image.h"

using namespace ZXing;

int main()
{
    // opencvのカメラの初期化
    cv::VideoCapture cap("./test4.png");

    if(!cap.isOpened())
    {
      std::cout << "cannot read camera" << std::endl;
      return -1;
    }

    // QRCodeのDecoderの設定
    DecodeHints hints;
    //hints.setTryHarder(false);
    
    //hints.setIsPure(false);

    cv::Mat frame;
    
    cap.read(frame);
    cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);

    //cv::threshold(frame, frame, 75, 255, cv::THRESH_BINARY);
    /*
    cv::adaptiveThreshold(frame,
                          frame,
                          125,
                          cv::ADAPTIVE_THRESH_MEAN_C, 
                          cv::THRESH_BINARY,
                          171,
                          2);
    */

    auto result = ReadBarcode({frame.data, frame.rows, frame.cols, ImageFormat::BGR}, hints);
	std::cout << "data is " << TextUtfEncoding::ToUtf8(result.text()) << std::endl;	

    cv::resize(frame, frame, cv::Size(), 500.0/frame.cols, 500.0/frame.rows);
    cv::imshow("sample",frame);
    cv::waitKey(0);
    cv::destroyAllWindows();
}
