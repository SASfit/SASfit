# test_inttypes.py -- Test MetaKit Python bindings for integral types
# $Id: test_inttypes.py,v 1.1 2003/01/10 21:58:07 wcvs Exp $
# This is part of MetaKit, see http://www.equi4.com/metakit/

from test.test_support import TestFailed, verbose
import os
import sys

# Keep track of failures as they happen, but don't die on the first
# one unless it's unrecoverable.  If failure_count > 0 when script
# finishes, raise TestFailed.
failure_count = 0

def fail(op, args, err=None, expected=None, actual=None):
    global failure_count
    print 'FAIL:', op, args
    print '     ',
    if err is not None: print err,
    if actual is not None: print 'got', actual, actual.__class__,
    if expected is not None: print 'expected', expected,
    print
    failure_count = failure_count + 1

# for overflow testing
MAXINT = sys.maxint
MININT = -MAXINT - 1
MAXLONGLONG = 2**63 - 1
MINLONGLONG = -2**63
MAXULONGLONG = 2**64

# Make sure we're using modules from the builds directory, assuming
# that's the current directory at the time we're run.
sys.path.insert(0, os.getcwd())

import metakit

# Check that data types are as we expect (will break with Python 2.4
# int/long integration?)
try:
    MAXLONGLONG = int(MAXLONGLONG)
    raise TestFailed('not expecting 2^63 - 1 to be a valid Python integer')
except OverflowError:
    pass

storage = metakit.storage()
v = storage.getas('test[intf:I,longf:L]')

# insert rows into view and Python array
arr = []

def checklen(**args):
    alen = len(arr)
    vlen = len(v)
    if alen != vlen:
        fail('append', args, 'view length mismatch', actual=vlen, expected=alen)
        try:
            print 'ARRAY CONTENTS:'
            for arow in arr: print arow
            metakit.dump(v, 'VIEW CONTENTS:')
        except: pass
        raise TestFailed('unexpected number of rows in view, aborting; run in verbose mode for details')

def insert(**args):
    try:
        v.append(args)
        arr.append(args)
    except Exception, e:
        fail('append', args, actual=e)
    try:
        checklen(**args)
    except TestFailed:
        raise
    except Exception, e:
        fail('append', args, 'spurious', actual=e)
   
def reject(exception_class=Exception, **args):
    try:
        ix = v.append(args)
        fail('append', args, 'succeeded', expected=exception_class)
        v.delete(ix)
    except Exception, e:
        if isinstance(e, exception_class):
            if verbose:
                print 'PASS: rejected', args
                print '      as expected <%s> %s' % (e.__class__, e)
        else:
            fail('append', args, expected=exception_class, actual=e)
    try:
        checklen(**args)
    except TestFailed:
        raise
    except Exception, e:
        fail('append', args, 'spurious', actual=e)

# defaults
insert(intf=0, longf=0)

# int field
insert(intf=1, longf=0)
insert(intf=-5, longf=0)
insert(intf=MAXINT, longf=0)
insert(intf=MININT, longf=0)
reject(TypeError, intf=MAXINT + 1, longf=0)
reject(TypeError, intf=MININT - 1, longf=0)

# long field
insert(intf=0, longf=-1L)
insert(intf=0, longf=5L)
insert(intf=0, longf=MAXLONGLONG)
insert(intf=0, longf=MINLONGLONG)
reject(ValueError, intf=0, longf=MAXULONGLONG)
reject(ValueError, intf=0, longf=MAXLONGLONG + 1)
reject(ValueError, intf=0, longf=MAXULONGLONG)
reject(ValueError, intf=0, longf=MINLONGLONG - 1)

# mixed valid int/long
insert(intf=1, longf=2)
insert(intf=-5, longf=-2**30)

# implicit conversion to int
insert(intf=14L, longf=0)
insert(intf=-30L, longf=0)
insert(intf=45.0, longf=0)
insert(intf=21.4, longf=0)
reject(TypeError, intf=float(MAXINT + 1), longf=0)
reject(TypeError, intf=float(MININT - 1), longf=0)
reject(TypeError, intf='215', longf=0)
reject(TypeError, intf='-318.19', longf=0)
reject(TypeError, intf=str(MAXINT + 1), longf=0)

# implicit conversion to long
insert(intf=0, longf=278)
insert(intf=0, longf=-213)
insert(intf=0, longf=95.0)
insert(intf=0, longf=27.3)
reject(ValueError, intf=0, longf=float(2 * MAXLONGLONG))
reject(ValueError, intf=0, longf=float(2 * MINLONGLONG))
reject(TypeError, intf=0, longf=str(MAXLONGLONG))
reject(TypeError, intf=0, longf=str(MINLONGLONG))
reject(TypeError, intf=0, longf='-21.39')
reject(TypeError, intf=0, longf=str(MAXULONGLONG))

# XXX should repeat with assignment instead of appending
# XXX test v.select()

if verbose:
    metakit.dump(v, 'VIEW CONTENTS:')

# compare view with array
for arow, vrow in zip(arr, v):
    failed = False
    for f in arow.keys():
        try:
    	    vf = getattr(vrow, f)
            af = arow[f]
            if af == vf:
                continue
            # Perform the same implicit coercion as Mk4py should
            if type(af) != type(vf):
                try:
                    af = type(vf)(af)
                    if af == vf:
                        continue
                except:
                    pass
            # If we get here, we got an exception or the values didn't match
            # even with coercion
            failed = True
            fail('%s access' % f, arow, expected=af, actual=vf)
    	except Exception, e:
            failed = True
            fail('%s access' % f, arow, expected=arow[f], actual=e)
    if not failed:
        if verbose:
            print 'PASS: retrieved', arow

if failure_count > 0:
    raise TestFailed('%d failures; run in verbose mode for details' % failure_count)
