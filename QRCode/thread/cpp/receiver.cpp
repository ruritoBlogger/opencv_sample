#include <iostream>
#include <string>
#include <unistd.h>
#include <thread>
#include <mutex>
#include "zmq_addon.hpp"
#include <opencv2/opencv.hpp>
#include "queue.cpp"

cv::Mat frame;
std::mutex mtx;

void my_free(void *data, void *hint)
{
        free(data);
}

void receive_image()
{
    zmq::context_t ctx;
    zmq::socket_t socket(ctx, zmq::socket_type::pull);
    socket.bind("tcp://*:11000");

    cv::Mat img;
    zmq::message_t rcv_msg;
    int rows, cols, type;
    void *data;
    std::vector<cv::Mat*> ary_ptr;
    
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
        }
        else {
            img = cv::Mat(rows, cols, CV_8UC3, data);
        }
        //printf("rows=%d, cols=%d type=%d\n", rows, cols, type);
        if( ary_ptr.size() < 3 )
        {
            ary_ptr.push_back(&img);
            std::lock_guard<std::mutex> lock(mtx);
            //frame = *ary_ptr[0];
            frame = img;
            ary_ptr.erase(ary_ptr.begin());
        }
    }
}

void show_image()
{
    cv::Mat img;
    while(true){
        std::lock_guard<std::mutex> lock(mtx);
        cv::imshow("test", frame);
        cv::waitKey(1);
    }
    cv::destroyAllWindows();
}

int main()
{
    std::thread receiver(receive_image);
    std::thread shower(show_image);

    receiver.join();
    shower.join();
}
