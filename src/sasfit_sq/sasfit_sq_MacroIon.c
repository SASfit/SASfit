/*
 * src/sasfit_sq/sasfit_sq_MacroIon.c
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

#include <gsl/gsl_math.h>
#include "include/sasfit_sq_utils.h"
#include <f2c.h>

// Common Block Declarations

struct {
    doublereal temp, sighp, ion, z__;
} s_hp__;

#define s_hp__1 s_hp__

union {
    struct {
	doublereal ahp, bhp, chp, fhp;
	doublereal eta, gek, ak, u, v, gamk, seta, sgek, sak, scal, g1;
    } _1;
    struct {
	doublereal ahp, bhp, chp, fhp;
	doublereal reta, rgek, rak, u, v, gamk, seta, sgek, sak, scal, g1;
    } _2;
    struct {
	doublereal ahp, bhp, chp, fhp;
	doublereal etaz, gekz, akz, u, v, gamk, seta, sgek, sak, scal, g1;
    } _3;
} sqhpb_;

#define sqhpb_1 (sqhpb_._1)
#define sqhpb_2 (sqhpb_._2)
#define sqhpb_3 (sqhpb_._3)

struct {
    integer ned[2];
} ned_;

#define ned_1 ned_

struct {
    logical berr;
} errors_;

#define errors_1 errors_

// Table of constant values

static integer c__1 = 1;
static integer c__2 = 2;
static integer c__3 = 3;
static integer c__4 = 4;
static integer c__9 = 9;
static doublereal c_b18 = .3333333333333;

int 		sq_hp_init__(integer * ierr,
			     doublereal * t_in_c__,
			     doublereal * z_eff__,
			     doublereal * r_hs__,
			     doublereal * ion_in__,
			     doublereal * eta_in__);
int 		sqcoef_(integer * ir);
int 		sqfun_(doublereal * fval, doublereal * evar, integer * ix, integer * ir);
doublereal 	sq_hp__(doublereal * q);

/*
doublereal SQ_MacroIon(Tcl_Interp *interp,
			double Q,
			double T_IN_C,
			double Z_EFF,
			double RHS,
			double ION,
			double ETA,
			bool  *error)
*/

scalar sasfit_sq_MacroIon(scalar q, sasfit_param * param)
{
	static doublereal RHS_old=-1.0;
	static doublereal T_IN_C_old=-1.0;
	static doublereal Z_EFF_old=-1.0;
	static doublereal ION_old=-1.0;
	static doublereal ETA_old=-1.0;

	doublereal SQ;
	doublereal QRHS2;
	doublereal T,Z,I,E,R;
	integer IERR;

	scalar T_IN_C, Z_EFF, RHS, ION, ETA;


	SASFIT_ASSERT_PTR( param );

	sasfit_get_param(param, 5, &T_IN_C, &Z_EFF, &RHS, &ION, &ETA);

	if ( (RHS_old!=RHS) || (T_IN_C_old!=T_IN_C) || (Z_EFF_old!=Z_EFF) || (ION_old!=ION)  || (ETA_old!=ETA))
	{
		R = (doublereal) RHS;
		T = (doublereal) T_IN_C;
		Z = (doublereal) Z_EFF;
		I = (doublereal) ION;
		E = (doublereal) ETA;
		sq_hp_init__(&IERR,&T, &Z, &R, &I, &E);

		if (IERR > 0)
		{
			RHS_old	 = RHS;
			T_IN_C_old = T_IN_C;
			Z_EFF_old  = Z_EFF;
			ION_old	 = ION;
			ETA_old	 = ETA;
		} else {
			switch (IERR)
			{
				case -1 :
					sasfit_param_set_err(param, DBGINFO("sq_hp_init FAILED: Newton iteration non-convergent in >SQCOEF<"));
					break;
				case -2 :
					sasfit_param_set_err(param, DBGINFO("sq_hp_init FAILED: Newton iteration non-convergent in >SQFUN<"));
					break;
				case -3 :
					sasfit_param_set_err(param, DBGINFO("sq_hp_init FAILED: Cannot rescale to G(1+) > 1"));
					break;
				default :
					sasfit_param_set_err(param, DBGINFO("sq_hp_init FAILED: unknown error type in SQ_MacroIon"));
					break;
			}
			return SASFIT_RETURNVAL_ON_ERROR;
		}
	}
	QRHS2 = q*2.*RHS;
	SQ = sq_hp__(&QRHS2);
	return SQ;
}

// Subroutine
int sq_hp_init__(integer * ierr,
		doublereal * t_in_c__,
		doublereal * z_eff__,
		doublereal * r_hs__,
		doublereal * ion_in__,
		doublereal * eta_in__)
{
    /* Initialized data */

    static doublereal od3 = (float).3333333333333333;

    /* System generated locals */
    doublereal r__1, r__2, r__3, r__4;
    doublereal d__1, d__2;

    /* Builtin functions */
//    double exp(), sqrt(), pow_dd();
//    integer s_wsle(), do_lio(), e_wsle();

    /* Local variables */
    static doublereal epsi, expo, s;
//    extern /* Subroutine */ int sqcoef_();
    static doublereal qaz, koz;

    /* Fortran I/O blocks */
    static cilist io___8 = { 0, 6, 0, 0, 0 };


/*      Routine to calculate S(Q*SIG) for a screened */
/*      Coulomb potential between finite particles of */
/*      diameter "SIG" at any volume fraction. This */
/*      routine is much more powerful than "SQHP" and */
/*      should be used to replace the latter in existing */
/*      programs. Note that the common area is changed; */
/*      in particular, the potential is passed directly */
/*      as "GEK"=GAMMA*EXP(-K)in the present routine. */

/*      John B. Hayter  ( I.L.L.) 19-AUG-81 */

/*      Calling sequence: */

/*               CALL SQHPA(QQ,SQ,NPT,IERR) */

/*      QQ:   Array of dimension NPT containing the values */
/*            of Q*SIG at which S(Q*SIG) will be calcula- */
/*            ted. */
/*      SQ:   Array of dimension NPT into which values of */
/*            S(Q*SIG) will be returned. */
/*      NPT:  Number of values of Q*SIG */

/*      IERR > 0: Normal exit; IERR = Number of iterations. */
/*            -1: Newton iteration non-convergent in */
/*              "SQCOEF" */
/*            -2: Newton iteration non-convergent in */
/*              "SQFUN" */
/*            -3: Cannot rescale to G(1+) > 1 */

/*      All other parameters are transmitted through a sin- */
/*      gle named common area: */

/*      REAL*8 A,B,C,F */
/*      COMMON/SQHPB/ETA,GEK,AK,A,B,C,F,U,V,GAMK,SETA,SGEK, */
/*      1            SAK,SCAL,G1 */

/*      On entry: */

/*      ETA: Volume fraction */
/*      GEK: The contact potential GAMMA*EXP(-K) */
/*      AK : The dimensionless screening constant */
/*           K = KAPPA*SIG where KAPPA is the inverse scre- */
/*           ening length and SIG is the particle diameter. */

/*      On exit: */

/*      GAMK is the coupling: 2*GAMMA*S*EXP(-K/S), */
/*      S=ETA**(1/3), SETA, SGEK, and SAK are the rescaled */
/*      input parameters. */
/*      SCAL is the rescaling factor: (ETA/SETA)**(1/3). */
/*      G1=G(1+), the contact value of G(R/SIG). */
/*      A,B,C,F,U,V are the constants appearing in the */
/*      analytic solution of the MSA [Hayter-Penfold; */
/*      Mol.Phys. 42: 109(1981)] */

/*      Notes: */

/*      (A) After the first call to SQHPA, S(Q*SIG) may be */
/*      evaluated at other Q*SIG values by redefining the */
/*      array QQ and calling "SQHCAL" directly from the */
/*      main program. */

/*      (B) The resulting S(Q*SIG) may be transformed to */
/*      G(R/SIG) using the routine "TROGS". */

/*      (C) No error checking of input parameters is per- */
/*      formed; it is the responsibility of the calling */
/*      program to verify validity. */

/*      Subroutines required by SQHPA: */

/*      (1) SQCOEF   Rescales the problem and calculates */
/*                   the appropriate coefficients for */
/*                   "SQHCAL". */

/*      (2) SQFUN    Calculates various values for */
/*                   "SQCOEF". */

/*      (3) SQHCAL   Calculates H-P S(Q*SIG) given A,B,C,F. */


/*      Calculate theoretical function . */


/*       DIMENSION AA(25) */


    s_hp__1.temp = *t_in_c__ + (float)273.15;
    s_hp__1.z__ = *z_eff__;
    s_hp__1.sighp = *r_hs__ * (float)2.;
    sqhpb_1.eta = *eta_in__;
    s_hp__1.ion = *ion_in__;
/*       IF(NXS.EQ.35)THEN */
/*         SIGHP=2.*AA(2)*AA(4)**0.33333333 */
/*         TEMP=273.15+AA(10) */
/*         Z=AA(8) */
/*         ETA=AA(7)*0.75E-3 */
/*         ION=0.5*(AA(9)+AA(9)+AA(8)*AA(7)/5800.) */
/*       END IF */
/*       IF(NXS.EQ.40)THEN */
/*         PI=ACOS(-1.) */
/*         V_POL= 709.    ! UNITS A**3 */
/*         V_APOL=486. */
/*         V_H2O=  30. */
/*         V_CUF2=40. */
/*         R_OUT=SQRT((AA(1)*(V_POL+V_CUF2+V_APOL+AA(2)*V_H2O)) */
/*     !         /(AA(3)*PI)) */
/*         R_HS=(R_OUT*R_OUT*AA(3)*0.75)**0.33333333 */
/*         SIGHP=2.*AA(5)*R_HS */
/*         TEMP=273.15+25. */
/*         Z=2.*AA(11)*AA(1) */
/*         ETA=AA(4) */
/*         ION=0.5*(4.*AA(10)*ABS(AA(11))) */
/*       END IF */
/*       IF(NXS.EQ.41)THEN */
/*         PI=ACOS(-1.) */
/*         V_POL= 709.    ! UNITS A**3 */
/*         V_APOL=486. */
/*         V_H2O=  30. */
/*         V_CUF2=40. */
/*         R_HS=((AA(1)*(V_POL+V_CUF2+V_APOL+AA(2)*V_H2O))/PI43) */
/*     !       **0.3333333 */
/* C         WRITE(*,*)' IN FUNC: R_HS =',R_HS */
/*         SIGHP=2.*AA(5)*R_HS */
/*         TEMP=273.15+25. */
/*         Z=2.*AA(11)*AA(1) */
/*         ETA=AA(4) */
/*         ION=0.5*(4.*AA(10)*ABS(AA(11))) */
/*       END IF */
/*       IF(NXS.EQ.58)THEN */
/*         PI=ACOS(-1.) */
/*         R_HS=(AA(2)**2*AA(3)*0.75)**0.3333333 */
/* C         WRITE(*,*)' IN FUNC: R_HS =',R_HS */
/*         SIGHP=2.*R_HS*AA(9) */
/*         TEMP=273.15+AA(8) */
/*         Z=AA(5) */
/*         ETA=AA(6)*1.3E-3 */
/*         ION=0.5*(AA(7)+AA(7)+AA(6)*AA(5)/36800.) */
/*       END IF */

/*      Calculate dielectric constant at a given tempera- */
/*      ture . */

    expo = s_hp__1.temp / (float)219.;
    epsi = exp(-expo) * (float)305.6;

/*      Calculate AKAP ( screening constant) . */

    koz = s_hp__1.ion / (epsi * s_hp__1.temp);
    sqhpb_1.ak = s_hp__1.sighp * (float)50.29 * sqrt(koz);

/*      Calculate QAZ ( surface charge density in */
/*      microcoulomb /cM ** 2) . */

    qaz = s_hp__1.z__ * (float)510.3 / (s_hp__1.sighp * s_hp__1.sighp);


/*      Calculate GAMMA *EXP( -AKAP) from charge density */
/*      -- other units may be used ( see detailed */
/*      description) . */

/* Computing 3rd power */
    r__1 = s_hp__1.sighp, r__2 = r__1;
/* Computing 2nd power */
    r__3 = qaz;
/* Computing 2nd power */
    r__4 = sqhpb_1.ak / (float)2. + (float)1.;
    sqhpb_1.gek = r__2 * (r__1 * r__1) * (float).6425 * (r__3 * r__3) / (epsi
	    * s_hp__1.temp * (r__4 * r__4));
    d__1 = (doublereal) sqhpb_1.eta;
    d__2 = (doublereal) od3;
    s = pow_dd(&d__1, &d__2);
    sqhpb_1.gamk = s * (float)2. * sqhpb_1.gek * exp(sqhpb_1.ak - sqhpb_1.ak /
	     s);

/*      Calculate coefficients, check all is well . */

    *ierr = 0;
    sqcoef_(ierr);
    if (*ierr >= 0) {
	goto L100;
    }
    s_wsle(&io___8);
    do_lio(&c__9, &c__1, "   No roots !!", 14L);
    e_wsle();
/*       PAUSE */
    errors_1.berr = TRUE_;
L100:
    return 0;
}

// Subroutine
int sqcoef_(integer * ir)
{
    /* Initialized data */

    static doublereal acc = (float)5e-6;
    static integer itm = 80;

    /* System generated locals */
    doublereal r__1;

    /* Local variables */
    static integer j;
    static doublereal e1, f1, e2, f2;
    static integer ig;
    static doublereal del;


/*      Calculates rescaled volume fraction and correspon- */
/*      ding coefficients for "SQHPA". */

/*      John B. Hayter  (I.L.L)   14-SEP-81 */

/*      On exit: */

/*      SETA is the rescaled volume fraction. */
/*      SGEK is the rescaled contact potential. */
/*      SAK is the rescaled screening constant. */
/*      A,B,C,F,U,V are the MSA coefficients. */
/*      G1=G(1+) is the contact value of G(R/SIG); */
/*      for the GILLAN condition, the difference from */
/*      zero indicates the computational accuracy. */

/*      IR > 0: Normal exit, IR is the number of itera- */
/*              tions. */
/*         < 0: Failed to converge */


/*       COMMON  / SQHPB /ETA, GEK, AK, A, B, C, F */
/*     1 , U, V, GAMK, SETA, SGEK, SAK, SCAL */
/*     2 , G1, AAAAAA( 5), LLLLLL */

    ig = 1;
    if (sqhpb_1.ak < sqhpb_1.eta * (float)8. + (float)1.) {
	goto L50;
    }
    ig = 0;
    sqfun_(&sqhpb_1.g1, &sqhpb_1.eta, &c__1, ir);
    if (*ir < 0 || sqhpb_1.g1 >= (float)0.) {
	return 0;
    }
L50:
    sqhpb_1.seta = dmin(sqhpb_1.eta,(float).2);
    if (ig == 1 && sqhpb_1.gamk < (float).15) {
	goto L400;
    }
    j = 0;
L100:
    ++j;
    if (j > itm) {
	goto L200;
    }
    if (sqhpb_1.seta <= (float)0.) {
	sqhpb_1.seta = sqhpb_1.eta / j;
    }
    if (sqhpb_1.seta > (float).6) {
	sqhpb_1.seta = (float).35 / j;
    }
    e1 = sqhpb_1.seta;
    sqfun_(&f1, &e1, &c__2, ir);
    e2 = sqhpb_1.seta * (float)1.01;
    sqfun_(&f2, &e2, &c__2, ir);
    e2 = e1 - (e2 - e1) * f1 / (f2 - f1);
    sqhpb_1.seta = e2;
    del = (r__1 = (e2 - e1) / e1, dabs(r__1));
    if (del > acc) {
	goto L100;
    }
    sqfun_(&sqhpb_1.g1, &e2, &c__4, ir);
    *ir = j;
    if (ig == 1) {
	goto L300;
    }
    return 0;
L200:
    *ir = -1;
    return 0;
L300:
    if (sqhpb_1.seta >= sqhpb_1.eta) {
	return 0;
    }
L400:
    sqfun_(&sqhpb_1.g1, &sqhpb_1.eta, &c__3, ir);
    if (*ir < 0 || sqhpb_1.g1 >= (float)0.) {
	return 0;
    }
    *ir = -3;
    return 0;
}

// Subroutine
int sqfun_(doublereal * fval, doublereal * evar, integer * ix, integer * ir)
{
    /* Initialized data */

    static doublereal acc = 1e-6;
    static integer itm = 80;

    /* System generated locals */
    doublereal d__1;

    /* Builtin functions */
//    double pow_dd(), exp(), sqrt();

    /* Local variables */
    static integer ibig;
    static doublereal ckma, eta21, eta22, eta32, skma, fund, eta2d, eta3d,
	    eta6d, d__;
    static integer i__;
    static doublereal eta2d2, a1, a2, a3, b1, d2, b2, b3, p1, p2, p3, t1, t2,
	    v1, v2, v3, t3, w4, w3, w2, w1, w0, ca, fa, e12, e24, ak, g24, ck,
	     pg, w12, w14, w15, sk, w16, pp, w13, w26, w25, w24, w36, w35,
	    w34, w32, w46, w56, be1, be2, dd2, be3, dd4, ak1, ak2, al1, al2,
	    al3, al4, al5, al6, ta1, ta2, ta3, ph1, ph2, ta4, ta5, ex1, ex2,
	    pp1, pp2, um1, um2, um3, um4, um5, um6, vu1, vu2, vu3, vu4, vu5,
	    dak, dd45, e24g, gek, fap, eta, p1p2, w3425, w3526, fun, pwk, qpw,
	     dak2, dak4, eta2, eta3;

/*      Calculates various coefficients and function valu- */
/*      es for "SQCOEF" ( used by "SQHPA"). */

/*      ** This routine works locally in double precision * */

/*      JOHN B. HAYTER ( I. L. L.) 23 -MAR -82 */

/*      IX      =1 : solve for large K, return G( 1+). */
/*              =2 : return function to solve for ETA */
/*                      ( Gillan). */
/*              =3 : assume near Gillan, solve, return */
/*                      G( 1+). */
/*              =4 : return G( 1+) for ETA = ETA( Gil- */
/*                      lan). */

/*      IMPLICIT REAL *8 ( A -H, O -Z) */
/*      REAL *4 RETA, RGEK, RAK, U, V, EVAR, FVAL */
/*      1       , GAMK, SETA, SGEK, SAK, SCAL */

/*      COMMON  / SQHPB /RETA, RGEK, RAK, AHP, BHP, CHP, FHP */
/*      1       , U, V, GAMK, SETA, SGEK, SAK, SCAL */

/*      Calculate constants ; notation is Hayter -Penfold */
/*      ( 1981). */

/*      Calculates scaled quartic coefficient for SQHP. */
/*      This routine works locally in double precision. */


/*       COMMON  / SQHPB /RETA, RGEK, RAK, A, B, C, F */
/*     1 , U, V, GAMK, SETA, SGEK, SAK, SCAL, AAAAAA, LLLLLL */

    eta = *evar;
    eta2 = eta * eta;
    eta3 = eta2 * eta;
    e12 = eta * 12.;
    e24 = e12 + e12;
    d__1 = (doublereal) (sqhpb_2.reta / *evar);
    sqhpb_2.scal = pow_dd(&d__1, &c_b18);
    sqhpb_2.sak = sqhpb_2.rak / sqhpb_2.scal;
    ibig = 0;
    if (sqhpb_2.sak > (float)15. && *ix == 1) {
	ibig = 1;
    }
    sqhpb_2.sgek = sqhpb_2.rgek * sqhpb_2.scal * exp(sqhpb_2.rak -
	    sqhpb_2.sak);
    gek = sqhpb_2.sgek;
    ak = sqhpb_2.sak;
    ak2 = ak * ak;
    ak1 = ak + 1.;
    dak2 = 1. / ak2;
    dak4 = dak2 * dak2;
    d__ = 1. - eta;
    d2 = d__ * d__;
    dak = d__ / ak;
    dd2 = 1. / d2;
    dd4 = dd2 * dd2;
    dd45 = dd4 * .2;
    eta3d = eta * 3.;
    eta6d = eta3d + eta3d;
    eta32 = eta3 + eta3;
    eta2d = eta + 2.;
    eta2d2 = eta2d * eta2d;
    eta21 = eta * 2. + 1.;
    eta22 = eta21 * eta21;

/*      ALPHA( I) */

    al1 = -eta21 * dak;
    al2 = (eta2 * 14. - eta * 4. - 1.) * dak2;
    al3 = eta2 * 36. * dak4;

/*      BETA( I) */

    be1 = -(eta2 + eta * 7. + 1.) * dak;
    be2 = eta * 9. * (eta2 + eta * 4. - 2.) * dak2;
    be3 = eta * 12. * (eta2 * 2. + eta * 8. - 1.) * dak4;

/*      NU( I) */

    vu1 = -(eta3 + eta2 * 3. + eta * 45. + 5.) * dak;
    vu2 = (eta32 + eta2 * 3. + eta * 42. - 20.) * dak2;
    vu3 = (eta32 + eta * 30. - 5.) * dak4;
    vu4 = vu1 + e24 * ak * vu3;
    vu5 = eta6d * (vu2 + vu3 * 4.);

/*      PHI( I) */

    ph1 = eta6d / ak;
    ph2 = d__ - e12 * dak2;

/*      TAU( I) */

    ta1 = (eta + 5.) / (ak * 5.);
    ta2 = eta2d * dak2;
    ta3 = -e12 * gek * (ta1 + ta2);
    ta4 = eta3d * ak2 * (ta1 * ta1 - ta2 * ta2);
    ta5 = eta3d * (eta + 8.) * .1 - eta22 * 2. * dak2;

/*      Double precision SINH( K), COSH( K). */

    ex1 = exp(ak);
    ex2 = 0.;
    if (sqhpb_2.sak < 20.)
    {
	ex2 = exp(-ak);
    }
    sk = (ex1 - ex2) * .5;
    ck = (ex1 + ex2) * .5;
    ckma = ck - 1. - ak * sk;
    skma = sk - ak * ck;

/*      A( I) */

    a1 = (e24 * gek * (al1 + al2 + ak1 * al3) - eta22) * dd4;
    if (ibig != 0)
    {
	goto L10;
    }
    a2 = e24 * (al3 * skma + al2 * sk - al1 * ck) * dd4;
    a3 = e24 * (eta22 * dak2 - d2 * .5 + al3 * ckma - al1 * sk + al2 * ck) *
	    dd4;

/*      B( I) */

L10:
    b1 = (eta * 1.5 * eta2d2 - e12 * gek * (be1 + be2 + ak1 * be3)) * dd4;
    if (ibig != 0)
    {
	goto L20;
    }
    b2 = e12 * (-be3 * skma - be2 * sk + be1 * ck) * dd4;
    b3 = e12 * (d2 * .5 * eta2d - eta3d * eta2d2 * dak2 - be3 * ckma + be1 *
	    sk - be2 * ck) * dd4;

/*      V( I) */

L20:
    v1 = (eta21 * (eta2 - eta * 2. + 10.) * .25 - gek * (vu4 + vu5)) * dd45;
    if (ibig != 0)
    {
	goto L30;
    }
    v2 = (vu4 * ck - vu5 * sk) * dd45;
    v3 = ((eta3 - eta2 * 6. + 5.) * d__ - eta6d * (eta3 * 2. - eta2 * 3. +
	    eta * 18. + 10.) * dak2 + e24 * vu3 + vu4 * sk - vu5 * ck) * dd45;

L30:
    pp1 = ph1 * ph1;
    pp2 = ph2 * ph2;
    pp = pp1 + pp2;
    p1p2 = ph1 * ph2 * 2.;

/*      P( I) */

    p1 = (gek * (pp1 + pp2 - p1p2) - eta2d * .5) * dd2;
    if (ibig != 0)
    {
	goto L40;
    }
    p2 = (pp * sk + p1p2 * ck) * dd2;
    p3 = (pp * ck + p1p2 * sk + pp1 - pp2) * dd2;

/*      T( I) */

L40:
    t1 = ta3 + ta4 * a1 + ta5 * b1;
    if (ibig != 0)
    {
	goto L600;
    }
    t2 = ta4 * a2 + ta5 * b2 + e12 * (ta1 * ck - ta2 * sk);
    t3 = ta4 * a3 + ta5 * b3 + e12 * (ta1 * sk - ta2 * (ck - 1.)) - eta * .4 *
	     (eta + 10.) - 1.;

/*      MU( I) */

    um1 = t2 * a2 - e12 * v2 * v2;
    um2 = t1 * a2 + t2 * a1 - e24 * v1 * v2;
    um3 = t2 * a3 + t3 * a2 - e24 * v2 * v3;
    um4 = t1 * a1 - e12 * v1 * v1;
    um5 = t1 * a3 + t3 * a1 - e24 * v1 * v3;
    um6 = t3 * a3 - e12 * v3 * v3;

/*      Gillan condition ? */

/*      Yes -- G( X = 1 +) = 0 */

/*      Coefficients and function value. */

/* CC    IF( ( IX .AND. 1) .NE. 0) GO TO 100 */
    if (*ix - (*ix / 2 << 1) != 0)
    {
	goto L100;
    }
/* CC */
    ca = ak2 * p1 + (b3 * p1 - b1 * p3) * 2.;
    ca = -ca / (ak2 * p2 + (b3 * p2 - b2 * p3) * 2.);
    fa = -(p1 + p2 * ca) / p3;
    if (*ix == 2)
    {
	*fval = um1 * ca * ca + (um2 + um3 * fa) * ca + um4 + um5 * fa + um6 * fa * fa;
    }
    if (*ix == 4)
    {
	*fval = -(p1 + p2 * ca + p3 * fa);
    }
L50:
    sqhpb_2.fhp = fa;
    sqhpb_2.chp = ca;
    sqhpb_2.bhp = b1 + b2 * ca + b3 * fa;
    sqhpb_2.ahp = a1 + a2 * ca + a3 * fa;
    sqhpb_2.v = (v1 + v2 * ca + v3 * fa) / sqhpb_2.ahp;
L60:
    g24 = e24 * gek * ex1;
    sqhpb_2.u = (ak * ak2 * ca - g24) / (ak2 * g24);
    return 0;

/*      No -- calculate remaining coefficients. */

/*      Lambda( I) */

L100:
    al1 = e12 * p2 * p2;
    al2 = e24 * p1 * p2 - b2 - b2;
    al3 = e24 * p2 * p3;
    al4 = e12 * p1 * p1 - b1 - b1;
    al5 = e24 * p1 * p3 - b3 - b3 - ak2;
    al6 = e12 * p3 * p3;

/*      OMEGA( I, J) */

    w16 = um1 * al6 - al1 * um6;
    w15 = um1 * al5 - al1 * um5;
    w14 = um1 * al4 - al1 * um4;
    w13 = um1 * al3 - al1 * um3;
    w12 = um1 * al2 - al1 * um2;
    w26 = um2 * al6 - al2 * um6;
    w25 = um2 * al5 - al2 * um5;
    w24 = um2 * al4 - al2 * um4;
    w36 = um3 * al6 - al3 * um6;
    w35 = um3 * al5 - al3 * um5;
    w34 = um3 * al4 - al3 * um4;
    w32 = um3 * al2 - al3 * um2;
    w46 = um4 * al6 - al4 * um6;
    w56 = um5 * al6 - al5 * um6;
    w3526 = w35 + w26;
    w3425 = w34 + w25;

/*      Quartic coefficients W( I) */

    w4 = w16 * w16 - w13 * w36;
    w3 = w16 * 2. * w15 - w13 * w3526 - w12 * w36;
    w2 = w15 * w15 + w16 * 2. * w14 - w13 * w3425 - w12 * w3526;
    w1 = w15 * 2. * w14 - w13 * w24 - w12 * w3425;
    w0 = w14 * w14 - w12 * w24;

/*      Estimate the starting value of F. */

    if (*ix == 1)
    {
	goto L200;
    }

/*      Assume not too far from Gillan condition. */
/*      If both GEK and AK are small, use P-W estimate. */

    sqhpb_2.g1 = eta2d * .5 * dd2 * exp(-gek);
    if (sqhpb_2.sgek > 2. || sqhpb_2.sak > 1.)
    {
	goto L150;
    }
    e24g = e24 * gek * exp(ak);
    pwk = sqrt(e24g);
    qpw = (1. - sqrt(d2 * 2. * d__ * pwk / eta22 + 1.)) * eta21 / d__;
    sqhpb_2.g1 = -qpw * qpw / e24 + eta2d * .5 * dd2;
L150:
    pg = p1 + sqhpb_2.g1;
    ca = ak2 * pg + (b3 * pg - b1 * p3) * 2. + e12 * sqhpb_2.g1 * sqhpb_2.g1 *
	     p3;
    ca = -ca / (ak2 * p2 + (b3 * p2 - b2 * p3) * 2.);
    fap = -(pg + p2 * ca) / p3;
    goto L250;

/*      Large K. */

L200:
    fap = (w14 - w34 - w46) / (w12 - w15 + w35 - w26 + w56 - w32);

/*      And refine it according to Newton. */

L250:
    i__ = 0;
L300:
    ++i__;
    if (i__ > itm)
    {
	goto L400;
    }
    fa = fap;
    fun = w0 + (w1 + (w2 + (w3 + w4 * fa) * fa) * fa) * fa;
    fund = w1 + (w2 * 2. + (w3 * 3. + w4 * 4. * fa) * fa) * fa;
    fap = fa - fun / fund;
    if ((d__1 = (fap - fa) / fa, abs(d__1)) > acc)
    {
	goto L300;
    }
    *ir += i__;
    goto L500;

/*      Falled to converge in ITM iterations. */

L400:
    *ir = -2;
    return 0;

L500:
    fa = fap;
    ca = -(w16 * fa * fa + w15 * fa + w14) / (w13 * fa + w12);
    sqhpb_2.g1 = -(p1 + p2 * ca + p3 * fa);
    *fval = sqhpb_2.g1;
    if (dabs(*fval) < (float).001)
    {
	*fval = (float)0.;
    }
    sqhpb_2.seta = *evar;
    goto L50;

/*      Very large screening : asymptotic solution. */

L600:
    v3 = ((eta3 - eta2 * 6. + 5.) * d__ - eta6d * (eta3 * 2. - eta2 * 3. +
	    eta * 18. + 10.) * dak2 + e24 * vu3) * dd45;
    t3 = ta4 * a3 + ta5 * b3 + e12 * ta2 - eta * .4 * (eta + 10.) - 1.;
    p3 = (pp1 - pp2) * dd2;
    b3 = e12 * (d2 * .5 * eta2d - eta3d * eta2d2 * dak2 + be3) * dd4;
    a3 = e24 * (eta22 * dak2 - d2 * .5 - al3) * dd4;
    um6 = t3 * a3 - e12 * v3 * v3;
    um5 = t1 * a3 + a1 * t3 - e24 * v1 * v3;
    um4 = t1 * a1 - e12 * v1 * v1;
    al6 = e12 * p3 * p3;
    al5 = e24 * p1 * p3 - b3 - b3 - ak2;
    al4 = e12 * p1 * p1 - b1 - b1;
    w56 = um5 * al6 - al5 * um6;
    w46 = um4 * al6 - al4 * um6;
    fa = -w46 / w56;
    ca = -fa;
    sqhpb_2.fhp = fa;
    sqhpb_2.chp = ca;
    sqhpb_2.bhp = b1 + b3 * fa;
    sqhpb_2.ahp = a1 + a3 * fa;
    sqhpb_2.v = v1 + v3 * fa;
    sqhpb_2.g1 = -(p1 + p3 * fa);
    *fval = sqhpb_2.g1;
    if (dabs(*fval) < (float).001)
    {
	*fval = (float)0.;
    }
    sqhpb_2.seta = *evar;
    goto L60;
}

doublereal sq_hp__(doublereal * q)
{
    /* System generated locals */
    doublereal ret_val;
    doublereal aqk1,aqk2,aqk3,aqk4;

    /* Builtin functions */
//    double exp(), sin(), cos();

    /* Local variables */
    static doublereal cosk, sink, asink, qcosk, x1, x2, e24, ak, ck, qk, sk,
	    ak2, q2k, qk2, qk3, gek, eta, aqk, qqk;

/*       SUBROUTINE SQHCAL( NPT) */

/*      Calculates HAYTER -PENFOLD S( Q *) from quartic */
/*      solution. */



/*      Data spaces for data of algorithm. */


/*       COMMON  / SQHPB  /ETAZ, GEKZ, AKZ, A, B, C, F */
/*     1 , U, V, GAMK, SETA, SGEK, SAK, SCAL, AAAAAA, LLLLLL */

/*      Calculate constants. */

/* L111: */
    qk = (doublereal) (*q);
    eta = sqhpb_3.seta;
    ak = sqhpb_3.sak;
    gek = sqhpb_3.sgek;
    e24 = eta * 24.;
    x1 = exp(ak);
    x2 = 0.;
    if (sqhpb_3.sak < (float)20.)
    {
	x2 = exp(-ak);
    }
    ck = (x1 + x2) * .5;
    sk = (x1 - x2) * .5;
    ak2 = ak * ak;


    if (qk <= 0.)
    {
	goto L100;
    }
    qk /= sqhpb_3.scal;
    q2k = qk * qk;
    qk2 = 1. / q2k;
    qk3 = qk2 / qk;
    qqk = 1. / (qk * (q2k + ak2));
    sink = sin(qk);
    asink = ak * sink;
    cosk = cos(qk);
    qcosk = qk * cosk;

    aqk1= (  sqhpb_3.ahp * (sink - qcosk) +
		     sqhpb_3.bhp * ( (qk2 * 2. - 1.) * qcosk +
			                  sink * 2. -
							  2. / qk
						   ) +
			 eta * .5 * sqhpb_3.ahp *
			    (  qk3 * 24. +
				  (1. - qk2 * 6.) * 4. * sink -
		          (1. - qk2 * 12. + qk2 * 24. * qk2) * qcosk
				)
		  ) * qk3 ;
    aqk2=sqhpb_3.chp * (ck * asink - sk * qcosk) * qqk ;
    aqk3=sqhpb_3.fhp * (sk * asink - qk * (ck * cosk - 1.)) * qqk ;
    aqk4=sqhpb_3.fhp * (cosk - 1.) * qk2 - gek * (asink + qcosk) * qqk;
    aqk=aqk1+aqk2+aqk3+aqk4;
    ret_val = (doublereal) (1. / (1. - e24 * aqk));
    goto L200;
L100:
    ret_val = (doublereal) (-1. / sqhpb_3.ahp);
L200:
    return ret_val;
}

