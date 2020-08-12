#include <iostream>
#include <string>
#include "zmq_addon.hpp"
#include <opencv2/opencv.hpp>
#include <ZXing/ReadBarcode.h>
#include <ZXing/TextUtfEncoding.h>
#include <ZXing/BarcodeFormat.h>
#include <ZXing/DecodeStatus.h>
#define STB_IMAGE_IMPLEMENTATION
#include "./thirdparty/stb/stb_image.h"

using namespace ZXing;

void my_free(void *data, void *hint)
{
        free(data);
}

int main()
{
    // QRCodeのDecoderの設定
    DecodeHints hints;
    hints.setTryHarder(false);

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
	    auto result = ReadBarcode({img.data, img.cols, img.rows, ImageFormat::Lum}, hints);
	    std::cout << "data is " << TextUtfEncoding::ToUtf8(result.text()) << std::endl;
    }
    cv::destroyAllWindows();
}
