# Basic script to "stream" video captured from a camera to localhost


import cv2
import numpy as np
import imutils
import socket

HOST = "127.0.0.1"
IMAGEPORT = 5000
DATAPORT = 5001

imageSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
dataSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
imageSocket.connect((HOST, IMAGEPORT))
dataSocket.connect((HOST, DATAPORT))
cap = cv2.VideoCapture(0)
while True:
    _, frame = cap.read()
    #frame = imutils.resize(frame, width=320)
    a = cv2.imencode('.jpg', frame)[1].tobytes()
    imageSocket.sendall(a)
    dummyValue = 280
    bytes = dummyValue.to_bytes(4, 'big')
    dataSocket.sendall(bytes)

    k = cv2.waitKey(30) & 0xff
    if k==27:
        break

cap.release()
cv2.destroyAllWindows()



