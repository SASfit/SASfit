/*
DE-Quadrature
Numerical Automatic Integrator for Improper Integral
    method    : Double Exponential (DE) Transformation
    dimension : one
    table     : not use
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
        void intde(double (*f)(double), double a, double b, double eps, 
            double *i, double *err);
    [usage]
        intde(f, a, b, eps, &i, &err);
    [parameters]
        f         : integrand f(x) (double (*f)(double))
        a         : lower limit of integration (double)
        b         : upper limit of integration (double)
        eps       : relative error requested (double)
        i         : approximation to the integral (double *)
        err       : estimate of the absolute error (double *)
    [remarks]
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
            err < 0  : abnormal termination (m >= mmax).
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
        void intdei(double (*f)(double), double a, double eps, 
            double *i, double *err);
    [usage]
        intdei(f, a, eps, &i, &err);
    [parameters]
        f         : integrand f(x) (double (*f)(double))
        a         : lower limit of integration (double)
        eps       : relative error requested (double)
        i         : approximation to the integral (double *)
        err       : estimate of the absolute error (double *)
    [remarks]
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
            err < 0  : abnormal termination (m >= mmax).
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
        void intdeo(double (*f)(double), double a, double omega, 
            double eps, double *i, double *err);
    [usage]
        intdeo(f, a, omega, eps, &i, &err);
    [parameters]
        f         : integrand f(x) (double (*f)(double))
        a         : lower limit of integration (double)
        omega     : frequency of oscillation (double)
        eps       : relative error requested (double)
        i         : approximation to the integral (double *)
        err       : estimate of the absolute error (double *)
    [remarks]
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
            err < 0  : abnormal termination (m >= mmax).
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

void intde(double (*f)(double), double a, double b, double eps, 
    double *i, double *err)
{
    /* ---- adjustable parameter ---- */
    int mmax = 256;
    double efs = 0.1, hoff = 8.5;
    /* ------------------------------ */
    int m;
    double pi2, epsln, epsh, h0, ehp, ehm, epst, ba, ir, h, iback, 
        irback, t, ep, em, xw, xa, wg, fa, fb, errt, errh, errd;
    
    pi2 = 2 * atan(1.0);
    epsln = 1 - log(efs * eps);
    epsh = sqrt(efs * eps);
    h0 = hoff / epsln;
    ehp = exp(h0);
    ehm = 1 / ehp;
    epst = exp(-ehm * epsln);
    ba = b - a;
    ir = (*f)((a + b) * 0.5) * (ba * 0.25);
    *i = ir * (2 * pi2);
    *err = fabs(*i) * epst;
    h = 2 * h0;
    m = 1;
    do {
        iback = *i;
        irback = ir;
        t = h * 0.5;
        do {
            em = exp(t);
            ep = pi2 * em;
            em = pi2 / em;
            do {
                xw = 1 / (1 + exp(ep - em));
                xa = ba * xw;
                wg = xa * (1 - xw);
                fa = (*f)(a + xa) * wg;
                fb = (*f)(b - xa) * wg;
                ir += fa + fb;
                *i += (fa + fb) * (ep + em);
                errt = (fabs(fa) + fabs(fb)) * (ep + em);
                if (m == 1) *err += errt * epst;
                ep *= ehp;
                em *= ehm;
            } while (errt > *err || xw > epsh);
            t += h;
        } while (t < h0);
        if (m == 1) {
            errh = (*err / epst) * epsh * h0;
            errd = 1 + 2 * errh;
        } else {
            errd = h * (fabs(*i - 2 * iback) + 4 * fabs(ir - 2 * irback));
        }
        h *= 0.5;
        m *= 2;
    } while (errd > errh && m < mmax);
    *i *= h;
    if (errd > errh) {
        *err = -errd * m;
    } else {
        *err = errh * epsh * m / (2 * efs);
    }
}



void intdei(double (*f)(double), double a, double eps, 
    double *i, double *err)
{
    /* ---- adjustable parameter ---- */
    int mmax = 256;
    double efs = 0.1, hoff = 11.0;
    /* ------------------------------ */
    int m;
    double pi4, epsln, epsh, h0, ehp, ehm, epst, ir, h, iback, irback, 
        t, ep, em, xp, xm, fp, fm, errt, errh, errd;
    
    pi4 = atan(1.0);
    epsln = 1 - log(efs * eps);
    epsh = sqrt(efs * eps);
    h0 = hoff / epsln;
    ehp = exp(h0);
    ehm = 1 / ehp;
    epst = exp(-ehm * epsln);
    ir = (*f)(a + 1);
    *i = ir * (2 * pi4);
    *err = fabs(*i) * epst;
    h = 2 * h0;
    m = 1;
    do {
        iback = *i;
        irback = ir;
        t = h * 0.5;
        do {
            em = exp(t);
            ep = pi4 * em;
            em = pi4 / em;
            do {
                xp = exp(ep - em);
                xm = 1 / xp;
                fp = (*f)(a + xp) * xp;
                fm = (*f)(a + xm) * xm;
                ir += fp + fm;
                *i += (fp + fm) * (ep + em);
                errt = (fabs(fp) + fabs(fm)) * (ep + em);
                if (m == 1) *err += errt * epst;
                ep *= ehp;
                em *= ehm;
            } while (errt > *err || xm > epsh);
            t += h;
        } while (t < h0);
        if (m == 1) {
            errh = (*err / epst) * epsh * h0;
            errd = 1 + 2 * errh;
        } else {
            errd = h * (fabs(*i - 2 * iback) + 4 * fabs(ir - 2 * irback));
        }
        h *= 0.5;
        m *= 2;
    } while (errd > errh && m < mmax);
    *i *= h;
    if (errd > errh) {
        *err = -errd * m;
    } else {
        *err = errh * epsh * m / (2 * efs);
    }
}



void intdeo(double (*f)(double), double a, double omega, double eps, 
    double *i, double *err)
{
    /* ---- adjustable parameter ---- */
    int mmax = 256, lmax = 5;
    double efs = 0.1, enoff = 0.40, pqoff = 2.9, ppoff = -0.72;
    /* ------------------------------ */
    int n, m, l, k;
    double pi4, epsln, epsh, frq4, per2, pp, pq, ehp, ehm, ir, h, iback, 
        irback, t, ep, em, tk, xw, wg, xa, fp, fm, errh, tn, errd;
    
    pi4 = atan(1.0);
    epsln = 1 - log(efs * eps);
    epsh = sqrt(efs * eps);
    n = (int) (enoff * epsln);
    frq4 = fabs(omega) / (2 * pi4);
    per2 = 4 * pi4 / fabs(omega);
    pq = pqoff / epsln;
    pp = ppoff - log(pq * pq * frq4);
    ehp = exp(2 * pq);
    ehm = 1 / ehp;
    xw = exp(pp - 2 * pi4);
    *i = (*f)(a + sqrt(xw * (per2 * 0.5)));
    ir = *i * xw;
    *i *= per2 * 0.5;
    *err = fabs(*i);
    h = 2;
    m = 1;
    do {
        iback = *i;
        irback = ir;
        t = h * 0.5;
        do {
            em = exp(2 * pq * t);
            ep = pi4 * em;
            em = pi4 / em;
            tk = t;
            do {
                xw = exp(pp - ep - em);
                wg = sqrt(frq4 * xw + tk * tk);
                xa = xw / (tk + wg);
                wg = (pq * xw * (ep - em) + xa) / wg;
                fm = (*f)(a + xa);
                fp = (*f)(a + xa + per2 * tk);
                ir += (fp + fm) * xw;
                fm *= wg;
                fp *= per2 - wg;
                *i += fp + fm;
                if (m == 1) *err += fabs(fp) + fabs(fm);
                ep *= ehp;
                em *= ehm;
                tk += 1;
            } while (ep < epsln);
            if (m == 1) {
                errh = *err * epsh;
                *err *= eps;
            }
            tn = tk;
            while (fabs(fm) > *err) {
                xw = exp(pp - ep - em);
                xa = xw / tk * 0.5;
                wg = xa * (1 / tk + 2 * pq * (ep - em));
                fm = (*f)(a + xa);
                ir += fm * xw;
                fm *= wg;
                *i += fm;
                ep *= ehp;
                em *= ehm;
                tk += 1;
            }
            fm = (*f)(a + per2 * tn);
            em = per2 * fm;
            *i += em;
            if (fabs(fp) > *err || fabs(em) > *err) {
                l = 0;
                for (;;) {
                    l++;
                    tn += n;
                    em = fm;
                    fm = (*f)(a + per2 * tn);
                    xa = fm;
                    ep = fm;
                    em += fm;
                    xw = 1;
                    wg = 1;
                    for (k = 1; k <= n - 1; k++) {
                        xw = xw * (n + 1 - k) / k;
                        wg += xw;
                        fp = (*f)(a + per2 * (tn - k));
                        xa += fp;
                        ep += fp * wg;
                        em += fp * xw;
                    }
                    wg = per2 * n / (wg * n + xw);
                    em = wg * fabs(em);
                    if (em <= *err || l >= lmax) break;
                    *i += per2 * xa;
                }
                *i += wg * ep;
                if (em > *err) *err = em;
            }
            t += h;
        } while (t < 1);
        if (m == 1) {
            errd = 1 + 2 * errh;
        } else {
            errd = h * (fabs(*i - 2 * iback) + pq * fabs(ir - 2 * irback));
        }
        h *= 0.5;
        m *= 2;
    } while (errd > errh && m < mmax);
    *i *= h;
    if (errd > errh) {
        *err = -errd;
    } else {
        *err *= m * 0.5;
    }
}

