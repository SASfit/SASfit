// This may look like C code, but it is really -*- C++ -*-
/*
 ************************************************************************
 *
 *		  Verify streams over matrices and vectors
 *
 *
 * $Id: vlastreams.cc,v 1.1 1998/12/13 23:29:39 oleg Exp oleg $
 *
 ************************************************************************
 */

#include "LAStreams.h"
#include "builtin.h"
#include <math.h>
#include "iostream.h"
#include <float.h>

/*
 *------------------------------------------------------------------------
 *	   Test allocation functions and compatibility check
 */

static void test_LAStreams(const int rsize, const int csize)
{
  const double pattern = 4.25;
  const double incr = 0.5;
  cout << "\n---> Test LAStreams\n";
  
  Matrix m(2,rsize+1,0,csize-1);
  {
    cout << "Fill out the matrix in the natural order via an ElementAction..." 
         << endl;
    struct filler : public ElementAction
    {
      double counter;
      const double incr;
      void operation(REAL& element, const int i, const int j)
	  		{ element = (counter += incr); }
      filler(const double _init_val, const double _incr) :
      	counter(_init_val), incr(_incr) {}
    };
    m.apply(filler(pattern,incr));
    assert( of_every(m) > pattern );
    assert( !(of_every(m) == pattern+incr) );	// There is at least one element
    assert( !(m != pattern+incr) );				// == and != to pattern+incr
  }
  
  {
    cout << "Check the filling order by a direct access. "
    	    "The row index varies first!" << endl;
    ConstMatrixDA ma(m);
    double counter = pattern;
    for(register int j=ma.q_col_lwb(); j<=ma.q_col_upb(); j++)
      for(register int i=ma.q_row_lwb(); i<=ma.q_row_upb(); i++)
         assert( ma(i,j) == (counter += incr) );
    assert( counter == pattern + m.q_no_elems() * incr );
  } 
  {
    cout << "Check the filling order via ElementWiseConstAction" << endl;
    struct checker : public ElementWiseConstAction
    {
      double counter;
      double incr;
      void operator () (const REAL element) 
      	{ assert(element == (counter += incr)); }
      checker(const double _init_val, const double _incr) :
      	counter(_init_val), incr(_incr) {}
    };
    of_every(m).apply(checker(pattern,incr));
  } 
  {
    cout << "Check the filling order via AREALStreamIn" << endl;
    LAStreamIn m_str(m);
    assert( !m_str.eof() );
    assert( m_str.bof() );
    double counter = pattern;
    AREALMark mark;
    assert( !mark ); // a user-created AREALMark is always invalid...
    assert( (bool)mark == false );
    mark = m_str.tell();
    assert( (bool)mark );	// the mark becomes valid now
    assert( m_str.get_pos(mark) == rowcol(m.q_row_lwb(),m.q_col_lwb()) );
    for(register int i=0; i<m.q_no_elems(); i++)
    {
      assert( !m_str.eof() );
      assert( m_str.peek() == (counter += incr) );
      assert( m_str.get() == counter );
      assert( !m_str.bof() );
    }
    assert( m_str.eof() );
    assert( m_str.get_pos(m_str.tell_prev()) ==
    	    rowcol(m.q_row_upb(),m.q_col_upb()) );
    cout << "\trewinding the stream and checking again..." << endl;
    m_str.rewind();
    assert( !m_str.eof() );
    assert( m_str.bof() );
    assert( m_str.get() == pattern + incr );
    assert( m_str.peek() == pattern + incr + incr );
    m_str.seek(mark);
    assert( !m_str.eof() );
    assert( m_str.bof() );
    assert( m_str.peek() == pattern + incr );
    assert( m_str.peek() == pattern + incr );
    assert( m_str.get() == pattern + incr );
  } 
  
  {
    cout << "Check the filling order via AREALStreamOut" << endl;
    Matrix m_copy = m;		// Duplicate the matrix
    assert( m_copy == m );
    LAStreamOut m_str(m_copy);
    assert( !m_str.eof() );
    assert( m_str.bof() );
    double counter = pattern;
    AREALMark mark = m_str.tell();
    assert( (bool)mark );
    assert( m_str.get_pos(mark) == 
    	    rowcol(m_copy.q_row_lwb(),m_copy.q_col_lwb()) );
    while( !m_str.eof() )
    {
      m_str.peek() -= (counter += incr);
      assert( m_str.get() == 0 );
      assert( !m_str.bof() );
    }
    assert( m_str.get_pos(m_str.tell_prev()) ==
    	    rowcol(m_copy.q_row_upb(),m_copy.q_col_upb()) );
    verify_element_value(m_copy,0);
    assert( !(m_copy == m) );
    
    cout << "\tCopying the matrices via streams" << endl;
    m_str.seek(m.q_no_elems(),LAS::end);
	assert( m_str.bof() );
    for(LAStreamIn m_in(m); !m_in.eof(); )
      m_str.get() = m_in.get();
    assert( m_str.eof() );
    assert( m_copy == m );
    cout << "\tChecking various seeks" << endl;
    m_str.seek(-m.q_no_elems());
    assert( !m_str.eof() );
    assert( m_str.bof() );
    assert( m_str.peek() == pattern + incr );
    assert( m_str.peek() == pattern + incr );
    assert( m_str.get() == pattern + incr );
    assert( m_str.get() == pattern + incr+incr );
	m_str.seek(-1,LAS::cur);
    assert( m_str.get() == pattern + incr+incr );
	assert( !m_str.bof() );
	m_str.seek(m.q_no_elems()-1,LAS::beg);
    assert( m_str.peek() == pattern + m.q_no_elems()*incr );
	m_str.ignore(1);
	assert( m_str.eof() );
	m_str.seek(1,LAS::end);
    assert( m_str.get() == pattern + m.q_no_elems()*incr );
	assert( m_str.eof() );
	m_str.seek(0,LAS::beg);
    assert( m_str.peek() == pattern + incr );
	m_str.seek(m.q_no_elems(),LAS::cur);
	assert( m_str.eof() );
  } 
  cout << "\nDone\n";
}

static void test_LAStrideStreams(const int rsize, const int csize,
				 const int stride)
{
  const double pattern = 4.25;
  const double incr = 1.5;
  cout << "\n---> Test LAStrideStreams over " << rsize << 'x' << csize
  	   << " matrix with a stride of " << stride << endl;
  
  Matrix m(-1,rsize-2,2,csize+1);
  {
    cout << "Fill out the matrix in the natural order via ElementWiseAction..." 
         << endl;
    struct filler : public ElementWiseAction
    {
      double counter;
      const double incr;
      void operator () (REAL& element) { element = counter; counter += incr; }
      filler(const double _init_val, const double _incr) :
      	counter(_init_val), incr(_incr) {}
    };
    to_every(m).apply(filler(pattern,incr));
    assert( of_every(m).max_abs() == pattern + incr*(m.q_no_elems()-1) );
    assert( abs( of_every(m).sum() - 
    	         (2*pattern+incr*(m.q_no_elems()-1))/2*m.q_no_elems() )
    	         <= FLT_EPSILON );
  }

  {
    cout << "Check the filling order via LAStrideStreamIn" << endl;
    LAStrideStreamIn m_str(m,stride);
    assert( !m_str.eof() );
    assert( m_str.bof() );
    double counter = pattern;
    const double stride_incr = incr * stride;
    AREALMark mark = m_str.tell();
    assert( (bool)mark );
    assert( m_str.get_pos(mark) == 
    	    rowcol(m.q_row_lwb(),m.q_col_lwb()) );
    register int stride_counter = 0;
    while( !m_str.eof() )
    {
      assert( m_str.peek() == counter );
      assert( m_str.get() == counter );
      assert( !m_str.bof() );
      counter += stride_incr;
      stride_counter++;
    }
    cout << "\tstrides made: " << stride_counter << endl;
    assert( stride_counter == (m.q_no_elems() + stride -1) / stride );
    assert( stride != 1 || m_str.get_pos(m_str.tell_prev()) ==
    	    rowcol(m.q_row_upb(),m.q_col_upb()) );
    m_str.seek(mark);
    assert( !m_str.eof() );
    assert( m_str.bof() );
    assert( m_str.peek() == pattern );
    assert( m_str.get() == pattern );
  }
  
  {
    cout << "Doubling every " << stride << " element " << endl;
    LAStrideStreamOut m_str(m,stride);
    assert( !m_str.eof() );
    assert( m_str.bof() );
    double counter = pattern;
    const double stride_incr = incr * stride;
    AREALMark mark = m_str.tell();
    assert( (bool)mark );
    assert( m_str.get_pos(mark) == 
    	    rowcol(m.q_row_lwb(),m.q_col_lwb()) );
    while( !m_str.eof() )
    {
      assert( m_str.peek() == counter );
      m_str.get() += counter;
      assert( !m_str.bof() );
      counter += stride_incr;
    }
    cout << "\tand checking it out..." << endl;
    register int stride_counter = 0;
    LAStreamIn m_in(m);
    counter = pattern;
    for(register int i=0; !m_in.eof(); )
    {
      assert( m_in.get() == (i==0 ? stride_counter++, 2*counter : counter) );
      counter += incr;
      if( ++i == stride )
        i = 0;
    }
    cout << "\tstrides made: " << stride_counter << endl;
    assert( stride_counter == (m.q_no_elems() + stride -1) / stride );
    assert( stride != 1 || m_str.get_pos(m_str.tell_prev()) ==
    	    rowcol(m.q_row_upb(),m.q_col_upb()) );
	{
	   cout << "\tchecking it out using regular stream with ignore..." << endl;
	   register int new_stride_counter = 0;
	   LAStreamIn m_in(m);
	   counter = pattern;
	   while( !m_in.eof() )
	   {
		 assert( m_in.get() == (new_stride_counter++, 2*counter) );
		 m_in.ignore(stride-1);
		 counter += stride*incr;
	   }
	   cout << "\tstrides made: " << new_stride_counter << endl;
	   assert( new_stride_counter == stride_counter );
	}
    cout << "\tChecking various seeks" << endl;
    m_str.seek(-stride_counter);
    assert( !m_str.eof() );
    assert( m_str.bof() );
    assert( m_str.peek() == 2*pattern );
    assert( m_str.get() == 2*pattern );
	if( stride_counter == 1 )	// The stride was way too big
	{
  	  m_str.seek(-1,LAS::cur);
	  assert( m_str.get() == 2*pattern );
	}
	else
	{
  	  assert( m_str.get() == 2*(pattern + stride*incr) );
	  m_str.seek(-1,LAS::cur);
      assert( m_str.get() == 2*(pattern + stride*incr) );
	}
	assert( !m_str.bof() );
	m_str.seek(stride_counter-1,LAS::beg);
    assert( m_str.peek() == 2*(pattern + stride*(stride_counter-1)*incr) );
	m_str.ignore(1);
	assert( m_str.eof() );
	m_str.seek(1,LAS::end);
    assert( m_str.get() == 2*(pattern + stride*(stride_counter-1)*incr) );
	assert( m_str.eof() );
	m_str.seek(0,LAS::beg);
    assert( m_str.peek() == 2*pattern );
	m_str.seek(m.q_no_elems(),LAS::cur);
	assert( m_str.eof() );
  }
  
  cout << "\nDone\n";
}


		// Make sure the stream represents an Arithmetic Progression
		// [first_el,next_to_last_el) with a stride incr
static void verify_ap_stream(AREALStreamIn& stream,
			  const double first_el, const double next_to_last_el,
			  const double incr)
{
  double ideal_val = first_el;
  for(; !stream.eof(); ideal_val += incr)
  {
    const REAL stream_val = stream.get();
    if( abs(stream_val - ideal_val) >= FLT_EPSILON )
      _error("verify_ap_stream: a value taken from the stream %g is different "
	     "from the expected %g\n",stream_val,ideal_val);
  }
  if( abs(ideal_val - next_to_last_el) >= FLT_EPSILON )
    _error("verify_ap_stream: the stream ended at %g while %g was expected",
	   ideal_val,next_to_last_el);
}

static void verify_ap_stream(AREALStrideStreamIn& stream,
			  const double first_el, const double next_to_last_el,
			  const double incr)
{
  double ideal_val = first_el;
  for(; !stream.eof(); ideal_val += incr)
  {
    const REAL stream_val = stream.get();
    if( abs(stream_val - ideal_val) >= FLT_EPSILON )
      _error("verify_ap_stream: a value taken from the stream %g is different "
	     "from the expected %g\n",stream_val,ideal_val);
  }
  if( abs(ideal_val - next_to_last_el) >= FLT_EPSILON )
    _error("verify_ap_stream: the stream ended at %g while %g was expected",
	   ideal_val,next_to_last_el);
}


static void test_subranging(const int size)
{
  const double pattern = 4.25;
  const double incr = 1.5;
  cout << "\n---> Test subranging of LAStreams of a " << size
  	   << " -element vector " << endl;
  
  Vector v(-1,size-2);
  {				// Make a test pattern
    struct filler : public ElementWiseAction
    {
      double counter;
      const double incr;
      void operator () (REAL& element) { element = counter; counter += incr; }
      filler(const double _init_val, const double _incr) :
      	counter(_init_val), incr(_incr) {}
    };
    to_every(v).apply(filler(pattern,incr));
    assert( of_every(v) >= pattern );
  }
  
  {
    cout << "\tchecking the verify_ap_stream routine..." << endl;
    LAStreamIn str(v);
    verify_ap_stream(str,pattern,size*incr+pattern,incr);
    assert( str.eof() );
    str.rewind();
    verify_ap_stream(str,pattern,size*incr+pattern,incr);
    LAStreamOut o_str(v);
    verify_ap_stream(o_str,pattern,size*incr+pattern,incr);
    o_str.rewind(); o_str.ignore(1);
    verify_ap_stream(o_str,pattern+incr,size*incr+pattern,incr);
  }
  
  {
    LAStreamIn str(v);
    IRange range_all_total = IRange::from(-IRange::INF);
    cout << "\tchecking subranging of an LAStream by " << range_all_total << endl;
    assert( !range_all_total.q_proper() );
    assert( !range_all_total.q_empty() );
    verify_ap_stream(LAStreamIn(str,range_all_total,LAS::beg),
		     pattern,size*incr+pattern,incr);
    assert( str.bof() );
    verify_ap_stream(LAStreamIn(str,range_all_total,LAS::end),
		     pattern,size*incr+pattern,incr);
    verify_ap_stream(LAStreamIn(str,range_all_total,LAS::cur),
		     pattern,size*incr+pattern,incr);
    LAStreamOut o_str(v);
    IRange range_all1 = IRange::through(IRange::INF);
    verify_ap_stream(LAStreamIn(o_str,range_all1,LAS::cur),
		     pattern,size*incr+pattern,incr);
    assert( o_str.bof() );
    o_str.ignore(1);
    verify_ap_stream(LAStreamOut(o_str,range_all1,LAS::beg),
		     pattern,size*incr+pattern,incr);
  }
  {
    LAStreamIn str(v);
    IRange range(-size-1,size+1);
    cout << "\tchecking subranging of an LAStreamIn by " << range << endl;
    assert( range.q_proper() );
    assert( !range.q_empty() );
    verify_ap_stream(LAStreamIn(str,range,LAS::beg),
		     pattern,size*incr+pattern,incr);
    assert( str.bof() );
    str.ignore(1);
    verify_ap_stream(LAStreamIn(str,range),
		     pattern,size*incr+pattern,incr);
  }
  {
    LAStreamOut str(v);
    IRange range(0,size-1);
    cout << "\tchecking subranging of an LAStreamOut by " << range << endl;
    assert( range.q_proper() );
    assert( !range.q_empty() );
    verify_ap_stream(LAStreamIn(str,range,LAS::beg),
		     pattern,size*incr+pattern,incr);
    assert( str.bof() );
    verify_ap_stream(LAStreamOut(str,range,LAS::end),
		     pattern,pattern,incr);
    str.ignore(1);
    verify_ap_stream(LAStreamOut(str,range,LAS::cur),
		     pattern+incr,size*incr+pattern,incr);
  }
  {
    LAStreamIn str(v);
    IRange range(0,0);
    cout << "\tchecking subranging of an LAStreamIn by " << range << endl;
    assert( range.q_proper() );
    assert( !range.q_empty() );
    verify_ap_stream(LAStreamIn(str,range,LAS::beg),
		     pattern,incr+pattern,incr);
    assert( str.bof() );
    verify_ap_stream(LAStreamIn(str,range,LAS::end),
		     pattern,pattern,incr);
    str.ignore(1);
    verify_ap_stream(LAStreamIn(str,range,LAS::cur),
		     pattern+incr,2*incr+pattern,incr);
  }
  {
    LAStreamIn str(v);
    IRange range(1,1);
    cout << "\tchecking subranging of an LAStreamIn by " << range << endl;
    assert( range.q_proper() );
    assert( !range.q_empty() );
    verify_ap_stream(LAStreamIn(str,range,LAS::beg),
		     incr+pattern,2*incr+pattern,incr);
    assert( str.bof() );
    verify_ap_stream(LAStreamIn(str,range,LAS::end),
		     (size-1)*incr+pattern,size*incr+pattern,incr);
    str.ignore(1);
    verify_ap_stream(LAStreamIn(str,range,LAS::cur),
		     pattern+2*incr,3*incr+pattern,incr);
  }
  {
    LAStreamIn str(v);
    IRange range(-2,-2);
    cout << "\tchecking subranging of an LAStreamIn by " << range << endl;
    assert( range.q_proper() );
    assert( !range.q_empty() );
    str.ignore(3);
    verify_ap_stream(LAStreamIn(str,range,LAS::cur),
		     pattern+incr,2*incr+pattern,incr);
  }
  {
    LAStreamIn str(v);
    IRange range(1,0);
    cout << "\tchecking subranging of an LAStreamIn by " << range << endl;
    assert( !range.q_proper() );
    assert( range.q_empty() );
    verify_ap_stream(LAStreamIn(str,range,LAS::beg),
		     pattern,pattern,incr);
    assert( str.bof() );
    verify_ap_stream(LAStreamIn(str,range,LAS::end),
		     (size-1)*incr+pattern,size*incr+pattern,incr);
    str.ignore(size-1);
    verify_ap_stream(LAStreamIn(str,range,LAS::cur),
		     pattern,pattern,incr);
  }
  {
    Vector v1 = v;
    assert( v1 == v );
    IRange range(1,2);
    cout << "\tchecking modifying via a subranged LAStreamOut by " << range << endl;
    assert( range.q_proper() );
    assert( !range.q_empty() );
    LAStreamOut str1(v1);
    LAStreamOut str2(str1,range,LAS::beg);
    str2.get() += 1;
    str2.get() += 1;
    assert( str2.eof() );
    verify_ap_stream(LAStreamIn(str1,range),
		     pattern+incr+1,3*incr+1+pattern,incr);
    v1(v1.q_lwb()+1) -= 1;
    v1(v1.q_lwb()+2) -= 1;
    assert( v1 == v );
    verify_ap_stream(str1,
		     pattern,size*incr+pattern,incr);
  }
  cout << "\nDone\n";
}

static void test_LAStride_subranging(const int size, const int stride)
{
  const double pattern = 4.25;
  const double incr = 1.5;
  const int nsteps = (size+stride-1)/stride;
  cout << "\n---> Test subranging of LAStrideStream of a " << size
  	   << " -element vector with a stride of " << stride << endl;
  
  Vector v(2,size+1);
  {				// Make a test pattern
    struct filler : public ElementWiseAction
    {
      double counter;
      const double incr;
      void operator () (REAL& element) { element = counter; counter += incr; }
      filler(const double _init_val, const double _incr) :
      	counter(_init_val), incr(_incr) {}
    };
    to_every(v).apply(filler(pattern,incr));
    assert( of_every(v) >= pattern );
  }
  
  {
    LAStrideStreamIn str(v,stride);
    IRange range_all_total = IRange::from(-IRange::INF);
    cout << "\tchecking subranging of an LAStrideStream by " << range_all_total << endl;
    assert( !range_all_total.q_proper() );
    assert( !range_all_total.q_empty() );
    verify_ap_stream(LAStrideStreamIn(str,range_all_total,LAS::beg),
		     pattern,nsteps*stride*incr+pattern,incr*stride);
    assert( str.bof() );
    verify_ap_stream(LAStrideStreamIn(str,range_all_total,LAS::end),
		     pattern,nsteps*stride*incr+pattern,incr*stride);
    verify_ap_stream(LAStrideStreamIn(str,range_all_total,LAS::cur),
		     pattern,nsteps*stride*incr+pattern,incr*stride);
    LAStrideStreamOut o_str(v,stride);
    IRange range_all1 = IRange::through(IRange::INF);
    verify_ap_stream(LAStrideStreamIn(o_str,range_all1,LAS::cur),
		     pattern,nsteps*stride*incr+pattern,incr*stride);
    assert( o_str.bof() );
    o_str.ignore(1);
    verify_ap_stream(LAStrideStreamOut(o_str,range_all1,LAS::beg),
		     pattern,nsteps*stride*incr+pattern,incr*stride);
  }
  {
    LAStrideStreamIn str(v,stride);
    IRange range(-size-1,size+1);
    cout << "\tchecking subranging of an LAStrideStreamIn by " << range << endl;
    assert( range.q_proper() );
    assert( !range.q_empty() );
    verify_ap_stream(LAStrideStreamIn(str,range,LAS::beg),
		     pattern,nsteps*stride*incr+pattern,incr*stride);
    assert( str.bof() );
    str.ignore(1);
    verify_ap_stream(LAStrideStreamIn(str,range),
		     pattern,nsteps*stride*incr+pattern,incr*stride);
  }
  {
    LAStrideStreamOut str(v,stride);
    IRange range(0,nsteps-1);
    cout << "\tchecking subranging of an LAStrideStreamOut by " << range << endl;
    assert( range.q_proper() );
    assert( !range.q_empty() );
    verify_ap_stream(LAStrideStreamIn(str,range,LAS::beg),
		     pattern,nsteps*incr*stride+pattern,incr*stride);
    assert( str.bof() );
    //verify_ap_stream(LAStrideStreamOut(str,range,LAS::end),
    //		     pattern,pattern,incr*stride);
    str.ignore(1);
    if( !str.eof() )
      verify_ap_stream(LAStrideStreamOut(str,range,LAS::cur),
		     pattern+incr*stride,nsteps*incr*stride+pattern,incr*stride);
  }
  {
    LAStrideStreamIn str(v,stride);
    IRange range(0,0);
    cout << "\tchecking subranging of an LAStrideStreamIn by " << range << endl;
    assert( range.q_proper() );
    assert( !range.q_empty() );
    verify_ap_stream(LAStrideStreamIn(str,range,LAS::beg),
		     pattern,incr*stride+pattern,incr*stride);
    assert( str.bof() );
    //verify_ap_stream(LAStrideStreamIn(str,range,LAS::end),
    //		     pattern,pattern,incr*stride);
    str.ignore(1);
    if( !str.eof() )
      verify_ap_stream(LAStrideStreamIn(str,range,LAS::cur),
		     pattern+incr*stride,2*incr*stride+pattern,incr*stride);
  }
  if( nsteps > 1 )
  {
    LAStrideStreamIn str(v,stride);
    IRange range(1,1);
    cout << "\tchecking subranging of an LAStrideStreamIn by " << range << endl;
    assert( range.q_proper() );
    assert( !range.q_empty() );
    verify_ap_stream(LAStrideStreamIn(str,range,LAS::beg),
		     incr*stride+pattern,2*incr*stride+pattern,incr*stride);
    assert( str.bof() );
    verify_ap_stream(LAStrideStreamIn(str,range,LAS::end),
		     (nsteps-1)*incr*stride+pattern,
		     nsteps*incr*stride+pattern,incr*stride);
    str.ignore(1);
    if( !str.eof() )
      verify_ap_stream(LAStrideStreamIn(str,range,LAS::cur),
		     pattern+2*incr*stride,3*incr*stride+pattern,incr*stride);
  }
  {
    LAStrideStreamIn str(v,stride);
    IRange range(-2,-2);
    cout << "\tchecking subranging of an LAStrideStreamIn by " << range << endl;
    assert( range.q_proper() );
    assert( !range.q_empty() );
    str.ignore(3);
    if( !str.eof() )
      verify_ap_stream(LAStrideStreamIn(str,range,LAS::cur),
		     pattern+incr*stride,2*incr*stride+pattern,incr*stride);
  }
  {
    LAStrideStreamIn str(v,stride);
    IRange range(1,0);
    cout << "\tchecking subranging of an LAStrideStreamIn by " << range << endl;
    assert( !range.q_proper() );
    assert( range.q_empty() );
    if( nsteps > 1 )
      verify_ap_stream(LAStrideStreamIn(str,range,LAS::beg),
		     pattern,pattern,incr*stride);
    assert( str.bof() );
    verify_ap_stream(LAStrideStreamIn(str,range,LAS::end),
		     (nsteps-1)*incr*stride+pattern,
		     nsteps*incr*stride+pattern,incr*stride);
    if( nsteps > 2 )
      str.ignore(nsteps-2),
      verify_ap_stream(LAStrideStreamIn(str,range,LAS::cur),
		     pattern,pattern,incr*stride);
  }
  if( nsteps > 2 )
  {
    Vector v1 = v;
    assert( v1 == v );
    IRange range(1,2);
    cout << "\tchecking modifying via a subranged LAStrideStreamOut by " << range << endl;
    assert( range.q_proper() );
    assert( !range.q_empty() );
    LAStrideStreamOut str1(v1,stride);
    LAStrideStreamOut str2(str1,range,LAS::beg);
    str2.get() += 1;
    str2.get() += 1;
    assert( str2.eof() );
    verify_ap_stream(LAStrideStreamIn(str1,range),
		     pattern+incr*stride+1,3*incr*stride+1+pattern,incr*stride);
    v1(v1.q_lwb()+stride) -= 1;
    v1(v1.q_lwb()+2*stride) -= 1;
    assert( v1 == v );
    verify_ap_stream(str1,
		     pattern,nsteps*incr*stride+pattern,incr*stride);
  }
  cout << "\nDone\n";
}

		// Make sure the stream represents a _2D_ Arithmetic Progression
		// seed + i*incr_small + j*incr_big,
		// 	i=0..(nrows-1), j=0..(ncols-1)
static void verify_ap_stream(AREALBlockStreamIn& stream,
			  const DimSpec dims,
			  const double seed, const double incr_small,
			  const double incr_big)
{
  double col_val = seed;
  for(register int j=0; j<dims.q_ncols();  j++, col_val += incr_big)
  {
    double row_val = col_val;
    for(register int i=0; i<dims.q_nrows(); i++, row_val += incr_small)
    {
      if( stream.eof() )
	_error("Unexpected stream EOF at (%d,%d)\n",i,j);
      const REAL stream_val = stream.get();
      if( abs(stream_val - row_val) >= FLT_EPSILON )
        _error("verify_ap_stream: a value taken from the stream %g is different "
	     "from the expected %g, at (%d,%d)\n",
	       stream_val,row_val,i,j);
    }
  }
  if( !stream.eof() )
    _error("The stream was supposed to end after (%d,%d)",
	   dims.q_nrows(),dims.q_ncols());
}

static void test_LABlockStreams(const int nrows, const int ncols)
{
  const double pattern = 4.25;
  const double incr = 0.5;
  
  Matrix m(2,nrows+1,0,ncols-1);

  cout << "\n---> Test LABlockStreams for " << static_cast<DimSpec&>(m)
       << endl;
  
  {
    cout << "\tFilling out the matrix in the natural order..." << endl;
    LAStreamOut str(m);
    for( double val = pattern; !str.eof(); val += incr )
      str.get() = val;
  }
  {
    cout << "\tchecking accessing of the whole matrix as a block stream..." << endl;
    LABlockStreamIn str(m,IRange::from(-IRange::INF),
		          IRange::from(-IRange::INF));
    //str.dump(cerr);
    assert( str.peek() == pattern );
    verify_ap_stream(str,m,pattern,incr,incr*nrows);
    assert( str.eof() );
    str.rewind();
    assert( str.bof() && !str.eof() );
    verify_ap_stream(str,m,pattern,incr,incr*nrows);
    str.seek(0,LAS::beg);
    for(register int i=0; i<m.q_nrows(); i++)
      str.get();
    AREALMark mark = str.tell();
    verify_ap_stream(str,DimSpec(m.q_nrows(),m.q_ncols()-1),
		     pattern+incr*nrows,incr,incr*nrows);
    str.seek(mark);
    verify_ap_stream(str,DimSpec(m.q_nrows(),m.q_ncols()-1),
		     pattern+incr*nrows,incr,incr*nrows);
    str.seek(m.q_nrows(),LAS::beg);
    AREALMark mark1 = str.tell_prev();
    str.get(); str.seek(m.q_nrows()-1);
    verify_ap_stream(str,DimSpec(m.q_nrows(),m.q_ncols()-2),
		     pattern+2*incr*nrows,incr,incr*nrows);
    str.seek(mark1); str.get();
    verify_ap_stream(str,DimSpec(m.q_nrows(),m.q_ncols()-1),
		     pattern+incr*nrows,incr,incr*nrows);
    str.seek(m.q_nrows() * (m.q_ncols() - 3),LAS::end);
    verify_ap_stream(str,DimSpec(m.q_nrows(),m.q_ncols()-3),
		     pattern+3*incr*nrows,incr,incr*nrows);
  }
  {
    cout << "\tchecking modifying the whole matrix as a block stream..." << endl;
    Matrix m1 = m;
    LABlockStreamOut str(m1,IRange::from(-IRange::INF),
		          IRange::from(-IRange::INF));
    //str.dump(cerr);
    assert( str.get() == pattern );
    AREALMark bos = str.tell_prev();
    str.seek(nrows*ncols);
    assert( str.eof() );
    str.seek(nrows*ncols-1,LAS::beg);
    assert( abs(str.get() - (pattern + incr*(nrows*ncols-1))) <= FLT_EPSILON );
    assert( str.eof() );
    str.seek(bos);
    assert( str.bof() );
    verify_ap_stream(str,m,pattern,incr,incr*nrows);
    str.rewind();
    while( !str.eof() )
      str.get() -= 1;
    str.rewind();
    verify_ap_stream(str,m,pattern-1,incr,incr*nrows);
    str.rewind();
    while( !str.eof() )
      str.peek() -= 1, str.ignore(1);
    str.rewind();
    verify_ap_stream(str,m,pattern-2,incr,incr*nrows);
    to_every(m1) += 2;
    verify_matrix_identity(m,m1);
  }
  {			// The first column of m
    IRange row_range = IRange::through(IRange::INF);
    IRange col_range = IRange::through(m.q_col_lwb());
    cout << "\tchecking LABlockStreams clipped as " << row_range
    	 << " x " << col_range << endl;
    LABlockStreamIn str(m,row_range,col_range);
    verify_ap_stream(str,DimSpec(nrows,1),pattern,incr,incr*nrows); 
  }
  {			// The second column of m
    IRange row_range = IRange::from(m.q_row_lwb());
    IRange col_range(m.q_col_lwb()+1,m.q_col_lwb()+1);
    cout << "\tchecking LABlockStreams clipped as " << row_range
    	 << " x " << col_range << endl;
    LABlockStreamIn str(m,row_range,col_range);
    verify_ap_stream(str,DimSpec(nrows,1),pattern+incr*nrows,incr,incr*nrows); 
  }
  {			// A part of the last column of m
    IRange row_range(m.q_row_lwb()+1,m.q_row_upb());
    IRange col_range = IRange::from(m.q_col_upb());
    cout << "\tchecking LABlockStreams clipped as " << row_range
    	 << " x " << col_range << endl;
    LABlockStreamIn str(m,row_range,col_range);
    verify_ap_stream(str,DimSpec(nrows-1,1),
		     pattern+incr*nrows*(ncols-1)+incr,incr,incr*nrows);
  }
  {			// The first row of m
    IRange row_range = IRange::through(m.q_row_lwb());
    IRange col_range = IRange::from(m.q_col_lwb());
    cout << "\tchecking LABlockStreams clipped as " << row_range
    	 << " x " << col_range << endl;
    LABlockStreamIn str(m,row_range,col_range);
    verify_ap_stream(str,DimSpec(1,ncols),pattern,incr,incr*nrows); 
  }
  {			// The second row of m
    IRange row_range(m.q_row_lwb()+1,m.q_row_lwb()+1);
    IRange col_range = IRange::through(m.q_col_upb());
    cout << "\tchecking LABlockStreams clipped as " << row_range
    	 << " x " << col_range << endl;
    LABlockStreamIn str(m,row_range,col_range);
    verify_ap_stream(str,DimSpec(1,ncols),pattern+incr,incr,incr*nrows); 
  }
  {			// A part of the last row of m
    IRange row_range = IRange::from(m.q_row_upb());
    IRange col_range = IRange::through(m.q_col_upb()-1);
    cout << "\tchecking LABlockStreams clipped as " << row_range
    	 << " x " << col_range << endl;
    LABlockStreamIn str(m,row_range,col_range);
    verify_ap_stream(str,DimSpec(1,ncols-1),
		     pattern+(nrows-1)*incr,incr,incr*nrows);
  }
  {			// The first matrix element
    IRange row_range(m.q_row_lwb(),m.q_row_lwb());
    IRange col_range(m.q_col_lwb(),m.q_col_lwb());
    cout << "\tchecking LABlockStreams clipped as " << row_range
    	 << " x " << col_range << endl;
    LABlockStreamIn str(m,row_range,col_range);
    verify_ap_stream(str,DimSpec(1,1),pattern,incr,incr*nrows);
  }
  {			// The last matrix element
    IRange row_range = IRange::from(m.q_row_upb());
    IRange col_range = IRange::from(m.q_col_upb());
    cout << "\tchecking LABlockStreams clipped as " << row_range
    	 << " x " << col_range << endl;
    LABlockStreamIn str(m,row_range,col_range);
    verify_ap_stream(str,DimSpec(1,1),
		     pattern+(nrows*ncols-1)*incr,incr,incr*nrows);
  }
  {			// A 2x3 block at the upper-right corner
    IRange row_range(m.q_row_lwb()+1,m.q_row_lwb()+2);
    IRange col_range = IRange::from(m.q_col_upb()-2);
    cout << "\tchecking LABlockStreams clipped as " << row_range
    	 << " x " << col_range << endl;
    LABlockStreamIn str(m,row_range,col_range);
    verify_ap_stream(str,DimSpec(2,3),
		     pattern+(ncols-3)*nrows*incr+incr,incr,incr*nrows);
  }
  {			// A 3x2 block at the lower-left corner
    IRange row_range(m.q_row_upb()-3,m.q_row_upb()-1);
    IRange col_range(m.q_col_lwb()+2,m.q_col_lwb()+3);
    cout << "\tchecking LABlockStreams clipped as " << row_range
    	 << " x " << col_range << endl;
    LABlockStreamIn str(m,row_range,col_range);
    verify_ap_stream(str,DimSpec(3,2),
		     pattern+2*nrows*incr+(nrows-4)*incr,incr,incr*nrows);
  }
  {			// Modifying the [2,3] matrix element
    IRange row_range(m.q_row_lwb()+2,m.q_row_lwb()+2);
    IRange col_range(m.q_col_lwb()+3,m.q_col_lwb()+3);
    cout << "\tchecking modifying LABlockStreams clipped as " << row_range
    	 << " x " << col_range << endl;
    Matrix m1 = m;
    LABlockStreamOut str(m1,row_range,col_range);
    str.get() += 1;
    assert( str.eof() );
    assert( !( m1 == m ) );
    (MatrixColumn(m1,col_range.lwb))(row_range.upb) -= 1;
    verify_matrix_identity(m,m1);
  }
  cout << "\nDone\n";
}

/*
 *------------------------------------------------------------------------
 *				Root module
 */

main()
{
  cout << "\n\n" << _Minuses << 
          "\n\t\tVerify streams over matrices and vectors\n";

  test_LAStreams(11,21);
  test_LAStrideStreams(10,20,1);
  test_LAStrideStreams(20,10,2);
  test_LAStrideStreams(20,11,3);
  test_LAStrideStreams(20,11,11);
  test_LAStrideStreams(20,11,20*11);
  
  test_subranging(10);
  test_LAStride_subranging(10,1);
  test_LAStride_subranging(10,2);
  test_LAStride_subranging(10,3);
  test_LAStride_subranging(10,11);
  
  test_LABlockStreams(11,21);
  cout << "\n\nAll test passed" << endl;
}

