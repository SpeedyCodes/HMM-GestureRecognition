#ifndef HMM_GESTURERECOGNITION_GESTURELIBRARY_H
#define HMM_GESTURERECOGNITION_GESTURELIBRARY_H
#include <iostream>
#include <vector>
#include <map>
#include <QObject>

typedef int Observable;
class Gesture;
class HMM;

class GestureLibrary: public QObject {
private:
    std::map<std::string, Gesture> gestures;
    std::map <int, std::vector<Observable > > possibleObservables; // Channel number (from 0), ids of possible observables
    std::vector<std::vector<double> > accumulatedLiveFeedData;
public:
    GestureLibrary(){};
    void setPossibleObservables(std::map <int, std::vector<Observable > > observables);
    HMM* getThresholdHMM() const;
    bool addGesture(Gesture& gesture);
    void fromDirectory(std::string& directory);
    void toDirectory(std::string& directory) const;
    bool modelTrainingAndSelection(std::vector<std::vector<Observable>>& observed, std::string gestureID);
    std::string recognizeGesture(std::vector<Observable>& observed, double& likelihood) const { return "";};
    bool isolatedRecognition(std::string& videoPath, std::string& gestureID);
    std::string realtimeRecognition(const std::vector<double>& frameLandmarks);
    bool
    fitAndSelect(std::vector<std::vector<Observable> > GestureData, const std::string &gestureID, double threshold = 0.0001);

private:
    HMM* createThreeStateHMM(const std::map<Observable, double>& emissionMap, const std::vector<Observable>& observables);

    HMM* createFourStateHMM(const std::map<Observable, double>& emissionMap, const std::vector<Observable>& observables);

    HMM* createFiveStateHMM(const std::map<Observable, double>& emissionMap, const std::vector<Observable>& observables);
};


#endif //HMM_GESTURERECOGNITION_GESTURELIBRARY_H
