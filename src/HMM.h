#ifndef HMM_GESTURERECOGNITION_HMM_H
#define HMM_GESTURERECOGNITION_HMM_H

#include <iostream>
#include <vector>
#include "src/HMMcomponents/hiddenState.h"

class HMM {
vector<hiddenState*> hiddenStates;
vector<Observable> observables;
public:
    HMM(const vector<hiddenState*> &hiddenStates, const vector<Observable> &observables);

    HMM(const string &saveFilePath, bool success);

    const vector<hiddenState *> &getHiddenStates() const;

    bool checkValues();
    
    double likelihood(std::vector<Observable>& observations);

    bool train(const vector<Observable> &data, int iterations);

    void train(const vector<vector<Observable> >&dataVector, int iterations);

    void print();

    void HMMtoJson(std::string file);

private:

    vector<vector<double>> calculateAlpha(const vector<Observable>& data);

    vector<vector<double>> calculateBeta(const vector<Observable>& data);

    double calculateDenominator(const vector<Observable> &data, vector<vector<double>> &alpha,
                                vector<vector<double>> &beta, int t);
};


#endif //HMM_GESTURERECOGNITION_HMM_H
