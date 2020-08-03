#include <iostream>
#include <string>
#include <unistd.h>
#include <zmq.hpp>
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
  
    char dir[255];
    getcwd(dir,255);
    std::string path = dir;
    cv::Mat image = cv::imread(path + "/" + "lena.png", cv::IMREAD_COLOR);

    int32_t  info[3];
    info[0] = (int32_t)image.rows;
    info[1] = (int32_t)image.cols;
    info[2] = (int32_t)image.type();

    // ヘッダーの生成（height, width, type)
    for (int i = 0; i < 3; i++) {
      zmq::message_t msg((void*)&info[i], sizeof(int32_t), NULL);
      //送信（通信が確立するまで待つ）
      socket.send(msg, ZMQ_SNDMORE);
    }

    // 画像のデータをvoid型としてコピー
    void* data = malloc(image.total() * image.elemSize());
    memcpy(data, image.data, image.total() * image.elemSize());

    // 実際にデータを送る
    zmq::message_t msg2(data, image.total() * image.elemSize(), my_free, NULL);
    socket.send(msg2);
    std::cout << "message sended" << std::endl;
}
