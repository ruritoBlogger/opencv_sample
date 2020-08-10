import zmq
import cv2
import struct
import pickle
import time
import numpy as np
import matplotlib.pyplot as plt

context = zmq.Context()
socket = context.socket(zmq.PUSH)
socket.connect("tcp://localhost:11000")
capture = cv2.VideoCapture("video.mp4")

while(True):
    ret, frame = capture.read()
    socket.send(pickle.dumps(frame)))
