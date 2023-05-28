#ifndef HMM_GESTURERECOGNITION_LOGPROBABILITY_H
#define HMM_GESTURERECOGNITION_LOGPROBABILITY_H

/**
 * A class to store probabilities as their natural logarithms.
 * This helps prevent underflow when using very small probabilities.
 *
 * All mathematical operators must be redefined according to the Log Semiring.
 * Only those that are actually used in the context of this project have been implemented.
 */
class logProbability{
    double value;
public:
/**
 * Initializes the probability to a value of 0 (a regular probability of one)
 */
    explicit logProbability();

/**
 * Initializes the probability to a given value
 * @param value The natural logarithm of a regular probability
 */

    explicit logProbability(double value);

/**
 * Calculates a log probability from a regular probability.
 * @param regularProbability The probability to convert.
 * @return The converted probability
 */

    static logProbability fromRegularProbability(double regularProbability);
    logProbability operator+(const logProbability & rightHandSide) const;
    logProbability& operator+=(const logProbability & rightHandSide);
    logProbability operator-(const logProbability & rightHandSide) const;
    logProbability& operator-=(const logProbability & rightHandSide);
    logProbability operator*(const logProbability & rightHandSide) const;
    logProbability operator/(const logProbability & rightHandSide) const;
    logProbability& operator/=(const logProbability & rightHandSide);
    logProbability operator/(const double & rightHandSide) const;
    logProbability& operator/=(const double & rightHandSide);
    bool operator>(const logProbability & rightHandSide) const;

/**
 * Gets the stored log probability
 * @return The probability
 */
    double getValueAsIs() const;

/**
 * Gets the stored log probability, converted back to a regular probability
 * @return The converted probability
 */
    double toRegularProbability() const;
};


#endif //HMM_GESTURERECOGNITION_LOGPROBABILITY_H
