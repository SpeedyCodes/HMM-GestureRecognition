#ifndef HMM_GESTURERECOGNITION_GESTURE_H
#define HMM_GESTURERECOGNITION_GESTURE_H

/**
 * This class serves as a sort of wrapper for the HMM class when using it to detect gestures.
 * It keeps some gesture-related data and provides some convenience-methods.
 * A Gesture does expose its underlying HMM though.
 */

#include <string>
#include "json.hpp"

using namespace std;

class HMM;

class Gesture {
public:

/**
 * Constructor to use when making a brand-new gesture with a HMM that has just been trained.
 * @param gestureId The name of the gesture
 * @param hiddenMarkovModel A pointer to the Hidden Markov Model to recognize the gesture
 */

    Gesture(const string &gestureId, HMM *hiddenMarkovModel);

/**
 * Constructor to load a gesture from a JSON file.
 * @param saveFilePath The path to the JSON file
 * @param success Will contain true if the Gesture object was constructed correctly. False can indicate a missing or incorrect file.
 */
    Gesture(const string &saveFilePath, bool &success);

    const string &getId() const;

    HMM *getHiddenMarkovModel() const;

private:
    string id;
    HMM* hiddenMarkovModel;
};


#endif //HMM_GESTURERECOGNITION_GESTURE_H
