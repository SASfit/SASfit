# metakit.py -- Utility code for the Python interface to MetaKit
# $Id: metakit.py,v 1.5 2001/11/25 21:10:38 wcvs Exp $
# This is part of MetaKit, see http://www.equi4.com/metakit/
# 
# This wraps the raw Mk4py compiled extension interface.
# To use MetaKit through this interface, simply do:
#   import metakit
# After that, things like metakit.storage(...) are available,
# as well as utilities defined below.  This assumes that both
# both mk.py and Mk4py.{dll,so} can be found by Python.

from Mk4py import *
import string

def dump(view, title=None):
  """pretty printer for MK views"""

  widths = []
  cols = []
  justs = []

  props = view.structure()
  for prop in props:
    widths.append(len(prop.name))
    cols.append([None])
    if prop.type in ('I','F','D','V'):
      justs.append(string.rjust)
    else:
      justs.append(string.ljust)

  for row in view:
    for c in range(len(props)):
      attr = getattr(row, props[c].name, None)
      if type(attr) is type(view):
        text = '%d rows' % len(attr)
      else:
        text = str(attr)
        if len(text) > 20:
          text = text[0:17] + '...'
      widths[c] = max(widths[c],len(text))
      cols[c].append(text)

  if title: print title

  for c in range(len(props)):
    cols[c][0] = widths[c] * '-'
    cols[c].append(cols[c][0])
    print '', string.ljust(props[c].name, widths[c]),
  print

  for r in xrange(len(view)+2):
    for c in range(len(props)):
      print '', justs[c](cols[c][r], widths[c]),
    print

  print " Total: %d rows" % len(view)

if __name__ == '__main__':
  db = storage()
  f = db.getas('frequents[drinker,bar,perweek:I]')
  s = db.getas('serves[bar,beer,quantity:I]')

  f.append(drinker='adam', bar='lolas', perweek=1)
  f.append(drinker='woody', bar='cheers', perweek=5)
  f.append(drinker='sam', bar='cheers', perweek=5)
  f.append(drinker='lola', bar='lolas', perweek=6)

  s.append(bar='cheers', beer='bud', quantity=500)
  s.append(bar='cheers', beer='samaddams', quantity=255)
  s.append(bar='lolas', beer='mickies', quantity=1515)

  dump(f, 'frequents:')
  dump(s, 'serves:')
  dump(f.join(s, s.bar), 'join on "bar":')
