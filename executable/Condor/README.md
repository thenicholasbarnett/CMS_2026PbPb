<img src="../../image/HTCondor_logo.png" alt="HTCondor logo" width="700"/>

HTCondor, formerly Condor, is a distributed high throughput computing system developed at University of Wisconsin-Madison. This is essentially a specialized workload management system. Users submit jobs to a scheduler, these jobs are placed in a queue, and HTCondor chooses when and where to run the jobs. HTCondor is open source with extensive documentation. Please see additional useful information regarding HTCondor in the resources section.
<br><br>
<h1>HTCondor Job Submission</h1>

Condor can be accessed through CMSSW, and these bash scripts can make this a smooth interaction. By specifying a working directory, with a CMSSW release, you can also use these bash scripts to schedule Condor jobs with ease.

<h2>Overview</h2>

This job submission setup consists of three bash scripts, each fulfilling a different role.
<br>

| Script | Role | Description |
|---|---|---|
| `MakeCondor.sh` | Driver | Entry point called by the user. Creates a timestamped working directory and delegates to the template. |
| `MakeCondor_template.sh` | Submit Generator | Generates the HTCondor submission file using a template, then submits the jobs. |
| `run_job.sh` | Run Wrapper | Executed on the worker node. Sets up CMSSW runtime environment and runs the user-provided executable based on its type (`.py`, `.C`, or `.sh`). |

<br>

> NOTE: Change CMSSW_src on line 17 of run_job.sh to desired CMSSW working area.

<br>

Below is a generic terminal command one can use to execute MakeCondor.sh in an lxplus terminal after changing CMSSW_src inside run_job.sh, and  running `chmod +x MakeCondor.sh`.

```
MakeCondor.sh JobName /path/to/executable.C(or executable.py/executable.sh) /path/to/filelist.txt /path/to/store/output/files
```

<br>

Below is a working example of how to submit some condor jobs using my files and writing to the currently staged directory.

```
MakeCondor.sh smeared_dijet_asymmetries_MC_2024ppRef /afs/cern.ch/user/n/nbarnett/public/4_6_2026_JER/smeared_asymmetry_generator_condor_2024ppRef_MC_4_9_2026.C /afs/cern.ch/user/n/nbarnett/public/txt_files/filename_txt_files/2024ppRef_MC_filenames/forests_2024ppRef_MC_withPU_10files.txt .
```

<h3>Resources</h3>

* [CERN Batch Service Documentation](https://batchdocs.web.cern.ch/) — official guide for HTCondor on lxplus
* [Condor Commands Reference (CERN TWiki)](https://twiki.cern.ch/twiki/bin/view/CENF/NeutrinoClusterCondorDoc) - reference for commands `condor_q`, `condor_status`, `condor_rm`, and more *(requires CERN login)*
* [HTCondor Documentation](https://htcondor.org/documentation/htcondor.html) - official HTCondor webpage, manual, and user guide
* [HTCondor Source Code Repository](https://github.com/htcondor/htcondor) - open-source codebase for HTCondor, licensed under Apache 2.0

