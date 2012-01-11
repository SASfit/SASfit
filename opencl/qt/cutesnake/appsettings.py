# -*- coding: utf-8 -*-
# appsettings.py
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

from PyQt4.QtCore import QSettings, QByteArray, QString
import logging

def settingsApplicationName(appversion):
    return "{0}_{1}".format(appversion.name(),
                            appversion.number())

class AppSettings(QSettings):
    def __init__(self, appversion):
        QSettings.__init__(QString(appversion.organizationName()),
                           QString(settingsApplicationName(appversion)))
        appversion.setApplicationMetaData()
        defaultSettings = appversion.defaultSettings()
        if (type(defaultSettings) is not dict):
            return
        custom, default = [], []
        for key, value in defaultSettings.iteritems():
            if self.contains(key):
                custom.append(key)
            else: # qsettings doesn't contain the key, add it
                self.setValue(key, QByteArray.fromBase64(value))
                default.append(key)
        logmsg = "Loaded settings."
        if len(default) > 0:
            logmsg += " Defaults for {0}.".format(", ".join(default))
        if len(custom) > 0:
            logmsg += " Previous ones for {0}.".format(", ".join(custom))
        logging.info(logmsg)

# TODO: tests?

# vim: set ts=4 sw=4 sts=4 tw=0:

