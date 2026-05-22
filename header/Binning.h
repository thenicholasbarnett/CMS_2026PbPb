#ifndef BINNING_H
#define BINNING_H

#include "TString.h"
#include "Rtypes.h"

#include <vector>

struct AxisBins{
    Int_t nBins;
    Float_t lo;
    Float_t hi;
};

struct RangeBin{
    Float_t lo;
    Float_t hi;
    TString title;
    TString shortName;
};

struct BinningStruct{

    // histogram axes
    AxisBins vz = {40, -20.0, 20.0};
    AxisBins pt = {100, 0.0, 1000.0};
    AxisBins eta = {50, -5.1, 5.1};
    AxisBins phi = {64, -3.2, 3.2};
    AxisBins nref = {30, 0, 30};
    AxisBins trig = {2, 0, 2};
    AxisBins pfFrac = {10, 0.0, 1.0};
    AxisBins pfMult = {50, 0, 50};
    AxisBins hiBin = {200, 0, 200};

    // centrality
    std::vector<RangeBin> hiBins = {
        {0, 200, "0 < hiBin < 200", "_hiBin_0_to_200"},
        {0, 20, "0 < hiBin < 20", "_hiBin_0_to_20"},
        {20, 60, "20 < hiBin < 60", "_hiBin_20_to_60"},
        {60, 100, "60 < hiBin < 100", "_hiBin_60_to_100"},
        {100, 160, "100 < hiBin < 160", "_hiBin_100_to_160"},
        {160, 200, "160 < hiBin < 200", "_hiBin_160_to_200"}
    };

    // eta
    std::vector<RangeBin> etaBins = {
        {0.0, 1.2, "|#eta| < 1.2", "_eta_0p0_to_1p2"},
        {0.0, 2.4, "|#eta| < 2.4", "_eta_0p0_to_2p4"},
        {0.0, 5.1, "|#eta| < 5.1", "_eta_0p0_to_5p1"},
        {2.7, 5.1, "2.7 < |#eta| < 5.1", "_eta_2p7_to_5p1"}
    };
};

#endif