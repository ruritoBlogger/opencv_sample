#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>
#include <zbar.h>
#include "zmq_addon.hpp"

void my_free(void *data, void *hint)
{
        free(data);
}

int main()
{
    // zbarの初期設定
    zbar::ImageScanner scanner;
    // disable all
    scanner.set_config(zbar::ZBAR_NONE, zbar::ZBAR_CFG_ENABLE, 0);

    // enable qr
    scanner.set_config(zbar::ZBAR_QRCODE, zbar::ZBAR_CFG_ENABLE, 1);

    zmq::context_t ctx;
    zmq::socket_t socket(ctx, zmq::socket_type::pull);
    socket.bind("tcp://*:11000");

    zmq::message_t rcv_msg;
    int rows, cols, type;
    cv::Mat img;
    void *data;

    while(true)
    {
        //heightの受信
        socket.recv(&rcv_msg, 0);
        rows = *(int*)rcv_msg.data();

        //widthの受信
        socket.recv(&rcv_msg, 0);
        cols = *(int*)rcv_msg.data();

        //chの受信
        socket.recv(&rcv_msg, 0);
        type = *(int*)rcv_msg.data();
        
        //データの受信
        socket.recv(&rcv_msg, 0);
        data = (void*)rcv_msg.data();
        printf("rows=%d, cols=%d type=%d\n", rows, cols, type);

        if (type == 2) {
            img = cv::Mat(rows, cols, CV_8UC1, data);
        }
        else {
            img = cv::Mat(rows, cols, CV_8UC3, data);
        }
	    cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);
	    zbar::Image image(img.cols, img.rows, "Y800", img.data, img.cols*img.rows);
	
	    int n = scanner.scan(image);
	
	    // Print results
	    for(zbar::Image::SymbolIterator symbol = image.symbol_begin(); symbol != image.symbol_end(); ++symbol)
	    {
	        std::cout << "Data : " << symbol->get_data() << std::endl;
	    }
    }
    cv::destroyAllWindows();
}
