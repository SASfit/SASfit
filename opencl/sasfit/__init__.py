# -*- coding: utf-8 -*-
# __init__.py
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

import pyopencl as cl
import numpy as np
import numpy.linalg as la
import sys
import inspect
import logging
import time
from cutesnake import DataFile

def getattributes(obj):
    return [(member, getattr(obj, member)) for member in dir(obj) if
        not member.startswith("_") and
        not inspect.ismethod(getattr(obj, member))]

def printobjectinfo(obj, obj_info):
    for name, attr in getattributes(obj_info):
        print name, ":", obj.get_info(attr)

class ScatteringData1D(DataFile):
    _dataFromFile = None

    def __init__(self, filename=""):
        data = DataFile.loadFile(self, filename)
        if data is not None and len(data) > 0:
            self._dataFromFile = np.array(data, dtype='float64')

    def processFileRow(self, fields, dummy):
        if (fields is None
            or len(fields) < 2):
            return None
        col0 = float(fields[0])
        col1 = float(fields[1])
        return (col0, col1)

    def data(self):
        return self._dataFromFile

class Descriptor(object):
    _handle = None
    _info = None
    _name = None

    def __init__(self, obj):
        self._handle = obj
        self._name = self._getinfo(obj, 'NAME')

    def _getinfo(self, obj, attrname):
        return obj.get_info(getattr(self._info, attrname))

    def __str__(self):
        return "{0}".format(self._name)

    def handle(self):
        return self._handle

class Platform(Descriptor):
    _info = cl.platform_info
    _profile = None

    def __init__(self, platform):
        Descriptor.__init__(self, platform)
        self._profile = self._getinfo(platform, 'PROFILE')

    def __str__(self):
        basestr = Descriptor.__str__(self)
        return "; ".join((basestr, self._profile))

class Device(Descriptor):
    _info = cl.device_info
    _platform = None
    _type = None

    def __init__(self, device):
        Descriptor.__init__(self, device)
        self._platform = Platform(self._getinfo(device, 'PLATFORM'))
        self._type = self._getinfo(device, 'TYPE')
        self._typestr = cl.device_type.to_string(self._type)

    def iscpu(self):
        return self._type == cl.device_type.CPU

    def isgpu(self):
        return self._type == cl.device_type.GPU

    def __str__(self):
        basestr = Descriptor.__str__(self)
        return "{t}: {name} [{platform}]".format(
                                        t = self._typestr,
                                        name = basestr,
                                        platform = self._platform)

def getdevices():
    devices = []
    for p in cl.get_platforms():
        for d in p.get_devices():
            devices.append(d)
    logging.info("Detected {0} devices:".format(len(devices)))
    return [Device(d) for d in devices]

available_devices = getdevices()
if len(available_devices) <= 0:
    logging.error("No OpenCL compatible devices detected!")
    sys.exit(0)

#print("available devices: ".format(available_devices))
for d in available_devices:
    print d
selected_device = available_devices[0]

def calc(context):
    queue = cl.CommandQueue(context)
    mf = cl.mem_flags

    # copy src data from file raw data, slices throw error
    # 'TypeError: expected a single-segment buffer object'
    data = np.array(ScatteringData1D("testdata.txt").data()[:,0], dtype='float32')
    print data, data.nbytes, data.shape, data.dtype
    srcbuf = cl.Buffer(context, mf.READ_ONLY | mf.COPY_HOST_PTR, hostbuf=data)
    destbuf = cl.Buffer(context, mf.WRITE_ONLY, data.nbytes)

    # TODO: PI?, gsl_pow_3?

    kernel = cl.Program(context, """
        __kernel void sphere(__global const float *in, __global float *out)
        {
          float R = 10.0, ETA = 1e-3;
          int gid = get_global_id(0);
          out[gid] = ETA * 4.0 * 3.14 * 
                     (sin(in[gid]*R) - in[gid]*R*cos(in[gid]*R))
                     /(in[gid]*in[gid]*in[gid]);
        }
        """).build()

    kernel.sphere(queue, data.shape, None, srcbuf, destbuf)

    result = np.zeros_like(data)
    event = cl.enqueue_read_buffer(queue, destbuf, result)
    event.wait()

    print result
    print data.shape, result.shape
    return (data, result)

context = None
try:
    context = cl.Context([selected_device.handle()])
except cl.RuntimeError, e:
    print("Could not get context of device '{0}'! "
          "Is the device occupied by another (OpenGL enabled) program, "
          "for example a web browser?"
          .format(str(selected_device)))
    print(e)

from PyQt4.QtGui import QApplication
from mainwindow import MainWindow

def eventloop(argv = None):
    if argv is None:
        argv = sys.argv
    app = QApplication(argv)
    w = MainWindow()
    w.show()
    if context is not None:
        w.setData(calc(context))
    return app.exec_()

def main(argv = None):
    return sys.exit(eventloop())

# vim: set ts=4 sw=4 sts=4 tw=0:
