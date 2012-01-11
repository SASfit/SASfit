# -*- coding: utf-8 -*-
# mainwindow.py
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

from ui_mainwindow import Ui_MainWindow
from cutesnake import MainWindowBase
from cutesnake import AppVersion

class TestVersion(AppVersion):
    _name = "MassChrom2D"
    _number = "2.0.1"
    _organizationName = "BAM"
    _defaultSettings = dict(
        geometry = r"AdnQywABAAAAAAIKAAAAGAAABh8AAANfAAACEAAAADUAAAYZAAADWQAAAAAAAA==",
        windowState = r"AAAA/wAAAAD9AAAAAgAAAAEAAAMRAAACNfwCAAAAAfsAAAAUAE0AYQB0AHIAaQB4AEQAbwBjAGsBAAAAAAAAAjUAAABlAP///wAAAAMAAAQKAAAAzPwBAAAAAfwAAAAAAAAECgAAAHcA////+gAAAAEBAAAAA/sAAAAOAEwAbwBnAEQAbwBjAGsBAAAAAP////8AAABOAP////sAAAAYAFMAcABlAGMAdAByAHUAbQBEAG8AYwBrAQAAAAD/////AAAAXgD////7AAAAEgBDAGgAcgBvAG0ARABvAGMAawEAAAAA/////wAAAF4A////AAAA8wAAAjUAAAAEAAAABAAAAAgAAAAI/AAAAAA="
    )

class MainWindow(MainWindowBase, Ui_MainWindow):
    def __init__(self, parent = None):
        MainWindowBase.__init__(self, TestVersion, parent)
        self.setupUi(self)

# vim: set ts=4 sw=4 sts=4 tw=0:

