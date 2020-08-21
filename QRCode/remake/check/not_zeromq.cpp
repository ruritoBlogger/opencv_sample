#include <iostream>
#include <string>
#include <unistd.h>
#include <opencv2/opencv.hpp>
#include <zbar.h>

void my_free(void *data, void *hint)
{
        free(data);
}

int main()
{
    cv::VideoCapture cap("./qr_fps60.mp4");
    if(!cap.isOpened())
    {
      std::cout << "cannot read camera" << std::endl;
      return -1;
    }
    int key = 0;

    cv::Mat frame;
    while(cap.read(frame))
    {
        cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
        // zbarの初期設定
        zbar::ImageScanner scanner;
        scanner.set_config(zbar::ZBAR_NONE, zbar::ZBAR_CFG_ENABLE, 0);
        scanner.set_config(zbar::ZBAR_QRCODE, zbar::ZBAR_CFG_ENABLE, 1);

        zbar::Image image(frame.cols, frame.rows, "Y800", frame.data, frame.cols*frame.rows);
        int n = scanner.scan(image);

        //if( n == 0 ) std::cout << "Data : " << std::endl;
        if( n != 0 ) key ++;

        // Print results
        for(zbar::Image::SymbolIterator symbol = image.symbol_begin(); symbol != image.symbol_end(); ++symbol)
        {
            std::cout << "Data : " << symbol->get_data() << std::endl;
        }
        /*
        cv::imshow("test", frame);
        cv::waitKey(1);
        */
    }
    cv::destroyAllWindows();
    std::cout << key << std::endl;
}
