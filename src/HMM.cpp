#include "HMM.h"

HMM::HMM(const vector<hiddenState*> &hiddenStates, const vector<Observable*> &observables) : hiddenStates(hiddenStates),
                                                                                observables(observables) {
    checkValues();
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
