<table>
<tr>
<td><img src="image/CMS_logo.png" alt="CMS logo" width="500"/></td>
<td>This repository is to be of use for the 2026 PbPb collisions at the Compact Muon Solenoid. Executables in this repository can be used to monitor jet trigger and offline object health, or for help with run related activities and studies. Any and all contents of this repository are welcome to be used by anyone for any reason.</td>
</tr>
</table>

<h1>Executables</h1>

Each executable file in this repository is documented in this table.
<br>
Details of each executable in this table are given below.

| Executable | Task |
| :-: | - |
| `HiForestJetProcessing.cpp` | Processing jets in HiForest files |
| `PlotJetHealth.cpp` | Plotting object health checks |
| `JetHLT_EfficiencyGenerator.cpp` | Make jet trigger efficiencies |
| `batch_hadd.sh` | Merge many ROOT files |
| `find_maxnref.cpp` | Find maximum nref in a filelist |

<h2>Processing HiForest files</h2>

Executables here can monitor jet health and HLT performances, but information from the events needs to be extracted first. High dimensional histograms are filled by information in HiForest files. These high dimensional histograms are sliced, rebinned, and projected by other macros to perform object health checks and produce jet trigger efficiency studies.

<h3> <em>HiForestJetProcessing.cpp</em> </h3>

This C++ macro can be compiled with `g++` into a standalone executable or interpreted directly with ROOT.
<br><br>
This macro can be compiled into a binary executable with the following line.
```
g++ -o JetProcessing CMS_2026PbPb/executable/HiForestJetProcessing.cpp $(root-config --cflags --libs)
```
After compiling, this standalone executable can be run with the following command.
```
./JetProcessing filelist.txt output.root isMC
```
This macro can also be executed with ROOT by using its built-in Cling interpreter. 
```
root -l -b -q 'CMS_2026PbPb/executable/HiForestJetProcessing.cpp("filelist.txt","output.root",isMC)'
```
Below are details on each positional argument this macro expects.
| Argument | Description |
| :-: | - |
| `filelist.txt` | Plain text file containing one input file, including its path, on each line. |
| `output.root` | ROOT file made by this macro. |
| `isMC` | Bool specifying to use weights or not. Can be `true`, `false`, `1`, or `0`. |

<h2>Object Health</h2>

Executables here can help monitor the health of jets, intended for use during the 2026 PbPb run. Object health is a necessary check to do during this data taking period, particularly due to the degraded detector conditions compared to previous years in Run 3. 
<br><br>
The main tool in this repository made to monitor jet health is an executable C++ file. This tool will make a timestamped directory with portable network graphic (`.png`) files displaying kinematics (p<sub>T</sub>, η, ϕ), maps of η vs ϕ, and particle-flow (PF) energy fractions. The η-ϕ maps are displayed for different minimum p<sub>T</sub> values, the PF fractions are shown for different |η| ranges, and all of these images are made for various centrality intervals. All of this dimensionality can be easily adjusted in some of the header files, particularly in `Binning.h`.
<br><br>
The file `JetHealthPlotting.h` makes the various histgrams plotted. The executable provided, `PlotJetHealth.cpp`, will produce these plots from the output of `HiForestJetProcessing.cpp`. Changing `JetHealthPlotting.h` and executing `PlotJetHealth.cpp` produces new displays after processing.

<h3> <em>PlotJetHealth.cpp</em> </h3>

This macro should be used to provide insight into the performance of jets. Comparing plots produced with this macro from jets collected during this data taking period to jets previously collected in Run 3, or generated in MC samples, will help display the health of jets being collected. This executable processes input HiForest files, stores jet information into an output ROOT file, and generates the plots described above. Additional plots can be made from the information stored in an optional output file from this macro using `PlotJetHealth.cpp`.
<br><br>
An output file name can be specified to this macro if desired, which will store all projected histograms. If the input file given to this macro is corrupt or not present then an ASCII zombie will show up in the terminal, and a standard runtime error will occur. 
<br><br>
As with all C++ macros in this repository this executable can be interpreted or compiled. Executing this file with the Cling interpreter using ROOT is recommended. 
```
root -l -q 'CMS_2026PbPb/executable/PlotJetHealth.cpp("input.root")'
```
> NOTE: An output file can also be provides as an argument e.g. ("input.root", "output.root")

This macro can also be compiled to be ran as a standalone binary. Any changes to the plotting composition here will require this macro to be compiled again.
```
g++ -o PlotJetHealth CMS_2026PbPb/executable/JetHealth_PbPb_lxplus.cpp $(root-config --cflags --libs)
```
```
PlotJetHealth <input.root> [output.root]
```
One argument is taken as an input to this macro.
| Argument | Description |
| :-: | - |
| `input.root` | Output ROOT file from `HiForestJetProcessing.cpp` |
| `output.root` | Optional output filename to write out projected histograms |

<h2>HLT Efficiencies</h2>

Executables used to generate jet HLT efficiencies for the 2026 PbPb run are in this repository. The executables below can be run sequentially to produce jet HLT effeciciencies.
<br><br>
Jet trigger efficiency can be defined as various ratios of leading jet p<sub>T</sub> spectra for different triggers.
Generating leading jet p<sub>T</sub> spectra is the first step to take when determining jet trigger efficiencies.
In this workflow executing `HiForestJetProcessing.cpp` on a list of ROOT files with a JetAnalyzer and event TTrees will provide an output ROOT file that can be used to generate HLT efficiencies for jets. The output of this step can then be used to calculate efficiency for different jet HLT paths using `JetHLT_EfficiencyGenerator.cpp`.

<h3> <em>JetHLT_EfficiencyGenerator.cpp</em> </h3>

The input to this macro is the output of `HiForestJetProcessing.cpp`. A trigger's efficiency can be defined as the ratio of the leading jet p<sub>T</sub> for events passing a particular trigger to the leading jet p<sub>T</sub> for all events in the same sample. Requiring the minimum bias trigger in both the numberator and denominator of this ratio will make the sample used to produce the efficiencies minimally biased.
<br><br>
An output file name can be specified to this macro if desired, which will store all projected histograms. If the input file given to this macro is corrupt or not present then an ASCII zombie will show up in the terminal, and a standard runtime error will occur. 
<br><br>
To compile this C++ script and execute it as a standalone binary use the following commands in sequence.
```
g++ -o JetEfficiency CMS_2026PbPb/executable/JetHLT_EfficiencyGenerator.cpp $(root-config --cflags --libs)
```
```
./JetEfficiency <input.root> [output.root]
```
Execute this macro using the Cling interpreter native to ROOT with the following command.
```
root -l -q 'CMS_2026PbPb/executable/JetHLT_EfficiencyGenerator.cpp("input.root")'
```
> NOTE: An output file can also be provides as an argument e.g. ("input.root", "output.root")

The arguments for this macro are simply an input and output ROOT file. The input file for this macro is the output of `HiForestJetProcessing.cpp`.
| Argument | Description |
| :-: | - |
| `input.root` | Output ROOT file from `HiForestJetProcessing.cpp` |
| `output.root` | ROOT file made by this macro containing jet HLT efficiencies. |

<h2>General Use</h2>

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
bash CMS_2026PbPb/executable/batch_hadd.sh OUT_FILE "IN_FILES" BATCH_SIZE NJOBS
```
After giving execute permission (`chmod +x CMS_2026PbPb/executable/batch_hadd.sh`) this shell script can also be executed in a child process like this.
```
./CMS_2026PbPb/executable/batch_hadd.sh OUT_FILE "IN_FILES" BATCH_SIZE NJOBS
```
This bash script can be executed with the following terminal commands in the current shell session.
> WARNING: Executing in the current shell session is not recommended because if the argument validation fails then the current terminal session will close entirely
```
source CMS_2026PbPb/executable/batch_hadd.sh OUT_FILE "IN_FILES" BATCH_SIZE NJOBS
```
```
. CMS_2026PbPb/executable/batch_hadd.sh OUT_FILE "IN_FILES" BATCH_SIZE NJOBS
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
g++ -o mnref CMS_2026PbPb/executable/find_maxnref.cpp $(root-config --cflags --libs)
```
After compiling this binary executable can be run.
```
./mnref <filelist.txt> <output.root> <JetAlgorithm> <isMC>
```
This macro can also be executed using ROOT with the Cling interpreter.
```
root -l -b -q 'CMS_2026PbPb/executable/find_maxnref.cpp("filelist.txt","output.root","JetAlgorithm",isMC)'
```
The input arguments for this macro are listed in this table.
| Argument | Description |
| :-: | - |
| `filelist.txt` | Plain text file of input ROOT files, one for each line. |
| `output.root` | Name of output ROOT file to store processed information. |
| `JetAlgorithm` | Clustering algorithm used to populate TTree. |
| `isMC` | Bool specifying whether the inputs are MC or not. |

</details>

<h2>Submitting Processing Jobs</h2>

- An HTCondor wrapper for easy (and fun!) Condor job submission on LXPLUS lives in this repo and [a separate one](https://github.com/thenicholasbarnett/cms-condor-wrapper) — see executable/Condor for the full README.
- Templates for CRAB3 submissions are also available in this repository, in executable/CRAB. There are two templates there, which allow for submissions to CRAB on a user provided list of input files as well as for one or multiple primary dataset(s) on the Data Aggregation System (DAS).
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
