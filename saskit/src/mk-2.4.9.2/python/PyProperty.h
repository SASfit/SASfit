// PyProperty.h --
// $Id: PyProperty.h,v 1.4 2001/11/03 23:56:18 wcvs Exp $
// This is part of MetaKit, see http://www.equi4.com/metakit/
//
//  Copyright 1999 McMillan Enterprises, Inc. -- www.mcmillan-inc.com
//  Copyright (C) 1999-2001 Jean-Claude Wippler <jcw@equi4.com>
//
//  Property class header

#if !defined INCLUDE_PYPROPERTY_H
#define INCLUDE_PYPROPERTY_H

#include <mk4.h>
#include "PyHead.h"

#define PyProperty_Check(ob) ((ob)->ob_type == &PyPropertytype)

extern PyTypeObject PyPropertytype;

class PyProperty : public PyHead, public c4_Property {
public:
	//PyProperty();
	PyProperty(const c4_Property& o) : PyHead(PyPropertytype), c4_Property (o) { }
	PyProperty(char t, const char* n) : PyHead(PyPropertytype), c4_Property (t, n) { }
	~PyProperty () {}
};

PyObject* PyProperty_new(PyObject* o, PyObject* _args);

#endif
