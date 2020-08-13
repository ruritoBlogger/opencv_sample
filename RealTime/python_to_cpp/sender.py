import zmq
import cv2
import struct
import time
import numpy as np
import matplotlib.pyplot as plt

context = zmq.Context()
socket = context.socket(zmq.PUSH)
socket.connect("tcp://localhost:11000")
capture = cv2.VideoCapture(0)

while(True):
    ret, frame = capture.read()
    rows, cols = frame.shape[:2]
    ndims = frame.ndim
    
    array = [ np.array( [rows] ), np.array( [cols] ), np.array( [ndims] ), frame.data ]
    socket.send_multipart(array)
