import cv2
import numpy as np
import os
import time
import mediapipe as mp
import sys
import cv2
import numpy as np
import os
import time
import mediapipe as mp
import sys

##############################################################
##################### YOU MAY EDIT THIS ######################
##############################################################
# Path for exported data
folder_name = os.path.join(os.getcwd(), '../../../Sign-language')

# Names of gestures
gestures = np.array(['luisteren'])
#gestures = np.array(['vooruit', 'achteruit', 'links', 'rechts', 'stop'])

# Number of videos per gesture
no_sequences = 1

# Do you want to draw everything?
draw_all = False

# Key to go to next video
stop_key = 'q'

##############################################################

fourcc = cv2.VideoWriter_fourcc(*'XVID') # Video writer
mp_holistic = mp.solutions.holistic  # Holistic model
mp_drawing = mp.solutions.drawing_utils  # Drawing utilities


def mediapipe_detection(image, model):
    image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)  # COLOR CONVERSION BGR 2 RGB
    image.flags.writeable = False  # Image is no longer writeable
    results = model.process(image)  # Make prediction
    image.flags.writeable = True  # Image is now writeable
    image = cv2.cvtColor(image, cv2.COLOR_RGB2BGR)  # COLOR COVERSION RGB 2 BGR
    return image, results


def alleen_nodige_resultaten(results):
    right_handmarks = results.right_hand_landmarks
    if right_handmarks is None: return
    # Gehardcoded number of landmarks
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
        results = alleen_nodige_resultaten(results)
        if results is None:
            print(None)
            return
        hand_connect = frozenset()
        mp_drawing.draw_landmarks(image, results.right_hand_landmarks, hand_connect,
                                  mp_drawing.DrawingSpec(color=(245, 117, 66), thickness=2, circle_radius=10),
                                  mp_drawing.DrawingSpec(color=(245, 66, 230), thickness=2, circle_radius=2)
                                  )
        print(results.right_hand_landmarks)


# Collect
# Keypoint
# Values
# for Training and Testing

def start_collection(folder_name, gestures, no_sequences, draw_all, stop_key):
    """
    Collects gesture video's from the feed and saves to the given folder

    :param folder_name: relative folder path
    :param gestures: array of gesture names
    :param no_sequences: number of sequences per gesture
    :param draw_all: do you want to draw all landmarks?
    :param stop_key: char to go the next video recording
    """

    OUTPUT_FOLDER = os.path.join(os.getcwd(), folder_name)

    # Set folders
    for gesture in gestures:
        try:
            os.makedirs(os.path.join(OUTPUT_FOLDER, gesture))
        except:
            pass
    cap = cv2.VideoCapture(0)
    # Set mediapipe model
    with mp_holistic.Holistic(min_detection_confidence=0.5, min_tracking_confidence=0.5) as holistic:
        # Loop through actions
        for gesture in gestures:
            # Loop through sequences aka videos
            for reeks in range(no_sequences):
                bool_record = True

                # Get number of files in the directory
                lst = os.listdir(os.path.join(OUTPUT_FOLDER, gesture))
                number_files = len(lst)

                # Create video file
                file_created = False
                file_duplicate_number = 0
                while not file_created:
                    # Check if file with given name already exists
                    if os.path.exists(os.path.join(OUTPUT_FOLDER, gesture, f'{gesture}{number_files}_{file_duplicate_number}.avi')):
                        file_duplicate_number += 1
                    else:
                        out = cv2.VideoWriter(os.path.join(OUTPUT_FOLDER, gesture, f'{gesture}{number_files}_{file_duplicate_number}.avi'),
                                              fourcc, 20.0, (640, 480))
                        file_created = True
                frame_num = 0
                while bool_record:

                    # Read feed
                    ret, frame = cap.read()

                    # Make detections
                    image, results = mediapipe_detection(frame, holistic)

                    # Draw landmarks
                    draw_styled_landmarks(image, results, draw_all)

                    # Flip image
                    image = cv2.flip(image, 1)
                    frame = cv2.flip(frame, 1)

                    out.write(frame)
                    # Apply wait logic
                    if frame_num == 0:
                        cv2.putText(image, 'BEGIN VERZAMELEN', (120, 200),
                                    cv2.FONT_HERSHEY_SIMPLEX, 1, (0, 255, 0), 4, cv2.LINE_AA)
                        cv2.putText(image, 'Verzamelen van frames voor gebaar {} video #{}'.format(gesture, reeks), (15, 12),
                                    cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 255), 1, cv2.LINE_AA)
                        # Show to screen
                        cv2.imshow('Opname van gebaren', image)
                        cv2.waitKey(2000)
                        frame_num += 1
                    else:
                        cv2.putText(image, 'Verzamelen van frames voor gebaar {} video #{}'.format(gesture, reeks), (15, 12),
                                    cv2.FONT_HERSHEY_SIMPLEX, 0.5, (0, 0, 255), 1, cv2.LINE_AA)
                        # Show to screen
                        cv2.imshow('Opname van gebaren', image)

                    # Break
                    if cv2.waitKey(10) & 0xFF == ord(stop_key):
                        bool_record = False
                        out.release()
                        break

        cap.release()
        cv2.destroyAllWindows()

    cap.release()
    cv2.destroyAllWindows()
start_collection(folder_name, gestures, no_sequences, draw_all, stop_key)