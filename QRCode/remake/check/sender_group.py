import zmq
import cv2
import threading
import numpy as np
import queue
import time
from operator import itemgetter

def sendData(img_que, flag_que, lock):

    context = zmq.Context()
    socket = context.socket(zmq.PUSH)
    socket.connect("tcp://localhost:11000")
    tmp = 0

    while(True):

        # スレッドの終了条件
        if( img_que.empty() and not flag_que.empty() ):
            flag = flag_que.get()
            if( flag == "predict" ):

                # C++側の処理を終了させるメッセージを送信
                array = [ np.array( [-1] ) ]
                socket.send_multipart(array)

                flag_que.put("end")
                print("sendData cnt is {}".format(tmp))
                break
            else:
                flag_que.put(flag)

        if( not img_que.empty() ):
            que = img_que.get()


            array = [ np.array( [que.qsize()] ) ]
            socket.send_multipart(array)
            #tmp += que.qsize()

            # 1グループ全ての画像を送信する
            while( not que.empty() ):
                tmp += 1
                frame = que.get()

                rows, cols = frame.shape[:2]
                ndims = frame.ndim
                array = [ np.array( [rows] ), np.array( [cols] ), np.array( [ndims] ), frame.data ]
                socket.send_multipart(array)

                """
                frame = cv2.resize(frame, dsize=(800, 600))
                cv2.imshow("test", frame)
                cv2.waitKey(1)
                """

def getData(img_que, send_que, flag_que, is_fast_mode):
    capture = cv2.VideoCapture("./qr_fps60.mp4")
    start = time.time()
    cnt = 0
    while(True):
        ret, frame = capture.read()

        # スレッドの終了条件
        #if( frame is None or time.time() - start > 10):
        if( frame is None ):
            print("getData cnt is {}".format(cnt))

            if( is_fast_mode ):
                flag_que.put("get")
            
            # 低速モードの場合は推論が終了したと仮定する
            else:
                flag_que.put("predict")
            break
        
        cnt += 1
        img_que.put(frame)

        #if( cnt%10 == 0 ):
            #time.sleep(1)
        # 低速モードの場合は1秒間隔を空けて撮影する
        if( not is_fast_mode ):
            time.sleep(1)

def predict(img_que, send_que, flag_que, fast_mode_props, lock):

    max_group_interval = fast_mode_props["max_interval"]/2
    group_list = []
    n_exist = 0
    start_time = time.time()
    num = 0

    while(True):

        # スレッドの終了条件
        if( img_que.empty() and not flag_que.empty() and len(group_list) == 0 ):
            print("predict cnt is {}".format(num))
            _ = flag_que.get()
            flag_que.put("predict")
            break

        if( not img_que.empty() ):
            frame = img_que.get()
            num += 1
            
            # ここで推論すると仮定
            group_list.append({"key": num, "img": frame})
            if( True ):
                n_exist += 1

            # グループ形成終了条件
            # 一定時間経過もしくは、img_queが空の時かつgroup_listに画像が格納されている時
            if( time.time() - start_time >= max_group_interval
                or (img_que.empty() and len(group_list) is not 0) ):

                # QRコードが存在する可能性が高い場合はデコードする
                if( n_exist > 0 ):
                    lock.acquire()
                    group_que = queue.Queue()

                    # QRコードが存在する可能性が高い順にソート
                    group_list.sort(key=itemgetter("key"))
                    for dic in group_list:
                        group_que.put(dic["img"])

                    send_que.put(group_que) 
                    lock.release()

                # グループの初期化
                group_list = []
                n_exist = 0
                start_time = time.time()

def main():

    # 高速モードで実行するかどうか
    is_fast_mode = True

    # 高速モードで実行する場合の設定
    fast_mode_props = {}
    fast_mode_props["velocity"] = 1
    fast_mode_props["distance"] = 1
    fast_mode_props["max_interval"] = 0.3

    img_que = queue.Queue()
    send_que = queue.Queue()
    
    # flag_queの要素 ["", "get", "predict", "end"]
    flag_que = queue.Queue() 

    lock = threading.Lock()

    t1 = threading.Thread(target=getData, args=(img_que, send_que, flag_que, is_fast_mode, ))
    t1.start()

    # 高速モードの時は推論を行うスレッドを動作させる
    # また、推論を用いて選別した画像のみを送信する
    if( is_fast_mode ):
        t2 = threading.Thread(target=predict, args=(img_que, send_que, flag_que, fast_mode_props, lock, ))
        t2.start()

        t3 = threading.Thread(target=sendData, args=(send_que, flag_que, lock, ))
        t3.setDaemon(True)
        t3.start()

    else:
        t2 = threading.Thread(target=sendData, args=(img_que, flag_que, lock, ))
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
