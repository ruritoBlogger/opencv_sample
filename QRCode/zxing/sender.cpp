#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>

int main()
{
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
        void* data = malloc(frame.total() * frame.elemSize());


        //qボタンが押されたとき処理を終了する
        const int key = cv::waitKey(1);
        if(key == 'q') break;
    }
    cv::destroyAllWindows();
}
