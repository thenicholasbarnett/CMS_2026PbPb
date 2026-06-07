#!/usr/bin/env python3
from CRABClient.UserUtilities import config

DO_DRY_RUN = True  # set to True to test configuration before submission
cfg = config()

cfg.General.requestName = '' # name for the CRAB submission
cfg.General.workArea = '' # name of directory to store CRAB submission info
cfg.General.transferOutputs = True
cfg.General.transferLogs = False

cfg.JobType.pluginName = 'Analysis'
cfg.JobType.pyCfgParams = ['noprint']
cfg.JobType.psetName = '' # name of executable
cfg.JobType.allowUndistributedCMSSW = True
cfg.JobType.outputFiles = [''] # name of output file (from executable)

# common adjustable parameters if needed
#cfg.JobType.maxMemoryMB = 10000
#cfg.JobType.numCores = 4
#cfg.JobType.maxJobRuntimeMin = 720

inputList='' # plain text file listing input files for executable, one input file per line
cfg.Data.userInputFiles = open(inputList).readlines()
cfg.Data.splitting = 'FileBased'
cfg.Data.unitsPerJob = 1
cfg.Data.totalUnits = -1

cfg.Data.outLFNDirBase = '' # directory to store output files
cfg.Data.publication = False

cfg.Site.storageSite = 'T2_CH_CERN'
cfg.Site.whitelist = ['T2_CH_CERN']

if __name__ == '__main__':
    from CRABAPI.RawCommand import crabCommand
    if DO_DRY_RUN:
        print("\n" + "="*50)
        print("  WARNING: DOING DRY RUN ONLY. NO JOBS WILL BE SUBMITTED.")
        print("="*50 + "\n")

    print(f"---> Processing input list: {inputList}")
    try:
        crabCommand('submit', config=cfg, dryrun=DO_DRY_RUN)
        if DO_DRY_RUN:
            print("     [DRY RUN SUCCESS] Passed checks.\n")
    except Exception as e:
        print(f"     [ERROR] Failed to submit: {e}\n")