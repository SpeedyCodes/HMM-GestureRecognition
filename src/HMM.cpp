#include <fstream>
#include "HMM.h"
#include "algorithm"
#include "json.hpp"

HMM::HMM(const vector<hiddenState*> &hiddenStates, const vector<Observable> &observables) : hiddenStates(hiddenStates),
                                                                                observables(observables) {
    checkValues();
}

HMM::~HMM() {
    for (auto state:hiddenStates){
        delete state;
    }
}

HMM::HMM(const string &saveFilePath, bool &success){
    //parse json
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

    //read the data
    map<int, hiddenState*> hiddenStates;
    vector<Observable> observables;
    for (auto& observable : data["observables"]) {
        int id = observable.get<int>();
        observables.push_back(id);
    }
    for(auto& hiddenStateData : data["hiddenStates"]){
        int id = hiddenStateData["id"].get<int>();
        hiddenState* hiddenstate = new hiddenState();
        hiddenstate->id = id;
        hiddenStates[id] = hiddenstate;
    }
    for(auto& hiddenStateData : data["hiddenStates"]){
        int id = hiddenStateData["id"].get<int>();
        hiddenState *hiddenstate = hiddenStates[id];
        hiddenstate->initialChance = hiddenStateData["initialProbability"].get<double>();
        for (auto& transitionData: hiddenStateData["transitions"]) {
            hiddenstate->transitionMap.insert({hiddenStates.at(transitionData["id"].get<int>()),
                                               transitionData["probability"].get<double>()});
        }
        for (auto& emissionData: hiddenStateData["emissions"]) {
            hiddenstate->emissionMap.insert({emissionData["id"].get<int>(),
                                             emissionData["probability"].get<double>()});
        }
    }
    for(auto& state: hiddenStates) {
        this->hiddenStates.push_back(state.second);
    }
    this->observables = observables;
    success = checkValues();
}

bool HMM::checkValues() {
    bool success = true;
    double sumInitialChances = 0;
    for(auto state:hiddenStates){
        map<Observable, double> emissionMap = state->emissionMap;
        for (int observable:observables){
            if (emissionMap.find(observable) == emissionMap.end()){
                cerr << "a certain state doesn't have an emission probability for observable " << observable << endl;
                success = false;
                break;
            }
        }
        if (!success) break;
    }
    for(auto state:hiddenStates){
        if (!state->isValid()){
            cerr << "probabilities do not add up to 1 for a certain state" << endl;
            success = false;
            break;
        }
    }
    for(auto state:hiddenStates){
        sumInitialChances += state->initialChance;
    }
    if (abs(sumInitialChances - 1) >= 1e-12){
        cerr << "initial probabilities do not add up to 1 for the HMM" << endl;
        success = false;
    }
    return true;
}

double HMM::likelihood(std::vector<Observable>& observations){

    std::vector<std::pair<hiddenState*, double>>previousAlpha;
    for(int i = 0; i<observations.size(); i++){
        std::vector<std::pair<hiddenState*, double>>alpha;
        std::pair<hiddenState*, double>elem;
        for(int j = 0; j<this->hiddenStates.size(); j++){
            double getal = 0;
            if(i == 0){
                getal = this->hiddenStates[j]->initialChance*this->hiddenStates[j]->emissionMap[observations[i]];
                elem.first = this->hiddenStates[j];
                elem.second = getal;
                alpha.push_back(elem);
            }
            else{
                for(auto piece: previousAlpha){
                    getal += piece.second * piece.first->transitionMap[this->hiddenStates[j]]*this->hiddenStates[j]->emissionMap[observations[i]];
                }
                elem.first = this->hiddenStates[j];
                elem.second = getal;
                alpha.push_back(elem);
            }
        }
        previousAlpha.clear();
        previousAlpha = alpha;
        alpha.clear();
    }
    double totalChance = 0;
    for(auto chance: previousAlpha){
        totalChance += chance.second;
    }
    return totalChance;
}

double HMM::likelihood(std::vector<std::vector<Observable>> observations) {
    double totalChance = 0;
    for (auto& observationVector:observations){
        totalChance += likelihood(observationVector);
    }
    totalChance /= observations.size();
    return totalChance;
}

bool HMM::train(const vector<Observable> &data, int iterations) {
    if (data.empty()){
        cerr << "No data was given to train with" << endl;
        return false;
    }
    for (Observable observable: data) {                                                         //TODO als er unregognized observables zijn, moet het direct stoppen of gwn deze observables er uit halen?
        if(find(observables.begin(), observables.end(), observable) == observables.end()){
            cerr << "Data contains some unrecognized observables" << endl;
            return false;
        }
    }

    int M = hiddenStates.size();
    int T = data.size();

    for (int iteration = 0; iteration != iterations; iteration++){
        vector<vector<double>> alpha = calculateAlpha(data);
        vector<vector<double>> beta = calculateBeta(data);

        vector<vector<vector<double>>> xi;
        for (int i = 0; i != M;i++){
            vector<vector<double>> tempVector1;
            for (int j = 0; j != M; j++){
                vector<double> tempVector2;
                for (int k = 0; k != T-1;k++){
                    tempVector2.push_back(0);
                }
                tempVector1.push_back(tempVector2);
            }
            xi.push_back(tempVector1);
        }

        for (int t = 0; t!= T-1; t++){
            double denominator = calculateDenominator(data,alpha,beta,t);
            for (int i = 0; i != M; i++){
                for (int j = 0; j != M; j++){
                    double numerator = alpha[t][i] * hiddenStates[i]->transitionMap[hiddenStates[j]] * hiddenStates[j]->emissionMap[data[t+1]] * beta[t+1][j];
                    xi[i][j][t]= numerator/denominator;
                }
            }
        }
        vector<vector<double>> gamma;
        for (const auto& v1:xi){
            vector<double> tempVec;
            for (int i = 0; i != T-1; i++){
                double tempVal = 0;
                for (const auto& v2:v1){
                    tempVal+= v2[i];
                }
                tempVec.push_back(tempVal);
            }
            gamma.push_back(tempVec);
        }
        vector<vector<double>> sumXi2;
        for (const auto& v1:xi) {
            vector<double> tempVec;
            for (const auto& v2:v1){
                double tempVal = 0;
                for(const auto& val:v2){
                    tempVal += val;
                }
                tempVec.push_back(tempVal);
            }
            sumXi2.push_back(tempVec);
        }
        vector<double> sumGamma1;
        for(const auto& v1:gamma){
            double newVal= 0;
            for (const auto& val:v1){
                newVal += val;
            }
            sumGamma1.push_back(newVal);
        }
        for (int i = 0; i != hiddenStates.size(); i++){
            for (int j = 0; j != hiddenStates.size(); j++){
                hiddenStates[i]->transitionMap[hiddenStates[j]] = sumXi2[i][j] / sumGamma1[i];
            }
        }

        for(int i = 0; i != xi.size(); i++){
            double newVal = 0;
            for (const auto& v1:xi) {
                newVal += v1[i][T - 2];
            }
            gamma[i].push_back(newVal);
        }

        vector<double> denominator;
        for (int i = 0; i!= gamma.size();i++){
            double newVal = 0;
            for (const auto& val:gamma[i]){
                newVal += val;
            }
            denominator.push_back(newVal);
        }


        for (int l = 0; l != observables.size();l++){
            Observable observable = observables[l];
            for (int i = 0; i != hiddenStates.size(); i++){
                double newVal = 0;
                for (int j = 0; j != data.size(); j++){
                    if (data[j]==observable){
                           newVal += gamma[i][j];
                    }
                }
                hiddenStates[i]->emissionMap[observable] = newVal;
            }
        }

        for (int i = 0; i != hiddenStates.size(); i++){
            for(int j = 0; j != observables.size(); j++){
                hiddenStates[i]->emissionMap[observables[j]] = hiddenStates[i]->emissionMap[observables[j]]/denominator[i];
            }
        }
    }
    return checkValues();
}
bool HMM::betaTrain(const vector<vector<Observable>> &dataVector){
    std::vector<vector<vector<double>>> vector_sumXi2;
    std::vector<std::vector<double>> vector_sumGamma1;
    std::vector<vector<vector<vector<double>>>> vector_xi;
    std::vector<vector<vector<double>>> vector_gamma;
    std::vector<std::vector<double>> vector_denominator;
    int M = hiddenStates.size();
    // First loop: get xi, gamma, sumXi2 and sumGamma1 for each vector of observables
    for (auto data: dataVector) {
        if (data.empty()) {
            cerr << "No data was given to train with" << endl;
            return false;
        }
        for (Observable observable: data) {                                                         //TODO als er unregognized observables zijn, moet het direct stoppen of gwn deze observables er uit halen?
            if (find(observables.begin(), observables.end(), observable) == observables.end()) {
                cerr << "Data contains some unrecognized observables" << endl;
                return false;
            }
        }
        int T = data.size();

        vector<vector<double>> alpha = calculateAlpha(data);
        vector<vector<double>> beta = calculateBeta(data);

        vector<vector<vector<double>>> xi;
        for (int i = 0; i != M; i++) {
            vector<vector<double>> tempVector1;
            for (int j = 0; j != M; j++) {
                vector<double> tempVector2;
                for (int k = 0; k != T - 1; k++) {
                    tempVector2.push_back(0);
                }
                tempVector1.push_back(tempVector2);
            }
            xi.push_back(tempVector1);
        }

        for (int t = 0; t != T - 1; t++) {
            double denominator = calculateDenominator(data, alpha, beta, t);
            for (int i = 0; i != M; i++) {
                for (int j = 0; j != M; j++) {
                    double numerator = alpha[t][i] * hiddenStates[i]->transitionMap[hiddenStates[j]] *
                                       hiddenStates[j]->emissionMap[data[t + 1]] * beta[t + 1][j];
                    xi[i][j][t] = numerator / denominator;
                }
            }
        }
        vector<vector<double>> gamma;
        for (const auto &v1: xi) {
            vector<double> tempVec;
            for (int i = 0; i != T - 1; i++) {
                double tempVal = 0;
                for (const auto &v2: v1) {
                    tempVal += v2[i];
                }
                tempVec.push_back(tempVal);
            }
            gamma.push_back(tempVec);
        }

        vector<vector<double>> sumXi2;
        for (const auto &v1: xi) {
            vector<double> tempVec;
            for (const auto &v2: v1) {
                double tempVal = 0;
                for (const auto &val: v2) {
                    tempVal += val;
                }
                tempVec.push_back(tempVal);
            }
            sumXi2.push_back(tempVec);
        }
        vector<double> sumGamma1;
        for (const auto &v1: gamma) {
            double newVal = 0;
            for (const auto &val: v1) {
                newVal += val;
            }
            sumGamma1.push_back(newVal);
        }
        vector_xi.push_back(xi);
        vector_gamma.push_back(gamma);
        vector_sumGamma1.push_back(sumGamma1);
        vector_sumXi2.push_back(sumXi2);
    }
    // Calculate sum of sumXi2 and sumGamma1
    vector<double> f_zero(hiddenStates.size(), 0);
    vector<vector<double>> sumXi2(hiddenStates.size(), f_zero); // TODO: set here zero's
    for(auto Xi2:vector_sumXi2){
        for (int i = 0; i != hiddenStates.size(); i++){
            for (int j = 0; j != hiddenStates.size(); j++){
                sumXi2[i][j] += Xi2[i][j];
            }
        }
    }
    std::vector<double> sumGamma1(hiddenStates.size(), 0); // TODO: zero's
    for(auto SG: vector_sumGamma1){
        for(int i = 0; i != hiddenStates.size(); i++){
            sumGamma1[i] += SG[i];
        }
    }
    // Iterate
    for (int i = 0; i != hiddenStates.size(); i++){
        for (int j = 0; j != hiddenStates.size(); j++){
            hiddenStates[i]->transitionMap[hiddenStates[j]] = sumXi2[i][j] / sumGamma1[i];
        }
    }

    // Set all observ to zero's
    for(int i = 0; i != hiddenStates.size(); i++){
        for(auto observ: observables){
            hiddenStates[i]->emissionMap[observ] = 0;
        }
    }
    int iter = 0;
    for(auto data:dataVector) {
        vector<vector<vector<double>>> xi = vector_xi[iter];
        vector<vector<double>> gamma = vector_gamma[iter];
        int T = data.size();
        // Adjust gamma's and xi's
        for (int i = 0; i != xi.size(); i++) {
            double newVal = 0;
            for (const auto &v1: xi) {
                newVal += v1[i][T - 2];
            }
            gamma[i].push_back(newVal);
        }

        vector<double> denominator;
        for (int i = 0; i != gamma.size(); i++) {
            double newVal = 0;
            for (const auto &val: gamma[i]) {
                newVal += val;
            }
            denominator.push_back(newVal);
        }
        vector_gamma[iter] = gamma;
        vector_denominator.push_back(denominator);
        iter++;
        for (int l = 0; l != observables.size();l++){
            Observable observable = observables[l];
            for (int i = 0; i != hiddenStates.size(); i++){
                double newVal = 0;
                for (int j = 0; j != data.size(); j++){
                    if (data[j]==observable){
                        newVal += gamma[i][j];
                    }
                }
                hiddenStates[i]->emissionMap[observable] += newVal;
            }
        }
    }
        // Sum all and denominators
    std::vector<double> denominator(hiddenStates.size(), 0);
    for(auto denom:vector_denominator){
        for(int i = 0; i != hiddenStates.size(); i++){
            denominator[i] += denom[i];
        }
    }
        // Just do this loop
        for (int i = 0; i != hiddenStates.size(); i++){
            for(int j = 0; j != observables.size(); j++){
                hiddenStates[i]->emissionMap[observables[j]] = hiddenStates[i]->emissionMap[observables[j]]/denominator[i];
            }
        }

    return checkValues();
}

bool HMM::train(const vector<vector<Observable>> &dataVector, int iterations) {
    bool success;
    for(int i = 0; i != iterations; i++){
        betaTrain(dataVector);
    }
    return true;
}

void HMM::HMMtoJson(string &file){
    nlohmann::json j;
    nlohmann::json j1;
    std::vector<nlohmann::json>hiddenStates;
    for(int i = 0; i<this->hiddenStates.size(); i++){
        nlohmann::json j2;
        j2["id"] = this->hiddenStates[i]->id;
        j2["initialProbability"] = this->hiddenStates[i]->initialChance;
        std::vector<nlohmann::json>transitions;
        map<hiddenState*, double>::iterator it;
        for (it = this->hiddenStates[i]->transitionMap.begin(); it != this->hiddenStates[i]->transitionMap.end(); it++){
            nlohmann::json j3;
            j3["id"] = it->first->id;
            j3["probability"] = it->second;
            transitions.push_back(j3);
        }
        j2["transitions"] = transitions;

        map<Observable, double>::iterator it2;
        std::vector<nlohmann::json>emissions;
        for(it2 = this->hiddenStates[i]->emissionMap.begin(); it2 != this->hiddenStates[i]->emissionMap.end(); it2++){
            nlohmann::json j4;
            j4["id"] = it2->first;
            j4["probability"] = it2->second;
            emissions.push_back(j4);
        }
        j2["emissions"] = emissions;
        hiddenStates.push_back(j2);
    }
    j["hiddenStates"] = hiddenStates;
    j["observables"] = observables;
    std::cout<<std::setw(4)<<j<<std::endl;
    std::ofstream stream;
    stream.open(file);
    stream<<std::setw(4)<<j<<std::endl;
    stream.close();
}

vector<vector<double>> HMM::calculateAlpha(const vector<Observable>& data) {
    vector<vector<double>> alpha;
    for (int i = 0; i != data.size(); i++){
        vector<double> tempVector;
        for (int j = 0; j != hiddenStates.size();j++){
            tempVector.push_back(0);
        }
        alpha.push_back(tempVector);
    }

    for (int i = 0; i != hiddenStates.size();i++){
        alpha[0][i] = hiddenStates[i]->initialChance * hiddenStates[i]->emissionMap[data[0]];
    }

    for (int t = 1; t != data.size();t++){
        for (int j = 0; j != hiddenStates.size();j++){
            double newValue = 0;
            int index = 0;
            for (int i = 0; i != hiddenStates.size(); i++){
                newValue += alpha[t-1][index] * hiddenStates[i]->transitionMap[hiddenStates[j]];
                index++;
            }
            alpha[t][j] = newValue * hiddenStates[j]->emissionMap[data[t]];
        }
    }
    return alpha;
}

vector<vector<double>> HMM::calculateBeta(const vector<Observable> &data) {
    vector<vector<double>> beta;

    for (int i = 0; i != data.size(); i++){
        vector<double> tempVector;
        for (int j = 0; j != hiddenStates.size();j++){
            tempVector.push_back(0);
        }
        beta.push_back(tempVector);
    }

    for (double & i : beta[data.size()-1]){
        i = 1;
    }

    for (int t = data.size()-2; t >= 0; t--){
        for (int j = 0; j !=  hiddenStates.size(); j++){
            double newValue = 0;
            int index = 0;
            vector<double> temp = beta[t+1];
            for (auto& chance:temp){
                chance = chance * hiddenStates[index]->emissionMap[data[t+1]];
                index++;
            }
            for (index=0;index != hiddenStates.size();index++){
                newValue += temp[index] * hiddenStates[j]->transitionMap[hiddenStates[index]];
            }
            beta[t][j] = newValue;
        }
    }
    return beta;
}

double HMM::calculateDenominator(const vector<Observable>& data, vector<vector<double>> &alpha, vector<vector<double>> &beta, int t) {
    double denominator = 0;
    vector<double> dotAlphaA;
    for (int i = 0; i != hiddenStates.size();i++){
        double value = 0;
        for (int j = 0; j != hiddenStates.size();j++){
            value += alpha[t][j] * hiddenStates[j]->transitionMap[hiddenStates[i]];
        }
        dotAlphaA.push_back(value);
    }
    vector<double> dotAlphaAB;
    for (int i = 0; i!= hiddenStates.size();i++){
        dotAlphaAB.push_back(dotAlphaA[i]*hiddenStates[i]->emissionMap[data[t+1]]);
    }

    for (int i = 0; i!= hiddenStates.size();i++){
        denominator += dotAlphaAB[i] * beta[t+1][i];
    }

    return denominator;
}

void HMM::print() {
    cout << "transition probabilities:" << endl;
    for (const auto& state:hiddenStates) {
        for(const auto& pair:state->transitionMap){
            cout << pair.second << " ";
        }
        cout << endl;
    }
    cout << "emission probabilities:" << endl;
    for (const auto& state:hiddenStates) {
        for(const auto& pair:state->emissionMap){
            cout << pair.second << " ";
        }
        cout << endl;
    }
}

const vector<hiddenState *> &HMM::getHiddenStates() const {
    return hiddenStates;
}

bool HMM::autoTrain(const vector<vector<Observable> > &dataVector, double threshold) {
    bool keepTraining = true;
    int iter = 0;
    while (keepTraining) {
        //save old transition and emission chances
        vector<map<hiddenState *, double> > transitionVector;
        vector<map<Observable, double> > emmissionVector;
        for (auto state: hiddenStates) {
            transitionVector.push_back(state->transitionMap);
            emmissionVector.push_back(state->emissionMap);
        }

        //train the HMM
        bool success = false;
        success = train(dataVector, 1);
        if (!success) {
            cerr << "training failed, was there something wrong with the given data?" << endl;
            return false;
        }
        keepTraining = false;
        // check if any transition or emission chances got changed by more than the given threshold
        // if so, continue training with the current data
        // otherwise start training with the next set of data
        for (auto i = 0; i != hiddenStates.size(); i++) {
            for (auto state: hiddenStates) {
                if (abs(hiddenStates[i]->transitionMap[state] - transitionVector[i][state]) >= threshold) {
                    keepTraining = true;
                    break;
                }
            }
            for (auto observable: observables) {
                if (abs(hiddenStates[i]->emissionMap[observable] - emmissionVector[i][observable]) >= threshold) {
                    keepTraining = true;
                    break;
                }
            }
        }
        iter++;
    }
    std::cout << "Number of iterations for autoTrain(): " << iter << std::endl;
    return true;
}

const vector<Observable> &HMM::getObservables() const {
    return observables;
}