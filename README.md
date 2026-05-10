## Finding Maximum nref in files
```
root 'executable/Find_maxnref.C("path/to/input/filenames.txt","path/to/output.root","Jet Clustering Algorithm (like akCs4PF for example)",true(iff scanning MC files))'
```
Above is a generic example of how to execute script Find_maxnref.C in a terminal to find maximum event level jet multiplicity (nref) in a list of root files with forests. Executable Find_maxnref.C can be trivially changed to work with root files containing different ttrees.

Below is a working example of using Find_maxnref.C that will produce maxnref.root in the current directory (immediately before it finishes executing).
```
root 'executable/Find_maxnref.C("/afs/cern.ch/user/n/nbarnett/public/txt_files/filename_txt_files/2026_filenames/filenames_forests_fdamas_2026_PbPb_Dijet_MC.txt","maxnref.root","akCs4PF",true)'
```

### Generating Jet HLT Efficiencies
#### IN PROGRESS

```
root 'executable/JetTurnOn_PbPb_MC_lxplus.C("path/to/input/filenames.txt","path/to/output.root")'
```

```
root 'executable/JetTurnOn_PbPb_MC_lxplus.C("/afs/cern.ch/user/n/nbarnett/public/txt_files/filename_txt_files/2026_filenames/filenames_forests_fdamas_2026_PbPb_Dijet_MC.txt","JetHLTEff_MC.root")'
```
