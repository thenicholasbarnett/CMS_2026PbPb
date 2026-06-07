#include "TMath.h"
#include "TFile.h"
#include "TH1F.h"
#include "TDirectory.h"
#include "TGraphAsymmErrors.h"
#include "TString.h"
#include "Rtypes.h"

#include <chrono>
#include <vector>

#include "../header/Binning.h"
#include "../header/JetEfficiency.h"
#include "../header/JetHistograms.h"
#include "../header/JetTriggers_2026PbPb.h"
// #include "../header/JetTriggers_2025PbPb.h"

// more than number of jets in any event that was processed
static constexpr Int_t maxnref = 150;

void run(const TString& input, const TString& output);

int main(int argc, char* argv[]){
    if(argc < 2){
        std::cerr << "Compiled Usage: ./JetHLT_Eff <input.root> <output.root>" << std::endl;
        std::cerr << "Interpreted Usage: root -l -q -b 'JetHLT_EfficiencyGenerator.cpp(\"input.root\", \"output.root\")'" << std::endl;
        return 1;
    }
    run(argv[1], argc >= 3 ? argv[2] : "");
    return 0;
}

void JetHLT_EfficiencyGenerator(const TString& input, const TString& output=""){run(input, output);}
 
void run(const TString& input, const TString& output){
    auto start_time = std::chrono::high_resolution_clock::now();
    gStyle->SetOptStat(0);

    // input file
    TFile* fi = TFile::Open(input, "read");
    if(!fi || fi->IsZombie()){
        std::cerr<<R"(
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠠⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⡄⠈⠀⢀⣀⡤⢄⠒⠒⡖⠒⡒⠒⡒⠒⢦⣤⡔⠂⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⢀⢄⠐⡰⢞⠡⢂⢁⠂⠌⡐⠠⠡⠐⠡⡀⢣⢇⠯⣙⢦⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⢉⣀⡠⢋⠠⡁⢂⡔⢦⣬⠴⠆⢃⠂⢡⠡⠐⢌⠾⣌⢣⠞⣳⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⡜⠔⠆⡈⢊⠱⠚⠓⠋⢉⠉⣉⢊⣥⣓⣲⣒⠤⡕⣣⠞⣡⢷⡐⠒⠂⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠸⣆⣲⣸⡄⠂⠌⠤⢁⡘⢀⢢⡿⠊⠀⠀⠀⠈⠙⣵⠜⣜⣡⣞⣇⠄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⡔⠋⠉⠙⢻⡀⠒⡠⠐⠂⣿⠀⠀⠀⠀⣀⠀⠐⠘⡯⡔⢦⣉⣿⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠸⠀⠀⠁⠀⢀⡇⡡⢄⢳⣣⢿⡄⠀⠀⠀⠀⠀⡐⢴⡟⡜⣢⢥⡏⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⢱⣤⣤⣤⡾⢁⠸⠗⡈⠳⢁⠺⣦⣀⣀⣀⣠⣴⣿⣹⢒⣥⣾⣀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⢣⣉⣋⣐⡀⠬⡤⣤⣥⣤⣆⡙⢭⣛⢛⡻⣙⢸⢧⣿⣿⣿⣿⣷⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠉⠉⠉⢀⣠⣹⣧⣀⣻⣷⣿⣦⠱⡣⠖⣍⣾⣛⢾⣿⣿⣿⣿⣿⣦⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⢀⣜⠛⣿⣿⣿⣿⣏⣀⣿⢃⡏⣭⣷⠟⠁⣾⣹⣿⣿⣿⣿⣿⣿⣦⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠨⠌⡍⣉⢉⡉⣉⢉⠭⠯⣞⣼⣾⣆⠁⡆⣟⣯⣿⣿⣿⣿⣿⣿⣿⣷⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠪⠤⠬⠦⠥⣬⣶⡿⣻⣿⣿⠯⣤⢼⣏⢷⣻⣿⣿⣿⣿⣿⣿⣿⣿⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⣏⣷⣭⣽⡟⠰⡁⣾⢎⡯⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣦⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⡿⢿⣿⣿⡇⡆⢼⢏⠮⣽⣻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣧⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢸⣿⣿⣿⣶⣿⠁⡀⠘⣏⡞⣵⣻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣷⡀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣬⣽⣛⣻⠿⣿⡀⠁⠀⢹⡜⣷⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣄⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣼⣿⣿⣿⣿⣿⣿⣧⠀⠀⠀⢿⣿⣿⣿⣿⣿⡟⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣦⡀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠐⣷⣶⣭⣟⡻⢿⣿⣿⣧⠀⠀⢸⣿⣿⣿⣿⡭⢙⣸⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠦⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⣾⣿⣿⣿⣿⣿⣿⢽⣿⣷⣄⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⣠⣿⣷⣮⣽⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡆⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⣿⣿⣿⣿⣿⣿⣿⣿⡿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡇⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⢰⣿⣿⢿⠿⠿⠿⣿⣿⡇⣻⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡻⣿⣿⣿⣿⣿⣿⣿⣿⡇⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠝⢠⡈⢉⢉⡱⣚⢇⠠⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⠃⣿⣿⣿⣿⣿⣿⣿⣿⣿⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⢀⣏⠆⠟⣶⢯⡚⡥⠃⠀⠘⠹⡻⠿⠿⣿⣿⣿⣿⣿⣿⣿⣿⠀⢿⣿⣿⣿⣿⢿⣿⣿⣿⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⡮⢪⣛⡟⡇⣧⢓⣎⢳⡄⡠⠆⢡⠉⡑⠒⠂⢧⢻⣿⣿⣿⣿⡇⠸⢋⣿⣿⣿⣿⣿⣿⣿⡇⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠐⢝⢖⣹⡞⠵⣎⣞⢽⡊⢌⣔⢶⡂⢡⡞⣸⡤⡟⡻⣻⣿⣿⣷⠀⠀⠉⠉⢙⠻⡟⠿⠿⠇⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠉⠀⠛⠓⠃⠀⣎⡹⢬⠇⠀⠸⢜⢧⡬⣙⠶⢷⣿⣿⣿⣿⡄⠀⠀⠀⢸⢣⡇⠀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢷⣓⡾⠀⠀⠑⢭⣎⡹⣾⣼⣿⣿⣿⣿⣿⣧⠀⠀⡔⢚⡰⢇⡀⠀⠀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣽⣿⣿⣿⣿⣿⣿⣿⠟⠋⠀⠀⠣⠦⡍⠄⠹⣳⡀⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢠⣾⣿⣿⣿⣟⣿⣿⣿⠇⠀⠀⠀⠀⠀⠀⠈⢎⡐⢱⡄⠀⠀⠀⠀⠀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣰⣿⡽⣿⣿⣾⣿⣿⣿⡏⠀⠀⠀⠀⠀⠀⠀⠀⠀⢲⣀⢧⣠⣄⡀⠀⣀⡀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣸⣿⣿⣾⣿⣿⣿⣿⣿⣿⣿⡀⠀⠀⠀⠀⠀⠀⢠⣶⣾⣿⣷⣾⣿⣿⣿⣿⠍⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢹⣿⣿⣿⢿⣻⣿⣿⣿⣿⣿⣿⣿⡀⠀⠀⠀⠀⢀⣾⣿⣿⣿⣿⣿⣿⣿⣿⣿⡀⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⢀⣴⣿⣿⣻⣿⢿⣿⣿⣿⣿⣿⣿⣿⣿⢿⣤⣤⣤⣄⣨⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣅⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠉⣿⣾⣭⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣯⣿⣯⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡦
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠈⠛⠿⢿⣿⣿⣿⠿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠁
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠙⢿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⣿⡿⠏⠛⠻⠟⠛⠉⠀
⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠉⠉⠛⠛⠻⠿⠿⣿⣿⡿⠟⠋⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀⠀
        )" <<"\n"<< std::endl;
        throw std::runtime_error("ERROR: Could not open input file " + std::string(input.Data()));}
    std::cout << "opened " << input << std::endl;
    fi->cd();
    
    // getting histograms
    BinningStruct bins;
    JetHistogramsStruct<maxnref> hists(bins, false);
    hists.L1T = (THnSparseF*)fi->Get("hn_L1T");
    hists.HLT = (THnSparseF*)fi->Get("hn_HLT");
    if(!hists.L1T){throw std::runtime_error("ERROR: Could not find hn_L1T in input file");}
    if(!hists.HLT){throw std::runtime_error("ERROR: Could not find hn_HLT in input file");}
    std::cout << "histograms retrieved from " << input << std::endl;
    
    // output file
    TFile* fo = nullptr;
    if(output != ""){fo = new TFile(output, "recreate");}

    // saving efficiency plots as .png
    PlotConfig cfg;
    cfg.runNumber = "404469";
    cfg.globalTag = "PromptReco";
    cfg.jetAlgo = "akCs4PF";
    cfg.xmin  = 15.0;
    cfg.xmax  = 250.0;
    cfg.ymax  = 1.05;

    GenerateEfficiencies<maxnref>(hists, bins, fo, cfg);
    auto elapsed = std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - start_time).count();
    int hours   = elapsed / 3600;
    int minutes = (elapsed % 3600) / 60;
    int seconds = elapsed % 60;
    if (hours > 0)
        std::cout << "finished generating efficiencies in " << hours << "h " << minutes << "m " << seconds << "s" << std::endl;
    else if (minutes > 0)
        std::cout << "finished generating efficiencies in " << minutes << "m " << seconds << "s" << std::endl;
    else
        std::cout << "finished generating efficiencies in " << seconds << "s" << std::endl;

    if(fo){fo->Close();}
    fi->Close();
}
