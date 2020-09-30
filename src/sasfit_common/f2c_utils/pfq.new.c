/* pfq.new.f -- translated by f2c (version 20100827).
   You must link the resulting object file with libf2c:
	on Microsoft Windows system, link with libf2c.lib;
	on Linux or Unix systems, link with .../path/to/libf2c.a -lm
	or, if you install libf2c.a in a standard place, with -lf2c -lm
	-- in that order, at the end of the command line, as in
		cc *.o -lf2c -lm
	Source for libf2c is in /netlib/f2c/libf2c.zip, e.g.,

		http://www.netlib.org/f2c/libf2c.zip
*/

#include "f2c.h"

/* Common Block Declarations */

struct consts_1_ {
    doublereal zero, half, one, two, ten, eps;
};

#define consts_1 (*(struct consts_1_ *) &consts_)

struct {
    integer nout;
} io_;

#define io_1 io_

/* Initialized data */

struct {
    doublereal e_1[6];
    } consts_ = { 0., .5, 1., 2., 10., 1e-10 };


/* Table of constant values */

static integer c__9 = 9;
static integer c__1 = 1;
static integer c__25000 = 25000;
static doublereal c_b64 = 10.;

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

/* Double Complex */ VOID pfq_(doublecomplex * ret_val, doublecomplex *a,
	doublecomplex *b, integer *ip, integer *iq, doublecomplex *z__,
	integer *lnpfq, integer *ix, integer *nsigfig, bool *ierr, char *errstr, ftnlen errstr_len)
{
    /* Format strings */
    static char fmt_300[] = "(/,1x,\002IP=\002,1i2,3x,\002IQ=\002,1i2,3x,"
	    "\002AND ABS(Z)=\002,1e12.5,2x,/,\002 WHICH IS GREATER THAN ONE--"
	    "SERIES DOES\002,\002 NOT CONVERGE\002)";

    /* System generated locals */
    doublereal d__1;
    doublecomplex z__1, z__2, z__3, z__4, z__5, z__6, z__7, z__8, z__9, z__10,
	     z__11, z__12, z__13, z__14, z__15, z__16, z__17;

    /* Builtin functions */
    integer s_wsle(cilist *), do_lio(integer *, integer *, char *, ftnlen),
	    e_wsle(void);
    /* Subroutine */ int s_stop(char *, ftnlen);
    double z_abs(doublecomplex *);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);
    void z_div(doublecomplex *, doublecomplex *, doublecomplex *), pow_zz(
	    doublecomplex *, doublecomplex *, doublecomplex *);
    double d_imag(doublecomplex *);
    integer i_dnnt(doublereal *);

    /* Local variables */
    static integer i__;
    static doublecomplex a1[2], b1[1], z1, gam1, gam2, gam3, gam4, gam5, gam6,
	     gam7;
    static doublereal diff, argi, argr, dnum;
    extern /* Double Complex */ VOID hyper_(doublecomplex *, doublecomplex *,
	    doublecomplex *, integer *, integer *, doublecomplex *, integer *,
	     integer *, integer *, bool *, char *, ftnlen);
    static doublecomplex hyper1, hyper2;
    extern /* Double Complex */ VOID cgamma_(doublecomplex *, doublecomplex *,
	     integer *, bool *, char *, ftnlen);
    static doublereal precis;

    /* Fortran I/O blocks */
    static cilist io___1 = { 0, 0, 0, 0, 0 };
    static cilist io___2 = { 0, 0, 0, fmt_300, 0 };


    /* Parameter adjustments */
    --a;
    --b;

    /* Function Body */
    io_1.nout = 6;
    z__1.r = consts_1.zero, z__1.i = consts_1.zero;
    if (z__->r == z__1.r && z__->i == z__1.i) {
	z__1.r = consts_1.one, z__1.i = consts_1.zero;
	 ret_val->r = z__1.r,  ret_val->i = z__1.i;
	return ;
    }
    if (*lnpfq != 0 && *lnpfq != 1) {
	io___1.ciunit = io_1.nout;
	s_wsle(&io___1);
	do_lio(&c__9, &c__1, " ERROR IN INPUT ARGUMENTS: LNPFQ /= 0 OR 1", (
		ftnlen)42);
	e_wsle();
    *ierr=1;
    ret_val->r=-1;
    ret_val->i=-1;
    sprintf(errstr, " ERROR IN INPUT ARGUMENTS: LNPFQ /= 0 OR 1\n");
    return;
	s_stop("", (ftnlen)0);
    }
    if (*ip > *iq && z_abs(z__) > consts_1.one && *ip > 2) {
	io___2.ciunit = io_1.nout;
	s_wsfe(&io___2);
	do_fio(&c__1, (char *)&(*ip), (ftnlen)sizeof(integer));
	do_fio(&c__1, (char *)&(*iq), (ftnlen)sizeof(integer));
	d__1 = z_abs(z__);
	do_fio(&c__1, (char *)&d__1, (ftnlen)sizeof(doublereal));
	e_wsfe();
    *ierr=1;
    ret_val->r=-1;
    ret_val->i=-1;
    sprintf(errstr, "IP=%d, IQ=%d AND ABS(Z)=%lf WHICH IS GREATER THAN ONE--\n SERIES DOES NOT CONVERGE\n", *ip, *iq, z_abs(z__));
    return;
	s_stop("", (ftnlen)0);
    }

/* For the 2F1 function with |z| > 1, use Abramowitz and Stegun, 15.3.7. */

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
	cgamma_(&z__1, &b[1], lnpfq, ierr, errstr,errstr_len);
	if (*ierr==1) {
        ret_val->r=-1;
        ret_val->i=-1;
        return;
	}
	gam1.r = z__1.r, gam1.i = z__1.i;
	z__2.r = a[2].r - a[1].r, z__2.i = a[2].i - a[1].i;
	cgamma_(&z__1, &z__2, lnpfq, ierr, errstr,errstr_len);
	if (*ierr==1) {
        ret_val->r=-1;
        ret_val->i=-1;
        return;
	}
	gam2.r = z__1.r, gam2.i = z__1.i;
	cgamma_(&z__1, &a[2], lnpfq, ierr, errstr,errstr_len);
	if (*ierr==1) {
        ret_val->r=-1;
        ret_val->i=-1;
        return;
	}
	gam3.r = z__1.r, gam3.i = z__1.i;
	z__2.r = b[1].r - a[1].r, z__2.i = b[1].i - a[1].i;
	cgamma_(&z__1, &z__2, lnpfq, ierr, errstr,errstr_len);
	if (*ierr==1) {
        ret_val->r=-1;
        ret_val->i=-1;
        return;
	}
	gam4.r = z__1.r, gam4.i = z__1.i;
	z__2.r = a[1].r - a[2].r, z__2.i = a[1].i - a[2].i;
	cgamma_(&z__1, &z__2, lnpfq, ierr, errstr,errstr_len);
	if (*ierr==1) {
        ret_val->r=-1;
        ret_val->i=-1;
        return;
	}
	gam5.r = z__1.r, gam5.i = z__1.i;
	cgamma_(&z__1, &a[1], lnpfq, ierr, errstr,errstr_len);
	if (*ierr==1) {
        ret_val->r=-1;
        ret_val->i=-1;
        return;
	}
	gam6.r = z__1.r, gam6.i = z__1.i;
	z__2.r = b[1].r - a[2].r, z__2.i = b[1].i - a[2].i;
	cgamma_(&z__1, &z__2, lnpfq, ierr, errstr,errstr_len);
	if (*ierr==1) {
        ret_val->r=-1;
        ret_val->i=-1;
        return;
	}
	gam7.r = z__1.r, gam7.i = z__1.i;
	hyper_(&z__1, a1, b1, ip, iq, &z1, lnpfq, ix, nsigfig, ierr, errstr, errstr_len);
	if ( *ierr==1) return;
	hyper1.r = z__1.r, hyper1.i = z__1.i;
	a1[0].r = a[2].r, a1[0].i = a[2].i;
	z__2.r = consts_1.one - b[1].r, z__2.i = -b[1].i;
	z__1.r = z__2.r + a[2].r, z__1.i = z__2.i + a[2].i;
	a1[1].r = z__1.r, a1[1].i = z__1.i;
	z__2.r = consts_1.one - a[1].r, z__2.i = -a[1].i;
	z__1.r = z__2.r + a[2].r, z__1.i = z__2.i + a[2].i;
	b1[0].r = z__1.r, b1[0].i = z__1.i;
	hyper_(&z__1, a1, b1, ip, iq, &z1, lnpfq, ix, nsigfig, ierr, errstr, errstr_len);
	if ( *ierr==1) return;
	hyper2.r = z__1.r, hyper2.i = z__1.i;
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
    if (*ip == 2 && *iq == 1 && z_abs(z__) > .9f) {
	if (*lnpfq == 1) {
	    goto L30;
	}

/*     Check to see if the Gamma function arguments are o.k.; if not, */
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

/*     USE THE SERIES EXPANSION IF THE ARGUMENT IS TOO NEAR A POLE */

		    diff = (d__1 = (doublereal) i_dnnt(&argr) - argr, abs(
			    d__1));
		    if (diff <= consts_1.two * precis) {
			goto L30;
		    }
		}
	    }
/* L20: */
	}
	cgamma_(&z__1, &b[1], lnpfq, ierr, errstr,errstr_len);
	if (*ierr==1) {
        ret_val->r=-1;
        ret_val->i=-1;
        return;
	}
	gam1.r = z__1.r, gam1.i = z__1.i;
	z__3.r = b[1].r - a[1].r, z__3.i = b[1].i - a[1].i;
	z__2.r = z__3.r - a[2].r, z__2.i = z__3.i - a[2].i;
	cgamma_(&z__1, &z__2, lnpfq, ierr, errstr,errstr_len);
	if (*ierr==1) {
        ret_val->r=-1;
        ret_val->i=-1;
        return;
	}
	gam2.r = z__1.r, gam2.i = z__1.i;
	z__2.r = b[1].r - a[1].r, z__2.i = b[1].i - a[1].i;
	cgamma_(&z__1, &z__2, lnpfq, ierr, errstr,errstr_len);
	if (*ierr==1) {
        ret_val->r=-1;
        ret_val->i=-1;
        return;
	}
	gam3.r = z__1.r, gam3.i = z__1.i;
	z__2.r = b[1].r - a[2].r, z__2.i = b[1].i - a[2].i;
	cgamma_(&z__1, &z__2, lnpfq, ierr, errstr,errstr_len);
	if (*ierr==1) {
        ret_val->r=-1;
        ret_val->i=-1;
        return;
	}
	gam4.r = z__1.r, gam4.i = z__1.i;
	z__3.r = a[1].r + a[2].r, z__3.i = a[1].i + a[2].i;
	z__2.r = z__3.r - b[1].r, z__2.i = z__3.i - b[1].i;
	cgamma_(&z__1, &z__2, lnpfq, ierr, errstr,errstr_len);
	if (*ierr==1) {
        ret_val->r=-1;
        ret_val->i=-1;
        return;
	}
	gam5.r = z__1.r, gam5.i = z__1.i;
	cgamma_(&z__1, &a[1], lnpfq, ierr, errstr,errstr_len);
	if (*ierr==1) {
        ret_val->r=-1;
        ret_val->i=-1;
        return;
	}
	gam6.r = z__1.r, gam6.i = z__1.i;
	cgamma_(&z__1, &a[2], lnpfq, ierr, errstr,errstr_len);
	if (*ierr==1) {
        ret_val->r=-1;
        ret_val->i=-1;
        return;
	}
	gam7.r = z__1.r, gam7.i = z__1.i;
	a1[0].r = a[1].r, a1[0].i = a[1].i;
	a1[1].r = a[2].r, a1[1].i = a[2].i;
	z__3.r = a[1].r + a[2].r, z__3.i = a[1].i + a[2].i;
	z__2.r = z__3.r - b[1].r, z__2.i = z__3.i - b[1].i;
	z__1.r = z__2.r + consts_1.one, z__1.i = z__2.i;
	b1[0].r = z__1.r, b1[0].i = z__1.i;
	z__1.r = consts_1.one - z__->r, z__1.i = -z__->i;
	z1.r = z__1.r, z1.i = z__1.i;
	hyper_(&z__1, a1, b1, ip, iq, &z1, lnpfq, ix, nsigfig, ierr, errstr, errstr_len);
	if (*ierr==1) {
        ret_val->r=-1;
        ret_val->i=-1;
        return;
	}
	hyper1.r = z__1.r, hyper1.i = z__1.i;
	z__1.r = b[1].r - a[1].r, z__1.i = b[1].i - a[1].i;
	a1[0].r = z__1.r, a1[0].i = z__1.i;
	z__1.r = b[1].r - a[2].r, z__1.i = b[1].i - a[2].i;
	a1[1].r = z__1.r, a1[1].i = z__1.i;
	z__3.r = b[1].r - a[1].r, z__3.i = b[1].i - a[1].i;
	z__2.r = z__3.r - a[2].r, z__2.i = z__3.i - a[2].i;
	z__1.r = z__2.r + consts_1.one, z__1.i = z__2.i;
	b1[0].r = z__1.r, b1[0].i = z__1.i;
	hyper_(&z__1, a1, b1, ip, iq, &z1, lnpfq, ix, nsigfig,  ierr, errstr, errstr_len);
	if (*ierr==1) {
        ret_val->r=-1;
        ret_val->i=-1;
        return;
	}
	hyper2.r = z__1.r, hyper2.i = z__1.i;
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
	if (*ierr==1) {
        ret_val->r=-1;
        ret_val->i=-1;
        return;
	}
     ret_val->r = z__1.r,  ret_val->i = z__1.i;
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
doublereal bits_(void)
{
    /* System generated locals */
    doublereal ret_val;

    /* Local variables */
    static doublereal bit, bit2;
    static integer count;

    bit = 1.f;
    count = 0;
L10:
    ++count;
    bit2 = bit * 2.f;
    bit = bit2 + 1.f;
    if (bit - bit2 != 0.f) {
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
/* Double Complex */ VOID hyper_(doublecomplex * ret_val, doublecomplex *a,
	doublecomplex *b, integer *ip, integer *iq, doublecomplex *z__,
	integer *lnpfq, integer *ix, integer *nsigfig, bool *ierr, char *errstr, ftnlen errstr_len)
{
    /* Format strings */
    static char fmt_300[] = "(1x,\002WARNING - REAL PART OF A(\002,1i2,\002)"
	    " WAS SET TO ZERO\002)";
    static char fmt_301[] = "(1x,\002WARNING - IMAG PART OF A(\002,1i2,\002)"
	    " WAS SET TO ZERO\002)";
    static char fmt_302[] = "(1x,\002WARNING - REAL PART OF B(\002,1i2,\002)"
	    " WAS SET TO ZERO\002)";
    static char fmt_303[] = "(1x,\002WARNING - IMAG PART OF B(\002,1i2,\002)"
	    " WAS SET TO ZERO\002)";
    static char fmt_304[] = "(1x,\002ERROR - ARGUMENT B(\002,1i2,\002) WAS E"
	    "QUAL TO ZERO\002)";
    static char fmt_305[] = "(1x,\002ERROR - ARGUMENT B(\002,1i2,\002) WAS A"
	    " NEGATIVE\002,\002 INTEGER\002)";
    static char fmt_306[] = "(1x,\002ERROR IN FN HYPER: L MUST BE < \002,1i4)"
	    ;
    static char fmt_307[] = "(1x,\002 WARNING--THE NUMBER OF SIGNIFICANT FIG"
	    "URES REQU\002,\002ESTED\002,/,\002IS GREATER THAN THE MACHINE PR"
	    "ECISION--\002,\002FINAL ANSWER\002,/,\002WILL BE ACCURATE TO ONLY"
	    "\002,i3,\002 DIGITS\002)";

    /* System generated locals */
    integer i__1, i__2, i__3, i__4, i__5;
    doublereal d__1, d__2, d__3;
    doublecomplex z__1, z__2, z__3, z__4, z__5;

    /* Builtin functions */
    double d_lg10(doublereal *), pow_di(doublereal *, integer *), d_int(
	    doublereal *), d_nint(doublereal *), d_imag(doublecomplex *);
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void),
	     s_wsle(cilist *), do_lio(integer *, integer *, char *, ftnlen),
	    e_wsle(void), i_dnnt(doublereal *);
    /* Subroutine */ int s_stop(char *, ftnlen);
    void z_log(doublecomplex *, doublecomplex *);
    double exp(doublereal);
    void z_div(doublecomplex *, doublecomplex *, doublecomplex *);
    double d_sign(doublereal *, doublereal *), pow_dd(doublereal *,
	    doublereal *), z_abs(doublecomplex *);

    /* Local variables */
    static integer i__, l;
    static doublereal x;
    static integer i1;
    static doublereal ai[10], ci[10], ar[10], cr[10], xi, xl, xr, ai2[10],
	    ci2[10], ar2[10], cr2[10], qi1[25002], qi2[25002], wk1[25002],
	    qr1[25002], qr2[25002], wk2[25002], wk3[25002], wk4[25002], wk5[
	    25002], wk6[25002], mx1, mx2, xr2, xi2;
    static integer ii10, ir10;
    static doublereal ri10, cnt, rr10, log2, dum1, dum2, accy;
    static integer ibit, lmax;
    extern doublereal bits_(void);
    static doublecomplex temp;
    static doublereal rmax, numi[25002], sumi[25002];
    static integer rexp;
    static doublereal numr[25002], sumr[25002];
    static doublecomplex cdum1, cdum2, temp1;
    static integer nmach;
    static doublereal creal;
    static doublecomplex final;
    static integer ixcnt;
    static doublereal expon;
    extern /* Subroutine */ int cmpadd_(doublereal *, doublereal *,
	    doublereal *, doublereal *, doublereal *, doublereal *,
	    doublereal *, integer *, doublereal *);
    static doublereal sigfig, denomi[25002];
    extern /* Double Complex */ VOID factor_(doublecomplex *, doublecomplex *)
	    ;
    static doublereal denomr[25002];
    extern /* Subroutine */ int cmpmul_(doublereal *, doublereal *,
	    doublereal *, doublereal *, doublereal *, doublereal *,
	    doublereal *, doublereal *, doublereal *, doublereal *,
	    doublereal *, doublereal *, integer *, doublereal *), arydiv_(
	    doublereal *, doublereal *, doublereal *, doublereal *,
	    doublecomplex *, integer *, integer *, doublereal *, integer *, bool *, char *, ftnlen);
    static integer icount;
    extern /* Subroutine */ int armult_(doublereal *, doublereal *,
	    doublereal *, doublereal *, integer *, doublereal *);
    static doublecomplex oldtemp;
    extern integer ipremax_(doublecomplex *, doublecomplex *, integer *,
	    integer *, doublecomplex *, bool *, char *, ftnlen);

    /* Fortran I/O blocks */
    static cilist io___41 = { 0, 0, 0, fmt_300, 0 };
    static cilist io___42 = { 0, 0, 0, fmt_301, 0 };
    static cilist io___43 = { 0, 0, 0, fmt_302, 0 };
    static cilist io___44 = { 0, 0, 0, fmt_303, 0 };
    static cilist io___45 = { 0, 0, 0, 0, 0 };
    static cilist io___46 = { 0, 0, 0, 0, 0 };
    static cilist io___49 = { 0, 0, 0, fmt_304, 0 };
    static cilist io___50 = { 0, 0, 0, fmt_305, 0 };
    static cilist io___59 = { 0, 0, 0, fmt_306, 0 };
    static cilist io___60 = { 0, 0, 0, fmt_307, 0 };


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
	    io___41.ciunit = io_1.nout;
	    s_wsfe(&io___41);
	    do_fio(&c__1, (char *)&i1, (ftnlen)sizeof(integer));
	    e_wsfe();
	}
	if (d_imag(&a[i1]) != consts_1.zero && ai[i1 - 1] == consts_1.zero &&
		ai2[i1 - 1] == consts_1.zero) {
	    io___42.ciunit = io_1.nout;
	    s_wsfe(&io___42);
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
	    io___43.ciunit = io_1.nout;
	    s_wsfe(&io___43);
	    do_fio(&c__1, (char *)&i1, (ftnlen)sizeof(integer));
	    e_wsfe();
	}
	if (d_imag(&b[i1]) != consts_1.zero && ci[i1 - 1] == consts_1.zero &&
		ci2[i1 - 1] == consts_1.zero) {
	    io___44.ciunit = io_1.nout;
	    s_wsfe(&io___44);
	    do_fio(&c__1, (char *)&i1, (ftnlen)sizeof(integer));
	    e_wsfe();
	}
/* L40: */
    }
    if (z__->r != consts_1.zero && xr == consts_1.zero && xr2 ==
	    consts_1.zero) {
	io___45.ciunit = io_1.nout;
	s_wsle(&io___45);
	do_lio(&c__9, &c__1, " WARNING - REAL PART OF Z WAS SET TO ZERO", (
		ftnlen)41);
	e_wsle();
	d__1 = d_imag(z__);
	z__1.r = consts_1.zero, z__1.i = d__1;
	z__->r = z__1.r, z__->i = z__1.i;
    }
    if (d_imag(z__) != consts_1.zero && xi == consts_1.zero && xi2 ==
	    consts_1.zero) {
	io___46.ciunit = io_1.nout;
	s_wsle(&io___46);
	do_lio(&c__9, &c__1, " WARNING - IMAG PART OF Z WAS SET TO ZERO", (
		ftnlen)41);
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

/*     SCREENING OF DENOMINATOR ARGUMENTS FOR ZEROES OR NEGATIVE INTEGERS. */

    i__1 = *iq;
    for (i1 = 1; i1 <= i__1; ++i1) {
	if (cr[i1 - 1] == consts_1.zero && cr2[i1 - 1] == consts_1.zero && ci[
		i1 - 1] == consts_1.zero && ci2[i1 - 1] == consts_1.zero) {
	    io___49.ciunit = io_1.nout;
	    s_wsfe(&io___49);
	    do_fio(&c__1, (char *)&i1, (ftnlen)sizeof(integer));
	    e_wsfe();
	    *ierr = 1;
        ret_val->r=-1;
        ret_val->i=-1;
        sprintf(errstr, "ERROR - ARGUMENT B(%d) WAS EQUAL TO ZERO\n",i1);
        return;
	    s_stop("", (ftnlen)0);
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
		io___50.ciunit = io_1.nout;
		s_wsfe(&io___50);
		do_fio(&c__1, (char *)&i1, (ftnlen)sizeof(integer));
		e_wsfe();
	    *ierr = 1;
        ret_val->r=-1;
        ret_val->i=-1;
        sprintf(errstr, "ERROR - ARGUMENT B(%d) WAS A  NEGATIVE INTEGER\n",i1);
        return;
		s_stop("", (ftnlen)0);
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
    if (*ierr == 1) return;
    if (l == 1) {
	goto L110;
    }

/*     First, estimate the exponent of the maximum term in the pFq series. */

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

/*     Now, estimate the exponent of where the pFq series will terminate. */

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

/*     Divide the number of significant figures necessary by the number of */
/*     digits available per array position. */

    l = ((l << 1) + *nsigfig) / nmach + 2;

/*     Make sure there are at least 5 array positions used. */

L110:
    l = max(l,5);
    l = max(l,*ix);
/*     write (6,*) ' Estimated value of L=',L */
    if (l < 0 || l > 25000) {
	io___59.ciunit = io_1.nout;
	s_wsfe(&io___59);
	do_fio(&c__1, (char *)&c__25000, (ftnlen)sizeof(integer));
	e_wsfe();
    *ierr = 1;
    ret_val->r=-1;
    ret_val->i=-1;
    sprintf(errstr, "ERROR IN FN HYPER: L MUST BE 0<L<25000, L=%d\n",1);
    return;
	s_stop("", (ftnlen)0);
    }
    if (*nsigfig > nmach) {
	io___60.ciunit = io_1.nout;
	s_wsfe(&io___60);
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
    dum1 *= pow_dd(&c_b64, &rr10);
    dum2 *= pow_dd(&c_b64, &ri10);
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
    dum1 *= pow_dd(&c_b64, &rr10);
    dum2 *= pow_dd(&c_b64, &ri10);
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
	if (mx1 - mx2 > 2.f) {
	    if (creal >= consts_1.zero) {
/*     write (6,*) ' cdabs(temp1/cnt)=',cdabs(temp1/cnt) */
		z__1.r = temp1.r / cnt, z__1.i = temp1.i / cnt;
		if (z_abs(&z__1) <= consts_1.one) {
		    goto L240;
		}
	    }
	}
    } else {
	arydiv_(sumr, sumi, denomr, denomi, &temp, &l, lnpfq, &rmax, &ibit, ierr, errstr, errstr_len);
    if (*ierr) return;
/*     First, estimate the exponent of the maximum term in the pFq series. */

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

/*     TAKE THE CURRENT SUM AND MULTIPLY BY THE DENOMINATOR OF THE NEXT */
/*     TERM, FOR BOTH THE MOST SIGNIFICANT HALF (CR,CI) AND THE LEAST */
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

/*     MULTIPLY BY THE SCALING FACTOR, SIGFIG, TO KEEP THE SCALE CORRECT. */

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

/*     MULTIPLY BY THE SCALING FACTOR, SIGFIG, TO KEEP THE SCALE CORRECT. */

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

/*     MULTIPLY BY THE SCALING FACTOR, SIGFIG, TO KEEP THE SCALE CORRECT. */

    i__1 = *iq - *ip;
    for (i1 = 1; i1 <= i__1; ++i1) {
	armult_(numr, &sigfig, numr, wk6, &l, &rmax);
	armult_(numi, &sigfig, numi, wk6, &l, &rmax);
/* L210: */
    }

/*     FINALLY, ADD THE NEW NUMERATOR TERM WITH THE CURRENT RUNNING */
/*     SUM OF THE NUMERATOR AND STORE THE NEW RUNNING SUM IN SUMR, SUMI. */

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
    arydiv_(sumr, sumi, denomr, denomi, &final, &l, lnpfq, &rmax, &ibit, ierr, errstr, errstr_len);
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
/* Subroutine */ int aradd_(doublereal *a, doublereal *b, doublereal *c__,
	doublereal *z__, integer *l, doublereal *rmax)
{
    /* System generated locals */
    integer i__1;
    doublereal d__1;

    /* Builtin functions */
    double d_nint(doublereal *);

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
/* Subroutine */ int arsub_(doublereal *a, doublereal *b, doublereal *c__,
	doublereal *wk1, doublereal *wk2, integer *l, doublereal *rmax)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__;
    extern /* Subroutine */ int aradd_(doublereal *, doublereal *, doublereal
	    *, doublereal *, integer *, doublereal *);

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
/* Subroutine */ int armult_(doublereal *a, doublereal *b, doublereal *c__,
	doublereal *z__, integer *l, doublereal *rmax)
{
    /* System generated locals */
    integer i__1;
    doublereal d__1;

    /* Builtin functions */
    double d_sign(doublereal *, doublereal *), d_int(doublereal *);

    /* Local variables */
    static integer i__;
    static doublereal b2, carry;

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
/* Subroutine */ int cmpadd_(doublereal *ar, doublereal *ai, doublereal *br,
	doublereal *bi, doublereal *cr, doublereal *ci, doublereal *wk1,
	integer *l, doublereal *rmax)
{
    extern /* Subroutine */ int aradd_(doublereal *, doublereal *, doublereal
	    *, doublereal *, integer *, doublereal *);

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
/* Subroutine */ int cmpsub_(doublereal *ar, doublereal *ai, doublereal *br,
	doublereal *bi, doublereal *cr, doublereal *ci, doublereal *wk1,
	doublereal *wk2, integer *l, doublereal *rmax)
{
    extern /* Subroutine */ int arsub_(doublereal *, doublereal *, doublereal
	    *, doublereal *, doublereal *, integer *, doublereal *);

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
/* Subroutine */ int cmpmul_(doublereal *ar, doublereal *ai, doublereal *br,
	doublereal *bi, doublereal *cr, doublereal *ci, doublereal *wk1,
	doublereal *wk2, doublereal *cr2, doublereal *d1, doublereal *d2,
	doublereal *wk6, integer *l, doublereal *rmax)
{
    /* System generated locals */
    integer i__1;

    /* Local variables */
    static integer i__;
    extern /* Subroutine */ int aradd_(doublereal *, doublereal *, doublereal
	    *, doublereal *, integer *, doublereal *), arsub_(doublereal *,
	    doublereal *, doublereal *, doublereal *, doublereal *, integer *,
	     doublereal *), armult_(doublereal *, doublereal *, doublereal *,
	    doublereal *, integer *, doublereal *);

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
/* Subroutine */ int arydiv_(doublereal *ar, doublereal *ai, doublereal *br,
	doublereal *bi, doublecomplex *c__, integer *l, integer *lnpfq,
	doublereal *rmax, integer *ibit, bool *ierr, char *errstr, ftnlen errstr_len)
{
    /* System generated locals */
    doublereal d__1;
    doublecomplex z__1;

    /* Builtin functions */
    double d_lg10(doublereal *), d_sign(doublereal *, doublereal *), pow_dd(
	    doublereal *, doublereal *), atan2(doublereal, doublereal), log(
	    doublereal);

    /* Local variables */
    static doublereal x, e1, e2, e3, n1, n2, n3, x1, x2, ae[4]	/* was [2][2]
	    */, be[4]	/* was [2][2] */, ce[4]	/* was [2][2] */;
    static integer ii10, ir10;
    static doublereal ri10, phi, rr10, dum1, dum2;
    extern /* Subroutine */ int eadd_(doublereal *, doublereal *, doublereal *
	    , doublereal *, doublereal *, doublereal *);
    static doublecomplex cdum;
    static real dnum;
    static integer rexp;
    extern /* Subroutine */ int conv12_(doublecomplex *, doublereal *),
	    conv21_(doublereal *, doublecomplex *, bool *, char *, ftnlen), emult_(doublereal *,
	    doublereal *, doublereal *, doublereal *, doublereal *,
	    doublereal *), ecpdiv_(doublereal *, doublereal *, doublereal *);
    static doublereal tenmax;
    static integer itnmax;

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
    dum1 *= pow_dd(&c_b64, &rr10);
    dum2 *= pow_dd(&c_b64, &ri10);
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
    dum1 *= pow_dd(&c_b64, &rr10);
    dum2 *= pow_dd(&c_b64, &ri10);
    z__1.r = dum1, z__1.i = dum2;
    cdum.r = z__1.r, cdum.i = z__1.i;
    conv12_(&cdum, be);
    be[2] += ir10;
    be[3] += ii10;
    ecpdiv_(ae, be, ce);
    if (*lnpfq == 0) {
	conv21_(ce, c__, ierr, errstr, errstr_len);
	if (*ierr==1) return 0;
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
/*     ON A SUN WORKSTATION.  REPLACE TENMAX WITH THE VALUE APPROPRIATE */
/*     FOR YOUR MACHINE. */

	tenmax = 320.;
	itnmax = 1;
	dnum = .1f;
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
/* Subroutine */ int emult_(doublereal *n1, doublereal *e1, doublereal *n2,
	doublereal *e2, doublereal *nf, doublereal *ef)
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
/* Subroutine */ int ediv_(doublereal *n1, doublereal *e1, doublereal *n2,
	doublereal *e2, doublereal *nf, doublereal *ef)
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
/* Subroutine */ int eadd_(doublereal *n1, doublereal *e1, doublereal *n2,
	doublereal *e2, doublereal *nf, doublereal *ef)
{
    /* Builtin functions */
    double pow_dd(doublereal *, doublereal *);

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
/* Subroutine */ int esub_(doublereal *n1, doublereal *e1, doublereal *n2,
	doublereal *e2, doublereal *nf, doublereal *ef)
{
    /* System generated locals */
    doublereal d__1;

    /* Local variables */
    extern /* Subroutine */ int eadd_(doublereal *, doublereal *, doublereal *
	    , doublereal *, doublereal *, doublereal *);

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
/* Subroutine */ int conv12_(doublecomplex *cn, doublereal *cae)
{
    /* Builtin functions */
    double d_imag(doublecomplex *);

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
/* Subroutine */ int conv21_(doublereal *cae, doublecomplex *cn, bool *ierr, char *errstr, ftnlen errstr_len)
{
    /* Format strings */
    static char fmt_300[] = "(\002 ERROR - VALUE OF EXPONENT REQUIRED FOR SU"
	    "MMATION\002,\002 WAS LARGER\002,/,\002 THAN THE MAXIMUM MACHINE "
	    "EXPONENT \002,1i3,/,\002 SUGGESTIONS:\002,/,\002 1) RE-RUN USING"
	    " LNPFQ=1.\002,/,\002 2) IF YOU ARE USING A VAX, TRY USING THE"
	    "\002,\002 FORTRAN/G_FLOATING OPTION\002)";

    /* System generated locals */
    doublereal d__1, d__2;
    doublecomplex z__1;

    /* Builtin functions */
    integer s_wsfe(cilist *), do_fio(integer *, char *, ftnlen), e_wsfe(void);
    /* Subroutine */ int s_stop(char *, ftnlen);
    double pow_dd(doublereal *, doublereal *);

    /* Local variables */
    static doublereal dnum, tenmax;
    static integer itnmax;

    /* Fortran I/O blocks */
    static cilist io___130 = { 0, 0, 0, fmt_300, 0 };



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
	io___130.ciunit = io_1.nout;
	s_wsfe(&io___130);
	do_fio(&c__1, (char *)&itnmax, (ftnlen)sizeof(integer));
	e_wsfe();
    *ierr=1;
    sprintf(errstr, "ERROR - VALUE OF EXPONENT REQUIRED FOR SUMMATION WAS LARGER THAN THE MAXIMUM MACHINE EXPONENT %d\nSUGGESTIONS: 1) RE-RUN USING LNPFQ=1. IF YOU ARE USING A VAX, TRY USING THE FORTRAN /G_FLOATING OPTION\n",itnmax);
    return 0;
	s_stop("", (ftnlen)0);
    } else if (cae[6] < -tenmax) {
	d__1 = cae[3] * pow_dd(&c_b64, &cae[5]);
	z__1.r = d__1, z__1.i = consts_1.zero;
	cn->r = z__1.r, cn->i = z__1.i;
    } else {
	d__1 = cae[3] * pow_dd(&c_b64, &cae[5]);
	d__2 = cae[4] * pow_dd(&c_b64, &cae[6]);
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
/* Subroutine */ int ecpmul_(doublereal *a, doublereal *b, doublereal *c__)
{
    static doublereal c2[4]	/* was [2][2] */, e1, e2, n1, n2;
    extern /* Subroutine */ int eadd_(doublereal *, doublereal *, doublereal *
	    , doublereal *, doublereal *, doublereal *), esub_(doublereal *,
	    doublereal *, doublereal *, doublereal *, doublereal *,
	    doublereal *), emult_(doublereal *, doublereal *, doublereal *,
	    doublereal *, doublereal *, doublereal *);

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
/* Subroutine */ int ecpdiv_(doublereal *a, doublereal *b, doublereal *c__)
{
    static doublereal b2[4]	/* was [2][2] */, c2[4]	/* was [2][2] */, e1,
	    e2, e3, n1, n2, n3;
    extern /* Subroutine */ int eadd_(doublereal *, doublereal *, doublereal *
	    , doublereal *, doublereal *, doublereal *), ediv_(doublereal *,
	    doublereal *, doublereal *, doublereal *, doublereal *,
	    doublereal *), emult_(doublereal *, doublereal *, doublereal *,
	    doublereal *, doublereal *, doublereal *), ecpmul_(doublereal *,
	    doublereal *, doublereal *);

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
integer ipremax_(doublecomplex *a, doublecomplex *b, integer *ip, integer *iq,
	 doublecomplex *z__, bool *ierr, char *errstr, ftnlen errstr_len)
{
    /* System generated locals */
    integer ret_val, i__1, i__2, i__3;
    doublereal d__1;
    doublecomplex z__1, z__2, z__3, z__4, z__5;

    /* Builtin functions */
    void z_log(doublecomplex *, doublecomplex *);
    double exp(doublereal), d_lg10(doublereal *);
    integer s_wsle(cilist *), do_lio(integer *, integer *, char *, ftnlen),
	    e_wsle(void);
    /* Subroutine */ int s_stop(char *, ftnlen);

    /* Local variables */
    static integer i__, j;
    static doublereal xl, xmax, expon, xterm;
    extern /* Double Complex */ VOID factor_(doublecomplex *, doublecomplex *)
	    ;

    /* Fortran I/O blocks */
    static cilist io___150 = { 0, 0, 0, 0, 0 };



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
    io___150.ciunit = io_1.nout;
    s_wsle(&io___150);
    do_lio(&c__9, &c__1, " ERROR IN IPREMAX--DID NOT FIND MAXIMUM EXPONENT", (
	    ftnlen)48);
    e_wsle();
    *ierr = 1;
    sprintf(errstr," ERROR IN IPREMAX--DID NOT FIND MAXIMUM EXPONENT\n");
    return 1;
    s_stop("", (ftnlen)0);
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
/* Double Complex */ VOID factor_(doublecomplex * ret_val, doublecomplex *z__)
{
    /* System generated locals */
    doublereal d__1;
    doublecomplex z__1, z__2, z__3, z__4, z__5, z__6, z__7, z__8, z__9, z__10,
	     z__11, z__12, z__13, z__14, z__15, z__16, z__17, z__18, z__19,
	    z__20, z__21;

    /* Builtin functions */
    double d_imag(doublecomplex *), z_abs(doublecomplex *), atan(doublereal),
	    log(doublereal);
    void z_log(doublecomplex *, doublecomplex *), z_div(doublecomplex *,
	    doublecomplex *, doublecomplex *);

    /* Local variables */
    static doublereal pi;


    if (z__->r == consts_1.one && d_imag(z__) == consts_1.zero || z_abs(z__)
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
/* Double Complex */ VOID cgamma_(doublecomplex * ret_val, doublecomplex *arg,
	 integer *lnpfq, bool *ierr, char *errstr, ftnlen errstr_len)
{
    /* Initialized data */

    static doublereal fn[7] = { 1.,-1.,1.,-1.,5.,-691.,7. };
    static doublereal fd[7] = { 6.,30.,42.,30.,66.,2730.,6. };
    static doublereal hlntpi = 1.;
    static logical first = TRUE_;
    static doublereal tenth = .1;

    /* Format strings */
    static char fmt_300[] = "(///\002 ***** ERROR IN SUBROUTINE CGAMMA ****"
	    "*\002)";
    static char fmt_301[] = "(\002 ARGUMENT (\002,1p,1d14.7,\002,\002,1d14"
	    ".7,\002) TOO CLOSE TO A\002,\002 POLE.\002)";
    static char fmt_302[] = "(\002 ARGUMENT TO EXPONENTIAL FUNCTION (\002,1p"
	    ",1d14.7,\002) OUT OF RANGE.\002)";

    /* System generated locals */
    doublereal d__1;
    doublecomplex z__1;

    /* Builtin functions */
    double d_imag(doublecomplex *), atan(doublereal), pow_di(doublereal *,
	    integer *), log(doublereal);
    integer i_dnnt(doublereal *), s_wsfe(cilist *), e_wsfe(void), do_fio(
	    integer *, char *, ftnlen);
    /* Subroutine */ int s_stop(char *, ftnlen);
    double sin(doublereal), sqrt(doublereal), atan2(doublereal, doublereal),
	    exp(doublereal), cos(doublereal);

    /* Local variables */
    static integer i__;
    static doublereal pi, fac, diff, argi, argr, resi, dnum, resr, twoi,
	    clngi, zfaci, clngr, obasq, argui, zfacr, argum, ovlfi, argur,
	    termi, ovlfr, termr, argui2, argur2, facneg;
    static logical negarg;
    static doublereal ovlfac, obasqi, precis, obasqr, tenmax;
    static integer itnmax;
    static doublereal expmax;

    /* Fortran I/O blocks */
    static cilist io___168 = { 0, 0, 0, fmt_300, 0 };
    static cilist io___169 = { 0, 0, 0, fmt_301, 0 };
    static cilist io___190 = { 0, 0, 0, fmt_300, 0 };
    static cilist io___191 = { 0, 0, 0, fmt_302, 0 };





/* ----------------------------------------------------------------------* */
/*     * */
/*     THESE ARE THE BERNOULLI NUMBERS B02, B04, ..., B14, EXPRESSED AS   * */
/*     RATIONAL NUMBERS. FROM ABRAMOWITZ AND STEGUN, P. 810.              * */


/* ----------------------------------------------------------------------* */




    argr = arg->r;
    argi = d_imag(arg);

/*     ON THE FIRST ENTRY TO THIS ROUTINE, SET UP THE CONSTANTS REQUIRED */
/*     FOR THE REFLECTION FORMULA (CF. ABRAMOWITZ AND STEGUN 6.1.17) AND */
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

/*     STOP WITH AN ERROR MESSAGE IF THE ARGUMENT IS TOO NEAR A POLE */

	    diff = (d__1 = (doublereal) i_dnnt(&argr) - argr, abs(d__1));
	    if (diff <= consts_1.two * precis) {
		io___168.ciunit = io_1.nout;
		s_wsfe(&io___168);
		e_wsfe();
		io___169.ciunit = io_1.nout;
		s_wsfe(&io___169);
		do_fio(&c__1, (char *)&argr, (ftnlen)sizeof(doublereal));
		do_fio(&c__1, (char *)&argi, (ftnlen)sizeof(doublereal));
		e_wsfe();
		*ierr=1;
		sprintf(errstr,"ERROR IN SUBROUTINE CGAMMA \nARGUMENT (%lf+I%lf) TOO CLOSE TO A POLE.\n",argr,argi);
		return;
		s_stop("010801", (ftnlen)6);
	    } else {

/*     OTHERWISE USE THE REFLECTION FORMULA (ABRAMOWITZ AND STEGUN 6.1.17) */
/*     TO ENSURE THAT THE ARGUMENT IS SUITABLE FOR STIRLING'S */
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
	io___190.ciunit = io_1.nout;
	s_wsfe(&io___190);
	e_wsfe();
	io___191.ciunit = io_1.nout;
	s_wsfe(&io___191);
	do_fio(&c__1, (char *)&clngr, (ftnlen)sizeof(doublereal));
	e_wsfe();
	*ierr=1;
	sprintf(errstr,"ERROR IN SUBROUTINE CGAMMA\nARGUMENT TO EXPONENTIAL FUNCTION exp(%lf) OUT OF RANGE.\n",clngr);
	return;
	s_stop("010802", (ftnlen)6);
    }
    resr = fac * cos(clngi);
    resi = fac * sin(clngi);
    z__1.r = resr, z__1.i = resi;
     ret_val->r = z__1.r,  ret_val->i = z__1.i;

    return ;


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
/* Subroutine */ int bldat1_(void)
{
    return 0;
} /* bldat1_ */



