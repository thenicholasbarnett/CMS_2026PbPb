<table>
<tr>
<td><img src="image/CMS_logo.png" alt="CMS logo" width="500"/></td>
<td>This repository was started to be of use for the 2026 PbPb collisions at the Compact Muon Solenoid. Executables in this repository can be used to monitor jet trigger and offline object health, or otherwise help with run related activities and studies. Any and all contents of this repository are welcome to be used by anyone for any reason.</td>
</tr>
</table>

<h1>Executables</h1>

Each executable script in this repository is documented in this table. Details of each executable in this table, including both a generic and working example, are given as dropdowns below.

| Executable                               | Task                                               |
| ---------------------------------------- | -------------------------------------------------- |
| MakeCondor.sh                            | Create and submit Condor jobs                      |
| batch_hadd.sh                            | Combine many ROOT files                            |
| Find_maxnref.C                           | Find maximum nref in a filelist                    |
| JetHLT_SpectraGenerator_PbPb_MC_lxplus.C | Make leading jet pT spectra for different triggers |
| JetHLT_EfficiencyGenerator.C             | Make jet trigger efficiencies                      |

<h2>General Use Executables</h2>

Some executable scripts in this repository can be used for general purposes. Including scripts to help interact with Condor and CRAB as well as combine ROOT files.

<details>
<summary>MakeCondor.sh</summary>

<h3>Submitting Condor Jobs</h3>

> NOTE: Change cmssw_dir inside run_job.sh to desired CMSSW working area.

HTCondor or Condor is a batch job scheduler or execution system. Condor can be accessed through CMSSW, and these scripts can make this a smooth interaction. This setup consists of one driver, one run wrapper, and one submit generator. By simply changing cmssw_dir inside run_job.sh you can also use these bash scripts to schedule Condor jobs with ease. 
<br><br>
Below is a generic terminal command one can use to execute MakeCondor.sh in an LXPLUS terminal after running `chmod +x MakeCondor.sh`, and after making the changes specified above.

```
./executable/Condor/MakeCondor.sh JobName /path/to/executable.C(or executable.py/executable.sh) /path/to/filelist.txt /path/to/store/output/files
```

Below is a working example of how to submit some condor jobs using my files and writing to my directory.

```
./executable/Condor/MakeCondor.sh smeared_dijet_asymmetries_MC_2024ppRef /afs/cern.ch/user/n/nbarnett/public/4_6_2026_JER/smeared_asymmetry_generator_condor_2024ppRef_MC_4_9_2026.C /afs/cern.ch/user/n/nbarnett/public/txt_files/filename_txt_files/2024ppRef_MC_filenames/forests_2024ppRef_MC_withPU_10files.txt /eos/cms/store/group/phys_heavyions/nbarnett/JetCalibrations/2024/smeared_dijet_asymmetries
```

</details>

<details>
<summary>batch_hadd.sh</summary>

<h3>Combining Many ROOT Files</h3>

> NOTE: Change WORKDIR inside batch_hadd.sh to the desired working area directory.

hadd is a ROOT command that will essentially add ttrees and histograms for multiple files, iff these files have matching names for histograms, ttrees, branchs, etc. 
hadd can be executed like this 'hadd output.root path/to/inputs/*.root'
When using hadd on very many files the process becomes slow and more likely to fail. This issue can be avoided by making bunches of files, using hadd on these bunches to get outputs, then using hadd on the ouputs. When needed this bunching process can be repeated until there is only one output file. The bash script batch_hadd.sh does exactly this, but multiple times simultaneously based on the specified number of cores provided when executing.
<br><br>
Below is a generic example of how to execute batch_hadd.sh in a terminal. 

```
./executable/batch_hadd.sh path/to/output.root "path/to/inputs/*.root" INT(number per bunch) INT(number of cores)
```

Below is a working example of using batch_hadd.sh to add all the ROOT files from /eos/cms/store/group/phys_heavyions/nbarnett/jra_files/condor_jra_production_ak6pf_2025OO_05_06_2026/ into a single output file in the same directory. In this example, the script groups ROOT files into batches of 10 and runs up to 4 simultaneous hadd processes across 4 CPU cores.

```
./executable/batch_hadd.sh /eos/cms/store/group/phys_heavyions/nbarnett/jra_files/output_jra_production_ak6pf_2025OO_all.root "/eos/cms/store/group/phys_heavyions/nbarnett/jra_files/condor_jra_production_ak6pf_2025OO_05_06_2026/*.root" 10 4
```

</details>

<details>
<summary>Find_maxnref.C</summary>

<h3>Finding Maximum nref in ROOT Files</h3>

```
root -l -q 'executable/Find_maxnref.C++("path/to/input/filenames.txt","path/to/output.root","Jet Clustering Algorithm (like akCs4PF for example)",true(iff scanning MC files))'
```

Above is a generic example of how to execute script Find_maxnref.C in a terminal to find maximum event level jet multiplicity (nref) in a list of root files with forests. Executable Find_maxnref.C can be trivially changed to work with root files containing different ttrees.

Below is a working example of using Find_maxnref.C that will produce maxnref.root in the current directory (immediately before it finishes executing). In this working example the jets being checked are clustered with akCs4PF from an MC sample.

```
root -l -q 'executable/Find_maxnref.C++("/afs/cern.ch/user/n/nbarnett/public/txt_files/filename_txt_files/2026_filenames/filenames_forests_fdamas_2026_PbPb_Dijet_MC.txt","maxnref.root","akCs4PF",true)'
```

</details>

<h2>Jet HLT Efficiency Executables</h2>

Some of the executable scripts in this repository are specifically used to generate jet HLT efficiencies for the 2026 PbPb run. The executables below can be used sequentially to produce jet HLT effeciciencies.

<details>
<summary>JetHLT_SpectraGenerator_PbPb_MC_lxplus.C</summary>

<h3>Generating Leading Jet p<sub>T</sub> Spectra for Jet HLT Efficiencies</h3>

```
root -l -q 'executable/JetHLT_SpectraGenerator_PbPb_MC_lxplus.C++("path/to/input/filenames.txt","path/to/output.root")'
```

Above is a generic terminal command to execute the script JetHLT_SpectraGenerator_PbPb_MC_lxplus.C on a list of ROOT files with forests. This is script uses one text file input list of ROOT files, with forests, and produces one output ROOT file.

Below is a working example of using JetTurnOn_PbPb_MC_lxplus.C to generate JetHLTEff_MC.root in the current directory on the list of files /afs/cern.ch/user/n/nbarnett/public/txt_files/filename_txt_files/2026_filenames/filenames_forests_fdamas_2026_PbPb_Dijet_MC.txt

```
root -l -q 'executable/JetHLT_SpectraGenerator_PbPb_MC_lxplus.C++("/afs/cern.ch/user/n/nbarnett/public/txt_files/filename_txt_files/2026_filenames/filenames_forests_fdamas_2026_PbPb_Dijet_MC.txt","JetHLTSpectra_MC.root")'
```

</details>

<details>
<summary>JetHLT_EfficiencyGenerator.C</summary>

<h3>Generating Jet HLT Efficiencies</h3>

```
root -l -q 'executable/JetHLT_EfficiencyGenerator.C++("path/to/input.root", "output.root")'
```

Above is an example of how to execute this script with generic inputs. This macro will take in the output from JetHLT_SpectraGenerator_PbPb_MC_lxplus.C, and makes jet trigger turn on curves from it. This script will generate the total jet trigger efficiency, that is the inefficiencies due to an HLT and its L1seed, and relative efficiency, being the inefficiency due to just the HLT. These efficiencies will be generated with and without offline-online object matching.
<br><br>
Below is a working example using an appropriate output to make actual jet efficiencies.

```
root -l -q 'executable/JetHLT_EfficiencyGenerator.C++("/eos/cms/store/group/phys_heavyions/nbarnett/JetHLTSpectra_MC.root", "JetEfficiencies.root")'
```

</details>

___

<h3>Glossary</h3>

<details>
<summary>LXPLUS</summary>

Linux Public Login User Service (LXPLUS) is the interactive logon service for CERN users. The LXPLUS services is comprised of 100 virtual machines provided by the CERN IT Department.  More information can be found at [lxplusdoc.web.cern.ch](https://lxplusdoc.web.cern.ch/)

</details>

<details>
<summary>ROOT</summary>

ROOT is a software framework developed at CERN used by physicists everywhere. ROOT files are objects in compressed binary form which also contain the object format. More information can be found at [root.cern](https://root.cern/)

</details>

<details>
<summary>HiForest</summary>

HiForest, or simply forest, refers to a ROOT file type used in the Heavy Ion Physics Analysis Group (HIN PAG) at CMS, containing TTrees in TDirectories. From a shared [CMSSW repository](https://github.com/CmsHI/cmssw/tree/forest_CMSSW_16_1_X) someone is able to make forests from a primary dataset stored as miniAOD files. This appears in scripts within this repository as specific TDirectory and TTree labeling within .root files being processed.

</details>

<h3>Author</h3>
Nicholas Shawn Barnett (that's me 🙂) is the author of and maintains this repository. I am happy to take any and all suggestions and feedback. If there's anything not working or you would like to see in this repository then please write to nbarne21@uic.edu about any actionable tasks.
