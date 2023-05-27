import cv2
import numpy as np
import os
import time
import mediapipe as mp
import sys

fourcc = cv2.VideoWriter_fourcc(*'XVID') # Video writer
mp_holistic = mp.solutions.holistic  # Holistic model
mp_drawing = mp.solutions.drawing_utils  # Drawing utilities


def mediapipe_detection(image, model=mp_holistic.Holistic(min_detection_confidence=0.5, min_tracking_confidence=0.5)):
    image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)  # COLOR CONVERSION BGR 2 RGB
    image.flags.writeable = False  # Image is no longer writeable
    results = model.process(image)  # Make prediction
    image.flags.writeable = True  # Image is now writeable
    image = cv2.cvtColor(image, cv2.COLOR_RGB2BGR)  # COLOR COVERSION RGB 2 BGR
    return image, results


def only_necessary_landmarks(results):
    right_handmarks = results.right_hand_landmarks
    if right_handmarks is None: return
    # Hardcoded number of landmarks
    r = 21
    to_add = None
    for i in range(r):
        # Gehardcoded number of the landmark (20 - 8)
        if i == 12:
            to_add = right_handmarks.landmark.pop()
            continue
        right_handmarks.landmark.pop()
    right_handmarks.landmark.insert(0, to_add)
    results.right_hand_landmarks = right_handmarks
    return results

def draw_styled_landmarks(image, results, draw_all):
    if draw_all:
        # Draw face connections
        mp_drawing.draw_landmarks(image, results.face_landmarks, mp_holistic.FACEMESH_CONTOURS,
                                  mp_drawing.DrawingSpec(color=(80, 110, 10), thickness=1, circle_radius=1),
                                  mp_drawing.DrawingSpec(color=(80, 256, 121), thickness=1, circle_radius=1)
                                  )
        # Draw pose connections
        mp_drawing.draw_landmarks(image, results.pose_landmarks, mp_holistic.POSE_CONNECTIONS,
                                  mp_drawing.DrawingSpec(color=(80, 22, 10), thickness=2, circle_radius=4),
                                  mp_drawing.DrawingSpec(color=(80, 44, 121), thickness=2, circle_radius=2)
                                  )
        # Draw left hand connections
        mp_drawing.draw_landmarks(image, results.left_hand_landmarks, mp_holistic.HAND_CONNECTIONS,
                                  mp_drawing.DrawingSpec(color=(121, 22, 76), thickness=2, circle_radius=4),
                                  mp_drawing.DrawingSpec(color=(121, 44, 250), thickness=2, circle_radius=2)
                                  )
        # Draw right hand connections
        mp_drawing.draw_landmarks(image, results.right_hand_landmarks, mp_holistic.HAND_CONNECTIONS,
                                  mp_drawing.DrawingSpec(color=(245, 117, 66), thickness=2, circle_radius=4),
                                  mp_drawing.DrawingSpec(color=(245, 66, 230), thickness=2, circle_radius=2)
                                  )
    else:
        results = only_necessary_landmarks(results)
        if results is None:
            print(None)
            return
        hand_connect = frozenset()
        mp_drawing.draw_landmarks(image, results.right_hand_landmarks, hand_connect,
                                  mp_drawing.DrawingSpec(color=(245, 117, 66), thickness=2, circle_radius=10),
                                  mp_drawing.DrawingSpec(color=(245, 66, 230), thickness=2, circle_radius=2)
                                  )
        print(results.right_hand_landmarks)

def extract_keypoints(results):
    # TODO: save everything?...

    # pose = np.array([[res.x, res.y, res.z, res.visibility] for res in
    #                  results.pose_landmarks.landmark]).flatten() if results.pose_landmarks else np.zeros(33 * 4)
    # face = np.array([[res.x, res.y, res.z] for res in
    #                  results.face_landmarks.landmark]).flatten() if results.face_landmarks else np.zeros(468 * 3)
    # lh = np.array([[res.x, res.y, res.z] for res in
    #                results.left_hand_landmarks.landmark]).flatten() if results.left_hand_landmarks else np.zeros(21 * 3)
    # rh = np.array([[res.x, res.y, res.z] for res in
    #                results.right_hand_landmarks.landmark]).flatten() if results.right_hand_landmarks else np.zeros(
    #     21 * 3)
    # try:
            if results is None: return [0, 0]
            if results.right_hand_landmarks:
                for res in results.right_hand_landmarks.landmark:
                    rh = [res.x, res.y]
            else:
                rh = [0, 0]
    # except:
    #     rh = [0,0]
            return rh
    # return np.concatenate([# pose,
    #     # face,
    #     # lh,
    #     rh])

def extract_realtime_keypoints(results):
    # TODO: save everything?...

    # pose = np.array([[res.x, res.y, res.z, res.visibility] for res in
    #                  results.pose_landmarks.landmark]).flatten() if results.pose_landmarks else np.zeros(33 * 4)
    # face = np.array([[res.x, res.y, res.z] for res in
    #                  results.face_landmarks.landmark]).flatten() if results.face_landmarks else np.zeros(468 * 3)
    # lh = np.array([[res.x, res.y, res.z] for res in
    #                results.left_hand_landmarks.landmark]).flatten() if results.left_hand_landmarks else np.zeros(21 * 3)
    # rh = np.array([[res.x, res.y, res.z] for res in
    #                results.right_hand_landmarks.landmark]).flatten() if results.right_hand_landmarks else np.zeros(
    #     21 * 3)
    # try:
    if results is None: return [-1, -1, -1]
    lefty = -1
    if results.left_hand_landmarks:
        lefty = 1
    if results.right_hand_landmarks:
        for res in results.right_hand_landmarks.landmark:
            rh = [res.x, res.y, lefty]      # TODO: it's not efficient, choose another method
    else:
        rh = [-1, -1, lefty]
    # except:
    #     rh = [0,0]
    return rh
# return np.concatenate([# pose,
#     # face,
#     # lh,
#     rh])

def get_landmarks_from_video(video_path):
    """
    Collects landmarks from the given video that previously was flipped (per frame)

    :param video_path: absolute video path
    :return only necessary landmarks (array of arrays of doubles)
    """

    timestart = time.time()
    to_return = []
    #cap = cv2.VideoCapture(os.path.join(os.getcwd(), video_path))
    cap = cv2.VideoCapture(video_path)
    # Set mediapipe model
    with mp_holistic.Holistic(min_detection_confidence=0.5, min_tracking_confidence=0.5) as holistic:
        while True:
                success, frame = cap.read()
                if not success: break
                # Flip frame
                frame = cv2.flip(frame, 1)
                # Make detections
                image, results = mediapipe_detection(frame, holistic)
                # For debug:
                # draw_styled_landmarks(image, results, True)
                # cv2.imshow("Try me, try me", image)
                to_return.append(extract_keypoints(only_necessary_landmarks(results)))
                if cv2.waitKey(1) & 0xFF == ord('q'): break

    cap.release()
    # print("Total time", time.time() - timestart)
    return to_return

#get_landmarks_from_video("C:/Users/dasha/PycharmProjects/HMM-GestureRecognition/Collected_videos/zero/zero7_0.avi")