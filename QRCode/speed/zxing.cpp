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

int loop()
{
    // opencvのカメラの初期化
    cv::VideoCapture cap("./qr.png");

    if(!cap.isOpened())
    {
      std::cout << "cannot read camera" << std::endl;
      return -1;
    }

    // QRCodeのDecoderの設定
    DecodeHints hints;
    hints.setTryHarder(false);

    cv::Mat frame;
    
    // カメラから情報を読み込み続ける
    while(cap.read(frame))
    {
	cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
        cv::imshow("real time",frame);
	
	auto result = ReadBarcode({frame.data, frame.rows, frame.cols, ImageFormat::Lum}, hints);
	//std::cout << "data is " << TextUtfEncoding::ToUtf8(result.text()) << std::endl;	
        //qボタンが押されたとき処理を終了する
        const int key = cv::waitKey(1);
        if(key == 'q') break;
    }
    cv::destroyAllWindows();
}

int main()
{
    std::chrono::system_clock::time_point start, end;
    
    // 計測開始 
    start = std::chrono::system_clock::now();
    for( int i = 0; i < 10000; i++ )
    {
        int n = loop();
    }
    end = std::chrono::system_clock::now();
    double result = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << result << std::endl;   
}
