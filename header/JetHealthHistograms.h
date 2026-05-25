#ifndef JETHEALTHHISTOGRAMS_H
#define JETHEALTHHISTOGRAMS_H

#include "TH1F.h"
#include "TH1I.h"
#include "THnSparse.h"
#include "TFile.h"
#include "TMath.h"
#include "TString.h"
#include "Rtypes.h"

#include <vector>
#include <cstddef>

#include "Binning.h"
#include "Utilities.h"

#include "JetStruct.h"

enum PFType{
    CHF = 0,
    NHF = 1,
    CEF = 2,
    NEF = 3,
    MUF = 4,
    PFTypes = 5
};

constexpr std::array<const char*, PFTypes> PFTypeNames  = {"CHF", "NHF", "CEF", "NEF", "MUF"};
constexpr std::array<const char*, PFTypes> PFTypeTitles = {"Charged Hadron Fraction","Neutral Hadron Fraction","Charged EM Fraction","Neutral EM Fraction","Muon Fraction"};

template <Int_t MAXNREF>
struct JetHealthStruct{

    // event histograms
    TH1F* vz_unpassed = nullptr;
    TH1F* vz = nullptr;
    TH1I* hiBin = nullptr;
    TH1I* nref = nullptr;

    // event filter histograms
    TH1I* pclustF = nullptr;
    TH1I* ppvF = nullptr;
    TH1I* pphfF = nullptr;

    // kimematics and PF fractions
    THnSparseF* kin = nullptr;
    THnSparseF* pf = nullptr;

    JetHealthStruct(const BinningStruct& bins){InitHistograms(bins);}
    
    void InitHistograms(const BinningStruct& bins){
        vz = MakeTH1<TH1F>("hvz", bins.vz);
        vz_unpassed = MakeTH1<TH1F>("hvz_unpassed", bins.vz);
        hiBin = MakeTH1<TH1I>("hhiBin", bins.hiBin);
        nref = MakeTH1<TH1I>("hnref", bins.nref);
        pclustF = MakeTH1<TH1I>("hpclustF", bins.trig);
        ppvF = MakeTH1<TH1I>("hppvF", bins.trig);
        pphfF = MakeTH1<TH1I>("hpphfF", bins.trig);
 
        kin = MakeTHnSparse<THnSparseF> ("hjetkin", "pt:eta:phi:hiBin", {bins.pt, bins.eta, bins.phi, bins.hiBin});
        const AxisBins pfType = {PFTypes, 0., (Float_t)PFTypes, "PF type (CHF/NHF/CEF/NEF/MUF)"};
        pf  = MakeTHnSparse<THnSparseF> ("hjetpf",  "pfFrac:pfType:abseta:hiBin", {bins.pfFrac, pfType, bins.abseta, bins.hiBin});
    }

    void FillKin(const typename JetStruct<MAXNREF>::RecoMomenta& reco, Int_t j, Int_t hiBinVal, Float_t w = 1.0){
        Double_t x[] = {reco.pt[j], reco.eta[j], reco.phi[j], (Double_t)hiBinVal};
        kin->Fill(x, w);
    }

    void FillPF(const typename JetStruct<MAXNREF>::RecoMomenta& reco, Int_t j, Int_t hiBinVal, Float_t w = 1.0){
        const Float_t fracs[PFTypes] = {reco.pf.CHF[j], reco.pf.NHF[j], reco.pf.CEF[j], reco.pf.NEF[j], reco.pf.MUF[j]};
        for (Int_t p = 0; p < PFTypes; p++) {
            Double_t x[] = {fracs[p], p + 0.5, TMath::Abs(reco.eta[j]), (Double_t)hiBinVal};
            pf->Fill(x, w);
        }
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
        if(kin){kin->Write();}
        if(pf){pf->Write();}
    }

};

#endif
