import zmq
import struct
import time
import numpy as np
import matplotlib.pyplot as plt

context = zmq.Context()
socket = context.socket(zmq.PUSH)
socket.connect("tcp://localhost:11000")

while(True):
    number = "2434"
    socket.send_string(number)
