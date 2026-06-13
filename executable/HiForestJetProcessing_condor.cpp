#include "TFile.h"
#include "TTree.h"
#include "TMath.h"
#include "TSystem.h"
#include "TDirectory.h"

#include <fstream>
#include <iostream>
#include <string>
#include <stdexcept>

#include "header/JetSelection_PbPb.h"
#include "header/JSON_handler.h"

#include "header/Binning.h"
#include "header/BranchMapping.h"

#include "header/EventStructs_PbPb.h"
#include "header/JetStruct.h"
#include "header/JetHistograms.h"

#include "header/JetTriggers_2026PbPb.h"
// #include "header/JetTriggers_2025PbPb.h"

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

void HiForestJetProcessing_condor(const TString& input, const TString& output, const TString& output_txt, const TString& isMCArg){
    if(isMCArg != "true" && isMCArg != "True" && isMCArg != "yes" && isMCArg != "Yes" && isMCArg != "false" && isMCArg != "False" && isMCArg != "no" && isMCArg != "No" && isMCArg != "1" && isMCArg != "0"){
        std::cerr << "ERROR: isMC must be true or false" << std::endl;
        return;
    }
    bool isMC = (isMCArg == "true" || isMCArg == "True" || isMCArg == "yes" || isMCArg == "Yes" || isMCArg == "1");
    run(input, output, output_txt, isMC);
}

const Int_t nBins = 200; // table of bin edges
const Double_t binTable[nBins+1] = {0, 10.5072, 11.2099, 11.8364, 12.478, 13.1194, 13.7623, 14.4081, 15.0709, 15.7532, 16.4673, 17.1881, 17.923, 18.673, 19.4865, 20.3033, 21.1536, 22.0086, 22.9046, 23.8196, 24.7924, 25.8082, 26.8714, 27.9481, 29.0828, 30.2757, 31.5043, 32.8044, 34.1572, 35.6142, 37.1211, 38.6798, 40.3116, 42.0398, 43.8572, 45.6977, 47.6312, 49.6899, 51.815, 54.028, 56.3037, 58.7091, 61.2024, 63.8353, 66.5926, 69.3617, 72.2068, 75.2459, 78.3873, 81.5916, 84.9419, 88.498, 92.1789, 95.9582, 99.8431, 103.739, 107.78, 111.97, 116.312, 120.806, 125.46, 130.269, 135.247, 140.389, 145.713, 151.212, 156.871, 162.729, 168.762, 174.998, 181.424, 188.063, 194.907, 201.942, 209.19, 216.683, 224.37, 232.291, 240.43, 248.807, 257.416, 266.256, 275.348, 284.668, 294.216, 304.053, 314.142, 324.488, 335.101, 345.974, 357.116, 368.547, 380.283, 392.29, 404.564, 417.122, 429.968, 443.116, 456.577, 470.357, 484.422, 498.78, 513.473, 528.479, 543.813, 559.445, 575.411, 591.724, 608.352, 625.344, 642.686, 660.361, 678.371, 696.749, 715.485, 734.608, 754.068, 773.846, 794.046, 814.649, 835.608, 856.972, 878.719, 900.887, 923.409, 946.374, 969.674, 993.435, 1017.62, 1042.21, 1067.28, 1092.72, 1118.64, 1144.96, 1171.71, 1198.98, 1226.67, 1254.82, 1283.46, 1312.65, 1342.21, 1372.27, 1402.85, 1433.93, 1465.49, 1497.62, 1530.29, 1563.49, 1597.22, 1631.49, 1666.37, 1701.8, 1737.75, 1774.35, 1811.51, 1849.29, 1887.75, 1926.79, 1966.6, 2006.97, 2047.99, 2089.71, 2132.1, 2175.23, 2219.17, 2263.72, 2309.2, 2355.43, 2402.47, 2450.33, 2499.05, 2548.66, 2599.16, 2650.59, 2703.03, 2756.32, 2810.75, 2866.27, 2922.91, 2980.54, 3039.47, 3099.53, 3160.98, 3223.66, 3287.71, 3353.18, 3420.34, 3489.13, 3559.72, 3632.06, 3706.18, 3782.42, 3860.78, 3941.42, 4024.52, 4110.27, 4199.4, 4292.8, 4394.49, 4519.52, 5199.95};

Int_t getHiBinFromhiHF(const Double_t hiHF){
    Int_t binPos = -1;
    for(int i = 0; i < nBins; ++i){
        if(hiHF >= binTable[i] && hiHF < binTable[i+1]){
        binPos = i;
        break;
        }
    }
    binPos = nBins - 1 - binPos;
    return (Int_t)(200*((Double_t)binPos)/((Double_t)nBins));
}
 
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

    Float_t hiHF;

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

    ttrees[0]->SetBranchStatus("hiHF",1);
    ttrees[0]->SetBranchAddress("hiHF",&hiHF);

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
        Int_t centBin = getHiBinFromhiHF(hiHF);

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
        hists.hiBin->Fill(centBin, evt.w);
        hists.nref->Fill(jt.reco.nref, evt.w);

        // jet loop
        for(unsigned int j=0; j<jt.reco.nref; j++){
            if(jt.reco.pt[j]<ptcut){continue;}
            if(!js.JetSelection(jt.reco.eta[j], jt.reco.phi[j], jt.reco.pf.CEF[j], jt.reco.pf.NEF[j],jt.reco.pf.MUF[j])){continue;}
            hists.FillKin(jt.reco, j, centBin, evt.w);
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
                    outputFile<<"run:lumi:event:hiBin of drop "<< evt.run<<":"<<evt.lumi<<":"<<evt.event<<":"<<centBin<<"\n";
                    outputFile<<"pT:eta:phi for leading offline jet of drop "<< jt.reco.pt[lj]<<":"<<jt.reco.eta[lj]<<":"<<jt.reco.phi[lj]<<"\n";
                    outputFile<<"CHF:NHF:CEF:NEF:MUF for leading offline jet of drop "<<jt.reco.pf.CHF[lj]<<":"<<jt.reco.pf.NHF[lj]<<":"<<jt.reco.pf.CEF[lj]<<":"<<jt.reco.pf.NEF[lj]<<":"<<jt.reco.pf.MUF[lj]<<"\n";
                    drop_flag=1;
                }
                outputFile<<sHLTrigs[t].name<<" didn't fire\n";
            }

            if(trg.HLT[t]==1){
                hists.FillHLT(t, JetHistogramsStruct<maxnref>::kNoDR, jt.reco.pt[lj], TMath::Abs(jt.reco.eta[lj]), centBin, evt.w);
                if(iHltMatch[t]==1){
                    hists.FillHLT(t, JetHistogramsStruct<maxnref>::kDR, jt.reco.pt[lj], TMath::Abs(jt.reco.eta[lj]), centBin, evt.w);
                }
            }
        }

        for(std::size_t t=0; t<nL1T; t++){
            if(trg.L1T[t]==1){
                hists.FillL1T(t, JetHistogramsStruct<maxnref>::kNoDR, jt.reco.pt[lj], TMath::Abs(jt.reco.eta[lj]), centBin, evt.w);
                if(iL1Match[t]==1){
                    hists.FillL1T(t, JetHistogramsStruct<maxnref>::kDR, jt.reco.pt[lj], TMath::Abs(jt.reco.eta[lj]), centBin, evt.w);
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
