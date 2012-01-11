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
    @classmethod
    def setApplicationMetaData(cls):
        for func, data in (
                (coreApp.setApplicationName, cls.name()),
                (coreApp.setApplicationVersion, cls.number()),
                (coreApp.setOrganizationName, cls.organizationName()),
                (coreApp.setOrganizationDomain, cls.organizationDomain())
                ):
            if type(data) is not str:
                continue
            func(data)

# vim: set ts=4 sw=4 sts=4 tw=0:

