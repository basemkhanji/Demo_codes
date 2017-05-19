#!/usr/bin/env python
import sys
import os
import multiprocessing as mp
import time
# Make sure Root is defined

Dir_input = str(sys.argv[1])
#Dir_output =  '/afs/cern.ch/work/b/bkhanji/public/Toys_Dmd_results'
#eos_dir = 'root://eoslhcb.cern.ch//eos/lhcb/wg/semileptonic/Bs2KmunuAna/Tuples/Bs2KMuNu_DATATUPLES_RAW_19June16/'
eos_dir = str(sys.argv[2])
# get all the names of the output files in ganga dir
list_ofnames = []

for root, dirs, files in os.walk(Dir_input):#os.walk("./"):
    for file in files:
        if file.endswith(".root"):
            if file not in list_ofnames: 
                print '============================='
                print 'new file name is found', file , 'Appending ... '
                print '============================='
                list_ofnames.append(file)
                print list_ofnames               

print 'Following is the list of files found in the directory:'
print list_ofnames
print 'appending now :'


def AppendRootFiles( s ):
    os.system("hadd -f " + eos_dir  + "/" + str(s) + " " + Dir_input+"/*/*/output/"+str(s))
    return 0

start = time.time()
#=================================================================================
pool   =  mp.Pool(processes=6)
result = [pool.apply_async( AppendRootFiles  , args = (s,)) for s in list_ofnames]
pool.close()
pool.join()

# for some reason xrdcp is not running in a pool 
"""
for s in list_ofnames:
    os.system("xrdcp " + "./" + str(s) + " " + eos_dir )
    os.system("rm -rf " + "./" + str(s) )
"""
#=================================================================================
end = time.time()
print 'Process took : ', float(end - start)/60. , ' minutes'


"""
i = 0
f = open("List_of_filenames.txt", "wa")
f.write('Here is a list of All the tuples:  \n')
f.write('===================================\n')
f.write(' \n')
for s in list_ofnames:
    
    i += 1
    f.write ( str(i)+ '- ' + str(s))
    f.write( ' \n')

f.write('-----------------------------------\n')
f.write(' End of the list')
f.write('===================================\n')    
"""
