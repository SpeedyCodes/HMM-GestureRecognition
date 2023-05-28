#include "hiddenState.h"
#include <cmath>

hiddenState::hiddenState() {}

hiddenState::hiddenState(int id, const map<hiddenState *, logProbability> &transitionMap,
                         const map<Observable, logProbability> &emissionMap, logProbability initialChance) : id(id), transitionMap(
        transitionMap), emissionMap(emissionMap), initialChance(initialChance) {}

bool hiddenState::isValid() const{
    logProbability sum = logProbability::fromRegularProbability(0);
    for (auto& pair:transitionMap){
        sum += pair.second;
    }
    if (abs(sum.toRegularProbability() - 1) >= 1e-12){
        return false;
    }
    sum = logProbability::fromRegularProbability(0);
    for (auto pair:emissionMap){
        sum += pair.second;
    }
    if (abs(sum.toRegularProbability() - 1) >= 1e-12){
        return false;
    }
    return true;
}

hiddenState::hiddenState(const hiddenState& other):id(other.id), transitionMap(other.transitionMap), emissionMap(other.emissionMap), initialChance(other.initialChance){};
