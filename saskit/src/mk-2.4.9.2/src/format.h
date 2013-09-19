// format.h --
// $Id: format.h,v 1.6 2001/11/25 21:10:38 wcvs Exp $
// This is part of MetaKit, see http://www.equi4.com/metakit/

/** @file
 * Encapsulation of all format handlers
 */

#ifndef __FORMAT_H__
#define __FORMAT_H__

/////////////////////////////////////////////////////////////////////////////
// Declarations in this file

  class c4_Handler;         // not defined here

  extern c4_Handler* f4_CreateFormat(const c4_Property&, c4_HandlerSeq&);
  extern int f4_ClearFormat(char);
  extern int f4_CompareFormat(char, const c4_Bytes&, const c4_Bytes&);

/////////////////////////////////////////////////////////////////////////////

#endif
