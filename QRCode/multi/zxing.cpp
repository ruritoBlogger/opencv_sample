#include <iostream>
#include <string>
#include <chrono>
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
    cv::VideoCapture cap("./test.png");

    if(!cap.isOpened())
    {
      std::cout << "cannot read camera" << std::endl;
      return -1;
    }

    // QRCodeのDecoderの設定
    DecodeHints hints;
    hints.setTryHarder(false);

    cv::Mat frame;
    
    cap.read(frame);
    cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);

    auto result = ReadBarcode({frame.data, frame.rows, frame.cols, ImageFormat::Lum}, hints);
    std::cout << "data is " << TextUtfEncoding::ToUtf8(result.text()) << std::endl;	
    cv::destroyAllWindows();
}
