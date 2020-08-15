#include <iostream>
#include <string>
#include <unistd.h>
#include <thread>
#include<mutex>
#include "zmq_addon.hpp"
#include <opencv2/opencv.hpp>
#include "queue.cpp"

cv::Mat frame;
std::mutex mtx;

void my_free(void *data, void *hint)
{
        free(data);
}

void receiver()
{
    zmq::context_t ctx;
    zmq::socket_t socket(ctx, zmq::socket_type::pull);
    socket.bind("tcp://*:11000");

    safe_queue<cv::Mat> que;
    cv::Mat img;
    zmq::message_t rcv_msg;
    int rows, cols, type;
    void *data;
    
    while(true)
    {
        std::lock_guard<std::mutex> lock(mtx);
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
	frame = img;	
        /*	
        printf("rows=%d, cols=%d type=%d\n", rows, cols, type);
      	cv::imshow("sample", img);
	cv::waitKey(1);	
	que.push(img);
        if( que.size() > 0 )
        {
            img = *que.pop().get();
            cv::imshow("sample",img);
            cv::waitKey(1);
        }
	*/
    }
}

void show()
{
    while(true)
    {
	std::cout << frame.size().width << std::endl;
        std::lock_guard<std::mutex> lock(mtx);
	if( frame.size().width > 0 && frame.size().height > 0 )
	{
	    cv::imshow("sample", frame);
	    cv::waitKey(1);
	}
    }
}

int main()
{
    std::thread t1(receiver);
    //std::thread t2(show);

    t1.join();
    //t2.join();
}
