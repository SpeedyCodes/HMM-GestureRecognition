#include "HMM.h"

HMM::HMM(const vector<hiddenState*> &hiddenStates, const vector<Observable*> &observables) : hiddenStates(hiddenStates),
                                                                                observables(observables) {
    if (!checkValues()){
        cout << " probabilities do not add up to 1 for a certain state" << endl;
    }
}
bool HMM::checkValues() {
    for(auto state:hiddenStates){
        if (!state.isValid()){
            return false;
        }
    }
    return true;
}


double HMM::likelihood(std::vector<Observable>observations){

    //goal: compute p(Zk, X(1:k)

    //initial chance van de beginstaat (= pi) kan je halen van "initialChance" van de hidden states

    // bij telkens keuze tussen 2 en wat is de kans dat het van welke van de twee komt: je eindigt met 2 kansen (voor elks van de 2 waartussen je kiest 1)
    //deze tel je dan bij elkaar op: https://www.youtube.com/watch?v=c6OrSKsH_gg

    //Z1->Zk: hidden states
    //X1->Xk: observables
    // X(1:k) = vector (X1, X2, ..., Xk)

    //ALPHA{k}(Zk) = P(Zk, X1:k) = SIGMA{M}{Zk-1=1} P(Zk, Zk-1, Zk-2,..., Z1:k)
    //= SIGMA{Z(k-1)}(P(Xk|Zk, Z(k-1), X1:(k-1))*P(Zk|Z(k-1), X1:(k-1))*P(Z(k-1), X(1:(k-1)))-->deze laatste is zoals de P(Zk, X1: k) van de lijn hierboven maar met k-1 idpv k
    //= SIGMA{Z(k-1)}(P(Xk|Zk) * P(Zk|Z(k-1)) * P(Z(k-1), X(1:(k-1))))
    //
    //P(Xk|Zk) = emission probability --> kennen we
    //P(Zk|Z(k-1)) = transition probabilities --> kennen we
    //P(Z(k-1), X(1:(k-1))) ! ALPHA{k-1}(Z(k-1) --> eentje lager dan op regel 21

    //ALPHA{k}(Zk) = SIGMA{Z(k-1) = 1 --> M}(P(Xk|Zk) * P(Zk|Z(k-1) * ALPHA{k-1}(Z(k-1)) --> werkt voor k = van 2 tot en met n

    // voorbeeldje:
    //ALPH{Z1} = P(Z1, X1) = P(Z1) * P(X1|Z1) --> eerste term berekenen om daarna naar ALPHA{Zk} te kunnen gaan
    //P(Z1) = initial distribution --> kennen we
    //P(X1|Z1) = emission probabilities --> kennen we

    // RESULTAAT: P(Zk, X(1:k)) voor elke k


}