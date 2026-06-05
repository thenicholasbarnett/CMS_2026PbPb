#include "TFile.h"
#include "TTree.h"
#include "TMath.h"
#include "TDirectory.h"

#include <chrono>
#include <fstream>
#include <iostream>
#include <string>
#include <stdexcept>

#include "../header/JetSelection_PbPb.h"
#include "../header/JSON_handler.h"

#include "../header/Binning.h"
#include "../header/BranchMapping.h"

#include "../header/EventStructs_PbPb.h"
#include "../header/JetStruct.h"
#include "../header/JetHistograms.h"

#include "../header/JetTriggers_2026PbPb.h"
// #include "../header/JetTriggers_2025PbPb.h"

// number of the run being processed
const TString& run_number = "";

// minimum pt of jets to include
static constexpr Float_t ptcut = 0.0;

// more than number of jets in any event being processed
static constexpr Int_t maxnref = 150;

void run(const TString& input_file_list, const TString& output, bool isMC);

int main(int argc, char* argv[]){
    if(argc < 4){
        std::cerr << "Compiled Usage: ./HiForestJetProcessing <filelist.txt> <output.root> <isMC>" << std::endl;
        std::cerr << "Interpreted Usage: root -l -q 'HiForestJetProcessing.cpp(\"filelist.txt\",\"output.root\",\"isMC\")'" << std::endl;
        return 1;
    }
    std::string isMCArg = argv[3];
    if(isMCArg != "true" && isMCArg != "True" && isMCArg != "yes" && isMCArg != "Yes" && isMCArg != "false" && isMCArg != "False" && isMCArg != "no" && isMCArg != "No" && isMCArg != "1" && isMCArg != "0"){
        std::cerr << "ERROR: isMC must be true or false" << std::endl;
        return 1;
    }
    bool isMC = (isMCArg == "true" || isMCArg == "True" || isMCArg == "yes" || isMCArg == "Yes" || isMCArg == "1");
    run(argv[1], argv[2], isMC);
    return 0;
}

void HiForestJetProcessing(const TString& input_file_list, const TString& output, bool isMC){run(input_file_list, output, isMC);}
 
void run(const TString& input_file_list, const TString& output, bool isMC){
    auto start_time = std::chrono::high_resolution_clock::now();

    // ttree names
    const Int_t nTTrees = 4;
    TString sTTrees[nTTrees] = {"hiEvtAnalyzer/HiTree", "skimanalysis/HltTree", "akCs4PFJetAnalyzer/t", "hltanalysis/HltTree"};
 
    // L1 object tree name
    const TString sL1ObjTree = "l1object/L1UpgradeFlatTree";

    // jet selections
    JetSelect js;

    // JSON handler
    JSON_handler* dcs = nullptr;
    if(!isMC){dcs = new JSON_handler();}

    // event objects
    EventStruct evt;
    FiltersStruct fltr;

    // jet object
    JetStruct<maxnref> jt;

    // trigger object
    TriggersStruct trg;

    // binning
    BinningStruct bins;

    // histograms
    JetHistogramsStruct<maxnref> hists(bins);

    // getting list of root files to process
    std::ifstream myfile(input_file_list);
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

        // getting HLT object ttrees
        TTree* HLTObjTTrees[nHLT];
        for(std::size_t t=0; t<nHLT; t++){
            TString objTreeName = GetHLTObjTreeName(t);
            HLTObjTTrees[t] = (TTree*)fi->Get(objTreeName);
            if(!HLTObjTTrees[t]){throw std::runtime_error(Form("ERROR: Could not find TTree '%s' in file",objTreeName.Data()));}
            SetBranches(HLTObjTTrees[t], trg.HLTObjBranchMap(t));
        }
 
        // getting L1 object ttree
        TTree* L1ObjTTree = (TTree*)fi->Get(sL1ObjTree);
        if(!L1ObjTTree){throw std::runtime_error(Form("ERROR: Could not find TTree '%s' in file", sL1ObjTree.Data()));}
        SetBranches(L1ObjTTree, trg.L1ObjBranchMap());

        // looping over events
        Long64_t nentries = ttrees[0]->GetEntries();
        for(Long64_t i=0; i<nentries; i++){
            
            // getting event and event filter info from each event
            ttrees[0]->GetEntry(i);
            ttrees[1]->GetEntry(i);
            if(!isMC && !dcs->isGood(evt.run, evt.lumi)){continue;}

            // filling event histograms
            hists.vz_unpassed->Fill(evt.vz, evt.w);
            //hists.pclustF->Fill(fltr.pclustF, evt.w);
            hists.ppvF->Fill(fltr.ppvF, evt.w);
            hists.pphfF->Fill(fltr.pphfF, evt.w);

            // checking if the event passes event filters and |vz| < 15
            // if((fltr.ppvF==0)||(fltr.pclustF==0)||(fltr.pphfF==0)||(TMath::Abs(evt.vz)>15)){continue;}
            if((fltr.ppvF==0)||(fltr.pphfF==0)||(TMath::Abs(evt.vz)>15)){continue;}

            // getting jet and trigger ttree info
            ttrees[2]->GetEntry(i);
            ttrees[3]->GetEntry(i);
            for(std::size_t t=0; t<nHLT; t++){HLTObjTTrees[t]->GetEntry(i);}
            L1ObjTTree->GetEntry(i);

            // terminal will yell at me if the leading jet index isn't zero (it never yells at me)
            Int_t lj=0;
            for(Int_t j=0; j<jt.reco.nref; j++){if(jt.reco.pt[j]>jt.reco.pt[lj]){lj=j;}}
            if(lj!=0){std::cout<<"leading jet index is "<<lj<<std::endl;}

            /// Skipping events without jets or with a leading jet below ptcut
            if((jt.reco.nref==0)||(jt.reco.pt[lj]<=ptcut)){continue;}

            // only looking at events that fire the minbias l1 trigger
            if(trg.L1T[0]==0){continue;}

            // using header file for jet identification
            if(!js.JetSelection(jt.reco.eta[lj], jt.reco.phi[lj], jt.reco.pf.CEF[lj], jt.reco.pf.NEF[lj],jt.reco.pf.MUF[lj])){continue;}

            // filling histograms for events passing cuts
            hists.vz->Fill(evt.vz, evt.w);
            hists.hiBin->Fill(evt.hiBin, evt.w);
            hists.nref->Fill(jt.reco.nref, evt.w);

            const Double_t abseta = TMath::Abs(jt.reco.eta[lj]);

            // looping through all jets in each event
            for(unsigned int j=0; j<jt.reco.nref; j++){

                // pt cut on jets
                if(jt.reco.pt[j]<ptcut){continue;}
                
                // using header file for jet identification and veto map
                if(!js.JetSelection(jt.reco.eta[j], jt.reco.phi[j], jt.reco.pf.CEF[j], jt.reco.pf.NEF[j],jt.reco.pf.MUF[j])){continue;}
                
                // filling kinematics histogram
                hists.FillKin(jt.reco, j, evt.hiBin, evt.w);
            }

            // matching HLT object to offline jet
            int iHltMatch[nHLT] = {0};
            for(std::size_t t=0; t<nHLT; t++){
                if(trg.HLT[t]==0){continue;}

                auto* obj_pt = trg.HLT_JetObj_pt[t];
                auto* obj_eta = trg.HLT_JetObj_eta[t];
                auto* obj_phi = trg.HLT_JetObj_phi[t];
                if(!obj_pt || !obj_eta || !obj_phi){continue;}

                for(std::size_t tj=0; tj<obj_pt->size(); tj++){

                    Double_t deta = TMath::Abs(obj_eta->at(tj) - jt.reco.eta[lj]);
                    Double_t dphi = TMath::ACos(TMath::Cos(obj_phi->at(tj) - jt.reco.phi[lj]));
                    Double_t dR = TMath::Sqrt(deta*deta + dphi*dphi);

                    if((dR<0.3) && (obj_pt->at(tj)>=GetJetTriggerThreshold(sHLTrigs[t].name))){
                        iHltMatch[t] = 1;
                        break;
                    }
                }
            }

            // matching L1 jet object to offline leading jet
            int iL1Match[nL1T] = {0};
            for(std::size_t t=1; t<nL1T; t++){
                if(trg.L1T[t]==0){continue;}
                
                for(Int_t tj=0; tj<trg.L1_nJets; tj++){
                    
                    Double_t deta = TMath::Abs(trg.L1_jetEta[tj] - jt.reco.eta[lj]);
                    Double_t dphi = TMath::ACos(TMath::Cos(trg.L1_jetPhi[tj] - jt.reco.phi[lj]));
                    Double_t dR   = TMath::Sqrt(deta*deta + dphi*dphi);

                    if((dR<0.3) && (trg.L1_jetEt[tj] >= GetJetTriggerThreshold(sL1Trigs[t].name))){
                        iL1Match[t] = 1;
                        break;
                    }
                }
            }
            
            for(std::size_t t=0; t<nHLT; t++){
                if(trg.HLT[t]==1){
                    hists.FillHLT(t, JetHistogramsStruct<maxnref>::kNoDR, jt.reco.pt[lj], abseta, evt.hiBin, evt.w);
                    if(iHltMatch[t]==1){
                        hists.FillHLT(t, JetHistogramsStruct<maxnref>::kDR, jt.reco.pt[lj], abseta, evt.hiBin, evt.w);
                    }
                }
            }

            for(std::size_t t=0; t<nL1T; t++){
                if(trg.L1T[t]==1){
                    hists.FillL1T(t, JetHistogramsStruct<maxnref>::kNoDR, jt.reco.pt[lj], abseta, evt.hiBin, evt.w);
                    if(iL1Match[t]==1){
                        hists.FillL1T(t, JetHistogramsStruct<maxnref>::kDR, jt.reco.pt[lj], abseta, evt.hiBin, evt.w);
                    }
                }
            }

        }
        fi->Close();
    }
    std::cout << "finished processing files in " << std::chrono::duration_cast<std::chrono::seconds>(std::chrono::high_resolution_clock::now() - start_time).count() << " seconds" << std::endl;
    delete dcs;
    
    // making output file and storing histograms
    TFile *fo = new TFile(output,"recreate");
    hists.Write(fo);
    fo->Close();
}
