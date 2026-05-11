#ifndef HISTOGRAMMAP_H
#define HISTOGRAMMAP_H

#include "TH1F.h"
#include "TH1I.h"
#include "TString.h"
#include "Rtypes.h"

#include "Binning.h"
#include "TriggerMap_PbPb_MC.h"

#include <vector>

inline TH1F* MakeTH1F(const TString& hname, const AxisBins& bins){
    return new TH1F(hname, hname, bins.nBins, bins.lo, bins.hi);
}

inline TH1I* MakeTH1I(const TString& hname, const AxisBins& bins){
    return new TH1I(hname, hname, bins.nBins, bins.lo, bins.hi);
}

struct HistogramStruct{

    // event histograms
    TH1F* vz_unpassed;
    TH1F* vz;
    TH1I* hiBin;
    TH1I* nref;

    // event filter histograms
    TH1I* pclustF;
    TH1I* ppvF;
    TH1I* pphfF;

    // jet histograms //
    // by eta
    std::vector<TH1F*> ljtpt_eta;
    std::vector<std::vector<TH1F*>> l1_ljtpt_eta;
    std::vector<std::vector<std::vector<TH1F*>>> hlt_ljtpt_eta;
    std::vector<std::vector<std::vector<TH1F*>>> l1hlt_ljtpt_eta;
    // by eta and hiBin
    std::vector<std::vector<TH1F*>> ljtpt_hibin;
    std::vector<std::vector<std::vector<TH1F*>>> l1_ljtpt_hibin;
    std::vector<std::vector<std::vector<std::vector<TH1F*>>>> hlt_ljtpt_hibin;
    std::vector<std::vector<std::vector<std::vector<TH1F*>>>> l1hlt_ljtpt_hibin;

    // constructor
    HistogramStruct(const BinningStruct& bins){Init(bins);}

    void Init(const BinningStruct& bins){
        TString hname;

        // intializing event histograms
        vz = MakeTH1F("hvz", bins.vz);
        vz_unpassed = MakeTH1F("hvz_unpassed", bins.vz);
        hiBin = MakeTH1I("hhiBin", bins.hiBin);
        nref = MakeTH1I("hnref", bins.nref);
        pclustF = MakeTH1I("hpclustF", bins.trig);
        ppvF = MakeTH1I("hppvF", bins.trig);
        pphfF = MakeTH1I("hpphfF", bins.trig);

        const std::size_t nEta  = bins.etaBins.size();
        const std::size_t nCent = bins.hiBins.size();

        // resizing jet histograms //
        // by eta
        ljtpt_eta.resize(nEta);
        l1_ljtpt_eta.resize(nEta, std::vector<TH1F*>(nL1T));
        hlt_ljtpt_eta.resize(2, std::vector<std::vector<TH1F*>>(nEta, std::vector<TH1F*>(nHLT)));
        l1hlt_ljtpt_eta.resize(2, std::vector<std::vector<TH1F*>>(nEta, std::vector<TH1F*>(nHLT)));
        // by hiBin
        ljtpt_hibin.resize(nEta, std::vector<TH1F*>(nCent));
        l1_ljtpt_hibin.resize(nEta, std::vector<std::vector<TH1F*>>(nL1T, std::vector<TH1F*>(nCent)));
        hlt_ljtpt_hibin.resize(2, std::vector<std::vector<std::vector<TH1F*>>>(nEta, std::vector<std::vector<TH1F*>>(nHLT, std::vector<TH1F*>(nCent))));
        l1hlt_ljtpt_hibin.resize(2, std::vector<std::vector<std::vector<TH1F*>>>(nEta, std::vector<std::vector<TH1F*>>(nHLT, std::vector<TH1F*>(nCent))));

        // initializing jet histograms //
        for(std::size_t b = 0; b < nEta; b++){
            const auto& etaBin = bins.etaBins[b];

            hname = "hjtpt" + etaBin.shortName;
            ljtpt_eta[b] = MakeTH1F(hname, bins.pt);

            for(std::size_t t = 0; t < nHLT; t++){

                TString hltShort = GetHLTShortName(sHLTrigs[t]);

                hname = "hjtpt" + etaBin.shortName + hltShort;
                hlt_ljtpt_eta[0][b][t] = MakeTH1F(hname, bins.pt);

                hname = "hjtpt_dR" + etaBin.shortName + hltShort;
                hlt_ljtpt_eta[1][b][t] = MakeTH1F(hname, bins.pt);

                hname = "hjtpt" + etaBin.shortName + "_l1t" + hltShort;
                l1hlt_ljtpt_eta[0][b][t] = MakeTH1F(hname, bins.pt);

                hname = "hjtpt_dR" + etaBin.shortName + "_l1t" + hltShort;
                l1hlt_ljtpt_eta[1][b][t] = MakeTH1F(hname, bins.pt);
            }

            for(std::size_t t = 0; t < nL1T; t++){

                TString l1Short = GetL1ShortName(sL1Trigs[t]);

                hname = "hjtpt" + etaBin.shortName + l1Short;
                l1_ljtpt_eta[b][t] = MakeTH1F(hname, bins.pt);
            }

            for(std::size_t hb = 0; hb < nCent; hb++){

                const auto& hiBin = bins.hiBins[hb];
                
                hname = "hjtpt" + etaBin.shortName + hiBin.shortName;
                ljtpt_hibin[b][hb] = MakeTH1F(hname, bins.pt);

                for(std::size_t t = 0; t < nHLT; t++){

                    TString hltShort = GetHLTShortName(sHLTrigs[t]);

                    hname = "hjtpt" + etaBin.shortName + hltShort + hiBin.shortName;
                    hlt_ljtpt_hibin[0][b][t][hb] = MakeTH1F(hname, bins.pt);

                    hname = "hjtpt_dR" + etaBin.shortName + hltShort + hiBin.shortName;
                    hlt_ljtpt_hibin[1][b][t][hb] = MakeTH1F(hname, bins.pt);

                    hname = "hjtpt" + etaBin.shortName + "_l1t" + hltShort + hiBin.shortName;
                    l1hlt_ljtpt_hibin[0][b][t][hb] = MakeTH1F(hname, bins.pt);

                    hname = "hjtpt_dR" + etaBin.shortName + "_l1t" + hltShort + hiBin.shortName;
                    l1hlt_ljtpt_hibin[1][b][t][hb] = MakeTH1F(hname, bins.pt);
                }

                for(std::size_t t = 0; t < nL1T; t++){

                    TString l1Short = GetL1ShortName(sL1Trigs[t]);

                    hname = "hjtpt" + etaBin.shortName + l1Short + hiBin.shortName;
                    l1_ljtpt_hibin[b][t][hb] = MakeTH1F(hname, bins.pt);
                }
            }
        }
    }
};

#endif
