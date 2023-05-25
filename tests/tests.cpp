#include <iostream>
#include "src/Gesture.h"
#include "src/HMM.h"
#include "src/HMMcomponents/hiddenState.h"
using namespace std;

bool testHiddenState(){
    auto* state1 = new hiddenState(1, logProbability::fromRegularProbability(0.75));
    auto* state2 = new hiddenState();
    auto* state3 = new hiddenState();

    if ( state1->id != 1){
        cerr << "ID of state was not set properly" << endl;
        return false;
    }
    if (abs(state1->initialChance.toRegularProbability() - 0.75) >= 1e-12){
        cerr << "initial chance of state was not set properly" << endl;
        return false;
    }

    state1->transitionMap = {{state1,logProbability::fromRegularProbability(0.6)},
                             {state2,logProbability::fromRegularProbability(0.2)},
                             {state3,logProbability::fromRegularProbability(0.3)}};
    state1->emissionMap = {{0,logProbability::fromRegularProbability(0.5)},{1,logProbability::fromRegularProbability(0.5)}};
    if (state1->isValid()){
        cerr << "state shows as valid when being invalid" << endl;
        return false;
    }
    state1->transitionMap[state1] = logProbability::fromRegularProbability(0.5);
    state1->emissionMap[0] = logProbability::fromRegularProbability(0.7);
    if (state1->isValid()){
        cerr << "state shows as valid when being invalid" << endl;
        return false;
    }
    state1->emissionMap[0] = logProbability::fromRegularProbability(0.5);
    if (!state1->isValid()){
        cerr << "state shows as invalid when being valid" << endl;
        return false;
    }
    return true;
}

bool testHMM(){
    bool success = false;
    HMM hmm = HMM("../tests/testHMM.json", success);
    if (!success){
        cerr << "generating HMM from json failed" << endl;
        return false;
    }
    vector<Observable> observables = {5,10};
    if (observables != hmm.getObservables()){
        cerr << "observables weren't read in properly" << endl;
        return false;
    }
    if (hmm.getHiddenStates().size() != 1){
        cerr << "hidden states weren't read in properly" << endl;
        return false;
    }
    hiddenState* state = hmm.getHiddenStates()[0];
    if (state->id != 7){
        cerr << "hidden state id wasn't read in properly" << endl;
        return false;
    }
    if (abs(state->transitionMap[state].toRegularProbability()- 1) >= 1e-12){
        cerr << "transition probability wasn't read in properly" << endl;
        return false;
    }
    if (abs(state->emissionMap[5].toRegularProbability() - 0.1) >= 1e-12 ||
    abs(state->emissionMap[10].toRegularProbability() - 0.9) >= 1e-12){
        cerr << "emission probabilities weren't read in properly" << endl;
        return false;
    }
    return true;
}

bool testLogProbability() {
    logProbability test = logProbability();
    double temp = log(0.1);
    if(test.getValueAsIs() != 0){
        cerr << "default constructor did not work as expected" << endl;
        return false;
    }
    test = logProbability(42);
    if(test.getValueAsIs() != 42){
        cerr << "logProbability(double) constructor did not work as expected" << endl;
        return false;
    }
    test = logProbability(5.5053315359323628046792407314181098610994011615823063394746372533908665315939611884417962231821375873679619994819030433228041556259245564173394842148421124776181068382690349840793128993047447350840868725523755781639896004520582160011631962527438600148632373646357446267212049734134568368132539922745190865399596564908884187599205099162706537099010612892279142138488889802991096280841261829196283636052027982613253228951913165168815290726500965824168406309014597635734359347036302700052204142615403259884717844607013580);
    if(abs(test.toRegularProbability() - 246) >= 1e-12){
        cerr << "toRegularProbability did not work as expected" << endl;
        return false;
    }
    test = logProbability::fromRegularProbability(5);
    if(abs(test.toRegularProbability() - 5) >= 1e-12){
        cerr << "fromRegularProbability did not work as expected" << endl;
        return false;
    }
    if(abs(test.getValueAsIs() - log(5)) >= 1e-12){
        cerr << "fromRegularProbability did not work as expected" << endl;
        return false;
    }

    //addition
    auto log1 = logProbability(2);
    auto log2 = logProbability(3);
    if(abs((log1+log2).getValueAsIs() - log(exp(2) + exp(3))) >= 1e-12){
        cerr << "operator + did not work as expected" << endl;
        return false;
    }
    log1 += log2;
    if(abs(log1.getValueAsIs() - log(exp(2) + exp(3))) >= 1e-12){
        cerr << "operator += did not work as expected" << endl;
        return false;
    }

    //subtraction
    log1 = logProbability(2);
    log2 = logProbability(3);
    if(abs((log1+log2).getValueAsIs() - log(exp(2) - exp(3))) >= 1e-12){
        cerr << "operator - did not work as expected" << endl;
        return false;
    }
    log1 -= log2;
    if(abs(log1.getValueAsIs() - log(exp(2) - exp(3))) >= 1e-12){
        cerr << "operator -= did not work as expected" << endl;
        return false;
    }

    //multiplication
    log1 = logProbability(2);
    log2 = logProbability(3);
    if(abs((log1*log2).getValueAsIs() - (2+3)) >= 1e-12){
        cerr << "operator * did not work as expected" << endl;
        return false;
    }

    //division (logProbability)
    log1 = logProbability(2);
    log2 = logProbability(3);
    if(abs((log1/log2).getValueAsIs() - (2-3)) >= 1e-12){
        cerr << "operator /(with logProbability) did not work as expected" << endl;
        return false;
    }
    log1 /= log2;
    if(abs(log1.getValueAsIs() - (2-3)) >= 1e-12){
        cerr << "operator /=(with logProbability) did not work as expected" << endl;
        return false;
    }

    //division (double)
    if(abs((log2/2).getValueAsIs() - ((double)3/2)) >= 1e-12){
        cerr << "operator /(with double) did not work as expected" << endl;
        return false;
    }
    log2 /= 2;
    if(abs(log2.getValueAsIs() - ((double)3/2)) >= 1e-12){
        cerr << "operator /=(with double) did not work as expected" << endl;
        return false;
    }

    //greater than
    log1 = logProbability(2);
    log2 = logProbability(3);
    if(log1 > log2){
        cerr << "operator > did not work as expected" << endl;
        return false;
    }
    return true;
}

bool testBaumWelch1(){

    // Set corpus
    std::vector<Observable > abba = {0,1,1,0};
    std::vector<Observable > bab = {1,0,1};
    std::vector<std::vector<Observable >> corpus(10, abba);
    std::vector<std::vector<Observable >> corpus2(20, bab);
    corpus.insert(corpus.end(), corpus2.begin(), corpus2.end());

    // Set first HMM
    hiddenState* s = new hiddenState(1, 0.85);
    hiddenState* t= new hiddenState(2, 0.15);
    s->emissionMap = {{0,0.4}, {1,0.6}};
    t->emissionMap = {{0,0.5}, {1,0.5}};
    s->transitionMap = {{s, 0.3}, {t, 0.7}};
    t->transitionMap = {{s, 0.1}, {t, 0.9}};
    HMM hmm({s,t}, {0,1});

    // Train
    hmm.autoTrain(corpus);
    hmm.autoTrain(corpus); // Number of iterations should be 1
    // Check manually
    return true;
}

int main(){
    if (testHiddenState() && testHMM() && testLogProbability() && testBaumWelch1()){
        cout << "all tests passed" << endl;
    }
    else {
        cerr << "not all tests have passed" << endl;
    }
    return 0;
}