#ifndef HMM_GESTURERECOGNITION_GESTURELIBRARY_H
#define HMM_GESTURERECOGNITION_GESTURELIBRARY_H
#include <iostream>
#include <vector>
#include <map>
#include "src/utils/MediapipeInterface.h"
#include "Gesture.h"
#include <QObject>


typedef int Observable;
class Gesture;
class HMM;

class GestureLibrary: public QObject {
private:
    std::map<std::string, Gesture> gestures;
    std::map <int, std::vector<Observable > > possibleObservables; // Channel number (from 0), ids of possible observables
    std::vector<std::vector<double> > accumulatedLiveFeedData;
    std::string directory;
    std::string name;
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
    std::string recognizeGesture(std::vector<Observable>& observed, double& likelihood) const { return "";};
    bool isolatedRecognition(std::string& videoPath, std::string& gestureID);
    std::string realtimeRecognition(const std::vector<double>& frameLandmarks);
    bool
    fitAndSelect(std::vector<std::vector<Observable> > GestureData, const std::string &gestureID, int stateAmount = 10,
                 double threshold = 0.0001);
    const std::map<std::string, Gesture>& getGestures() const;

    std::string recognizeFromVideo(const char *AbsolutePath, MediapipeInterface* interface);

    std::pair<std::string, double> recognizeGesture(std::vector<int> &observed);
private:
    HMM* createHMM(const std::map<Observable, double>& emissionMap, const std::vector<Observable>& observables, int states);
};


#endif //HMM_GESTURERECOGNITION_GESTURELIBRARY_H
