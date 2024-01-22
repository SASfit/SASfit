# -*- coding: utf-8 -*-
"""
    PSISANS1toHMI.py is part of pySASfit package
"""
__author__ = "Joachim Kohlbrecher"
__copyright__ = "Copyright 2023, The SASfit Project"
__license__ = "GPL"
__version__ = "1.0"
__maintainer__ = "Joachim Kohlbrecher"
__email__ = "joachim.kohlbrecher@psi.ch"
__status__ = "Production"

from SASformats import SANSdata, readBerSANStrans
import os
import sys
import re
import pprint
import tkinter

def rawhdf2hmi(FullFileNameHDF, FullFileNameHMI, Ignore=None, Tfiles=None, thickness=None, rwl=''):
    if Ignore is None:
        Ignore = []
    if Tfiles is None:
        Tfiles = {}
    if thickness is None:
        thickness = {}
    Data = SANSdata(FullFileNameHDF)
    wlHDF = float(Data.BerSANS["%Setup,Lambda"])
    transmissionfile = ""
    for key, val in Tfiles.items():
        if abs(float(key)-wlHDF)/wlHDF < 0.05:
            transmissionfile = val
            print(f"Tfiles: {key}:{val}")
    if rwl != '':
        Data.BerSANS.update({"%Setup,Lambda":float(rwl)})
    transD = {}
    if os.path.isfile(transmissionfile):
        transD = readBerSANStrans(transmissionfile)
    #print(f"filename for transmission {transmissionfile}")
    if Data.BerSANS['%File,Type'] != 'SANSDRawhdf':
        raise RuntimeError('input file needs to be raw data from SANS-1.')
    BERSANS = open(FullFileNameHMI, 'w', encoding="utf-8")
    resFile = {
        key: val 
        for key, val in Data.BerSANS.items() 
        if re.search("^%File", key)
    }
    resSetup = {
        key: val 
        for key, val in Data.BerSANS.items() 
        if re.search("^%Setup", key)
    }
    resSample = {
        key: val 
        for key, val in Data.BerSANS.items() 
        if re.search("^%Sample", key)
    }
    resCounter = {
        key: val 
        for key, val in Data.BerSANS.items() 
        if re.search("^%Counter", key)
    }
    resHistory = {
        key: val 
        for key, val in Data.BerSANS.items() 
        if re.search("^%History", key)
    }
    BERSANS.write('%File\n')
    for key, val in resFile.items():
        f = key.split(",")
        if val=='SANSDRawhdf':
            val = 'SANSDRaw'
        if len(f) > 1:
            if f[1]=='FileName':
                fn=os.path.basename(FullFileNameHMI)
                f = fn.split('.')
                BERSANS.write(f'FileName={f[0]}\n')
            else:
                BERSANS.write(f'{f[1]}={val}\n')
    BERSANS.write('%Setup\n')
    for key, val in resSetup.items():
        f = key.split(",")
        if len(f) > 1:
            BERSANS.write(f'{f[1]}={val}\n')
    BERSANS.write('%Sample\n')
    for key, val in resSample.items():
        f = key.split(",")
        if len(f) > 1:
            BERSANS.write(f'{f[1]}={val}\n')
    BERSANS.write('%Counter\n')
    for key, val in resCounter.items():
        f = key.split(",")
        if len(f) > 1:
            BERSANS.write(f'{f[1]}={val}\n')
    BERSANS.write('%History\n')
    for key, val in resHistory.items():
        f = key.split(",")
        if len(f) > 1:
            BERSANS.write(f'{f[1]}={val}\n')
    if f'{Data.BerSANS["%Sample,SampleName"]}' in transD.keys():
        BERSANS.write(f'Transmission={transD[Data.BerSANS["%Sample,SampleName"]]}\n') 
    if "%History,Attenuation" not in Data.BerSANS.keys():
        BERSANS.write('Attenuation=1\n') 
    if "%History,Probability" not in Data.BerSANS.keys():
        BERSANS.write('Probability=0\n') 
    if "%History,Scaling" not in Data.BerSANS.keys():
        if Data.BerSANS["%Sample,SampleName"] in thickness.keys():
            BERSANS.write(f'Scaling={float(thickness[Data.BerSANS["%Sample,SampleName"]])/10.}\n')
        elif re.search('1mm',Data.BerSANS["%Sample,SampleName"]):
            BERSANS.write('Scaling=0.1\n')
        elif re.search('2mm',Data.BerSANS["%Sample,SampleName"]):
            BERSANS.write('Scaling=0.2\n')
        elif re.search('4mm',Data.BerSANS["%Sample,SampleName"]):
            BERSANS.write('Scaling=0.4\n')
        else:
            BERSANS.write('Scaling=0.1\n')
    BERSANS.write('%Counts\n')
    try:
        DetData = Data.BerSANS['%Counts,DetCounts']
    except Exception:
        raise RuntimeError(f'no data available')
    for j in range(int(Data.BerSANS['%File,DataSizeY'])):
        for i in range(int(Data.BerSANS['%File,DataSizeX'])//8):
            BERSANS.write(f'{DetData[j,i*8+0]},{DetData[j,i*8+1]},{DetData[j,i*8+2]},{DetData[j,i*8+3]},'+
                          f'{DetData[j,i*8+4]},{DetData[j,i*8+5]},{DetData[j,i*8+6]},{DetData[j,i*8+7]}\n')

"""
try:
    Trans_path = 'C:\\Users\\kohlbrecher\\switchdrive\\SANS\\user\\Genix\\raw\\'
    T8 = readBerSANStrans(f'{Trans_path}trans8A.txt')
    print(T8)
    Tfilenames = {0.80 : f'{Trans_path}trans8A.txt' }
    rawhdf2hmi(sys.argv[1], sys.argv[2],Tfiles=Tfilenames)
except Exception:
    print(f'Could not read {Trans_path}trans8A.txt')
rawhdf2hmi(FullFileNameHDF, FullFileNameHMI, Ignore=None, Tfiles=None, thickness=None, rwl='')
"""
