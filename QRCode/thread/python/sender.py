import zmq
import cv2
import threading
import numpy as np
import queue
import random

def sendData(que):

    context = zmq.Context()
    socket = context.socket(zmq.PUSH)
    socket.connect("tcp://localhost:11000")

    while(True):
        if( not que.empty() ):
            frame = que.get()
            rows, cols = frame.shape[:2]
            ndims = frame.ndim
            array = [ np.array( [rows] ), np.array( [cols] ), np.array( [ndims] ), frame.data ]
            socket.send_multipart(array)

def getData(que):
    capture = cv2.VideoCapture(0)
    while(True):
        ret, frame = capture.read()
        que.put(frame)

def predict(img_que, send_que):
    while(True):
        if( not img_que.empty() ):
            frame = img_que.get()
            
            # ここで推論すると仮定
            #if( random.random() < 0.2 ):
            send_que.put(frame)

def main():
    img_que = queue.Queue()
    send_que = queue.Queue()
    t1 = threading.Thread(target=getData, args=(img_que, ))
    t2 = threading.Thread(target=predict, args=(img_que, send_que, ))
    t3 = threading.Thread(target=sendData, args=(send_que, ))

    t1.start()
    t2.start()
    t3.start()

if __name__ == "__main__":
    main()
