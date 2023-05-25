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

    const string &getId() const;

    HMM *getHiddenMarkovModel() const;

    void setHiddenMarkovModel(HMM *hiddenMarkovModel);
/**
 * writes out a JSON of the associated HMM and returns a JSON object to be incorporated into the GestureLibrary JSON file.
 * @param directory the directory to put the HMM JSON in
 * @return a JSON object with relevant gesture data
 */
    nlohmann::json toJson(const string& directory);

private:
    string id;
    HMM* hiddenMarkovModel;
};


#endif //HMM_GESTURERECOGNITION_GESTURE_H
