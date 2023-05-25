#include "GestureLibrary.h"
#include "HMM.h"
#include "Gesture.h"
#include "HMMcomponents/hiddenState.h"
#include <set>
#include <fstream>
#include <QDir>
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
            state->initialChance = logProbability::fromRegularProbability(0); // Set initial chance to zero
            // Change outgoing transition probabilities
            for(std::pair<hiddenState*, logProbability> transitionPair: state->transitionMap){
                // Skip self-loops and zero-transitions
                if(transitionPair.first == state || isinf(transitionPair.second.getValueAsIs())){ // ln(0)=-inf
                    continue;
                }
                transitionPair.second = (logProbability::fromRegularProbability(1) - transitionPair.second)/(numberOfStates - 1);
            }
            allHiddenStates.push_back(state); // Push to allHiddenStates
        }
    }
    // TODO: if real-time recognition works slowly, add relative entropy
    // Get possible observables
    // TODO: if multiple channels: input of channel number instead of magic number 1
    //  (or if only one channel, remove map from possibleObservables)

    std::vector<Observable > observables = possibleObservables.at(0);
    // Create observation map with all zero emission probs
    map<Observable, logProbability> zeroEmissionMap;
    for(Observable observable: observables) zeroEmissionMap[observable] = logProbability::fromRegularProbability(0);
    // Create transition map for start state of threshold HMM to all ex-gesture states
    map<hiddenState*, logProbability> startStateTransitionMap;
    for(hiddenState* state: allHiddenStates){
        startStateTransitionMap[state] = logProbability::fromRegularProbability(1);
    }
    hiddenState* startState = new hiddenState(1, startStateTransitionMap, zeroEmissionMap, logProbability::fromRegularProbability(1)); // Create start state of threshold HMM
    map<hiddenState*, logProbability> endStateTransitionMap = {{startState, logProbability::fromRegularProbability(1)}}; // Create transition map from end state to begin
    hiddenState* endState = new hiddenState(0, endStateTransitionMap, zeroEmissionMap, logProbability::fromRegularProbability(0)); // Create end state
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
    std::pair<std::string, logProbability> probableGesture = recognizeGesture(observables);
    // Get threshold HMM
    if(thresholdHMM == nullptr){
        thresholdHMM = getThresholdHMM();
    }
    // Calculate the likelihood of the threshold HMM
    logProbability threshold = thresholdHMM->likelihood(observables);
    // Remove the first element of the accumulated live feed
    accumulatedLiveFeedData.erase(accumulatedLiveFeedData.begin());
    // Compare the gesture likelihood to threshold
    if(probableGesture.second > threshold) return probableGesture;
    else return "";
}
bool
GestureLibrary::fitAndSelect(std::vector<std::vector<Observable> > GestureData, const std::string &gestureID,
                             int stateAmount,
                             double threshold) {
    //check if the stateAmount given is 3 or larger
    if (stateAmount < 3){
        cerr << "stateAmount cannot be smaller than 3" << endl;
    }
    //check if GestureData contains at least 3 vectors
    if (GestureData.size() < 3){
        cerr << "Not enough data to fit and select a model" << endl;
        return false;
    }
    //check if the given data vectors aren't empty
    for (auto& dataVector:GestureData){
        if (dataVector.empty()){
            cerr << "One of the vectors in the given data is empty" << endl;
            return false;
        }
    }
    //check if there's a gesture with the given gesture ID
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
    map<Observable, logProbability> emissionMap;
    for (int observable: observables) {
        double count = 0;
        for (auto &dataSet: GestureData) {
            for (Observable angle: dataSet) {
                if (observable == angle) count += 1;
            }
        }
        emissionMap[observable] = logProbability::fromRegularProbability(count / totalAmntOfAngles);
    }

    //calculate which HMM would have the highest success rate
    vector<logProbability> successRates;
    for (int i = 2; i != stateAmount; i++){
        successRates.push_back(logProbability::fromRegularProbability(0));
    }
    for (int i = 1; i != 4; i++){
        //create test HMMs with their respective amount of hidden states
        vector<HMM*> HMMvector;
        for (int states = 3; states != stateAmount + 1; states++){
            HMMvector.push_back(createHMM(emissionMap,observables,states));
        }

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
        for (int hmm = 0; hmm != stateAmount - 2; hmm++){
            HMMvector[hmm]->autoTrain(trainingVector,threshold);
            successRates[hmm] += HMMvector[hmm]->likelihood(testLikelihoodVector);
        }

        //delete test HMMs
        for (auto hmm:HMMvector){
            delete hmm;
        }
    }
    logProbability highestSuccessRate = logProbability::fromRegularProbability(0);
    int stateAmountHighestSuccess = 0;
    for (int i = 0; i != stateAmount - 2; i++){
        if (successRates[i] > highestSuccessRate){
            highestSuccessRate = successRates[i];
            stateAmountHighestSuccess = i;
        }
    }
    stateAmountHighestSuccess += 3;

    //create and train the HMM with the highest success rate and assign it to the gesture
    HMM* newHMM = createHMM(emissionMap,observables,stateAmountHighestSuccess);
    newHMM->autoTrain(GestureData, threshold);
    cout << "state amount: " << stateAmountHighestSuccess << endl;  //TODO deze feedback er uit halen?
    cout << "success rate: " << newHMM->likelihood(GestureData).toRegularProbability() << endl;
    gestures.at(gestureID).setHiddenMarkovModel(newHMM);

    return true;
}

HMM *GestureLibrary::createHMM(const map<Observable, logProbability> &emissionMap, const vector<Observable> &observables, int states) {
    //create hidden states
    //initial probability of the first/"left most state" is 1, the rest is 0
    vector<hiddenState *> hiddenStates;
    hiddenStates.push_back(new hiddenState(0,logProbability::fromRegularProbability(1)));
    for (int i = 1; i != states; i++){
        hiddenStates.push_back(new hiddenState(i,logProbability::fromRegularProbability(0)));
    }

    //assign transitionMap to all hidden states
    //chance to transition to itself is 80%, to "the next state" is 20% and to all other states is 0%
    //the "final state" has no "next state" so it can only transition to itself, therefor the chance to transition to itself is 100%
    for (int i = 0; i != states-1; i++){
        for (auto& state:hiddenStates){
            if (hiddenStates[i] == state){
                hiddenStates[i]->transitionMap[state] = logProbability::fromRegularProbability(0.8);
            }
            else if (hiddenStates[i+1] == state){
                hiddenStates[i]->transitionMap[state] = logProbability::fromRegularProbability(0.2);
            }
            else {
                hiddenStates[i]->transitionMap[state] = logProbability::fromRegularProbability(0);
            }
        }
    }
    for (int i = 0; i != states-1; i++){
        hiddenStates[states-1]->transitionMap[hiddenStates[i]] = logProbability::fromRegularProbability(0);
    }
    hiddenStates[states-1]->transitionMap[hiddenStates[states-1]] = logProbability::fromRegularProbability(1);

    //assign emissionMaps to all hidden states
    for (auto hiddenState: hiddenStates) { hiddenState->emissionMap = emissionMap; }

    //create HMM and train it
    HMM* nStateHMM = new HMM(hiddenStates, observables);
    return nStateHMM;
}

bool GestureLibrary::addGesture(string &gestureID) {
    gestures.insert({gestureID, Gesture(gestureID, nullptr)});
    return false;
}

GestureLibrary::GestureLibrary(std::string& path){
    if(path!=""){
        readIn(path);
    }
}

void GestureLibrary::updateSavedGestures() const {
    assert(directory != "" && name != "");
    using json = nlohmann::json;
    string fullPath = QDir(QString::fromStdString(directory)).filePath(QString::fromStdString(name)).toStdString();
    ifstream f(fullPath);
    json data;
    try {
        data = json::parse(f);
    } catch (json::exception& exception){
        cout << exception.what();
        return;
    }
    for(auto gesture : gestures){
        if(!data["gestures"].contains(gesture.first)){
            //TODO cleanup: don't use a function that does two things that have so little to do with eachother
            data["gestures"][gesture.first] = gesture.second.toJson(directory);
        }
    }
    std::ofstream stream;
    stream.open(fullPath);
    stream<<std::setw(4)<<data<<std::endl;
    stream.close();
}

bool GestureLibrary::initiateFileSystem(const string &path) {
    using json = nlohmann::json;
    directory = QFileInfo(QString::fromStdString(path)).dir().absolutePath().toStdString();
    name = QFileInfo(QString::fromStdString(path)).fileName().toStdString();
    json data;
    data["gestures"] = json(json::value_t::object);//empty object to be used as map
    std::ofstream stream; //TODO helper method for json-to-file
    stream.open(path);
    stream<<std::setw(4)<<data<<std::endl;
    stream.close();
    return true; //TODO handle invalid directories/filenames
}

bool GestureLibrary::isFileSystemInitiated() const {
    return !directory.empty();
}

void GestureLibrary::readIn(const string &path) {
    directory = QFileInfo(QString::fromStdString(path)).dir().absolutePath().toStdString();
    name = QFileInfo(QString::fromStdString(path)).fileName().toStdString();
    using json = nlohmann::json;
    ifstream f(path);
    json data;
    try {
        data = json::parse(f);
    } catch (json::exception& exception){
        cerr << exception.what();
        return;
    }
    for (const auto& item : data["gestures"].items())
    {
        bool success;
        const string &relativeJsonPath = item.value()["HMMpath"].get<string>();
        const string absoluteJsonPath = QDir(QString::fromStdString(directory)).filePath(QString::fromStdString(relativeJsonPath)).toStdString();
        gestures.insert({item.key(), Gesture(item.key(), new HMM(absoluteJsonPath, success))});
        if(!success) cerr << "Gesture " << item.key() << "points to an invalid HMM JSON file" << endl;
    }
}


const std::map<std::string, Gesture> &GestureLibrary::getGestures() const {
    return gestures;
}

std::string GestureLibrary::recognizeFromVideo(const char* AbsolutePath, MediapipeInterface* interface){
    std::vector<std::vector<double>> landmarks = interface->getLandmarksFromVideo(AbsolutePath);
    std::vector<int> data = MediapipeInterface::preprocessData(landmarks);
    std::pair<std::string, logProbability>gesture = recognizeGesture(data);
    return gesture.first;
}

std::pair<std::string, logProbability> GestureLibrary::recognizeGesture(vector<int>& observed){
    std::map<std::string, logProbability>likelyhoodHMM;
    std::map<std::string, Gesture>::iterator it;
    for (it = gestures.begin(); it != gestures.end(); it++){
        logProbability likely = it->second.getHiddenMarkovModel()->likelihood(observed);
        likelyhoodHMM[it->first] = likely;
    }
    std::pair<std::string, logProbability>gesture;
    gesture.first = likelyhoodHMM.begin()->first;
    gesture.second = likelyhoodHMM.begin()->second;
    std::map<std::string, logProbability>::iterator it2;
    for(it2 = likelyhoodHMM.begin(); it2!=likelyhoodHMM.end(); it2++){
        if(it2->second > gesture.second){
            gesture.first = it2->first;
            gesture.second = it2->second;
        }
    }
    return gesture;
}