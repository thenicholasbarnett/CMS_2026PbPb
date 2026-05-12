#ifndef JETTRIGGERS_PBPB_H
#define JETTRIGGERS_PBPB_H

#include "TString.h"
#include "Rtypes.h"

#include <vector>
#include <utility>
#include <array>
#include <cctype>

// HLT trigger names
const std::array sHLTrigs = {
    TString("HLT_HIPuAK4CaloJet40Eta5p1_MinBiasHF1AND_v9"),
    TString("HLT_HIPuAK4CaloJet60Eta5p1_MinBiasHF1AND_v9"),
    TString("HLT_HIPuAK4CaloJet80Eta5p1_v17"),
    TString("HLT_HIPuAK4CaloJet100Eta5p1_v17"),
    TString("HLT_HIPuAK4CaloJet120Eta5p1_v17")
};

// L1 trigger names
const std::array sL1Trigs = {
    TString("L1_MinimumBiasHF1_AND_BptxAND"),
    TString("L1_SingleJet60_BptxAND"),
    TString("L1_SingleJet80_BptxAND")
};

// automatically derived sizes
constexpr std::size_t nHLT = sHLTrigs.size();
constexpr std::size_t nL1T = sL1Trigs.size();

// L1 seed for each HLT
const std::array<Int_t, nHLT> L1SeedHLT = {0, 0, 1, 1, 2};

// getting the pt threshold of the jet trigger from the trigger name
inline Float_t GetJetTriggerThreshold(const TString& trig){

    // find "Jet" in HLT name
    Ssiz_t jetPos = trig.Index("Jet");

    // if not found return invalid value
    if(jetPos == kNPOS) return -1.0;

    // move to first character after "Jet"
    jetPos += 3;
    TString number = "";

    // collect consecutive digits
    while(jetPos < trig.Length() && isdigit(trig[jetPos])){
        number += trig[jetPos];
        jetPos++;
    }
    return number.Atof();
}

inline TString GetHLTShortName(const TString& trig){
    return Form("_HLT_Jet%.0f", GetJetTriggerThreshold(trig));
}

inline TString GetL1ShortName(const TString& trig){
    if(trig.Contains("MinimumBias")){return "_L1_MinBias";}
    return Form("_L1_Jet%.0f", GetJetTriggerThreshold(trig));
}

// HLT object tree information
const TString sHLTObjDir = "hltobject/";

inline TString GetHLTObjTreeName(std::size_t i){
    TString trig = sHLTrigs[i];
    trig.Remove(trig.Last('v') + 1);
    return sHLTObjDir + trig;
}

// declaring trigger structure
struct TriggersStruct{

    // trigger decisions
    Int_t HLT[nHLT]={0};
    Int_t L1T[nL1T]={0};

    // HLT object variables
    std::vector<Double_t>* HLT_JetObj_pt[nHLT];
    std::vector<Double_t>* HLT_JetObj_eta[nHLT];
    std::vector<Double_t>* HLT_JetObj_phi[nHLT];

    // constructor
    TriggersStruct(){
        for(std::size_t i=0; i<nHLT; i++){
            HLT_JetObj_pt[i]  = nullptr;
            HLT_JetObj_eta[i] = nullptr;
            HLT_JetObj_phi[i] = nullptr;
        }
    }

    // mapping trigger decisions to branches
    std::vector<std::pair<TString, void*>> BranchMap(){
        std::vector<std::pair<TString, void*>> branches;
        for(std::size_t i=0; i<nHLT; i++){branches.push_back({sHLTrigs[i], &HLT[i]});}
        for(std::size_t i=0; i<nL1T; i++){branches.push_back({sL1Trigs[i], &L1T[i]});}
        return branches;
    }

    // mapping HLT object branches
    std::vector<std::pair<TString, void*>> HLTObjBranchMap(Int_t iTrig){
        return{
            {"pt",  &HLT_JetObj_pt[iTrig]},
            {"eta", &HLT_JetObj_eta[iTrig]},
            {"phi", &HLT_JetObj_phi[iTrig]}
        };
    }
};

#endif
