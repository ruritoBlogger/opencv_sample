#include <iostream>
#include <opencv2/opencv.hpp>
#include<thread>
#include<time.h>
#include<mutex>
#include <zbar.h>

cv::VideoCapture cap(0);
cv::Mat frame;
std::mutex mtx;


void GetFrameFromCameraFunction() {

    while (true) {
        std::lock_guard<std::mutex> lock(mtx);
        printf("\nRead Thread\n");
        cap.read(frame);
	cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
    }
}

int ShowFrameFromCameraFunction() {

    // zbarの初期設定
    zbar::ImageScanner scanner;
    // disable all
    scanner.set_config(zbar::ZBAR_NONE, zbar::ZBAR_CFG_ENABLE, 0);

    // enable qr
    scanner.set_config(zbar::ZBAR_QRCODE, zbar::ZBAR_CFG_ENABLE, 1);
    while (true) {
        std::lock_guard<std::mutex> lock(mtx);
        printf("\nDecode Thread\n");
	zbar::Image image(frame.cols, frame.rows, "Y800", frame.data, frame.cols*frame.rows);
	
	int n = scanner.scan(image);

	// Print results
	for(zbar::Image::SymbolIterator symbol = image.symbol_begin(); symbol != image.symbol_end(); ++symbol)
	{
	    std::cout << "Data : " << symbol->get_data() << std::endl;
	}
        cv::imshow("WIndow1", frame);
        cv::waitKey(1);
    }   
}


int main(int argh, char* argv[])
{

    std::thread GetFrameThread(GetFrameFromCameraFunction);
    std::thread ShowFrameThread(ShowFrameFromCameraFunction);

    ShowFrameThread.join();
    GetFrameThread.join();


    cv::destroyAllWindows();
    return 0;

}
