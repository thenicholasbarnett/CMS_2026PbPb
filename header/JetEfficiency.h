#ifndef JETEFFICIENCY_H
#define JETEFFICIENCY_H
 
#include "TFile.h"
#include "TH1F.h"
#include "TH1D.h"
#include "TGraphAsymmErrors.h"
#include "THnSparse.h"
#include "TString.h"

#include <iostream>
#include <stdexcept>
#include <vector>

#include "Binning.h"
#include "Utilities.h"
#include "JetSpectraHistograms.h"
#include "JetTriggers_2026PbPb.h"
// #include "JetTriggers_2025PbPb.h"

enum EffType{
    kFull = 0, // HLT+L1seed+minBias/minBias
    kRelative = 1, // HLT+L1seed+minBias/L1seed+minBias
    kL1 = 2, // L1seed+minBias/minBias
    kNEffTypes = 3
};

inline TString EffTypeName(EffType e){
    if(e == kFull) return "full";
    if(e == kRelative) return "relative";
    if(e == kL1) return "L1";
    return "unknown";
}

inline std::size_t NTrigs(EffType e){
    return (e == kFull || e == kRelative) ? nHLT : nL1T;
}

inline TString TrigLabel(EffType effType, std::size_t t){
    if(effType == kFull || effType == kRelative) return GetHLTShortName(sHLTrigs[t].name);
    return GetL1ShortName(sL1Trigs[t].name);
}

inline TString YAxisTitle(EffType effType){
    if(effType == kFull)     return "HLT + MinBias / MinBias";
    if(effType == kRelative) return "HLT + MinBias / L1seed + MinBias";
    return "L1 + MinBias / MinBias";
}

inline void StyleGraph(TGraphAsymmErrors* g, EffType effType, std::size_t t){
    Int_t color = (effType == kFull || effType == kRelative)
        ? sHLTrigs[t].color
        : sL1Trigs[t].color;
    g->SetMarkerColor(color);
    g->SetLineColor(color);
    g->SetMarkerStyle(20);
    g->SetMarkerSize(1.5);
    g->SetLineWidth(3);
}

inline void GenerateEfficiencies(JetSpectraStruct& hists, const BinningStruct& bins, TFile* fo, const PlotConfig& cfg){
    SetPlotStyle(cfg);
    TString plotsBase = MakePlotDir("plots");
    std::cout << "saving plots to " << plotsBase << std::endl;

    const std::size_t nEta   = bins.etaBins.size();
    const std::size_t nhiBin = bins.hiBins.size();

    for(int e=0; e<kNEffTypes; e++){
        auto effType = static_cast<EffType>(e);
        const std::size_t nTrigs = NTrigs(effType);

        for(int m=0; m<JetSpectraStruct::kNMatchTypes; m++){
            auto matchType = static_cast<JetSpectraStruct::MatchType>(m);

            TString matchStr = matchType == JetSpectraStruct::kDR ? "dR" : "noDR";
            TString outDir = plotsBase + "/" + EffTypeName(effType) + "_" + matchStr;
            gSystem->mkdir(outDir, true);

            for(std::size_t b=0; b<nEta; b++){
                const auto& etaBin = bins.etaBins[b];

                for(std::size_t hb=0; hb<nhiBin; hb++){
                    const auto& hiBinRange = bins.hiBins[hb];

                    const Double_t etalo = etaBin.lo;
                    const Double_t etahi = etaBin.hi;
                    const Int_t hiBinlo = hiBinRange.lo;
                    const Int_t hiBinhi = hiBinRange.hi;

                    TH1D* denom_minbias = hists.ProjectL1T_pt( 0, JetSpectraStruct::kNoDR, etalo, etahi, hiBinlo, hiBinhi, Form("_denom_eta%zu_hb%zu_e%d_m%d", b, hb, e, m));

                    TString cname = "Efficiency_" + EffTypeName(effType) + "_" + matchStr + etaBin.shortName + hiBinRange.shortName;
                    TCanvas* c = MakeSinglePadCanvas(cname, cfg, true);

                    TLegend* ltrig = MakeLegend(0.45, 0.15, 0.93, 0.45);
                    ltrig->SetTextSize(0);

                    for(std::size_t t=0; t<nTrigs; t++){
                        TString suffix = Form("_eta%zu_hb%zu_t%zu_e%d_m%d", b, hb, t, e, m);

                        TH1D* numer = (effType == kFull || effType == kRelative)
                            ? hists.ProjectHLT_pt(t, matchType, etalo, etahi, hiBinlo, hiBinhi, suffix)
                            : hists.ProjectL1T_pt(t, matchType, etalo, etahi, hiBinlo, hiBinhi, suffix);

                        TH1D* denom = (effType == kRelative)
                            ? hists.ProjectL1T_pt(L1SeedHLT[t], JetSpectraStruct::kNoDR, etalo, etahi, hiBinlo, hiBinhi, suffix + "_denom")
                            : denom_minbias;

                        TGraphAsymmErrors* g = new TGraphAsymmErrors(numer, denom, "cl=0.683 b(1,1) mode");
                        StyleGraph(g, effType, t);
                        g->SetName(cname + (effType == kFull || effType == kRelative
                            ? GetHLTShortName(sHLTrigs[t].name)
                            : GetL1ShortName(sL1Trigs[t].name)));

                        fo->cd();
                        g->Write();
                        c->cd();
                        if(t == 0){
                            g->Draw("ap");
                            g->SetTitle("");
                            g->GetXaxis()->SetTitle("p_{T,leading jet} (GeV)");
                            g->GetYaxis()->SetTitle(
                                effType == kFull     ? "HLT + MinBias / MinBias" :
                                effType == kRelative ? "HLT + MinBias / L1seed + MinBias" :
                                                       "L1 + MinBias / MinBias"
                            );
                            g->GetXaxis()->CenterTitle(true);
                            g->GetYaxis()->CenterTitle(true);
                            g->GetXaxis()->SetTitleOffset(1.3);
                            g->GetYaxis()->SetTitleOffset(1.3);
                            g->SetMinimum(cfg.ymin);
                            g->SetMaximum(cfg.ymax);
                            g->GetXaxis()->SetRangeUser(cfg.xmin, cfg.xmax);
                        } else {
                            g->Draw("p same");
                        }

                        ltrig->AddEntry(g,
                            effType == kFull || effType == kRelative
                                ? GetHLTShortName(sHLTrigs[t].name)
                                : GetL1ShortName(sL1Trigs[t].name),
                            "lp"
                        );

                        delete numer;
                        if(effType == kRelative) delete denom;
                    }

                    DrawRefLine(cfg.xmin, cfg.xmax);
                    ltrig->Draw("same");
                    TLegend* linfo = MakeLegend(0.45, 0.50, 0.93, 0.75);
                    linfo->SetTextSize(0);
                    AddInfoEntries(linfo, cfg);
                    linfo->AddEntry((TObject*)nullptr, hiBinRange.title, "");
                    linfo->AddEntry((TObject*)nullptr, etaBin.title, "");
                    linfo->AddEntry((TObject*)nullptr,
                        matchType == JetSpectraStruct::kDR ? "#Delta R < 0.3" : "", "");
                    linfo->Draw("same");
                    DrawCMSLabel();
                    DrawLabel(Form(" Run %s", cfg.runNumber.Data()), 0.12, 0.9, 0.035);

                    c->Update();
                    c->SaveAs(outDir + "/" + cname + ".png");
                    delete c;
                    delete denom_minbias;
                }
            }
        }
    }
    std::cout << "all efficiency plots saved to " << plotsBase << std::endl;
}

#endif
