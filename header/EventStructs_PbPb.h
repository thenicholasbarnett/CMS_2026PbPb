#ifndef EVENTSTRUCTS_PBPB_H
#define EVENTSTRUCTS_PBPB_H

#include "TString.h"
#include "Rtypes.h"

#include <vector>
#include <utility>

// declaring event stucture
struct EventStruct{

    // event variables //
    // weight is 1 for data and is mapped to a branch for MC
    Float_t w = 1.0f;
    // vertex position
    Float_t vz;
    // centrality proxy
    Int_t hiBin;
    // run number
    UInt_t run;
    // event number
    ULong64_t event;
    // lumisection
    UInt_t lumi;

    // mapping variables to branches
    // weight is mapped to a branch iff the file being processed is MC
    std::vector<std::pair<TString, void*>> BranchMap(bool isMC){
        std::vector<std::pair<TString, void*>> branches = {
            {"vz", &vz},
            {"hiBin", &hiBin}
        };
        if(isMC){branches.push_back({"weight", &w});}
        if(!isMC){
            branches.insert(branches.end(), {
                {"run", &run},
                {"evt", &event},
                {"lumi", &lumi}
            });
        }
        return branches;
    }
};

// declaring event filter stucture
struct FiltersStruct{

    // variables for event filters
    Int_t pclustF;
    Int_t ppvF;
    Int_t pphfF;

    // mapping variables to branches
    std::vector<std::pair<TString, void*>> BranchMap(){
        return{
            {"pclusterCompatibilityFilter", &pclustF},
            {"pprimaryVertexFilter", &ppvF},
            {"pphfCoincFilterPF2Th4", &pphfF}
        };
    }
};

#endif
