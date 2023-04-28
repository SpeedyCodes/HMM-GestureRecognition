#ifndef HMM_GESTURERECOGNITION_HMM_H
#define HMM_GESTURERECOGNITION_HMM_H

#include <iostream>
#include <vector>
#include "src/HMMcomponents/hiddenState.h"
#include "src/HMMcomponents/Observable.h"

class HMM {
vector<hiddenState*> hiddenStates;
vector<Observable*> observables;
public:
    HMM(const vector<hiddenState*> &hiddenStates, const vector<Observable*> &observables);

    bool checkValues();

    double likelihood(std::vector<Observable*>observations);
};


#endif //HMM_GESTURERECOGNITION_HMM_H
