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

    cv::VideoCapture cap(0);

    if(!cap.isOpened())
    {
      std::cout << "cannot read camera" << std::endl;
      return -1;
    }

    cv::Mat frame;
    while(cap.read(frame))
    {
        //cv::imshow("real time",frame);

        int32_t  info[3];
        info[0] = (int32_t)frame.rows;
        info[1] = (int32_t)frame.cols;
        info[2] = (int32_t)frame.type();

        // ヘッダーの生成（height, width, type)
        for (int i = 0; i < 3; i++) {
            zmq::message_t msg((void*)&info[i], sizeof(int32_t), NULL);
            //送信（通信が確立するまで待つ）
            socket.send(msg, ZMQ_SNDMORE);
        }

         // 画像のデータをvoid型としてコピー
        void* data = malloc(frame.total() * frame.elemSize());
        memcpy(data, frame.data, frame.total() * frame.elemSize());

        // 実際にデータを送る
        zmq::message_t msg2(data, frame.total() * frame.elemSize(), my_free, NULL);
        socket.send(msg2);

        //qボタンが押されたとき処理を終了する
        const int key = cv::waitKey(1);
        if(key == 'q') break;
    }
    cv::destroyAllWindows();
}
