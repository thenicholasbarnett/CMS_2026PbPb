#ifndef JETSPECTRAHISTOGRAMS_H
#define JETSPECTRAHISTOGRAMS_H

#include "TH1F.h"
#include "TH1I.h"
#include "TString.h"
#include "Rtypes.h"

#include <vector>
#include <cstddef>

#include "Binning.h"
// #include "../header/JetTriggers_2026PbPb_MC.h"
#include "../header/JetTriggers_2025PbPb.h"

inline TH1F* MakeTH1F(const TString& hname, const AxisBins& bins) {
    return new TH1F(hname, hname, bins.nBins, bins.lo, bins.hi);
}

inline TH1I* MakeTH1I(const TString& hname, const AxisBins& bins) {
    return new TH1I(hname, hname, bins.nBins, bins.lo, bins.hi);
}

struct JetSpectraStruct{

    enum MatchType {
        kNoDR = 0, // no offline-HLT object matching
        kDR = 1, // offline leading jet matched to HLT object
        kNMatchTypes = 2
    };

    static TString MatchSuffix(MatchType matchType) {
        return matchType == kDR ? "_dR" : "";
    }

    // event histograms
    TH1F* vz_unpassed = nullptr;
    TH1F* vz = nullptr;
    TH1I* hiBin = nullptr;
    TH1I* nref = nullptr;

    // event filter histograms
    TH1I* pclustF = nullptr;
    TH1I* ppvF = nullptr;
    TH1I* pphfF = nullptr;

    // leading jet pT histograms

    // [Eta]
    std::vector<TH1F*> ljtpt_eta;
    // [Eta][L1]
    std::vector<std::vector<TH1F*>> l1_ljtpt_eta;
    // [MatchType][Eta][HLT]
    std::vector<std::vector<std::vector<TH1F*>>> hlt_ljtpt_eta;
    // [Eta][hiBin]
    std::vector<std::vector<TH1F*>> ljtpt_hibin;
    // [Eta][L1][hiBin]
    std::vector<std::vector<std::vector<TH1F*>>> l1_ljtpt_hibin;
    // [MatchType][Eta][HLT][hiBin]
    std::vector<std::vector<std::vector<std::vector<TH1F*>>>> hlt_ljtpt_hibin;

    JetSpectraStruct() = default;
    JetSpectraStruct(const BinningStruct& bins){
        InitAxes(bins);
        InitHistograms(bins);
    }
    JetSpectraStruct(const BinningStruct& bins, bool allocate){
        InitAxes(bins);
        if(allocate){InitHistograms(bins);}
    }
    void InitAxes(const BinningStruct& bins){
        const std::size_t nEta = bins.etaBins.size();
        const std::size_t nhiBin = bins.hiBins.size();
        // [Eta]
        ljtpt_eta.resize(nEta, nullptr);
        // [Eta][L1]
        l1_ljtpt_eta.resize(nEta,std::vector<TH1F*>(nL1T, nullptr));
        // [MatchType][Eta][HLT]
        hlt_ljtpt_eta.resize(kNMatchTypes,std::vector<std::vector<TH1F*>>(nEta,std::vector<TH1F*>(nHLT, nullptr)));
        // [Eta][hiBin]
        ljtpt_hibin.resize(nEta,std::vector<TH1F*>(nhiBin, nullptr));
        // [Eta][L1][hiBin]
        l1_ljtpt_hibin.resize(nEta,std::vector<std::vector<TH1F*>>(nL1T,std::vector<TH1F*>(nhiBin, nullptr)));
        // [MatchType][Eta][HLT][hiBin]
        hlt_ljtpt_hibin.resize(kNMatchTypes,std::vector<std::vector<std::vector<TH1F*>>>(nEta,std::vector<std::vector<TH1F*>>(nHLT,std::vector<TH1F*>(nhiBin, nullptr))));
    }

    void InitHistograms(const BinningStruct& bins) {
        TString hname;
 
        const std::size_t nEta   = bins.etaBins.size();
        const std::size_t nhiBin = bins.hiBins.size();
        
        vz = MakeTH1F("hvz", bins.vz);
        vz_unpassed = MakeTH1F("hvz_unpassed", bins.vz);
        hiBin = MakeTH1I("hhiBin", bins.hiBin);
        nref = MakeTH1I("hnref", bins.nref);
        pclustF = MakeTH1I("hpclustF", bins.trig);
        ppvF = MakeTH1I("hppvF", bins.trig);
        pphfF = MakeTH1I("hpphfF", bins.trig);

        
        for (std::size_t b=0; b<nEta; b++){
            const auto& etaBin = bins.etaBins[b]; 

            hname = "hjtpt" + etaBin.shortName;
            ljtpt_eta[b] = MakeTH1F(hname, bins.pt);

            for (std::size_t t = 0; t < nHLT; t++){
                TString hltShort = GetHLTShortName(sHLTrigs[t]);

                for (int m=0; m<kNMatchTypes; m++){
                    MatchType matchType = static_cast<MatchType>(m);
                    hname = "hjtpt" + MatchSuffix(matchType) + etaBin.shortName + hltShort;
                    hlt_ljtpt_eta[matchType][b][t] = MakeTH1F(hname, bins.pt);
                }
            }

            for (std::size_t t=0; t<nL1T; t++){
                TString l1Short = GetL1ShortName(sL1Trigs[t]);
                hname = "hjtpt" + etaBin.shortName + l1Short;
                l1_ljtpt_eta[b][t] = MakeTH1F(hname, bins.pt);
            }

            for (std::size_t hb=0; hb<nhiBin; hb++){
                const auto& hiBin = bins.hiBins[hb];

                hname = "hjtpt" + etaBin.shortName + hiBin.shortName;
                ljtpt_hibin[b][hb] = MakeTH1F(hname, bins.pt);

                for (std::size_t t=0; t<nHLT; t++){
                    TString hltShort = GetHLTShortName(sHLTrigs[t]);

                    for (int m=0; m<kNMatchTypes; m++){
                        MatchType matchType = static_cast<MatchType>(m);
                        hname = "hjtpt" + MatchSuffix(matchType) + etaBin.shortName + hltShort + hiBin.shortName;
                        hlt_ljtpt_hibin[matchType][b][t][hb] = MakeTH1F(hname, bins.pt);
                    }
                }
                
                for (std::size_t t=0; t<nL1T; t++){
                    TString l1Short = GetL1ShortName(sL1Trigs[t]);
                    hname = "hjtpt"+ etaBin.shortName+ l1Short+ hiBin.shortName;
                    l1_ljtpt_hibin[b][t][hb] = MakeTH1F(hname, bins.pt);
                }
            }
        }
    }
};

#endif
