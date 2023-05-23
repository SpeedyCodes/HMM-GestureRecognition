#ifndef HMM_GESTURERECOGNITION_HIDDENSTATE_H
#define HMM_GESTURERECOGNITION_HIDDENSTATE_H

#include <map>
#include "logProbability.h"

using namespace std;

typedef int Observable;

class hiddenState {
public:
    int id;
    map<hiddenState*,logProbability> transitionMap;
    map<Observable,logProbability> emissionMap;
    logProbability initialChance;

    hiddenState();

    hiddenState(int id, logProbability initialChance) : id(id), initialChance(initialChance) {}

    hiddenState(int id, const map<hiddenState *, logProbability> &transitionMap, const map<Observable, logProbability> &emissionMap,
                logProbability initialChance);


    /**
     * Checks if the transition and emission probabilities both add up to 1
     * @return True if the sum of both types of probabilities add up to 1
     */
    bool isValid();
};


#endif //HMM_GESTURERECOGNITION_HIDDENSTATE_H
