import asyncio
import zmq
import zmq.asyncio
import time

is_last = False

async def manage_receive() -> None:
    ctx: zmq.asyncio.Context = zmq.asyncio.Context()
    soc: zmq.asyncio.Socket = ctx.socket(zmq.PULL)
    soc.connect("tcp://127.0.0.1:50000")

    sem = asyncio.Semaphore(3)

    async def wait_message(soc: zmq.asyncio.Socket) -> None:
        """ 受け取ったメッセージを返す """

        # NOTE: 同時に実行する数を制限しておく
        with await sem:
            msg = await soc.recv_string()
            print(msg)

    while not is_last:
        tasks = [wait_message(soc) for i in range(3)]
        await asyncio.wait(tasks)

async def test() -> None:
    await asyncio.sleep(1)
    key = 0
    while True:
        print("あああああ")
        await asyncio.sleep(2)
        key += 1

        if key > 10:
            is_last = True
            break


if __name__ == "__main__":
    print("START")
    loop = asyncio.get_event_loop()
    tasks = asyncio.gather(*[manage_receive(), test()])
    loop.run_until_complete(tasks)