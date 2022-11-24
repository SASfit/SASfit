# -*- coding: utf-8 -*-
#
# @fileoverview Copyright (c) 2019-2021, Stefano Gualandi,
#                via Ferrata, 1, I-27100, Pavia, Italy
#
#  @author stefano.gualandi@gmail.com (Stefano Gualandi)
#

import plotly.figure_factory as ff

import numpy as np
from OT1D import OT1D, parasort

np.random.seed(13)

N = 1000

Palette = ['#0d0887', '#46039f', '#7201a8', '#9c179e', '#bd3786', '#d8576b', '#ed7953', '#fb9f3a', '#fdca26', '#f0f921']

# Uniform samples
x = [i for i in np.random.normal(4, 0.5, N)]
y = [j for j in np.random.normal(1, 0.3, N//2)] + [j for j in np.random.normal(0, 0.2, N//2)]

z, T, W = OT1D(x, y, p=1, sorting=True, threads=16, plan=True)

hist_data = [x, y]
colors = [Palette[-2], Palette[0]]
group_labels = ['mu=N(4, 0.5)', 'nu = N(1, 0.3) + N(0, 0.2)']

idx = 2
for a in [1, 3, 5, 7]:
	alpha = 0.1+0.1*a
	z = []
	for e in range(len(W)):
		i, j = T[e,0], T[e,1]
		z.append(alpha*x[i] + (1 - alpha)*y[j])
	hist_data.append(z)
	colors.append(Palette[idx])
	idx += 2
	group_labels.append('Interpolate alpha={}'.format(alpha))


# Create distplot with curve_type set to 'normal'
fig = ff.create_distplot(hist_data, group_labels, show_hist=False, colors=colors)

# Add title
fig.update_layout(title_text='Interpolation of 1D Measures by Optimal Transport')
fig.show()

# PROVARE CON SLIDERS: https://plotly.com/python/sliders/

# VEDI QUESTO: https://stackoverflow.com/questions/42899023/plotting-consecutive-histograms-with-time-slider-in-plotly-python