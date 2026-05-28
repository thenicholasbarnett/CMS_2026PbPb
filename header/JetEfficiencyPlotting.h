#ifndef JETEFFICIENCYPLOTTING_H
#define JETEFFICIENCYPLOTTING_H

#include "TCanvas.h"
#include "TDatime.h"
#include "TGraphAsymmErrors.h"
#include "TLegend.h"
#include "TLine.h"
#include "TString.h"
#include "TStyle.h"
#include "TSystem.h"
 
#include <vector>
#include <array>
#include <stdexcept>
#include <iostream>
 
#include "Binning.h"
#include "JetEfficiency.h"
#include "Utilities.h"
#include "JetSpectraHistograms.h"
#include "JetTriggers_2026PbPb_MC.h"
// #include "JetTriggers_2025PbPb.h"

inline void DrawRefLine(float xmin, float xmax, float y = 1.0) {
    TLine* line = new TLine(xmin, y, xmax, y);
    line->SetLineWidth(1);
    line->SetLineColor(kBlack);
    line->SetLineStyle(2);
    line->Draw("same");
}

inline void StyleGraph(TGraphAsymmErrors* g, std::size_t triggerIndex) {
    static const std::array<int, 8> colors = {kBlack, kBlue, kRed, kGreen+2, kMagenta, kOrange+1, kCyan+1, kViolet};
    if(triggerIndex >= colors.size()){throw std::runtime_error(Form("StyleGraph: trigger index %zu exceeds available colours", triggerIndex));}
    g->SetMarkerColor(colors[triggerIndex]);
    g->SetLineColor(colors[triggerIndex]);
    g->SetMarkerStyle(20);
    g->SetMarkerSize(1.5);
    g->SetLineWidth(3);
}

inline void DrawLegends(TPad* legpad, const std::vector<TGraphAsymmErrors*>& clones, const PlotConfig& cfg, const BinningStruct& bins, JetSpectraStruct::MatchType matchType, std::size_t etaIndex, std::size_t hiBinIndex){
    legpad->cd();

    const auto& etaBin = bins.etaBins[etaIndex];
    const auto& hiBin  = bins.hiBins[hiBinIndex];

    // trigger legend
    TLegend* ltrig = MakeLegend(0.02, 0.03, 0.65, 0.97);
    ltrig->SetNColumns(1);
    for(std::size_t t=0; t<nHLT; t++){ltrig->AddEntry(clones[t], sHLTrigs[t], "lp");}
    ltrig->Draw("same");

    // info legend
    TLegend* linfo = MakeLegend(0.65, 0.03, 0.98, 0.97);
    AddInfoEntries(linfo, cfg);
    linfo->AddEntry((TObject*)nullptr, hiBin.title,  "");
    linfo->AddEntry((TObject*)nullptr, etaBin.title, "");
    if(matchType == JetSpectraStruct::kDR){linfo->AddEntry((TObject*)0, "#Delta R < 0.3", "");}
    linfo->Draw("same");
}

inline void SaveEfficiencyCanvas(const JetEfficiencyOutputStruct& out, const BinningStruct& bins, JetSpectraStruct::MatchType matchType, JetEfficiencyOutputStruct::EfficiencyType effType, std::size_t etaIndex, std::size_t hiBinIndex, const TString& outDir, const PlotConfig& cfg){
    const auto& etaBin = bins.etaBins[etaIndex];
    const auto& hiBin  = bins.hiBins[hiBinIndex];

    TString hname = "heff" + JetSpectraStruct::MatchSuffix(matchType) + "_" + JetEfficiencyOutputStruct::EfficiencyName(effType) + etaBin.shortName + hiBin.shortName;

    SplitCanvas cs = MakeSplitPadCanvas(hname, cfg);

    std::vector<TGraphAsymmErrors*> clones(nHLT, nullptr);
    for(std::size_t t=0; t<nHLT; t++){
        clones[t] = (TGraphAsymmErrors*)out.jetEfficiencies[matchType][effType][etaIndex][hiBinIndex][t]->Clone();
        StyleGraph(clones[t], t);
    }

    cs.plotpad->cd();
    clones[0]->Draw("ap");
    clones[0]->SetTitle("");
    clones[0]->GetXaxis()->SetTitle("p_{T,leading jet} (GeV)");

    TString yTitle;
    if(effType == JetEfficiencyOutputStruct::kFull) yTitle = "HLT + MinBias / MinBias";
    else if(effType == JetEfficiencyOutputStruct::kRelative) yTitle = "HLT + MinBias / L1seed + MinBias";
    else yTitle = "L1seed / MinBias";
    clones[0]->GetYaxis()->SetTitle(yTitle);

    clones[0]->GetXaxis()->CenterTitle(true);
    clones[0]->GetYaxis()->CenterTitle(true);
    clones[0]->GetXaxis()->SetTitleOffset(1.0);
    clones[0]->GetYaxis()->SetTitleOffset(1.0);
    clones[0]->SetMinimum(cfg.ymin);
    clones[0]->SetMaximum(cfg.ymax);
    clones[0]->GetXaxis()->SetRangeUser(cfg.xmin, cfg.xmax);

    for(std::size_t t=1; t<nHLT; t++){clones[t]->Draw("p same");}
    DrawRefLine(cfg.xmin, cfg.xmax);
    DrawLegends(cs.legpad, clones, cfg, bins, matchType, etaIndex, hiBinIndex);

    cs.c->SaveAs(outDir + "/" + hname + ".png");
    delete cs.c;
}

inline void SaveEfficiencyPlots(const JetEfficiencyOutputStruct& out, const BinningStruct& bins, const PlotConfig& cfg){
    SetPlotStyle(cfg);

    TString plotsBase = MakePlotDir("plots");
    std::cout << "saving plots to " << plotsBase << std::endl;


    const std::size_t nEta = bins.etaBins.size();
    const std::size_t nhiBin = bins.hiBins.size();

    for(int m=0; m<JetSpectraStruct::kNMatchTypes; m++){
        auto matchType = static_cast<JetSpectraStruct::MatchType>(m);

        for(int e=0; e<JetEfficiencyOutputStruct::kNEfficiencyTypes; e++){
            auto effType = static_cast<JetEfficiencyOutputStruct::EfficiencyType>(e);

            TString matchStr = JetSpectraStruct::MatchSuffix(matchType).Strip(TString::kLeading, '_');
            TString effStr = JetEfficiencyOutputStruct::EfficiencyName(effType);
            TString subDir = matchStr.IsNull() ? effStr : matchStr + "_" + effStr;
            TString outDir = plotsBase + "/" + subDir;
            gSystem->mkdir(outDir, true);

            for(std::size_t b=0; b<nEta; b++){
                for(std::size_t hb=0; hb<nhiBin; hb++){SaveEfficiencyCanvas(out, bins, matchType, effType, b, hb, outDir, cfg);}
            }
        }
    }
    std::cout<<"all efficiency plots saved to "<<plotsBase<<std::endl;
}

#endif
