#include <iostream>
#include <string>
#include <unistd.h>
#include <thread>
#include <zbar.h>
#include "zmq_addon.hpp"
#include <opencv2/opencv.hpp>
#include "queue.cpp"

void my_free(void *data, void *hint)
{
        free(data);
}

void receiver(safe_queue<cv::Mat*> &que)
{
    zmq::context_t ctx;
    zmq::socket_t socket(ctx, zmq::socket_type::pull);
    socket.bind("tcp://*:11000");

    cv::Mat img;
    zmq::message_t rcv_msg;
    int rows, cols, type;
    void *data;
    
    while(true)
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
	//que.push(std::make_tuple(img.size().width, img.size().height, CV_8UC1, img.data));
        printf("rows=%d, cols=%d type=%d\n", rows, cols, type);
    }
}

void show(safe_queue<cv::Mat*> &que)
{
    // zbarの初期設定
    zbar::ImageScanner scanner;
    // disable all
    scanner.set_config(zbar::ZBAR_NONE, zbar::ZBAR_CFG_ENABLE, 0);

    // enable qr
    scanner.set_config(zbar::ZBAR_QRCODE, zbar::ZBAR_CFG_ENABLE, 1);
    while(true)
    {
	cv::Mat frame;
	if( !que.empty() )
	{
	    frame = *(*que.pop().get());
	    //cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);

	    if( frame.size().width > 0 && frame.size().height > 0 )
	    {
	    	zbar::Image image(frame.cols, frame.rows, "Y800", frame.data, frame.cols*frame.rows);
	
		int n = scanner.scan(image);

	        // Print results
	        for(zbar::Image::SymbolIterator symbol = image.symbol_begin(); symbol != image.symbol_end(); ++symbol)
	        {
	            std::cout << "Data : " << symbol->get_data() << std::endl;
	        }
	        cv::imshow("sample", frame);
	        cv::waitKey(1);
	    }
	}
    }
}

int main()
{
    safe_queue<cv::Mat*> que;
    std::thread t1(receiver, std::ref(que));
    std::thread t2(show, std::ref(que));

    t1.join();
    t2.join();
}
