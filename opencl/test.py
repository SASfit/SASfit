import pyopencl as cl
import numpy
import numpy.linalg as la
import sys
import inspect
import logging

def getattributes(obj):
    return [(member, getattr(obj, member)) for member in dir(obj) if
        not member.startswith("_") and
        not inspect.ismethod(getattr(obj, member))]

def printobjectinfo(obj, obj_info):
    for name, attr in getattributes(obj_info):
        print name, ":", obj.get_info(attr)

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
for d in available_devices:
    print d
selected_devices = [available_devices[0].handle()]

context = cl.Context(selected_devices)
queue = cl.CommandQueue(context)

# TODO: create context with specific device(type)

sys.exit(0)

a = numpy.random.rand(50000).astype(numpy.float32)
b = numpy.random.rand(50000).astype(numpy.float32)

ctx = cl.create_some_context()
queue = cl.CommandQueue(ctx)

mf = cl.mem_flags
a_buf = cl.Buffer(ctx, mf.READ_ONLY | mf.COPY_HOST_PTR, hostbuf=a)
b_buf = cl.Buffer(ctx, mf.READ_ONLY | mf.COPY_HOST_PTR, hostbuf=b)
dest_buf = cl.Buffer(ctx, mf.WRITE_ONLY, b.nbytes)

prg = cl.Program(ctx, """
    __kernel void sum(__global const float *a,
    __global const float *b, __global float *c)
    {
      int gid = get_global_id(0);
      c[gid] = a[gid] + b[gid];
    }
    """).build()

prg.sum(queue, a.shape, None, a_buf, b_buf, dest_buf)

a_plus_b = numpy.empty_like(a)
cl.enqueue_read_buffer(queue, dest_buf, a_plus_b).wait()

print la.norm(a_plus_b - (a+b))

