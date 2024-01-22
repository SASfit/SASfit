import numpy as np
import matplotlib as mpl
import matplotlib.pyplot as plt
import codecs
import os

def read_SANS1_eff_map():
    with codecs.open('../data/eff_map_SANS1_PSI.dat', encoding='utf-8') as f:
        X = np.loadtxt(f)
    return X

#print(read_SANS1_eff_map())
effMapSANS1 = read_SANS1_eff_map()
#plt.imshow(np.clip(effMapSANS1,0.5,1.3), interpolation='none',origin='lower')
#plt.colorbar()
#plt.show()

f=open("../tools/D0000001.999","w")
f.write("%Counts\n")
ii = 0
for j in range(128):
    for ir in range(16):
        for i in range(8):
            f.write(f"{effMapSANS1[ir*8+i][j]:+1.3e}")
        ii = ii+8
        f.write("\n")
f.write("%Errors\n")
ii = 0
for j in range(128):
    for ir in range(16):
        for i in range(8):
            f.write(f"{0.01:+1.3e}")
        ii = ii+8
        f.write("\n")
f.close()
