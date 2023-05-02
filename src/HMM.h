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

    void train(const vector<int> &data, int iterations);

    void train(const vector<vector<int> >&dataVector, int iterations);

    void print();

private:

    vector<vector<double>> calculateAlpha(const vector<int>& data);

    vector<vector<double>> calculateBeta(const vector<int>& data);

    Observable* getObservable(int id);

    double calculateDenominator(const vector<int> &data, vector<vector<double>> &alpha, vector<vector<double>> &beta, int t);
};


#endif //HMM_GESTURERECOGNITION_HMM_H
