#include <iostream>
#include <string>
#include <unistd.h>
#include <chrono>
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

    zmq::message_t rcv_msg;
    int cnt;
    bool flag = false;
    int rows, cols, type;
    cv::Mat img;
    void *data;
    std::chrono::system_clock::time_point start, end;

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
        
        if( !flag )
        {
            flag = true;
            cnt = 0;
            cnt++;
            start = std::chrono::system_clock::now();
        }


        //データの受信
        socket.recv(&rcv_msg, 0);
        data = (void*)rcv_msg.data();
        //printf("rows=%d, cols=%d type=%d\n", rows, cols, type);

        if (type == 2) {
            img = cv::Mat(rows, cols, CV_8UC1, data);
        }
        else {
            img = cv::Mat(rows, cols, CV_8UC3, data);
        }
        cnt++;

        if( cnt == 500 )
        {
            end = std::chrono::system_clock::now();
            break;
        }

        //cv::imshow("receive_image", img);
        //cv::waitKey(0);
    }
    double result = std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count();
    std::cout << result << std::endl;
}
