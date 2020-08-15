#include <iostream>
#include <string>
#include <unistd.h>
#include <thread>
#include "zmq_addon.hpp"
#include <opencv2/opencv.hpp>
#include <zbar.h>
#include "queue.cpp"

void my_free(void *data, void *hint)
{
        free(data);
}

int main()
{
    zmq::context_t ctx;
    zmq::socket_t socket(ctx, zmq::socket_type::pull);
    socket.bind("tcp://*:11000");

    // zbarの初期設定
    zbar::ImageScanner scanner;
    // disable all
    scanner.set_config(zbar::ZBAR_NONE, zbar::ZBAR_CFG_ENABLE, 0);

    // enable qr
    scanner.set_config(zbar::ZBAR_QRCODE, zbar::ZBAR_CFG_ENABLE, 1);
 
    safe_queue<cv::Mat> que;
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
        }
        else {
            img = cv::Mat(rows, cols, CV_8UC3, data);
        }
	cv::cvtColor(img, img, cv::COLOR_BGR2GRAY);
        //printf("rows=%d, cols=%d type=%d\n", rows, cols, type);
	zbar::Image image(img.cols, img.rows, "Y800", img.data, img.cols*img.rows);
	
	int n = scanner.scan(image);

	// Print results
	for(zbar::Image::SymbolIterator symbol = image.symbol_begin(); symbol != image.symbol_end(); ++symbol)
	{
	    std::cout << "Data : " << symbol->get_data() << std::endl;
	}
      	cv::imshow("sample", img);
	cv::waitKey(1);	
        /*	
	que.push(img);
        if( que.size() > 0 )
        {
            img = *que.pop().get();
            cv::imshow("sample",img);
            cv::waitKey(1);
        }
	*/
    }
    cv::destroyAllWindows();
}
