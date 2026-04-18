// This may look like C code, but it is really -*- C++ -*-
/*
 ************************************************************************
 *
 *			  Linear Algebra Package
 *			  
 * The following functions support a _sequential_ traversal of a matrix
 * or its parts (a row, a column, a diagonal, or an arbitrary rectangular
 * block). As the README file explains, a random access to matrix elements
 * is often an unnecessary luxury. Many Numerical Math algorithms access
 * matrices by walking them in regular sequential ways. This _serial_
 * access can be implemented far more efficiently than the arbitrary
 * access.
 * 
 * The classes below act as input/ouput "matrix" streams. As with regular
 * streams, be sure to check for eof() before attempting to get() an
 * element.
 * Note, since all the methods of the matrix streams are implemented inline,
 * the whole iteration would be inline, too.
 * Note, that if the stream is applied to the entire Matrix or its
 * rectangular block, it would be traversed in the natural, that is,
 * COLUMN major order.
 *
 * $Id: LAStreams.h,v 1.1 1998/12/06 23:39:12 oleg Exp oleg $
 *
 ************************************************************************
 */

#ifndef __GNUC__
#pragma once
#endif

#ifndef _LAStreams_h
#define _LAStreams_h 1

#if !defined(_LinAlg_h)
#include "LinAlg.h"
#endif

class LAS {			// A "namespace" to define some common constants
public:
 enum seek_dir { beg, cur, end};
 
 static inline const REAL * min(const REAL * const p1, const REAL * const p2)
  	{ return p1 > p2 ? p2 : p1; }
	
 static inline const REAL * max(const REAL * const p1, const REAL * const p2)
  	{ return p1 > p2 ? p1 : p2; }
	
};
          			// A bookmark in the stream
class AREALMark
{
  friend class AREALStreamIn;
  friend class AREALStreamOut;
  friend class AREALStrideStreamIn;
  friend class AREALStrideStreamOut;
  friend class AREALBlockStreamIn;
  friend class AREALBlockStreamOut;
  size_t offset;
  enum { invalid_offset = (size_t)(-1) };
  AREALMark(const size_t _offset) : offset(_offset) {}
public:
  AREALMark(void) : offset(invalid_offset) {}
  operator bool (void) const	{ return offset != invalid_offset; }
  bool operator ! (void) const	{ return offset == invalid_offset; }
  rowcol get_2dpos(const DimSpec dims) const
  	{ assert( (bool)(*this) && dims.q_nrows() > 0 );
  	  div_t res = div(offset,dims.q_nrows());
  	  return rowcol(res.rem+dims.q_row_lwb(),res.quot+dims.q_col_lwb()); }
  friend ostream& operator << (ostream& os, const AREALMark& mark);
};
  
 
 			// The following are the pair of "streams" over
 			// the field of REALs
 			// These classes are mixins - they provide an
 			// "abstract" stream functionality. The classes
 			// don't have public constructors, as none of
 			// mixin classes do. The classes are supposed to be
 			// "blended" with other classes to imbue them with
 			// stream properties: the derived class
 			// must then properly set the current element
 			// pointer and the last element pointer.
 			// Since the classes do not declare any virtual
 			// functions, they are lightweight (and can be
 			// inlined)

class AREALStreamIn
{
  const REAL * curr_el_p;            // curr_el_p <= last_el_p always
  const REAL * const first_el_p;     // The first element pointer
  const REAL * const last_el_p;      // The == condition means EOF
  
  AREALStreamIn(const AREALStreamIn&);	 // Not implemented and forbidden:
  void operator = (const AREALStreamIn&);// no cloning/assignment allowed

  				// Apply range:dir to yield a new first_el_p
  				// invariant: new_first_el_p >= first_el_p
  const REAL * subrange_first(const IRange range, LAS::seek_dir dir) const
  {
    if( dir == LAS::beg )
      return range.lwb <= 0 ? first_el_p : first_el_p + range.lwb;
    else if( dir == LAS::end )
      return range.lwb == - IRange::INF ? first_el_p : 
   		LAS::max(first_el_p,last_el_p - range.lwb);
    else
      return range.lwb == - IRange::INF ? curr_el_p : 
   		LAS::max(first_el_p,curr_el_p + range.lwb);
  }

  				// Apply range:dir to yield a new last_el_p
  				// invariant: new_last_el_p <= last_el_p
  const REAL * subrange_last(const IRange range, LAS::seek_dir dir) const
  {
    if( dir == LAS::beg )
      return range.upb == IRange::INF ? last_el_p : 
   		LAS::min(last_el_p,first_el_p + range.upb + 1);
    else if( dir == LAS::end )
      return last_el_p;
    else
      return range.upb == IRange::INF ? last_el_p : 
   		LAS::min(last_el_p,curr_el_p + range.upb + 1);
  }

protected:
				// A protected constructor
  AREALStreamIn(const REAL * _beg_ptr, const REAL * _end_ptr)
        : curr_el_p(_beg_ptr),
          first_el_p(_beg_ptr),
          last_el_p(_end_ptr)
          {}

	  			// Subrange the current stream by applying
	  			// the range. seek_dir determines if the
	  			// range applies to the current state
	  			// of the stream or its init position
  AREALStreamIn(const AREALStreamIn& proto, const IRange range,
		LAS::seek_dir dir)
        : first_el_p(proto.subrange_first(range,dir)),
          last_el_p(proto.subrange_last(range,dir))
          { curr_el_p = first_el_p;
	    assert(first_el_p <= last_el_p); }

                                // Get the current elem _ref_ and advance
                                // in the stream
  const REAL& get_ref(void)
        { if( curr_el_p >= last_el_p)
           _error("Can't get() the AREALStream boundary!");
          return *curr_el_p++; }
  const REAL& peek_ref(void) const      // Does *not* advance the "stream ptr"
        { if( curr_el_p >= last_el_p)
           _error("Can't peek() past the AREALStream boundary!");
          return *curr_el_p; }

public:
			// No public constructors!

  bool eof(void) const          { return curr_el_p == last_el_p; }
  bool bof(void) const          { return curr_el_p == first_el_p; }

                                // Get the current elem and advance in the
                                // stream
  REAL get(void)		{ return get_ref(); }
  REAL peek(void) const         // Does *not* advance the "stream ptr"
  				{ return peek_ref(); }
          
          			// Reset the stream at the beginning
  void rewind(void)		{ curr_el_p = first_el_p; }
 
  			// Note the current position in the stream
  AREALMark tell(void) const
  	{ return AREALMark(curr_el_p - first_el_p); }
  
  			// Note the previous (that is, just gotten)
  			// position in the stream
  AREALMark tell_prev(void) const
  	{ if( curr_el_p == first_el_p )
  	    _error("The stream is at its beginng, no previous pos exists");
  	  return AREALMark(curr_el_p - first_el_p - 1); }
 
 			// Set the current position at the mark
  AREALStreamIn& seek(const AREALMark mark)
  	{ assert( (bool) mark );
  	  curr_el_p = first_el_p + mark.offset;
  	  assert( curr_el_p >= first_el_p && curr_el_p < last_el_p );
  	  return *this; }
	  

 			// Set the current position according to the
	  		// given offset and seek_dir.
	  		// Note: the offset can be arbitrarily large:
	  		// if it points beyond this stream, no run-time
	   		// error is generated but the EOF condition is set
	  		// (which can later be checked with eof())
  AREALStreamIn& seek(const int offset, LAS::seek_dir dir = LAS::cur)
  	{ switch(dir)
	  { case LAS::beg: curr_el_p = first_el_p + offset; break;
	    case LAS::cur: curr_el_p = curr_el_p + offset; break;
	    case LAS::end: curr_el_p = last_el_p - offset; break;
	    default: assert(0 /*wrong seek_dir*/); };
	  if( curr_el_p > last_el_p ) curr_el_p = last_el_p;
  	  else assert( curr_el_p >= first_el_p );
  	  return *this; }
	  
  AREALStreamIn& ignore(const int how_many)
  	{ return seek(how_many,LAS::cur); }
 
  	  		// Dump the current status of the stream
  ostream& dump(ostream& os) const;
};

			// This is a readable _and_ writable stream
class AREALStreamOut : public AREALStreamIn
{
  AREALStreamOut(const AREALStreamOut&);  // Not implemented and forbidden:
  void operator = (const AREALStreamOut&);// no cloning/assignment allowed

protected:
				// A protected constructor
  AREALStreamOut(REAL * _beg_ptr, REAL * _end_ptr)
        : AREALStreamIn(_beg_ptr,_end_ptr)
          {}

  AREALStreamOut(const AREALStreamIn& proto, const IRange range,
		LAS::seek_dir dir)
  	: AREALStreamIn(proto,range,dir) {}

public:
			// No public constructors!

                                // Get the current elem and advance in the
                                // stream
  REAL& get(void)		{ return const_cast<REAL&>(get_ref()); }
  REAL& peek(void) const         // Does *not* advance the "stream ptr"
  				{ return const_cast<REAL&>(peek_ref()); }
          
  	  		// Dump the current status of the stream
//  ostream& dump(ostream& os) const;
};
 

                                // The following class is an STL-like iterator
                                // to access _values_ of Matrix elements
                                // one-by-one.
                                // Note this is a _read-only_ stream
class LAStreamIn : protected Matrix::ConstReference,
		   public AREALStreamIn
{
  LAStreamIn(const LAStreamIn&);	// Not implemented and forbidden:
  void operator = (const LAStreamIn&);	// no cloning/assignment allowed

public:
  LAStreamIn(const Matrix& m)
        : Matrix::ConstReference(m),
          AREALStreamIn(m.elements,m.elements+m.nelems)
          {}
  inline LAStreamIn(const ConstMatrixColumn& mc);
  LAStreamIn(const LAStreamIn& ls, const IRange range,
	    LAS::seek_dir dir = LAS::cur)
  	: Matrix::ConstReference(ls.ref()),
	  AREALStreamIn(ls,range,dir) {}
  inline LAStreamIn(const LAStreamOut& ls, const IRange range,
		    LAS::seek_dir dir = LAS::cur);
  rowcol get_pos(const AREALMark mark) const
  	  { return mark.get_2dpos(operator const Matrix& ()); }
};

inline LAStreamIn::LAStreamIn(const ConstMatrixColumn& mc)
        : Matrix::ConstReference(mc.ref()),
          AREALStreamIn(mc.col_ptr,mc.col_ptr+mc.nrows)
          {}

                                // This class provides read *and* write
                                // access to matrix elements, one-by-one
class LAStreamOut : protected Matrix::Reference,
		    public AREALStreamOut
{
  friend class LAStreamIn;
  LAStreamOut(const LAStreamOut&);	// Not implemented and forbidden:
  void operator = (const LAStreamOut&);	// no cloning/assignment allowed
public:
  LAStreamOut(Matrix& m)
        : Matrix::Reference(m),
          AREALStreamOut(m.elements,m.elements+m.nelems)
          {}
  inline LAStreamOut(const MatrixColumn& mc);
  LAStreamOut(const LAStreamOut& ls, const IRange range,
	    LAS::seek_dir dir = LAS::cur)
  	: Matrix::Reference(ls.ref()),
	  AREALStreamOut(ls,range,dir) {}
  rowcol get_pos(const AREALMark mark)
  	  { return mark.get_2dpos(operator Matrix& ()); }
};

inline LAStreamOut::LAStreamOut(const MatrixColumn& mc)
        : Matrix::Reference(const_cast<Matrix&>(mc.ref())),
          AREALStreamOut(const_cast<REAL*>(mc.col_ptr),
          		 const_cast<REAL*>(mc.col_ptr)+mc.q_nrows())
          {}

inline LAStreamIn::LAStreamIn(const LAStreamOut& ls, const IRange range,
		    LAS::seek_dir dir)
  	: Matrix::ConstReference(ls.ref()),
	  AREALStreamIn(ls,range,dir) {}

 
 			// The following pair of streams are similar
 			// to the ones above, with an exception they
 			// walk a sequence of REALs with a non-unit
 			// stride
class AREALStrideStreamIn
{
  const REAL * curr_el_p;
  const REAL * const first_el_p;     // The first element pointer
  const REAL * const last_el_p;      // The curr_el_p >= condition means EOF
  const int stride;

  AREALStrideStreamIn(const AREALStrideStreamIn&); // Not implemented and forbidden:
  void operator = (const AREALStrideStreamIn&);// no cloning/assignment allowed

  				// Apply range:dir to yield a new first_el_p
  				// invariant: new_first_el_p >= first_el_p
  const REAL * subrange_first(const IRange range, LAS::seek_dir dir) const
  {
    if( dir == LAS::beg )
      return range.lwb <= 0 ? first_el_p : first_el_p + stride*range.lwb;
    else if( dir == LAS::end )
     if( range.lwb == - IRange::INF )
       return first_el_p;
     else
     { const int r = (last_el_p - first_el_p) % stride;
       return LAS::max(first_el_p,
		       (r == 0 ? last_el_p : last_el_p - r + stride)
	    				- range.lwb*stride); }
    else
      return range.lwb == - IRange::INF ? curr_el_p : 
   		LAS::max(first_el_p,curr_el_p + stride*range.lwb);
  }

  				// Apply range:dir to yield a new last_el_p
  				// invariant: new_last_el_p <= last_el_p
  const REAL * subrange_last(const IRange range, LAS::seek_dir dir) const
  {
    if( dir == LAS::beg )
      return range.upb == IRange::INF ? last_el_p : 
   		LAS::min(last_el_p,first_el_p + stride*(range.upb+1));
    else if( dir == LAS::end )
      return last_el_p;
    else
      return range.upb == IRange::INF ? last_el_p : 
   		LAS::min(last_el_p,curr_el_p + stride*(range.upb+1));
  }
 
protected:
				// A protected constructor
  AREALStrideStreamIn(const REAL * _beg_ptr, const REAL * _end_ptr,
  		      const int _stride)
        : curr_el_p(_beg_ptr),
          first_el_p(_beg_ptr),
          last_el_p(_end_ptr),
          stride(_stride)
          {}

	  			// Subrange the current stream by applying
	  			// the range. seek_dir determines if the
	  			// range applies to the current state
	  			// of the stream or its init position
  AREALStrideStreamIn(const AREALStrideStreamIn& proto, const IRange range,
		LAS::seek_dir dir)
        : first_el_p(proto.subrange_first(range,dir)),
          last_el_p(proto.subrange_last(range,dir)),
	  stride(proto.stride)
          { curr_el_p = first_el_p;
	    assert(first_el_p <= last_el_p); }

                                // Get the current elem _ref_ and advance
                                // in the stream
  const REAL& get_ref(void)
        { if( curr_el_p >= last_el_p)
           _error("Can't get() past the AREALStrideStream boundary!");
          const REAL& curr = *curr_el_p;
          return curr_el_p += stride, curr; }
  const REAL& peek_ref(void) const      // Does *not* advance the "stream ptr"
        { if( curr_el_p >= last_el_p)
           _error("Can't peek() past the AREALStrideStream boundary!");
          return *curr_el_p; }

public:
			// No public constructors!

  bool eof(void) const          { return curr_el_p >= last_el_p; }
  bool bof(void) const          { return curr_el_p == first_el_p; }

                                // Get the current elem and advance in the
                                // stream
  REAL get(void)		{ return get_ref(); }
  REAL peek(void) const         // Does *not* advance the "stream ptr"
  				{ return peek_ref(); }
          
          			// Reset the stream at the beginning
  void rewind(void)		{ curr_el_p = first_el_p; }
 
  			// Note the current position in the stream
  AREALMark tell(void) const
  	{ return AREALMark(curr_el_p - first_el_p); }
  
  			// Note the previous (that is, just gotten)
  			// position in the stream
  AREALMark tell_prev(void) const
  	{ if( curr_el_p == first_el_p )
  	    _error("The stream is at its beginng, no previous pos exists");
  	  return AREALMark(curr_el_p - first_el_p - stride); }
 
 			// Set the current position at the mark
  AREALStrideStreamIn& seek(const AREALMark mark)
  	{ assert( (bool) mark );
  	  curr_el_p = first_el_p + mark.offset;
  	  assert( curr_el_p >= first_el_p && curr_el_p < last_el_p );
  	  return *this; }
  
 			// Set the current position according to the
	  		// given offset and seek_dir.
	  		// Note: the offset can be arbitrarily large:
	  		// if it points beyond this stream, no run-time
	   		// error is generated but the EOF condition is set
	  		// (which can later be checked with eof())
  AREALStrideStreamIn& seek(const int offset, LAS::seek_dir dir = LAS::cur)
  	{ switch(dir)
	  { case LAS::beg: curr_el_p = first_el_p + offset*stride; break;
	    case LAS::cur: curr_el_p = curr_el_p + offset*stride; break;
	    case LAS::end: { const int r = (last_el_p - first_el_p) % stride;
	    		curr_el_p = (r == 0 ? last_el_p : last_el_p - r + stride)
	    				- offset*stride; } break;
	    default: assert(0 /*wrong seek_dir*/); };
	  assert( curr_el_p >= first_el_p );
  	  return *this; }
	  
  AREALStrideStreamIn& ignore(const int how_many)
  	{ return seek(how_many,LAS::cur); }
	
  	  		// Dump the current status of the stream
  ostream& dump(ostream& os) const;
};

			// This is a readable _and_ writable stream
class AREALStrideStreamOut : public AREALStrideStreamIn
{
  AREALStrideStreamOut(const AREALStrideStreamOut&);  // Not implemented and forbidden:
  void operator = (const AREALStrideStreamOut&);// no cloning/assignment allowed

protected:
				// A protected constructor
  AREALStrideStreamOut(REAL * _beg_ptr, REAL * _end_ptr,
  		       const int _stride)
        : AREALStrideStreamIn(_beg_ptr,_end_ptr,_stride)
          {}


  AREALStrideStreamOut(const AREALStrideStreamIn& proto, const IRange range,
		LAS::seek_dir dir)
  	: AREALStrideStreamIn(proto,range,dir) {}

public:
			// No public constructors!

                                // Get the current elem and advance in the
                                // stream
  REAL& get(void)		{ return const_cast<REAL&>(get_ref()); }
  REAL& peek(void) const         // Does *not* advance the "stream ptr"
  				{ return const_cast<REAL&>(peek_ref()); }
          
  	  		// Dump the current status of the stream
//  ostream& dump(ostream& os) const;
};
 


                                // LAStreams with a stride
class LAStrideStreamIn : protected Matrix::ConstReference,
		   public AREALStrideStreamIn
{
  LAStrideStreamIn(const LAStrideStreamIn&);	// Not implemented and forbidden:
  void operator = (const LAStrideStreamIn&);	// no cloning/assignment allowed

public:
  LAStrideStreamIn(const Matrix& m, const int stride)
        : Matrix::ConstReference(m),
          AREALStrideStreamIn(m.elements,m.elements+m.nelems,stride)
          {}
  LAStrideStreamIn(const LAStrideStreamIn& ls, const IRange range,
	    LAS::seek_dir dir = LAS::cur)
  	: Matrix::ConstReference(ls.ref()),
	  AREALStrideStreamIn(ls,range,dir) {}
 inline LAStrideStreamIn(const LAStrideStreamOut& ls, const IRange range,
		    LAS::seek_dir dir = LAS::cur);
 inline LAStrideStreamIn(const ConstMatrixRow& mr);
 inline LAStrideStreamIn(const ConstMatrixDiag& md);
 rowcol get_pos(const AREALMark mark) const
  	  { return mark.get_2dpos(operator const Matrix& ()); }
};

inline
LAStrideStreamIn::LAStrideStreamIn(const ConstMatrixRow& mr)
  	: Matrix::ConstReference(mr.ref()),
  	  AREALStrideStreamIn(const_cast<REAL*>(mr.row_ptr),
  	  		      const_cast<REAL*>(mr.end_ptr),mr.stride) 
  	{ }
inline
LAStrideStreamIn::LAStrideStreamIn(const ConstMatrixDiag& md)
  	: Matrix::ConstReference(md.ref()),
  	  AREALStrideStreamIn(const_cast<REAL*>(md.start_ptr),
  	  		      const_cast<REAL*>(md.end_ptr),md.stride) 
  	{ }



class LAStrideStreamOut : protected Matrix::Reference,
		    public AREALStrideStreamOut
{
  friend class LAStrideStreamIn;
  LAStrideStreamOut(const LAStrideStreamOut&);	// Not implemented and forbidden:
  void operator = (const LAStrideStreamOut&);	// no cloning/assignment allowed
public:
  LAStrideStreamOut(Matrix& m, const int stride)
        : Matrix::Reference(m),
          AREALStrideStreamOut(m.elements,m.elements+m.nelems,stride)
          {}
  LAStrideStreamOut(const LAStrideStreamOut& ls, const IRange range,
	    LAS::seek_dir dir = LAS::cur)
  	: Matrix::Reference(ls.ref()),
	  AREALStrideStreamOut(ls,range,dir) {}
  inline LAStrideStreamOut(const MatrixRow& mr);
  inline LAStrideStreamOut(const MatrixDiag& md);
  rowcol get_pos(const AREALMark mark)
  	  { return mark.get_2dpos(operator Matrix& ()); }
};

inline LAStrideStreamOut::LAStrideStreamOut(const MatrixRow& mr)
        : Matrix::Reference(const_cast<Matrix&>(mr.ref())),
          AREALStrideStreamOut(const_cast<REAL*>(mr.row_ptr),
          		      const_cast<REAL*>(mr.end_ptr),mr.stride)
          {}
inline LAStrideStreamOut::LAStrideStreamOut(const MatrixDiag& md)
        : Matrix::Reference(const_cast<Matrix&>(md.ref())),
          AREALStrideStreamOut(const_cast<REAL*>(md.start_ptr),
          		       const_cast<REAL*>(md.end_ptr),md.stride)
          {}

inline LAStrideStreamIn::LAStrideStreamIn(const LAStrideStreamOut& ls, 
					  const IRange range,
					 LAS::seek_dir dir)
  	: Matrix::ConstReference(ls.ref()),
	  AREALStrideStreamIn(ls,range,dir) {}





 			// The following pair of streams are similar
 			// to the ones above, with an exception they
 			// sequentially walk a *block* of REALs, in
 			// a column major order
class AREALBlockStreamIn
{
  const REAL * curr_el_p;
  const REAL * const first_el_p;     	// The first element pointer
  const REAL * const last_el_p;      	// The curr_el_p >= condition means EOF
  const REAL * last_col_el_p;		// The end-boundary of the curr column
  const int col_size;		     	// The size of the column in a block
  const int eoc_jump;			// The amount to forward-feed the
  				// curr_el_p to point at the beginning of the
  				// next col once we have moved past the curr col
  
  AREALBlockStreamIn(const AREALBlockStreamIn&); // Not implemented and forbidden:
  void operator = (const AREALBlockStreamIn&);// no cloning/assignment allowed

  				// Set curr_el_p to p and compute the
  				// corresponding column boundary
  void set_curr_col(const REAL * const p)
  {
    curr_el_p = p;
    last_col_el_p = curr_el_p
 	- ((curr_el_p - first_el_p) % (col_size + eoc_jump))
	+ col_size;
  }
protected:
				// A protected constructor
  AREALBlockStreamIn(const REAL * _beg_ptr, const REAL * _end_ptr,
  		      const int _col_size, const int _eoc_jump)
        : curr_el_p(_beg_ptr),
          first_el_p(_beg_ptr),
          last_el_p(_end_ptr),
	  last_col_el_p(_beg_ptr+_col_size),
          col_size(_col_size),
	  eoc_jump(_eoc_jump)
          {}

	  
				// Another protected constructor
  AREALBlockStreamIn(const REAL * orig_ptr, const DimSpecSubranged& clip)
        : first_el_p(orig_ptr + clip.q_min_offset()),
          last_el_p(orig_ptr + clip.q_max_offset()+1),
          col_size(clip.q_nrows()),
	  eoc_jump(clip.q_row_diff())
          { last_col_el_p = (curr_el_p = first_el_p)  + col_size;
	    assert(eoc_jump>=0); }


                                // Get the current elem _ref_ and advance
                                // in the stream
  const REAL& get_ref(void)
        { if( curr_el_p >= last_el_p)
           _error("Can't get() past the AREALBlockStreamIn boundary!");
          const REAL& curr = *curr_el_p++;
	  if( curr_el_p >= last_col_el_p )
	    last_col_el_p = (curr_el_p += eoc_jump) + col_size;
	  return curr; }
  const REAL& peek_ref(void) const      // Does *not* advance the "stream ptr"
        { if( curr_el_p >= last_el_p)
           _error("Can't peek() past the AREALBlockStreamIn boundary!");
          return *curr_el_p; }
public:
			// No public constructors!

  bool eof(void) const          { return curr_el_p >= last_el_p; }
  bool bof(void) const          { return curr_el_p == first_el_p; }

                                // Get the current elem and advance in the
                                // stream
  REAL get(void)		{ return get_ref(); }
  REAL peek(void) const         // Does *not* advance the "stream ptr"
  				{ return peek_ref(); }
          
          			// Reset the stream at the beginning
  void rewind(void)
  	{ last_col_el_p = (curr_el_p = first_el_p) + col_size; }
 
  			// Note the current position in the stream
  AREALMark tell(void) const
  	{ return AREALMark(curr_el_p - first_el_p); }
  
  			// Note the previous (that is, just gotten)
  			// position in the stream
  AREALMark tell_prev(void) const
  	{ if( curr_el_p == first_el_p )
  	    _error("The stream is at its beginng, no previous pos exists");
  	  return AREALMark( curr_el_p - first_el_p -
		( curr_el_p + col_size > last_col_el_p ? 1
			 : 1 + eoc_jump)); }
 
 			// Set the current position at the mark
  AREALBlockStreamIn& seek(const AREALMark mark)
  	{ assert( (bool) mark );
  	  set_curr_col(first_el_p + mark.offset);
  	  assert( curr_el_p >= first_el_p && curr_el_p < last_el_p );
  	  return *this; }
  
 			// Set the current position according to the
	  		// given offset and seek_dir.
	  		// Note: the offset can be arbitrarily large:
	  		// if it points beyond this stream, no run-time
	   		// error is generated but the EOF condition is set
	  		// (which can later be checked with eof())
  AREALBlockStreamIn& seek(const int offset, LAS::seek_dir dir = LAS::cur)
  	{ div_t res = div(offset,col_size);
	  switch(dir)
	  { case LAS::beg: curr_el_p = first_el_p + res.quot*(col_size+eoc_jump);
			   last_col_el_p = curr_el_p + col_size;
			   curr_el_p += res.rem; break;
	    case LAS::cur: curr_el_p += res.rem;
			   if( curr_el_p >= last_col_el_p )
			   	curr_el_p += eoc_jump, last_col_el_p += col_size+eoc_jump;
			   curr_el_p += res.quot*(col_size+eoc_jump);
			   last_col_el_p += res.quot*(col_size+eoc_jump);
			   break;
	    case LAS::end: last_col_el_p = last_el_p - res.quot*(col_size+eoc_jump);
			   curr_el_p = last_col_el_p - res.rem; break;
	    default: assert(0 /*wrong seek_dir*/); };
	  assert( curr_el_p >= first_el_p );
  	  return *this; }
	  
  AREALBlockStreamIn& ignore(const int how_many)
  	{ return seek(how_many,LAS::cur); }
	
  	  		// Dump the current status of the stream
  ostream& dump(ostream& os) const;
};

			// This is a readable _and_ writable stream
class AREALBlockStreamOut : public AREALBlockStreamIn
{
  AREALBlockStreamOut(const AREALBlockStreamOut&);  // Not implemented and forbidden:
  void operator = (const AREALBlockStreamOut&);// no cloning/assignment allowed

protected:
				// A protected constructor
  AREALBlockStreamOut(REAL * _beg_ptr, REAL * _end_ptr,
  		       const int _col_size, const int _eoc_jump)
        : AREALBlockStreamIn(_beg_ptr,_end_ptr,_col_size,_eoc_jump)
          {}
  AREALBlockStreamOut(REAL * orig_ptr, const DimSpecSubranged& clip)
        : AREALBlockStreamIn(orig_ptr,clip)
          {}

public:
			// No public constructors!

                                // Get the current elem and advance in the
                                // stream
  REAL& get(void)		{ return const_cast<REAL&>(get_ref()); }
  REAL& peek(void) const         // Does *not* advance the "stream ptr"
  				{ return const_cast<REAL&>(peek_ref()); }
          
};
 


                                // LAStreams over a sub-block of a matrix
class LABlockStreamIn : protected Matrix::ConstReference,
		   public AREALBlockStreamIn
{
  LABlockStreamIn(const LABlockStreamIn&);	// Not implemented and forbidden:
  void operator = (const LABlockStreamIn&);	// no cloning/assignment allowed

public:
  LABlockStreamIn(const Matrix& m, const IRange row_range,
		  const IRange col_range)
        : Matrix::ConstReference(m),
          AREALBlockStreamIn(m.elements,DimSpecSubranged(m,row_range,col_range))
          {}
  rowcol get_pos(const AREALMark mark) const
  	  { return mark.get_2dpos(operator const Matrix& ()); }
};

class LABlockStreamOut : protected Matrix::Reference,
		    public AREALBlockStreamOut
{
  LABlockStreamOut(const LABlockStreamOut&);	// Not implemented and forbidden:
  void operator = (const LABlockStreamOut&);	// no cloning/assignment allowed
public:
  LABlockStreamOut(Matrix& m, const IRange row_range,
		  const IRange col_range)
        : Matrix::Reference(m),
          AREALBlockStreamOut(m.elements,DimSpecSubranged(m,row_range,col_range))
          {}
  rowcol get_pos(const AREALMark mark)
  	  { return mark.get_2dpos(operator Matrix& ()); }
};

#endif

