// PyStorage.h --
// $Id: PyStorage.h,v 1.5 2001/11/25 21:10:38 wcvs Exp $
// This is part of MetaKit, see http://www.equi4.com/metakit/
//
//  Copyright 1999 McMillan Enterprises, Inc. -- www.mcmillan-inc.com
//  Copyright (C) 1999-2001 Jean-Claude Wippler <jcw@equi4.com>
//
//  Storage class header

#if !defined INCLUDE_PYSTORAGE_H
#define INCLUDE_PYSTORAGE_H

#include <mk4.h>
#include "PyHead.h"

extern PyTypeObject PyStoragetype;
class SiasStrategy;

#define PyStorage_Check(v) ((v)->ob_type==&PyStoragetype)

class PyStorage : public PyHead, public c4_Storage {
public:
  PyStorage() 
    : PyHead(PyStoragetype) { }
  PyStorage(c4_Strategy& strategy_, bool owned_ =false, int mode_ =1)
    : PyHead(PyStoragetype), c4_Storage(strategy_, owned_, mode_) { }
  PyStorage(const char *fnm, int mode)
    : PyHead(PyStoragetype), c4_Storage(fnm, mode) { }
  PyStorage(const c4_Storage& storage_)
    : PyHead(PyStoragetype), c4_Storage(storage_) { }
//  PyStorage(const char *fnm, const char *descr) 
//    : PyHead(PyStoragetype), c4_Storage(fnm, descr) { }
  ~PyStorage() { }
};

#endif
