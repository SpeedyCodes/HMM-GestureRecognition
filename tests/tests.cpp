#include <iostream>
#include "src/Gesture.h"
#include "src/HMM.h"
#include "src/HMMcomponents/hiddenState.h"
using namespace std;

bool testHiddenState(){
    auto* state1 = new hiddenState(1, 0.75);
    auto* state2 = new hiddenState();
    auto* state3 = new hiddenState();

    if ( state1->id != 1){
        cerr << "ID of state was not set properly" << endl;
        return false;
    }
    if ( state1->initialChance != 0.75){
        cerr << "initial chance of state was not set properly" << endl;
        return false;
    }

    state1->transitionMap = {{state1,0.6},{state2,0.2},{state3,0.3}};
    state1->emissionMap= {{0,0.5},{1,0.5}};
    if (state1->isValid()){
        cerr << "state shows as valid when being invalid" << endl;
        return false;
    }
    state1->transitionMap[state1] = 0.5;
    state1->emissionMap[0] = 0.7;
    if (state1->isValid()){
        cerr << "state shows as valid when being invalid" << endl;
        return false;
    }
    state1->emissionMap[0] = 0.5;
    if (!state1->isValid()){
        cerr << "state shows as invalid when being valid" << endl;
        return false;
    }
    return true;
}

bool testHMM(){
    bool success = false;
    HMM hmm = HMM("../tests/testHMM.json", success);
    if (!success){
        cerr << "generating HMM from json failed" << endl;
        return false;
    }
    vector<Observable> observables = {5,10};
    if (observables != hmm.getObservables()){
        cerr << "observables weren't read in properly" << endl;
        return false;
    }
    if (hmm.getHiddenStates().size() != 1){
        cerr << "hidden states weren't read in properly" << endl;
        return false;
    }
    hiddenState* state = hmm.getHiddenStates()[0];
    if (state->id != 7){
        cerr << "hidden state id wasn't read in properly" << endl;
        return false;
    }
    if (state->transitionMap[state] != 1){
        cerr << "transition probability wasn't read in properly" << endl;
        return false;
    }
    if (state->emissionMap[5] != 0.1 || state->emissionMap[10] != 0.9){
        cerr << "emission probabilities weren't read in properly" << endl;
        return false;
    }
    return true;
}

bool testGesture() {
    bool success = false;
    Gesture gesture = Gesture("../tests/testGesture.json", success);
    if (!success){
        cerr << "generating gesture form json failed" << endl;
        return false;
    }
    if(gesture.getId() != "wave"){
        cerr << "gestureID wasn't read in properly" << endl;
        return false;
    }
    HMM* gestureHMM = gesture.getHiddenMarkovModel();
    vector<Observable> observables = {0,1};
    if (observables != gestureHMM->getObservables()){
        cerr << "observables weren't read in properly" << endl;
        return false;
    }
    if (gestureHMM->getHiddenStates().size() != 1){
        cerr << "hidden states weren't read in properly" << endl;
        return false;
    }
    hiddenState* state = gestureHMM->getHiddenStates()[0];
    if (state->id != 0){
        cerr << "hidden state id wasn't read in properly" << endl;
        return false;
    }
    if (state->transitionMap[state] != 1){
        cerr << "transition probability wasn't read in properly" << endl;
        return false;
    }
    if (state->emissionMap[0] != 0.7 || state->emissionMap[1] != 0.3){
        cerr << "emission probabilities weren't read in properly" << endl;
        return false;
    }
    HMM* hmm2 = new HMM("../tests/testHMM.json", success);
    gesture.setHiddenMarkovModel(hmm2);
    if (gesture.getHiddenMarkovModel() != hmm2){
        cerr << "the gesture's HMM wasn't set properly" << endl;
        return false;
    }
    return true;
}

bool testBaumWelch1(){

    // Set corpus
    std::vector<Observable > abba = {0,1,1,0};
    std::vector<Observable > bab = {1,0,1};
    std::vector<std::vector<Observable >> corpus(10, abba);
    std::vector<std::vector<Observable >> corpus2(20, bab);
    corpus.insert(corpus.end(), corpus2.begin(), corpus2.end());

    // Set first HMM
    hiddenState* s = new hiddenState(1, 0.85);
    hiddenState* t= new hiddenState(2, 0.15);
    s->emissionMap = {{0,0.4}, {1,0.6}};
    t->emissionMap = {{0,0.5}, {1,0.5}};
    s->transitionMap = {{s, 0.3}, {t, 0.7}};
    t->transitionMap = {{s, 0.1}, {t, 0.9}};
    HMM hmm({s,t}, {0,1});

    // Train
    hmm.autoTrain(corpus);
    hmm.autoTrain(corpus); // Number of iterations should be 1
    // Check manually
    return true;
}

int main(){
    if(testBaumWelch1()){
        std::cout << "Yay" << std::endl;
    }
    if (testHiddenState() && testHMM() && testGesture()){
        cout << "all tests passed" << endl;
    }
    else {
        cerr << "not all tests have passed" << endl;
    }
    return 0;
}