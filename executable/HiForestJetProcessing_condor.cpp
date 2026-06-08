#include "TFile.h"
#include "TTree.h"
#include "TMath.h"
#include "TSystem.h"
#include "TDirectory.h"

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
static constexpr Int_t maxnref = 100;

void run(const TString& input, const TString& output, const TString& output_txt, bool isMC);

int main(int argc, char* argv[]){
    if(argc < 5){
        std::cerr << "Compiled Usage: ./HiForestJetProcessing <input.root> <output.root> <output.txt> <isMC>" << std::endl;
        std::cerr << "Interpreted Usage: root -l -b -q 'HiForestJetProcessing_condor.cpp(\"input.root\",\"output.root\",\"output.txt\",\"isMC\")'" << std::endl;
        return 1;
    }
    std::string isMCArg = argv[4];
    if(isMCArg != "true" && isMCArg != "True" && isMCArg != "yes" && isMCArg != "Yes" && isMCArg != "false" && isMCArg != "False" && isMCArg != "no" && isMCArg != "No" && isMCArg != "1" && isMCArg != "0"){
        std::cerr << "ERROR: isMC must be true or false" << std::endl;
        return 1;
    }
    bool isMC = (isMCArg == "true" || isMCArg == "True" || isMCArg == "yes" || isMCArg == "Yes" || isMCArg == "1");
    run(argv[1], argv[2], argv[3], isMC);
    return 0;
}

void HiForestJetProcessing_condor(const TString& input, const TString& output, const TString& output_txt, bool isMC){run(input, output, output_txt, isMC);}
 
void run(const TString& input, const TString& output, const TString& output_txt, bool isMC){
    
    const Int_t nTTrees = 5;
    TString sTTrees[nTTrees] = {"hiEvtAnalyzer/HiTree", "skimanalysis/HltTree", "akCs4PFJetAnalyzer/t", "hltanalysis/HltTree", "l1object/L1UpgradeFlatTree"};
 
    JetSelect js;
    JSON_handler dcs;
    EventStruct evt;
    FiltersStruct fltr;
    JetStruct<maxnref> jt;
    TriggersStruct trg;
    BinningStruct bins;
    JetHistogramsStruct<maxnref> hists(bins);

    TFile *fi = TFile::Open(input,"read");
    if(!fi || fi->IsZombie()){throw std::runtime_error("ERROR: Could not open input file " + std::string(input.Data()));}
    fi->cd();

    // trigger efficiency drops
    std::ofstream outputFile(output_txt);
    
    TTree *ttrees[nTTrees];
    for(int t=0; t<nTTrees; t++){
        ttrees[t] = (TTree*)fi->Get(sTTrees[t]);
        if(!ttrees[t]){throw std::runtime_error(Form("ERROR: Could not find TTree '%s' in file",sTTrees[t].Data()));}
    }

    // mapping variables to branches
    SetBranches(ttrees[0],evt.BranchMap(isMC));
    SetBranches(ttrees[1],fltr.BranchMap());
    SetBranches(ttrees[2],jt.BranchMap(isMC));
    SetBranches(ttrees[3],trg.BranchMap());
    SetBranches(ttrees[4],trg.L1ObjBranchMap());

    // getting HLT object ttrees
    TTree* HLTObjTTrees[nHLT];
    for(std::size_t t=0; t<nHLT; t++){
        TString objTreeName = GetHLTObjTreeName(t);
        HLTObjTTrees[t] = (TTree*)fi->Get(objTreeName);
        if(!HLTObjTTrees[t]){throw std::runtime_error(Form("ERROR: Could not find TTree '%s' in file",objTreeName.Data()));}
        SetBranches(HLTObjTTrees[t], trg.HLTObjBranchMap(t));
    }

    // event loop
    Long64_t nentries = ttrees[0]->GetEntries();
    for(Long64_t i=0; i<nentries; i++){
        
        // getting event info
        ttrees[0]->GetEntry(i);
        ttrees[1]->GetEntry(i);
        if(!dcs.isGood(evt.run, evt.lumi)){continue;}

        // filling event histograms
        hists.vz_unpassed->Fill(evt.vz, evt.w);
        //hists.pclustF->Fill(fltr.pclustF, evt.w);
        hists.ppvF->Fill(fltr.ppvF, evt.w);
        hists.pphfF->Fill(fltr.pphfF, evt.w);

        // event cuts
        // if((fltr.ppvF==0)||(fltr.pclustF==0)||(fltr.pphfF==0)||(TMath::Abs(evt.vz)>15)){continue;}
        if((fltr.ppvF==0)||(fltr.pphfF==0)||(TMath::Abs(evt.vz)>15)){continue;}

        // getting jet and trigger info
        ttrees[2]->GetEntry(i);
        ttrees[3]->GetEntry(i);
        for(std::size_t t=0; t<nHLT; t++){HLTObjTTrees[t]->GetEntry(i);}
        ttrees[4]->GetEntry(i);

        // terminal will yell at me if the leading jet index isn't zero (it never yells at me)
        Int_t lj=0;
        for(Int_t j=0; j<jt.reco.nref; j++){if(jt.reco.pt[j]>jt.reco.pt[lj]){lj=j;}}
        if(lj!=0){std::cout<<"leading jet index is "<<lj<<std::endl;}
        
        if((jt.reco.nref==0)||(jt.reco.pt[lj]<=ptcut)){continue;}

        // only looking at events that fire the minbias l1 trigger
        if(trg.L1T[0]==0){continue;}

        // jet ID & veto map
        if(!js.JetSelection(jt.reco.eta[lj], jt.reco.phi[lj], jt.reco.pf.CEF[lj], jt.reco.pf.NEF[lj],jt.reco.pf.MUF[lj])){continue;}

        hists.vz->Fill(evt.vz, evt.w);
        hists.hiBin->Fill(evt.hiBin, evt.w);
        hists.nref->Fill(jt.reco.nref, evt.w);

        // jet loop
        for(unsigned int j=0; j<jt.reco.nref; j++){
            if(jt.reco.pt[j]<ptcut){continue;}
            if(!js.JetSelection(jt.reco.eta[j], jt.reco.phi[j], jt.reco.pf.CEF[j], jt.reco.pf.NEF[j],jt.reco.pf.MUF[j])){continue;}
            hists.FillKin(jt.reco, j, evt.hiBin, evt.w);
        }

        // online HLT object to offline object matching
        int iHltMatch[nHLT] = {0};
        for(std::size_t t=0; t<nHLT; t++){
            if(trg.HLT[t]==0){continue;}
            if(!trg.HLT_JetObj_pt[t] || !trg.HLT_JetObj_eta[t] || !trg.HLT_JetObj_phi[t]){continue;}

            for(std::size_t tj=0; tj<trg.HLT_JetObj_pt[t]->size(); tj++){
                Double_t deta = TMath::Abs(trg.HLT_JetObj_eta[t]->at(tj) - jt.reco.eta[lj]);
                Double_t dphi = TMath::ACos(TMath::Cos(trg.HLT_JetObj_phi[t]->at(tj) - jt.reco.phi[lj]));
                Double_t dR = TMath::Sqrt(deta*deta + dphi*dphi);
                if((dR<0.3) && (trg.HLT_JetObj_pt[t]->at(tj)>=GetJetTriggerThreshold(sHLTrigs[t].name))){
                    iHltMatch[t] = 1;
                    break;
                }
            }
        }

        // online L1T object to offline object matching
        int iL1Match[nL1T] = {0};
        for(std::size_t t=1; t<nL1T; t++){
            if(trg.L1T[t]==0){continue;}
            
            for(Int_t tj=0; tj<trg.L1_nJets; tj++){
                Double_t deta = TMath::Abs(trg.L1_jetEta[tj] - jt.reco.eta[lj]);
                Double_t dphi = TMath::ACos(TMath::Cos(trg.L1_jetPhi[tj] - jt.reco.phi[lj]));
                Double_t dR = TMath::Sqrt(deta*deta + dphi*dphi);
                if((dR<0.3) && (trg.L1_jetEt[tj] >= GetJetTriggerThreshold(sL1Trigs[t].name))){
                    iL1Match[t] = 1;
                    break;
                }
            }
        }
        
        int drop_flag = 0;
        for(std::size_t t=0; t<nHLT; t++){
            
            // writing efficiency drops to output.txt
            if((trg.HLT[t]==0)&&(jt.reco.pt[lj]>(GetJetTriggerThreshold(sHLTrigs[t].name)+50.0))){
                if(t<2){continue;}
                if(drop_flag==0){
                    outputFile<<"\n"<<"filename:"<<input<<"\n";
                    outputFile<<"run:lumi:event:hiBin of drop "<< evt.run<<":"<<evt.lumi<<":"<<evt.event<<":"<<evt.hiBin<<"\n";
                    outputFile<<"pT:eta:phi for leading offline jet of drop "<< jt.reco.pt[lj]<<":"<<jt.reco.eta[lj]<<":"<<jt.reco.phi[lj]<<"\n";
                    outputFile<<"CHF:NHF:CEF:NEF:MUF for leading offline jet of drop "<<jt.reco.pf.CHF[lj]<<":"<<jt.reco.pf.NHF[lj]<<":"<<jt.reco.pf.CEF[lj]<<":"<<jt.reco.pf.NEF[lj]<<":"<<jt.reco.pf.MUF[lj]<<"\n";
                    drop_flag=1;
                }
                outputFile<<sHLTrigs[t].name<<" didn't fire\n";
            }

            if(trg.HLT[t]==1){
                hists.FillHLT(t, JetHistogramsStruct<maxnref>::kNoDR, jt.reco.pt[lj], TMath::Abs(jt.reco.eta[lj]), evt.hiBin, evt.w);
                if(iHltMatch[t]==1){
                    hists.FillHLT(t, JetHistogramsStruct<maxnref>::kDR, jt.reco.pt[lj], TMath::Abs(jt.reco.eta[lj]), evt.hiBin, evt.w);
                }
            }
        }

        for(std::size_t t=0; t<nL1T; t++){
            if(trg.L1T[t]==1){
                hists.FillL1T(t, JetHistogramsStruct<maxnref>::kNoDR, jt.reco.pt[lj], TMath::Abs(jt.reco.eta[lj]), evt.hiBin, evt.w);
                if(iL1Match[t]==1){
                    hists.FillL1T(t, JetHistogramsStruct<maxnref>::kDR, jt.reco.pt[lj], TMath::Abs(jt.reco.eta[lj]), evt.hiBin, evt.w);
                }
            }
        }

    }
    fi->Close();

    // output files
    outputFile.close();
    TFile *fo = new TFile(output,"recreate");
    hists.Write(fo);
    fo->Close();
}
