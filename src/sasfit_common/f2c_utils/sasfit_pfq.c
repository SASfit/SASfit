// generalized hypergeometric Funtions

/* pfq.f -- translated by f2c (version 19970805).
   You must link the resulting object file with the libraries:
	-lf2c -lm   (in that order)
*/

#include <math.h>

#include <stdio.h>
#include <stdlib.h>

#include "sasfit.h"
#include "SASFIT_nr.h"
#include "sasfit_sq.h"
#include "sasfit_ff.h"
#include <gsl/gsl_sf.h>

#include "bool.h"
#include "tcl.h"
#undef float

#include "f2c.h"

unsigned short MAXNUM[4] = {0xffff,0xffff,0xffff,0x7fef};
unsigned short MACHEP[4] = {0x0000,0x0000,0x0000,0x3ca0};

float MAXNUMF = 1.7014117331926442990585209174225846272e38;
float MACHEPF = 5.9604644775390625E-8;

/* Common Block Declarations */

struct {
    doublereal zero, half, one, two, ten, eps;
} consts_1 = { 0., .5, 1., 2., 10., 1e-10 };

struct {
    integer nout;
} io_;

#define io_1 io_


/* Table of constant values */

static integer c__9 = 9;
static integer c__1 = 1;
static integer c__25000 = 25000;
static doublereal c_b65 = 10.;

/*     **************************************************************** */
/*     *                                                              * */
/*     *    SOLUTION TO THE GENERALIZED HYPERGEOMETRIC FUNCTION       * */
/*     *                                                              * */
/*     *                           by                                 * */
/*     *                                                              * */
/*     *                      W. F. PERGER,                           * */
/*     *                                                              * */
/*     *              MARK NARDIN  and ATUL BHALLA                    * */
/*     *                                                              * */
/*     *                                                              * */
/*     *            Electrical Engineering Department                 * */
/*     *            Michigan Technological University                 * */
/*     *                  1400 Townsend Drive                         * */
/*     *                Houghton, MI  49931-1295   USA                * */
/*     *                     Copyright 1993                           * */
/*     *                                                              * */
/*     *               e-mail address: wfp@mtu.edu                    * */
/*     *                                                              * */
/*     *  Description : A numerical evaluator for the generalized     * */
/*     *    hypergeometric function for complex arguments with large  * */
/*     *    magnitudes using a direct summation of the Gauss series.  * */
/*     *    The method used allows an accuracy of up to thirteen      * */
/*     *    decimal places through the use of large integer arrays    * */
/*     *    and a single final division.                              * */
/*     *    (original subroutines for the confluent hypergeometric    * */
/*     *    written by Mark Nardin, 1989; modifications made to cal-  * */
/*     *    culate the generalized hypergeometric function were       * */
/*     *    written by W.F. Perger and A. Bhalla, June, 1990)         * */
/*     *                                                              * */
/*     *  The evaluation of the pFq series is accomplished by a func- * */
/*     *  ion call to PFQ, which is a double precision complex func-  * */
/*     *  tion.  The required input is:                               * */
/*     *  1. Double precision complex arrays A and B.  These are the  * */
/*     *     arrays containing the parameters in the numerator and de-* */
/*     *     nominator, respectively.                                 * */
/*     *  2. Integers IP and IQ.  These integers indicate the number  * */
/*     *     of numerator and denominator terms, respectively (these  * */
/*     *     are p and q in the pFq function).                        * */
/*     *  3. Double precision complex argument Z.                     * */
/*     *  4. Integer LNPFQ.  This integer should be set to '1' if the * */
/*     *     result from PFQ is to be returned as the natural logaritm* */
/*     *     of the series, or '0' if not.  The user can generally set* */
/*     *     LNPFQ = '0' and change it if required.                   * */
/*     *  5. Integer IX.  This integer should be set to '0' if the    * */
/*     *     user desires the program PFQ to estimate the number of   * */
/*     *     array terms (in A and B) to be used, or an integer       * */
/*     *     greater than zero specifying the number of integer pos-  * */
/*     *     itions to be used.  This input parameter is escpecially  * */
/*     *     useful as a means to check the results of a given run.   * */
/*     *     Specificially, if the user obtains a result for a given  * */
/*     *     set of parameters, then changes IX and re-runs the eval- * */
/*     *     uator, and if the number of array positions was insuffi- * */
/*     *     cient, then the two results will likely differ.  The rec-* */
/*     *     commended would be to generally set IX = '0' and then set* */
/*     *     it to 100 or so for a second run.  Note that the LENGTH  * */
/*     *     parameter currently sets the upper limit on IX to 777,   * */
/*     *     but that can easily be changed (it is a single PARAMETER * */
/*     *     statement) and the program recompiled.                   * */
/*     *  6. Integer NSIGFIG.  This integer specifies the requested   * */
/*     *     number of significant figures in the final result.  If   * */
/*     *     the user attempts to request more than the number of bits* */
/*     *     in the mantissa allows, the program will abort with an   * */
/*     *     appropriate error message.  The recommended value is 10. * */
/*     *                                                              * */
/*     *     Note: The variable NOUT is the file to which error mess- * */
/*     *           ages are written (default is 6).  This can be      * */
/*     *           changed in the FUNCTION PFQ to accomodate re-      * */
/*     *           of output to another file                          * */
/*     *                                                              * */
/*     *  Subprograms called: HYPER.                                  * */
/*     *                                                              * */
/*     **************************************************************** */

/* Double Complex */ VOID pfq_( ret_val, a, b, ip, iq, z__, lnpfq, ix,
	nsigfig, ierr, errstr, errstr_len)
doublecomplex * ret_val;
doublecomplex *a, *b;
integer *ip, *iq;
doublecomplex *z__;
integer *lnpfq, *ix, *nsigfig;
bool *ierr;
char *errstr;
ftnlen errstr_len;
{
    /* Format strings */
    static char fmt_300[] = "(/,1x,\002IP=\002,1i2,3x,\002IQ=\002,1i2,3x,\
\002AND ABS(Z)=\002,1e12.5,2x,/,\002 WHICH IS GREATER THAN ONE--SERIES DOE\
S\002,\002 NOT CONVERGE\002)";

    /* System generated locals */
    doublereal d__1;
    doublecomplex z__1, z__2, z__3, z__4, z__5, z__6, z__7, z__8, z__9, z__10,
	     z__11, z__12, z__13, z__14, z__15, z__16, z__17;
    icilist ici__1;

    /* Builtin functions */
    integer s_wsli(), do_lio(), e_wsli();
    double z_abs();
    integer s_wsfi(), do_fio(), e_wsfi();
    void z_div(), pow_zz();
    double d_imag();
    integer i_dnnt();

    /* Local variables */
    static doublereal diff, argi, argr, dnum;
    static integer i__;
    static doublecomplex a1[2], b1[1];
    extern /* Double Complex */ VOID hyper_();
    static doublecomplex z1, hyper1, hyper2;
    extern /* Double Complex */ VOID cgamma_();
    static doublereal precis;
    static doublecomplex gam1, gam2, gam3, gam4, gam5, gam6, gam7;

    /* Parameter adjustments */
    --a;
    --b;

    /* Function Body */
    io_1.nout = 6;
    *ierr = 0;
    z__1.r = consts_1.zero, z__1.i = consts_1.zero;
    if (z__->r == z__1.r && z__->i == z__1.i) {
	z__1.r = consts_1.one, z__1.i = consts_1.zero;
	 ret_val->r = z__1.r,  ret_val->i = z__1.i;
	return ;
    }
    if (*lnpfq != 0 && *lnpfq != 1) {
	ici__1.icierr = 0;
	ici__1.icirnum = 1;
	ici__1.icirlen = 256;
	ici__1.iciunit = errstr;
	ici__1.icifmt = 0;
	s_wsli(&ici__1);
	do_lio(&c__9, &c__1, " ERROR IN INPUT ARGUMENTS: LNPFQ /= 0 OR 1",
		42L);
	e_wsli();
	*ierr = 1;
	 ret_val->r = (float)999.99,  ret_val->i = (float)0.;
	return ;
    }
    if (*ip > *iq && z_abs(z__) > consts_1.one && *ip > 2) {
	ici__1.icierr = 0;
	ici__1.icirnum = 1;
	ici__1.icirlen = 256;
	ici__1.iciunit = errstr;
	ici__1.icifmt = fmt_300;
	s_wsfi(&ici__1);
	do_fio(&c__1, (char *)&(*ip), (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&(*iq), (ftnlen)sizeof(integer));
	d__1 = z_abs(z__);
	do_fio(&c__1, (char *)&d__1, (ftnlen)sizeof(doublereal));
	e_wsfi();
	*ierr = 1;
	 ret_val->r = (float)999.99,  ret_val->i = (float)0.;
	return ;
    }

/* For the 2F1 function with |z| > 1, use Abramowitz and Stegun, 15.3.7.
*/

    if (*ip == 2 && *iq == 1 && z_abs(z__) > consts_1.one) {
	z__2.r = consts_1.one, z__2.i = 0.;
	z_div(&z__1, &z__2, z__);
	z1.r = z__1.r, z1.i = z__1.i;
	a1[0].r = a[1].r, a1[0].i = a[1].i;
	z__2.r = consts_1.one - b[1].r, z__2.i = -b[1].i;
	z__1.r = z__2.r + a[1].r, z__1.i = z__2.i + a[1].i;
	a1[1].r = z__1.r, a1[1].i = z__1.i;
	z__2.r = consts_1.one - a[2].r, z__2.i = -a[2].i;
	z__1.r = z__2.r + a[1].r, z__1.i = z__2.i + a[1].i;
	b1[0].r = z__1.r, b1[0].i = z__1.i;
	cgamma_(&z__1, &b[1], lnpfq, ierr, errstr, errstr_len);
	gam1.r = z__1.r, gam1.i = z__1.i;
	if (*ierr == 1) {
	     ret_val->r = (float)999.99,  ret_val->i = (float)0.;
	    return ;
	}
	z__2.r = a[2].r - a[1].r, z__2.i = a[2].i - a[1].i;
	cgamma_(&z__1, &z__2, lnpfq, ierr, errstr, errstr_len);
	gam2.r = z__1.r, gam2.i = z__1.i;
	if (*ierr == 1) {
	     ret_val->r = (float)999.99,  ret_val->i = (float)0.;
	    return ;
	}
	cgamma_(&z__1, &a[2], lnpfq, ierr, errstr, errstr_len);
	gam3.r = z__1.r, gam3.i = z__1.i;
	if (*ierr == 1) {
	     ret_val->r = (float)999.99,  ret_val->i = (float)0.;
	    return ;
	}
	z__2.r = b[1].r - a[1].r, z__2.i = b[1].i - a[1].i;
	cgamma_(&z__1, &z__2, lnpfq, ierr, errstr, errstr_len);
	gam4.r = z__1.r, gam4.i = z__1.i;
	if (*ierr == 1) {
	     ret_val->r = (float)999.99,  ret_val->i = (float)0.;
	    return ;
	}
	z__2.r = a[1].r - a[2].r, z__2.i = a[1].i - a[2].i;
	cgamma_(&z__1, &z__2, lnpfq, ierr, errstr, errstr_len);
	gam5.r = z__1.r, gam5.i = z__1.i;
	if (*ierr == 1) {
	     ret_val->r = (float)999.99,  ret_val->i = (float)0.;
	    return ;
	}
	cgamma_(&z__1, &a[1], lnpfq, ierr, errstr, errstr_len);
	gam6.r = z__1.r, gam6.i = z__1.i;
	if (*ierr == 1) {
	     ret_val->r = (float)999.99,  ret_val->i = (float)0.;
	    return ;
	}
	z__2.r = b[1].r - a[2].r, z__2.i = b[1].i - a[2].i;
	cgamma_(&z__1, &z__2, lnpfq, ierr, errstr, errstr_len);
	gam7.r = z__1.r, gam7.i = z__1.i;
	if (*ierr == 1) {
	     ret_val->r = (float)999.99,  ret_val->i = (float)0.;
	    return ;
	}
	hyper_(&z__1, a1, b1, ip, iq, &z1, lnpfq, ix, nsigfig, ierr, errstr, errstr_len);
	hyper1.r = z__1.r, hyper1.i = z__1.i;
	if (*ierr == 1) {
	     ret_val->r = (float)999.99,  ret_val->i = (float)0.;
	    return ;
	}
	a1[0].r = a[2].r, a1[0].i = a[2].i;
	z__2.r = consts_1.one - b[1].r, z__2.i = -b[1].i;
	z__1.r = z__2.r + a[2].r, z__1.i = z__2.i + a[2].i;
	a1[1].r = z__1.r, a1[1].i = z__1.i;
	z__2.r = consts_1.one - a[1].r, z__2.i = -a[1].i;
	z__1.r = z__2.r + a[2].r, z__1.i = z__2.i + a[2].i;
	b1[0].r = z__1.r, b1[0].i = z__1.i;
	hyper_(&z__1, a1, b1, ip, iq, &z1, lnpfq, ix, nsigfig, ierr, errstr, errstr_len);
	hyper2.r = z__1.r, hyper2.i = z__1.i;
	if (*ierr == 1) {
	     ret_val->r = (float)999.99,  ret_val->i = (float)0.;
	    return ;
	}
	z__5.r = gam1.r * gam2.r - gam1.i * gam2.i, z__5.i = gam1.r * gam2.i
		+ gam1.i * gam2.r;
	z__7.r = -z__->r, z__7.i = -z__->i;
	z__8.r = -a[1].r, z__8.i = -a[1].i;
	pow_zz(&z__6, &z__7, &z__8);
	z__4.r = z__5.r * z__6.r - z__5.i * z__6.i, z__4.i = z__5.r * z__6.i
		+ z__5.i * z__6.r;
	z__3.r = z__4.r * hyper1.r - z__4.i * hyper1.i, z__3.i = z__4.r *
		hyper1.i + z__4.i * hyper1.r;
	z__9.r = gam3.r * gam4.r - gam3.i * gam4.i, z__9.i = gam3.r * gam4.i
		+ gam3.i * gam4.r;
	z_div(&z__2, &z__3, &z__9);
	z__13.r = gam1.r * gam5.r - gam1.i * gam5.i, z__13.i = gam1.r *
		gam5.i + gam1.i * gam5.r;
	z__15.r = -z__->r, z__15.i = -z__->i;
	z__16.r = -a[2].r, z__16.i = -a[2].i;
	pow_zz(&z__14, &z__15, &z__16);
	z__12.r = z__13.r * z__14.r - z__13.i * z__14.i, z__12.i = z__13.r *
		z__14.i + z__13.i * z__14.r;
	z__11.r = z__12.r * hyper2.r - z__12.i * hyper2.i, z__11.i = z__12.r *
		 hyper2.i + z__12.i * hyper2.r;
	z__17.r = gam6.r * gam7.r - gam6.i * gam7.i, z__17.i = gam6.r *
		gam7.i + gam6.i * gam7.r;
	z_div(&z__10, &z__11, &z__17);
	z__1.r = z__2.r + z__10.r, z__1.i = z__2.i + z__10.i;
	 ret_val->r = z__1.r,  ret_val->i = z__1.i;
	return ;
    }
    if (*ip == 2 && *iq == 1 && z_abs(z__) > (float).9) {
	if (*lnpfq == 1) {
	    goto L30;
	}

/*     Check to see if the Gamma function arguments are o.k.; if not,
*/
/*     then the series will have to be used. */

/*     PRECIS - MACHINE PRECISION */

	precis = consts_1.one;
L10:
	precis /= consts_1.two;
	dnum = precis + consts_1.one;
	if (dnum > consts_1.one) {
	    goto L10;
	}
	precis = consts_1.two * precis;
	for (i__ = 1; i__ <= 6; ++i__) {
	    if (i__ == 1) {
		argi = d_imag(&b[1]);
		argr = b[1].r;
	    } else if (i__ == 2) {
		z__2.r = b[1].r - a[1].r, z__2.i = b[1].i - a[1].i;
		z__1.r = z__2.r - a[2].r, z__1.i = z__2.i - a[2].i;
		argi = d_imag(&z__1);
		z__2.r = b[1].r - a[1].r, z__2.i = b[1].i - a[1].i;
		z__1.r = z__2.r - a[2].r, z__1.i = z__2.i - a[2].i;
		argr = z__1.r;
	    } else if (i__ == 3) {
		z__1.r = b[1].r - a[1].r, z__1.i = b[1].i - a[1].i;
		argi = d_imag(&z__1);
		z__1.r = b[1].r - a[1].r, z__1.i = b[1].i - a[1].i;
		argr = z__1.r;
	    } else if (i__ == 4) {
		z__2.r = a[1].r + a[2].r, z__2.i = a[1].i + a[2].i;
		z__1.r = z__2.r - b[1].r, z__1.i = z__2.i - b[1].i;
		argi = d_imag(&z__1);
		z__2.r = a[1].r + a[2].r, z__2.i = a[1].i + a[2].i;
		z__1.r = z__2.r - b[1].r, z__1.i = z__2.i - b[1].i;
		argr = z__1.r;
	    } else if (i__ == 5) {
		argi = d_imag(&a[1]);
		argr = a[1].r;
	    } else if (i__ == 6) {
		argi = d_imag(&a[2]);
		argr = a[2].r;
	    }

/*     CASES WHERE THE ARGUMENT IS REAL */

	    if (argi == consts_1.zero) {

/*     CASES WHERE THE ARGUMENT IS REAL AND NEGATIVE */

		if (argr <= consts_1.zero) {

/*     USE THE SERIES EXPANSION IF THE ARGUMENT IS TOO NEA
R A POLE */

		    diff = (d__1 = (doublereal) i_dnnt(&argr) - argr, abs(
			    d__1));
		    if (diff <= consts_1.two * precis) {
			goto L30;
		    }
		}
	    }
/* L20: */
	}
	cgamma_(&z__1, &b[1], lnpfq, ierr, errstr, errstr_len);
	gam1.r = z__1.r, gam1.i = z__1.i;
	if (*ierr == 1) {
	     ret_val->r = (float)999.99,  ret_val->i = (float)0.;
	    return ;
	}
	z__3.r = b[1].r - a[1].r, z__3.i = b[1].i - a[1].i;
	z__2.r = z__3.r - a[2].r, z__2.i = z__3.i - a[2].i;
	cgamma_(&z__1, &z__2, lnpfq, ierr, errstr, errstr_len);
	gam2.r = z__1.r, gam2.i = z__1.i;
	if (*ierr == 1) {
	     ret_val->r = (float)999.99,  ret_val->i = (float)0.;
	    return ;
	}
	z__2.r = b[1].r - a[1].r, z__2.i = b[1].i - a[1].i;
	cgamma_(&z__1, &z__2, lnpfq, ierr, errstr, errstr_len);
	gam3.r = z__1.r, gam3.i = z__1.i;
	if (*ierr == 1) {
	     ret_val->r = (float)999.99,  ret_val->i = (float)0.;
	    return ;
	}
	z__2.r = b[1].r - a[2].r, z__2.i = b[1].i - a[2].i;
	cgamma_(&z__1, &z__2, lnpfq, ierr, errstr, errstr_len);
	gam4.r = z__1.r, gam4.i = z__1.i;
	if (*ierr == 1) {
	     ret_val->r = (float)999.99,  ret_val->i = (float)0.;
	    return ;
	}
	z__3.r = a[1].r + a[2].r, z__3.i = a[1].i + a[2].i;
	z__2.r = z__3.r - b[1].r, z__2.i = z__3.i - b[1].i;
	cgamma_(&z__1, &z__2, lnpfq, ierr, errstr, errstr_len);
	gam5.r = z__1.r, gam5.i = z__1.i;
	if (*ierr == 1) {
	     ret_val->r = (float)999.99,  ret_val->i = (float)0.;
	    return ;
	}
	cgamma_(&z__1, &a[1], lnpfq, ierr, errstr, errstr_len);
	gam6.r = z__1.r, gam6.i = z__1.i;
	if (*ierr == 1) {
	     ret_val->r = (float)999.99,  ret_val->i = (float)0.;
	    return ;
	}
	cgamma_(&z__1, &a[2], lnpfq, ierr, errstr, errstr_len);
	gam7.r = z__1.r, gam7.i = z__1.i;
	if (*ierr == 1) {
	     ret_val->r = (float)999.99,  ret_val->i = (float)0.;
	    return ;
	}
	a1[0].r = a[1].r, a1[0].i = a[1].i;
	a1[1].r = a[2].r, a1[1].i = a[2].i;
	z__3.r = a[1].r + a[2].r, z__3.i = a[1].i + a[2].i;
	z__2.r = z__3.r - b[1].r, z__2.i = z__3.i - b[1].i;
	z__1.r = z__2.r + consts_1.one, z__1.i = z__2.i;
	b1[0].r = z__1.r, b1[0].i = z__1.i;
	z__1.r = consts_1.one - z__->r, z__1.i = -z__->i;
	z1.r = z__1.r, z1.i = z__1.i;
	hyper_(&z__1, a1, b1, ip, iq, &z1, lnpfq, ix, nsigfig, ierr, errstr, errstr_len);
	hyper1.r = z__1.r, hyper1.i = z__1.i;
	if (*ierr == 1) {
	     ret_val->r = (float)999.99,  ret_val->i = (float)0.;
	    return ;
	}
	z__1.r = b[1].r - a[1].r, z__1.i = b[1].i - a[1].i;
	a1[0].r = z__1.r, a1[0].i = z__1.i;
	z__1.r = b[1].r - a[2].r, z__1.i = b[1].i - a[2].i;
	a1[1].r = z__1.r, a1[1].i = z__1.i;
	z__3.r = b[1].r - a[1].r, z__3.i = b[1].i - a[1].i;
	z__2.r = z__3.r - a[2].r, z__2.i = z__3.i - a[2].i;
	z__1.r = z__2.r + consts_1.one, z__1.i = z__2.i;
	b1[0].r = z__1.r, b1[0].i = z__1.i;
	hyper_(&z__1, a1, b1, ip, iq, &z1, lnpfq, ix, nsigfig, ierr, errstr, errstr_len);
	hyper2.r = z__1.r, hyper2.i = z__1.i;
	if (*ierr == 1) {
	     ret_val->r = (float)999.99,  ret_val->i = (float)0.;
	    return ;
	}
	z__4.r = gam1.r * gam2.r - gam1.i * gam2.i, z__4.i = gam1.r * gam2.i
		+ gam1.i * gam2.r;
	z__3.r = z__4.r * hyper1.r - z__4.i * hyper1.i, z__3.i = z__4.r *
		hyper1.i + z__4.i * hyper1.r;
	z__5.r = gam3.r * gam4.r - gam3.i * gam4.i, z__5.i = gam3.r * gam4.i
		+ gam3.i * gam4.r;
	z_div(&z__2, &z__3, &z__5);
	z__11.r = consts_1.one - z__->r, z__11.i = -z__->i;
	z__13.r = b[1].r - a[1].r, z__13.i = b[1].i - a[1].i;
	z__12.r = z__13.r - a[2].r, z__12.i = z__13.i - a[2].i;
	pow_zz(&z__10, &z__11, &z__12);
	z__9.r = z__10.r * gam1.r - z__10.i * gam1.i, z__9.i = z__10.r *
		gam1.i + z__10.i * gam1.r;
	z__8.r = z__9.r * gam5.r - z__9.i * gam5.i, z__8.i = z__9.r * gam5.i
		+ z__9.i * gam5.r;
	z__7.r = z__8.r * hyper2.r - z__8.i * hyper2.i, z__7.i = z__8.r *
		hyper2.i + z__8.i * hyper2.r;
	z__14.r = gam6.r * gam7.r - gam6.i * gam7.i, z__14.i = gam6.r *
		gam7.i + gam6.i * gam7.r;
	z_div(&z__6, &z__7, &z__14);
	z__1.r = z__2.r + z__6.r, z__1.i = z__2.i + z__6.i;
	 ret_val->r = z__1.r,  ret_val->i = z__1.i;
	return ;
    }
L30:
    hyper_(&z__1, &a[1], &b[1], ip, iq, z__, lnpfq, ix, nsigfig, ierr, errstr, errstr_len);
     ret_val->r = z__1.r,  ret_val->i = z__1.i;
    if (*ierr == 1) {
	 ret_val->r = (float)999.99,  ret_val->i = (float)0.;
	return ;
    }
    return ;
} /* pfq_ */

/*     **************************************************************** */
/*     *                                                              * */
/*     *                   FUNCTION BITS                              * */
/*     *                                                              * */
/*     *                                                              * */
/*     *  Description : Determines the number of significant figures  * */
/*     *    of machine precision to arrive at the size of the array   * */
/*     *    the numbers must be stored in to get the accuracy of the  * */
/*     *    solution.                                                 * */
/*     *                                                              * */
/*     *  Subprograms called: none                                    * */
/*     *                                                              * */
/*     **************************************************************** */
doublereal bits_()
{
    /* System generated locals */
    doublereal ret_val;

    /* Local variables */
    static integer count;
    static doublereal bit, bit2;

    bit = (float)1.;
    count = 0;
L10:
    ++count;
    bit2 = bit * (float)2.;
    bit = bit2 + (float)1.;
    if (bit - bit2 != (float)0.) {
	goto L10;
    }
    ret_val = (doublereal) (count - 3);
    return ret_val;
} /* bits_ */

/*     **************************************************************** */
/*     *                                                              * */
/*     *                   FUNCTION HYPER                             * */
/*     *                                                              * */
/*     *                                                              * */
/*     *  Description : Function that sums the Gauss series.          * */
/*     *                                                              * */
/*     *  Subprograms called: ARMULT, ARYDIV, BITS, CMPADD, CMPMUL,   * */
/*     *                      IPREMAX.                                * */
/*     *                                                              * */
/*     **************************************************************** */
/* Double Complex */ VOID hyper_( ret_val, a, b, ip, iq, z__, lnpfq, ix,
	nsigfig, ierr, errstr, errstr_len)
doublecomplex * ret_val;
doublecomplex *a, *b;
integer *ip, *iq;
doublecomplex *z__;
integer *lnpfq, *ix, *nsigfig;
bool *ierr;
char *errstr;
ftnlen errstr_len;
{
    /* Format strings */
    static char fmt_300[] = "(1x,\002WARNING - REAL PART OF A(\002,1i2,\002)\
 WAS SET TO ZERO\002)";
    static char fmt_301[] = "(1x,\002WARNING - IMAG PART OF A(\002,1i2,\002)\
 WAS SET TO ZERO\002)";
    static char fmt_302[] = "(1x,\002WARNING - REAL PART OF B(\002,1i2,\002)\
 WAS SET TO ZERO\002)";
    static char fmt_303[] = "(1x,\002WARNING - IMAG PART OF B(\002,1i2,\002)\
 WAS SET TO ZERO\002)";
    static char fmt_304[] = "(1x,\002ERROR - ARGUMENT B(\002,1i2,\002) WAS E\
QUAL TO ZERO\002)";
    static char fmt_305[] = "(1x,\002ERROR - ARGUMENT B(\002,1i2,\002) WAS A\
 NEGATIVE\002,\002 INTEGER\002)";
    static char fmt_306[] = "(1x,\002ERROR IN FN HYPER: L MUST BE < \002,1i4)"
	    ;
    static char fmt_307[] = "(1x,\002 WARNING--THE NUMBER OF SIGNIFICANT FIG\
URES REQU\002,\002ESTED\002,/,\002IS GREATER THAN THE MACHINE PRECISION--\
\002,\002FINAL ANSWER\002,/,\002WILL BE ACCURATE TO ONLY\002,i3,\002 DIGIT\
S\002)";

    /* System generated locals */
    integer i__1, i__2, i__3, i__4, i__5;
    doublereal d__1, d__2, d__3;
    doublecomplex z__1, z__2, z__3, z__4, z__5;
    icilist ici__1;

    /* Builtin functions */
    double d_lg10(), pow_di(), d_int(), d_nint(), d_imag();
    integer s_wsfe(), do_fio(), e_wsfe(), s_wsle(), do_lio(), e_wsle(),
	    i_dnnt(), s_wsfi(), e_wsfi();
    /* Subroutine */ int s_stop();
    void z_log();
    double exp();
    void z_div();
    double d_sign(), pow_dd(), z_abs();

    /* Local variables */
    static doublereal accy;
    static integer ibit, lmax;
    extern doublereal bits_();
    static doublecomplex temp;
    static doublereal rmax, numi[25002], sumi[25002];
    static integer rexp;
    static doublereal numr[25002], sumr[25002];
    static doublecomplex cdum1, cdum2, temp1;
    static integer i__, l, nmach;
    static doublereal creal, x;
    static doublecomplex final;
    static integer ixcnt;
    static doublereal expon;
    static integer i1;
    static doublereal ai[10], ci[10], ar[10], cr[10];
    extern /* Subroutine */ int cmpadd_();
    static doublereal xi, xl, sigfig, xr, denomi[25002];
    extern /* Double Complex */ VOID factor_();
    static doublereal denomr[25002], ai2[10];
    extern /* Subroutine */ int cmpmul_();
    static doublereal ci2[10];
    extern /* Subroutine */ int arydiv_();
    static integer icount;
    extern /* Subroutine */ int armult_();
    static doublereal ar2[10], cr2[10], qi1[25002], qi2[25002], wk1[25002],
	    qr1[25002], qr2[25002], wk2[25002], wk3[25002], wk4[25002], wk5[
	    25002], wk6[25002], mx1, mx2, xr2, xi2;
    static integer ii10, ir10;
    static doublereal ri10, cnt, rr10;
    static doublecomplex oldtemp;
    extern integer ipremax_();
    static doublereal log2, dum1, dum2;

    /* Fortran I/O blocks */
    static cilist io___39 = { 0, 0, 0, fmt_300, 0 };
    static cilist io___40 = { 0, 0, 0, fmt_301, 0 };
    static cilist io___41 = { 0, 0, 0, fmt_302, 0 };
    static cilist io___42 = { 0, 0, 0, fmt_303, 0 };
    static cilist io___43 = { 0, 0, 0, 0, 0 };
    static cilist io___44 = { 0, 0, 0, 0, 0 };
    static cilist io___55 = { 0, 0, 0, fmt_307, 0 };


    /* Parameter adjustments */
    --a;
    --b;

    /* Function Body */
    consts_1.zero = 0.;
    log2 = d_lg10(&consts_1.two);
    ibit = (integer) bits_();
    i__1 = ibit / 2;
    rmax = pow_di(&consts_1.two, &i__1);
    i__1 = ibit / 4;
    sigfig = pow_di(&consts_1.two, &i__1);

    i__1 = *ip;
    for (i1 = 1; i1 <= i__1; ++i1) {
	i__2 = i1;
	ar2[i1 - 1] = a[i__2].r * sigfig;
	ar[i1 - 1] = d_int(&ar2[i1 - 1]);
	d__1 = (ar2[i1 - 1] - ar[i1 - 1]) * rmax;
	ar2[i1 - 1] = d_nint(&d__1);
	ai2[i1 - 1] = d_imag(&a[i1]) * sigfig;
	ai[i1 - 1] = d_int(&ai2[i1 - 1]);
	d__1 = (ai2[i1 - 1] - ai[i1 - 1]) * rmax;
	ai2[i1 - 1] = d_nint(&d__1);
/* L10: */
    }
    i__1 = *iq;
    for (i1 = 1; i1 <= i__1; ++i1) {
	i__2 = i1;
	cr2[i1 - 1] = b[i__2].r * sigfig;
	cr[i1 - 1] = d_int(&cr2[i1 - 1]);
	d__1 = (cr2[i1 - 1] - cr[i1 - 1]) * rmax;
	cr2[i1 - 1] = d_nint(&d__1);
	ci2[i1 - 1] = d_imag(&b[i1]) * sigfig;
	ci[i1 - 1] = d_int(&ci2[i1 - 1]);
	d__1 = (ci2[i1 - 1] - ci[i1 - 1]) * rmax;
	ci2[i1 - 1] = d_nint(&d__1);
/* L20: */
    }
    xr2 = z__->r * sigfig;
    xr = d_int(&xr2);
    d__1 = (xr2 - xr) * rmax;
    xr2 = d_nint(&d__1);
    xi2 = d_imag(z__) * sigfig;
    xi = d_int(&xi2);
    d__1 = (xi2 - xi) * rmax;
    xi2 = d_nint(&d__1);

/*     WARN THE USER THAT THE INPUT VALUE WAS SO CLOSE TO ZERO THAT IT */
/*     WAS SET EQUAL TO ZERO. */

    i__1 = *ip;
    for (i1 = 1; i1 <= i__1; ++i1) {
	i__2 = i1;
	if (a[i__2].r != consts_1.zero && ar[i1 - 1] == consts_1.zero && ar2[
		i1 - 1] == consts_1.zero) {
	    io___39.ciunit = io_1.nout;
	    s_wsfe(&io___39);
	    do_fio(&c__1, (char *)&i1, (ftnlen)sizeof(integer));
	    e_wsfe();
	}
	if (d_imag(&a[i1]) != consts_1.zero && ai[i1 - 1] == consts_1.zero &&
		ai2[i1 - 1] == consts_1.zero) {
	    io___40.ciunit = io_1.nout;
	    s_wsfe(&io___40);
	    do_fio(&c__1, (char *)&i1, (ftnlen)sizeof(integer));
	    e_wsfe();
	}
/* L30: */
    }
    i__1 = *iq;
    for (i1 = 1; i1 <= i__1; ++i1) {
	i__2 = i1;
	if (b[i__2].r != consts_1.zero && cr[i1 - 1] == consts_1.zero && cr2[
		i1 - 1] == consts_1.zero) {
	    io___41.ciunit = io_1.nout;
	    s_wsfe(&io___41);
	    do_fio(&c__1, (char *)&i1, (ftnlen)sizeof(integer));
	    e_wsfe();
	}
	if (d_imag(&b[i1]) != consts_1.zero && ci[i1 - 1] == consts_1.zero &&
		ci2[i1 - 1] == consts_1.zero) {
	    io___42.ciunit = io_1.nout;
	    s_wsfe(&io___42);
	    do_fio(&c__1, (char *)&i1, (ftnlen)sizeof(integer));
	    e_wsfe();
	}
/* L40: */
    }
    if (z__->r != consts_1.zero && xr == consts_1.zero && xr2 ==
	    consts_1.zero) {
	io___43.ciunit = io_1.nout;
	s_wsle(&io___43);
	do_lio(&c__9, &c__1, " WARNING - REAL PART OF Z WAS SET TO ZERO", 41L)
		;
	e_wsle();
	d__1 = d_imag(z__);
	z__1.r = consts_1.zero, z__1.i = d__1;
	z__->r = z__1.r, z__->i = z__1.i;
    }
    if (d_imag(z__) != consts_1.zero && xi == consts_1.zero && xi2 ==
	    consts_1.zero) {
	io___44.ciunit = io_1.nout;
	s_wsle(&io___44);
	do_lio(&c__9, &c__1, " WARNING - IMAG PART OF Z WAS SET TO ZERO", 41L)
		;
	e_wsle();
	d__1 = z__->r;
	z__1.r = d__1, z__1.i = consts_1.zero;
	z__->r = z__1.r, z__->i = z__1.i;
    }

/*     SCREENING OF NUMERATOR ARGUMENTS FOR NEGATIVE INTEGERS OR ZERO. */
/*     ICOUNT WILL FORCE THE SERIES TO TERMINATE CORRECTLY. */

    i__1 = (integer) bits_();
    d__1 = pow_di(&consts_1.two, &i__1);
    nmach = (integer) d_lg10(&d__1);
    icount = -1;
    i__1 = *ip;
    for (i1 = 1; i1 <= i__1; ++i1) {
	if (ar2[i1 - 1] == consts_1.zero && ar[i1 - 1] == consts_1.zero &&
		ai2[i1 - 1] == consts_1.zero && ai[i1 - 1] == consts_1.zero) {
	    z__1.r = consts_1.one, z__1.i = consts_1.zero;
	     ret_val->r = z__1.r,  ret_val->i = z__1.i;
	    return ;
	}
	i__2 = i1;
	if (ai[i1 - 1] == consts_1.zero && ai2[i1 - 1] == consts_1.zero && a[
		i__2].r < consts_1.zero) {
	    i__2 = i1;
	    i__3 = i1;
	    d__2 = a[i__3].r;
	    i__4 = -nmach;
	    if ((d__1 = a[i__2].r - (doublereal) i_dnnt(&d__2), abs(d__1)) <
		    pow_di(&consts_1.ten, &i__4)) {
		if (icount != -1) {
/* Computing MIN */
		    i__4 = i1;
		    d__1 = a[i__4].r;
		    i__2 = icount, i__3 = -i_dnnt(&d__1);
		    icount = min(i__2,i__3);
		} else {
		    i__2 = i1;
		    d__1 = a[i__2].r;
		    icount = -i_dnnt(&d__1);
		}
	    }
	}
/* L50: */
    }

/*     SCREENING OF DENOMINATOR ARGUMENTS FOR ZEROES OR NEGATIVE INTEGERS.
 */

    i__1 = *iq;
    for (i1 = 1; i1 <= i__1; ++i1) {
	if (cr[i1 - 1] == consts_1.zero && cr2[i1 - 1] == consts_1.zero && ci[
		i1 - 1] == consts_1.zero && ci2[i1 - 1] == consts_1.zero) {
	    ici__1.icierr = 0;
	    ici__1.icirnum = 1;
	    ici__1.icirlen = 256;
	    ici__1.iciunit = errstr;
	    ici__1.icifmt = fmt_304;
	    s_wsfi(&ici__1);
	    do_fio(&c__1, (char *)&i1, (ftnlen)sizeof(integer));
	    e_wsfi();
	     ret_val->r = 9.9999e11,  ret_val->i = 9.9999e11;
	    *ierr = 1;
	    return ;
	    s_stop("", 0L);
	}
	i__2 = i1;
	if (ci[i1 - 1] == consts_1.zero && ci2[i1 - 1] == consts_1.zero && b[
		i__2].r < consts_1.zero) {
	    i__2 = i1;
	    i__3 = i1;
	    d__2 = b[i__3].r;
	    i__4 = -nmach;
	    i__5 = i1;
	    d__3 = b[i__5].r;
	    if ((d__1 = b[i__2].r - (doublereal) i_dnnt(&d__2), abs(d__1)) <
		    pow_di(&consts_1.ten, &i__4) && (icount >= -i_dnnt(&d__3)
		    || icount == -1)) {
		ici__1.icierr = 0;
		ici__1.icirnum = 1;
		ici__1.icirlen = 256;
		ici__1.iciunit = errstr;
		ici__1.icifmt = fmt_305;
		s_wsfi(&ici__1);
		do_fio(&c__1, (char *)&i1, (ftnlen)sizeof(integer));
		e_wsfi();
		 ret_val->r = 9.9999e11,  ret_val->i = 9.9999e11;
		*ierr = 1;
		return ;
		s_stop("", 0L);
	    }
	}
/* L60: */
    }

    d__1 = pow_di(&consts_1.two, &ibit);
    nmach = (integer) d_lg10(&d__1);
/* Computing MIN */
    d__1 = pow_di(&consts_1.two, &ibit);
    i__1 = *nsigfig, i__2 = (integer) d_lg10(&d__1);
    *nsigfig = min(i__1,i__2);
    i__1 = -(*nsigfig);
    accy = pow_di(&consts_1.ten, &i__1);
    l = ipremax_(&a[1], &b[1], ip, iq, z__, ierr, errstr, errstr_len);
    if (l == 1) {
	goto L110;
    }

/*     First, estimate the exponent of the maximum term in the pFq series.
 */

    expon = consts_1.zero;
    xl = (doublereal) l;
    i__1 = *ip;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = i__;
	z__3.r = a[i__2].r + xl, z__3.i = a[i__2].i;
	z__2.r = z__3.r - consts_1.one, z__2.i = z__3.i;
	factor_(&z__1, &z__2);
	i__3 = i__;
	z__5.r = a[i__3].r - consts_1.one, z__5.i = a[i__3].i;
	factor_(&z__4, &z__5);
	expon = expon + z__1.r - z__4.r;
/* L70: */
    }
    i__1 = *iq;
    for (i__ = 1; i__ <= i__1; ++i__) {
	i__2 = i__;
	z__3.r = b[i__2].r + xl, z__3.i = b[i__2].i;
	z__2.r = z__3.r - consts_1.one, z__2.i = z__3.i;
	factor_(&z__1, &z__2);
	i__3 = i__;
	z__5.r = b[i__3].r - consts_1.one, z__5.i = b[i__3].i;
	factor_(&z__4, &z__5);
	expon = expon - z__1.r + z__4.r;
/* L80: */
    }
    z_log(&z__1, z__);
    z__3.r = xl, z__3.i = consts_1.zero;
    factor_(&z__2, &z__3);
    expon = expon + xl * z__1.r - z__2.r;
    d__1 = exp(consts_1.one);
    lmax = (integer) (d_lg10(&d__1) * expon);
    l = lmax;

/*     Now, estimate the exponent of where the pFq series will terminate.
*/

    z__1.r = consts_1.one, z__1.i = consts_1.zero;
    temp1.r = z__1.r, temp1.i = z__1.i;
    creal = consts_1.one;
    i__1 = *ip;
    for (i1 = 1; i1 <= i__1; ++i1) {
	i__2 = i1 - 1;
	i__3 = i1 - 1;
	z__3.r = ar[i__2], z__3.i = ai[i__3];
	z__2.r = temp1.r * z__3.r - temp1.i * z__3.i, z__2.i = temp1.r *
		z__3.i + temp1.i * z__3.r;
	z__1.r = z__2.r / sigfig, z__1.i = z__2.i / sigfig;
	temp1.r = z__1.r, temp1.i = z__1.i;
/* L90: */
    }
    i__1 = *iq;
    for (i1 = 1; i1 <= i__1; ++i1) {
	i__2 = i1 - 1;
	i__3 = i1 - 1;
	z__3.r = cr[i__2], z__3.i = ci[i__3];
	z__2.r = z__3.r / sigfig, z__2.i = z__3.i / sigfig;
	z_div(&z__1, &temp1, &z__2);
	temp1.r = z__1.r, temp1.i = z__1.i;
	creal *= cr[i1 - 1];
/* L100: */
    }
    z__2.r = xr, z__2.i = xi;
    z__1.r = temp1.r * z__2.r - temp1.i * z__2.i, z__1.i = temp1.r * z__2.i +
	    temp1.i * z__2.r;
    temp1.r = z__1.r, temp1.i = z__1.i;

/*     Triple it to make sure. */

    l *= 3;

/*     Divide the number of significant figures necessary by the number of
 */
/*     digits available per array position. */

    l = ((l << 1) + *nsigfig) / nmach + 2;

/*     Make sure there are at least 5 array positions used. */

L110:
    l = max(l,5);
    l = max(l,*ix);
/*     write (6,*) ' Estimated value of L=',L */
    if (l < 0 || l > 25000) {
	ici__1.icierr = 0;
	ici__1.icirnum = 1;
	ici__1.icirlen = 256;
	ici__1.iciunit = errstr;
	ici__1.icifmt = fmt_306;
	s_wsfi(&ici__1);
	do_fio(&c__1, (char *)&c__25000, (ftnlen)sizeof(integer));
	e_wsfi();
	 ret_val->r = 9.9999e11,  ret_val->i = 9.9999e11;
	*ierr = 1;
	return ;
	s_stop("", 0L);
    }
    if (*nsigfig > nmach) {
	io___55.ciunit = io_1.nout;
	s_wsfe(&io___55);
	do_fio(&c__1, (char *)&nmach, (ftnlen)sizeof(integer));
	e_wsfe();
    }
    sumr[0] = consts_1.one;
    sumi[0] = consts_1.one;
    numr[0] = consts_1.one;
    numi[0] = consts_1.one;
    denomr[0] = consts_1.one;
    denomi[0] = consts_1.one;
    i__1 = l + 1;
    for (i__ = 0; i__ <= i__1; ++i__) {
	sumr[i__ + 1] = consts_1.zero;
	sumi[i__ + 1] = consts_1.zero;
	numr[i__ + 1] = consts_1.zero;
	numi[i__ + 1] = consts_1.zero;
	denomr[i__ + 1] = consts_1.zero;
	denomi[i__ + 1] = consts_1.zero;
/* L120: */
    }
    sumr[2] = consts_1.one;
    numr[2] = consts_1.one;
    denomr[2] = consts_1.one;
    cnt = sigfig;
    z__1.r = consts_1.zero, z__1.i = consts_1.zero;
    temp.r = z__1.r, temp.i = z__1.i;
    oldtemp.r = temp.r, oldtemp.i = temp.i;
    ixcnt = 0;
    rexp = ibit / 2;
    x = rexp * (sumr[l + 2] - 2);
    rr10 = x * log2;
    ir10 = (integer) rr10;
    rr10 -= ir10;
    x = rexp * (sumi[l + 2] - 2);
    ri10 = x * log2;
    ii10 = (integer) ri10;
    ri10 -= ii10;
    d__1 = sumr[2] * rmax * rmax + sumr[3] * rmax + sumr[4];
    dum1 = d_sign(&d__1, sumr);
    d__1 = sumi[2] * rmax * rmax + sumi[3] * rmax + sumi[4];
    dum2 = d_sign(&d__1, sumi);
    dum1 *= pow_dd(&c_b65, &rr10);
    dum2 *= pow_dd(&c_b65, &ri10);
    z__1.r = dum1, z__1.i = dum2;
    cdum1.r = z__1.r, cdum1.i = z__1.i;
    x = rexp * (denomr[l + 2] - 2);
    rr10 = x * log2;
    ir10 = (integer) rr10;
    rr10 -= ir10;
    x = rexp * (denomi[l + 2] - 2);
    ri10 = x * log2;
    ii10 = (integer) ri10;
    ri10 -= ii10;
    d__1 = denomr[2] * rmax * rmax + denomr[3] * rmax + denomr[4];
    dum1 = d_sign(&d__1, denomr);
    d__1 = denomi[2] * rmax * rmax + denomi[3] * rmax + denomi[4];
    dum2 = d_sign(&d__1, denomi);
    dum1 *= pow_dd(&c_b65, &rr10);
    dum2 *= pow_dd(&c_b65, &ri10);
    z__1.r = dum1, z__1.i = dum2;
    cdum2.r = z__1.r, cdum2.i = z__1.i;
    z_div(&z__1, &cdum1, &cdum2);
    temp.r = z__1.r, temp.i = z__1.i;
/*     130 IF (IP .GT. 0) THEN */
L130:
    if (*ip < 0) {
	if (sumr[2] < consts_1.half) {
	    mx1 = sumi[l + 2];
	} else if (sumi[2] < consts_1.half) {
	    mx1 = sumr[l + 2];
	} else {
/* Computing MAX */
	    d__1 = sumr[l + 2], d__2 = sumi[l + 2];
	    mx1 = max(d__1,d__2);
	}
	if (numr[2] < consts_1.half) {
	    mx2 = numi[l + 2];
	} else if (numi[2] < consts_1.half) {
	    mx2 = numr[l + 2];
	} else {
/* Computing MAX */
	    d__1 = numr[l + 2], d__2 = numi[l + 2];
	    mx2 = max(d__1,d__2);
	}
	if (mx1 - mx2 > (float)2.) {
	    if (creal >= consts_1.zero) {
/*     write (6,*) ' cdabs(temp1/cnt)=',cdabs(temp1/cnt) */
		z__1.r = temp1.r / cnt, z__1.i = temp1.i / cnt;
		if (z_abs(&z__1) <= consts_1.one) {
		    goto L240;
		}
	    }
	}
    } else {
	arydiv_(sumr, sumi, denomr, denomi, &temp, &l, lnpfq, &rmax, &ibit,
		ierr, errstr, errstr_len);
	if (*ierr == 0) {
	     ret_val->r = 9.9999e11,  ret_val->i = 9.9999e11;
	    return ;
	}

/*     First, estimate the exponent of the maximum term in the pFq ser
ies. */

	expon = consts_1.zero;
	xl = (doublereal) ixcnt;
	i__1 = *ip;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    i__2 = i__;
	    z__3.r = a[i__2].r + xl, z__3.i = a[i__2].i;
	    z__2.r = z__3.r - consts_1.one, z__2.i = z__3.i;
	    factor_(&z__1, &z__2);
	    i__3 = i__;
	    z__5.r = a[i__3].r - consts_1.one, z__5.i = a[i__3].i;
	    factor_(&z__4, &z__5);
	    expon = expon + z__1.r - z__4.r;
/* L140: */
	}
	i__1 = *iq;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    i__2 = i__;
	    z__3.r = b[i__2].r + xl, z__3.i = b[i__2].i;
	    z__2.r = z__3.r - consts_1.one, z__2.i = z__3.i;
	    factor_(&z__1, &z__2);
	    i__3 = i__;
	    z__5.r = b[i__3].r - consts_1.one, z__5.i = b[i__3].i;
	    factor_(&z__4, &z__5);
	    expon = expon - z__1.r + z__4.r;
/* L150: */
	}
	z_log(&z__1, z__);
	z__3.r = xl, z__3.i = consts_1.zero;
	factor_(&z__2, &z__3);
	expon = expon + xl * z__1.r - z__2.r;
	d__1 = exp(consts_1.one);
	lmax = (integer) (d_lg10(&d__1) * expon);
	z__1.r = oldtemp.r - temp.r, z__1.i = oldtemp.i - temp.i;
	z__2.r = accy * temp.r, z__2.i = accy * temp.i;
	if (z_abs(&z__1) < z_abs(&z__2)) {
	    goto L240;
	}
	oldtemp.r = temp.r, oldtemp.i = temp.i;
    }
    if (ixcnt == icount) {
	goto L240;
    }
    ++ixcnt;
    i__1 = *iq;
    for (i1 = 1; i1 <= i__1; ++i1) {

/*     TAKE THE CURRENT SUM AND MULTIPLY BY THE DENOMINATOR OF THE NEX
T */
/*     TERM, FOR BOTH THE MOST SIGNIFICANT HALF (CR,CI) AND THE LEAST
*/
/*     SIGNIFICANT HALF (CR2,CI2). */

	cmpmul_(sumr, sumi, &cr[i1 - 1], &ci[i1 - 1], qr1, qi1, wk1, wk2, wk3,
		 wk4, wk5, wk6, &l, &rmax);
	cmpmul_(sumr, sumi, &cr2[i1 - 1], &ci2[i1 - 1], qr2, qi2, wk1, wk2,
		wk3, wk4, wk5, wk6, &l, &rmax);
	qr2[l + 2] += -1;
	qi2[l + 2] += -1;

/*     STORE THIS TEMPORARILY IN THE SUM ARRAYS. */

	cmpadd_(qr1, qi1, qr2, qi2, sumr, sumi, wk1, &l, &rmax);
/* L160: */
    }

/*     MULTIPLY BY THE FACTORIAL TERM. */

    armult_(sumr, &cnt, sumr, wk6, &l, &rmax);
    armult_(sumi, &cnt, sumi, wk6, &l, &rmax);

/*     MULTIPLY BY THE SCALING FACTOR, SIGFIG, TO KEEP THE SCALE CORRECT.
*/

    i__1 = *ip - *iq;
    for (i1 = 1; i1 <= i__1; ++i1) {
	armult_(sumr, &sigfig, sumr, wk6, &l, &rmax);
	armult_(sumi, &sigfig, sumi, wk6, &l, &rmax);
/* L170: */
    }
    i__1 = *iq;
    for (i1 = 1; i1 <= i__1; ++i1) {

/*     UPDATE THE DENOMINATOR. */

	cmpmul_(denomr, denomi, &cr[i1 - 1], &ci[i1 - 1], qr1, qi1, wk1, wk2,
		wk3, wk4, wk5, wk6, &l, &rmax);
	cmpmul_(denomr, denomi, &cr2[i1 - 1], &ci2[i1 - 1], qr2, qi2, wk1,
		wk2, wk3, wk4, wk5, wk6, &l, &rmax);
	qr2[l + 2] += -1;
	qi2[l + 2] += -1;
	cmpadd_(qr1, qi1, qr2, qi2, denomr, denomi, wk1, &l, &rmax);
/* L180: */
    }

/*     MULTIPLY BY THE FACTORIAL TERM. */

    armult_(denomr, &cnt, denomr, wk6, &l, &rmax);
    armult_(denomi, &cnt, denomi, wk6, &l, &rmax);

/*     MULTIPLY BY THE SCALING FACTOR, SIGFIG, TO KEEP THE SCALE CORRECT.
*/

    i__1 = *ip - *iq;
    for (i1 = 1; i1 <= i__1; ++i1) {
	armult_(denomr, &sigfig, denomr, wk6, &l, &rmax);
	armult_(denomi, &sigfig, denomi, wk6, &l, &rmax);
/* L190: */
    }

/*     FORM THE NEXT NUMERATOR TERM BY MULTIPLYING THE CURRENT */
/*     NUMERATOR TERM (AN ARRAY) WITH THE A ARGUMENT (A SCALAR). */

    i__1 = *ip;
    for (i1 = 1; i1 <= i__1; ++i1) {
	cmpmul_(numr, numi, &ar[i1 - 1], &ai[i1 - 1], qr1, qi1, wk1, wk2, wk3,
		 wk4, wk5, wk6, &l, &rmax);
	cmpmul_(numr, numi, &ar2[i1 - 1], &ai2[i1 - 1], qr2, qi2, wk1, wk2,
		wk3, wk4, wk5, wk6, &l, &rmax);
	qr2[l + 2] += -1;
	qi2[l + 2] += -1;
	cmpadd_(qr1, qi1, qr2, qi2, numr, numi, wk1, &l, &rmax);
/* L200: */
    }

/*     FINISH THE NEW NUMERATOR TERM BY MULTIPLYING BY THE Z ARGUMENT. */

    cmpmul_(numr, numi, &xr, &xi, qr1, qi1, wk1, wk2, wk3, wk4, wk5, wk6, &l,
	    &rmax);
    cmpmul_(numr, numi, &xr2, &xi2, qr2, qi2, wk1, wk2, wk3, wk4, wk5, wk6, &
	    l, &rmax);
    qr2[l + 2] += -1;
    qi2[l + 2] += -1;
    cmpadd_(qr1, qi1, qr2, qi2, numr, numi, wk1, &l, &rmax);

/*     MULTIPLY BY THE SCALING FACTOR, SIGFIG, TO KEEP THE SCALE CORRECT.
*/

    i__1 = *iq - *ip;
    for (i1 = 1; i1 <= i__1; ++i1) {
	armult_(numr, &sigfig, numr, wk6, &l, &rmax);
	armult_(numi, &sigfig, numi, wk6, &l, &rmax);
/* L210: */
    }

/*     FINALLY, ADD THE NEW NUMERATOR TERM WITH THE CURRENT RUNNING */
/*     SUM OF THE NUMERATOR AND STORE THE NEW RUNNING SUM IN SUMR, SUMI.
*/

    cmpadd_(sumr, sumi, numr, numi, sumr, sumi, wk1, &l, &rmax);

/*     BECAUSE SIGFIG REPRESENTS "ONE" ON THE NEW SCALE, ADD SIGFIG */
/*     TO THE CURRENT COUNT AND, CONSEQUENTLY, TO THE IP ARGUMENTS */
/*     IN THE NUMERATOR AND THE IQ ARGUMENTS IN THE DENOMINATOR. */

    cnt += sigfig;
    i__1 = *ip;
    for (i1 = 1; i1 <= i__1; ++i1) {
	ar[i1 - 1] += sigfig;
/* L220: */
    }
    i__1 = *iq;
    for (i1 = 1; i1 <= i__1; ++i1) {
	cr[i1 - 1] += sigfig;
/* L230: */
    }
    goto L130;
L240:
    arydiv_(sumr, sumi, denomr, denomi, &final, &l, lnpfq, &rmax, &ibit, ierr,
	     errstr, errstr_len);
/*     write (6,*) 'Number of terms=',ixcnt */
     ret_val->r = final.r,  ret_val->i = final.i;
    return ;
} /* hyper_ */

/*     **************************************************************** */
/*     *                                                              * */
/*     *                 SUBROUTINE ARADD                             * */
/*     *                                                              * */
/*     *                                                              * */
/*     *  Description : Accepts two arrays of numbers and returns     * */
/*     *    the sum of the array.  Each array is holding the value    * */
/*     *    of one number in the series.  The parameter L is the      * */
/*     *    size of the array representing the number and RMAX is     * */
/*     *    the actual number of digits needed to give the numbers    * */
/*     *    the desired accuracy.                                     * */
/*     *                                                              * */
/*     *  Subprograms called: none                                    * */
/*     *                                                              * */
/*     **************************************************************** */
/* Subroutine */ int aradd_(a, b, c__, z__, l, rmax)
doublereal *a, *b, *c__, *z__;
integer *l;
doublereal *rmax;
{
    /* System generated locals */
    integer i__1;
    doublereal d__1;

    /* Builtin functions */
    double d_nint();

    /* Local variables */
    static integer i__, j, ediff;

    /* Parameter adjustments */
    ++z__;
    ++c__;
    ++b;
    ++a;

    /* Function Body */
    i__1 = *l + 1;
    for (i__ = 0; i__ <= i__1; ++i__) {
	z__[i__] = consts_1.zero;
/* L10: */
    }
    d__1 = a[*l + 1] - b[*l + 1];
    ediff = (integer) d_nint(&d__1);
    if (abs(a[1]) < consts_1.half || ediff <= -(*l)) {
	goto L20;
    }
    if (abs(b[1]) < consts_1.half || ediff >= *l) {
	goto L40;
    }
    goto L60;
L20:
    i__1 = *l + 1;
    for (i__ = -1; i__ <= i__1; ++i__) {
	c__[i__] = b[i__];
/* L30: */
    }
    goto L350;
L40:
    i__1 = *l + 1;
    for (i__ = -1; i__ <= i__1; ++i__) {
	c__[i__] = a[i__];
/* L50: */
    }
    goto L350;
L60:
    z__[-1] = a[-1];
    if ((d__1 = a[-1] - b[-1], abs(d__1)) < consts_1.half) {
	goto L80;
    }
    if (ediff > 0) {
	z__[*l + 1] = a[*l + 1];
	goto L190;
    }
    if (ediff < 0) {
	z__[*l + 1] = b[*l + 1];
	z__[-1] = b[-1];
	goto L240;
    }
    i__1 = *l;
    for (i__ = 1; i__ <= i__1; ++i__) {
	if (a[i__] > b[i__]) {
	    z__[*l + 1] = a[*l + 1];
	    goto L190;
	}
	if (a[i__] < b[i__]) {
	    z__[*l + 1] = b[*l + 1];
	    z__[-1] = b[-1];
	    goto L240;
	}
/* L70: */
    }
    goto L300;
L80:
    if (ediff > 0) {
	goto L110;
    }
    if (ediff < 0) {
	goto L150;
    }
    z__[*l + 1] = a[*l + 1];
    for (i__ = *l; i__ >= 1; --i__) {
	z__[i__] = a[i__] + b[i__] + z__[i__];
	if (z__[i__] >= *rmax) {
	    z__[i__] -= *rmax;
	    z__[i__ - 1] = consts_1.one;
	}
/* L90: */
    }
    if (z__[0] > consts_1.half) {
	for (i__ = *l; i__ >= 1; --i__) {
	    z__[i__] = z__[i__ - 1];
/* L100: */
	}
	z__[*l + 1] += consts_1.one;
	z__[0] = consts_1.zero;
    }
    goto L300;
L110:
    z__[*l + 1] = a[*l + 1];
    i__1 = ediff + 1;
    for (i__ = *l; i__ >= i__1; --i__) {
	z__[i__] = a[i__] + b[i__ - ediff] + z__[i__];
	if (z__[i__] >= *rmax) {
	    z__[i__] -= *rmax;
	    z__[i__ - 1] = consts_1.one;
	}
/* L120: */
    }
    for (i__ = ediff; i__ >= 1; --i__) {
	z__[i__] = a[i__] + z__[i__];
	if (z__[i__] >= *rmax) {
	    z__[i__] -= *rmax;
	    z__[i__ - 1] = consts_1.one;
	}
/* L130: */
    }
    if (z__[0] > consts_1.half) {
	for (i__ = *l; i__ >= 1; --i__) {
	    z__[i__] = z__[i__ - 1];
/* L140: */
	}
	++z__[*l + 1];
	z__[0] = consts_1.zero;
    }
    goto L300;
L150:
    z__[*l + 1] = b[*l + 1];
    i__1 = 1 - ediff;
    for (i__ = *l; i__ >= i__1; --i__) {
	z__[i__] = a[i__ + ediff] + b[i__] + z__[i__];
	if (z__[i__] >= *rmax) {
	    z__[i__] -= *rmax;
	    z__[i__ - 1] = consts_1.one;
	}
/* L160: */
    }
    for (i__ = -ediff; i__ >= 1; --i__) {
	z__[i__] = b[i__] + z__[i__];
	if (z__[i__] >= *rmax) {
	    z__[i__] -= *rmax;
	    z__[i__ - 1] = consts_1.one;
	}
/* L170: */
    }
    if (z__[0] > consts_1.half) {
	for (i__ = *l; i__ >= 1; --i__) {
	    z__[i__] = z__[i__ - 1];
/* L180: */
	}
	z__[*l + 1] += consts_1.one;
	z__[0] = consts_1.zero;
    }
    goto L300;
L190:
    if (ediff > 0) {
	goto L210;
    }
    for (i__ = *l; i__ >= 1; --i__) {
	z__[i__] = a[i__] - b[i__] + z__[i__];
	if (z__[i__] < consts_1.zero) {
	    z__[i__] += *rmax;
	    z__[i__ - 1] = -consts_1.one;
	}
/* L200: */
    }
    goto L290;
L210:
    i__1 = ediff + 1;
    for (i__ = *l; i__ >= i__1; --i__) {
	z__[i__] = a[i__] - b[i__ - ediff] + z__[i__];
	if (z__[i__] < consts_1.zero) {
	    z__[i__] += *rmax;
	    z__[i__ - 1] = -consts_1.one;
	}
/* L220: */
    }
    for (i__ = ediff; i__ >= 1; --i__) {
	z__[i__] = a[i__] + z__[i__];
	if (z__[i__] < consts_1.zero) {
	    z__[i__] += *rmax;
	    z__[i__ - 1] = -consts_1.one;
	}
/* L230: */
    }
    goto L290;
L240:
    if (ediff < 0) {
	goto L260;
    }
    for (i__ = *l; i__ >= 1; --i__) {
	z__[i__] = b[i__] - a[i__] + z__[i__];
	if (z__[i__] < consts_1.zero) {
	    z__[i__] += *rmax;
	    z__[i__ - 1] = -consts_1.one;
	}
/* L250: */
    }
    goto L290;
L260:
    i__1 = 1 - ediff;
    for (i__ = *l; i__ >= i__1; --i__) {
	z__[i__] = b[i__] - a[i__ + ediff] + z__[i__];
	if (z__[i__] < consts_1.zero) {
	    z__[i__] += *rmax;
	    z__[i__ - 1] = -consts_1.one;
	}
/* L270: */
    }
    for (i__ = -ediff; i__ >= 1; --i__) {
	z__[i__] = b[i__] + z__[i__];
	if (z__[i__] < consts_1.zero) {
	    z__[i__] += *rmax;
	    z__[i__ - 1] = -consts_1.one;
	}
/* L280: */
    }
L290:
    if (z__[1] > consts_1.half) {
	goto L330;
    }
    i__ = 1;
L300:
    ++i__;
    if (z__[i__] < consts_1.half && i__ < *l + 1) {
	goto L300;
    }
    if (i__ == *l + 1) {
	z__[-1] = consts_1.one;
	z__[*l + 1] = consts_1.zero;
	goto L330;
    }
    i__1 = *l + 1 - i__;
    for (j = 1; j <= i__1; ++j) {
	z__[j] = z__[j + i__ - 1];
/* L310: */
    }
    i__1 = *l;
    for (j = *l + 2 - i__; j <= i__1; ++j) {
	z__[j] = consts_1.zero;
/* L320: */
    }
    z__[*l + 1] = z__[*l + 1] - i__ + 1;
L330:
    i__1 = *l + 1;
    for (i__ = -1; i__ <= i__1; ++i__) {
	c__[i__] = z__[i__];
/* L340: */
    }
L350:
    if (c__[1] < consts_1.half) {
	c__[-1] = consts_1.one;
	c__[*l + 1] = consts_1.zero;
    }
    return 0;
} /* aradd_ */

/*     **************************************************************** */
/*     *                                                              * */
/*     *                 SUBROUTINE ARSUB                             * */
/*     *                                                              * */
/*     *                                                              * */
/*     *  Description : Accepts two arrays and subtracts each element * */
/*     *    in the second array from the element in the first array   * */
/*     *    and returns the solution.  The parameters L and RMAX are  * */
/*     *    the size of the array and the number of digits needed for * */
/*     *    the accuracy, respectively.                               * */
/*     *                                                              * */
/*     *  Subprograms called: ARADD                                   * */
/*     *                                                              * */
/*     **************************************************************** */
/* Subroutine */ int arsub_(a, b, c__, wk1, wk2, l, rmax)
doublereal *a, *b, *c__, *wk1, *wk2;
integer *l;
doublereal *rmax;
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__;
    extern /* Subroutine */ int aradd_();

    /* Parameter adjustments */
    ++wk2;
    ++wk1;
    ++c__;
    ++b;
    ++a;

    /* Function Body */
    i__1 = *l + 1;
    for (i__ = -1; i__ <= i__1; ++i__) {
	wk2[i__] = b[i__];
/* L10: */
    }
    wk2[-1] = -consts_1.one * wk2[-1];
    aradd_(&a[-1], &wk2[-1], &c__[-1], &wk1[-1], l, rmax);
    return 0;
} /* arsub_ */

/*     **************************************************************** */
/*     *                                                              * */
/*     *                 SUBROUTINE ARMULT                            * */
/*     *                                                              * */
/*     *                                                              * */
/*     *  Description : Accepts two arrays and returns the product.   * */
/*     *    L and RMAX are the size of the arrays and the number of   * */
/*     *    digits needed to represent the numbers with the required  * */
/*     *    accuracy.                                                 * */
/*     *                                                              * */
/*     *  Subprograms called: none                                    * */
/*     *                                                              * */
/*     **************************************************************** */
/* Subroutine */ int armult_(a, b, c__, z__, l, rmax)
doublereal *a, *b, *c__, *z__;
integer *l;
doublereal *rmax;
{
    /* System generated locals */
    integer i__1;
    doublereal d__1;

    /* Builtin functions */
    double d_sign(), d_int();

    /* Local variables */
    static integer i__;
    static doublereal carry, b2;

    /* Parameter adjustments */
    ++z__;
    ++c__;
    ++a;

    /* Function Body */
    z__[-1] = d_sign(&consts_1.one, b) * a[-1];
    b2 = abs(*b);
    z__[*l + 1] = a[*l + 1];
    i__1 = *l;
    for (i__ = 0; i__ <= i__1; ++i__) {
	z__[i__] = consts_1.zero;
/* L10: */
    }
    if (b2 <= consts_1.eps || a[1] <= consts_1.eps) {
	z__[-1] = consts_1.one;
	z__[*l + 1] = consts_1.zero;
	goto L60;
    }
    for (i__ = *l; i__ >= 1; --i__) {
	z__[i__] = a[i__] * b2 + z__[i__];
	if (z__[i__] >= *rmax) {
	    d__1 = z__[i__] / *rmax;
	    carry = d_int(&d__1);
	    z__[i__] -= carry * *rmax;
	    z__[i__ - 1] = carry;
	}
/* L20: */
    }
    if (z__[0] < consts_1.half) {
	goto L50;
    }
    for (i__ = *l; i__ >= 1; --i__) {
	z__[i__] = z__[i__ - 1];
/* L30: */
    }
    z__[*l + 1] += consts_1.one;
    if (z__[1] >= *rmax) {
	for (i__ = *l; i__ >= 1; --i__) {
	    z__[i__] = z__[i__ - 1];
/* L40: */
	}
	d__1 = z__[1] / *rmax;
	carry = d_int(&d__1);
	z__[2] -= carry * *rmax;
	z__[1] = carry;
	z__[*l + 1] += consts_1.one;
    }
    z__[0] = consts_1.zero;
L50:
L60:
    i__1 = *l + 1;
    for (i__ = -1; i__ <= i__1; ++i__) {
	c__[i__] = z__[i__];
/* L70: */
    }
    if (c__[1] < consts_1.half) {
	c__[-1] = consts_1.one;
	c__[*l + 1] = consts_1.zero;
    }
    return 0;
} /* armult_ */

/*     **************************************************************** */
/*     *                                                              * */
/*     *                 SUBROUTINE CMPADD                            * */
/*     *                                                              * */
/*     *                                                              * */
/*     *  Description : Takes two arrays representing one real and    * */
/*     *    one imaginary part, and adds two arrays representing      * */
/*     *    another complex number and returns two array holding the  * */
/*     *    complex sum.                                              * */
/*     *              (CR,CI) = (AR+BR, AI+BI)                        * */
/*     *                                                              * */
/*     *  Subprograms called: ARADD                                   * */
/*     *                                                              * */
/*     **************************************************************** */
/* Subroutine */ int cmpadd_(ar, ai, br, bi, cr, ci, wk1, l, rmax)
doublereal *ar, *ai, *br, *bi, *cr, *ci, *wk1;
integer *l;
doublereal *rmax;
{
    extern /* Subroutine */ int aradd_();

    /* Parameter adjustments */
    ++wk1;
    ++ci;
    ++cr;
    ++bi;
    ++br;
    ++ai;
    ++ar;

    /* Function Body */
    aradd_(&ar[-1], &br[-1], &cr[-1], &wk1[-1], l, rmax);
    aradd_(&ai[-1], &bi[-1], &ci[-1], &wk1[-1], l, rmax);
    return 0;
} /* cmpadd_ */

/*     **************************************************************** */
/*     *                                                              * */
/*     *                 SUBROUTINE CMPSUB                            * */
/*     *                                                              * */
/*     *                                                              * */
/*     *  Description : Takes two arrays representing one real and    * */
/*     *    one imaginary part, and subtracts two arrays representing * */
/*     *    another complex number and returns two array holding the  * */
/*     *    complex sum.                                              * */
/*     *              (CR,CI) = (AR+BR, AI+BI)                        * */
/*     *                                                              * */
/*     *  Subprograms called: ARADD                                   * */
/*     *                                                              * */
/*     **************************************************************** */
/* Subroutine */ int cmpsub_(ar, ai, br, bi, cr, ci, wk1, wk2, l, rmax)
doublereal *ar, *ai, *br, *bi, *cr, *ci, *wk1, *wk2;
integer *l;
doublereal *rmax;
{
    extern /* Subroutine */ int arsub_();

    /* Parameter adjustments */
    ++wk2;
    ++wk1;
    ++ci;
    ++cr;
    ++bi;
    ++br;
    ++ai;
    ++ar;

    /* Function Body */
    arsub_(&ar[-1], &br[-1], &cr[-1], &wk1[-1], &wk2[-1], l, rmax);
    arsub_(&ai[-1], &bi[-1], &ci[-1], &wk1[-1], &wk2[-1], l, rmax);
    return 0;
} /* cmpsub_ */

/*     **************************************************************** */
/*     *                                                              * */
/*     *                 SUBROUTINE CMPMUL                            * */
/*     *                                                              * */
/*     *                                                              * */
/*     *  Description : Takes two arrays representing one real and    * */
/*     *    one imaginary part, and multiplies it with two arrays     * */
/*     *    representing another complex number and returns the       * */
/*     *    complex product.                                          * */
/*     *                                                              * */
/*     *  Subprograms called: ARMULT, ARSUB, ARADD                    * */
/*     *                                                              * */
/*     **************************************************************** */
/* Subroutine */ int cmpmul_(ar, ai, br, bi, cr, ci, wk1, wk2, cr2, d1, d2,
	wk6, l, rmax)
doublereal *ar, *ai, *br, *bi, *cr, *ci, *wk1, *wk2, *cr2, *d1, *d2, *wk6;
integer *l;
doublereal *rmax;
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__;
    extern /* Subroutine */ int aradd_(), arsub_(), armult_();

    /* Parameter adjustments */
    ++d2;
    ++d1;
    ++cr2;
    ++wk2;
    ++wk1;
    ++ci;
    ++cr;
    ++ai;
    ++ar;

    /* Function Body */
    armult_(&ar[-1], br, &d1[-1], wk6, l, rmax);
    armult_(&ai[-1], bi, &d2[-1], wk6, l, rmax);
    arsub_(&d1[-1], &d2[-1], &cr2[-1], &wk1[-1], &wk2[-1], l, rmax);
    armult_(&ar[-1], bi, &d1[-1], wk6, l, rmax);
    armult_(&ai[-1], br, &d2[-1], wk6, l, rmax);
    aradd_(&d1[-1], &d2[-1], &ci[-1], &wk1[-1], l, rmax);
    i__1 = *l + 1;
    for (i__ = -1; i__ <= i__1; ++i__) {
	cr[i__] = cr2[i__];
/* L10: */
    }
    return 0;
} /* cmpmul_ */

/*     **************************************************************** */
/*     *                                                              * */
/*     *                 SUBROUTINE ARYDIV                            * */
/*     *                                                              * */
/*     *                                                              * */
/*     *  Description : Returns the double precision complex number   * */
/*     *    resulting from the division of four arrays, representing  * */
/*     *    two complex numbers.  The number returned will be in one  * */
/*     *    of two different forms:  either standard scientific or as * */
/*     *    the log (base 10) of the number.                          * */
/*     *                                                              * */
/*     *  Subprograms called: CONV21, CONV12, EADD, ECPDIV, EMULT.    * */
/*     *                                                              * */
/*     **************************************************************** */
/* Subroutine */ int arydiv_(ar, ai, br, bi, c__, l, lnpfq, rmax, ibit, ierr,
	errstr, errstr_len)
doublereal *ar, *ai, *br, *bi;
doublecomplex *c__;
integer *l, *lnpfq;
doublereal *rmax;
integer *ibit;
bool *ierr;
char *errstr;
ftnlen errstr_len;
{
    /* System generated locals */
    doublereal d__1;
    doublecomplex z__1;

    /* Builtin functions */
    double d_lg10(), d_sign(), pow_dd(), atan2(), log();

    /* Local variables */
    static doublecomplex cdum;
    static real dnum;
    static integer rexp;
    static doublereal x;
    extern /* Subroutine */ int conv12_(), conv21_();
    static doublereal e1, e2, e3;
    extern /* Subroutine */ int emult_();
    static doublereal n1, n2, n3, x1, x2, ae[4]	/* was [2][2] */, be[4]	/*
	    was [2][2] */, ce[4]	/* was [2][2] */;
    extern /* Subroutine */ int ecpdiv_();
    static doublereal tenmax;
    static integer itnmax, ii10, ir10;
    static doublereal ri10, phi, rr10, dum1, dum2;
    extern /* Subroutine */ int eadd_();

    /* Parameter adjustments */
    ++bi;
    ++br;
    ++ai;
    ++ar;

    /* Function Body */
    rexp = *ibit / 2;
    x = rexp * (ar[*l + 1] - 2);
    rr10 = x * d_lg10(&consts_1.two) / d_lg10(&consts_1.ten);
    ir10 = (integer) rr10;
    rr10 -= ir10;
    x = rexp * (ai[*l + 1] - 2);
    ri10 = x * d_lg10(&consts_1.two) / d_lg10(&consts_1.ten);
    ii10 = (integer) ri10;
    ri10 -= ii10;
    d__1 = ar[1] * *rmax * *rmax + ar[2] * *rmax + ar[3];
    dum1 = d_sign(&d__1, &ar[-1]);
    d__1 = ai[1] * *rmax * *rmax + ai[2] * *rmax + ai[3];
    dum2 = d_sign(&d__1, &ai[-1]);
    dum1 *= pow_dd(&c_b65, &rr10);
    dum2 *= pow_dd(&c_b65, &ri10);
    z__1.r = dum1, z__1.i = dum2;
    cdum.r = z__1.r, cdum.i = z__1.i;
    conv12_(&cdum, ae);
    ae[2] += ir10;
    ae[3] += ii10;
    x = rexp * (br[*l + 1] - 2);
    rr10 = x * d_lg10(&consts_1.two) / d_lg10(&consts_1.ten);
    ir10 = (integer) rr10;
    rr10 -= ir10;
    x = rexp * (bi[*l + 1] - 2);
    ri10 = x * d_lg10(&consts_1.two) / d_lg10(&consts_1.ten);
    ii10 = (integer) ri10;
    ri10 -= ii10;
    d__1 = br[1] * *rmax * *rmax + br[2] * *rmax + br[3];
    dum1 = d_sign(&d__1, &br[-1]);
    d__1 = bi[1] * *rmax * *rmax + bi[2] * *rmax + bi[3];
    dum2 = d_sign(&d__1, &bi[-1]);
    dum1 *= pow_dd(&c_b65, &rr10);
    dum2 *= pow_dd(&c_b65, &ri10);
    z__1.r = dum1, z__1.i = dum2;
    cdum.r = z__1.r, cdum.i = z__1.i;
    conv12_(&cdum, be);
    be[2] += ir10;
    be[3] += ii10;
    ecpdiv_(ae, be, ce);
    if (*lnpfq == 0) {
	conv21_(ce, c__, ierr, errstr, errstr_len);
	if (*ierr == 1) {
	    return 0;
	}
    } else {
	emult_(ce, &ce[2], ce, &ce[2], &n1, &e1);
	emult_(&ce[1], &ce[3], &ce[1], &ce[3], &n2, &e2);
	eadd_(&n1, &e1, &n2, &e2, &n3, &e3);
	n1 = ce[0];
	e1 = ce[2] - ce[3];
	x2 = ce[1];

/*     TENMAX - MAXIMUM SIZE OF EXPONENT OF 10 */
/*     THE FOLLOWING CODE CAN BE USED TO DETERMINE TENMAX, BUT IT */
/*     WILL LIKELY GENERATE AN IEEE FLOATING POINT UNDERFLOW ERROR */
/*     ON A SUN WORKSTATION.  REPLACE TENMAX WITH THE VALUE APPROPRIAT
E */
/*     FOR YOUR MACHINE. */

	tenmax = 320.;
	itnmax = 1;
	dnum = (float).1;
L10:
	++itnmax;
	dnum *= .1;
	if (dnum > consts_1.zero) {
	    goto L10;
	}
	--itnmax;
	tenmax = (doublereal) itnmax;

	if (e1 > tenmax) {
	    x1 = tenmax;
	} else if (e1 < -tenmax) {
	    x1 = consts_1.zero;
	} else {
	    x1 = n1 * pow_dd(&consts_1.ten, &e1);
	}
	if (x2 != consts_1.zero) {
	    phi = atan2(x2, x1);
	} else {
	    phi = consts_1.zero;
	}
	d__1 = consts_1.half * (log(n3) + e3 * log(consts_1.ten));
	z__1.r = d__1, z__1.i = phi;
	c__->r = z__1.r, c__->i = z__1.i;
    }
    return 0;
} /* arydiv_ */

/*     **************************************************************** */
/*     *                                                              * */
/*     *                 SUBROUTINE EMULT                             * */
/*     *                                                              * */
/*     *                                                              * */
/*     *  Description : Takes one base and exponent and multiplies it * */
/*     *    by another numbers base and exponent to give the product  * */
/*     *    in the form of base and exponent.                         * */
/*     *                                                              * */
/*     *  Subprograms called: none                                    * */
/*     *                                                              * */
/*     **************************************************************** */
/* Subroutine */ int emult_(n1, e1, n2, e2, nf, ef)
doublereal *n1, *e1, *n2, *e2, *nf, *ef;
{
    *nf = *n1 * *n2;
    *ef = *e1 + *e2;
    if (abs(*nf) >= consts_1.ten) {
	*nf /= consts_1.ten;
	*ef += consts_1.one;
    }
    return 0;
} /* emult_ */

/*     **************************************************************** */
/*     *                                                              * */
/*     *                 SUBROUTINE EDIV                              * */
/*     *                                                              * */
/*     *                                                              * */
/*     *  Description : returns the solution in the form of base and  * */
/*     *    exponent of the division of two exponential numbers.      * */
/*     *                                                              * */
/*     *  Subprograms called: none                                    * */
/*     *                                                              * */
/*     **************************************************************** */
/* Subroutine */ int ediv_(n1, e1, n2, e2, nf, ef)
doublereal *n1, *e1, *n2, *e2, *nf, *ef;
{
    *nf = *n1 / *n2;
    *ef = *e1 - *e2;
    if (abs(*nf) < consts_1.one && *nf != consts_1.zero) {
	*nf *= consts_1.ten;
	*ef -= consts_1.one;
    }
    return 0;
} /* ediv_ */

/*     **************************************************************** */
/*     *                                                              * */
/*     *                 SUBROUTINE EADD                              * */
/*     *                                                              * */
/*     *                                                              * */
/*     *  Description : Returns the sum of two numbers in the form    * */
/*     *    of a base and an exponent.                                * */
/*     *                                                              * */
/*     *  Subprograms called: none                                    * */
/*     *                                                              * */
/*     **************************************************************** */
/* Subroutine */ int eadd_(n1, e1, n2, e2, nf, ef)
doublereal *n1, *e1, *n2, *e2, *nf, *ef;
{
    /* Builtin functions */
    double pow_dd();

    /* Local variables */
    static doublereal ediff;

    ediff = *e1 - *e2;
    if (ediff > 36.) {
	*nf = *n1;
	*ef = *e1;
    } else if (ediff < -36.) {
	*nf = *n2;
	*ef = *e2;
    } else {
	*nf = *n1 * pow_dd(&consts_1.ten, &ediff) + *n2;
	*ef = *e2;
L10:
	if (abs(*nf) < consts_1.ten) {
	    goto L20;
	}
	*nf /= consts_1.ten;
	*ef += consts_1.one;
	goto L10;
L20:
	if (abs(*nf) >= consts_1.one || *nf == consts_1.zero) {
	    goto L30;
	}
	*nf *= consts_1.ten;
	*ef -= consts_1.one;
	goto L20;
    }
L30:
    return 0;
} /* eadd_ */

/*     **************************************************************** */
/*     *                                                              * */
/*     *                 SUBROUTINE ESUB                              * */
/*     *                                                              * */
/*     *                                                              * */
/*     *  Description : Returns the solution to the subtraction of    * */
/*     *    two numbers in the form of base and exponent.             * */
/*     *                                                              * */
/*     *  Subprograms called: EADD                                    * */
/*     *                                                              * */
/*     **************************************************************** */
/* Subroutine */ int esub_(n1, e1, n2, e2, nf, ef)
doublereal *n1, *e1, *n2, *e2, *nf, *ef;
{
    /* System generated locals */
    doublereal d__1;

    /* Local variables */
    extern /* Subroutine */ int eadd_();

    d__1 = *n2 * (-consts_1.one);
    eadd_(n1, e1, &d__1, e2, nf, ef);
    return 0;
} /* esub_ */

/*     **************************************************************** */
/*     *                                                              * */
/*     *                 SUBROUTINE CONV12                            * */
/*     *                                                              * */
/*     *                                                              * */
/*     *  Description : Converts a number from complex notation to a  * */
/*     *    form of a 2x2 real array.                                 * */
/*     *                                                              * */
/*     *  Subprograms called: none                                    * */
/*     *                                                              * */
/*     **************************************************************** */
/* Subroutine */ int conv12_(cn, cae)
doublecomplex *cn;
doublereal *cae;
{
    /* Builtin functions */
    double d_imag();

    /* Parameter adjustments */
    cae -= 3;

    /* Function Body */
    cae[3] = cn->r;
    cae[5] = consts_1.zero;
L10:
    if (abs(cae[3]) < consts_1.ten) {
	goto L20;
    }
    cae[3] /= consts_1.ten;
    cae[5] += consts_1.one;
    goto L10;
L20:
    if (abs(cae[3]) >= consts_1.one || cae[3] == consts_1.zero) {
	goto L30;
    }
    cae[3] *= consts_1.ten;
    cae[5] -= consts_1.one;
    goto L20;
L30:
    cae[4] = d_imag(cn);
    cae[6] = consts_1.zero;
L40:
    if (abs(cae[4]) < consts_1.ten) {
	goto L50;
    }
    cae[4] /= consts_1.ten;
    cae[6] += consts_1.one;
    goto L40;
L50:
    if (abs(cae[4]) >= consts_1.one || cae[4] == consts_1.zero) {
	goto L60;
    }
    cae[4] *= consts_1.ten;
    cae[6] -= consts_1.one;
    goto L50;
L60:
    return 0;
} /* conv12_ */

/*     **************************************************************** */
/*     *                                                              * */
/*     *                 SUBROUTINE CONV21                            * */
/*     *                                                              * */
/*     *                                                              * */
/*     *  Description : Converts a number represented in a 2x2 real   * */
/*     *    array to the form of a complex number.                    * */
/*     *                                                              * */
/*     *  Subprograms called: none                                    * */
/*     *                                                              * */
/*     **************************************************************** */
/* Subroutine */ int conv21_(cae, cn, ierr, errstr, errstr_len)
doublereal *cae;
doublecomplex *cn;
bool *ierr;
char *errstr;
ftnlen errstr_len;
{
    /* Format strings */
    static char fmt_300[] = "(\002 ERROR - VALUE OF EXPONENT REQUIRED FOR SU\
MMATION\002,\002 WAS LARGER\002,/,\002 THAN THE MAXIMUM MACHINE EXPONENT \
\002,1i3,/,\002 SUGGESTIONS:\002,/,\002 1) RE-RUN USING LNPFQ=1.\002,/,\002 \
2) IF YOU ARE USING A VAX, TRY USING THE\002,\002 FORTRAN/G_FLOATING OPTIO\
N\002)";

    /* System generated locals */
    doublereal d__1, d__2;
    doublecomplex z__1;
    icilist ici__1;

    /* Builtin functions */
    integer s_wsfi(), do_fio(), e_wsfi();
    /* Subroutine */ int s_stop();
    double pow_dd();

    /* Local variables */
    static doublereal dnum, tenmax;
    static integer itnmax;


/*     TENMAX - MAXIMUM SIZE OF EXPONENT OF 10 */

    /* Parameter adjustments */
    cae -= 3;

    /* Function Body */
    itnmax = 1;
    dnum = .1;
L1:
    ++itnmax;
    dnum *= .1;
    if (dnum > consts_1.zero) {
	goto L1;
    }
    itnmax += -2;
    tenmax = (doublereal) itnmax;

    if (cae[5] > tenmax || cae[6] > tenmax) {
/*     CN=DCMPLX(TENMAX,TENMAX) */
	ici__1.icierr = 0;
	ici__1.icirnum = 1;
	ici__1.icirlen = 256;
	ici__1.iciunit = errstr;
	ici__1.icifmt = fmt_300;
	s_wsfi(&ici__1);
	do_fio(&c__1, (char *)&itnmax, (ftnlen)sizeof(integer));
	e_wsfi();
	cn->r = 9.9999e11, cn->i = 9.9999e11;
	*ierr = 1;
	return 0;
	s_stop("", 0L);
    } else if (cae[6] < -tenmax) {
	d__1 = cae[3] * pow_dd(&c_b65, &cae[5]);
	z__1.r = d__1, z__1.i = consts_1.zero;
	cn->r = z__1.r, cn->i = z__1.i;
    } else {
	d__1 = cae[3] * pow_dd(&c_b65, &cae[5]);
	d__2 = cae[4] * pow_dd(&c_b65, &cae[6]);
	z__1.r = d__1, z__1.i = d__2;
	cn->r = z__1.r, cn->i = z__1.i;
    }
    return 0;
} /* conv21_ */

/*     **************************************************************** */
/*     *                                                              * */
/*     *                 SUBROUTINE ECPMUL                            * */
/*     *                                                              * */
/*     *                                                              * */
/*     *  Description : Multiplies two numbers which are each         * */
/*     *    represented in the form of a two by two array and returns * */
/*     *    the solution in the same form.                            * */
/*     *                                                              * */
/*     *  Subprograms called: EMULT, ESUB, EADD                       * */
/*     *                                                              * */
/*     **************************************************************** */
/* Subroutine */ int ecpmul_(a, b, c__)
doublereal *a, *b, *c__;
{
    extern /* Subroutine */ int esub_();
    static doublereal c2[4]	/* was [2][2] */, e1, e2;
    extern /* Subroutine */ int emult_();
    static doublereal n1, n2;
    extern /* Subroutine */ int eadd_();

    /* Parameter adjustments */
    c__ -= 3;
    b -= 3;
    a -= 3;

    /* Function Body */
    emult_(&a[3], &a[5], &b[3], &b[5], &n1, &e1);
    emult_(&a[4], &a[6], &b[4], &b[6], &n2, &e2);
    esub_(&n1, &e1, &n2, &e2, c2, &c2[2]);
    emult_(&a[3], &a[5], &b[4], &b[6], &n1, &e1);
    emult_(&a[4], &a[6], &b[3], &b[5], &n2, &e2);
    eadd_(&n1, &e1, &n2, &e2, &c__[4], &c__[6]);
    c__[3] = c2[0];
    c__[5] = c2[2];
    return 0;
} /* ecpmul_ */

/*     **************************************************************** */
/*     *                                                              * */
/*     *                 SUBROUTINE ECPDIV                            * */
/*     *                                                              * */
/*     *                                                              * */
/*     *  Description : Divides two numbers and returns the solution. * */
/*     *    All numbers are represented by a 2x2 array.               * */
/*     *                                                              * */
/*     *  Subprograms called: EADD, ECPMUL, EDIV, EMULT               * */
/*     *                                                              * */
/*     **************************************************************** */
/* Subroutine */ int ecpdiv_(a, b, c__)
doublereal *a, *b, *c__;
{
    extern /* Subroutine */ int ediv_();
    static doublereal b2[4]	/* was [2][2] */, c2[4]	/* was [2][2] */, e1,
	    e2, e3;
    extern /* Subroutine */ int emult_();
    static doublereal n1, n2, n3;
    extern /* Subroutine */ int ecpmul_(), eadd_();

    /* Parameter adjustments */
    c__ -= 3;
    b -= 3;
    a -= 3;

    /* Function Body */
    b2[0] = b[3];
    b2[2] = b[5];
    b2[1] = -consts_1.one * b[4];
    b2[3] = b[6];
    ecpmul_(&a[3], b2, c2);
    emult_(&b[3], &b[5], &b[3], &b[5], &n1, &e1);
    emult_(&b[4], &b[6], &b[4], &b[6], &n2, &e2);
    eadd_(&n1, &e1, &n2, &e2, &n3, &e3);
    ediv_(c2, &c2[2], &n3, &e3, &c__[3], &c__[5]);
    ediv_(&c2[1], &c2[3], &n3, &e3, &c__[4], &c__[6]);
    return 0;
} /* ecpdiv_ */

/*     **************************************************************** */
/*     *                                                              * */
/*     *                   FUNCTION IPREMAX                           * */
/*     *                                                              * */
/*     *                                                              * */
/*     *  Description : Predicts the maximum term in the pFq series   * */
/*     *    via a simple scanning of arguments.                       * */
/*     *                                                              * */
/*     *  Subprograms called: none.                                   * */
/*     *                                                              * */
/*     **************************************************************** */
integer ipremax_(a, b, ip, iq, z__, ierr, errstr, errstr_len)
doublecomplex *a, *b;
integer *ip, *iq;
doublecomplex *z__;
bool *ierr;
char *errstr;
ftnlen errstr_len;
{
    /* System generated locals */
    integer ret_val, i__1, i__2, i__3;
    doublereal d__1;
    doublecomplex z__1, z__2, z__3, z__4, z__5;
    icilist ici__1;

    /* Builtin functions */
    void z_log();
    double exp(), d_lg10();
    integer s_wsli(), do_lio(), e_wsli();
    /* Subroutine */ int s_stop();

    /* Local variables */
    static doublereal xmax;
    static integer i__, j;
    static doublereal expon, xterm, xl;
    extern /* Double Complex */ VOID factor_();


    /* Parameter adjustments */
    --a;
    --b;

    /* Function Body */
    xterm = 0.;
    for (j = 1; j <= 100000; ++j) {

/*     Estimate the exponent of the maximum term in the pFq series. */

	expon = consts_1.zero;
	xl = (doublereal) j;
	i__1 = *ip;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    i__2 = i__;
	    z__3.r = a[i__2].r + xl, z__3.i = a[i__2].i;
	    z__2.r = z__3.r - consts_1.one, z__2.i = z__3.i;
	    factor_(&z__1, &z__2);
	    i__3 = i__;
	    z__5.r = a[i__3].r - consts_1.one, z__5.i = a[i__3].i;
	    factor_(&z__4, &z__5);
	    expon = expon + z__1.r - z__4.r;
/* L20: */
	}
	i__1 = *iq;
	for (i__ = 1; i__ <= i__1; ++i__) {
	    i__2 = i__;
	    z__3.r = b[i__2].r + xl, z__3.i = b[i__2].i;
	    z__2.r = z__3.r - consts_1.one, z__2.i = z__3.i;
	    factor_(&z__1, &z__2);
	    i__3 = i__;
	    z__5.r = b[i__3].r - consts_1.one, z__5.i = b[i__3].i;
	    factor_(&z__4, &z__5);
	    expon = expon - z__1.r + z__4.r;
/* L30: */
	}
	z_log(&z__1, z__);
	z__3.r = xl, z__3.i = consts_1.zero;
	factor_(&z__2, &z__3);
	expon = expon + xl * z__1.r - z__2.r;
	d__1 = exp(consts_1.one);
	xmax = d_lg10(&d__1) * expon;
	if (xmax < xterm && j > 2) {
	    ret_val = j;
	    return ret_val;
	}
	xterm = max(xmax,xterm);
/* L10: */
    }
    ici__1.icierr = 0;
    ici__1.icirnum = 1;
    ici__1.icirlen = 256;
    ici__1.iciunit = errstr;
    ici__1.icifmt = 0;
    s_wsli(&ici__1);
    do_lio(&c__9, &c__1, "ERROR IN IPREMAX: DID NOT FIND MAXIMUM EXPONENT",
	    47L);
    e_wsli();
    ret_val = 999;
    *ierr = 1;
    return ret_val;
    s_stop("", 0L);
    return ret_val;
} /* ipremax_ */

/*     **************************************************************** */
/*     *                                                              * */
/*     *                   FUNCTION FACTOR                            * */
/*     *                                                              * */
/*     *                                                              * */
/*     *  Description : This function is the log of the factorial.    * */
/*     *                                                              * */
/*     *  Subprograms called: none.                                   * */
/*     *                                                              * */
/*     **************************************************************** */
/* Double Complex */ VOID factor_( ret_val, z__)
doublecomplex * ret_val;
doublecomplex *z__;
{
    /* System generated locals */
    doublereal d__1;
    doublecomplex z__1, z__2, z__3, z__4, z__5, z__6, z__7, z__8, z__9, z__10,
	     z__11, z__12, z__13, z__14, z__15, z__16, z__17, z__18, z__19,
	    z__20, z__21;

    /* Builtin functions */
    double d_imag(), z_abs(), atan(), log();
    void z_log(), z_div();

    /* Local variables */
    static doublereal pi;


    if ((z__->r == consts_1.one && d_imag(z__) == consts_1.zero) || z_abs(z__)
	    == consts_1.zero) {
	z__1.r = consts_1.zero, z__1.i = consts_1.zero;
	 ret_val->r = z__1.r,  ret_val->i = z__1.i;
	return ;
    }
    pi = consts_1.two * consts_1.two * atan(consts_1.one);
    d__1 = consts_1.half * log(consts_1.two * pi);
    z__5.r = z__->r + consts_1.half, z__5.i = z__->i;
    z_log(&z__6, z__);
    z__4.r = z__5.r * z__6.r - z__5.i * z__6.i, z__4.i = z__5.r * z__6.i +
	    z__5.i * z__6.r;
    z__3.r = d__1 + z__4.r, z__3.i = z__4.i;
    z__2.r = z__3.r - z__->r, z__2.i = z__3.i - z__->i;
    z__9.r = consts_1.one, z__9.i = 0.;
    z__10.r = z__->r * 12., z__10.i = z__->i * 12.;
    z_div(&z__8, &z__9, &z__10);
    z__14.r = consts_1.one, z__14.i = 0.;
    z__16.r = z__->r * 30., z__16.i = z__->i * 30.;
    z__15.r = z__16.r * z__->r - z__16.i * z__->i, z__15.i = z__16.r * z__->i
	    + z__16.i * z__->r;
    z_div(&z__13, &z__14, &z__15);
    z__19.r = consts_1.two, z__19.i = 0.;
    z__21.r = z__->r * 7., z__21.i = z__->i * 7.;
    z__20.r = z__21.r * z__->r - z__21.i * z__->i, z__20.i = z__21.r * z__->i
	    + z__21.i * z__->r;
    z_div(&z__18, &z__19, &z__20);
    z__17.r = consts_1.one - z__18.r, z__17.i = -z__18.i;
    z__12.r = z__13.r * z__17.r - z__13.i * z__17.i, z__12.i = z__13.r *
	    z__17.i + z__13.i * z__17.r;
    z__11.r = consts_1.one - z__12.r, z__11.i = -z__12.i;
    z__7.r = z__8.r * z__11.r - z__8.i * z__11.i, z__7.i = z__8.r * z__11.i +
	    z__8.i * z__11.r;
    z__1.r = z__2.r + z__7.r, z__1.i = z__2.i + z__7.i;
     ret_val->r = z__1.r,  ret_val->i = z__1.i;
    return ;
} /* factor_ */

/*     **************************************************************** */
/*     *                                                              * */
/*     *                   FUNCTION CGAMMA                            * */
/*     *                                                              * */
/*     *                                                              * */
/*     *  Description : Calculates the complex gamma function.  Based * */
/*     *     on a program written by F.A. Parpia published in Computer* */
/*     *     Physics Communications as the `GRASP2' program (public   * */
/*     *     domain).                                                 * */
/*     *                                                              * */
/*     *                                                              * */
/*     *  Subprograms called: none.                                   * */
/*     *                                                              * */
/*     **************************************************************** */
/* Double Complex */ VOID cgamma_( ret_val, arg, lnpfq, ierr, errstr,
	errstr_len)
doublecomplex * ret_val;
doublecomplex *arg;
integer *lnpfq;
bool *ierr;
char *errstr;
ftnlen errstr_len;
{
    /* Initialized data */

    static doublereal fn[7] = { 1.,-1.,1.,-1.,5.,-691.,7. };
    static doublereal fd[7] = { 6.,30.,42.,30.,66.,2730.,6. };
    static doublereal hlntpi = 1.;
    static logical first = TRUE_;
    static doublereal tenth = .1;

    /* Format strings */
    static char fmt_301[] = "(\002 ARGUMENT (\002,1p,1d14.7,\002,\002,1d14\
.7,\002) TOO CLOSE TO A\002,\002 POLE. ***** ERROR IN SUBROUTINE CGAMMA *****\
\002)";
    static char fmt_302[] = "(\002 ARGUMENT TO EXPONENTIAL FUNCTION (\002,1p\
,1d14.7,\002) OUT OF RANGE. ***** ERROR IN SUBROUTINE CGAMMA *****\002)";

    /* System generated locals */
    doublereal d__1;
    doublecomplex z__1;
    icilist ici__1;

    /* Builtin functions */
    double d_imag(), atan(), pow_di(), log();
    integer i_dnnt(), s_wsfi(), do_fio(), e_wsfi();
    /* Subroutine */ int s_stop();
    double sin(), sqrt(), atan2(), exp(), cos();

    /* Local variables */
    static doublereal diff, argi, argr, resi, dnum, resr, twoi;
    static integer i__;
    static doublereal clngi, zfaci, clngr, obasq, argui, zfacr, argum, ovlfi,
	    argur, termi, ovlfr, termr, argui2, argur2, facneg, pi;
    static logical negarg;
    static doublereal ovlfac, obasqi, precis, obasqr, tenmax;
    static integer itnmax;
    static doublereal expmax, fac;




/* ----------------------------------------------------------------------*
 */
/*     * */
/*    THESE ARE THE BERNOULLI NUMBERS B02, B04, ..., B14, EXPRESSED AS
**/
/*    RATIONAL NUMBERS. FROM ABRAMOWITZ AND STEGUN, P. 810.
**/


/* ----------------------------------------------------------------------*
 */




    argr = arg->r;
    argi = d_imag(arg);

/*     ON THE FIRST ENTRY TO THIS ROUTINE, SET UP THE CONSTANTS REQUIRED
*/
/*     FOR THE REFLECTION FORMULA (CF. ABRAMOWITZ AND STEGUN 6.1.17) AND
*/
/*     STIRLING'S APPROXIMATION (CF. ABRAMOWITZ AND STEGUN 6.1.40). */

    if (first) {
	pi = atan(consts_1.one) * 4.;

/*     SET THE MACHINE-DEPENDENT PARAMETERS: */

/*     TENMAX - MAXIMUM SIZE OF EXPONENT OF 10 */

	itnmax = 1;
	dnum = tenth;
L10:
	++itnmax;
	dnum *= tenth;
	if (dnum > consts_1.zero) {
	    goto L10;
	}
	--itnmax;
	tenmax = (doublereal) itnmax;

/*     EXPMAX - MAXIMUM SIZE OF EXPONENT OF E */

	dnum = pow_di(&tenth, &itnmax);
	expmax = -log(dnum);

/*     PRECIS - MACHINE PRECISION */

	precis = consts_1.one;
L20:
	precis /= consts_1.two;
	dnum = precis + consts_1.one;
	if (dnum > consts_1.one) {
	    goto L20;
	}
	precis = consts_1.two * precis;

	hlntpi = consts_1.half * log(consts_1.two * pi);

	for (i__ = 1; i__ <= 7; ++i__) {
	    fn[i__ - 1] /= fd[i__ - 1];
	    twoi = consts_1.two * (doublereal) i__;
	    fn[i__ - 1] /= twoi * (twoi - consts_1.one);
/* L30: */
	}

	first = FALSE_;

    }

/*     CASES WHERE THE ARGUMENT IS REAL */

    if (argi == consts_1.zero) {

/*     CASES WHERE THE ARGUMENT IS REAL AND NEGATIVE */

	if (argr <= consts_1.zero) {

/*     STOP WITH AN ERROR MESSAGE IF THE ARGUMENT IS TOO NEAR A PO
LE */

	    diff = (d__1 = (doublereal) i_dnnt(&argr) - argr, abs(d__1));
	    if (diff <= consts_1.two * precis) {
/*               WRITE (NOUT,300) */
		ici__1.icierr = 0;
		ici__1.icirnum = 1;
		ici__1.icirlen = 256;
		ici__1.iciunit = errstr;
		ici__1.icifmt = fmt_301;
		s_wsfi(&ici__1);
		do_fio(&c__1, (char *)&argr, (ftnlen)sizeof(doublereal));
		do_fio(&c__1, (char *)&argi, (ftnlen)sizeof(doublereal));
		e_wsfi();
		*ierr = 1;
		 ret_val->r = 9.9999e11,  ret_val->i = 9.9999e11;
		return ;
		s_stop("010801", 6L);
	    } else {

/*     OTHERWISE USE THE REFLECTION FORMULA (ABRAMOWITZ AND ST
EGUN 6.1.17) */
/*     TO ENSURE THAT THE ARGUMENT IS SUITABLE FOR STIRLING'S
*/
/*     FORMULA */

		argum = pi / (-argr * sin(pi * argr));
		if (argum < consts_1.zero) {
		    argum = -argum;
		    clngi = pi;
		} else {
		    clngi = consts_1.zero;
		}
		facneg = log(argum);
		argur = -argr;
		negarg = TRUE_;

	    }

/*     CASES WHERE THE ARGUMENT IS REAL AND POSITIVE */

	} else {

	    clngi = consts_1.zero;
	    argur = argr;
	    negarg = FALSE_;

	}

/*     USE ABRAMOWITZ AND STEGUN FORMULA 6.1.15 TO ENSURE THAT */
/*     THE ARGUMENT IN STIRLING'S FORMULA IS GREATER THAN 10 */

	ovlfac = consts_1.one;
L40:
	if (argur < consts_1.ten) {
	    ovlfac *= argur;
	    argur += consts_1.one;
	    goto L40;
	}

/*     NOW USE STIRLING'S FORMULA TO COMPUTE LOG (GAMMA (ARGUM)) */

	clngr = (argur - consts_1.half) * log(argur) - argur + hlntpi;
	fac = argur;
	obasq = consts_1.one / (argur * argur);
	for (i__ = 1; i__ <= 7; ++i__) {
	    fac *= obasq;
	    clngr += fn[i__ - 1] * fac;
/* L50: */
	}

/*     INCLUDE THE CONTRIBUTIONS FROM THE RECURRENCE AND REFLECTION */
/*     FORMULAE */

	clngr -= log(ovlfac);
	if (negarg) {
	    clngr = facneg - clngr;
	}

    } else {

/*     CASES WHERE THE ARGUMENT IS COMPLEX */

	argur = argr;
	argui = argi;
	argui2 = argui * argui;

/*     USE THE RECURRENCE FORMULA (ABRAMOWITZ AND STEGUN 6.1.15) */
/*     TO ENSURE THAT THE MAGNITUDE OF THE ARGUMENT IN STIRLING'S */
/*     FORMULA IS GREATER THAN 10 */

	ovlfr = consts_1.one;
	ovlfi = consts_1.zero;
L60:
	argum = sqrt(argur * argur + argui2);
	if (argum < consts_1.ten) {
	    termr = ovlfr * argur - ovlfi * argui;
	    termi = ovlfr * argui + ovlfi * argur;
	    ovlfr = termr;
	    ovlfi = termi;
	    argur += consts_1.one;
	    goto L60;
	}

/*     NOW USE STIRLING'S FORMULA TO COMPUTE LOG (GAMMA (ARGUM)) */

	argur2 = argur * argur;
	termr = consts_1.half * log(argur2 + argui2);
	termi = atan2(argui, argur);
	clngr = (argur - consts_1.half) * termr - argui * termi - argur +
		hlntpi;
	clngi = (argur - consts_1.half) * termi + argui * termr - argui;
/* Computing 2nd power */
	d__1 = 1 / (argur2 + argui2);
	fac = d__1 * d__1;
	obasqr = (argur2 - argui2) * fac;
	obasqi = -consts_1.two * argur * argui * fac;
	zfacr = argur;
	zfaci = argui;
	for (i__ = 1; i__ <= 7; ++i__) {
	    termr = zfacr * obasqr - zfaci * obasqi;
	    termi = zfacr * obasqi + zfaci * obasqr;
	    fac = fn[i__ - 1];
	    clngr += termr * fac;
	    clngi += termi * fac;
	    zfacr = termr;
	    zfaci = termi;
/* L70: */
	}

/*     ADD IN THE RELEVANT PIECES FROM THE RECURRENCE FORMULA */

	clngr -= consts_1.half * log(ovlfr * ovlfr + ovlfi * ovlfi);
	clngi -= atan2(ovlfi, ovlfr);

    }
    if (*lnpfq == 1) {
	z__1.r = clngr, z__1.i = clngi;
	 ret_val->r = z__1.r,  ret_val->i = z__1.i;
	return ;
    }

/*     NOW EXPONENTIATE THE COMPLEX LOG GAMMA FUNCTION TO GET */
/*     THE COMPLEX GAMMA FUNCTION */

    if (clngr <= expmax && clngr >= -expmax) {
	fac = exp(clngr);
    } else {
/*         WRITE (NOUT,300) */
	ici__1.icierr = 0;
	ici__1.icirnum = 1;
	ici__1.icirlen = 256;
	ici__1.iciunit = errstr;
	ici__1.icifmt = fmt_302;
	s_wsfi(&ici__1);
	do_fio(&c__1, (char *)&clngr, (ftnlen)sizeof(doublereal));
	e_wsfi();
	*ierr = 1;
	 ret_val->r = 9.9999e11,  ret_val->i = 9.9999e11;
	return ;
	s_stop("010802", 6L);
    }
    resr = fac * cos(clngi);
    resi = fac * sin(clngi);
    z__1.r = resr, z__1.i = resi;
     ret_val->r = z__1.r,  ret_val->i = z__1.i;

    return ;

/* L300: */

} /* cgamma_ */

/*     **************************************************************** */
/*     *                                                              * */
/*     *                 BLOCK DATA BLDAT1                            * */
/*     *                                                              * */
/*     *                                                              * */
/*     *  Description : Sets of frequently used numbers in a common   * */
/*     *    block.  This makes it easier to convert the code to a     * */
/*     *    single precision version.                                 * */
/*     *                                                              * */
/*     **************************************************************** */
/* Subroutine */ int bldat1_()
{
	return 0;
} /* bldat1_ */


double onef2( a, b, c, x, err )
double a, b, c, x;
double *err;
{
	double n, a0, sum, t;
	double an, bn, cn, max, z;

	an = a;
	bn = b;
	cn = c;
	a0 = 1.0;
	sum = 1.0;
	n = 1.0;
	t = 1.0;
	max = 0.0;

	do {
		if( an == 0 )
			goto done;
		if( bn == 0 )
			goto error;
		if( cn == 0 )
			goto error;
		if( (a0 > 1.0e34) || (n > 200) )
			goto error;
		a0 *= (an * x) / (bn * cn * n);
		sum += a0;
		an += 1.0;
		bn += 1.0;
		cn += 1.0;
		n += 1.0;
		z = fabs( a0 );
		if( z > max )
			max = z;
		if( sum != 0 )
			t = fabs( a0 / sum );
		else
			t = z;
	} while( t > MACHEPF );

	done:

	*err = fabs( MACHEPF*max /sum );
	goto xit;

	error:
	*err = MAXNUMF;

	xit:
	return (sum);
}

double Hypergeom0F1(double c, double x)
{
	return gsl_sf_hyperg_0F1(c,x);
}

double Hypergeom1F2(double a1, double b1, double b2, double x)
{
	doublereal Res1F2;
	doublecomplex A[1], B[2], z, ret_val;
	integer ln_pFq, ix, nsigfig, ip, iq,errorstr_len=256;
	bool error;
	char errorstr[256];
//	double err;

//	Res1F2 = onef2(a1,b1,b2,x,&err);
//	return Res1F2;

	ln_pFq = 1;
	ix = 0;
	nsigfig = 10;
	ip = 1;
	iq = 2;

	A[0].i = 0;
	A[0].r = a1;
	B[0].i = 0;
	B[0].r = b1;
	B[1].i = 0;
	B[1].r = b2;

	z.i=0;
	z.r=x;

	pfq_( &ret_val, &A, &B, &ip, &iq, &z, &ln_pFq, &ix, &nsigfig, &error, errorstr,&errorstr_len);

	Res1F2 = ret_val.r;
	return exp(Res1F2);
}


double sasfit_2f1(double a, double b,
                  double c, double x,
                  sasfit_param * param)
{
/*
	doublereal Res2F1;
	doublecomplex A[2], B[1], z, ret_val;
	integer ln_pFq, ix, nsigfig, ip, iq,errorstr_len=256;
	bool error;
	char errorstr[256];
	double err;

 	ln_pFq = 0;
	ix = 700;
	nsigfig = 20;
	ip = 2;
	iq = 1;

	A[0].i = 0;
	A[0].r = a;

	A[1].i = 0;
	A[1].r = b;

	B[0].i = 0;
	B[0].r = c;

	z.i=0;
	z.r=x;

	pfq_( &ret_val, &A, &B, &ip, &iq, &z, &ln_pFq, &ix, &nsigfig, &error, errorstr,&errorstr_len);
    if (error == TRUE) sasfit_err("%s\ncall of 2F1(%lf,%lf,%lf,%lf) failed\n",errorstr,a,b,c,x);
	Res2F1 = ret_val.r;
	return Res2F1;
	return exp(Res2F1);
*/

    // Forrey, R. C.
    // Computing the Hypergeometric Function
    // Journal of Computational Physics,
    // 1997, 137, 79 - 100
    // http://dx.doi.org/10.1006/jcph.1997.5794
    double coeff1, coeff2, w;
    gsl_sf_result  c1,c2,c3,c4,arg;
    if (x<-1) {
        // I. w = 1/(1-z)
        w = 1./(1.-x);
        gsl_sf_lngamma_complex_e(c,0,  &c1,&arg);
        gsl_sf_lngamma_complex_e(b-a,0,&c2,&arg);
        gsl_sf_lngamma_complex_e(b,0,  &c3,&arg);
        gsl_sf_lngamma_complex_e(c-a,0,&c4,&arg);
        coeff1=pow(w,a)*exp(c1.val+c2.val-c3.val-c4.val);
        coeff1=pow(w,a)*gsl_sf_gamma(c)*gsl_sf_gamma(b-a)
                      /(gsl_sf_gamma(b)*gsl_sf_gamma(c-a));

        gsl_sf_lngamma_complex_e(c,0,  &c1,&arg);
        gsl_sf_lngamma_complex_e(a-b,0,&c2,&arg);
        gsl_sf_lngamma_complex_e(a,0,  &c3,&arg);
        gsl_sf_lngamma_complex_e(c-b,0,&c4,&arg);
        coeff2=pow(w,a)*exp(c1.val+c2.val-c3.val-c4.val);

        coeff2=pow(w,b)*gsl_sf_gamma(c)*gsl_sf_gamma(a-b)
                      /(gsl_sf_gamma(a)*gsl_sf_gamma(c-b));
        return   coeff1*gsl_sf_hyperg_2F1(a,c-b,a-b+1,w)
                +coeff2*gsl_sf_hyperg_2F1(b,c-a,b-a+1,w);
    } else if (-1<=x && x < 0) {
        // II. w=z/(z-1)
        w=x/(x-1.);
        return pow(1-w,a)*gsl_sf_hyperg_2F1(a,c-b,c,w);
    } else if (0<=x && x <=0.5) {
        // III. w = z
        return gsl_sf_hyperg_2F1(a,b,c,x);
    } else if (0.5<x && x<=1) {
        // IV. w=1-z
        w = 1.-x;
        coeff1 =  gsl_sf_gamma(c)  *gsl_sf_gamma(c-a-b)
                /(gsl_sf_gamma(c-a)*gsl_sf_gamma(c-b));
        coeff2 =  gsl_sf_gamma(c)*gsl_sf_gamma(a+b-c)
                /(gsl_sf_gamma(a)*gsl_sf_gamma(b))
                * pow(w,c-a-b);
        return   coeff1*gsl_sf_hyperg_2F1(a,b,a+b-c+1,w)
                +coeff2*gsl_sf_hyperg_2F1(c-a,c-b,c-a-b+1,w);
    } else if (1<x && x <=2) {
        // V. w = 1-1/z
        w = 1.-1./x;
        coeff1 =  gsl_sf_gamma(c)  *gsl_sf_gamma(c-a-b)
                /(gsl_sf_gamma(c-a)*gsl_sf_gamma(c-b))
                * pow(x,-a);
        coeff2 =  gsl_sf_gamma(c)*gsl_sf_gamma(a+b-c)
                /(gsl_sf_gamma(a)*gsl_sf_gamma(b))
                * pow(1-x,c-a-b)*pow(x,a-c);
        return   coeff1*gsl_sf_hyperg_2F1(a,a-c+1,a+b-c+1,w)
                +coeff2*gsl_sf_hyperg_2F1(c-a,1-a,c-a-b+1,w);
    } else {
        // VI. w = 1/z
        w = 1./x;
        coeff1 =  gsl_sf_gamma(c)*gsl_sf_gamma(b-a)
                /(gsl_sf_gamma(b)*gsl_sf_gamma(c-a))
                *pow(fabs(w),a);
        coeff2 =  gsl_sf_gamma(c)*gsl_sf_gamma(a-b)
                /(gsl_sf_gamma(a)*gsl_sf_gamma(c-b))
                * pow(fabs(w),b);
        // The real part is given by:
        return   cos(M_PI*a)* gsl_sf_hyperg_2F1(a,a-c+1,a-b+1,w)
                +cos(M_PI*b)* gsl_sf_hyperg_2F1(b-c+1,b,b-a+1,w);

        // The imaginary part is given by:
//        return   sin(M_PI*a)* gsl_sf_hyperg_2F1(a,a-c+1,a-b+1,w)
//                +sin(M_PI*b)* gsl_sf_hyperg_2F1(b-c+1,b,b-a+1,w);
    }

}

double sasfit_3f2(double a1, double a2, double a3,
                  double b1, double b2, double x,
                  sasfit_param * param)
{

	doublereal Res3F2;
	doublecomplex A[3], B[2], z, ret_val;
	integer ln_pFq, ix, nsigfig, ip, iq, errstr_len = STRLEN;

	SASFIT_ASSERT_PTR(param);

	ln_pFq = 1;
	ix = 0;
	nsigfig = 4;
	ip = 3;
	iq = 2;

	A[0].i = 0;
	A[0].r = a1;
	A[1].i = 0;
	A[1].r = a2;
	A[2].i = 0;
	A[2].r = a3;
	B[0].i = 0;
	B[0].r = b1;
	B[1].i = 0;
	B[1].r = b2;

	z.i = 0;
	z.r = x;

	// I assume ierr is just a flag here ?
	pfq_( &ret_val, A, B, &ip, &iq, &z, &ln_pFq, &ix, &nsigfig,
		param->errStatus, param->errStr, &errstr_len);
	// is errstr_len written written with the final str length afterwards ?
	// param->errLen = errstr_len;

	Res3F2 = ret_val.r;
	return exp(Res3F2);
}

double sasfit_pfq(double *p_r, double *p_i,  double *q_r, double *q_i, int ip, int iq,
                  double z_r, double z_i, int ln_pFq, int ix, double *pFq_r, double *pFq_i, int nsigfig, sasfit_param * param)
{
	doublecomplex *P, *Q, z, ret_val;
	integer errstr_len = STRLEN;
	int i;

	P = malloc(ip * sizeof(doublecomplex));
	Q = malloc(iq * sizeof(doublecomplex));

    for (i=0; i<ip; i++) {
        P[i].r = p_r[i];
        P[i].i = p_i[i];
    }
    for (i=0; i<iq; i++) {
        Q[i].r = q_r[i];
        Q[i].i = q_i[i];
    }
  	z.i = z_i;
	z.r = z_r;
	pfq_( &ret_val, P, Q, &ip, &iq, &z, &ln_pFq, &ix, &nsigfig,&param->errStatus, param->errStr, &errstr_len);
    *pFq_r = ret_val.r;
    *pFq_i = ret_val.i;
    free(P);
    free(Q);
    return sqrt(ret_val.r*ret_val.r+ret_val.i*ret_val.i);
}
#define float double
