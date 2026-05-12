#ifndef JETSTRUCT_H
#define JETSTRUCT_H

#include "TString.h"
#include "Rtypes.h"

#include <vector>
#include <utility>

// declaring event stucture
template <Int_t MAXNREF>
struct JetStruct{

    // jet variables //
    // more than any number of jets in an event in ttrees being processed
    static constexpr Int_t maxnref = MAXNREF;
    // predefined minimum pt of jets to include
    Float_t ptcut;
    // number in event
    Int_t nref;
    // momenta
    Float_t rawpt[maxnref];
    Float_t pt[maxnref];
    Float_t eta[maxnref];
    Float_t phi[maxnref];
    // PF fractions
    Float_t PfCHF[maxnref];
    Float_t PfNHF[maxnref];
    Float_t PfCEF[maxnref];
    Float_t PfNEF[maxnref];
    Float_t PfMUF[maxnref];
    // PF multiplicities
    Int_t PfCHM[maxnref];
    Int_t PfNHM[maxnref];
    Int_t PfCEM[maxnref];
    Int_t PfNEM[maxnref];
    Int_t PfMUM[maxnref];

    // constuctor
    JetStruct(Float_t cut = 0.0) : ptcut(cut), nref(0){}

    // mapping variables to branches
    std::vector<std::pair<TString, void*>> BranchMap(){
        return{
            {"nref", &nref},
            {"rawpt", rawpt},
            {"jtpt", pt},
            {"jteta", eta},
            {"jtphi", phi},
            {"jtPfCHF", PfCHF},
            {"jtPfNHF", PfNHF},
            {"jtPfCEF", PfCEF},
            {"jtPfNEF", PfNEF},
            {"jtPfMUF", PfMUF},
            {"jtPfCHM", PfCHM},
            {"jtPfNHM", PfNHM},
            {"jtPfCEM", PfCEM},
            {"jtPfNEM", PfNEM},
            {"jtPfMUM", PfMUM}
        };
    }
};

#endif