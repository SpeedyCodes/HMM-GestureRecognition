#include "HMM.h"

HMM::HMM(const vector<hiddenState*> &hiddenStates, const vector<Observable*> &observables) : hiddenStates(hiddenStates),
                                                                                observables(observables) {
    if (!checkValues()){
        cout << " probabilities do not add up to 1 for a certain state" << endl;
    }
}
bool HMM::checkValues() {
    for(auto state:hiddenStates){
        if (!state->isValid()){
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