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
    cv::VideoCapture cap(0);

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
        cv::imshow("real time",frame);
	
	auto result = ReadBarcode({frame.data, frame.cols, frame.rows, ImageFormat::BGR}, hints);
	std::cout << "data is " << TextUtfEncoding::ToUtf8(result.text()) << std::endl;	
        //qボタンが押されたとき処理を終了する
        const int key = cv::waitKey(1);
        if(key == 'q') break;
    }
    cv::destroyAllWindows();
}
