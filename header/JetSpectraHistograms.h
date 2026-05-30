#ifndef JETSPECTRAHISTOGRAMS_H
#define JETSPECTRAHISTOGRAMS_H

#include "TH1F.h"
#include "TH1I.h"
#include "THnSparse.h"
#include "TString.h"
#include "Rtypes.h"

#include <cstddef>
#include <iostream>

#include "Binning.h"
#include "Utilities.h"
#include "JetTriggers_2026PbPb.h"
// #include "JetTriggers_2025PbPb.h"

struct JetSpectraStruct{

    enum MatchType{
        kNoDR = 0, // no offline-HLT object matching
        kDR = 1, // offline leading jet matched to HLT object
        kNMatchTypes = 2
    };
    
    // Axis indices for hn_L1T: {pt, |eta|, hiBin, L1T index, matchType}
    enum L1TAxis{
        kL1T_pt = 0,
        kL1T_eta = 1,
        kL1T_hiBin = 2,
        kL1T_trig = 3,
        kL1T_matchType = 4
    };
 
    // Axis indices for hn_HLT: {pt, |eta|, hiBin, HLT index, matchType}
    enum HLTAxis {
        kHLT_pt = 0,
        kHLT_eta = 1,
        kHLT_hiBin = 2,
        kHLT_trig = 3,
        kHLT_matchType = 4
    };

    // event histograms 
    TH1F* vz_unpassed = nullptr;
    TH1F* vz = nullptr;
    TH1I* hiBin = nullptr;
    TH1I* nref = nullptr;

    // event filter histograms
    TH1I* pclustF = nullptr;
    TH1I* ppvF = nullptr;
    TH1I* pphfF = nullptr;

    // higher dimensional histograms
    THnSparseF* hn_L1T = nullptr;
    THnSparseF* hn_HLT = nullptr;

    // constructors
    JetSpectraStruct(const BinningStruct& bins){InitHistograms(bins);}
    JetSpectraStruct(const BinningStruct& bins, bool allocate){if(allocate){InitHistograms(bins);}}

    void InitHistograms(const BinningStruct& bins){
        // event histograms 
        vz = MakeTH1<TH1F>("hvz", bins.vz);
        vz_unpassed = MakeTH1<TH1F>("hvz_unpassed", bins.vz);
        hiBin = MakeTH1<TH1I>("hhiBin", bins.hiBin);
        nref = MakeTH1<TH1I>("hnref", bins.nref);
        // event filter histograms
        pclustF = MakeTH1<TH1I>("hpclustF", bins.trig);
        ppvF = MakeTH1<TH1I>("hppvF", bins.trig);
        pphfF = MakeTH1<TH1I>("hpphfF", bins.trig);

        hn_L1T = MakeTHnSparse<THnSparseF>("hn_L1T", "L1T leading jet p_{T}",{bins.pt,bins.abseta,bins.hiBin,{(Int_t)nL1T, 0.0, (Float_t)nL1T, "L1T index"},{(Int_t)kNMatchTypes, 0.0, (Float_t)kNMatchTypes, "match type"}});
        hn_HLT = MakeTHnSparse<THnSparseF>("hn_HLT", "HLT leading jet p_{T}",{bins.pt,bins.abseta,bins.hiBin,{(Int_t)nHLT, 0.0, (Float_t)nHLT, "HLT index"},{(Int_t)kNMatchTypes, 0.0, (Float_t)kNMatchTypes, "match type"}});  

        for(std::size_t t=0; t<nL1T; t++){hn_L1T->GetAxis(kL1T_trig)->SetBinLabel(t+1, GetL1ShortName(sL1Trigs[t].name));}
        for(std::size_t t=0; t<nHLT; t++){hn_HLT->GetAxis(kHLT_trig)->SetBinLabel(t+1, GetHLTShortName(sHLTrigs[t].name));}
        hn_HLT->GetAxis(kHLT_matchType)->SetBinLabel(kNoDR+1, "noDR");
        hn_HLT->GetAxis(kHLT_matchType)->SetBinLabel(kDR+1, "dR");
        hn_L1T->GetAxis(kL1T_matchType)->SetBinLabel(kNoDR+1, "noDR");
        hn_L1T->GetAxis(kL1T_matchType)->SetBinLabel(kDR+1, "dR");
    }

    void FillL1T(std::size_t iL1T, MatchType matchType, Double_t pt,Double_t abseta, Int_t hiBin, Double_t w = 1.0){
        Double_t x[5] = {pt, abseta, (Double_t)hiBin, (Double_t)iL1T + 0.5, (Double_t)matchType + 0.5};
        hn_L1T->Fill(x, w);
    }

    void FillHLT(std::size_t iHLT, MatchType matchType, Double_t pt, Double_t abseta, Int_t hiBin, Double_t w = 1.0){
        Double_t x[5] = {pt, abseta, (Double_t)hiBin, (Double_t)iHLT + 0.5, (Double_t)matchType + 0.5};
        hn_HLT->Fill(x, w);
    }

    TH1D* ProjectL1T_pt(std::size_t iL1T, MatchType matchType, Double_t etalo, Double_t etahi, Int_t hiBinlo, Int_t hiBinhi, const TString& suffix = "") const{
        return ProjectTHnSparse1D(hn_L1T, kL1T_pt,
            {
                {kL1T_eta, etalo, etahi},
                {kL1T_hiBin, (Double_t)hiBinlo, (Double_t)hiBinhi},
                {kL1T_trig, (Double_t)iL1T + 0.1, (Double_t)iL1T + 0.9},
                {kL1T_matchType, (Double_t)matchType + 0.1, (Double_t)matchType + 0.9}
            }, suffix);
    }
 
    TH1D* ProjectHLT_pt(std::size_t iHLT, MatchType matchType, Double_t etalo, Double_t etahi, Int_t hiBinlo, Int_t hiBinhi, const TString& suffix = "") const {
        return ProjectTHnSparse1D(hn_HLT, kHLT_pt,
            {
                {kHLT_eta, etalo, etahi},
                {kHLT_hiBin, (Double_t)hiBinlo, (Double_t)hiBinhi},
                {kHLT_trig, (Double_t)iHLT + 0.1, (Double_t)iHLT + 0.9},
                {kHLT_matchType, (Double_t)matchType + 0.1, (Double_t)matchType + 0.9}
            }, suffix);
    }

    void Write(TFile* f) {
        f->cd();
        WriteAll(vz_unpassed);
        WriteAll(vz);
        WriteAll(hiBin);
        WriteAll(nref);
        WriteAll(pclustF);
        WriteAll(ppvF);
        WriteAll(pphfF);
        if(hn_L1T) hn_L1T->Write();
        if(hn_HLT) hn_HLT->Write();
    }
};

#endif
