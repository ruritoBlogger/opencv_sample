#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include <zbar.h>

int main()
{
    // opencvのカメラの初期化
    cv::VideoCapture cap(0);

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
    
    // カメラから情報を読み込み続ける
    while(cap.read(frame))
    {
        cv::imshow("real time",frame);
	cv::cvtColor(frame,frame,CV_BGR2GRAY);

	zbar::Image image(frame.cols, frame.rows, "Y800", frame.data, frame.rows*frame.cols);
	
	int n = scanner.scan(image);
	
	  // Print results
	  for(zbar::Image::SymbolIterator symbol = image.symbol_begin(); symbol != image.symbol_end(); ++symbol)
	  {
	    std::cout << "Data : " << symbol->get_data() << std::endl;
	  }

	//auto result = ReadBarcode({frame.data, frame.rows, frame.cols, ImageFormat::RGBX}, hints);
	//std::cout << "data is " << TextUtfEncoding::ToUtf8(result.text()) << std::endl;	
        //qボタンが押されたとき処理を終了する
        const int key = cv::waitKey(1);
        if(key == 'q') break;
    }
    cv::destroyAllWindows();
}
