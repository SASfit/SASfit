# -*- coding: utf-8 -*-
# qdict.py
# 
# Copyright (c) 2010-2012 BAM
# Federal Institute for Materials Research and Testing
# Division I.3 Structure Analysis; Polymer Analysis
# Richard-Willstaetter-Strasse 11, 12489 Berlin, Germany
# 
# This file is part of cutesnake.
#
# cutesnake is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# cutesnake is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with cutesnake.  If not, see <http://www.gnu.org/licenses/>.
# 
# Author(s) of this file:
# Ingo Bressler (cutesnake@ingobressler.net)

class QDict(dict):
    """
    Python dictionary with an interface compatible to QSettings.

    Created to assist testing Qt programs.
    """
    _wrapperClass = None

    def __init__(self, *args, **kwargs):
        self._wrapperClass = kwargs.pop('wrapper', None)
        dict.__init__(self, *args, **kwargs)

    def contains(self, key):
        return key in self

    def value(self, key):
        if self._wrapperClass is not None:
            return self.get(key, self._wrapperClass())
        else:
            return self.get(key, None)

    def setValue(self, key, value):
        if self._wrapperClass is not None:
            self[key] = self._wrapperClass(value)
        else:
            self[key] = value

# vim: set ts=4 sw=4 sts=4 tw=0: 
