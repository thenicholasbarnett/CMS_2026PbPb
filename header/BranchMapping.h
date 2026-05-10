#ifndef BRANCHMAPPING_H
#define BRANCHMAPPING_H

#include "TTree.h"
#include "TString.h"

#include <stdexcept>
#include <vector>
#include <utility>

// function to get branch variables from ttree
void SetBranches(TTree* t, const std::vector<std::pair<TString, void*>>& branches){

    // looping through branches
    for(const auto& branch : branches){

        // declaring name of the branch and its address
        TString sBranchName = branch.first;
        void* address = branch.second;

        // if any declared branch is not in the ttree then ending the macro and printing an error
        if(!t->GetBranch(sBranchName)){
            throw std::runtime_error(Form("ERROR: Branch '%s' not found in TTree '%s'",sBranchName.Data(),t->GetName()));
        }

        // assigning variables to branches in ttree
        t->SetBranchAddress(sBranchName, address);
    }
}

#endif