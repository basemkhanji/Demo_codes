from ROOT import *
import sys
import os
from array import array
import multiprocessing as mp
import time
from multiprocessing.pool import ThreadPool
gROOT.Reset()

#--------------------------------------------------------------
def RunMyJobs( y , c  ):
    print 'Hi , here is the suffix ', y , ' '  , c 
    os.system('root -l "MakeTrainingTuple.cc+(\\"'+y+'\\",\\"'+c +'\\" )"')
    #os.system('root -l "Add_BDTToTuple.cc+(\\"'+y+'\\",\\"'+c +'\\" )"')
    return 0
#--------------------------------------------------------------

#--------------------------------------------------------------
def RunMyJobs_wrap(args):
      return RunMyJobs(*args)
#--------------------------------------------------------------

#=================================================================================
# run 
start_t     =   time.time()
Year        = [ "2011","2012" ,"2015","2016" ]
Channel     = [ "Bd2JpsiKstar","Bu2JpsiK"    ]
#Suffix_year  = [ y + '*'+ c for y in Year for c in Channel  ]
Suffix_year = [ ( y , c ) for y in Year for c in Channel ]

pool   =  mp.Pool( processes = 8 )
result =  pool.map( RunMyJobs_wrap  , Suffix_year )

finish_t = time.time()
print '==============================================='
print "time consumed : " ,  (finish_t - start_t)/60. , ' minutes'  
print '==============================================='
#=================================================================================

