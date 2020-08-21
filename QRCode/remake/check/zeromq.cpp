#include <iostream>
#include <string>
#include <unistd.h>
#include <thread>
#include <zbar.h>
#include <mutex>
#include "zmq_addon.hpp"
#include <opencv2/opencv.hpp>

void my_free(void *data, void *hint)
{
        free(data);
}

int main()
{
    zmq::context_t ctx;
    zmq::socket_t socket(ctx, zmq::socket_type::pull);
    socket.bind("tcp://*:11000");

    cv::Mat img;
    zmq::message_t rcv_msg;
    int num, rows, cols, type;
    void *data;

    zbar::ImageScanner scanner;
    scanner.set_config(zbar::ZBAR_NONE, zbar::ZBAR_CFG_ENABLE, 0);
    scanner.set_config(zbar::ZBAR_QRCODE, zbar::ZBAR_CFG_ENABLE, 1);
    int cnt = 0; 
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

        if (type == 2) {
            img = cv::Mat(rows, cols, CV_8UC1, data);
        //cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);
        }
        else {
            img = cv::Mat(rows, cols, CV_8UC3, data);
            cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);
        }

        //printf("rows=%d, cols=%d type=%d\n", rows, cols, type);
        zbar::Image image(img.cols, img.rows, "Y800", img.data, img.cols*img.rows);
        int n = scanner.scan(image);

        for(zbar::Image::SymbolIterator symbol = image.symbol_begin(); symbol != image.symbol_end(); ++symbol)
        {
            std::cout << "Data : " << symbol->get_data() << std::endl;
        }
        if( n != 0 ) cnt++;
    }
    std::cout << cnt << std::endl;
}
