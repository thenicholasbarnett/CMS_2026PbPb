<img src="../../image/HTCondor_logo.png" alt="CMS logo" width="500"/>

<h3>Submitting Condor Jobs</h3>

> NOTE: Change cmssw_dir inside run_job.sh to desired CMSSW working area.

HTCondor or Condor is a batch job scheduler or execution system. Condor can be accessed through CMSSW, and these scripts can make this a smooth interaction. This setup consists of one driver, one run wrapper, and one submit generator. By simply changing cmssw_dir inside run_job.sh you can also use these bash scripts to schedule Condor jobs with ease. 
<br><br>
Below is a generic terminal command one can use to execute MakeCondor.sh in an lxplus terminal after running `chmod +x MakeCondor.sh`, and after making the changes specified above.

```
MakeCondor.sh JobName /path/to/executable.C(or executable.py/executable.sh) /path/to/filelist.txt /path/to/store/output/files
```

Below is a working example of how to submit some condor jobs using my files and writing to my directory.

```
MakeCondor.sh smeared_dijet_asymmetries_MC_2024ppRef /afs/cern.ch/user/n/nbarnett/public/4_6_2026_JER/smeared_asymmetry_generator_condor_2024ppRef_MC_4_9_2026.C /afs/cern.ch/user/n/nbarnett/public/txt_files/filename_txt_files/2024ppRef_MC_filenames/forests_2024ppRef_MC_withPU_10files.txt /eos/cms/store/group/phys_heavyions/nbarnett/JetCalibrations/2024/smeared_dijet_asymmetries
```
