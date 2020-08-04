#include <iostream>
#include <string>
#include <unistd.h>
#include "zmq_addon.hpp"
#include <opencv2/opencv.hpp>

void my_free(void *data, void *hint)
{
        free(data);
}

int main()
{
    zmq::context_t ctx;
    zmq::socket_t socket(ctx, zmq::socket_type::push);
    socket.connect("tcp://localhost:11000");

    //cv::VideoCapture cap("/home/rurito/mysketch/opencv_sample/RealTime/video.mp4");
    cv::VideoCapture cap(0);

    if(!cap.isOpened())
    {
      std::cout << "cannot read camera" << std::endl;
      return -1;
    }

    cv::Mat frame;
    while(cap.read(frame))
    {
        cv::imshow("real time",frame);

        //qボタンが押されたとき処理を終了する
        const int key = cv::waitKey(1);
        if(key == 'q') break;
    }
    cv::destroyAllWindows();
}
