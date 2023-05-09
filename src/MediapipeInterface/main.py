# Basic script to "stream" video captured from a camera to localhost


import cv2
import numpy as np
import imutils
import socket

HOST = "127.0.0.1"
PORT = 5000

with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sock:
    sock.connect((HOST, PORT))
    #data = sock.recv(1024)
    cap = cv2.VideoCapture(0)
    _, frame = cap.read()
    while True:
        _, frame = cap.read()
        #frame = imutils.resize(frame, width=320)
        #frame = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        a = cv2.imencode('.jpg', frame)[1].tobytes()
        sock.sendall(a)

        k = cv2.waitKey(30) & 0xff
        if k==27:
            break
cap.release()
cv2.destroyAllWindows()



