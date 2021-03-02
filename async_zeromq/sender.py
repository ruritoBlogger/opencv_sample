import zmq
import time

def send_message(soc: zmq.Socket ,msg: str) -> None:
    """ メッセージを送信する """
    soc.send_string(msg)


if __name__ == "__main__":
    ctx: zmq.Context = zmq.Context()
    soc: zmq.Socket = ctx.socket(zmq.PUSH)
    soc.bind("tcp://127.0.0.1:50000")
    key = 0

    while(True):
        time.sleep(1)
        send_message(soc, '{}回目のメッセージ'.format(key))
        key += 1
