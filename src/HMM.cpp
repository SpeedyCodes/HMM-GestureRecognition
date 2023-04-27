#include "HMM.h"

HMM::HMM(const vector<hiddenState> &hiddenStates, const vector<Observable> &observables) : hiddenStates(hiddenStates),
                                                                                observables(observables) {
    if (!checkValues()){
        cout << " probabilities do not add up to 1 for a certain state" << endl;
    }
}
bool HMM::checkValues() {
    for(auto state:hiddenStates){
        if (!state.isValid()){
            return false;
        }
    }
    return true;
}
