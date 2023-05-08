#include "src/GestureLibrary.h"
#include "src/Gesture.h"
#include "src/HMM.h"

// TODO: rewrite it in gtest
bool testThresholdHMM(){
    GestureLibrary gl;

    map<Observable, double> emissionMap1 = {{0, 0.7}, {1, 0.3}};
    map<Observable, double> emissionMap2 = {{0, 0.5}, {1, 0.5}};
    map<Observable, double> emissionMap3 = {{0, 0.1}, {1, 0.9}};

    hiddenState* h11 = new hiddenState();
    hiddenState* h12 = new hiddenState();
    hiddenState* h13 = new hiddenState();
    hiddenState* h21 = new hiddenState();
    hiddenState* h22 = new hiddenState();
    hiddenState* h23 = new hiddenState();

    h11->transitionMap = {{h11, 0.3}, {h12, 0.7}};
    h12->transitionMap = {{h12, 0.9}, {h13, 0.1}};
    h13->transitionMap = {{h13, 1}};
    h21->transitionMap = {{h21, 0.4}, {h22, 0.6}};
    h22->transitionMap = {{h22, 0.5}, {h23, 0.5}};
    h23->transitionMap = {{h23, 1}};

    h11->emissionMap = emissionMap3;
    for(hiddenState* state: {h12, h22}) state->emissionMap = emissionMap1;
    for(hiddenState* state: {h13, h21, h23}) state->emissionMap = emissionMap2;

    for(hiddenState* state: {h11, h21}) state->initialChance = 1;
    for(hiddenState* state: {h12, h13, h22, h23}) state->initialChance = 0;

    vector<hiddenState*> hiddenStates1 = {h11, h12, h13};
    vector<hiddenState*> hiddenStates2 = {h21, h22, h23};
    vector<Observable> observables = {0,1};

    HMM* hmm1 = new HMM(hiddenStates1, observables);
    HMM* hmm2 = new HMM(hiddenStates2, observables);

    Gesture g1 = Gesture("g1", hmm1);
    Gesture g2 = Gesture("g2", hmm2);

    gl.addGesture(g1);
    gl.addGesture(g2);

    std::map <int, std::vector<Observable > > observablesMap = {{1, {0,1}}};
    gl.setPossibleObservables(observablesMap);

    HMM* thresholdHMM = gl.getThresholdHMM();

    if(thresholdHMM->getHiddenStates().size() == 8) return true;

    return false;
}
