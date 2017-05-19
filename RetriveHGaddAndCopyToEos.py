#!/usr/bin/env python
import sys
import os
from os import listdir
from os.path import isfile, join
import multiprocessing as mp

MyLocalPath = '/media/a060d16c-0c22-4e53-a512-f435c75ed23a/bkhanji/gangadir/workspace/bkhanji/LocalXML/'
MyDir       = './'
os.system('cd ' + MyLocalPath )
#jobs_location_Ok = {} # dictionary of Job name and a list of its LFNs
jobs_subjobs_NotOk = {} # dictionary of Job name and a list of its LFNs
##################################################################
# process on 4 threads :
pool = mp.Pool(processes=4)

def RetriveLFN( sj ):
    print 'SubJobs id is : ' , sj.id, 'SubJobs status is : ' , str(sj.status)
    MyDir = str(j.id)  + '/' + str(sj.id) + '/'
    os.system('mkdir -p '+ MyLocalPath  + MyDir  )
    MyLfn = sj.outputfiles.get(DiracFile)[0].lfn
    print 'LFN is :' , MyLfn
    if (MyLfn == ''):
        print 'This subjob : ' , sj.id , 'has no LFN,  save the id and try later'
        List_of_SubJobs_NotOK.append(sj.id)
    else:
        List_of_lfns.append(MyLfn)
        os.system('dirac-dms-get-file ' + str(MyLfn) + ' -D ' + MyLocalPath  + MyDir )
        
    return 0

for j in jobs:
    print 'Looking at Job :', j.id
    List_of_lfns = []
    List_of_SubJobs_NotOK = []
    #result = [pool.apply_async(RetriveLFN, args = (sj)) for sj in j.subjobs]
    for sj in j.subjobs:
        RetriveLFN(sj)
    jobs_subjobs_NotOk[j.id] = List_of_SubJobs_NotOK

# Write in a file the LFN locations , write also the Jobs/Subjobs IDs with no LFNs
os.system(MyLocalPath)
f_lfns = open(MyLocalPath + 'RootFiles_LFNs.txt' , 'wa')
for item_lfn in List_of_lfns:
    f_lfns.write(item_lfn + '\n')
f_lfns.close()

f_jids = open(MyLocalPath + 'JosId_NotOK.txt' , 'wa')
for Job_id_Sjob_idlist in jobs_subjobs_NotOk.items():
    f_jids.write( Job_id_Sjob_idlist + '\n')
f_jids.close()


print '..........................................................................'
print '..........................................................................'
print '.................  I finished retriving the jobs .........................'
print '.......................... Start Merging .................................'
print '..........................................................................'
print '..........................................................................'

# call now the Merging script :
os.system('MrgTpls ' + ' ./ '  + ' /media/a060d16c-0c22-4e53-a512-f435c75ed23a/bkhanji/Tuples/ ')

    

"""
Dir_input = sys.argv[1] #'/tmp/bkhanji/'
dir_name_eos_target = sys.argv[2]
#'root://eoslhcb.cern.ch//eos/lhcb/wg/semileptonic/Bs2KmunuAna/Tuples/Bs2KmuNu_Tuples_14Apr/' 
# get all the names of the output files in ganga dir

import os
onlyfiles = [f for f in listdir(Dir_input) if isfile(join( Dir_input , f ) ) ]

for files_name in onlyfiles:
    os.system("xrdcp " + Dir_input  + str(files_name) +  " " + dir_name_eos_target  +str(files_name) )
    
"""
