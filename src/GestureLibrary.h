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
public:

/**
 * Default constructor for when a directory to read/write from/to has not been set by the user.
 */

    explicit GestureLibrary(){};

/**
 * Constructor for when a directory to read/write from/to has been set by the user already.
 * @param path
 */

    explicit GestureLibrary(std::string& path);
    void setPossibleObservables(std::map <int, std::vector<Observable > > observables);
    HMM* getThresholdHMM() const;
    bool addGesture(Gesture& gesture);
    bool addGesture(string& gestureID, std::map<std::string,bool> gestureFeatures = {});

/**
 * Reads a gesture library JSON file.
 * @param path Path to the file.
 */

    void readIn(const string &path);

/**
 * Creates a JSON file with an empty gesture list.
 * Future calls to updateSavedGestures() will result in new JSON files in the directory of this file.
 * @param path The path to the file to create
 */

    void initiateFileSystem(const string &path);
    void updateSavedGestures() const;

/**
 * @return True if this instance knows where to write new gestures out to
 */

    bool isFileSystemInitiated() const;
    std::string realtimeRecognition(const std::vector<double>& frameLandmarks);
    bool

    /**
     * trains and tests HMMs with hidden states amount going from 3 to a given amount for a given data set and assigns the one with
     * the highest success rate to a given gesture
     * @param GestureData data set to train/test HMM with
     * @param gestureID id of the gesture that the new HMM is going to be assigned to
     * @param stateAmount the highest amount of states a HMM can have
     * @param threshold threshold used to train the HMMs
     * @return true if a new model was assigned to the given gesture, otherwise false
     */
    fitAndSelect(std::vector<std::vector<Observable> > GestureData, const std::string &gestureID, int stateAmount = 5,
                 double threshold = 0.0001);
    const std::map<std::string, Gesture>& getGestures() const;

    std::map<std::string,Gesture> getFilteredGestures(std::map<std::string,bool> dataFilters)const;

    std::string recognizeFromVideo(const char *AbsolutePath, MediapipeInterface* interface);

    std::pair<std::string, logProbability> recognizeGesture(std::vector<int> &observed) const;
    std::pair<std::string, logProbability> recognizeFromGivenGestures(vector<int>& observed, const std::map<std::string, Gesture>& givenGestures) const;

    bool isMultipleOn() const;

    void setMultipleOn(bool multipleOn);

    bool filterPressed = false;

    bool allowFilter = true;

    bool startAnalysis = false;

private:
    /**
     * creates a "left to right" HMM with a certain amount of hidden states
     * @param emissionMap emission map for every hidden state of the HMM
     * @param observables vector of observables of the HMM
     * @param states the amount of hidden states the HMM needs to have
     * @return pointer to the newly created HMM
     */
    HMM* createHMM(const map<Observable, logProbability> &emissionMap, const std::vector<Observable>& observables, int states);

    std::map<std::string, Gesture> gestures;
    std::map <int, std::vector<Observable > > possibleObservables = {{0, {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18}}}; //TODO remove hardcoded Channel number (from 0), ids of possible observables
    std::vector<std::vector<double> > accumulatedLiveFeedData;
    std::string directory;
    std::string name;
    unsigned int counterOfEmptiness = 0;
    HMM* thresholdHMM = nullptr;
    bool multipleOn = false;
};


#endif //HMM_GESTURERECOGNITION_GESTURELIBRARY_H
