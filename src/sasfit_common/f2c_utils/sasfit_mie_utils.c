/*
 * src/sasfit_common/f2c_utils/sasfit_mie_utils.c
 *
 * Copyright (c) 2008-2009, Paul Scherrer Institute (PSI)
 *
 * This file is part of SASfit.
 *
 * SASfit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SASfit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SASfit.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 *   Ingo Bressler (ingo@cs.tu-berlin.de)
 */

/* ErrPack.f -- translated by f2c (version 20021022).
   You must link the resulting object file with the libraries:
	-lf2c -lm   (in that order)
*/

#include "../include/sasfit_utils_f2c.h"

/* Table of constant values */

static integer c__1 = 1;
static logical c_true = TRUE_;

int sasfit_f2c_errmsg_(char *messag, logical *fatal, ftnlen messag_len)
{
    /* Initialized data */

    static integer nummsg = 0;
    static integer maxmsg = 100;
    static logical msglim = FALSE_;

    /* Format strings */
    static char fmt_9000[] = "(/,/,\002 ****** TOO MANY WARNING MESSAGES -"
	    "-  \002,\002They will no longer be printed *******\002,/,/)";

    /* Builtin functions */
    /*
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);
    int s_stop(char *, ftnlen);
    */

    /* Fortran I/O blocks */
    static cilist io___4 = { 0, 6, 0, "(//,2A,//)", 0 };
    static cilist io___5 = { 0, 6, 0, "(/,2A,/)", 0 };
    static cilist io___6 = { 0, 6, 0, fmt_9000, 0 };


/*        Print out a warning or error message;  abort if error */
/*        after making symbolic dump (machine-specific) */
/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Subroutines .. */
/* cccc EXTERNAL  SYMDUMP */
/*     .. */
    if (*fatal) {
	s_wsfe(&io___4);
	do_fio(&c__1, " ****** ERROR *****  ", (ftnlen)21);
	do_fio(&c__1, messag, messag_len);
	e_wsfe();
/*                                 ** Example symbolic dump call for Cray */
/* cccc    CALL SYMDUMP( '-B -c3' ) */
	s_stop("", (ftnlen)0);
    }
    ++nummsg;
    if (msglim) {
	return 0;
    }
    if (nummsg <= maxmsg) {
	s_wsfe(&io___5);
	do_fio(&c__1, " ****** WARNING *****  ", (ftnlen)23);
	do_fio(&c__1, messag, messag_len);
	e_wsfe();
    } else {
	s_wsfe(&io___6);
	e_wsfe();
	msglim = TRUE_;
    }
    return 0;
} /* errmsg_ */

logical sasfit_f2c_wrtbad_(char *varnam, ftnlen varnam_len)
{
    /* Initialized data */

    static integer nummsg = 0;
    static integer maxmsg = 50;

    /* System generated locals */
    logical ret_val;

    /* Builtin functions */
    /*
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);
    */

    /* Local variables */
    /*
    extern int errmsg_(char *, logical *, ftnlen);
    */

    /* Fortran I/O blocks */
    static cilist io___9 = { 0, 6, 0, "(3A)", 0 };


/*          Write names of erroneous variables and return 'TRUE' */
/*      INPUT :   VarNam = Name of erroneous variable to be written */
/*                         ( CHARACTER, any length ) */
/*     .. Scalar Arguments .. */
/*     .. */
/*     .. Local Scalars .. */
/*     .. */
/*     .. External Subroutines .. */
/*     .. */
    ret_val = TRUE_;
    ++nummsg;
    s_wsfe(&io___9);
    do_fio(&c__1, " ****  Input variable  ", (ftnlen)23);
    do_fio(&c__1, varnam, varnam_len);
    do_fio(&c__1, "  in error  ****", (ftnlen)16);
    e_wsfe();
    if (nummsg == maxmsg) {
	sasfit_f2c_errmsg_("Too many input errors.  Aborting...", &c_true, (ftnlen)35);
    }
    return ret_val;
} /* wrtbad_ */

logical sasfit_f2c_wrtdim_(char *dimnam, integer *minval, ftnlen dimnam_len)
{
    /* System generated locals */
    logical ret_val;

    /* Builtin functions */
    /*
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);
    */

    /* Fortran I/O blocks */
    static cilist io___10 = { 0, 6, 0, "(3A,I7)", 0 };


/*          Write name of too-small symbolic dimension and */
/*          the value it should be increased to;  return 'TRUE' */
/*      INPUT :  DimNam = Name of symbolic dimension which is too small */
/*                        ( CHARACTER, any length ) */
/*               Minval = Value to which that dimension should be */
/*                        increased (at least) */
/*     .. Scalar Arguments .. */
/*     .. */
    s_wsfe(&io___10);
    do_fio(&c__1, " ****  Symbolic dimension  ", (ftnlen)27);
    do_fio(&c__1, dimnam, dimnam_len);
    do_fio(&c__1, "  should be increased to at least ", (ftnlen)34);
    do_fio(&c__1, (char *)&(*minval), (ftnlen)sizeof(integer));
    e_wsfe();
    ret_val = TRUE_;
    return ret_val;
} /* wrtdim_ */

logical sasfit_f2c_tstbad_(char *varnam, real *relerr, ftnlen varnam_len)
{
    /* System generated locals */
    real r__1;
    logical ret_val;

    /* Builtin functions */
    /*
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);
    */

    /* Fortran I/O blocks */
    static cilist io___11 = { 0, 6, 0, "(/,3A,1P,E11.2,A)", 0 };


/*       Write name (VarNam) of variable failing self-test and its */
/*       percent error from the correct value;  return  'FALSE'. */
/*     .. Scalar Arguments .. */
/*     .. */
    ret_val = FALSE_;
    s_wsfe(&io___11);
    do_fio(&c__1, " *** Output variable ", (ftnlen)21);
    do_fio(&c__1, varnam, varnam_len);
    do_fio(&c__1, " differed by ", (ftnlen)13);
    r__1 = *relerr * 100.f;
    do_fio(&c__1, (char *)&r__1, (ftnlen)sizeof(real));
    do_fio(&c__1, " per cent from correct value.  Self-test failed.", (ftnlen)
	    48);
    e_wsfe();
    return ret_val;
} /* tstbad_ */

