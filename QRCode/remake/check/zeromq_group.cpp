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
    int key = 0;
    
    while(true)
    {
        // 同時に送られてくる画像のサイズの受信
        socket.recv(&rcv_msg, 0);
        num = *(int*)rcv_msg.data();

        // スレッドの終了条件
        if( num == -1 )
        {
            std::lock_guard<std::mutex> lock(mtx);
            flag = true;
            std::cout << "KEY IS " << key << std::endl;
            break;
        }

        safe_queue<cv::Mat*> que;

        // 同時に送られてくる画像を全て受信する
        for( int i = 0; i < num; i++ )
        {
            key++;
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
            //printf("rows=%d, cols=%d type=%d\n", rows, cols, type);
        }
        ary.push(que);
    }
}

void show(safe_queue<safe_queue<cv::Mat*> > &ary, bool &flag, std::mutex &mtx)
{
    // zbarの初期設定
    zbar::ImageScanner scanner;
    scanner.set_config(zbar::ZBAR_NONE, zbar::ZBAR_CFG_ENABLE, 0);
    scanner.set_config(zbar::ZBAR_QRCODE, zbar::ZBAR_CFG_ENABLE, 1);
    int cnt = 0;
    int tmp1 = 0;
    int ok_cnt = 0;

    std::vector<safe_queue<cv::Mat*> > _ary;

    while(true)
    {
        // スレッドの終了条件
        {
            std::lock_guard<std::mutex> lock(mtx);
            if( ary.empty() && !_ary.empty() && flag )
            {
                std::cout << "tmp1 is " << tmp1 << std::endl;
                std::cout << cnt << std::endl;
                std::cout << "result is " << ok_cnt << std::endl;
                break;
            }
        }

        while( !ary.empty() )
        {
            cv::Mat frame;
            _ary.push_back(*ary.pop().get());

            if( !_ary.empty() )
            {
                safe_queue<cv::Mat*> que(_ary.front());

                //safe_queue<cv::Mat*> que(*tmp_ary.pop().get());
                // デコードに成功するまで1グループの画像全てをデコードする
                //std::cout << que.size() << std::endl;
                cnt += que.size();
                std::cout << "que is " << que.size() << std::endl;
                while( !que.empty() )
                {
                    std::cout << tmp1 << std::endl;
                    frame = *(*que.pop().get());
                    bool is_decoded = false;

                    if( frame.size().width > 0 && frame.size().height > 0 )
                    {
                        tmp1++;
                        
                        zbar::Image image(frame.cols, frame.rows, "Y800", frame.data, frame.cols*frame.rows);
                        
                        cv::resize(frame, frame, cv::Size(), 800.0/frame.cols, 600.0/frame.rows);
                        cv::imshow("test", frame);
                        cv::waitKey(1);

                        int n = scanner.scan(image);

                        if( n > 0 )
                        {
                            is_decoded = true;
                            ok_cnt++;
                        }
                        //else std::cout << "Data : " << std::endl;

                        // Print results
                        for(zbar::Image::SymbolIterator symbol = image.symbol_begin(); symbol != image.symbol_end(); ++symbol)
                        {
                            std::cout << "Data : " << symbol->get_data() << std::endl;
                        }
                        // 画像のデコードに成功した場合は他の画像を全て破棄する
                        if( is_decoded ) que.clear();
                    }
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
