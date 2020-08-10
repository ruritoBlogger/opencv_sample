import cv2
import time



cap = cv2.VideoCapture(2)

n = 0
s = time.time()

while True:
    _, frame = cap.read()
    
     
    if frame is not None:
        print(frame.shape)
        cv2.imshow("title", frame)
        n += 1

        if n == 500:
            t = time.time() - s
            print(t)
            break
