#ifndef HMM_GESTURERECOGNITION_GESTURE_H
#define HMM_GESTURERECOGNITION_GESTURE_H

#include <string>
#include "json.hpp"

using namespace std;

class HMM;

/**
 * This class serves as a sort of wrapper for the HMM class when using it to detect gestures.
 * It keeps some gesture-related data and provides some convenience-methods.
 * A Gesture does expose its underlying HMM though.
 */

class Gesture {
public:

/**
 * Constructor to use when making a brand-new gesture with a HMM that has just been trained.
 * @param gestureId The name of the gesture
 * @param hiddenMarkovModel A pointer to the Hidden Markov Model to recognize the gesture
 * @param features Global gesture features
 */

    Gesture(const string &gestureId, HMM *hiddenMarkovModel, std::map<std::string,bool> features = {});

/**
 * Gets the ID of the gesture.
 * @return A reference to the ID.
 */
    const string &getId() const;

/**
 * Gets the Hidden Markov Model associated with the gesture.
 * @return A pointer to the Hidden Markov Model.
 */

    HMM *getHiddenMarkovModel() const;

/**
 * Sets the Hidden Markov Model associated with the gesture.
 * Useful when the HMM has not been trained yet when constructing the gesture.
 * @param hiddenMarkovModel A pointer to the Hidden Markov Model.
 */

    void setHiddenMarkovModel(HMM *hiddenMarkovModel);
/**
 * writes out a JSON of the associated HMM and returns a JSON object to be incorporated into the GestureLibrary JSON file.
 * @param directory the directory to put the HMM JSON in
 * @return a JSON object with relevant gesture data
 */
    nlohmann::json toJson(const string& directory);

    const map<std::string, bool> &getGestureFeatures() const;

    void setGestureFeatures(const map<std::string, bool> &gestureFeatures);

    void setId(const string &id);

    void addGestureFeature(const std::string& featureName, bool value);

    void removeGestureFeature(const std::string& featureName);

private:
    string id;
    HMM* hiddenMarkovModel;
    std::map<std::string,bool> gestureFeatures; // description of the gesture features
};


#endif //HMM_GESTURERECOGNITION_GESTURE_H
