### Finding Maximum nref in files
```
root 'executable/Find_maxnref.C("path/to/input/filenames.txt","path/to/output.root","Jet Clustering Algorithm (like akCs4PF for example)",true(iff scanning MC files))'
```
Above is a generic example of how to execute script Find_maxnref.C in a terminal to find maximum event level jet multiplicity (nref) in a list of root files with <sup>*<sup>forests. Executable Find_maxnref.C can be trivially changed to work with root files containing different ttrees.

Below is a working example of using Find_maxnref.C that will produce maxnref.root in the current directory (immediately before it finishes executing). In this working example the jets being checked are clustered with akCs4PF from an MC sample.
```
root 'executable/Find_maxnref.C("/afs/cern.ch/user/n/nbarnett/public/txt_files/filename_txt_files/2026_filenames/filenames_forests_fdamas_2026_PbPb_Dijet_MC.txt","maxnref.root","akCs4PF",true)'
```

### Generating Jet HLT Efficiencies
#### IN PROGRESS
```
root 'executable/JetTurnOn_PbPb_MC_lxplus.C("path/to/input/filenames.txt","path/to/output.root")'
```
Above is a generic terminal command to execute the script JetTurnOn_PbPb_MC_lxplus.C on a list of root files with <sup>*<sup>forests. This is script uses one text file input list of root files, with <sup>*<sup>forests, and produces one output root file.

Below is a working example of using JetTurnOn_PbPb_MC_lxplus.C to generate JetHLTEff_MC.root in the current directory on the list of files /afs/cern.ch/user/n/nbarnett/public/txt_files/filename_txt_files/2026_filenames/filenames_forests_fdamas_2026_PbPb_Dijet_MC.txt
```
root 'executable/JetTurnOn_PbPb_MC_lxplus.C("/afs/cern.ch/user/n/nbarnett/public/txt_files/filename_txt_files/2026_filenames/filenames_forests_fdamas_2026_PbPb_Dijet_MC.txt","JetHLTEff_MC.root")'
```
##### *Forests
Forests are something used in the Heavy Ion Physics Analysis Group (HIN PAG) at CMS, when making root files of TTrees. From a shared CMSSW repository one is able to use code already written to make forests out of a primary dataset stored as miniAOD files. This essentially only shows up in these scripts as a specific TDirectory and TTree labeling within the .root files being processed.
