#include <fstream>
#include "HMM.h"
#include "json.hpp"

HMM::HMM(const vector<hiddenState*> &hiddenStates, const vector<Observable*> &observables) : hiddenStates(hiddenStates),
                                                                                observables(observables) {
    checkValues();
}

HMM::HMM(const string &saveFilePath, bool success){
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
    map<int, Observable*> observables;
    for (auto& observable : data["observables"]) {
        int id = observable.get<int>();
        observables[id] = new Observable(id);
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
            hiddenstate->emissionMap.insert({observables.at(emissionData["id"].get<int>()),
                                             emissionData["probability"].get<double>()});
        }
    }
    for(auto& state: hiddenStates) {
        this->hiddenStates.push_back(state.second);
    }
    for(auto& observable: observables){
        this->observables.push_back(observable.second);
    }
    success = true;
}



bool HMM::checkValues() {
    for(auto state:hiddenStates){
        if (!state->isValid()){
            cerr << " probabilities do not add up to 1 for a certain state" << endl;
            return false;
        }
    }
    return true;
}

double HMM::likelihood(std::vector<Observable*>observations){

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



void HMM::train(const vector<int> &data, int iterations) {
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
                    double numerator = alpha[t][i] * hiddenStates[i]->transitionMap[hiddenStates[j]] * hiddenStates[j]->emissionMap[getObservable(data[t+1])] * beta[t+1][j];
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
            Observable* observable = observables[l];
            for (int i = 0; i != hiddenStates.size(); i++){
                double newVal = 0;
                for (int j = 0; j != data.size(); j++){
                    if (data[j]==observable->id){
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
    checkValues();
}

void HMM::train(const vector<vector<int>> &dataVector, int iterations) {
    for(const auto& data:dataVector){
        train(data,iterations);
    }
}

vector<vector<double>> HMM::calculateAlpha(const vector<int>& data) {
    vector<vector<double>> alpha;
    for (int i = 0; i != data.size(); i++){
        vector<double> tempVector;
        for (int j = 0; j != hiddenStates.size();j++){
            tempVector.push_back(0);
        }
        alpha.push_back(tempVector);
    }

    for (int i = 0; i != hiddenStates.size();i++){
        alpha[0][i] = hiddenStates[i]->initialChance * hiddenStates[i]->emissionMap[getObservable(data[0])];
    }

    for (int t = 1; t != data.size();t++){
        for (int j = 0; j != hiddenStates.size();j++){
            double newValue = 0;
            int index = 0;
            for (int i = 0; i != hiddenStates.size(); i++){
                newValue += alpha[t-1][index] * hiddenStates[i]->transitionMap[hiddenStates[j]];
                index++;
            }
            alpha[t][j] = newValue * hiddenStates[j]->emissionMap[getObservable(data[t])];
        }
    }
    return alpha;
}

vector<vector<double>> HMM::calculateBeta(const vector<int> &data) {
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
                chance = chance * hiddenStates[index]->emissionMap[getObservable(data[t+1])];
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

Observable *HMM::getObservable(int id) {
    for (auto& i:observables){
        if (i->id==id){
            return i;
        }
    }
    cerr << "obeservable with this id doesnt exist" << endl;
}

double HMM::calculateDenominator(const vector<int>& data, vector<vector<double>> &alpha, vector<vector<double>> &beta, int t) {
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
        dotAlphaAB.push_back(dotAlphaA[i]*hiddenStates[i]->emissionMap[getObservable(data[t+1])]);
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

std::map<int, double>HMM::learnDistributionFromSamples(std::vector<std::vector<int>>samples){
    std::map<int, double> grade;
    int amount = samples.size()*samples[0].size();
    for(int i = 0; i<samples.size(); i++){
        for(int j = 0; j<samples[i].size(); j++){
            if(grade.count(samples[i][j]) == 0){
                grade[samples[i][j]] = 1;
            }
            else{
                grade[samples[i][j]] += 1;
            }
        }
    }
    map<int, double>::iterator it;
    for (it = grade.begin(); it != grade.end(); it++)
    {
        it->second = it->second/amount;
    }
    return grade;
}


void HMM::HMMtoJson(std::string file){
    nlohmann::json j;
    nlohmann::json j1;
    std::vector<std::string>observables;
    for(int i = 0; i<this->observables.size(); i++){
        observables.push_back(to_string(this->observables[i]->id));
    }
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

        map<Observable*, double>::iterator it2;
        std::vector<nlohmann::json>emissions;
        for(it2 = this->hiddenStates[i]->emissionMap.begin(); it2 != this->hiddenStates[i]->emissionMap.end(); it2++){
            nlohmann::json j4;
            j4["id"] = it2->first->id;
            j4["probability"] = it->second;
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
