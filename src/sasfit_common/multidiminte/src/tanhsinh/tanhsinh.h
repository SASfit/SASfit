#ifndef G_PQ4ZS0ORCGDV0GJ9P4XN178ZS9XXS
#define G_PQ4ZS0ORCGDV0GJ9P4XN178ZS9XXS
/** @file
    Numerical integration in one dimension using the tanh-sinh quadrature.

    This quadrature method provides good results for a variety of functions
    and is about as general purpose as feasible.  It is also fairly robust at
    handling singularities at the endpoints of the integral.

    The method is described in the [paper by Masatake Mori and Masaaki
    Sugihara][1] and is sometimes referred to as the double-exponential method.

    The code was originally written by [John D. Cook][2], who has dedicated
    the code to the [public domain][3].

    [1]: http://dx.doi.org/10.1016/S0377-0427%2800%2900501-X
    [2]: http://codeproject.com/Articles/31550/Fast-Numerical-Integration
    [3]: https://creativecommons.org/publicdomain/zero/1.0
*/
#ifdef __cplusplus
extern "C" {
#endif

/** Integrate an analytic real function over a finite interval.

    @param[in]     f        integrand
    @param[in,out] ctx      arbitrary argument for `f`
    @param[in]     a        left limit of integration
    @param[in]     b        right limit of integration
    @param[in]     abs_err  desired bound on error
    @param[out]    est_err  estimated error of integral (can be `NULL`)
    @param[out]    num_eval number of function evaluations used (can be `NULL`)
    @return                 The value of the integral.
*/
double tanhsinh_quad(double (*f)(double, void *), void *,
                     double, double, double,
                     double *, unsigned *);
double qthsh(double (*f)(double, void *), void *, double, double, int, double, double *);
#ifdef __cplusplus
}
#endif
#endif
