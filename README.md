<table>
<tr>
<td><img src="image/CMS_logo.png" alt="CMS logo" width="500"/></td>
<td>This repository is to be of use for the 2026 PbPb collisions at the Compact Muon Solenoid. Executables in this repository can be used to monitor jet trigger and offline object health, or for help with run related activities and studies. Any and all contents of this repository are welcome to be used by anyone for any reason.</td>
</tr>
</table>

<h1>Executables</h1>

Each executable file in this repository is documented in this table.
<br>
Details of each executable in this table are given as dropdowns below.

| Executable | Task |
| :-: | - |
| `batch_hadd.sh` | Merge many ROOT files |
| `Find_maxnref.C` | Find maximum nref in a filelist |
| `JetHLT_SpectraGenerator_PbPb_lxplus.cpp` | Make leading jet p<sub>T</sub> spectra for different triggers |
| `JetHLT_EfficiencyGenerator.C` | Make jet trigger efficiencies |
| `JetHealth_PbPb_lxplus.cpp` | Object health checks |

<h2>General Use Executables</h2>

Some executable files in this repository can be used for general purposes. 
<br>
Interacting with HTCondor and CRAB as well as combining ROOT files are some example uses of the executables in this repository.

<details>
<summary>batch_hadd.sh</summary>

<h3>Merging Many ROOT Files</h3>

Many ROOT files can be merged using the command hadd. The command `hadd output.root path/to/inputs/*root` will add compatible ROOT objects, such as TTrees, RNTuples, and histograms. To be compatible the input files must have the same object names. These added objects are placed into an output ROOT file upon running hadd. This command is particularly useful after processing many files in parallel. More information on hadd can be found in the [ROOT documentation](https://root.cern/doc/v638/hadd_8cxx.html).
<br><br>
Using hadd on very many files may take an exceedingly long time and is more likely to give ill-defined behavior or crash. By using hadd on subsets of a large number of files, then using hadd on the output, these issues can be avoided. This shell script avoids some issues with hadd by iteratively merging batches of ROOT files. Additionally, this bash script runs hadd multiple times in parallel on the same machine, further reducing the time to add many ROOT files.
<br><br>
This bash script can be executed by launching a new bash process in a couple ways.
```
bash batch_hadd.sh OUT_FILE "IN_FILES" BATCH_SIZE NJOBS
```
After giving execute permission (`chmod +x batch_hadd.sh`) this shell script can also be executed in a child process like this.
```
./batch_hadd.sh OUT_FILE "IN_FILES" BATCH_SIZE NJOBS
```
This bash script can be executed with the following terminal commands in the current shell session.
> WARNING: Executing in the current shell session is not recommended because if the argument validation fails then the current terminal session will close entirely
```
source batch_hadd.sh OUT_FILE "IN_FILES" BATCH_SIZE NJOBS
```
```
. batch_hadd.sh OUT_FILE "IN_FILES" BATCH_SIZE NJOBS
```
Details on each positional argument this shell takes as an input.
| Argument | Description |
| :-: | - |
| `OUT_FILE` | Merged output ROOT file. |
| `IN_FILES` | Pattern for the input ROOT files to be merged. Example: `path/to/files/*root` |
| `BATCH_SIZE` | Number of ROOT files in each batch to hadd. |
| `NJOBS` | Number of parallel hadd commands to run. |
</details>

<details>
<summary>find_maxnref.cpp</summary>

<h3>Finding Maximum nref in ROOT Files</h3>

The number of jets in a single event is stored in a HiForest as nref, in the JetAnalyzer TTree. Jet variables, such as p<sub>T</sub>, are stored as arrays in the same TTree. If the arrays declared that are mapped to each jet variable for an event have less entries than nref for that event then ill-defined behavior will occur or the execution will crash. Finding the maximum nref for a set of ROOT files containing JetAnalyzer TTrees can be useful so that the size of arrays declared when processing these files do not cause issues.
<br><br>
Like most C++ macros in this repository this file can be both compiled with `g++` as well as interpreted with Cling. 
Cling is an interactive C++ interpreter that is built in to ROOT. 
<br><br>
This macro can be compiled into a stadalone executable with the following command.
```
g++ -o mnref find_maxnref.cpp $(root-config --cflags --libs)
```
After compiling this binary executable can be run.
```
./mnref <filelist.txt> <output.root> <JetAlgorithm> <isMC>
```
This macro can also be executed using ROOT with the Cling interpreter.
```
root -l -b -q 'find_maxnref.cpp("filelist.txt","output.root","JetAlgorithm",isMC)'
```
The input arguments for this macro are listed in this table.
| Argument | Description |
| :-: | - |
| `filelist.txt` | Plain text file of input ROOT files, one for each line. |
| `output.root` | Name of output ROOT file to store processed information. |
| `JetAlgorithm` | Clustering algorithm used to populate TTree. |
| `isMC` | Bool specifying whether the inputs are MC or not. |

</details>

<h2>Jet HLT Efficiency Executables</h2>

Executables used to generate jet HLT efficiencies for the 2026 PbPb run are in this repository. The executables below can be run sequentially to produce jet HLT effeciciencies.
<br><br>
Jet trigger efficiency can be defined as various ratios of leading jet p<sub>T</sub> spectra for different triggers.
Generating leading jet p<sub>T</sub> spectra is the first step to take when determining jet trigger efficiencies.
In this workflow executing `JetHLT_SpectraGenerator_PbPb_lxplus.cpp` on a list of ROOT files with JetAnalyzer and TTrees will provide an output ROOT file that can be used to generate HLT efficiencies for jets.

<details>
<summary>JetHLT_SpectraGenerator_PbPb_lxplus.cpp</summary>

<h3>Generating Leading Jet p<sub>T</sub> Spectra for Jet HLT Efficiencies</h3>

This C++ macro can be compiled with `g++` into a standalone executable or interpreted directly with ROOT.
<br><br>
This macro can be compiled into a binary executable with the following line.
```
g++ -o JetSpectra JetHLT_SpectraGenerator_PbPb_lxplus.cpp $(root-config --cflags --libs)
```
After compiling, this standalone executable can be run with the following command.
```
./JetSpectra filelist.txt output.root isMC
```
This macro can also be executed with ROOT by using its built-in Cling interpreter. 
```
root -l -b -q 'JetHLT_SpectraGenerator_PbPb_lxplus.cpp("filelist.txt","output.root",isMC)'
```
Below are details on each positional argument this macro expects.
| Argument | Description |
| :-: | - |
| `filelist.txt` | Plain text file containing one input file, including its path, on each line. |
| `output.root` | ROOT file made by this macro containing jet spectra. |
| `isMC` | Bool specifying to use weights or not. Can be `true`, `false`, `1`, or `0`. |

</details>

<details>
<summary>JetHLT_EfficiencyGenerator.cpp</summary>

<h3>Generating Jet HLT Efficiencies</h3>

> IN PROGRESS

</details>

<h2>Object Health Checks</h2>

Executables here can help monitor the health of jets during the 2026 PbPb run. Object health is a necessary check to do during this run, particularly due to the degraded detector conditions in comparison the rest of Run 3.

<details>
<summary>JetHealth_PbPb_lxplus.cpp</summary>

<h3>Checking Jet Health</h3>

Similarly to other executables in this repository this C++ macro can be both compiled and interpreted.
Compile this macro with `g++` and execute as a compiled binary using the following commands.

```
g++ -o JetHealth CMS_2026PbPb/executable/JetHealth_PbPb_lxplus.cpp $(root-config --cflags --libs)
```
```
./JetHealth <filelist.txt> <output.root> <isMC>
```
Execute this macro with Cling using ROOT with the following command.
```
root -l -b -q 'JetHealth_PbPb_lxplus.cpp("filelist.txt","output.root",isMC)'
```
The input arguments for this macro are listed in this table.
| Argument | Description |
| :-: | - |
| `filelist.txt` | Plain text file of input ROOT files, one for each line. |
| `output.root` | Name of output ROOT file to store processed information. |
| `isMC` | Bool specifying whether the inputs are MC or not. |
</details>

<h2>Submitting Condor Jobs</h2>

An HTCondor wrapper for easy (and fun!) Condor job submission on LXPLUS lives in this repo and [a separate one](https://github.com/thenicholasbarnett/cms-condor-wrapper) — see executable/Condor for the full README.
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
