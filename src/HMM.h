#ifndef HMM_GESTURERECOGNITION_HMM_H
#define HMM_GESTURERECOGNITION_HMM_H

#include <iostream>
#include <vector>
#include "src/HMMcomponents/hiddenState.h"
#include "src/HMMcomponents/Observable.h"
#include "src/HMMcomponents/hiddenState.h"

class HMM {
vector<hiddenState*> hiddenStates;
vector<Observable*> observables;
public:
    HMM(const vector<hiddenState*> &hiddenStates, const vector<Observable*> &observables);

    /**
 * Constructor to load a HMM from a JSON file.
 * @param saveFilePath The path to the JSON file
 * @param success Will contain true if the HMM object was constructed correctly. False can indicate a missing or incorrect file.
 */
    HMM(const string &saveFilePath, bool success);

    bool checkValues();
    
    double likelihood(std::vector<Observable*>observations);

    void train(const vector<int> &data, int iterations);

    void train(const vector<vector<int> >&dataVector, int iterations);

    void print();

    map<int, double> learnDistributionFromSamples(vector<std::vector<int>> samples);

    void HMMtoJson(std::string file);

private:

    vector<vector<double>> calculateAlpha(const vector<int>& data);

    vector<vector<double>> calculateBeta(const vector<int>& data);

    Observable* getObservable(int id);

    double calculateDenominator(const vector<int> &data, vector<vector<double>> &alpha, vector<vector<double>> &beta, int t);

};


#endif //HMM_GESTURERECOGNITION_HMM_H
