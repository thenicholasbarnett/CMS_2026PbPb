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
#include "JetTriggers_PbPb_MC.h"

inline void GetHistograms(TFile* fi, const BinningStruct& bins, JetSpectraStruct& hists){

    auto GetTH1F = [&](const TString& hname) -> TH1F* {
        TH1F* h = (TH1F*)fi->Get(hname);
        if(!h){throw std::runtime_error(Form("ERROR: Could not find histogram '%s' in file", hname.Data()));}
        return h;
    };

    const TString ji = "jet_inclusive/hjtpt";
    const TString jh = "jet_hibin/hjtpt";
    const std::size_t nEta = bins.etaBins.size();
    const std::size_t nhiBin = bins.hiBins.size();

    // looping through eta bins
    for(std::size_t b=0; b<nEta; b++){
        const auto& etaBin = bins.etaBins[b];
        hists.ljtpt_eta[b] = GetTH1F(ji + etaBin.shortName);

        // looping through HLT
        for (std::size_t t=0; t<nHLT; t++){
            TString hltShort = GetHLTShortName(sHLTrigs[t]);

            // looping through dR matching types
            for (int m=0; m<JetSpectraStruct::kNMatchTypes; m++){
                auto matchType = static_cast<JetSpectraStruct::MatchType>(m);
                hists.hlt_ljtpt_eta[matchType][b][t] = GetTH1F(ji + JetSpectraStruct::MatchSuffix(matchType) + etaBin.shortName + hltShort);
            }
        }

        // looping through L1T
        for (std::size_t t = 0; t < nL1T; t++){
            TString l1Short = GetL1ShortName(sL1Trigs[t]);
            hists.l1_ljtpt_eta[b][t] = GetTH1F(ji + etaBin.shortName + l1Short);
        }

        // looping through hiBins
        for (std::size_t hb=0; hb<nhiBin; hb++){
            const auto& hiBin = bins.hiBins[hb];
            hists.ljtpt_hibin[b][hb] = GetTH1F(jh + etaBin.shortName + hiBin.shortName);

            for (std::size_t t=0; t<nHLT; t++){
                TString hltShort = GetHLTShortName(sHLTrigs[t]);

                for (int m=0; m<JetSpectraStruct::kNMatchTypes; m++){
                    auto matchType = static_cast<JetSpectraStruct::MatchType>(m);
                    hists.hlt_ljtpt_hibin[matchType][b][t][hb] = GetTH1F(jh + JetSpectraStruct::MatchSuffix(matchType) + etaBin.shortName + hltShort + hiBin.shortName);
                }
            }

            for (std::size_t t=0; t<nL1T; t++) {
                TString l1Short = GetL1ShortName(sL1Trigs[t]);
                hists.l1_ljtpt_hibin[b][t][hb] = GetTH1F(jh + etaBin.shortName + l1Short + hiBin.shortName);
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

    // [MatchType][EfficiencyType][Eta][HLT]
    std::vector<std::vector<std::vector<std::vector<TGraphAsymmErrors*>>>> jetEfficiencies_inclusive;

    // [MatchType][EfficiencyType][Eta][hiBin][HLT]
    std::vector<std::vector<std::vector<std::vector<std::vector<TGraphAsymmErrors*>>>>> jetEfficiencies_hibin;

    JetEfficiencyOutputStruct() = default;
    JetEfficiencyOutputStruct(const BinningStruct& bins){Init(bins);}

    void Init(const BinningStruct& bins){
        const std::size_t nEta = bins.etaBins.size();
        const std::size_t nhiBin = bins.hiBins.size();

        jetEfficiencies_inclusive.resize(JetSpectraStruct::kNMatchTypes,std::vector<std::vector<std::vector<TGraphAsymmErrors*>>>(kNEfficiencyTypes,std::vector<std::vector<TGraphAsymmErrors*>>(nEta, std::vector<TGraphAsymmErrors*>(nHLT, nullptr))));
        jetEfficiencies_hibin.resize(JetSpectraStruct::kNMatchTypes,std::vector<std::vector<std::vector<std::vector<TGraphAsymmErrors*>>>>(kNEfficiencyTypes,std::vector<std::vector<std::vector<TGraphAsymmErrors*>>>(nEta,std::vector<std::vector<TGraphAsymmErrors*>>(nhiBin,std::vector<TGraphAsymmErrors*>(nHLT, nullptr)))));
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

                        // ternary operator
                        TH1F* denom_eta = (effType == kFull) ? hists.ljtpt_eta[b] : hists.l1_ljtpt_eta[b][L1SeedHLT[t]];

                        // performing ratio to get jet efficiency
                        jetEfficiencies_inclusive[matchType][effType][b][t] = new TGraphAsymmErrors(hists.hlt_ljtpt_eta[matchType][b][t], denom_eta, "cl=0.683 b(1,1) mode");

                        // looping over hiBin
                        for(std::size_t hb=0; hb<nhiBin; hb++){
                            TH1F* denom_hibin = (effType == kFull) ? hists.ljtpt_hibin[b][hb] : hists.l1_ljtpt_hibin[b][L1SeedHLT[t]][hb];
                            jetEfficiencies_hibin[matchType][effType][b][hb][t] = new TGraphAsymmErrors(hists.hlt_ljtpt_hibin[matchType][b][t][hb], denom_hibin, "cl=0.683 b(1,1) mode");
                        }
                    }
                }
            }
        }
    }
};

#endif