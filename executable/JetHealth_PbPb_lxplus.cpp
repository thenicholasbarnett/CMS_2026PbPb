#include "TFile.h"
#include "TTree.h"
#include "TMath.h"
#include "TDirectory.h"

#include <fstream>
#include <iostream>
#include <string>
#include <stdexcept>

#include "../header/Binning.h"
#include "../header/BranchMapping.h"
#include "../header/EventStructs_PbPb.h"
#include "../header/JetSelection_PbPb.h"
#include "../header/JetStruct.h"
#include "../header/JetHealthHistograms.h"

// minimum pt of jets to include
static constexpr Float_t ptcut = 20.0;

// more than number of jets in any event being processed
static constexpr Int_t maxnref = 150;

void run(const TString& input_filelist, const TString& output, bool isMC);

int main(int argc, char* argv[]){
    if(argc < 4){
        std::cerr << "Usage: ./JetHLT <filelist.txt> <output.root> <isMC>" << std::endl;
        return 1;
    }
    std::string isMCArg = argv[3];
    if(isMCArg != "true" && isMCArg != "false" && isMCArg != "1" && isMCArg != "0"){
        std::cerr << "ERROR: isMC must be true or false" << std::endl;
        return 1;
    }
    bool isMC = (isMCArg == "true" || isMCArg == "1");
    run(argv[1], argv[2], isMC);
    return 0;
}

void JetHealth_PbPb_lxplus(const TString& input_filelist, const TString& output, bool isMC){run(input_filelist, output, isMC);}
 
void run(const TString& input_filelist, const TString& output, bool isMC){

    // ttree names
    const Int_t nTTrees = 4;
    TString sTTrees[nTTrees] = {"hiEvtAnalyzer/HiTree", "skimanalysis/HltTree", "akCs4PFJetAnalyzer/t", "hltanalysis/HltTree"};

    // for applying Jet Selections from headers
    JetSelect js;

    // event objects
    EventStruct evt;
    FiltersStruct fltr;

    // jet object
    JetStruct<maxnref> jt;

    // trigger object
    TriggersStruct trg;

    // binning
    BinningStruct bins;

    // initializing histograms
    JetHealthStruct hists(bins);

    // getting list of root files to process
    std::ifstream myfile(input_filelist);
    std::string filename;

    // keeping track of how many files have been processed
    int filenumber = 0;

    // looping over files
    while(getline(myfile, filename)){
        filenumber+=1;

        // reading and staging input file
        TString input = filename;
        TFile *fi = TFile::Open(input,"read");
        if(!fi || fi->IsZombie()){throw std::runtime_error("ERROR: Could not open input file " + std::string(input.Data()));}
        fi->cd();

        // showing the file being processed in the terminal
        std::cout<<"processing file "<< filenumber <<": "<<input<<std::endl;

        // getting ttrees, printing iff the ttree isn't in the input file
        TTree *ttrees[nTTrees];
        for(int t=0; t<nTTrees; t++){
            ttrees[t] = (TTree*)fi->Get(sTTrees[t]);
            if(!ttrees[t]){throw std::runtime_error(Form("ERROR: Could not find TTree '%s' in file",sTTrees[t].Data()));}
        }

        // assigning variables to branches
        SetBranches(ttrees[0],evt.BranchMap(isMC));
        SetBranches(ttrees[1],fltr.BranchMap());
        SetBranches(ttrees[2],jt.BranchMap(isMC));
        SetBranches(ttrees[3],trg.BranchMap());

        // looping over events
        Long64_t nentries = ttrees[0]->GetEntries();
        for(Long64_t i=0; i<nentries; i++){
            
            // getting event and event filter info from each event
            ttrees[0]->GetEntry(i);
            ttrees[1]->GetEntry(i);

            // // applying JSON //
            // if(!dcs.isGood(run, lumi)){continue;}

            // filling event histograms
            hists.vz_unpassed->Fill(evt.vz, evt.w);
            hists.pclustF->Fill(fltr.pclustF, evt.w);
            hists.ppvF->Fill(fltr.ppvF, evt.w);
            hists.pphfF->Fill(fltr.pphfF, evt.w);

            // checking if the event passes event filters and |vz| < 15
            if((fltr.ppvF==0)||(fltr.pclustF==0)||(fltr.pphfF==0)||(TMath::Abs(evt.vz)>15)){continue;}

            // getting jet and trigger ttree info
            ttrees[2]->GetEntry(i);
            ttrees[3]->GetEntry(i);

            /// Skipping events without jets or that don't fire teh minbias l1 trigger
            if((jt.reco.nref==0)||(trg.L1T[0]==0)){continue;}

            // looping through all jets in each event
            for(unsigned int j=0; j<jt.reco.nref; j++){

                // pt cut on jets
                if(jt.reco.pt[j]<ptcut){continue;}
                
                // using header file for jet identification
                if(!js.JetSelection(jt.reco.eta[j], jt.reco.phi[j], jt.reco.pf.CEF[j], jt.reco.pf.NEF[j],jt.reco.pf.MUF[j])){continue;}
                
                // filling histograms //

                for(std::size_t b=0; b<bins.etaBins.size(); b++){
                    const auto& etaBin = bins.etaBins[b];
                    if((TMath::Abs(jt.reco.eta[j])<etaBin.lo)||(TMath::Abs(jt.reco.eta[j])>=etaBin.hi)){continue;}

                    // looping through centrality intervals
                    for(std::size_t hb=0; hb<bins.hiBins.size(); hb++){
                        const auto& hiBinRange = bins.hiBins[hb];
                        if((evt.hiBin<hiBinRange.lo)||(evt.hiBin>=hiBinRange.hi)){continue;}
 
                        hists.pt[hb]->Fill(jt.reco.pt[j], evt.w);
                        hists.eta[hb]->Fill(jt.reco.eta[j], evt.w);
                        hists.phi[hb]->Fill(jt.reco.phi[j], evt.w);
                        hists.etaphi[hb]->Fill(jt.reco.eta[j], jt.reco.phi[j], evt.w);
                        hists.CHF[hb]->Fill(jt.reco.pf.CHF[j], evt.w);
                        hists.NHF[hb]->Fill(jt.reco.pf.NHF[j], evt.w);
                        hists.CEF[hb]->Fill(jt.reco.pf.CEF[j], evt.w);
                        hists.NEF[hb]->Fill(jt.reco.pf.NEF[j], evt.w);
                        hists.MUF[hb]->Fill(jt.reco.pf.MUF[j], evt.w);
                    }
                }
            }
        }
        fi->Close();
    }

    // making output file and storing histograms
    TFile *fo = new TFile(output,"recreate");
    hists.Write(fo);
}