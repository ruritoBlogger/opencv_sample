from pyzbar.pyzbar import decode, ZBarSymbol
import cv2
import time
from PIL import Image

key = 0
start = time.time()

while True:

    if( key > 1000 ):
        break

    # opencv2で読み込む場合
    img = cv2.imread("./not.jpg")
    img = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)
    # data = decode(img, symbols=[ZBarSymbol.QRCODE])

    # 以下はバイナリからデコードする場合
    height, width = img.shape
    data = decode((img.tobytes(), height, width), symbols=[ZBarSymbol.QRCODE])
   
    # PILで読み込む場合
    # img = Image.open("./not.jpg")
    # img = img.convert('L')
    # data = decode(img, symbols=[ZBarSymbol.QRCODE])
    

    # 以下はバイナリからデコードする場合
    # height, width = img.size
    # data = decode((img.tobytes(), height, width), symbols=[ZBarSymbol.QRCODE])
    key += 1

print("result time is {}".format(time.time() - start))
