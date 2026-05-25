#include "TApplication.h"
#include "TFile.h"
#include "THnSparse.h"
#include "TString.h"

#include <iostream>
#include <stdexcept>

#include "../header/Binning.h"
#include "../header/JetHealthHistograms.h"
#include "../header/JetHealthPlotting.h"

static constexpr Int_t maxnref = 150;

void plot(const TString& input);

int main(int argc, char* argv[]){
    if(argc < 2){
        std::cerr << "Usage: ./PlotJetHealth <input.root>" << std::endl;
        return 1;
    }
    plot(argv[1]);
    return 0;
}

void PlotJetHealth(const TString& input){plot(input);}

void plot(const TString& input){
    // gROOT->SetBatch(true);
    
    TFile* fi = TFile::Open(input, "read");
    if(!fi || fi->IsZombie()){ throw std::runtime_error("ERROR: Could not open input file " + std::string(input.Data())); }
    std::cout << "opened " << input << std::endl;

    BinningStruct bins;
    JetHealthStruct<maxnref> hists(bins);
    
    hists.kin = (THnSparseF*)fi->Get("hjetkin");
    hists.pf  = (THnSparseF*)fi->Get("hjetpf");
    if(!hists.kin){ throw std::runtime_error("ERROR: could not find hjetkin in file"); }
    if(!hists.pf) { throw std::runtime_error("ERROR: could not find hjetpf in file");  }

    JetHealthPlotConfig cfg;
    cfg.jetAlgo      = "akCs4PF";
    cfg.etaPhiPtCuts = {50.0, 100.0, 200.0};
    SaveJetHealthPlots(hists, bins, cfg);

    fi->Close();
}