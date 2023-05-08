#include "Gesture.h"
#include <fstream>
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

Gesture::Gesture(const string &saveFilePath, bool &success) {
    // Parse the json
    using json = nlohmann::json;
    ifstream f(saveFilePath);
    json data;
    try {
        data = json::parse(f);
    } catch (json::exception& exception){
        cout << exception.what();
        success = false;
        return;
    }

    //Read the gesture ID
    id = data["gestureID"].get<string>();

    //Read the HMM data
    auto& hmmData = data["HMM"];
    map<int, hiddenState*> hiddenStates;
    map<int, Observable> observables;
    for (auto& observableData : hmmData["observables"]) {
        int observableId = observableData.get<int>();
        observables[observableId] = observableId;
    }
    for(auto& hiddenStateData : hmmData["hiddenStates"]){
        int id = hiddenStateData["id"].get<int>();
        hiddenState* hiddenstate = new hiddenState();
        hiddenstate->id = id;
        hiddenStates[id] = hiddenstate;
    }
    for(auto& hiddenStateData : hmmData["hiddenStates"]){
        int id = hiddenStateData["id"].get<int>();
        hiddenState *hiddenstate = hiddenStates[id];
        hiddenstate->initialChance = hiddenStateData["initialProbability"].get<double>();
        for (auto& transitionData: hiddenStateData["transitions"]) {
            hiddenstate->transitionMap.insert({hiddenStates.at(transitionData["id"].get<int>()),
                                               transitionData["probability"].get<double>()});
        }
        for (auto& emissionData: hiddenStateData["emissions"]) {
            hiddenstate->emissionMap.insert({observables.at(emissionData["id"].get<int>()),
                                             emissionData["probability"].get<double>()});
        }
    }
    vector<hiddenState*> hiddenStatesFinal;
    vector<Observable> observablesFinal;
    for(auto& state: hiddenStates) {
        hiddenStatesFinal.push_back(state.second);
    }
    for(auto& observable: observables){
        observablesFinal.push_back(observable.second);
    }
    hiddenMarkovModel = new HMM(hiddenStatesFinal, observablesFinal);
    //json j = *hiddenMarkovModel;

    success = true;
}
