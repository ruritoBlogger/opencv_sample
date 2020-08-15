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

void receiver(safe_queue<std::string> &que)
{
    zmq::context_t ctx;
    zmq::socket_t socket(ctx, zmq::socket_type::pull);
    socket.bind("tcp://*:11000");

    zmq::message_t rcv_msg;
    int rows, cols, type;
    void *data;
    std::string number;
    
    while(true)
    {
        socket.recv(&rcv_msg, 0);
        number = std::string(static_cast<char*>(rcv_msg.data()), rcv_msg.size());
	que.push(number);
    }
}

void show(safe_queue<std::string> &que)
{
    while(true)
    {
	if( !que.empty() ) std::cout << *que.pop().get() << std::endl;
    }
}

int main()
{
    safe_queue<std::string> que;
    std::thread t1(receiver, std::ref(que));
    std::thread t2(show, std::ref(que));

    t1.join();
    t2.join();
}
