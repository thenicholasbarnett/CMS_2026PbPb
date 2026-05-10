from CRABClient.UserUtilities import config
config = config()

config.General.requestName = '' # name for the CRAB submission
config.General.workArea = 'crab_projects'
config.General.transferOutputs = True
config.General.transferLogs = False

config.JobType.pluginName = 'Analysis'
config.JobType.psetName = '' # name of executable
config.JobType.outputFiles = ['HiForestMiniAOD.root']
#config.JobType.maxMemoryMB = 10000
#config.JobType.numCores = 4
#config.JobType.maxJobRuntimeMin = 720

config.Data.inputDataset = '' # path of Primary Dataset
#config.Data.lumiMask = ''
config.Data.inputDBS = 'https://cmsweb.cern.ch/dbs/prod/global/DBSReader' # change prod to phys03 for a privte Primary Dataset
config.Data.splitting = 'LumiBased'
config.Data.unitsPerJob = 1

config.Data.outLFNDirBase = '' # location to store output files
config.Data.publication = False

config.Site.storageSite = 'T2_CH_CERN'
