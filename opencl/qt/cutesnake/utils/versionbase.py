# -*- coding: utf-8 -*-
# versionbase.py
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

class VersionBase(object):
    """
    Stores version meta data.
    """
    _name = None
    _number = None
    _organizationName = None
    _organizationDomain = None
    _defaultSettings = None

    def __init__(self, programName = None,
                       versionNumber = None,
                       organizationName = None,
                       organizationDomain = None,
                       defaultSettings = None):
        assert type(programName) is str
        assert len(programName) > 0
        self._name = programName
        assert type(versionNumber) is str
        assert len(versionNumber) > 0
        self._number = versionNumber
        if organizationName is not None:
            assert type(organizationName) is str
            assert len(organizationName) > 0
            self._organizationName = organizationName
        if organizationDomain is not None:
            assert type(organizationDomain) is str
            assert len(organizationDomain) > 0
            self._organizationDomain = organizationDomain
        if defaultSettings is not None:
            assert type(defaultSettings) is dict
            assert len(defaultSettings) > 0
            self._defaultSettings = defaultSettings

    def name(self):
        return self._name

    def number(self):
        # pywin32 has a problem with version nr like "20111110"
        return self._number

    def organizationName(self):
        return self._organizationName

    def organizationDomain(self):
        return self._organizationDomain

    def defaultSettings(self):
        return self._defaultSettings

    @classmethod
    def isValid(cls, other):
        return issubclass(type(other), cls)

# vim: set ts=4 sw=4 sts=4 tw=0:

