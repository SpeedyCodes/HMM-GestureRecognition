//
// Created by dasha on 08.05.2023.
//

#include "GestureLibrary.h"
#include "HMM.h"
#include "Gesture.h"
#include "HMMcomponents/hiddenState.h"

HMM* GestureLibrary::getThresholdHMM() const{
    std::vector<hiddenState*> allHiddenStates;
    // Collect hidden states from all gestures
    for(std::pair<std::string, Gesture> gesturePair: gestures){
        std::string gestureName = gesturePair.first; // We will use gestureName for new hidden state ids
        Gesture gesture = gesturePair.second; // Get gesture
        HMM* gestureHMM = gesture.getHiddenMarkovModel();
        std::vector<hiddenState*> gestureStates = gestureHMM->getHiddenStates();
        int numberOfStates = gestureStates.size();
        // Traverse all states of the gesture
        for(hiddenState* &state: gestureStates){
            state->id = stoi(gestureName + to_string(state->id)); // Give a new id to the state
            state->initialChance = 0; // Set initial chance to zero
            // Change outgoing transition probabilities
            for(std::pair<hiddenState*, double> transitionPair: state->transitionMap){
                if(transitionPair.first == state || transitionPair.second == 0) continue; // Skip self-loops and zero-transitions
                transitionPair.second = (1 - transitionPair.second)/(numberOfStates - 1);
            }
            allHiddenStates.push_back(state); // Push to allHiddenStates
        }
    }
    // TODO: if real-time recognition works slowly, add relative entropy
    // Get possible observables
    // TODO: if multiple channels: input of channel number instead of magic number 1
    //  (or if only one channel, remove map from possibleObservables)
    std::vector<Observable > observables = possibleObservables.at(1);
    // Create observation map with all zero emission probs
    map<Observable, double> zeroEmissionMap;
    for(Observable observable: observables) zeroEmissionMap.at(observable) = 0;
    // Create transition map for start state of threshold HMM to all ex-gesture states
    map<hiddenState*, double> startStateTransitionMap;
    for(hiddenState* state: allHiddenStates){
        startStateTransitionMap[state] = 1;
    }
    hiddenState* startState = new hiddenState(0, startStateTransitionMap, zeroEmissionMap, 1); // Create start state of threshold HMM
    map<hiddenState*, double> endStateTransitionMap = {{startState, 1}}; // Create transition map from end state to begin
    hiddenState* endState = new hiddenState(0, endStateTransitionMap, zeroEmissionMap, 0); // Create end state
    allHiddenStates.push_back(startState);
    allHiddenStates.push_back(endState);
    HMM* thresholdHMM = new HMM(allHiddenStates, observables);
    return thresholdHMM;
}
