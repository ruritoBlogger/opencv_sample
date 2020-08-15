#include <iostream>
#include <opencv2/opencv.hpp>
#include<thread>
#include<time.h>
#include<mutex>

cv::VideoCapture cap(0);
cv::Mat frame;
std::mutex mtx;


void GetFrameFromCameraFunction() {

    while (true) {
        std::lock_guard<std::mutex> lock(mtx);
        printf("\nRead Thread\n");
        cap.read(frame);
    }
}

int ShowFrameFromCameraFunction() {

    while (true) {
        std::lock_guard<std::mutex> lock(mtx);
        printf("\nShow Thread\n");
        cv::imshow("WIndow1", frame);
        const int key = cv::waitKey(1);
        if (key == 27) {

            printf("\n Close Window !!\n");
            return 1;
        }
        else if (key == 's') {

            cv::imwrite("th2.png", frame);

        }
    }   
}


int main(int argh, char* argv[])
{

    std::thread GetFrameThread(GetFrameFromCameraFunction);
    std::thread ShowFrameThread(ShowFrameFromCameraFunction);

    ShowFrameThread.join();
    GetFrameThread.join();


    cv::destroyAllWindows();
    return 0;

}
