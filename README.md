# HMM-GestureRecognition

A proof of concept for using Hidden Markov Models (HMMs) to recognize hand gestures from video, 
using OpenCV and Mediapipe for image processing and Qt for UI and networking.

This project was worked on for the Languages and Automata course of the 
bachelor Computer Science at the University of Antwerp.
## How it works

We take in a video in Python via OpenCV to feed the image data to Mediapipe. This gives us the on-screen coordinates of 
any hand features we want to track. In this case we mostly just use the tip of the index finger.

With a dataset of roughly 45 videos per gesture, we train a HMM for every gesture with our own implementation of the Baum-Welch algorithm.

When the HMMs have been sufficiently trained, we use them with the Forward algorithm to calculate the probability of
new unseen data: either from video files or from a realtime camera feed.

To demonstrate the possibilities that gesture recognition creates, two demo-applications are accessible from the GUI:
- Controlling a little remote-controlled vehicle through a TCP connection, with gestures corresponding to vehicle movements.
- Writing text with gestures: every gesture corresponds to a word.

## Compilation and running for the first time
### Dependencies
- Qt 5 or 6 (tested with 5.15 and 6.5)
- Python 3 with the imutils, OpenCV and Mediapipe packages (`pip install imutils opencv-python mediapipe`) 
### Compilation
- If on Windows: find the directory with Qt binaries for your compiler and add it to PATH, e.g. `D:/Qt/6.5.0/mingw_64/bin`
- Add the parent directory of the previous directory to the `CMAKE_PREFIX_PATH` CMake option, e.g. `-DCMAKE_PREFIX_PATH=D:/Qt/6.5.0/mingw_64`
- In CMakelists.txt, add the path to the python headers and binaries, according to the given examples
- If on your system, the command line command to run Python 3 is "python" instead of "python3", you can edit this in CMakelists.txt as well.

## Training Data
The collect-data branch contains every version of all our training videos, along with some helper Python scripts we used to create them.
Students and staff of the University of Antwerp can find the latest versions, that we use ourselves, [here](https://uantwerpen-my.sharepoint.com/:f:/g/personal/s0220671_ad_ua_ac_be/El_XvSfQ1CJPkldVhC1tJ_UBOGQ44X7ueCbLYlMyHeXK_w?e=Pfgn3M).
