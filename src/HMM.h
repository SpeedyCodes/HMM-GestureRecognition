#ifndef HMM_GESTURERECOGNITION_HMM_H
#define HMM_GESTURERECOGNITION_HMM_H

#include <iostream>
#include <vector>
#include "src/HMMcomponents/hiddenState.h"
#include "HMMcomponents/logProbability.h"

/**
 * The class to represent a Hidden Markov Model.
 * Includes training and likelihood functions as wel as JSON file I/O.
 */

class HMM {
public:
/**
 * Constructs a Hidden Markov Model from predefined hidden states and observables
 * @param hiddenStates The hidden states
 * @param observables The observables
 */

    HMM(const vector<hiddenState*> &hiddenStates, const vector<Observable> &observables);

/**
 * Constructs a Hidden Markov Model from a JSON file.
 * See "tests/testHMM.json" for an example of the format used.
 * @param saveFilePath The path to the JSON file
 * @param success Indicates if the read operation has resulted in a valid HMM. If not, this is either due to an
 * unreadable file or a file with invalid values.
 */

    HMM(const string &saveFilePath, bool &success);

    HMM(const HMM& other);
  
/**
 * Hidden Markov Model destructor
 */

    virtual ~HMM();

/**
 * Gets the hidden states that define the model.
 * @return A reference to the hidden states.
 */

    const vector<hiddenState *> &getHiddenStates() const;

/**
 * Gets the observables used by the model.
 * @return A reference to the observables.
 */
    const vector<Observable> &getObservables() const;

/**
 * Checks if the HMM is valid. This is true when all states have emission probablities for all observables,
 * and all groups of probabilities add up to 1.
 * @return True if the HMM is valid.
 */

    bool checkValues() const;

/**
 * Calculates the likelihood of a series of observations according to this HMM.
 * Uses the Forward Algorithm.
 * @param observations The observations
 * @return The probability of this series of observables occurring according to this HMM
 */
  
    logProbability likelihood(const vector<Observable> &observations) const;

/**
 * Wrapper to calculate the likelihood of several series of observations
 * @param observations The observations
 * @return The probability of these series of observables occurring according to this HMM
 */

    logProbability likelihood(const vector<vector<Observable>> &observations) const;

/**
 * Trains the HMM with given series of observations: the training data.
 * Uses the Baum-Welch algorithm.
 * @param dataVector The observations
 * @return True if the HMM was trained correctly. False can indicate empty vectors in the training data,
 * data with unrecognized observables or the HMM having become invalid during the training process.
 */

    bool train(const vector<vector<Observable>> &dataVector);

/**
 * Wrapper to train the HMM with the given data as long as either emission or transition probabilities are still altered
 * by new iterations.
 * @param dataVector The observations
 * @param threshold The minimal change in probabilities in consecutive training iterations to consider meaningful.
 * @return True if the HMM was trained correctly. See the base "train" method for what False can mean.
 */

    bool autoTrain(const vector<vector<Observable> > &dataVector, double threshold = 0.00001);

/**
 * Prints out all emission and transition probabilities as log probabilities.
 */

    void print() const;

/**
 * Writes out the HMM to a JSON file.
 * See "tests/testHMM.json" for an example of the format used.
 * @param file The path for the newly created file.
 */

    void HMMtoJson(const string &file) const;

private:

/**
 * Utility function for the Baum-Welch algorithm used by the train function
 */

    vector<vector<logProbability>> calculateAlpha(const vector<Observable>& data);

/**
 * Utility function for the Baum-Welch algorithm used by the train function
 */

    vector<vector<logProbability>> calculateBeta(const vector<Observable>& data);

/**
 * Utility function for the Baum-Welch algorithm used by the train function
 */

    logProbability calculateDenominator(const vector<Observable> &data, vector<vector<logProbability>> &alpha,
                                        vector<vector<logProbability>> &beta, int t);

/**
 * Calculates how the occurrence of given series of observables is distributed.
 * @param samples The observables
 * @return A map of all observables to a value between 0 and 1 indicating how much they are used relative to the total
 * amount of observables.
 */

    map<Observable , double> learnDistributionFromSamples(vector<vector<Observable>> samples);

    vector<hiddenState*> hiddenStates;
    vector<Observable> observables;
};


#endif //HMM_GESTURERECOGNITION_HMM_H
