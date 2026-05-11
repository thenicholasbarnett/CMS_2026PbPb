#include "TFile.h"
#include "TTree.h"
#include "TMath.h"
#include "TDirectory.h"

#include <fstream>
#include <iostream>
#include <string>
#include <stdexcept>

#include "../header/BranchMapping.h"
#include "../header/EventMap_MC_PbPb.h"
#include "../header/JetMap.h"
#include "../header/TriggerMap_PbPb_MC.h"
#include "../header/Binning.h"
#include "../header/JetSpectraHistograms.h"
#include "../header/JetSelection_PbPb.h"

void JetHLT_SpectraGenerator_PbPb_MC_lxplus(const TString& input_file_list, const TString& output){

    // minimum pt of jets to include
    static constexpr Float_t ptcut = 20.0;

    // more than number of jets in any event being processed
    static constexpr Int_t maxnref = 150;

    // ttree names
    const Int_t nTTrees = 4;
    TString sTTrees[nTTrees] = {"hiEvtAnalyzer/HiTree", "skimanalysis/HltTree", "akCs4PFJetAnalyzer/t", "hltanalysis/HltTree"};

    // for applying Jet Selections from headers
    JetSelect js;

    // event objects
    EventStruct evt;
    FiltersStruct fltr;

    // jet object
    JetStruct<maxnref> jt(ptcut);

    // trigger object
    TriggersStruct trg;

    // binning
    BinningStruct bins;

    // histograms
    JetSpectraStruct hists(bins);

    // getting list of root files to process
    ifstream myfile(input_file_list);
    string filename;

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
        cout<<"processing file "<< filenumber <<": "<<input<<endl;

        // getting ttrees, printing iff the ttree isn't in the input file
        TTree *ttrees[nTTrees];
        for(int t=0; t<nTTrees; t++){
            ttrees[t] = (TTree*)fi->Get(sTTrees[t]);
            if(!ttrees[t]){throw std::runtime_error(Form("ERROR: Could not find TTree '%s' in file",sTTrees[t].Data()));}
        }

        // assigning variables to branches
        SetBranches(ttrees[0],evt.BranchMap());
        SetBranches(ttrees[1],fltr.BranchMap());
        SetBranches(ttrees[2],jt.BranchMap());
        SetBranches(ttrees[3],trg.BranchMap());

        // getting HLT object ttrees
        TTree* HLTObjTTrees[nHLT];
        for(std::size_t t=0; t<nHLT; t++){
            TString objTreeName = GetHLTObjTreeName(t);
            HLTObjTTrees[t] = (TTree*)fi->Get(objTreeName);
            if(!HLTObjTTrees[t]){throw std::runtime_error(Form("ERROR: Could not find TTree '%s' in file",objTreeName.Data()));}
            SetBranches(HLTObjTTrees[t], trg.HLTObjBranchMap(t));
        }

        // looping over events
        Long64_t nentries = ttrees[0]->GetEntries();
        for(Long64_t i=0; i<nentries; i++){
            
            // getting event and event filter info from each event
            ttrees[0]->GetEntry(i);
            ttrees[1]->GetEntry(i);

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
            for(std::size_t t=0; t<nHLT; t++){HLTObjTTrees[t]->GetEntry(i);}

            // terminal will yell at me if the leading jet index isn't zero (it never yells at me)
            Int_t lj=0;
            for(Int_t j=0; j<jt.nref; j++){if(jt.pt[j]>jt.pt[lj]){lj=j;}}
            if(lj!=0){cout<<"leading jet index is "<<lj<<endl;}

            /// Skipping events without jets or with a leading jet below ptcut
            if((jt.nref==0)||(jt.pt[lj]<=ptcut)){continue;}

            // only looking at events that fire the minbias l1 trigger
            if(trg.L1T[0]==0){continue;}

            // using header file for jet identification
            if(!js.JetSelection(jt.eta[lj], jt.phi[lj], jt.PfCEF[lj], jt.PfNEF[lj],jt.PfMUF[lj])){continue;}

            // filling histograms for events passing cuts
            hists.vz->Fill(evt.vz, evt.w);
            hists.hiBin->Fill(evt.hiBin, evt.w);
            hists.nref->Fill(jt.nref, evt.w);

            // matching HLT object to offline jet
            int iHltMatch[nHLT] = {0};
            for(std::size_t t=0; t<nHLT; t++){
                if(trg.HLT[t]==0){continue;}

                auto* obj_pt  = trg.HLT_JetObj_pt[t];
                auto* obj_eta = trg.HLT_JetObj_eta[t];
                auto* obj_phi = trg.HLT_JetObj_phi[t];
                if(!obj_pt || !obj_eta || !obj_phi){continue;}

                for(std::size_t tj=0; tj<obj_pt->size(); tj++){

                    Double_t deta = TMath::Abs(obj_eta->at(tj) - jt.eta[lj]);
                    Double_t dphi = TMath::ACos(TMath::Cos(obj_phi->at(tj) - jt.phi[lj]));
                    Double_t dR = TMath::Sqrt(deta*deta + dphi*dphi);

                    if((dR<0.3) && (obj_pt->at(tj)>=GetJetTriggerThreshold(sHLTrigs[t]))){
                        iHltMatch[t] = 1;
                        break;
                    }
                }
            }

            // looping through eta ranges
            for(std::size_t b=0; b<bins.etaBins.size(); b++){
                const auto& etaBin = bins.etaBins[b];
                if((TMath::Abs(jt.eta[lj])<etaBin.lo)||(TMath::Abs(jt.eta[lj])>etaBin.hi)){continue;}

                hists.ljtpt_eta[b]->Fill(jt.pt[lj], evt.w);

                // looping through HLTs
                for(std::size_t t=0; t<nHLT; t++){
                    if(trg.HLT[t]==1){
                        hists.hlt_ljtpt_eta[JetSpectraStruct::kNoDR][b][t]->Fill(jt.pt[lj], evt.w);
                        if(iHltMatch[t]==1){hists.hlt_ljtpt_eta[JetSpectraStruct::kDR][b][t]->Fill(jt.pt[lj], evt.w);}
                    }
                }

                // looping through L1Ts
                for(std::size_t t=0; t<nL1T; t++){
                    if(trg.L1T[t] == 1){hists.l1_ljtpt_eta[b][t]->Fill(jt.pt[lj], evt.w);}}

                for(std::size_t hb=0; hb<bins.hiBins.size(); hb++){
                    const auto& hiBin = bins.hiBins[hb];
                    if((evt.hiBin<hiBin.lo)||(evt.hiBin>hiBin.hi)){continue;}

                    hists.ljtpt_hibin[b][hb]->Fill(jt.pt[lj], evt.w);
                    
                    for(std::size_t t=0; t<nHLT; t++){
                        if(trg.HLT[t]==1){
                            hists.hlt_ljtpt_hibin[JetSpectraStruct::kNoDR][b][t][hb]->Fill(jt.pt[lj], evt.w);
                            if(iHltMatch[t]==1){hists.hlt_ljtpt_hibin[JetSpectraStruct::kDR][b][t][hb]->Fill(jt.pt[lj], evt.w);}
                        }
                    }

                    // looping through L1Ts
                    for(std::size_t t=0; t<nL1T; t++){if(trg.L1T[t] == 1){hists.l1_ljtpt_hibin[b][t][hb]->Fill(jt.pt[lj], evt.w);}}
                }
            }
        }
        fi->Close();
    }

    // making output file to store histograms
    TFile *fo = new TFile(output,"recreate");
    fo->cd();

    // writing the histogram to output file //

    // making directories in output root file
    TDirectory *devt = fo->mkdir("event");
    TDirectory *dji = fo->mkdir("jet_inclusive");
    TDirectory *djh = fo->mkdir("jet_hibin");

    // event info histograms
    devt->cd();
    hists.vz_unpassed->Write();
    hists.pclustF->Write();
    hists.ppvF->Write();
    hists.pphfF->Write();
    hists.vz->Write();
    hists.hiBin->Write();
    hists.nref->Write();

    // jet histograms by eta 
    for(std::size_t b=0; b<bins.etaBins.size(); b++){
        dji->cd(); 
        hists.ljtpt_eta[b]->Write();
        for(std::size_t t=0; t<nHLT; t++){
            hists.hlt_ljtpt_eta[JetSpectraStruct::kNoDR][b][t]->Write();
            hists.hlt_ljtpt_eta[JetSpectraStruct::kDR][b][t]->Write();
            if((t>=nL1T)){continue;}
            hists.l1_ljtpt_eta[b][t]->Write();
        }
        // for each hibin
        djh->cd();
        for(std::size_t hb=0; hb<bins.hiBins.size(); hb++){
            hists.ljtpt_hibin[b][hb]->Write();
            for(std::size_t t=0; t<nHLT; t++){
                hists.hlt_ljtpt_hibin[JetSpectraStruct::kNoDR][b][t][hb]->Write();
                hists.hlt_ljtpt_hibin[JetSpectraStruct::kDR][b][t][hb]->Write();
                if((t>=nL1T)){continue;}
                hists.l1_ljtpt_hibin[b][t][hb]->Write();
            }
        }
    }
    fo->Close();
}
