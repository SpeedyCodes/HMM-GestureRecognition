#ifndef HMM_GESTURERECOGNITION_HIDDENSTATE_H
#define HMM_GESTURERECOGNITION_HIDDENSTATE_H

#include <map>
#include "logProbability.h"

using namespace std;

typedef int Observable;

/**
 * Representation of a hidden state of a Hidden Markov Model
 */
struct hiddenState {
    int id;
    map<hiddenState*,logProbability> transitionMap;
    map<Observable,logProbability> emissionMap;
    logProbability initialChance;

/**
 * Initializes an empty hidden state.
 */
    hiddenState();

    hiddenState(const hiddenState& other);
  
/**
 * Initializes a hidden state of which only the initial probability is known
 * @param id The ID to identify the state with in the parent HMM.
 * @param initialChance The initial probability for this state
 */

    hiddenState(int id, logProbability initialChance) : id(id), initialChance(initialChance) {}

/**
 * Initializes a hidden state of which everything is known
 * @param id The ID to identify the state with in the parent HMM.
 * @param transitionMap The probabilities for transitions originating in this state.
 * @param emissionMap The probabilities for emissions originating in this state.
 * @param initialChance  The initial probability for this state
 */

    hiddenState(int id, const map<hiddenState *, logProbability> &transitionMap, const map<Observable, logProbability> &emissionMap,
                logProbability initialChance);

/**
 * Checks if the transition and emission probabilities both add up to 1
 * @return True if the sum of both types of probabilities add up to 1
 */

    bool isValid() const;
};


#endif //HMM_GESTURERECOGNITION_HIDDENSTATE_H
