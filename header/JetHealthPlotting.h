#ifndef JETHEALTHPLOTTING_H
#define JETHEALTHPLOTTING_H

#include "TCanvas.h"
#include "TDatime.h"
#include "TH1.h"
#include "TH2.h"
#include "TLegend.h"
#include "TString.h"
#include "TStyle.h"
#include "TSystem.h"
#include "THnSparse.h"
 
#include "Binning.h"
#include "Utilities.h"

#include "JetHealthHistograms.h"

struct JetHealthPlotConfig : PlotConfig {std::vector<Float_t> etaPhiPtCuts = {20.0, 100.0, 200.0};};

inline void SaveKinPlot(THnSparseF* hkin,Int_t axis,const TString& axisLabel,const TString& plotName,const BinningStruct& bins,const JetHealthPlotConfig& cfg,const TString& outDir){
    TCanvas* c = MakeSinglePadCanvas(plotName, cfg, false);
    TLegend* l = MakeLegend();
 
    const std::size_t nhiBin = bins.hiBins.size();
    std::vector<TH1D*> hists;
    double ymax = 0.0;

    for(std::size_t hb = 0; hb < nhiBin; hb++){
        const auto& hiBin = bins.hiBins.at(hb);
        TString suf = Form("_%s_hb%zu", plotName.Data(), hb);
        TH1D* h = ProjectTHn1D(hkin, axis, {{3, hiBin.lo, hiBin.hi}}, suf);
        StyleTH1(h, hiBin.color);
        ymax = std::max(ymax, (double)h->GetMaximum());
        hists.push_back(h);
        l->AddEntry(h, hiBin.title, "l");
    }

    hists.at(0)->GetYaxis()->SetRangeUser(0.0, ymax * 1.35);
    hists.at(0)->GetXaxis()->SetTitle(axisLabel);
    hists.at(0)->GetXaxis()->CenterTitle(true);
    hists.at(0)->GetXaxis()->SetTitleOffset(1.1);
    hists.at(0)->GetYaxis()->SetTitleOffset(1.3);
    for(std::size_t hb = 0; hb < nhiBin; hb++){hists.at(hb)->Draw(hb == 0 ? "hist" : "hist same");}

    AddInfoEntries(l, cfg);
    l->Draw("same");
    c->SaveAs(outDir + "/" + plotName + ".png");

    for(auto* h : hists){delete h;}
    delete c;
}

inline void SavePFPlot(THnSparseF* hpf, Int_t pfType, const BinningStruct& bins, const JetHealthPlotConfig& cfg, const TString& outDir) {
    TString plotName = Form("hpf_%s", PFTypeNames.at(pfType));
    TCanvas* c = MakeSinglePadCanvas(plotName, cfg, true);
    TLegend* l = MakeLegend();

    const std::size_t nhiBin = bins.hiBins.size();
    std::vector<TH1D*> hists;

    double ymax = 0.0;
    for (std::size_t hb = 0; hb < nhiBin; hb++) {
        const auto& hiBin = bins.hiBins.at(hb);
        TString suf = Form("_%s_hb%zu", plotName.Data(), hb);
        TH1D* h = ProjectTHn1D(hpf, 0, {{1, (double)pfType, pfType + 1.0}, {3, hiBin.lo, hiBin.hi}}, suf);
        StyleTH1(h, hiBin.color);
        NormalizeTH1(h);
        ymax = std::max(ymax, (double)h->GetMaximum());
        hists.push_back(h);
        l->AddEntry(h, hiBin.title, "l");
    }

    hists.at(0)->GetYaxis()->SetRangeUser(0.0, ymax * 1.35);
    hists.at(0)->GetXaxis()->SetTitle(PFTypeTitles.at(pfType));
    hists.at(0)->GetXaxis()->CenterTitle(true);
    hists.at(0)->GetXaxis()->SetTitleOffset(1.1);
    hists.at(0)->GetYaxis()->SetTitleOffset(1.3);

    for (std::size_t hb=0; hb<nhiBin; hb++){hists.at(hb)->Draw(hb == 0 ? "hist" : "hist same");}

    AddInfoEntries(l, cfg);
    l->Draw("same");
    c->SaveAs(outDir + "/" + plotName + ".png");

    for (auto* h : hists) { delete h; }
    delete c;
}

inline void SaveEtaPhiPlot(THnSparseF* hkin, Float_t ptCut, std::size_t hiBinIndex, const BinningStruct& bins, const JetHealthPlotConfig& cfg, const TString& outDir){
    const auto& hiBin = bins.hiBins.at(hiBinIndex);
    TString plotName = Form("hetaphi_pt%.0f%s", ptCut, hiBin.shortName.Data());
    TCanvas* c = MakeColzCanvas(plotName, cfg);

    TH2D* h = ProjectTHn2D(hkin, 1, 2, {{0, ptCut, 1e6}, {3, hiBin.lo, hiBin.hi}},Form("_%s", plotName.Data()));

    h->GetXaxis()->SetTitle("|#eta|");
    h->GetYaxis()->SetTitle("#phi (rad)");
    h->GetXaxis()->CenterTitle(true);
    h->GetYaxis()->CenterTitle(true);
    h->GetXaxis()->SetTitleOffset(1.1);
    h->GetYaxis()->SetTitleOffset(1.3);
    h->Draw("colz");

    // info box — top-left corner
    TLegend* l = MakeLegend(0.13, 0.80, 0.55, 0.93);
    l->AddEntry((TObject*)nullptr, Form("p_{T} > %.0f GeV/c", ptCut), "");
    l->AddEntry((TObject*)nullptr, hiBin.title, "");
    AddInfoEntries(l, cfg);
    l->Draw("same");

    c->SaveAs(outDir + "/" + plotName + ".png");
    delete h;
    delete c;
}

template <Int_t MAXNREF>
inline void SaveJetHealthPlots(const JetHealthStruct<MAXNREF>& hists, const BinningStruct& bins, const JetHealthPlotConfig& cfg){
    SetPlotStyle(cfg);

    TString plotsBase = MakePlotDir("jethealth_plots");
    std::cout << "saving jet health plots to " << plotsBase << std::endl;

    TString kinDir    = plotsBase + "/kinematics";
    TString pfDir     = plotsBase + "/pf_fractions";
    TString etaPhiDir = plotsBase + "/eta_phi_maps";
    gSystem->mkdir(kinDir, true);
    gSystem->mkdir(pfDir, true);
    gSystem->mkdir(etaPhiDir, true);

    // 1D kinematic plots
    SaveKinPlot(hists.kin, 0, "p_{T} (GeV/c)", "hkin_pt", bins, cfg, kinDir);
    SaveKinPlot(hists.kin, 1, "|#eta|", "hkin_abseta", bins, cfg, kinDir);
    SaveKinPlot(hists.kin, 2, "#phi (rad)", "hkin_phi", bins, cfg, kinDir);

    // PF fraction plots
    for(Int_t p=0; p<PFTypes; p++){SavePFPlot(hists.pf, p, bins, cfg, pfDir);}

    // eta-phi 2D plots
    const std::size_t nhiBin = bins.hiBins.size();
    for (Float_t ptCut : cfg.etaPhiPtCuts) {
        for (std::size_t hb = 0; hb < nhiBin; hb++) {
            SaveEtaPhiPlot(hists.kin, ptCut, hb, bins, cfg, etaPhiDir);
        }
    }

    std::cout << "all jet health plots saved to " << plotsBase << std::endl;
}

#endif
