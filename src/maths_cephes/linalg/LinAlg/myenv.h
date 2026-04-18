// This may look like C code, but it is really -*- C++ -*-
// ************************************************************************
//
//			A standard environment
//			  I am accustomed to
//
// $Id: myenv.h,v 2.2 1998/12/20 23:34:06 oleg Exp oleg $

#ifndef __GNUC__
#pragma once
#endif
#ifndef _myenv_h
#define _myenv_h

#ifdef __GNUC__
#pragma interface
#endif

#include <stdio.h>


//------------------------------------------------------------------------
//		    Patches to the standard environment


#if 0 // uncomment if the compiler sucks...
typedef int bool;
#define false 0
#define true (!false)
//enum bool {false,true};
#endif

                                // Like strncpy(), but ALWAYS terminates
                                // the destination string
char * xstrncpy(char * dest, const char * src, const int len);

inline long int sqr(const int x)		{ return x*x; }

double       pow(double, long);
double       pow(long, long);

		// Works just as a regular getenv:
		// searches the process' environment for a string of the form
		// name=value and, if the string is
		// present, returns a pointer to the 'value' part of the
		// string.
		// If the string is not present, the default_value is
		// returned, unless it is nil.
		// If the default_value was nil and the string wasn't found,
		// the function prints the message that the name wasn't
		// found, and aborts the program

const char * xgetenv(const char * name, const char * default_value);

				// Return TRUE if string s1 starts with s2
				// i.e., s2 is a prefix of s1
				// Case doesn't matter
bool does_start_with_ci(const char * s1, const char * s2);


		// Obtain the size of an existing file
		// If you don't want the function to abort when
		// the file wasn't found/couldn't be accessed,
		// supply your own default action by subclassing from
		// GFS_Default and overriding 'operator ()'
extern struct GFS_Default { virtual size_t operator () (const char * file_name); }
	GFS_default;
size_t get_file_size(const char * file_name,
		     GFS_Default& on_error = GFS_default);

				// libg++ nifty timing functions
				// return_elapsed_time(Last_time) returns
				// process time (in secs) since Last_Time
				// If Last_time == 0.0, return time since
				// the last call to start_timer()
#if !defined(__GNUC__) || ((__GNUC__ == 2) && (__GNUC_MINOR__ > 7))
double start_timer(void);
double return_elapsed_time(const double Last_Time);
#endif


//------------------------------------------------------------------------
//	Assertions, aborts, and related stderr printing

				// Print an error message on stderr and
				// abort, arguments like those of printf()
void _error(const char * message,...);

				// Print a message on stderr
				// It looks and acts like printf(), only
				// prints on stderr (which is unbuffered...)
void message(const char * message,...);

#ifndef assert
#define assert(ex) \
        (void)((ex) ? 1 : \
              (_error("Failed assertion " #ex " at line %d of `%s'.\n", \
               __LINE__, __FILE__), 0))
#define assertval(ex) assert(ex)
#endif

#define assure(expr,message)				\
	if	(expr) {;}				\
	else _error("%s\n at line %d of '%s'.",message,__LINE__, __FILE__)


	
				// Strings of symbols
				// They may be used as a delimiting lines
extern const char _Minuses [];
extern const char _Asteriscs [];
extern const char _Equals [];


#endif
