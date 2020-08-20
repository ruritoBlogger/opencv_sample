import zmq
import cv2
import threading
import numpy as np
import queue
import heapq
import time
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

def getData(img_que, flag_que, is_fast_mode):
    capture = cv2.VideoCapture(0)
    start = time.time()
    while(True):
        ret, frame = capture.read()

        # スレッドの終了条件
        if( frame is None or time.time() - start > 10):
            print("getData done")

            if( is_fast_mode ):
                flag_que.put("get")
            
            # 低速モードの場合は推論が終了したと仮定する
            else:
                flag_que.put("predict")
            break
        
        img_que.put(frame)

        # 低速モードの場合は1秒間隔を空けて撮影する
        if( not is_fast_mode ):
            time.sleep(1)

def predict(img_que, send_que, flag_que, fast_mode_props):

    max_group_interval = fast_mode_props["max_interval"]/2
    group_que = []
    n_exist = 0
    start_time = time.time()

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
            key = random.random()
            heapq.heappush(group_que, (-1*key, frame) )
            if( key < 0.2 ):
                n_exist += 1

            # グループ形成終了条件
            if( time.time() - start_time >= max_group_interval ):
                # QRコードが存在する可能性が高い場合はデコードする
                if( n_exist > 0 ):
                    p, great_frame = heapq.heappop(group_que)
                    send_que.put(great_frame)

                # グループの初期化
                group_que = []
                n_exist = 0
                start_time = time.time()

def main():

    # 高速モードで実行するかどうか
    is_fast_mode = True

    # 高速モードで実行する場合の設定
    fast_mode_props = {}
    fast_mode_props["velocity"] = 1
    fast_mode_props["distance"] = 1
    fast_mode_props["max_interval"] = 0.1

    img_que = queue.Queue()
    send_que = queue.Queue()
    
    # flag_queの要素 ["", "get", "predict", "end"]
    flag_que = queue.Queue() 

    t1 = threading.Thread(target=getData, args=(img_que, flag_que, is_fast_mode, ))
    t1.start()

    # 高速モードの時は推論を行うスレッドを動作させる
    # また、推論を用いて選別した画像のみを送信する
    if( is_fast_mode ):
        t2 = threading.Thread(target=predict, args=(img_que, send_que, flag_que, fast_mode_props, ))
        t2.start()

        t3 = threading.Thread(target=sendData, args=(send_que, flag_que, ))
        t3.setDaemon(True)
        t3.start()

    else:
        t2 = threading.Thread(target=sendData, args=(img_que, flag_que, ))
        t2.setDaemon(True)
        t2.start()

    # スレッドの実行を待機
    t1.join()

    if( is_fast_mode ):
        t2.join()

        # t3スレッドの実行状態を補足する
        while(True):
            if( not flag_que.empty() ):
                flag = flag_que.get()
                flag_que.put(flag)

                if( flag == "end" ):
                    break
    
    else:
        # t2スレッドの実行状態を補足する
        while(True):
            if( not flag_que.empty() ):
                flag = flag_que.get()
                flag_que.put(flag)

                if( flag == "end" ):
                    break

if __name__ == "__main__":
    main()
