# -*- coding: utf-8 -*-
"""
    conversion.py is part of pySASfit package
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
#from pySASfit.src.io_tools.SASformats import SANSdata


def rawhdf2hmi(FullFileNameHDF, FullFileNameHMI, Ignore=[],transmissionfile='',wl=''):
    Data = SANSdata(FullFileNameHDF)
    if wl != '':
        Data.BerSANS.update({"%Setup,Lambda":float(wl)})
    transD = {}
    if os.path.isfile(transmissionfile):
        transD = readBerSANStrans(transmissionfile)
    if Data.BerSANS['%File,Type'] != 'SANSDRawhdf':
        raise RuntimeError('input file needs to be raw data from SANS-1.')
    BERSANS = open(Data_path+ HMI_filename, 'w', encoding="utf-8")
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
    if Data.BerSANS["%Sample,SampleName"] in transD.keys():
        BERSANS.write(f'Transmission={transD[Data.BerSANS["%Sample,SampleName"]]}\n') 
    if "%History,Attenuation" not in Data.BerSANS.keys():
        BERSANS.write('Attenuation=1\n') 
    if "%History,Probability" not in Data.BerSANS.keys():
        BERSANS.write('Probability=0\n') 
    BERSANS.write('%Counts\n')
    try:
        DetData = Data.BerSANS['%Counts,DetCounts']
    except Exception:
       print("No DetData")
    for j in range(int(Data.BerSANS['%File,DataSizeY'])):
        for i in range(int(Data.BerSANS['%File,DataSizeX'])//8):
            BERSANS.write(f'{DetData[j,i*8+0]},{DetData[j,i*8+1]},{DetData[j,i*8+2]},{DetData[j,i*8+3]},'+
                          f'{DetData[j,i*8+4]},{DetData[j,i*8+5]},{DetData[j,i*8+6]},{DetData[j,i*8+7]}\n')
            

Data_path = 'C:\\Users\\kohlbrecher\\switchdrive\\SANS\\user\\Kenneth\\'
year = '2023'
from_number = 54263
to_number = 54359

Ignore = []

for filenumber in range(to_number-from_number+1):
    HDF_filename = f"sans{year}n%06d"%(filenumber+from_number)
    HMI_filename = "D%07d"%(filenumber+from_number)+'.001'
    try:
        lambdaoverwrite=''
        if filenumber+from_number >= 54360 and filenumber+from_number <= 54383:
            lambdaoverwrite = '1.3026'
        rawhdf2hmi(
            Data_path + HDF_filename + '.hdf',
            Data_path + HMI_filename + '.001',
            transmissionfile = f'{Data_path}transmissions.dat',
            wl=lambdaoverwrite
        )
    except Exception:
        print(f'ignoring file number {filenumber+from_number}')
        continue
