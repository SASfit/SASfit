# -*- coding: utf-8 -*-
#
# @fileoverview Copyright (c) 2019-2021, Stefano Gualandi,
#                via Ferrata, 1, I-27100, Pavia, Italy
#
#  @author stefano.gualandi@gmail.com (Stefano Gualandi)
#

import numpy as np
from OT1D import OT1D, parasort

np.random.seed(13)

N = 10

# Uniform samples
x = [i for i in np.random.uniform(1, 2, N)]
y = [j for j in np.random.uniform(0, 1, N)]
a = [1/N for i in np.random.uniform(0, 1, N)]
b = [1/N for j in np.random.uniform(0, 1, N)]

z, T, W = OT1D(x, y, p=1, sorting=True, threads=16, plan=True)

print('Wasserstein distance of order 2, W2(x,y) =', z)

for e in range(len(W)):
	i, j = T[e,0], T[e,1]
	print('{} -> {} = {:.4f} | {:.4f} {:.4f}'.format(i, j, W[i], x[i], y[j]))


z, T, W = OT1D(x, y, a, b, p=1, sorting=True, threads=16, plan=True)

print('Wasserstein distance of order 2, W2(x,y) =', z)

for e in range(len(W)):
	i, j = T[e,0], T[e,1]
	print('{} -> {} = {:.4f} | {:.4f} {:.4f}'.format(i, j, W[i], x[i], y[j]))

# Output:
# Wasserstein distance of order 2, W2(x,y) = 1.0002549459050794