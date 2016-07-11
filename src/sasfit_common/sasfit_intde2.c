/*
 * The code is an adaption of a numerical package from Takuya OOURA 
 * (email: ooura@mmm.t.u-tokyo.ac.jp)
 * under http://www.kurims.kyoto-u.ac.jp/~ooura/ he supplies several numerical packages in Fortran and C.
 * The code below has been taken from his libarary package for numerical integration (Quadrature) - 
 * DE Formula (Almighty Quadrature).
 * 
 * see also the References:
 * [1] M.Mori, Developments in the double exponential formula for numerical integration, Proceedings of the International Congress of Mathematicians, Kyoto 1990, 1991, Springer-Verlag, 1585-1594.
 * [2] H.Takahasi and M.Mori, Double exponential formulas for numerical integration, Pub. RIMS Kyoto Univ. 9, 1974, 721-741
 * [3] T.Ooura and M.Mori, The Double exponential formula for oscillatory functions over the half infinite interval, Journal of Computational and Applied Mathematics 38, 1991, 353-360
 * [4] M.Mori and T.Ooura, Double exponential formulas for Fourier type integrals with a divergent integrand, Contributions in Numerical Mathematics, ed. R.P.Agarwal, World Scientific Series in Applicable Analysis, 2, 1993, 301-308
 * [5] H.Toda and H.Ono, Some remarks for efficient usage of the double exponential formulas(in Japanese), Kokyuroku, RIMS, Kyoto Univ. 339, 1978, 74-109. 
 */
 
/*
DE-Quadrature
Numerical Automatic Integrator for Improper Integral
    method    : Double Exponential (DE) Transformation
    dimension : one
    table     : use
functions
    intde  : integrator of f(x) over (a,b).
    intdei : integrator of f(x) over (a,infinity), 
                 f(x) is non oscillatory function.
    intdeo : integrator of f(x) over (a,infinity), 
                 f(x) is oscillatory function.
*/

/*
intde
    [description]
        I = integral of f(x) over (a,b)
    [declaration]
        void intdeini(int lenaw, double tiny, double eps, double *aw);
        void intde(double (*f)(double), double a, double b, 
            double *aw, double *i, double *err);
    [usage]
        intdeini(lenaw, tiny, eps, aw);  // initialization of aw
        ...
        intde(f, a, b, aw, &i, &err);
    [parameters]
        lenaw     : length of aw (int)
        tiny      : minimum value that 1/tiny does not 
                    overflow (double)
        eps       : relative error requested (double)
        aw        : points and weights of the quadrature 
                    formula, aw[0...lenaw-1] (double *)
        f         : integrand f(x) (double (*f)(double))
        a         : lower limit of integration (double)
        b         : upper limit of integration (double)
        i         : approximation to the integral (double *)
        err       : estimate of the absolute error (double *)
    [remarks]
        initial parameters
            lenaw > 1000, 
            IEEE double :
                lenaw = 8000;
                tiny = 1.0e-307;
        function
            f(x) needs to be analytic over (a,b).
        relative error
            eps is relative error requested excluding 
            cancellation of significant digits.
            i.e. eps means : (absolute error) / 
                             (integral_a^b |f(x)| dx).
            eps does not mean : (absolute error) / I.
        error message
            err >= 0 : normal termination.
            err < 0  : abnormal termination.
                       i.e. convergent error is detected :
                           1. f(x) or (d/dx)^n f(x) has 
                              discontinuous points or sharp 
                              peaks over (a,b).
                              you must divide the interval 
                              (a,b) at this points.
                           2. relative error of f(x) is 
                              greater than eps.
                           3. f(x) has oscillatory factor 
                              and frequency of the oscillation 
                              is very high.
*/

/*
intdei
    [description]
        I = integral of f(x) over (a,infinity), 
            f(x) has not oscillatory factor.
    [declaration]
        void intdeiini(int lenaw, double tiny, double eps, double *aw);
        void intdei(double (*f)(double), double a, double *aw, 
            double *i, double *err);
    [usage]
        intdeiini(lenaw, tiny, eps, aw);  // initialization of aw
        ...
        intdei(f, a, aw, &i, &err);
    [parameters]
        lenaw     : length of aw (int)
        tiny      : minimum value that 1/tiny does not 
                    overflow (double)
        eps       : relative error requested (double)
        aw        : points and weights of the quadrature 
                    formula, aw[0...lenaw-1] (double *)
        f         : integrand f(x) (double (*f)(double))
        a         : lower limit of integration (double)
        i         : approximation to the integral (double *)
        err       : estimate of the absolute error (double *)
    [remarks]
        initial parameters
            lenaw > 1000, 
            IEEE double :
                lenaw = 8000;
                tiny = 1.0e-307;
        function
            f(x) needs to be analytic over (a,infinity).
        relative error
            eps is relative error requested excluding 
            cancellation of significant digits.
            i.e. eps means : (absolute error) / 
                             (integral_a^infinity |f(x)| dx).
            eps does not mean : (absolute error) / I.
        error message
            err >= 0 : normal termination.
            err < 0  : abnormal termination.
                       i.e. convergent error is detected :
                           1. f(x) or (d/dx)^n f(x) has 
                              discontinuous points or sharp 
                              peaks over (a,infinity).
                              you must divide the interval 
                              (a,infinity) at this points.
                           2. relative error of f(x) is 
                              greater than eps.
                           3. f(x) has oscillatory factor 
                              and decay of f(x) is very slow 
                              as x -> infinity.
*/

/*
intdeo
    [description]
        I = integral of f(x) over (a,infinity), 
            f(x) has oscillatory factor :
            f(x) = g(x) * sin(omega * x + theta) as x -> infinity.
    [declaration]
        void intdeoini(int lenaw, double tiny, double eps, 
            double *aw);
        void intdeo(double (*f)(double), double a, double omega, 
            double *aw, double *i, double *err);
    [usage]
        intdeoini(lenaw, tiny, eps, aw);  // initialization of aw
        ...
        intdeo(f, a, omega, aw, &i, &err);
    [parameters]
        lenaw     : length of aw (int)
        tiny      : minimum value that 1/tiny does not 
                    overflow (double)
        eps       : relative error requested (double)
        aw        : points and weights of the quadrature 
                    formula, aw[0...lenaw-1] (double *)
        f         : integrand f(x) (double (*f)(double))
        a         : lower limit of integration (double)
        omega     : frequency of oscillation (double)
        i         : approximation to the integral (double *)
        err       : estimate of the absolute error (double *)
    [remarks]
        initial parameters
            lenaw > 1000, 
            IEEE double :
                lenaw = 8000;
                tiny = 1.0e-307;
        function
            f(x) needs to be analytic over (a,infinity).
        relative error
            eps is relative error requested excluding 
            cancellation of significant digits.
            i.e. eps means : (absolute error) / 
                             (integral_a^R |f(x)| dx).
            eps does not mean : (absolute error) / I.
        error message
            err >= 0 : normal termination.
            err < 0  : abnormal termination.
                       i.e. convergent error is detected :
                           1. f(x) or (d/dx)^n f(x) has 
                              discontinuous points or sharp 
                              peaks over (a,infinity).
                              you must divide the interval 
                              (a,infinity) at this points.
                           2. relative error of f(x) is 
                              greater than eps.
*/


#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef MACOSX
#include <sys/malloc.h>
#else
#include <malloc.h>
#endif

#include <sasfit_sd.h>
#include <sasfit_sq.h>

#include <tcl.h>


#include <sasfit.h>
#include <sasfit_common.h>
#include <sasfit_core.h>

#include "sasfit_fft2f.c"
#include "sasfit_intcc2.c"

void sasfit_intdeini(int lenaw, double tiny, double eps, double *aw)
{
    /* ---- adjustable parameter ---- */
    double efs = 0.1, hoff = 8.5;
    /* ------------------------------ */
    int noff, nk, k, j;
    double pi2, tinyln, epsln, h0, ehp, ehm, h, t, ep, em, xw, wg;
    
    pi2 = 2 * atan(1.0);
    tinyln = -log(tiny);
    epsln = 1 - log(efs * eps);
    h0 = hoff / epsln;
    ehp = exp(h0);
    ehm = 1 / ehp;
    aw[2] = eps;
    aw[3] = exp(-ehm * epsln);
    aw[4] = sqrt(efs * eps);
    noff = 5;
    aw[noff] = 0.5;
    aw[noff + 1] = h0;
    aw[noff + 2] = pi2 * h0 * 0.5;
    h = 2;
    nk = 0;
    k = noff + 3;
    do {
        t = h * 0.5;
        do {
            em = exp(h0 * t);
            ep = pi2 * em;
            em = pi2 / em;
            j = k;
            do {
                xw = 1 / (1 + exp(ep - em));
                wg = xw * (1 - xw) * h0;
                aw[j] = xw;
                aw[j + 1] = wg * 4;
                aw[j + 2] = wg * (ep + em);
                ep *= ehp;
                em *= ehm;
                j += 3;
            } while (ep < tinyln && j <= lenaw - 3);
            t += h;
            k += nk;
        } while (t < 1);
        h *= 0.5;
        if (nk == 0) {
            if (j > lenaw - 6) j -= 3;
            nk = j - noff;
            k += nk;
            aw[1] = nk;
        }
    } while (2 * k - noff - 3 <= lenaw);
    aw[0] = k - 3;
}


void sasfit_intde(double (*f)(double, void *), double a, double b, double *aw, 
    double *i, double *err, void *fparams)
{
    int noff, lenawm, nk, k, j, jtmp, jm, m, klim;
    double epsh, ba, ir, xa, fa, fb, errt, errh, errd, h, iback, irback;
    
    noff = 5;
    lenawm = (int) (aw[0] + 0.5);
    nk = (int) (aw[1] + 0.5);
    epsh = aw[4];
    ba = b - a;
    *i = (*f)((a + b) * aw[noff],fparams);
    ir = *i * aw[noff + 1];
    *i *= aw[noff + 2];
    *err = fabs(*i);
    k = nk + noff;
    j = noff;
    do {
        j += 3;
        xa = ba * aw[j];
        fa = (*f)(a + xa,fparams);
        fb = (*f)(b - xa,fparams);
        ir += (fa + fb) * aw[j + 1];
        fa *= aw[j + 2];
        fb *= aw[j + 2];
        *i += fa + fb;
        *err += fabs(fa) + fabs(fb);
    } while (aw[j] > epsh && j < k);
    errt = *err * aw[3];
    errh = *err * epsh;
    errd = 1 + 2 * errh;
    jtmp = j;
    while (fabs(fa) > errt && j < k) {
        j += 3;
        fa = (*f)(a + ba * aw[j],fparams);
        ir += fa * aw[j + 1];
        fa *= aw[j + 2];
        *i += fa;
    }
    jm = j;
    j = jtmp;
    while (fabs(fb) > errt && j < k) {
        j += 3;
        fb = (*f)(b - ba * aw[j],fparams);
        ir += fb * aw[j + 1];
        fb *= aw[j + 2];
        *i += fb;
    }
    if (j < jm) jm = j;
    jm -= noff + 3;
    h = 1;
    m = 1;
    klim = k + nk;
    while (errd > errh && klim <= lenawm) {
        iback = *i;
        irback = ir;
        do {
            jtmp = k + jm;
            for (j = k + 3; j <= jtmp; j += 3) {
                xa = ba * aw[j];
                fa = (*f)(a + xa,fparams);
                fb = (*f)(b - xa,fparams);
                ir += (fa + fb) * aw[j + 1];
                *i += (fa + fb) * aw[j + 2];
            }
            k += nk;
            j = jtmp;
            do {
                j += 3;
                fa = (*f)(a + ba * aw[j],fparams);
                ir += fa * aw[j + 1];
                fa *= aw[j + 2];
                *i += fa;
            } while (fabs(fa) > errt && j < k);
            j = jtmp;
            do {
                j += 3;
                fb = (*f)(b - ba * aw[j],fparams);
                ir += fb * aw[j + 1];
                fb *= aw[j + 2];
                *i += fb;
            } while (fabs(fb) > errt && j < k);
        } while (k < klim);
        errd = h * (fabs(*i - 2 * iback) + fabs(ir - 2 * irback));
        h *= 0.5;
        m *= 2;
        klim = 2 * klim - noff;
    }
    *i *= h * ba;
    if (errd > errh) {
        *err = -errd * (m * fabs(ba));
    } else {
        *err = *err * aw[2] * (m * fabs(ba));
    }
}



void sasfit_intdeiini(int lenaw, double tiny, double eps, double *aw)
{
    /* ---- adjustable parameter ---- */
    double efs = 0.1, hoff = 11.0;
    /* ------------------------------ */
    int noff, nk, k, j;
    double pi4, tinyln, epsln, h0, ehp, ehm, h, t, ep, em, xp, xm, 
        wp, wm;
    
    pi4 = atan(1.0);
    tinyln = -log(tiny);
    epsln = 1 - log(efs * eps);
    h0 = hoff / epsln;
    ehp = exp(h0);
    ehm = 1 / ehp;
    aw[2] = eps;
    aw[3] = exp(-ehm * epsln);
    aw[4] = sqrt(efs * eps);
    noff = 5;
    aw[noff] = 1;
    aw[noff + 1] = 4 * h0;
    aw[noff + 2] = 2 * pi4 * h0;
    h = 2;
    nk = 0;
    k = noff + 6;
    do {
        t = h * 0.5;
        do {
            em = exp(h0 * t);
            ep = pi4 * em;
            em = pi4 / em;
            j = k;
            do {
                xp = exp(ep - em);
                xm = 1 / xp;
                wp = xp * ((ep + em) * h0);
                wm = xm * ((ep + em) * h0);
                aw[j] = xm;
                aw[j + 1] = xp;
                aw[j + 2] = xm * (4 * h0);
                aw[j + 3] = xp * (4 * h0);
                aw[j + 4] = wm;
                aw[j + 5] = wp;
                ep *= ehp;
                em *= ehm;
                j += 6;
            } while (ep < tinyln && j <= lenaw - 6);
            t += h;
            k += nk;
        } while (t < 1);
        h *= 0.5;
        if (nk == 0) {
            if (j > lenaw - 12) j -= 6;
            nk = j - noff;
            k += nk;
            aw[1] = nk;
        }
    } while (2 * k - noff - 6 <= lenaw);
    aw[0] = k - 6;
}


void sasfit_intdei(double (*f)(double, void *), double a, double *aw, double *i, 
    double *err, void * fparams)
{
    int noff, lenawm, nk, k, j, jtmp, jm, m, klim;
    double epsh, ir, fp, fm, errt, errh, errd, h, iback, irback;
    
    noff = 5;
    lenawm = (int) (aw[0] + 0.5);
    nk = (int) (aw[1] + 0.5);
    epsh = aw[4];
    *i = (*f)(a + aw[noff],fparams);
    ir = *i * aw[noff + 1];
    *i *= aw[noff + 2];
    *err = fabs(*i);
    k = nk + noff;
    j = noff;
    do {
        j += 6;
        fm = (*f)(a + aw[j],fparams);
        fp = (*f)(a + aw[j + 1],fparams);
        ir += fm * aw[j + 2] + fp * aw[j + 3];
        fm *= aw[j + 4];
        fp *= aw[j + 5];
        *i += fm + fp;
        *err += fabs(fm) + fabs(fp);
    } while (aw[j] > epsh && j < k);
    errt = *err * aw[3];
    errh = *err * epsh;
    errd = 1 + 2 * errh;
    jtmp = j;
    while (fabs(fm) > errt && j < k) {
        j += 6;
        fm = (*f)(a + aw[j],fparams);
        ir += fm * aw[j + 2];
        fm *= aw[j + 4];
        *i += fm;
    }
    jm = j;
    j = jtmp;
    while (fabs(fp) > errt && j < k) {
        j += 6;
        fp = (*f)(a + aw[j + 1],fparams);
        ir += fp * aw[j + 3];
        fp *= aw[j + 5];
        *i += fp;
    }
    if (j < jm) jm = j;
    jm -= noff + 6;
    h = 1;
    m = 1;
    klim = k + nk;
    while (errd > errh && klim <= lenawm) {
        iback = *i;
        irback = ir;
        do {
            jtmp = k + jm;
            for (j = k + 6; j <= jtmp; j += 6) {
                fm = (*f)(a + aw[j],fparams);
                fp = (*f)(a + aw[j + 1],fparams);
                ir += fm * aw[j + 2] + fp * aw[j + 3];
                *i += fm * aw[j + 4] + fp * aw[j + 5];
            }
            k += nk;
            j = jtmp;
            do {
                j += 6;
                fm = (*f)(a + aw[j],fparams);
                ir += fm * aw[j + 2];
                fm *= aw[j + 4];
                *i += fm;
            } while (fabs(fm) > errt && j < k);
            j = jtmp;
            do {
                j += 6;
                fp = (*f)(a + aw[j + 1],fparams);
                ir += fp * aw[j + 3];
                fp *= aw[j + 5];
                *i += fp;
            } while (fabs(fp) > errt && j < k);
        } while (k < klim);
        errd = h * (fabs(*i - 2 * iback) + fabs(ir - 2 * irback));
        h *= 0.5;
        m *= 2;
        klim = 2 * klim - noff;
    }
    *i *= h;
    if (errd > errh) {
        *err = -errd * m;
    } else {
        *err *= aw[2] * m;
    }
}



void sasfit_intdeoini(int lenaw, double tiny, double eps, double *aw)
{
    /* ---- adjustable parameter ---- */
    int lmax = 5;
    double efs = 0.1, enoff = 0.40, pqoff = 2.9, ppoff = -0.72;
    /* ------------------------------ */
    int noff0, nk0, noff, k, nk, j;
    double pi4, tinyln, epsln, frq4, per2, pp, pq, ehp, ehm, h, t, 
        ep, em, tk, xw, wg, xa;
    
    pi4 = atan(1.0);
    tinyln = -log(tiny);
    epsln = 1 - log(efs * eps);
    frq4 = 1 / (2 * pi4);
    per2 = 4 * pi4;
    pq = pqoff / epsln;
    pp = ppoff - log(pq * pq * frq4);
    ehp = exp(2 * pq);
    ehm = 1 / ehp;
    aw[3] = lmax;
    aw[4] = eps;
    aw[5] = sqrt(efs * eps);
    noff0 = 6;
    nk0 = 1 + (int) (enoff * epsln);
    aw[1] = nk0;
    noff = 2 * nk0 + noff0;
    wg = 0;
    xw = 1;
    for (k = 1; k <= nk0; k++) {
        wg += xw;
        aw[noff - 2 * k] = wg;
        aw[noff - 2 * k + 1] = xw;
        xw = xw * (nk0 - k) / k;
    }
    wg = per2 / wg;
    for (k = noff0; k <= noff - 2; k += 2) {
        aw[k] *= wg;
        aw[k + 1] *= wg;
    }
    xw = exp(pp - 2 * pi4);
    aw[noff] = sqrt(xw * (per2 * 0.5));
    aw[noff + 1] = xw * pq;
    aw[noff + 2] = per2 * 0.5;
    h = 2;
    nk = 0;
    k = noff + 3;
    do {
        t = h * 0.5;
        do {
            em = exp(2 * pq * t);
            ep = pi4 * em;
            em = pi4 / em;
            tk = t;
            j = k;
            do {
                xw = exp(pp - ep - em);
                wg = sqrt(frq4 * xw + tk * tk);
                xa = xw / (tk + wg);
                wg = (pq * xw * (ep - em) + xa) / wg;
                aw[j] = xa;
                aw[j + 1] = xw * pq;
                aw[j + 2] = wg;
                ep *= ehp;
                em *= ehm;
                tk += 1;
                j += 3;
            } while (ep < tinyln && j <= lenaw - 3);
            t += h;
            k += nk;
        } while (t < 1);
        h *= 0.5;
        if (nk == 0) {
            if (j > lenaw - 6) j -= 3;
            nk = j - noff;
            k += nk;
            aw[2] = nk;
        }
    } while (2 * k - noff - 3 <= lenaw);
    aw[0] = k - 3;
}


void sasfit_intdeo(double (*f)(double, void *), double a, double omega, double *aw, 
    double *i, double *err, void *fparams)
{
    int lenawm, nk0, noff0, nk, noff, lmax, m, k, j, jm, l;
    double eps, per, perw, w02, ir, h, iback, irback, t, tk, 
        xa, fm, fp, errh, s0, s1, s2, errd;
    
    
    lenawm = (int) (aw[0] + 0.5);
    nk0 = (int) (aw[1] + 0.5);
    noff0 = 6;
    nk = (int) (aw[2] + 0.5);
    noff = 2 * nk0 + noff0;
    lmax = (int) (aw[3] + 0.5);
    eps = aw[4];
    per = 1 / fabs(omega);
    w02 = 2 * aw[noff + 2];
    perw = per * w02;
    
    *i = (*f)(a + aw[noff] * per,fparams);
    
    ir = *i * aw[noff + 1];
    
    *i *= aw[noff + 2];
    
    *err = fabs(*i);
    
    h = 2;
    m = 1;
    k = noff;
    do {
        iback = *i;
        irback = ir;
        
        t = h * 0.5;
        do {
            if (k == noff) {
                tk = 1;
                k += nk;
                j = noff;
                do {
                    j += 3;
                    xa = per * aw[j];
                    fm = (*f)(a + xa,fparams);
                    fp = (*f)(a + xa + perw * tk,fparams);
                    ir += (fm + fp) * aw[j + 1];
                    fm *= aw[j + 2];
                    fp *= w02 - aw[j + 2];
                    *i += fm + fp;
                    *err += fabs(fm) + fabs(fp);
                    tk += 1;
                } while (aw[j] > eps && j < k);
                errh = *err * aw[5];
                *err *= eps;
                jm = j - noff;
            } else {
                tk = t;
                for (j = k + 3; j <= k + jm; j += 3) {
                    xa = per * aw[j];
                    fm = (*f)(a + xa,fparams);
                    fp = (*f)(a + xa + perw * tk,fparams);
                    ir += (fm + fp) * aw[j + 1];
                    fm *= aw[j + 2];
                    fp *= w02 - aw[j + 2];
                    *i += fm + fp;
                    tk += 1;
                }
                j = k + jm;
                k += nk;
            }
            while (fabs(fm) > *err && j < k) {
                j += 3;
                fm = (*f)(a + per * aw[j],fparams);
                ir += fm * aw[j + 1];
                fm *= aw[j + 2];
                *i += fm;
            }
            fm = (*f)(a + perw * tk,fparams);
            s2 = w02 * fm;
            *i += s2;
            if (fabs(fp) > *err || fabs(s2) > *err) {
                l = 0;
                for (;;) {
                    l++;
                    s0 = 0;
                    s1 = 0;
                    s2 = fm * aw[noff0 + 1];
                    for (j = noff0 + 2; j <= noff - 2; j += 2) {
                        tk += 1;
                        fm = (*f)(a + perw * tk,fparams);
                        s0 += fm;
                        s1 += fm * aw[j];
                        s2 += fm * aw[j + 1];
                    }
                    if (s2 <= *err || l >= lmax) break;
                    *i += w02 * s0;
                }
                *i += s1;
                if (s2 > *err) *err = s2;
            }
            t += h;
        } while (t < 1);
        if (m == 1) {
            errd = 1 + 2 * errh;
        } else {
            errd = h * (fabs(*i - 2 * iback) + fabs(ir - 2 * irback));
        }
        h *= 0.5;
        m *= 2;
    } while (errd > errh && 2 * k - noff <= lenawm);
    *i *= h * per;
    if (errd > errh) {
        *err = -errd * per;
    } else {
        *err *= per * m * 0.5;
    }
}

