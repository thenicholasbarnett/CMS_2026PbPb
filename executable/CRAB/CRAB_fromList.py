from CRABClient.UserUtilities import config
config = config()

config.General.requestName = '' # name for the CRAB submission
config.General.workArea = 'crab_projects'
config.General.transferOutputs = True
config.General.transferLogs = False

config.JobType.pluginName = 'Analysis'
config.JobType.pyCfgParams = ['noprint']
config.JobType.psetName = '' # name of executable
config.JobType.allowUndistributedCMSSW = True
#config.JobType.maxMemoryMB = 10000
#config.JobType.numCores = 4
#config.JobType.maxJobRuntimeMin = 720

inputList='' # list of files to submit CRAB jobs for
config.Data.userInputFiles = open(inputList).readlines()
#config.Data.lumiMask = ''
config.Data.splitting = 'FileBased'
config.Data.unitsPerJob = 1
config.Data.totalUnits = -1

config.Data.outLFNDirBase = '' # location to store output files
config.Data.publication = False

config.Site.storageSite = 'T2_CH_CERN'
