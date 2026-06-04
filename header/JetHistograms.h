#ifndef JETHISTOGRAMS_H
#define JETHISTOGRAMS_H

#include "TH1F.h"
#include "TH1I.h"
#include "THnSparse.h"
#include "TFile.h"
#include "TMath.h"
#include "TString.h"
#include "Rtypes.h"

#include <cstddef>
#include <iostream>

#include "Binning.h"
#include "Utilities.h"
#include "JetStruct.h"
#include "JetTriggers_2026PbPb.h"

enum PFType{CHF = 0, NHF = 1, CEF = 2, NEF = 3, MUF = 4, PFTypes = 5};
constexpr std::array<const char*, PFTypes> PFTypeNames  = {"CHF", "NHF", "CEF", "NEF", "MUF"};
constexpr std::array<const char*, PFTypes> PFTypeTitles = {"Charged Hadron Fraction","Neutral Hadron Fraction","Charged EM Fraction","Neutral EM Fraction","Muon Fraction"};

template <Int_t MAXNREF>
struct JetHistogramsStruct{

    // Axis indices for hn_kin: {pt, |eta|, phi, hiBin, pfFrac, pfType}
    enum KinAxis{
        kKin_pt = 0,
        kKin_eta = 1,
        kKin_phi = 2,
        kKin_hiBin = 3,
        kKin_pfFrac = 4,
        kKin_pfType = 5
    };

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
    // TH1I* pclustF = nullptr;
    TH1I* ppvF = nullptr;
    TH1I* pphfF = nullptr;

    // higher dimensional histograms
    THnSparseF* hn_kin = nullptr;
    THnSparseF* hn_L1T = nullptr;
    THnSparseF* hn_HLT = nullptr;

    // constructors
    JetHistogramsStruct(const BinningStruct& bins){InitHistograms(bins);}
    JetHistogramsStruct(const BinningStruct& bins, bool allocate){if(allocate){InitHistograms(bins);}}

    // initialization
    void InitHistograms(const BinningStruct& bins){
        // event histograms 
        vz = MakeTH1<TH1F>("hvz", bins.vz);
        vz_unpassed = MakeTH1<TH1F>("hvz_unpassed", bins.vz);
        hiBin = MakeTH1<TH1I>("hhiBin", bins.hiBin);
        nref = MakeTH1<TH1I>("hnref", bins.nref);
        // event filter histograms
        // pclustF = MakeTH1<TH1I>("hpclustF", bins.trig);
        ppvF = MakeTH1<TH1I>("hppvF", bins.trig);
        pphfF = MakeTH1<TH1I>("hpphfF", bins.trig);

        const AxisBins pfTypeAxis = {PFTypes, 0.0, (Float_t)PFTypes, "PF type"};
        hn_kin = MakeTHnSparse<THnSparseF>(
            "hn_kin", "Jet kinematics and PF fractions",
            {bins.pt, bins.abseta, bins.phi, bins.hiBin, bins.pfFrac, pfTypeAxis}
        );

        hn_L1T = MakeTHnSparse<THnSparseF>("hn_L1T", "L1T leading jet p_{T}",{bins.pt,bins.abseta,bins.hiBin,{(Int_t)nL1T, 0.0, (Float_t)nL1T, "L1T index"},{(Int_t)kNMatchTypes, 0.0, (Float_t)kNMatchTypes, "match type"}});
        hn_HLT = MakeTHnSparse<THnSparseF>("hn_HLT", "HLT leading jet p_{T}",{bins.pt,bins.abseta,bins.hiBin,{(Int_t)nHLT, 0.0, (Float_t)nHLT, "HLT index"},{(Int_t)kNMatchTypes, 0.0, (Float_t)kNMatchTypes, "match type"}});  

        for(std::size_t t=0; t<nL1T; t++){hn_L1T->GetAxis(kL1T_trig)->SetBinLabel(t+1, GetL1ShortName(sL1Trigs[t].name));}
        for(std::size_t t=0; t<nHLT; t++){hn_HLT->GetAxis(kHLT_trig)->SetBinLabel(t+1, GetHLTShortName(sHLTrigs[t].name));}
        hn_HLT->GetAxis(kHLT_matchType)->SetBinLabel(kNoDR+1, "noDR");
        hn_HLT->GetAxis(kHLT_matchType)->SetBinLabel(kDR+1, "dR");
        hn_L1T->GetAxis(kL1T_matchType)->SetBinLabel(kNoDR+1, "noDR");
        hn_L1T->GetAxis(kL1T_matchType)->SetBinLabel(kDR+1, "dR");
        for(Int_t p=0; p<PFTypes; p++){hn_kin->GetAxis(kKin_pfType)->SetBinLabel(p+1, PFTypeNames[p]);}
    }

    // filling functions
    
    void FillKin(const typename JetStruct<MAXNREF>::RecoMomenta& reco, Int_t j, Int_t hiBinVal, Double_t w = 1.0){
        const Float_t fracs[PFTypes] = {reco.pf.CHF[j], reco.pf.NHF[j],reco.pf.CEF[j], reco.pf.NEF[j], reco.pf.MUF[j]};
        for(Int_t p=0; p<PFTypes; p++){
            Double_t x[6] = {reco.pt[j], TMath::Abs(reco.eta[j]), reco.phi[j],(Double_t)hiBinVal, fracs[p], p + 0.5};
            hn_kin->Fill(x, w);
        }
    }

    void FillL1T(std::size_t iL1T, MatchType matchType, Double_t pt,Double_t abseta, Int_t hiBin, Double_t w = 1.0){
        Double_t x[5] = {pt, abseta, (Double_t)hiBin, (Double_t)iL1T + 0.5, (Double_t)matchType + 0.5};
        hn_L1T->Fill(x, w);
    }

    void FillHLT(std::size_t iHLT, MatchType matchType, Double_t pt, Double_t abseta, Int_t hiBin, Double_t w = 1.0){
        Double_t x[5] = {pt, abseta, (Double_t)hiBin, (Double_t)iHLT + 0.5, (Double_t)matchType + 0.5};
        hn_HLT->Fill(x, w);
    }

    // projection helpers

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
        // WriteAll(pclustF);
        WriteAll(ppvF);
        WriteAll(pphfF);
        if(hn_kin) hn_kin->Write();
        if(hn_L1T) hn_L1T->Write();
        if(hn_HLT) hn_HLT->Write();
    }
};
#endif