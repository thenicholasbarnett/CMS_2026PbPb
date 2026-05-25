#ifndef HISTUTILITIES_H
#define HISTUTILITIES_H

#include "TH1F.h"
#include "TH1I.h"
#include "THnSparse.h"
#include "TString.h"

#include <initializer_list>
#include <vector>

#include "Binning.h"

inline TH1F* MakeTH1F(const TString& hname, const AxisBins& bins) {
    TH1F* h = new TH1F(hname, hname, bins.nBins, bins.lo, bins.hi);
    h->SetTitle(bins.title);
    return h;
}

inline TH1I* MakeTH1I(const TString& hname, const AxisBins& bins) {
    TH1I* h = new TH1I(hname, hname, bins.nBins, bins.lo, bins.hi);
    h->SetTitle(bins.title);
    return h;
}

inline THnSparseF* MakeTHnSparseF(const TString& name, const TString& title, std::initializer_list<AxisBins> axes) {
    const std::vector<AxisBins> axVec(axes);
    const Int_t ndim = axVec.size();
    std::vector<Int_t> nbins;
    std::vector<Double_t> xmin, xmax;
    for (const auto& ax : axVec) {
        nbins.push_back(ax.nBins);
        xmin.push_back(ax.lo);
        xmax.push_back(ax.hi);
    }
    THnSparseF* h = new THnSparseF(name, title, ndim, nbins.data(), xmin.data(), xmax.data());
    for(Int_t i = 0; i < ndim; i++){h->GetAxis(i)->SetTitle(axVec[i].title);}
    return h;
}
 
template<typename T>
void WriteAll(T* h){if(h)h->Write();}
 
template<typename T>
void WriteAll(std::vector<T>& v) {for(auto& entry : v) WriteAll(entry);}
 
#endif
