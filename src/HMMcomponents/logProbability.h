#ifndef HMM_GESTURERECOGNITION_LOGPROBABILITY_H
#define HMM_GESTURERECOGNITION_LOGPROBABILITY_H


class logProbability{
    double value;
public:
    explicit logProbability();
    explicit logProbability(double value);
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
    double getValueAsIs() const;
    double toRegularProbability() const;
};


#endif //HMM_GESTURERECOGNITION_LOGPROBABILITY_H
