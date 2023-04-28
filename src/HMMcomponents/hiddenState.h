#ifndef HMM_GESTURERECOGNITION_HIDDENSTATE_H
#define HMM_GESTURERECOGNITION_HIDDENSTATE_H

#include <map>
#include "Observable.h"

using namespace std;

class hiddenState {
public:
    int id;
    map<hiddenState*,double> transitionMap;
    map<Observable*,double > emissionMap;
    double initialChance;

    hiddenState();

    hiddenState(int id, const map<hiddenState*, double> &transitionMap, const map<Observable*, double> &emissionMap,
                double initialChance);


    /**
     * Checks if the transition and emission probabilities both add up to 1
     * @return True if the sum of both types of probabilities add up to 1
     */
    bool isValid();
};


#endif //HMM_GESTURERECOGNITION_HIDDENSTATE_H
