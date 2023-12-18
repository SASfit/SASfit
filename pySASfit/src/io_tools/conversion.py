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
import pprint
import tkinter
from PSISANS1toHMI import rawhdf2hmi

Data_path = 'C:/Users/kohlbrecher/switchdrive/SANS/user/Genix/raw/'
year = '2023'
from_number = 84927
to_number = 85114

SkipF = [ ]

"""
rawhdf2hmi("C:/Users/kohlbrecher/switchdrive/SANS/user/Genix/raw/sans2023n085152.hdf", "C:/Users/kohlbrecher/switchdrive/SANS/user/Genix/raw/D0085152.001", Tfiles={0.80:"C:/Users/kohlbrecher/switchdrive/SANS/user/Genix/raw/trans8A.txt"})

exec(open(f'{Data_path}thickness.py').read())
print(f'try to read {Data_path}trans8A.txt')
if not os.path.isfile(f'{Data_path}trans8A.txt'):
    raise RuntimeError(f'file {Data_path}trans8A.txt does not exists!')
transD = readBerSANStrans(f'{Data_path}trans8A.txt')
pp = pprint.PrettyPrinter(indent=4)
#pp.pprint(thicknessD)
"""
"""
for filenumber in range(to_number-from_number+1):
    HDF_filename = f"sans{year}n%06d"%(filenumber+from_number)
    HMI_filename = "D%07d"%(filenumber+from_number)+'.001'
    try:
        lambdaoverwrite=''
#        if filenumber+from_number >= 54360 and filenumber+from_number <= 54383:
#            lambdaoverwrite = '1.3026'
        rawhdf2hmi(
            Data_path + HDF_filename + '.hdf',
            Data_path + HMI_filename + '.001',
            Tfiles = {0.50:f'{Data_path}transmission.dat', 1.00:f"{Data_path}trans1p0.dat"} ,
            thickness=thicknessD,
            rwl=lambdaoverwrite
        )
    except Exception:
        print(f'ignoring file number {filenumber+from_number}')
        continue
"""


Tfiles_tcl = {}
gui = tkinter.Tk()

def rawhdf2hmi_tcl (FullFileNameHDF, FullFileNameHMI):
    global Tfiles_tcl
    #rawhdf2hmi(FullFileNameHDF, FullFileNameHMI Tfiles=None, thickness=None, rwl='')
    rawhdf2hmi(FullFileNameHDF, FullFileNameHMI, Tfiles=Tfiles_tcl)

def adddict_Tfiles(*args):
    global Tfiles_tcl
    #print(len(args))
    if len(args) >=2:
        Tfiles_tcl.update({args[0]:args[1]})
    if len(args)>=4:
        Tfiles_tcl.update({args[2]:args[3]})
    if len(args)>=6:
        Tfiles_tcl.update({args[4]:args[5]})
        

# register it as a tcl command:
tcl_command_name = "adddict_Tfiles"
python_function = adddict_Tfiles
cmd = gui.createcommand(tcl_command_name, python_function)


def register(my_python_function, tcl_cmd_name=None):
    if tcl_cmd_name is None:
        tcl_cmd_name = my_python_function.__name__
    tcl_cmd = gui.register(my_python_function)
    gui.eval('catch {rename ' + tcl_cmd_name + ' ""}')
    gui.call('rename', tcl_cmd, tcl_cmd_name)
    return tcl_cmd_name

# Python variable access function definitions
def set_pyvar(var_name, var_value):
         globals()[var_name] = var_value

def get_pyvar(var_name):
    return globals()[var_name]

# Register the access functions
register(set_pyvar)
register(get_pyvar)
register(rawhdf2hmi_tcl)

def disable_event():
   pass
gui.tk.eval(open(f'{os.path.dirname(__file__)}{os.sep}PSI2HMI.tcl','r').read())
#Disable the Close Window Control Icon
#gui.protocol("WM_DELETE_WINDOW", disable_event)
gui.mainloop()
