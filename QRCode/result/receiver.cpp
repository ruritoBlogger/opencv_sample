#include <iostream>
#include <string>
#include <unistd.h>
#include <thread>
#include <zbar.h>
#include <mutex>
#include "zmq_addon.hpp"
#include <chrono>
#include <opencv2/opencv.hpp>

void my_free(void *data, void *hint)
{
        free(data);
}

void receiver(std::vector<std::queue<cv::Mat*> > &ary,
              bool &first_flag,
              std::mutex &mtx,
              std::chrono::system_clock::time_point &start)
{
    zmq::context_t ctx;
    zmq::socket_t socket(ctx, zmq::socket_type::pull);
    socket.bind("tcp://*:11000");

    cv::Mat img;
    zmq::message_t rcv_msg;
    int num, rows, cols, type;
    void *data;
    bool flag = true;
    
    while(true)
    {
        // 同時に送られてくる画像のサイズの受信
        socket.recv(&rcv_msg, 0);
        num = *(int*)rcv_msg.data();

        if( flag )
        {
            flag = false;
            start = std::chrono::system_clock::now();
        }

        // スレッドの終了条件
        if( num == -1 )
        {
            first_flag = true;
            break;
        }

        std::queue<cv::Mat*> que;

        // 同時に送られてくる画像を全て受信する
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
        {
            std::lock_guard<std::mutex> lock(mtx);
            ary.push_back(que);
        }
    }
}

void devide(std::vector<std::queue<cv::Mat*> > &ary,
            std::mutex &mtx,
            bool &first_flag,
            bool &second_flag,
            std::vector<std::queue<cv::Mat*> > &que)
{
    int cnt = 0;
    while(true)
    {
        {
            std::lock_guard<std::mutex> lock(mtx);
            // スレッドの終了条件
            if( ary.empty() && first_flag )
            {
                second_flag = true;
                std::cout << "CNT is " << cnt << std::endl;
                break;
            }
            if( ary.empty() ) continue;
        }

        cnt += ary[0].size();
        int max_size = 999999;
        int key = 0;

        for( int i = 0; i < que.size(); i++ )
        {
            std::lock_guard<std::mutex> lock(mtx);
            if( max_size > que[i].size() )
            {
                max_size = que[i].size();
                key = i;
            }
        }
        {
            std::lock_guard<std::mutex> lock(mtx);
            que[key].swap(ary[0]);
            ary.erase(ary.begin());
        }
    }
}

void show(std::queue<cv::Mat*> &que, bool &flag, std::mutex &mtx, std::string msg)
{
    // zbarの初期設定
    zbar::ImageScanner scanner;
    scanner.set_config(zbar::ZBAR_NONE, zbar::ZBAR_CFG_ENABLE, 0);
    scanner.set_config(zbar::ZBAR_QRCODE, zbar::ZBAR_CFG_ENABLE, 1);

    std::queue<cv::Mat*> result_que;


    while(true)
    {
        cv::Mat frame;
        {
            std::lock_guard<std::mutex> lock(mtx);
            // スレッドの終了条件
            if( que.empty() && flag )
            {
                cv::destroyAllWindows();
                break;
            }
            else if( !que.empty() )
            {
                result_que.swap(que);
            }
        }

        // デコードに成功するまで1グループの画像全てをデコードする
        while( !result_que.empty() )
        {
            frame = *result_que.front();
            result_que.pop();
            bool is_decoded = false;

            if( frame.size().width > 0 && frame.size().height > 0 )
            {
                zbar::Image image(frame.cols, frame.rows, "Y800", frame.data, frame.cols*frame.rows);
                int n = scanner.scan(image);

                if( n > 0 ) is_decoded = true;
                //else std::cout << "Data : " << std::endl;

                // Print results
                for(zbar::Image::SymbolIterator symbol = image.symbol_begin(); symbol != image.symbol_end(); ++symbol)
                {
                    std::cout << "Data : " << symbol->get_data() << std::endl;
                }
                /*
                cv::imshow("sample", frame);
                cv::waitKey(1);
                */
                // 画像のデコードに成功した場合は他の画像を全て破棄する
                if( is_decoded ) std::queue<cv::Mat*>().swap(result_que);
            }
        }
    }
}

int main()
{
    std::chrono::system_clock::time_point start;
    std::mutex mtx;
    bool first_flag = false;
    bool second_flag = false;
    // デコード用スレッドの数
    int thread_cnt = 3;
    std::vector<std::queue<cv::Mat*> >ary;
    std::vector<std::queue<cv::Mat*> >que(thread_cnt);
    //std::queue<cv::Mat*> tmp;
    //que.push_back(tmp);

    std::thread t1(receiver, std::ref(ary), std::ref(first_flag), std::ref(mtx), std::ref(start));
    std::thread t2(devide, std::ref(ary), std::ref(mtx), std::ref(first_flag), std::ref(second_flag), std::ref(que));

    //std::thread t3(show, std::ref(que[0]), std::ref(second_flag), std::ref(mtx), "sample");
    std::vector<std::thread> decode_threads(thread_cnt);

    for( int i = 0; i < thread_cnt; i++ )
    {
        std::queue<cv::Mat*> tmp;
        que.push_back( tmp );
    }
    for( int i = 0; i < thread_cnt; i++ )
    {
        decode_threads[i] = std::thread(show, std::ref(que[i]), std::ref(second_flag), std::ref(mtx), "que" + std::to_string(i+1));
        /*
        std::thread tmp_thread(show, std::ref(que[i]), std::ref(second_flag), std::ref(mtx), "que" + std::to_string(i+1));
        decode_threads.push_back(&tmp_thread);
        */
    }

    t1.join();
    t2.join();
    //t3.join();
    for( int i = 0; i < thread_cnt; i++ ) decode_threads[i].join();

    double result = std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now() - start).count();
    std::cout << "the time is " << result << " (ms)" << std::endl;
}
