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
 
#include "Binning.h"
#include "TriggerMap_PbPb_MC.h"
#include "JetSpectraHistograms.h"
#include "JetEfficiency.h"

struct PlotConfig{
    TString runNumber = "";
    TString globalTag = "";
    TString jetAlgo = "akCs4PF";
    float ptmin = 20.0;
    float ptmax = 300.0;
    float effmax = 1.4;
    float legxmin = 0.1;
    float legxmax = 0.9;
    float legymin = 0.68;
    float legymax = 0.9;
};

inline TCanvas* MakeCanvas(const TString& name) {
    TCanvas* c = new TCanvas(name, "", 2400, 2400);
    return c;
}

inline TLegend* MakeLegend(float xmin, float ymin, float xmax, float ymax) {
    TLegend* l = new TLegend(xmin, ymin, xmax, ymax);
    l->SetBorderSize(0);
    l->SetFillStyle(0);
    return l;
}

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
}

inline void SaveEfficiencyCanvas(const JetEfficiencyOutputStruct& out, const BinningStruct& bins, JetSpectraStruct::MatchType matchType, JetEfficiencyOutputStruct::EfficiencyType effType, std::size_t etaIndex, const TString& outDir, const PlotConfig& cfg){
    const auto& etaBin = bins.etaBins[etaIndex];
    TString hname = "heff" + JetSpectraStruct::MatchSuffix(matchType) + "_" + JetEfficiencyOutputStruct::EfficiencyName(effType) + etaBin.shortName;

    TCanvas* c = MakeCanvas(hname);
    c->cd();

    std::vector<TGraphAsymmErrors*> clones(nHLT, nullptr);
    for(std::size_t t=0; t<nHLT; t++){
        clones[t] = (TGraphAsymmErrors*)out.jetEfficiencies_inclusive[matchType][effType][etaIndex][t]->Clone();
        StyleGraph(clones[t], t);
    }

    clones[0]->Draw("ap");
    clones[0]->SetTitle("");
    clones[0]->GetXaxis()->SetTitle("p_{T,leading jet} (GeV)");
    clones[0]->GetYaxis()->SetTitle(effType == JetEfficiencyOutputStruct::kFull ? "HLT + L1seed + minBias / minBias" : "HLT + L1seed + minBias / L1seed + minBias");
    clones[0]->GetXaxis()->CenterTitle(true);
    clones[0]->GetYaxis()->CenterTitle(true);
    clones[0]->SetMinimum(0.0);
    clones[0]->SetMaximum(cfg.effmax);
    clones[0]->GetXaxis()->SetRangeUser(cfg.ptmin, cfg.ptmax);

    for(std::size_t t=1; t<nHLT; t++){clones[t]->Draw("p same");}
    DrawRefLine(cfg.ptmin, cfg.ptmax);

    TLegend* l = MakeLegend(cfg.legxmin, cfg.legymin, cfg.legxmax, cfg.legymax);
    for(std::size_t t=0; t<nHLT; t++){l->AddEntry(clones[t], sHLTrigs[t], "lp");}
    if(!cfg.runNumber.IsNull()){l->AddEntry((TObject*)0, cfg.runNumber, "");}
    if(!cfg.globalTag.IsNull()){l->AddEntry((TObject*)0, cfg.globalTag, "");}
    if(!cfg.jetAlgo.IsNull()){l->AddEntry((TObject*)0, cfg.jetAlgo,   "");}
    l->AddEntry((TObject*)0, etaBin.title, "");
    if(matchType == JetSpectraStruct::kDR){l->AddEntry((TObject*)0, "#Delta R < 0.3", "");}
    l->Draw("same");

    c->SaveAs(outDir + "/" + hname + ".png");
 
    delete c;
    delete l;
}

inline void SaveEfficiencyCanvas_hiBin(const JetEfficiencyOutputStruct& out, const BinningStruct& bins, JetSpectraStruct::MatchType matchType, JetEfficiencyOutputStruct::EfficiencyType effType, std::size_t etaIndex, std::size_t hiBinIndex, const TString& outDir, const PlotConfig& cfg){
    const auto& etaBin = bins.etaBins[etaIndex];
    const auto& hiBin  = bins.hiBins[hiBinIndex];

    TString hname = "heff" + JetSpectraStruct::MatchSuffix(matchType) + "_" + JetEfficiencyOutputStruct::EfficiencyName(effType) + etaBin.shortName + hiBin.shortName;

    TCanvas* c = MakeCanvas(hname);
    c->cd();

    std::vector<TGraphAsymmErrors*> clones(nHLT, nullptr);
    for(std::size_t t=0; t<nHLT; t++){
        clones[t] = (TGraphAsymmErrors*)out.jetEfficiencies_hibin[matchType][effType][etaIndex][hiBinIndex][t]->Clone();
        StyleGraph(clones[t], t);
    }

    clones[0]->Draw("ap");
    clones[0]->SetTitle("");
    clones[0]->GetXaxis()->SetTitle("p_{T,leading jet} (GeV)");
    clones[0]->GetYaxis()->SetTitle(effType == JetEfficiencyOutputStruct::kFull ? "HLT / MinBias" : "HLT / L1 Seed");
    clones[0]->GetXaxis()->CenterTitle(true);
    clones[0]->GetYaxis()->CenterTitle(true);
    clones[0]->SetMinimum(0.0);
    clones[0]->SetMaximum(cfg.effmax);
    clones[0]->GetXaxis()->SetRangeUser(cfg.ptmin, cfg.ptmax);

    for(std::size_t t=1; t<nHLT; t++){clones[t]->Draw("p same");}
    DrawRefLine(cfg.ptmin, cfg.ptmax);

    TLegend* l = MakeLegend(cfg.legxmin, cfg.legymin, cfg.legxmax, cfg.legymax);
    for(std::size_t t=0; t<nHLT; t++){l->AddEntry(clones[t], sHLTrigs[t], "lp");}
    if(!cfg.runNumber.IsNull()){l->AddEntry((TObject*)0, cfg.runNumber, "");}
    if(!cfg.globalTag.IsNull()){l->AddEntry((TObject*)0, cfg.globalTag, "");}
    if(!cfg.jetAlgo.IsNull()){l->AddEntry((TObject*)0, cfg.jetAlgo,   "");}
    l->AddEntry((TObject*)0, hiBin.title, "");
    l->AddEntry((TObject*)0, etaBin.title, "");
    if(matchType == JetSpectraStruct::kDR){l->AddEntry((TObject*)0, "#Delta R < 0.3", "");}
    l->Draw("same");

    c->SaveAs(outDir + "/" + hname + ".png");
 
    delete c;
    delete l;
}

inline void SaveEfficiencyPlots( const JetEfficiencyOutputStruct& out, const BinningStruct& bins, const PlotConfig& cfg){
    gStyle->SetImageScaling(3.0);
    gStyle->SetTitleSize(0.04, "XY");
    gStyle->SetLabelSize(0.04, "XY");

    TDatime now;
    TString timestamp = Form("%04d-%02d-%02d_%02d-%02d-%02d", now.GetYear(), now.GetMonth(), now.GetDay(), now.GetHour(), now.GetMinute(), now.GetSecond());
    TString plotsBase = "plots_" + timestamp;
    gSystem->mkdir(plotsBase, true);
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
                SaveEfficiencyCanvas(out, bins, matchType, effType, b, outDir, cfg);
                for(std::size_t hb=0; hb<nhiBin; hb++){SaveEfficiencyCanvas_hiBin(out, bins, matchType, effType, b, hb, outDir, cfg);}
            }
        }
    }
    std::cout<<"all efficiency plots saved to "<<plotsBase<<std::endl;
}

#endif