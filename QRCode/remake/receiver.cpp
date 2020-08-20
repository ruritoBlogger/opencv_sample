#include <iostream>
#include <string>
#include <unistd.h>
#include <thread>
#include <zbar.h>
#include <mutex>
#include "zmq_addon.hpp"
#include <opencv2/opencv.hpp>
#include "queue.cpp"

void my_free(void *data, void *hint)
{
        free(data);
}

void receiver(safe_queue<safe_queue<cv::Mat*> > &ary, bool &flag, std::mutex &mtx)
{
    zmq::context_t ctx;
    zmq::socket_t socket(ctx, zmq::socket_type::pull);
    socket.bind("tcp://*:11000");

    cv::Mat img;
    zmq::message_t rcv_msg;
    int num, rows, cols, type;
    void *data;
    
    while(true)
    {
        std::lock_guard<std::mutex> lock(mtx);
        // 同時に送られてくる画像のサイズの受信
        socket.recv(&rcv_msg, 0);
        num = *(int*)rcv_msg.data();

        // スレッドの終了条件
        if( num == -1 )
        {
            flag = true;
            break;
        }

        safe_queue<cv::Mat*> que;
       
        for( int i = 0; i < num; i++ )
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
            //cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);
            }
            else {
                img = cv::Mat(rows, cols, CV_8UC3, data);
                cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);
            }
            que.push(&img);
        }
        ary.push(que);
    }
}

void show(safe_queue<safe_queue<cv::Mat*> > &ary, bool &flag, std::mutex &mtx)
{
    // zbarの初期設定
    zbar::ImageScanner scanner;
    // disable all
    scanner.set_config(zbar::ZBAR_NONE, zbar::ZBAR_CFG_ENABLE, 0);

    // enable qr
    scanner.set_config(zbar::ZBAR_QRCODE, zbar::ZBAR_CFG_ENABLE, 1);
    while(true)
    {
        std::lock_guard<std::mutex> lock(mtx);
        cv::Mat frame;

        // スレッドの終了条件
        if( ary.empty() && flag )
        {
            cv::destroyAllWindows();
            break;
        }

        while( !ary.empty() )
        {
            safe_queue<cv::Mat*> que(*ary.pop().get());
            std::cout << que.size() << std::endl;
            while( !que.empty() )
            {
                frame = *(*que.pop().get());
                bool is_decoded = false;
                //cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);

                if( frame.size().width > 0 && frame.size().height > 0 )
                {
                    zbar::Image image(frame.cols, frame.rows, "Y800", frame.data, frame.cols*frame.rows);
                    int n = scanner.scan(image);

                    if( n > 0 ) is_decoded = true;
                    else std::cout << "Data : " << std::endl;

                    // Print results
                    for(zbar::Image::SymbolIterator symbol = image.symbol_begin(); symbol != image.symbol_end(); ++symbol)
                    {
                        std::cout << "Data : " << symbol->get_data() << std::endl;
                    }
                    cv::imshow("sample", frame);
                    cv::waitKey(1);
                    // 画像のデコードに成功した場合は他の画像を全て破棄する
                    if( is_decoded ) que.clear();
                }
            }
        }
    }
}

int main()
{
    std::mutex mtx;
    bool flag = false;
    safe_queue<safe_queue<cv::Mat*> >ary;
    std::thread t1(receiver, std::ref(ary), std::ref(flag), std::ref(mtx));
    std::thread t2(show, std::ref(ary), std::ref(flag), std::ref(mtx));

    t1.join();
    t2.join();
}
