#ifndef HMM_GESTURERECOGNITION_GESTURELIBRARY_H
#define HMM_GESTURERECOGNITION_GESTURELIBRARY_H
#include <iostream>
#include <vector>
#include <map>
#include "src/utils/MediapipeInterface.h"
#include "Gesture.h"
#include <QObject>
#include "cmath"
#include "HMMcomponents/logProbability.h"


typedef int Observable;
class Gesture;
class HMM;

class GestureLibrary: public QObject {
private:
    std::map<std::string, Gesture> gestures;
    std::map <int, std::vector<Observable > > possibleObservables = {{0, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18}}}; //TODO remove hardcoded Channel number (from 0), ids of possible observables
    std::vector<std::vector<double> > accumulatedLiveFeedData;
    std::string directory;
    std::string name;
    HMM* thresholdHMM = nullptr;
public:
    explicit GestureLibrary(){};
    explicit GestureLibrary(std::string& path);
    void setPossibleObservables(std::map <int, std::vector<Observable > > observables);
    HMM* getThresholdHMM() const;
    bool addGesture(Gesture& gesture);
    bool addGesture(string& gestureID);
    void readIn(const string &path);
    bool initiateFileSystem(const string &path);
    void updateSavedGestures() const;
    bool isFileSystemInitiated() const;
    bool modelTrainingAndSelection(std::vector<std::vector<Observable>>& observed, std::string gestureID);
    bool isolatedRecognition(std::string& videoPath, std::string& gestureID);
    std::string realtimeRecognition(const std::vector<double>& frameLandmarks);
    bool
    fitAndSelect(std::vector<std::vector<Observable> > GestureData, const std::string &gestureID, int stateAmount = 5,
                 double threshold = 0.0001);
    const std::map<std::string, Gesture>& getGestures() const;

    std::string recognizeFromVideo(const char *AbsolutePath, MediapipeInterface* interface);

    std::pair<std::string, logProbability> recognizeGesture(std::vector<int> &observed);
private:
    HMM* createHMM(const map<Observable, logProbability> &emissionMap, const std::vector<Observable>& observables, int states);
};


#endif //HMM_GESTURERECOGNITION_GESTURELIBRARY_H
