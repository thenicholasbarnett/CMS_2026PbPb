## How to Use Executable Scripts
<details>
  <summary>Finding Maximum nref in files</summary>
  
```
root 'executable/Find_maxnref.C("path/to/input/filenames.txt","path/to/output.root","Jet Clustering Algorithm (like akCs4PF for example)",true(iff scanning MC files))'
```
Above is a generic example of how to execute script Find_maxnref.C in a terminal to find maximum event level jet multiplicity (nref) in a list of root files with forests<sup>[1]</sup>. Executable Find_maxnref.C can be trivially changed to work with root files containing different ttrees.

Below is a working example of using Find_maxnref.C that will produce maxnref.root in the current directory (immediately before it finishes executing). In this working example the jets being checked are clustered with akCs4PF from an MC sample.
```
root 'executable/Find_maxnref.C("/afs/cern.ch/user/n/nbarnett/public/txt_files/filename_txt_files/2026_filenames/filenames_forests_fdamas_2026_PbPb_Dijet_MC.txt","maxnref.root","akCs4PF",true)'
```
</details>

<details>
  <summary>Generating Jet HLT Efficiencies</summary>

#### IN PROGRESS

```
root 'executable/JetTurnOn_PbPb_MC_lxplus.C("path/to/input/filenames.txt","path/to/output.root")'
```
Above is a generic terminal command to execute the script JetTurnOn_PbPb_MC_lxplus.C on a list of ROOT files with forests<sup>[1]</sup>. This is script uses one text file input list of ROOT files, with forests<sup>[1]</sup>, and produces one output ROOT file.

Below is a working example of using JetTurnOn_PbPb_MC_lxplus.C to generate JetHLTEff_MC.root in the current directory on the list of files /afs/cern.ch/user/n/nbarnett/public/txt_files/filename_txt_files/2026_filenames/filenames_forests_fdamas_2026_PbPb_Dijet_MC.txt
```
root 'executable/JetTurnOn_PbPb_MC_lxplus.C("/afs/cern.ch/user/n/nbarnett/public/txt_files/filename_txt_files/2026_filenames/filenames_forests_fdamas_2026_PbPb_Dijet_MC.txt","JetHLTEff_MC.root")'
```
</details>

<details>
  <summary>Combining Many Files</summary>

hadd is a ROOT command that will essentially add ttrees and histograms for multiple files, iff these files have matching names for histograms, ttrees, branchs, etc. 
hadd can be executed like this 'hadd output.root path/to/inputs/*.root'
When using hadd on very many files the process becomes slow and more likely to fail. This issue can be avoided by making bunches of files, using hadd on these bunches to get outputs, then using hadd on the ouputs. When needed this bunching process can be repeated until there is only one output file. The bash script batch_hadd.sh does exactly this, but multiple times simultaneously based on the specified number of cores provided when executing.

Below is a generic example of how to execute batch_hadd.sh in a terminal. 
```
./batch_hadd.sh path/to/output.root "path/to/inputs/*.root" INT(number per bunch) INT(number of cores)
```
Below is a working example of using batch_hadd.sh to add all the ROOT files from /eos/cms/store/group/phys_heavyions/nbarnett/jra_files/condor_jra_production_ak6pf_2025OO_05_06_2026/ into a single output file in the same directory. In this example, the script groups ROOT files into batches of 10 and runs up to 4 simultaneous hadd processes across 4 CPU cores.
```
./hadd_tree.sh /eos/cms/store/group/phys_heavyions/nbarnett/jra_files/output_jra_production_ak6pf_2025OO_all.root "/eos/cms/store/group/phys_heavyions/nbarnett/jra_files/condor_jra_production_ak6pf_2025OO_05_06_2026/*.root" 10 4
```
NOTE: When using this script you should change the working directory to your own directory.
</details>

<details>
  <summary>[1] Forests or HiForest</summary>
  
Forests are something used in the Heavy Ion Physics Analysis Group (HIN PAG) at CMS, when making ROOT files of TTrees. From a shared [CMSSW repository](https://github.com/CmsHI/cmssw/tree/forest_CMSSW_16_1_X) someone is able to use code already written to make forests out of a primary dataset stored as miniAOD files. This essentially only shows up in these scripts as a specific TDirectory and TTree labeling within the .root files being processed.
</details>
