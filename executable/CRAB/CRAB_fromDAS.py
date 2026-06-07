#!/usr/bin/env python3
from CRABClient.UserUtilities import config

DO_DRY_RUN = True  # set to True to test configuration before submission
cfg = config()

cfg.General.workArea = '' # name of directory to store CRAB submission info
cfg.General.transferOutputs = True
cfg.General.transferLogs = False

cfg.JobType.pluginName = 'Analysis'
cfg.JobType.psetName = '' # name of executable

# common adjustable parameters if needed
#cfg.JobType.maxMemoryMB = 10000
#cfg.JobType.numCores = 4
#cfg.JobType.maxJobRuntimeMin = 720

cfg.Data.inputDBS = 'global'
cfg.Data.splitting = 'LumiBased' # job splitting (most granular possible here)
cfg.Data.unitsPerJob = 1
cfg.Data.runRange = '' # runs to process if desired (e.g. 123456-123456)

cfg.Data.lumiMask = '' # JSON file with certified luminosity sections (e.g. from DCS)
cfg.Data.publication = False

cfg.Data.outLFNDirBase = '' # directory to store output files
cfg.Site.storageSite = 'T2_CH_CERN' 

dataset_indices = [1, 2] # list of primary dataset indicies to process (e.g. for /HIPhysicsRawPrime1, /HIPhysicsRawPrime2, etc.)

if __name__ == '__main__':
    from CRABAPI.RawCommand import crabCommand    
    if cfg.Data.lumiMask:
        print(f"Using JSON mask: {cfg.Data.lumiMask}")
    if DO_DRY_RUN:
        print("\n" + "="*50)
        print("  WARNING: DOING DRY RUN ONLY. NO JOBS WILL BE SUBMITTED.")
        print("="*50 + "\n")
    
    for i in dataset_indices:
        cfg.Data.inputDataset = f'/HIPhysicsRawPrime{i}/HIRun2026A-PromptReco-v1/MINIAOD'
        if cfg.Data.runRange:
            cfg.General.requestName = f'HiForest_RawPrime{i}_Run{cfg.Data.runRange}'
        else:
            cfg.General.requestName = f'HiForest_RawPrime{i}'

        print(f"---> Processing dataset: {cfg.Data.inputDataset}")
        try:
            # The dryrun parameter handles the safety check
            crabCommand('submit', config=cfg, dryrun=DO_DRY_RUN)
            
            if DO_DRY_RUN:
                print(f"     [DRY RUN SUCCESS] Passed checks for stream {i}.\n")
                
        except Exception as e:
            print(f"     [ERROR] Failed to process stream {i}: {e}\n")
