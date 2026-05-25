#ifndef JETEFFICIENCY_H
#define JETEFFICIENCY_H
 
#include "TFile.h"
#include "TH1F.h"
#include "TGraphAsymmErrors.h"
#include "TString.h"

#include <vector>
#include <stdexcept>

#include "Binning.h"
#include "JetSpectraHistograms.h"
#include "JetTriggers_2026PbPb_MC.h"
// #include "JetTriggers_2025PbPb.h"

inline void GetHistograms(TFile* fi, const BinningStruct& bins, JetSpectraStruct& hists){

    auto GetTH1F = [&](const TString& hname) -> TH1F* {
        TH1F* h = (TH1F*)fi->Get(hname);
        if(!h){throw std::runtime_error(Form("ERROR: Could not find histogram '%s' in file", hname.Data()));}
        return h;
    };

    const std::size_t nEta = bins.etaBins.size();
    const std::size_t nhiBin = bins.hiBins.size();

    // looping through eta bins
    for(std::size_t b=0; b<nEta; b++){
        const auto& etaBin = bins.etaBins[b];

        // looping through hiBins
        for (std::size_t hb=0; hb<nhiBin; hb++){
            const auto& hiBin = bins.hiBins[hb];
            hists.ljtpt[b][hb] = GetTH1F("hjtpt" + etaBin.shortName + hiBin.shortName);

            for (std::size_t t=0; t<nHLT; t++){
                TString hltShort = GetHLTShortName(sHLTrigs[t]);

                for (int m=0; m<JetSpectraStruct::kNMatchTypes; m++){
                    auto matchType = static_cast<JetSpectraStruct::MatchType>(m);
                    hists.hlt_ljtpt[matchType][b][t][hb] = GetTH1F("hjtpt" + JetSpectraStruct::MatchSuffix(matchType) + etaBin.shortName + hltShort + hiBin.shortName);
                }
            }

            for (std::size_t t=0; t<nL1T; t++) {
                TString l1Short = GetL1ShortName(sL1Trigs[t]);
                hists.l1_ljtpt[b][t][hb] = GetTH1F("hjtpt" + etaBin.shortName + l1Short + hiBin.shortName);
            }
        }
    }
}

struct JetEfficiencyOutputStruct{
    
    enum EfficiencyType{
        kFull = 0, // HLT+L1seed+minBias/minBias
        kRelative = 1, // HLT+L1seed+minBias/L1seed+minBias
        kNEfficiencyTypes = 2
    };

    static TString EfficiencyName(EfficiencyType effType){
        if(effType == kFull){return "full";}
        if(effType == kRelative){return "relative";}
        return "unknown";
    }

    // [MatchType][EfficiencyType][Eta][hiBin][HLT]
    std::vector<std::vector<std::vector<std::vector<std::vector<TGraphAsymmErrors*>>>>> jetEfficiencies;

    JetEfficiencyOutputStruct(const BinningStruct& bins){Init(bins);}

    void Init(const BinningStruct& bins){
        const std::size_t nEta = bins.etaBins.size();
        const std::size_t nhiBin = bins.hiBins.size();

        jetEfficiencies.resize(JetSpectraStruct::kNMatchTypes,std::vector<std::vector<std::vector<std::vector<TGraphAsymmErrors*>>>>(kNEfficiencyTypes,std::vector<std::vector<std::vector<TGraphAsymmErrors*>>>(nEta,std::vector<std::vector<TGraphAsymmErrors*>>(nhiBin,std::vector<TGraphAsymmErrors*>(nHLT, nullptr)))));
    }
    
    // generates efficiencies from jet spectra
    void ComputeEfficiencies(const JetSpectraStruct& hists, const BinningStruct& bins){
        const std::size_t nEta = bins.etaBins.size();
        const std::size_t nhiBin = bins.hiBins.size();

        // looping over dR matching type
        for(int m=0; m<JetSpectraStruct::kNMatchTypes; m++){
            auto matchType = static_cast<JetSpectraStruct::MatchType>(m);

            // looping over efficiency type
            for(int e=0; e<kNEfficiencyTypes; e++){
                auto effType = static_cast<EfficiencyType>(e);

                // looping over eta
                for(std::size_t b=0; b<nEta; b++){

                    // looping over HLT
                    for(std::size_t t=0; t<nHLT; t++){

                        // looping over hiBin
                        for(std::size_t hb=0; hb<nhiBin; hb++){
                            TH1F* denom = (effType == kFull) ? hists.ljtpt[b][hb] : hists.l1_ljtpt[b][L1SeedHLT[t]][hb];
                            jetEfficiencies[matchType][effType][b][hb][t] = new TGraphAsymmErrors(hists.hlt_ljtpt[matchType][b][t][hb], denom, "cl=0.683 b(1,1) mode");
                        }
                    }
                }
            }
        }
    }

    void Write(TFile* f){
        f->cd();
        WriteAll(jetEfficiencies);
        f->Close();
    }
};

#endif
