#ifndef JETHEALTHHISTOGRAMS_H
#define JETHEALTHHISTOGRAMS_H

#include "TH1F.h"
#include "TH1I.h"
#include "TFile.h"
#include "TString.h"
#include "Rtypes.h"

#include <vector>
#include <cstddef>

#include "Binning.h"
#include "HistUtilities.h"

inline TH1F* MakeTH1F(const TString& hname, const AxisBins& bins) {
    return new TH1F(hname, hname, bins.nBins, bins.lo, bins.hi);
}
 
inline TH2F* MakeTH2F(const TString& hname, const AxisBins& xBins, const AxisBins& yBins) {
    return new TH2F(hname, hname, xBins.nBins, xBins.lo, xBins.hi, yBins.nBins, yBins.lo, yBins.hi);
}

inline TH1I* MakeTH1I(const TString& hname, const AxisBins& bins) {
    return new TH1I(hname, hname, bins.nBins, bins.lo, bins.hi);
}

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

    // kimematics
    std::vector<TH1F*> pt;
    std::vector<TH1F*> eta;
    std::vector<TH1F*> phi;

    // eta-phi maps
    std::vector<TH2F*> etaphi;
    
    // inclusive PF fractions
    std::vector<TH1F*> CHF;
    std::vector<TH1F*> NHF;
    std::vector<TH1F*> CEF;
    std::vector<TH1F*> NEF;
    std::vector<TH1F*> MUF;

    JetHealthStruct(const BinningStruct& bins) {
        InitAxes(bins);
        InitHistograms(bins);
    }

    void InitAxes(const BinningStruct& bins) {
        const std::size_t nhiBin = bins.hiBins.size();
        pt.resize(nhiBin, nullptr);
        eta.resize(nhiBin, nullptr);
        phi.resize(nhiBin, nullptr);
        etaphi.resize(nhiBin, nullptr);
        CHF.resize(nhiBin, nullptr);
        NHF.resize(nhiBin, nullptr);
        CEF.resize(nhiBin, nullptr);
        NEF.resize(nhiBin, nullptr);
        MUF.resize(nhiBin, nullptr);
    }

    void InitHistograms(const BinningStruct& bins) {
        const std::size_t nhiBin = bins.hiBins.size();
        
        vz = MakeTH1F("hvz", bins.vz);
        vz_unpassed = MakeTH1F("hvz_unpassed", bins.vz);
        hiBin = MakeTH1I("hhiBin", bins.hiBin);
        nref = MakeTH1I("hnref", bins.nref);
        pclustF = MakeTH1I("hpclustF", bins.trig);
        ppvF = MakeTH1I("hppvF", bins.trig);
        pphfF = MakeTH1I("hpphfF", bins.trig);
 
        for (std::size_t hb = 0; hb < nhiBin; hb++) {
            const TString& s = bins.hiBins[hb].shortName;
            
            pt[hb] = MakeTH1F("hjtpt"+s, bins.pt);
            eta[hb] = MakeTH1F("hjteta"+s, bins.eta);
            phi[hb] = MakeTH1F("hjtphi"+s, bins.phi);
            etaphi[hb] = MakeTH2F("hjtetaphi"+s, bins.eta, bins.phi);
            CHF[hb] = MakeTH1F("hjtCHF"+s, bins.pfFrac);
            NEF[hb] = MakeTH1F("hjtNEF"+s, bins.pfFrac);
            CEF[hb] = MakeTH1F("hjtCEF"+s, bins.pfFrac);
            MUF[hb] = MakeTH1F("hjtMUF"+s, bins.pfFrac);
            NHF[hb] = MakeTH1F("hjtNHF"+s, bins.pfFrac);
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
        WriteAll(pt);
        WriteAll(eta);
        WriteAll(phi);
        WriteAll(etaphi);
        WriteAll(CHF);
        WriteAll(NHF);
        WriteAll(CEF);
        WriteAll(MUF);
        WriteAll(NEF);
        f->Close();
    }

};


#endif
