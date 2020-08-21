import zmq
import cv2
import threading
import numpy as np
import queue
import heapq
import time
import random

context = zmq.Context()
socket = context.socket(zmq.PUSH)
socket.connect("tcp://localhost:11000")

cap = cv2.VideoCapture("./qr_fps60.mp4")

while(True):
    _, frame = cap.read()
    if( frame is None ):
        break
    
    rows, cols = frame.shape[:2]
    ndims = frame.ndim
    array = [ np.array( [rows] ), np.array( [cols] ), np.array( [ndims] ), frame.data ]
    socket.send_multipart(array)
