
#include "logProbability.h"
#include "cmath"
logProbability::logProbability(double value) : value(value) {}

logProbability logProbability::operator*(const logProbability &rightHandSide) const{
    return logProbability(value + rightHandSide.value);
}

logProbability logProbability::fromRegularProbability(double regularProbability) {
    return logProbability(log(regularProbability));
}

logProbability logProbability::operator+(const logProbability &rightHandSide) const {
    return logProbability(log(exp(value) + exp(rightHandSide.value)));
}

logProbability::logProbability() : value(0) {}

logProbability& logProbability::operator+=(const logProbability & rightHandSide){
    this->value = (*this+rightHandSide).value;
    return *this;
}

logProbability logProbability::operator/(const logProbability &rightHandSide) const {
    return logProbability(value - rightHandSide.value);
}

logProbability &logProbability::operator/=(const logProbability &rightHandSide) {
    this->value = (*this/rightHandSide).value;
    return *this;
}

logProbability logProbability::operator/(const double &rightHandSide) const {
    return logProbability(value/rightHandSide);
}

logProbability &logProbability::operator/=(const double &rightHandSide) {
    this->value = (*this/rightHandSide).value;
    return *this;
}

double logProbability::getValueAsIs() const {
    return value;
}

double logProbability::toRegularProbability() const {
    return exp(value);
}

logProbability logProbability::operator-(const logProbability &rightHandSide) const {
    return logProbability(log(exp(value) - exp(rightHandSide.value)));
}

logProbability &logProbability::operator-=(const logProbability &rightHandSide) {
    this->value = (*this-rightHandSide).value;
    return *this;
}

bool logProbability::operator>(const logProbability &rightHandSide) const {
    return value>rightHandSide.value;
}
