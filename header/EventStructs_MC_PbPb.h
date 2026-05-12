#ifndef EVENTSTRUCTS_MC_PBPB_H
#define EVENTSTRUCTS_MC_PBPB_H

#include "TString.h"
#include "Rtypes.h"

#include <vector>
#include <utility>

// declaring event stucture
struct EventStruct{

    // event variables //
    // weight
    Float_t w;
    // vertex position
    Float_t vz;
    // centrality proxy
    Int_t hiBin;

    // mapping variables to branches
    std::vector<std::pair<TString, void*>> BranchMap(){
        return{
            {"weight", &w},
            {"vz", &vz},
            {"hiBin", &hiBin}
        };
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