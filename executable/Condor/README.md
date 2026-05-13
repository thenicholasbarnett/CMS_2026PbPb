<img src="../../image/HTCondor_logo.png" alt="HTCondor logo" width="700"/>

HTCondor, formerly Condor, is a distributed high throughput computing system developed at University of Wisconsin-Madison. This is essentially a specialized workload management system. Users submit jobs to a scheduler, these jobs are placed in a queue, and HTCondor chooses when and where to run the jobs. HTCondor is open source with extensive documentation. See the following resources for more useful information regarding HTCondor.
* [Source Code Repository](https://github.com/htcondor/htcondor)
* [Documentation](https://htcondor.org/documentation/htcondor.html)
* [CERN TWiki](https://twiki.cern.ch/twiki/bin/view/CENF/NeutrinoClusterCondorDoc)
<br><br>
<h3>Submitting Condor Jobs</h3>

Condor can be accessed through CMSSW, and these bash scripts can make this a smooth interaction. This setup consists of one driver, one run wrapper, and one submit generator. By specifying a working directory, with a CMSSW release, for the run wrapper you can also use these bash scripts to schedule Condor jobs with ease. 

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
