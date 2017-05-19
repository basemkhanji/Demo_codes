#!/usr/bin/env python
import sys
import os
import subprocess, multiprocessing 
from os.path import isfile
# input:
grid_dir_prefix = 'root://eoslhcb.cern.ch//eos/lhcb/grid/user'  #  /eos/lhcb/grid/user/lhcb/user/b/bkhanji/
#eos_ana_dir     = 'root://eoslhcb.cern.ch//eos/lhcb/grid/user/lhcb/user/b/bkhanji/'
eos_ana_dir     = '/afs/cern.ch/work/b/bkhanji/public/'
#eos_ana_dir     = 'root://eoslhcb.cern.ch//eos/lhcb/wg/semileptonic/Bs2KmunuAna/Tuples/Bs2KMuNu_DATATUPLES_RAW_19June16/'
#parallelization:
#pool = multiprocessing.Pool(4)
# command:

def IsAtCERN(Sjobs):
    dirac_file = Sjobs.outputfiles.get(DiracFile)[0]
    #os.system('eos cp ' + in_file_and_out_file)
    return dirac_file.locations[0] == 'CERN-USER'

def IsCompleted(Sjobs):
    return Sjobs.status == 'completed'

list_of_CERN_jobs = []
i = 0
for j in jobs:
    print 'Looking at Job :', j.id
    for sj in j.subjobs:
        print 'looking at subjob: ' , sj.id
        i+=1
        if( IsCompleted(sj) ):
            MyLfn      = sj.outputfiles.get(DiracFile)[0].lfn
            input_f    = grid_dir_prefix + MyLfn
            output_f   = eos_ana_dir + str(i) + '_'
            Name_ptrn  = sj.outputfiles.get(DiracFile)[0].namePattern 
            in_and_out = input_f + ' ' + output_f + Name_ptrn
            if (IsAtCERN(sj)) :
                print 'subjob is at CERN'
                print in_and_out
                #list_of_CERN_jobs.append(in_and_out) 
                os.system('xrdcp ' + in_and_out)
            else :
                print 'Subjob ', sj.id , 'is not at CERN'
                print 'Get its URL from dirac ... '
                
                print 'Copying it to eos ...'
                os.system('xrdcp ./' + Name_ptrn + " " + output_f + Name_ptrn)
                os.system('rm -rf ' + Name_ptrn )
                
                            
                
        
        


