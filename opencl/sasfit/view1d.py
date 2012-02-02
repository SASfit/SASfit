# -*- coding: utf-8 -*-
# view1d.py
# 
# Copyright (c) 2010-2012 BAM
# Federal Institute for Materials Research and Testing
# Division I.3 Structure Analysis; Polymer Analysis
# Richard-Willstaetter-Strasse 11, 12489 Berlin, Germany
# 
# This file is part of SASfit.
#
# SASfit is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# SASfit is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with SASfit.  If not, see <http://www.gnu.org/licenses/>.
# 
# Author(s) of this file:
# Ingo Bressler (sasfit@ingobressler.net)

from cutesnake import PlotViewBase

class View1D(PlotViewBase):

    def setData(self, datalist):
        for data in datalist:
            item = self.plot(x = data[:, 0], y = data[:, 1])

# vim: set ts=4 sw=4 sts=4 tw=0:

