#include <iostream>
#include <string>
#include <unistd.h>
#include <thread>
#include<mutex>
#include "zmq_addon.hpp"
#include <opencv2/opencv.hpp>
#include "queue.cpp"

std::string number;
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
        socket.recv(&rcv_msg, 0);
        number = std::string(static_cast<char*>(rcv_msg.data()), rcv_msg.size());
	//std::cout << number << std::endl;	
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
        std::lock_guard<std::mutex> lock(mtx);
	std::cout << number << std::endl;
    }
}

int main()
{
    std::thread t1(receiver);
    std::thread t2(show);

    t1.join();
    t2.join();
}
