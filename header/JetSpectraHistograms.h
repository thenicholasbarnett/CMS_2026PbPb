#ifndef JETSPECTRAHISTOGRAMS_H
#define JETSPECTRAHISTOGRAMS_H

#include "TH1F.h"
#include "TH1I.h"
#include "TString.h"
#include "Rtypes.h"

#include <vector>
#include <cstddef>

#include "Binning.h"
#include "Utilities.h"
#include "JetTriggers_2026PbPb_MC.h"
// #include "JetTriggers_2025PbPb.h"

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
    // [Eta][hiBin]
    std::vector<std::vector<TH1F*>> ljtpt;
    // [Eta][L1][hiBin]
    std::vector<std::vector<std::vector<TH1F*>>> l1_ljtpt;
    // [MatchType][Eta][HLT][hiBin]
    std::vector<std::vector<std::vector<std::vector<TH1F*>>>> hlt_ljtpt;

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
        // [Eta][hiBin]
        ljtpt.resize(nEta,std::vector<TH1F*>(nhiBin, nullptr));
        // [Eta][L1][hiBin]
        l1_ljtpt.resize(nEta,std::vector<std::vector<TH1F*>>(nL1T,std::vector<TH1F*>(nhiBin, nullptr)));
        // [MatchType][Eta][HLT][hiBin]
        hlt_ljtpt.resize(kNMatchTypes,std::vector<std::vector<std::vector<TH1F*>>>(nEta,std::vector<std::vector<TH1F*>>(nHLT,std::vector<TH1F*>(nhiBin, nullptr))));
    }

    void InitHistograms(const BinningStruct& bins) {
        TString hname;
 
        const std::size_t nEta = bins.etaBins.size();
        const std::size_t nhiBin = bins.hiBins.size();
        
        vz = MakeTH1<TH1F>("hvz", bins.vz);
        vz_unpassed = MakeTH1<TH1F>("hvz_unpassed", bins.vz);
        hiBin = MakeTH1<TH1I>("hhiBin", bins.hiBin);
        nref = MakeTH1<TH1I>("hnref", bins.nref);
        pclustF = MakeTH1<TH1I>("hpclustF", bins.trig);
        ppvF = MakeTH1<TH1I>("hppvF", bins.trig);
        pphfF = MakeTH1<TH1I>("hpphfF", bins.trig);

        
        for (std::size_t b=0; b<nEta; b++){
            const auto& etaBin = bins.etaBins[b];

            for (std::size_t hb=0; hb<nhiBin; hb++){
                const auto& hiBin = bins.hiBins[hb];

                hname = "hjtpt" + etaBin.shortName + hiBin.shortName;
                ljtpt[b][hb] = MakeTH1<TH1F>(hname, bins.pt);

                for (std::size_t t=0; t<nHLT; t++){
                    TString hltShort = GetHLTShortName(sHLTrigs[t]);

                    for (int m=0; m<kNMatchTypes; m++){
                        MatchType matchType = static_cast<MatchType>(m);
                        hname = "hjtpt" + MatchSuffix(matchType) + etaBin.shortName + hltShort + hiBin.shortName;
                        hlt_ljtpt[matchType][b][t][hb] = MakeTH1<TH1F>(hname, bins.pt);
                    }
                }
                
                for (std::size_t t=0; t<nL1T; t++){
                    TString l1Short = GetL1ShortName(sL1Trigs[t]);
                    hname = "hjtpt"+ etaBin.shortName+ l1Short+ hiBin.shortName;
                    l1_ljtpt[b][t][hb] = MakeTH1<TH1F>(hname, bins.pt);
                }
            }
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
        WriteAll(ljtpt);
        WriteAll(l1_ljtpt);
        WriteAll(hlt_ljtpt);
    }
};

#endif
