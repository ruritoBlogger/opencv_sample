import zmq
import cv2
import struct
import time
import numpy as np
import matplotlib.pyplot as plt

context = zmq.Context()
socket = context.socket(zmq.PULL)
socket.bind("tcp://*:11000")
flag = False

while(True):
    # ヘッダーの受信（受信できるまで待つ）
    byte_rows, byte_cols, byte_mat_type, data = socket.recv_multipart()
    if( not flag ):
        start = time.time()
        cnt = 0
        flag = True
    
    # ヘッダーを解釈する
    rows = struct.unpack('i', byte_rows)
    cols = struct.unpack('i', byte_cols)
    mat_type = struct.unpack('i', byte_mat_type)

    #実際に受信する
    if mat_type[0] == 0:
        # Gray Scale
        image = np.frombuffer(data, dtype=np.uint8).reshape((rows[0],cols[0]));
    else:
        # BGR Color
        image = np.frombuffer(data, dtype=np.uint8).reshape((rows[0],cols[0],3));

    cnt += 1

    if( cnt == 500 ):
        end = time.time()
        break

print(end - start)
