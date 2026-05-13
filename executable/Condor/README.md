<img src="../../image/HTCondor_logo.png" alt="HTCondor logo" width="700"/>

HTCondor, formerly Condor, is a distributed high throughput computing system developed at University of Wisconsin-Madison. This is essentially a specialized workload management system. Users submit jobs to a scheduler, these jobs are placed in a queue, and HTCondor chooses when and where to run the jobs. HTCondor is open source with extensive documentation. Please see additional useful information regarding HTCondor in the resources section.
<br><br>
<h1>HTCondor Job Submission</h1>

Condor can be accessed through CMSSW, and these bash scripts can make this a smooth interaction. By specifying a working directory, with a CMSSW release, you can also use these bash scripts to schedule Condor jobs with ease. Here is a set of three bash scripts acting as a Condor submission wrapper.

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
| `OUTPUT_DIR` | Directory to store output files. A timestamped output subdirectory is made here. |

<details>
<summary><h3>Working Directory</h3></summary>

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

</details>

<h2>Example</h2>

> Don't forget to run `chmod +x MakeCondor.sh` before executing driver

<h3>Resources</h3>

* [CERN Batch Service Documentation](https://batchdocs.web.cern.ch/) — official guide for HTCondor on lxplus
* [Condor Commands Reference (CERN TWiki)](https://twiki.cern.ch/twiki/bin/view/CENF/NeutrinoClusterCondorDoc) - reference for commands `condor_q`, `condor_status`, `condor_rm`, and more *(requires CERN login)*
* [HTCondor Documentation](https://htcondor.org/documentation/htcondor.html) - official HTCondor webpage, manual, and user guide
* [HTCondor Source Code Repository](https://github.com/htcondor/htcondor) - open-source codebase for HTCondor, licensed under Apache 2.0

