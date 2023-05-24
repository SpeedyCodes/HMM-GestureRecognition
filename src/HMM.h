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

    HMM(const string &saveFilePath, bool &success);

    virtual ~HMM();

    const vector<hiddenState *> &getHiddenStates() const;

    bool checkValues();

    double likelihood(std::vector<Observable>& observations);

    double likelihood(std::vector<std::vector<Observable> > observations);

    bool train(const vector<vector<Observable> >&dataVector, int iterations);

    bool train(const vector<vector<Observable>> &dataVector);

    bool autoTrain(const vector<vector<Observable> > &dataVector, double threshold = 0.00001);

    void print();

    void HMMtoJson(string &file);

    const vector<Observable> &getObservables() const;

private:

    vector<vector<double>> calculateAlpha(const vector<Observable>& data);

    vector<vector<double>> calculateBeta(const vector<Observable>& data);

    double calculateDenominator(const vector<Observable> &data, vector<vector<double>> &alpha,
                                vector<vector<double>> &beta, int t);
};


#endif //HMM_GESTURERECOGNITION_HMM_H
