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
    rows = int.from_bytes(byte_rows, byteorder='big', signed=True)
    cols = int.from_bytes(byte_cols, byteorder='big', signed=True)
    mat_type = int.from_bytes(byte_mat_type, byteorder='big', signed=True)
    print(rows,cols)
    #実際に受信する
    if mat_type == 0:
        # Gray Scale
        image = np.frombuffer(data, dtype=np.uint8).reshape((rows,cols));
    else:
        # BGR Color
        image = np.frombuffer(data, dtype=np.uint8).reshape((rows,cols,3));

    cnt += 1

    if( cnt == 500 ):
        end = time.time()
        break

print(end - start)
