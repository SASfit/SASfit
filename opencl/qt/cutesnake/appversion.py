# -*- coding: utf-8 -*-
# appversion.py
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

from PyQt4.QtCore import QCoreApplication as coreApp
from utils import VersionBase

class AppVersion(VersionBase):
    """
    Set QCoreApplication properties based on version meta data.
    """
    def __init__(self, *args, **kwargs):
        VersionBase.__init__(self, *args, **kwargs)
        self._setApplicationMetaData()

    def settingsKey(self):
        """
        Version dependent settings key.
        """
        return "{0}_{1}".format(self.name(), self.number())

    def _setApplicationMetaData(self):
        for func, data in (
                (coreApp.setApplicationName, self.name()),
                (coreApp.setApplicationVersion, self.number()),
                (coreApp.setOrganizationName, self.organizationName()),
                (coreApp.setOrganizationDomain, self.organizationDomain())
                ):
            if type(data) is str:
                func(data)

# vim: set ts=4 sw=4 sts=4 tw=0:

