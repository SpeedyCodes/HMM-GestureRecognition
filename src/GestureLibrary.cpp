#include "GestureLibrary.h"
#include "HMM.h"
#include "Gesture.h"
#include "src/utils/MediapipeInterface.h"
#include "HMMcomponents/hiddenState.h"
#include <set>
#include "cmath"

bool GestureLibrary::addGesture(Gesture& gesture){
    gestures.insert(std::make_pair(gesture.getId(), gesture));
    return true;
}

void GestureLibrary::setPossibleObservables(std::map <int, std::vector<Observable > > observables){
    possibleObservables = observables;
}

HMM* GestureLibrary::getThresholdHMM() const{
    std::vector<hiddenState*> allHiddenStates;
    // Collect hidden states from all gestures
    for(std::pair<std::string, Gesture> gesturePair: gestures){
        Gesture gesture = gesturePair.second; // Get gesture
        HMM* gestureHMM = gesture.getHiddenMarkovModel();
        std::vector<hiddenState*> gestureStates = gestureHMM->getHiddenStates();
        int numberOfStates = gestureStates.size();
        // Traverse all states of the gesture
        for(hiddenState* &state: gestureStates){
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
    for(Observable observable: observables) zeroEmissionMap[observable] = 0;
    // Create transition map for start state of threshold HMM to all ex-gesture states
    map<hiddenState*, double> startStateTransitionMap;
    for(hiddenState* state: allHiddenStates){
        startStateTransitionMap[state] = 1;
    }
    hiddenState* startState = new hiddenState(1, startStateTransitionMap, zeroEmissionMap, 1); // Create start state of threshold HMM
    map<hiddenState*, double> endStateTransitionMap = {{startState, 1}}; // Create transition map from end state to begin
    hiddenState* endState = new hiddenState(0, endStateTransitionMap, zeroEmissionMap, 0); // Create end state
    allHiddenStates.push_back(startState);
    allHiddenStates.push_back(endState);
    HMM* thresholdHMM = new HMM(allHiddenStates, observables);
    return thresholdHMM;
}
std::string GestureLibrary::realtimeRecognition(const std::vector<double>& frameLandmarks){
    accumulatedLiveFeedData.push_back(frameLandmarks);
    if(accumulatedLiveFeedData.size() < 5) return "";  // TODO: remove hardcoded sliding window size or find the best value experimentally
    // Preprocess data
    std::vector<Observable > observables = MediapipeInterface::preprocessData(accumulatedLiveFeedData);
    // Get the highest likelihood and the name of the most probable gesture
    double gestureLikelihood = 0;
    std::string probableGesture = recognizeGesture(observables, gestureLikelihood);
    // Get threshold HMM
    HMM* thresholdHMM = getThresholdHMM();
    // Calculate the likelihood of the threshold HMM
    double threshold = thresholdHMM->likelihood(observables);
    // Remove the first element of the accumulated live feed
    accumulatedLiveFeedData.erase(accumulatedLiveFeedData.begin());
    // Compare the gesture likelihood to threshold
    if(gestureLikelihood > threshold) return probableGesture;
    else return "";
}
bool
GestureLibrary::fitAndSelect(std::vector<std::vector<Observable> > GestureData, const std::string &gestureID,
                             double threshold) {
    //check if GestureData contains at least 3 vectors
    if (GestureData.size() < 3){
        cerr << "Not enough data to fit and select a model" << endl;
        return false;
    }
    for (auto dataVector:GestureData){
        if (dataVector.empty()){
            cerr << "One of the vectors in the given data is empty" << endl;
            return false;
        }
    }
    if (gestures.find(gestureID) == gestures.end()){
        cerr << "Library does not contain gesture with ID \"" << gestureID << "\""<< endl;
        return false;
    }

    //fill vector will all possible observables from the given data
    vector<Observable> observables;
    std::set<Observable> temp;
    for (auto& dataVector:GestureData) {
        for(auto angle:dataVector){
            temp.emplace(angle);
        }
    }
    for(auto angle:temp){
        observables.push_back(angle);
    }

    //calculate the amount of individual angles there are in the given data,
    //this will be used to calculate the emission probabilities
    int totalAmntOfAngles = 0;
    for (auto &dataVector: GestureData) {
        totalAmntOfAngles += dataVector.size();
    }

    //calculate the emission probabilities and put them in a map
    //emissionProbability = amount of times the angle appears in the given data / total amount of angles
    map<Observable, double> emissionMap;
    for (int observable: observables) {
        double count = 0;
        for (auto &dataSet: GestureData) {
            for (Observable angle: dataSet) {
                if (observable == angle) count += 1;
            }
        }
        emissionMap[observable] = count / totalAmntOfAngles;
    }

    //calculate which HMM would have the highest success rate
    double successRateThreeStateHMM = 0;
    double successRateFourStateHMM = 0;
    double successRateFiveStateHMM = 0;
    for (int i = 1; i != 4; i++){
        //create test HMMs with their respective amount of hidden states
        HMM* threeStateHMM = createThreeStateHMM(emissionMap,observables);
        HMM* fourStateHMM = createFourStateHMM(emissionMap,observables);
        HMM* fiveStateHMM = createFiveStateHMM(emissionMap,observables);

        //split gestureData: 2/3 to train with, 1/3 to calculate likelihood
        vector<vector<Observable> > trainingVector = GestureData;
        vector<vector<Observable> >::iterator beginIt,endIt;
        if (GestureData.size()%3 == 0){
            beginIt = trainingVector.begin() + (GestureData.size() / 3) * (i - 1);
            endIt = trainingVector.begin() + (GestureData.size() / 3) * i;
        }
        else if (GestureData.size()%3 == 1){
            beginIt = trainingVector.begin() + ::lround((GestureData.size() / 3)-0.5) * (i - 1);
            endIt = trainingVector.begin() + ::lround((GestureData.size() / 3)-0.5) * i;
        }
        else{
            beginIt = trainingVector.begin() + ::lround((GestureData.size() / 3)+0.5) * (i - 1);
            endIt = trainingVector.begin() + ::lround((GestureData.size() / 3)+0.5) * i;
        }
        if (i == 3) endIt = trainingVector.end();
        vector<vector<Observable> > testLikelihoodVector(beginIt, endIt);
        trainingVector.erase(beginIt, endIt);

        //train and test all HMMs
        threeStateHMM->autoTrain(trainingVector, threshold);
        successRateThreeStateHMM += threeStateHMM->likelihood(testLikelihoodVector);

        fourStateHMM->autoTrain(trainingVector, threshold);
        successRateFourStateHMM += fourStateHMM->likelihood(testLikelihoodVector);

        fiveStateHMM->autoTrain(trainingVector, threshold);
        successRateFiveStateHMM += fiveStateHMM->likelihood(testLikelihoodVector);

        //delete test HMMs
        delete threeStateHMM;
        delete fourStateHMM;
        delete fiveStateHMM;
    }

    double highestSuccessRate = max(max(successRateThreeStateHMM,successRateFourStateHMM),successRateFiveStateHMM);

    //create and train the HMM with the highest success rate and assign it to the gesture
    if (successRateThreeStateHMM == highestSuccessRate){
        //create HMM with three states and train it
        HMM* threeStateHMM = createThreeStateHMM(emissionMap,observables);
        threeStateHMM->autoTrain(GestureData, threshold);

        //assign selected model to the gesture
        gestures.at(gestureID).setHiddenMarkovModel(threeStateHMM);
    }
    else if (successRateFourStateHMM == highestSuccessRate){
        //create HMM with four states and train it
        HMM* fourStateHMM = createFourStateHMM(emissionMap,observables);
        fourStateHMM->autoTrain(GestureData, threshold);

        //assign selected model to the gesture
        gestures.at(gestureID).setHiddenMarkovModel(fourStateHMM);
    }
    else {
        //create HMM with five states and train it
        HMM* fiveStateHMM = createFiveStateHMM(emissionMap,observables);
        fiveStateHMM->autoTrain(GestureData, threshold);

        //assign selected model to the gesture
        gestures.at(gestureID).setHiddenMarkovModel(fiveStateHMM);
    }
    return true;
}

HMM* GestureLibrary::createThreeStateHMM(const std::map<Observable, double>& emissionMap,const std::vector<Observable>& observables) {
    //create hidden states
    //initial probability of the first/"left most state" is 1, the rest is 0
    vector<hiddenState *> hiddenStates;
    auto *hmm1o1 = new hiddenState(1, 1);
    auto *hmm1o2 = new hiddenState(2, 0);
    auto *hmm1o3 = new hiddenState(3, 0);

    hiddenStates = {hmm1o1, hmm1o2, hmm1o3};
    //assign transitionMap to all hidden states
    //chance to transition to itself is 80%, to "the next state" is 20% and to all other states is 0%
    //the "final state" has no "next state" so it can only transition to itself, therefor the chance to transition to itself is 100%
    hmm1o1->transitionMap = {{hmm1o1, 0.8},{hmm1o2, 0.2},{hmm1o3, 0}};
    hmm1o2->transitionMap = {{hmm1o1, 0},{hmm1o2, 0.8},{hmm1o3, 0.2}};
    hmm1o3->transitionMap = {{hmm1o1, 0},{hmm1o2, 0},{hmm1o3, 1}};

    //assign emissionMaps to all hidden states
    for (auto hiddenState: hiddenStates) { hiddenState->emissionMap = emissionMap; }

    //create HMM and train it
    HMM* threeStateHMM = new HMM({hmm1o1, hmm1o2, hmm1o3}, observables);
    return threeStateHMM;
}

HMM* GestureLibrary::createFourStateHMM(const std::map<Observable, double>& emissionMap, const std::vector<Observable>& observables) {
    //create hidden states
    //initial probability of the first/"left most state" is 1, the rest is 0
    vector<hiddenState *> hiddenStates;
    auto *hmm2o1 = new hiddenState(1, 1);
    auto *hmm2o2 = new hiddenState(2, 0);
    auto *hmm2o3 = new hiddenState(3, 0);
    auto *hmm2o4 = new hiddenState(4, 0);

    hiddenStates = {hmm2o1, hmm2o2, hmm2o3, hmm2o4};
    //assign transitionMaps to all hidden states
    //chance to transition to itself is 80%, to "the next state" is 20% and to all other states is 0%
    //the "final state" has no "next state" so it can only transition to itself, therefor the chance to transition to itself is 100%
    hmm2o1->transitionMap = {{hmm2o1, 0.8},{hmm2o2, 0.2},{hmm2o3, 0},{hmm2o4, 0}};
    hmm2o2->transitionMap = {{hmm2o1, 0},{hmm2o2, 0.8},{hmm2o3, 0.2},{hmm2o4, 0}};
    hmm2o3->transitionMap = {{hmm2o1, 0},{hmm2o2, 0},{hmm2o3, 0.8},{hmm2o4, 0.2}};
    hmm2o4->transitionMap = {{hmm2o1, 0},{hmm2o2, 0}, {hmm2o3, 0},{hmm2o4, 1}};

    //assign emissionMaps to all hidden states
    for (auto hiddenState: hiddenStates) { hiddenState->emissionMap = emissionMap; }

    //create HMM and train it
    HMM* fourStateHMM = new HMM({hmm2o1, hmm2o2, hmm2o3, hmm2o4}, observables);
    return fourStateHMM;
}

HMM* GestureLibrary::createFiveStateHMM(const std::map<Observable, double>& emissionMap, const std::vector<Observable>& observables) {
    //create hidden states
    //initial probability of the first/"left most state" is 1, the rest is 0
    vector<hiddenState *> hiddenStates;
    auto *hmm3o1 = new hiddenState(1, 1);
    auto *hmm3o2 = new hiddenState(2, 0);
    auto *hmm3o3 = new hiddenState(3, 0);
    auto *hmm3o4 = new hiddenState(4, 0);
    auto *hmm3o5 = new hiddenState(5, 0);

    hiddenStates = {hmm3o1, hmm3o2, hmm3o3, hmm3o4, hmm3o5};
    //assign transitionMaps to all hidden states
    //chance to transition to itself is 80%, to "the next state" is 20% and to all other states is 0%
    //the "final state" has no "next state" so it can only transition to itself, therefor the chance to transition to itself is 100%
    hmm3o1->transitionMap = {{hmm3o1, 0.8},{hmm3o2, 0.2},{hmm3o3, 0},{hmm3o4, 0},{hmm3o5, 0}};
    hmm3o2->transitionMap = {{hmm3o1, 0},{hmm3o2, 0.8},{hmm3o3, 0.2},{hmm3o4, 0},{hmm3o5, 0}};
    hmm3o3->transitionMap = {{hmm3o1, 0},{hmm3o2, 0},{hmm3o3, 0.8},{hmm3o4, 0.2},{hmm3o5, 0}};
    hmm3o4->transitionMap = {{hmm3o1, 0},{hmm3o2, 0},{hmm3o3, 0},{hmm3o4, 0.8},{hmm3o5, 0.2}};
    hmm3o5->transitionMap = {{hmm3o1, 0},{hmm3o2, 0},{hmm3o3, 0},{hmm3o4, 0},{hmm3o5, 1}};

    //assign emissionMaps to all hidden states
    for (auto hiddenState: hiddenStates) { hiddenState->emissionMap = emissionMap; }

    //create HMM and train it
    HMM* fiveStateHMM = new HMM({hmm3o1, hmm3o2, hmm3o3, hmm3o4, hmm3o5}, observables);
    return fiveStateHMM;
}


