# Basic script to "stream" video captured from a camera to localhost


import cv2
import numpy as np
import imutils
import struct
import socket
import get_landmarks as utils


def run_tcp_sewer():
    """
    Start the TCP server that transfers frames with drawn landmarks and the landmarks self

    :return: Nothing to return, but the pain
    """
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
        image, results = utils.mediapipe_detection(frame)  # Get the landmarks
        utils.draw_styled_landmarks(image, results, True)  # Draw the landmarks on the image
        image = cv2.flip(image, 1)  # Flip the image
        #frame = imutils.resize(frame, width=320)
        a = cv2.imencode('.jpg', image)[1].tobytes()  # Convert image to bytes
        imageSocket.sendall(a)  # Send the frame with the landmarks on it
        results = utils.extract_keypoints(results)  # Extract only necessary landmarks
        # dummyValue = 280
        # bytes = dummyValue.to_bytes(4, 'big')
        bytes_data = struct.pack('>' + 'd' * len(results), *results) # '>' stands for 'big endian'
        dataSocket.sendall(bytes_data)

        k = cv2.waitKey(30) & 0xff
        if k==27:
            break

    cap.release()
    cv2.destroyAllWindows()

run_tcp_sewer()

