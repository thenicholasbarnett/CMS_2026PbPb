#include "TMath.h"
#include "TFile.h"
#include "TH1F.h"
#include "TDirectory.h"
#include "TGraphAsymmErrors.h"
#include "TString.h"
#include "Rtypes.h"

#include <vector>

#include "../header/Binning.h"
#include "../header/JetEfficiency.h"
#include "../header/JetEfficiencyPlotting.h"
#include "../header/JetSpectraHistograms.h"
#include "../header/JetTriggers_PbPb_MC.h"

void JetHLT_EfficiencyGenerator(const TString& input, const TString& output){
    
    // no preset legends in histograms
    gStyle->SetOptStat(0);

    // opening input
    TFile *fi = TFile::Open(input,"read");
    if(!fi || fi->IsZombie()){throw std::runtime_error("ERROR: Could not open input file " + std::string(input.Data()));}
    fi->cd();
    
    // making and getting histograms
    BinningStruct bins;
    JetSpectraStruct hists(bins, false);
    GetHistograms(fi, bins, hists);
    std::cout<<"histograms retrieved from "<<input<<std::endl;

    // generating efficiencies
    JetEfficiencyOutputStruct out(bins);
    out.ComputeEfficiencies(hists, bins);
    std::cout<<"efficiencies genereated"<<std::endl;
    
    // making output file
    TFile *fo = new TFile(output,"recreate");
    fo->cd();
    TDirectory *d_in = fo->mkdir("inputs");
    TDirectory *d_out = fo->mkdir("outputs");

    const std::size_t nEta = bins.etaBins.size();
    const std::size_t nhiBin = bins.hiBins.size();

    for(int m=0; m<JetSpectraStruct::kNMatchTypes; m++){
        auto matchType = static_cast<JetSpectraStruct::MatchType>(m);

        for(std::size_t b=0; b<nEta; b++){
            for(std::size_t t=0; t<nHLT; t++){

                d_in->cd();
                hists.hlt_ljtpt_eta[matchType][b][t]->Write();

                if(matchType == JetSpectraStruct::kNoDR){
                    hists.ljtpt_eta[b]->Write();
                    if(t<nL1T){hists.l1_ljtpt_eta[b][t]->Write();}
                }

                for (int e=0; e<JetEfficiencyOutputStruct::kNEfficiencyTypes; e++){
                    auto effType = static_cast<JetEfficiencyOutputStruct::EfficiencyType>(e);

                    d_out->cd();
                    out.jetEfficiencies_inclusive[matchType][effType][b][t]->Write();
                }

                for(std::size_t hb=0; hb<nhiBin; hb++){
                    d_in->cd();
                    hists.hlt_ljtpt_hibin[matchType][b][t][hb]->Write();

                    if(matchType == JetSpectraStruct::kNoDR){
                        hists.ljtpt_hibin[b][hb]->Write();
                        if(t<nL1T){hists.l1_ljtpt_hibin[b][t][hb]->Write();}
                    }

                    for(int e=0; e<JetEfficiencyOutputStruct::kNEfficiencyTypes; e++){
                        auto effType = static_cast<JetEfficiencyOutputStruct::EfficiencyType>(e);

                        d_out->cd();
                        out.jetEfficiencies_hibin[matchType][effType][b][hb][t]->Write();
                    }
                }
            }
        }
    }
    std::cout<<"all histogram saved to output file"<<std::endl;
 
    fo->Close();
    fi->Close();

    // saving efficiency plots as .png
    PlotConfig cfg;
    cfg.runNumber = "";
    cfg.globalTag = "";
    cfg.jetAlgo   = "akCs4PF";
    cfg.ptmin     = 20.0;
    cfg.ptmax     = 300.0;
    cfg.effmax    = 1.4;

    SaveEfficiencyPlots(out, bins, cfg);
}