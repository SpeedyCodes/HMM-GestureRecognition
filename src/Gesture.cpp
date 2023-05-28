#include "Gesture.h"
#include <iostream>
#include "HMM.h"

Gesture::Gesture(const string &gestureId, HMM *hiddenMarkovModel, std::map<std::string,bool> features) : id(gestureId),
                                                                    hiddenMarkovModel(hiddenMarkovModel), gestureFeatures(features) {}

const string &Gesture::getId() const {
    return id;
}

HMM *Gesture::getHiddenMarkovModel() const {
    return hiddenMarkovModel;
}

void Gesture::setHiddenMarkovModel(HMM *hiddenMarkovModel) {
    Gesture::hiddenMarkovModel = hiddenMarkovModel;
}

nlohmann::json Gesture::toJson(const string &directory) {
    using json = nlohmann::json;
    json output;
    string path = directory + "/" + id + ".json";
    hiddenMarkovModel->HMMtoJson(path);
    output["HMMpath"] = id + ".json";
    output["features"] = gestureFeatures;
    return output;
}

const map<std::string, bool> &Gesture::getGestureFeatures() const {
    return gestureFeatures;
}

void Gesture::setGestureFeatures(const map<std::string, bool> &gestureFeatures) {
    Gesture::gestureFeatures = gestureFeatures;
}

void Gesture::setId(const string &id) {
    Gesture::id = id;
}

void Gesture::addGestureFeature(const std::string& featureName, bool value){
    gestureFeatures.insert(std::make_pair(featureName, value));
}

void Gesture::removeGestureFeature(const std::string& featureName){
    gestureFeatures.erase(featureName);
}
