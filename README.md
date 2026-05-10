## Finding Maximum nref in files
```
root 'Find_maxnref.C("path/to/input/filenames.txt","path/to/output.root","Jet Clustering Algorithm (like akCs4PF for example)",true(iff scanning MC files))'
```
```
root 'Find_maxnref.C("/afs/cern.ch/user/n/nbarnett/public/txt_files/filename_txt_files/2026_filenames/filenames_forests_fdamas_2026_PbPb_Dijet_MC.txt","maxnref.root","akCs4PF",true)'
```

## Generating Jet HLT Efficiencies

```
root 'executable/JetTurnOn_PbPb_MC_lxplus.C("path/to/input/filenames.txt","path/to/output.root")'
```

```
root 'executable/JetTurnOn_PbPb_MC_lxplus.C("/afs/cern.ch/user/n/nbarnett/public/txt_files/filename_txt_files/2026_filenames/filenames_forests_fdamas_2026_PbPb_Dijet_MC.txt","JetHLTEff_MC.root")'
```
