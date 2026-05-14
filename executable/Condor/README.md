<img src="../../image/HTCondor_logo.png" alt="HTCondor logo" width="700"/>

HTCondor, formerly Condor, is a distributed high throughput computing system developed at University of Wisconsin-Madison, providing a specialized workload management system. Users submit jobs to a scheduler, these jobs are placed in a queue, and HTCondor chooses when and where to run the jobs. HTCondor is open source, licensed under Apache 2.0, with extensive documentation. Please see the resources section for additional information.
<br><br>
<h1>HTCondor Job Submission</h1>

Condor can be accessed through LXPLUS, and these bash scripts can make this a smooth interaction. By specifying a working directory, with a CMSSW release, you can also use these bash scripts to schedule Condor jobs with ease. Here is a set of three bash scripts acting as a Condor submission wrapper.

> Prerequisites: Access to LXPLUS and a CMSSW release on AFS or EOS

<br>

| Script | Role | Description |
|----|----|----|
| `MakeCondor.sh` | Driver | Entry point called by the user. Creates a timestamped working directory and delegates to the template. |
| `MakeCondor_template.sh` | Submit Generator | Generates the HTCondor submission file using a template, then submits the jobs. |
| `run_job.sh` | Runtime Wrapper | Executed on the worker node. Sets up CMSSW runtime environment and runs the user-provided executable based on its type (`.py`, `.C`, or `.sh`). |

<h2>Usage</h2>

Specify which CMSSW release to use by setting `CMSSW_src` on line 17 of `run_job.sh` to the desired CMSSW working area before submitting jobs. The CMSSW release will be pinned on any node executing a job submitted through this wrapper, fixing the versions of ROOT, python, and all other dependencies for reproducible job execution.

Executing `MakeCondor.sh` with the following command will submit jobs to an HTCondor scheduler. A working directory is made in the directory staged when executing `MakeCondor.sh`.

```
MakeCondor.sh JOBNAME EXECUTABLE FILELIST OUTPUT_DIR
```

| Argument | Description |
| - | - |
| `JOBNAME` | Label for set of jobs being submitted. Used in directory and file naming. |
| `EXECUTABLE` | Path to macro executing on each worker node for each input file. |
| `FILELIST` | Path to plain text file containing one input file on every line. One Condor job is submitted for each input file. |
| `OUTPUT_DIR` | Directory to store output ROOT files. A timestamped output subdirectory is made here. |

<h2>Working Example</h2>

In this example a C++ macro is generating dijet asymmetries from ten HiForest files made from a hard QCD MC sample of the 2024 pp reference run (5.36 TeV) generated wth Pythia. This pp reference run was collected for comparisons with the PbPb collisions that followed shortly after.

> Don't forget to run `chmod +x MakeCondor.sh` before executing driver

```
./MakeCondor.sh DijetAsymmetry_2024ppRef /afs/cern.ch/user/n/nbarnett/public/executable_files/asymmetry_generator_condor_2024ppRef_MC_5_12_2026.C /afs/cern.ch/user/n/nbarnett/public/txt_files/filelists/filelist_HiForest_2024ppref_MC_withPU_10files.txt .
```

The filelist used is shown below. This format is needed to properly use this Condor submission wrapper.

```
/eos/cms/store/group/phys_heavyions/nbarnett/Forests/MC/forests_2024ppRef_MC_withPU/HiForestMiniAOD_1006.root
/eos/cms/store/group/phys_heavyions/nbarnett/Forests/MC/forests_2024ppRef_MC_withPU/HiForestMiniAOD_1007.root
/eos/cms/store/group/phys_heavyions/nbarnett/Forests/MC/forests_2024ppRef_MC_withPU/HiForestMiniAOD_1008.root
/eos/cms/store/group/phys_heavyions/nbarnett/Forests/MC/forests_2024ppRef_MC_withPU/HiForestMiniAOD_1009.root
/eos/cms/store/group/phys_heavyions/nbarnett/Forests/MC/forests_2024ppRef_MC_withPU/HiForestMiniAOD_100.root
/eos/cms/store/group/phys_heavyions/nbarnett/Forests/MC/forests_2024ppRef_MC_withPU/HiForestMiniAOD_1010.root
/eos/cms/store/group/phys_heavyions/nbarnett/Forests/MC/forests_2024ppRef_MC_withPU/HiForestMiniAOD_1011.root
/eos/cms/store/group/phys_heavyions/nbarnett/Forests/MC/forests_2024ppRef_MC_withPU/HiForestMiniAOD_1012.root
/eos/cms/store/group/phys_heavyions/nbarnett/Forests/MC/forests_2024ppRef_MC_withPU/HiForestMiniAOD_1013.root
/eos/cms/store/group/phys_heavyions/nbarnett/Forests/MC/forests_2024ppRef_MC_withPU/HiForestMiniAOD_1014.root
```

<h3>Executable Interface</h3>

This Condor submission wrapper as written works with an executable that accepts exactly two positional arguments. This convention is enforced within `run_job.sh`, regardless of the executable file type. The executable provided will be executed in one of the following ways. Compatability of this execution with the specified executable should be tested before any jobs requests are handed off to the schedule daemon assigned to the current LXPLUS node.

```
./Executable.sh <INPUT_FILE> <OUTPUT.root>
```

```
root -l -b -q Executable.C <INPUT_FILE> <OUTPUT.root>
```

```
cmsRun Executable.py <INPUT_FILE> <OUTPUT.root>
```
<h3>Working Directory</h3>

The working directory is where the Condor submission file is made and submitted. The filelist, executable, jobname, submission file, submit generator, and runtime wrapper are all put into this working directory. The working directory is timestamped and contains everything used in the Condor submission. The name and output structure of the working directory is shown below.

```
condor_<JOBNAME>_<YEAR-MONTH-DAY_HOUR-MINUTE-SECOND>/
├── MakeCondor_<JOBNAME>.sh
├── submit_<JOBNAME>.condor
├── <EXECUTABLE>
├── <FILELIST>
├── run_job.sh
└── logs/
    ├── out/   # stdout per job
    ├── err/   # stderr per job
    └── log/   # HTCondor log per job
```

<details>

<summary><h3>Condor Commands</h3></summary>

These commands can be run in a terminal logged into LXPLUS to interact with HTCondor. These are the most common Condor commands, but there are many more.

| Command | Description |
| - | - |
| `condor_q` | Show job queue on the local schedd. |
| `condor_q -better-analyze <ID>` | Diagnose, potentially idle, job with specified ID. |
| `condor_rm <JOB_ID>` | Remove job with specified ID from the queue. |
| `condor_rm -all` | Remove all submitted jobs from the queue. |

</details>

<h3>Resources</h3>

* [CERN Batch Service Documentation](https://batchdocs.web.cern.ch/) — official guide for HTCondor on lxplus
* [Condor Commands Reference (CERN TWiki)](https://twiki.cern.ch/twiki/bin/view/CENF/NeutrinoClusterCondorDoc) - reference for commands `condor_q`, `condor_status`, `condor_rm`, and more *(requires CERN login)*
* [HTCondor Documentation](https://htcondor.org/documentation/htcondor.html) - official HTCondor webpage, manual, and user guide
* [HTCondor Source Code Repository](https://github.com/htcondor/htcondor) - open-source codebase for HTCondor, licensed under Apache 2.0

