#ifndef JETHEALTHPLOTTING_H
#define JETHEALTHPLOTTING_H

#include "TCanvas.h"
#include "TDatime.h"
#include "TH1.h"
#include "TH2.h"
#include "TLatex.h"
#include "TLegend.h"
#include "TString.h"
#include "TStyle.h"
#include "TSystem.h"
#include "THnSparse.h"
 
#include "Binning.h"
#include "Utilities.h"
#include "ProgressBar.h"
#include "JetHistograms.h"

struct JetHealthPlotConfig : PlotConfig {std::vector<Float_t> etaPhiPtCuts = {20.0, 50.0, 100.0};};
inline bool PFTypeLogY(Int_t pfType){return pfType == CEF || pfType == NHF || pfType == MUF;}

inline void SaveKinPlot(THnSparseF* hkin, Int_t axis, const TString& axisLabel, const TString& plotName, const BinningStruct& bins, const JetHealthPlotConfig& cfg, const TString& outDir, ProgressBar& pb, TFile *outFile = nullptr){
    TCanvas* c = MakeSinglePadCanvas(plotName, cfg, false);
    TLegend* l = MakeLegend();
 
    const std::size_t nhiBin = bins.hiBins.size();
    std::vector<TH1D*> hists;
    double ymax = 0.0;
    SetAxisRange(hkin, 0, bins.ptmin, 500);

    for(std::size_t hb = nhiBin; hb-- > 0;){
        const auto& hiBin = bins.hiBins.at(hb);
        TString suf = Form("_%s_hb%zu", plotName.Data(), hb);
        TH1D* h = ProjectTHnSparse1D(hkin, axis, {{3, hiBin.lo, hiBin.hi}}, suf);
        StyleTH1(h, hiBin.color);
        NormalizeTH1(h);
        ymax = std::max(ymax, (double)h->GetMaximum());
        hists.push_back(h);
        l->AddEntry(h, hiBin.title, "l");
    }

    hists.at(0)->GetYaxis()->SetRangeUser(0.0, ymax * 1.35);
    if(axis==0){
        hists.at(0)->GetYaxis()->SetRangeUser(1e-6, ymax * 5.0);
        hists.at(0)->GetXaxis()->SetRangeUser(20, 500);
        c->SetLogy(true);
        c->SetLogx(true);
    }
    hists.at(0)->GetXaxis()->SetTitle(axisLabel);
    hists.at(0)->GetXaxis()->CenterTitle(true);
    hists.at(0)->GetXaxis()->SetTitleOffset(1.3);
    hists.at(0)->GetYaxis()->SetTitleOffset(1.3);

    for(std::size_t hb = 0; hb < nhiBin; hb++){
        hists.at(hb)->Draw(hb == 0 ? "ep" : "ep same");
        if(outFile && !outFile->IsZombie()){
            outFile->cd();
            hists.at(hb)->Write();
        }
    }

    DrawCMSLabel("Internal");
    DrawLabel(Form(" Run %s",cfg.runNumber.Data()),0.12,0.9, 0.035);
    DrawJetAlgoLabel(cfg.jetAlgo);
    DrawLabel(Form("#bf{p_{T} > %.0f GeV/c}", bins.ptmin), 0.4, 0.97, 0.035);
    
    l->Draw("same");
    gErrorIgnoreLevel = kWarning;
    c->SaveAs(outDir + "/" + plotName + ".png");
    gErrorIgnoreLevel = kInfo;
    pb.Update();

    for(auto* h : hists){delete h;}
    delete c;
}

inline void SavePFPlot(THnSparseF* hkin, Int_t pfType, const BinningStruct& bins, const JetHealthPlotConfig& cfg, const TString& outDir, ProgressBar& pb, TFile *outFile = nullptr){
    const Int_t pfAxis = JetHistogramsStruct<1>::PFTypeToAxis(pfType);
    
    for(const auto& etaBin : bins.etaBins){
        TString plotName = Form("hpf_%s%s", PFTypeNames.at(pfType), etaBin.shortName.Data());
        TCanvas* c = MakeSinglePadCanvas(plotName, cfg, true);
        TLegend* l = MakeLegend();

        const std::size_t nhiBin = bins.hiBins.size();
        std::vector<TH1D*> hists;
        SetAxisRange(hkin, 0, bins.ptmin, 500);

        double ymax = 0.0;
        for(std::size_t hb = nhiBin; hb-- > 0;){
            const auto& hiBin = bins.hiBins.at(hb);
            TString suf = Form("_%s_hb%zu", plotName.Data(), hb);
            TH1D* h = ProjectTHnSparse1D(hkin, pfAxis, {{1, etaBin.lo, etaBin.hi}, {3, hiBin.lo, hiBin.hi}}, suf);
            StyleTH1(h, hiBin.color);
            NormalizeTH1(h);
            ymax = std::max(ymax, (double)h->GetMaximum());
            hists.push_back(h);
            l->AddEntry(h, hiBin.title, "l");
        }

        hists.at(0)->GetXaxis()->SetTitle(PFTypeTitles.at(pfType));
        hists.at(0)->GetXaxis()->CenterTitle(true);
        hists.at(0)->GetXaxis()->SetTitleOffset(1.3);
        hists.at(0)->GetYaxis()->SetTitleOffset(1.3);
        if(PFTypeLogY(pfType)){
            hists.at(0)->GetYaxis()->SetRangeUser(1e-6, ymax * 5.0);
            c->SetLogy();
        } else {
            hists.back()->GetYaxis()->SetRangeUser(0.0, ymax * 1.35);
        }

        for(std::size_t hb = 0; hb < nhiBin; hb++){
            hists.at(hb)->Draw(hb == 0 ? "ep" : "ep same");
            if(outFile && !outFile->IsZombie()){
                outFile->cd();
                hists.at(hb)->Write();
            }
        }

        DrawCMSLabel("Internal");
        DrawLabel(Form(" Run %s",cfg.runNumber.Data()),0.12,0.9, 0.035);
        DrawJetAlgoLabel(cfg.jetAlgo);
        DrawLabel(Form("#bf{p_{T} > %.0f GeV/c}", bins.ptmin), 0.4, 0.975, 0.035);
        DrawLabel(Form("#bf{%s}", etaBin.title.Data()), 0.7, 0.965, 0.035);

        l->Draw("same");
        gErrorIgnoreLevel = kWarning;
        c->SaveAs(outDir + "/" + plotName + ".png");
        gErrorIgnoreLevel = kInfo;
        pb.Update();

        for(auto* h : hists){delete h;}
        delete c;
    }
}

inline void SaveEtaPhiPlot(THnSparseF* hkin, Float_t ptCut, std::size_t hiBinIndex, const BinningStruct& bins, const JetHealthPlotConfig& cfg, const TString& outDir, ProgressBar& pb, TFile *outFile = nullptr){
    const auto& hiBin = bins.hiBins.at(hiBinIndex);
    TString plotName = Form("hetaphi_pt%.0f%s", ptCut, hiBin.shortName.Data());
    TCanvas* c = MakeColzCanvas(plotName, cfg);

    TH2D* h = ProjectTHnSparse2D(hkin, 1, 2, {{0, ptCut, 1e6}, {3, hiBin.lo, hiBin.hi}}, Form("_%s", plotName.Data()));

    h->GetXaxis()->SetTitle("#eta");
    h->GetYaxis()->SetTitle("#phi (rad)");
    h->GetXaxis()->CenterTitle(true);
    h->GetYaxis()->CenterTitle(true);
    h->GetXaxis()->SetTitleOffset(1.3);
    h->GetYaxis()->SetTitleOffset(1.3);
    gStyle->SetPalette(kRedBlue);
    h->Draw("colz");

    DrawCMSLabel("Internal", 0.12, 0.93, 0.035);
    DrawLabel(Form("Run %s",cfg.runNumber.Data()),0.12,0.97, 0.035);
    DrawJetAlgoLabel(cfg.jetAlgo, 0.75, 0.04, 0.035);
    DrawLabel(Form("#bf{p_{T} > %.0f GeV/c}", ptCut), 0.12, 0.04, 0.035);
    DrawLabel(Form("#bf{%s}", bins.hiBins.at(hiBinIndex).title.Data()), 0.5, 0.93, 0.035);

    gErrorIgnoreLevel = kWarning;
    c->SaveAs(outDir + "/" + plotName + ".png");
    gErrorIgnoreLevel = kInfo;
    pb.Update();

    if(outFile && !outFile->IsZombie()){
        outFile->cd();
        h->Write();
    }

    delete h;
    delete c;
}

template <Int_t MAXNREF>
inline void SaveJetHealthPlots(const JetHistogramsStruct<MAXNREF>& hists, const BinningStruct& bins, const JetHealthPlotConfig& cfg, TFile *outFile = nullptr){
    SetPlotStyle(cfg);
    Float_t PFptcut = 50.0;
    if(!outFile || outFile->IsZombie()){std::cout << "Output file not specified or null"<< std::endl;}

    TString plotsBase = MakePlotDir("jethealth_plots_run"+cfg.runNumber);
    std::cout << "saving jet health plots to " << plotsBase << std::endl;

    TString kinDir = plotsBase + "/kinematics";
    TString pfDir = plotsBase + "/pf_fractions";
    TString etaPhiDir = plotsBase + "/eta_phi_maps";
    gSystem->mkdir(kinDir, true);
    gSystem->mkdir(pfDir, true);
    gSystem->mkdir(etaPhiDir, true);

    const std::size_t nEta    = bins.etaBins.size();
    const std::size_t nhiBin  = bins.hiBins.size();
    const std::size_t nPtCuts = cfg.etaPhiPtCuts.size();
    const int totalPlots = 3 + (int)(PFTypes * nEta) + (int)(nPtCuts * nhiBin);
    ProgressBar pb("Jet health plots:", totalPlots, ProgressBar::kRandom);

    // 1D kinematic plots
    SaveKinPlot(hists.kin, 0, "p_{T} (GeV/c)", "hkin_pt", bins, cfg, kinDir, pb, outFile);
    SaveKinPlot(hists.kin, 1, "#eta", "hkin_eta", bins, cfg, kinDir, pb, outFile);
    SaveKinPlot(hists.kin, 2, "#phi (rad)", "hkin_phi", bins, cfg, kinDir, pb, outFile);

    // PF fraction plots
    for(Int_t p=0; p<PFTypes; p++){SavePFPlot(hists.kin, p, bins, cfg, pfDir, pb, outFile);}

    // eta-phi 2D plots
    for (Float_t ptCut : cfg.etaPhiPtCuts) {
        for(std::size_t hb = nhiBin; hb-- > 0;){SaveEtaPhiPlot(hists.kin, ptCut, hb, bins, cfg, etaPhiDir, pb, outFile);}
    }

    std::cout << "\n"<<"all jet health plots saved to " << plotsBase << std::endl;
}

#endif
