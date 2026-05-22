#include "TFile.h"
#include "TTree.h"
#include "TString.h"
#include "TH1.h"

#include <iostream>
#include <fstream>
#include <string>
#include <stdexcept>

void run(const TString& input_filelist, const TString& output, const TString& sJetAlgorithm, bool isMC);

int main(int argc, char* argv[]){
    if(argc < 5){
        std::cerr << "Usage: ./jet_tests <filelist.txt> <output.root> <jet clustering algorithm> <isMC>" << std::endl;
        return 1;
    }
    std::string isMCArg = argv[4];
    if(isMCArg != "true" && isMCArg != "false" && isMCArg != "1" && isMCArg != "0"){
        std::cerr << "ERROR: isMC must be true or false" << std::endl;
        return 1;
    }
    bool isMC = (isMCArg == "true" || isMCArg == "1");
    run(argv[1], argv[2], argv[3], isMC);
    return 0;
}

void find_maxnref(const TString& input_filelist, const TString& output, const TString& sJetAlgorithm, const bool isMC){run(input_filelist, output, sJetAlgorithm, isMC);}
 
void run(const TString& input_filelist, const TString& output, const TString& sJetAlgorithm, const bool isMC){

    // initializing variables
    Float_t w=0.f;
    Int_t nref=0;
    Int_t maxnref=-1;
    constexpr int maxnref_guess = 200;

    // initializing histogram
    TH1I hnref("hnref", "hnref", maxnref_guess, 0, maxnref_guess);

    // getting list of root files to process
    std::ifstream filelist(input_filelist.Data());
    if(!filelist.is_open()){throw std::runtime_error(std::string("ERROR: Could not open input file list ")+input_filelist);}
    
    std::string filename;
    int filenumber = 0;

    // looping over files
    while(std::getline(filelist, filename)){

        // opening input file
        TFile *fi = TFile::Open(filename.c_str(),"READ");        
        if(!fi||fi->IsZombie()){throw std::runtime_error(std::string("ERROR: Could not open input file ")+filename);}

        // tracking file in list
        ++filenumber;
        std::cout<<"processing file "<< filenumber <<": "<<filename<<std::endl;

        // getting jet ttree from input file
        TString sJetTTree = sJetAlgorithm+"JetAnalyzer/t";
        TTree* JetTTree = dynamic_cast<TTree*>(fi->Get(sJetTTree));
        if(!JetTTree){throw std::runtime_error(std::string("ERROR: Could not open ")+sJetTTree.Data());}

        // getting jet branches
        if(!JetTTree->GetBranch("nref")){throw std::runtime_error(std::string("ERROR: 'nref' branch not in ")+sJetTTree.Data());}
        JetTTree->SetBranchStatus("*", 0);
        JetTTree->SetBranchStatus("nref", 1);
        JetTTree->SetBranchAddress("nref", &nref);

        // getting weight for the event if input is MC
        TTree* EventTTree = nullptr;
        if(isMC){
            EventTTree = dynamic_cast<TTree*>(fi->Get("hiEvtAnalyzer/HiTree"));
            if(!EventTTree){throw std::runtime_error(std::string("ERROR: Could not open hiEvtAnalyzer/HiTree"));}
            if(!EventTTree->GetBranch("weight")){throw std::runtime_error(std::string("ERROR: 'weight' branch not in hiEvtAnalyzer/HiTree"));}
            EventTTree->SetBranchStatus("*", 0);
            EventTTree->SetBranchStatus("weight", 1);
            EventTTree->SetBranchAddress("weight", &w);
        }

        if(isMC){if(JetTTree->GetEntries() != EventTTree->GetEntries()){throw std::runtime_error(std::string("ERROR: Jet TTree and event TTree have different entries in ")+filename);}}
        const Long64_t nEntries = JetTTree->GetEntries();

        // looping over events
        for(Long64_t i=0; i<nEntries; ++i){
            JetTTree->GetEntry(i);
            if(isMC){EventTTree->GetEntry(i);} else{w=1.f;}
            hnref.Fill(nref, w);
            if(nref>maxnref){maxnref=nref;}
        }
        fi->Close();
        delete fi;
    }
    std::cout<<"\nMaximum nref found was "<<maxnref<<std::endl;
    TFile fo(output,"RECREATE");
    hnref.Write();
    fo.Close();
}
