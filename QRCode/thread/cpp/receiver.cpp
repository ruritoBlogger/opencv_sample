#include <iostream>
#include <string>
#include <unistd.h>
#include <thread>
#include "zmq_addon.hpp"
#include <opencv2/opencv.hpp>
#include "queue.cpp"

void my_free(void *data, void *hint)
{
        free(data);
}

void receive_image(safe_queue<cv::Mat*> &que)
{
    zmq::context_t ctx;
    zmq::socket_t socket(ctx, zmq::socket_type::pull);
    socket.bind("tcp://*:11000");

    cv::Mat img;
    zmq::message_t rcv_msg;
    int rows, cols, type;
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

        if (type == 2) {
            img = cv::Mat(rows, cols, CV_8UC1, data);
        }
        else {
            img = cv::Mat(rows, cols, CV_8UC3, data);
        }
	que.push(&img);
    }
}

void show_image(safe_queue<cv::Mat*> &que)
{
    cv::Mat* frame;
    while(true){
	if( !que.empty() )
	{
	    frame = *que.pop().get();
	    if( frame->size().width > 0 && frame->size().height > 0 )
	    {
            	cv::imshow("test", *frame);
            	cv::waitKey(1);
	    }
	}
    }
    cv::destroyAllWindows();
}

int main()
{
    safe_queue<cv::Mat*> que;
    std::thread receiver(receive_image, std::ref(que));
    std::thread shower(show_image, std::ref(que));

    receiver.join();
    shower.join();
}
