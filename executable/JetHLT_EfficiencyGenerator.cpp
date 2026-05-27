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
#include "../header/JetTriggers_2026PbPb_MC.h"
// #include "../header/JetTriggers_2025PbPb.h"

void run(const TString& input, const TString& output);

int main(int argc, char* argv[]){
    if(argc < 3){
        std::cerr << "Usage: ./JetHLT_Eff <input.root> <output.root>" << std::endl;
        return 1;
    }
    run(argv[1], argv[2]);
    return 0;
}

void JetHLT_EfficiencyGenerator(const TString& input, const TString& output){run(input, output);}
 
void run(const TString& input, const TString& output){
    
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
    out.Write(fo);
    std::cout<<"all histogram saved to output file"<<std::endl;
    fi->Close();

    // saving efficiency plots as .png
    PlotConfig cfg;
    cfg.runNumber = "";
    cfg.globalTag = "";
    cfg.jetAlgo = "akCs4PF";
    cfg.xmin  = 20.0;
    cfg.xmax  = 300.0;
    cfg.ymax  = 1.4;

    SaveEfficiencyPlots(out, bins, cfg);
}
