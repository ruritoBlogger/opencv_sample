#include <iostream>
#include <string>
#include <unistd.h>
#include <zbar.h>
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
    int key = 0;
    int ok_cnt = 0;

    // zbarの初期設定
    zbar::ImageScanner scanner;
    scanner.set_config(zbar::ZBAR_NONE, zbar::ZBAR_CFG_ENABLE, 0);
    scanner.set_config(zbar::ZBAR_QRCODE, zbar::ZBAR_CFG_ENABLE, 1);
    
    while(true)
    {
        // 同時に送られてくる画像のサイズの受信
        socket.recv(&rcv_msg, 0);
        num = *(int*)rcv_msg.data();

        // スレッドの終了条件
        if( num == -1 )
        {
            std::cout << "Key is " << key << std::endl;
            std::cout << "Result is " << ok_cnt << std::endl;
            break;
        }

        cv::Mat frame;
        bool decode_flag = false;

        // 同時に送られてくる画像を全て受信する
        for( int i = 0; i < num; i++ )
        {
            key++;
            std::cout << key << std::endl;
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

            if( decode_flag ) continue;

            if (type == 2) {
                frame = cv::Mat(rows, cols, CV_8UC1, data);
            }
            else {
                frame = cv::Mat(rows, cols, CV_8UC3, data);
                cv::cvtColor(frame, frame, cv::COLOR_BGR2GRAY);
            }

            if( frame.size().width > 0 && frame.size().height > 0 )
            {
                zbar::Image image(frame.cols, frame.rows, "Y800", frame.data, frame.cols*frame.rows);
               
                int n = scanner.scan(image);

                if( n > 0 )
                {
                    ok_cnt++;
                    decode_flag = true;
                }
                //else std::cout << "Data : " << std::endl;

                // Print results
                for(zbar::Image::SymbolIterator symbol = image.symbol_begin(); symbol != image.symbol_end(); ++symbol)
                {
                    std::cout << "Data : " << symbol->get_data() << std::endl;
                }
            }
        }
    }
}
