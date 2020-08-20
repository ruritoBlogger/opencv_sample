import zmq
import cv2
import threading
import numpy as np
import queue
import random

def sendData(img_que, flag_que):

    context = zmq.Context()
    socket = context.socket(zmq.PUSH)
    socket.connect("tcp://localhost:11000")

    while(True):

        # スレッドの終了条件
        if( img_que.empty() and not flag_que.empty() ):
            flag = flag_que.get()
            if( flag == "predict" ):
                flag_que.put("end")
                print("sendData done")
                break
            else:
                flag_que.put(flag)

        if( not img_que.empty() ):
            frame = img_que.get()
            rows, cols = frame.shape[:2]
            ndims = frame.ndim
            array = [ np.array( [rows] ), np.array( [cols] ), np.array( [ndims] ), frame.data ]
            socket.send_multipart(array)

def getData(img_que, flag_que):
    capture = cv2.VideoCapture("./qr_fps60.mp4")
    while(True):
        ret, frame = capture.read()
        if( frame is None ):
            print("getData done")
            flag_que.put("get")
            break
        
        img_que.put(frame)

def predict(img_que, send_que, flag_que):
    while(True):

        # スレッドの終了条件
        if( img_que.empty() and not flag_que.empty() ):
            print("predict done")
            _ = flag_que.get()
            flag_que.put("predict")
            break
        
        if( not img_que.empty() ):
            frame = img_que.get()
            
            # ここで推論すると仮定
            #if( random.random() < 0.2 ):
            send_que.put(frame)

def main():
    img_que = queue.Queue()
    send_que = queue.Queue()
    flag_que = queue.Queue() 
    
    t1 = threading.Thread(target=getData, args=(img_que, flag_que, ))
    t2 = threading.Thread(target=predict, args=(img_que, send_que, flag_que, ))
    t3 = threading.Thread(target=sendData, args=(send_que, flag_que, ))
    t3.setDaemon(True)

    t1.start()
    t2.start()
    t3.start()

    t1.join()
    t2.join()
    # t3スレッドの実行状態を補足する
    while(True):
        if( not flag_que.empty() ):
            flag = flag_que.get()
            flag_que.put(flag)

            if( flag == "end" ):
                break


if __name__ == "__main__":
    main()
