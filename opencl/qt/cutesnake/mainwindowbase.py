# -*- coding: utf-8 -*-
# mainwindowbase.py
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

import logging
from PyQt4.QtGui import QMainWindow
from PyQt4.QtCore import QSettings, QByteArray, QString
from appversion import AppVersion

class MainWindowBase(QMainWindow):
    _appversion = None # AppVersion
    _appsettings = None # AppSettings

    def __init__(self, appversion, parent = None):
        QMainWindow.__init__(self, parent)
        assert AppVersion.isValid(appversion), "Please provide a valid AppVersion."
        self._appversion = appversion
        self._appsettings = self._restoreSettings()

    def closeEvent(self, event):
        self._storeSettings()
        QMainWindow.closeEvent(self, event)

    def _storeSettings(self):
        if self._appsettings is None:
            return
        geometry = self.saveGeometry()
        windowState = self.saveState()
        #print geometry.toBase64()
        #print windowState.toBase64()
        self._appsettings.setValue("geometry", geometry)
        self._appsettings.setValue("windowState", windowState)

    def _restoreSettings(self):
        """
        Load defaults for settings if missing and available.
        """
        settings = QSettings(QString(self._appversion.organizationName()),
                             QString(self._appversion.settingsKey()))
        defaultSettings = self._appversion.defaultSettings()
        if defaultSettings is None:
            return
        custom, default = [], []
        for key, value in defaultSettings.iteritems():
            if settings.contains(key):
                custom.append(key)
            else: # qsettings doesn't contain the key, add it
                settings.setValue(key, QByteArray.fromBase64(value))
                default.append(key)
        logmsg = "Loaded settings."
        if len(default) > 0:
            logmsg += " Defaults for {0}.".format(", ".join(default))
        if len(custom) > 0:
            logmsg += " Previous ones for {0}.".format(", ".join(custom))
        logging.info(logmsg)
        self.restoreGeometry(settings.value('geometry').toByteArray())
        self.restoreState(settings.value('windowState').toByteArray())
        return settings


# TODO: tests?

# vim: set ts=4 sw=4 sts=4 tw=0:

