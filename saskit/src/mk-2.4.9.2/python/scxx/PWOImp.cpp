/******************************************** 
  copyright 1999 McMillan Enterprises, Inc.
  www.mcmillan-inc.com
*********************************************/
#include "PWOSequence.h"
#include "PWOMSequence.h"
#include "PWOMapping.h"
#include "PWOCallable.h"

// dummy exception singleton
const PWDException PWDPyExceptionObj;
const PWDException& PWDPyException = PWDPyExceptionObj;

  // incref new owner, and decref old owner, and adjust to new owner
void PWOBase::GrabRef(PyObject* newObj)
{
    // be careful to incref before decref if old is same as new
  Py_XINCREF(newObj);
  Py_XDECREF(_own);
  _own = _obj = newObj;
}

PWOTuple::PWOTuple(const PWOList& list)
  : PWOSequence (PyList_AsTuple(list)) { LoseRef(_obj); }

PWOListMmbr::PWOListMmbr(PyObject* obj, PWOList& parent, int ndx) 
  : PWOBase(obj), _parent(parent), _ndx(ndx) { }

PWOListMmbr& PWOListMmbr::operator=(const PWOBase& other) {
  GrabRef(other);
  //Py_XINCREF(_obj); // this one is for setItem to steal
  _parent.setItem(_ndx, *this);
  return *this;
}
PWOMappingMmbr& PWOMappingMmbr::operator=(const PWOBase& other) {
  GrabRef(other);
  _parent.setItem(_key, *this);
  return *this;
}
PWOBase PWOCallable::call() const {
  static PWOTuple _empty;
  PyObject *rslt = PyEval_CallObjectWithKeywords(*this, _empty, NULL);
  if (rslt == 0)
    throw PWDPyException;
  return rslt;
}
PWOBase PWOCallable::call(PWOTuple& args) const {
  PyObject *rslt = PyEval_CallObjectWithKeywords(*this, args, NULL);
  if (rslt == 0)
    throw PWDPyException;
  return rslt;
}
PWOBase PWOCallable::call(PWOTuple& args, PWOMapping& kws) const {
  PyObject *rslt = PyEval_CallObjectWithKeywords(*this, args, kws);
  if (rslt == 0)
    throw PWDPyException;
  return rslt;
}

void Fail(PyObject* exc, const char* msg)
{
  PyErr_SetString(exc, msg);
  throw PWDPyException;
}

void FailIfPyErr()
{
  PyObject *exc = PyErr_Occurred();
  if (exc != NULL) throw PWDPyException;
}
