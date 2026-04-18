// This may look like C code, but it is really -*- C++ -*-
/*
 ************************************************************************
 *			Service C++ functions 
 *	     that support the standard environment for me
 *
 * $Id: myenv.cc,v 2.3 1998/12/20 23:29:08 oleg Exp oleg $
 */

#if defined(__GNUC__)
#pragma implementation
//#pragma implementation "Logger.h"
#endif

#include "myenv.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <ctype.h>
#include <fstream.h>
extern "C"
{
#if defined(macintosh)
#include <types.h>
#include <stat.h>
#elif defined(WIN32)
#include <sys/types.h>
#include <sys/stat.h>
#define stat _stat
#define STDERR_FILENO 2
#else
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#endif
int stat(const char * path, struct stat * buf);
}
//#include "Logger.h"

/*
 *-----------------------------------------------------------------------
 *		Some global constant pertaining to input/output
 */

const char _Minuses [] = "\
-------------------------------------------------------------------------------";

const char _Asteriscs [] = "\
*******************************************************************************";

const char _Equals [] = "\
===============================================================================";

//------------------------------------------------------------------------
//				A better getenv
//
// It works just as a regular getenv: searches the process' environment for
// a string of the form name=value and, if the string is present, returns
// a pointer to the 'value' part of it.
// If the string is not present, the default_value is returned, unless it
// is nil.
// If the default_value was nil and the string wasn't found,
// the function prints the message that the name wasn't
// found, and aborts the program

const char * xgetenv(const char * name, const char * default_value)
{
  const char * env_value = ::getenv(name);
  if( env_value != 0 )
    return env_value;
  else if( default_value != 0 )
    return default_value;
  else
    return _error("xgetenv: env variable '%s' wasn't found, but was required",
    	          name), (const char *)0;
}



/*
 *------------------------------------------------------------------------
 *	        Print an error message at stderr and abort
 * Synopsis
 *	volatile void _error(const char * message,... );
 *	Message may contain format control sequences %x. Items to print 
 *	with the control sequences are to be passed as additional arguments to
 *	the function call.
 */

void _error(const char * message,...)
{
  va_list args;
  va_start(args,message);		/* Init 'args' to the beginning of */
					/* the variable length list of args*/
  fprintf(stderr,"\n_error:\n"); 	
  vfprintf(stderr,message,args);
  fputs("\n",stderr);
#ifdef __MWERKS__
  exit(4);
#else  
  abort();
#endif
}


/*
 *------------------------------------------------------------------------
 *	       		 Print a message at stderr
 * Synopsis
 *	void message(const char * text,... );
 * It looks and acts like printf(), only prints on stderr
 * (which is usually unbuffered...)
 */

void message(const char * text,...)
{
  va_list args;
  va_start(args,text);		/* Init 'args' to the beginning of */
					/* the variable length list of args*/
  vfprintf(stderr,text,args);
}

//------------------------------------------------------------------------
//			A logging service
//
// It filters and logs various system activities onto stderr
// (unless it is set to log to something different)

#if 0
ostream_withassign Logger::log_stream(new filebuf(STDERR_FILENO));

  			// Set the log to append to a specified file
void Logger::set_log(const char log_file_name [])
{
  filebuf * const new_filebuf = new filebuf();
  if( new_filebuf->open(log_file_name,ios::out|ios::ate) == 0 )
      perror("Log file open error"),
      _error("Failed to open the log file '%s' because of the error above",
      	     log_file_name);
  log_stream = new_filebuf;		// set new streambuf and delete the old one
}
#endif


//------------------------------------------------------------------------
//			Obtaining the size of a file

		// Default action when the file wasn't found/can't be
		// accessed
size_t GFS_Default::operator () (const char * file_name)
{
  perror("getting file status");
  _error("Failed to get status of the file <%s> because of the error "
         "above",file_name);
  return (size_t)EOF;
}

GFS_Default GFS_default;

size_t get_file_size(const char * file_name, GFS_Default& on_error)
{
  struct stat file_status;
  if( stat(file_name,&file_status) != 0 )
    return on_error(file_name);
  return file_status.st_size;
}

//------------------------------------------------------------------------
//		    Patches to the standard environment

                                // Like strncpy(), but ALWAYS terminates
                                // the destination string
char * xstrncpy(char * dest, const char * src, const int len)
{
  strncpy(dest,src,len);
  dest[len] = '\0';
  return dest;
}

				// Convert char c to lower case
				// Unlike traditional tolower(), it
				// applies conversion only if c is a letter
				// in uppercase
static inline int to_lower(const char c)
{
  return isupper(c) ? (c - 'A') + 'a' : c;
}

				// Return TRUE if string s1 starts with s2
				// i.e., s2 is the prefix of s1
				// Case doesn't matter
bool does_start_with_ci(const char * s1, const char * s2)
{
  while( *s2 != '\0' )		// Alas, stricmp is not standard
    if( *s1 == '\0' )
      return false;			// s1 is shorter than s2
    else if( to_lower(*s1++) != to_lower(*s2++) )
      return false;
  return true;
}


		// Convenient pow functions that are often missing
		// They both compute x^y where y is an integer
		// This code is based on target.c of GNU's F77 code, and sets
		// the results in controversial cases as that code.
double pow(long x, long y)
{
  if( x == 0 )
    return 0;

  if( y == 0 )
    return 1;

  if( x == 1 )
    return 1;

  double multiplier = y > 0 ? x : (1.0/x);
  if( y < 0 )
    y = -y;

  while( (y & 1) == 0 )
  {
    multiplier *= multiplier;
    y >>= 1;
  }

  double accum = multiplier;
  y >>= 1;

  while( y != 0 )
  {
    multiplier *= multiplier;
    if( (y & 1) == 1)
	accum *= multiplier;
    y >>= 1;
  }

  return accum;
}

double pow(double x, long y)
{
  if( x == 0 )
    return 0;

  if( y == 0 )
    return 1;

  if( x == 1 )
    return 1;

  if( y < 0 )
    y = -y, x = 1.0/x;

  while( (y & 1) == 0 )
  {
    x *= x;
    y >>= 1;
  }

  double accum = x;
  y >>= 1;

  while( y != 0 )
  {
    x *= x;
    if( (y & 1) == 1)
	accum *= x;
    y >>= 1;
  }

  return accum;
}

#if !defined(__GNUC__) || ((__GNUC__ == 2) && (__GNUC_MINOR__ > 7))
#include <time.h>

				// libg++ nifty timing functions
				// Very rough and dirty implementation for
				// platforms w/o libg++
static time_t time_set;

double start_timer(void)
{
  return time_set = time(0);
}
				// return_elapsed_time(last_time) returns
				// process time (in secs) since Last_Time
				// If Last_time == 0.0, return time since
				// the last call to start_timer()
double return_elapsed_time(const double last_time)
{
  time_t new_time = time(0);
  if( time_set == 0 )
    return -1;				// timer wasn't started
  return new_time - (last_time == 0.0 ? time_set : last_time);
}

#endif
