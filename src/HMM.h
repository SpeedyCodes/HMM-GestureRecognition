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

    vector<vector<double>> calculateAlpha(const vector<int>& data);

    vector<vector<double>> calculateBeta(const vector<int>& data);

    void train(const vector<int> &data, int iterations);

    Observable* getObservable(int id);

    double calculateDenominator(const vector<int> &data, vector<vector<double>> &alpha, vector<vector<double>> &beta, int t);

    void print();
};


#endif //HMM_GESTURERECOGNITION_HMM_H
