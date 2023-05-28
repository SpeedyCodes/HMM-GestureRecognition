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
 * @param features Global gesture features
 */

    Gesture(const string &gestureId, HMM *hiddenMarkovModel, std::map<std::string,bool> features = {});

/**
 * Constructor to load a gesture from a JSON file.
 * @param saveFilePath The path to the JSON file
 * @param success Will contain true if the Gesture object was constructed correctly. False can indicate a missing or incorrect file.
 */
    Gesture(const string &saveFilePath, bool &success);

    const string &getId() const;

    HMM *getHiddenMarkovModel() const;

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
