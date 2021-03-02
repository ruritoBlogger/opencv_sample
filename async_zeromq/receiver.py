import asyncio
import zmq
import zmq.asyncio


async def manage_receive(is_last: bool) -> None:
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

    while is_last:
        tasks = [wait_message(soc) for i in range(10)]
        await asyncio.wait(tasks)

if __name__ == "__main__":
    print("START")
    loop = asyncio.get_event_loop()
    loop.run_until_complete(manage_receive(True))