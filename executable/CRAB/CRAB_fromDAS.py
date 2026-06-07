from CRABAPI.RawCommand import crabCommand
from CRABClient.ClientExceptions import ClientException
from http.client import HTTPException

from CRABClient.UserUtilities import config
config = config()

config.General.workArea = 'crab_projects'
config.General.transferOutputs = True
config.General.transferLogs = False

config.JobType.pluginName = 'Analysis'
config.JobType.psetName = '' # name of executable
config.JobType.maxMemoryMB = 3000
#config.JobType.numCores = 4
config.JobType.allowUndistributedCMSSW = True
#config.JobType.maxJobRuntimeMin = 2000

config.Data.inputDBS = 'https://cmsweb.cern.ch/dbs/prod/global/DBSReader' # change prod to phys03 for a privte Primary Dataset
#config.Data.lumiMask = ''
#config.Data.totalUnits = -1
config.Data.splitting = 'LumiBased'
config.Data.unitsPerJob = 5
config.Data.allowNonValidInputDataset = True
config.Data.publication = False

config.Site.storageSite = 'T2_CH_CERN'
#config.Site.whitelist = ['T2_US_*']

# Multi crab part

def submit(config):
    try:
        crabCommand('submit', config = config, dryrun=False)
    except HTTPException as hte:
        print('Failed submitting task: %s' % (hte.headers))
    except ClientException as cle:
        print('Failed submitting task: %s' % (cle))

config.Data.outLFNDirBase = '' # location to store output files

for i in range(5):

    config.General.requestName = f'name_of_request_{i}'
    #config.Data.inputDataset = f'/PPRefHardProbes{i}/Run2024J-PromptReco-v1/MINIAOD'
    config.Data.inputDataset = f'/{i}//' # names of PDs with iterator i, example above
    config.Data.outputDatasetTag = config.General.requestName

    print('Submitting CRAB job for: '+ config.Data.inputDataset)
    submit(config)
