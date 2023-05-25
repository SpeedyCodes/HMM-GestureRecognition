#include "Gesture.h"
#include <iostream>
#include "HMM.h"

Gesture::Gesture(const string &gestureId, HMM *hiddenMarkovModel) : id(gestureId),
                                                                    hiddenMarkovModel(hiddenMarkovModel) {}

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
    return output;
}
