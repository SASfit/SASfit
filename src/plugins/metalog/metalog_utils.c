/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joahim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sd.h>

scalar mk(int k, scalar y, scalar *a, size_t n, sasfit_param * param) {
    SASFIT_CHECK_COND2((n < k), param, "n(%d) < k(%d)",n,k);
    SASFIT_CHECK_COND1((k < 2), param, "k(%d) < 2(%d)",k);
    scalar logity,y1my;
    logity = log(y/(1-y));
    y1my = (y*(1-y));
    switch (k) {
    case 2: return (y*(1-y))/a[1];
            break;
    case 3: return 1./(a[1]/y1my+a[2]*((y-0.5)/y1my+logity));
            break;
    case 4: return 1./(a[1]/y1my+a[2]*((y-0.5)/y1my+logity)+a[3]);
            break;
    default:if (GSL_IS_ODD(k)) {
                return 1./(1./mk(k-1,y,a,n,param)+a[k-1]*(k-1)/2.*gsl_pow_int(y-0.5,(k-3)/2));
            } else {
                return 1./(1./mk(k-1,y,a,n,param)+a[k-1]*(gsl_pow_int(y-0.5,k/2-1)/y1my+(k/2-1)*gsl_pow_int(y-0.5,k/2-2)*logity));
            }
            break;
    }
}

scalar Mk(int k, scalar y, scalar *a, size_t n, sasfit_param * param) {
    SASFIT_CHECK_COND2((n < k), param, "n(%d) < k(%d)",n,k);
    SASFIT_CHECK_COND1((k < 2), param, "k(%d) < 2(%d)",k);
    scalar logity;
    logity = log(y/(1-y));
    switch (k) {
    case 2: return a[0]+a[1]*logity;
            break;
    case 3: return a[0]+a[1]*logity+a[2]*(y-0.5)*logity;
            break;
    case 4: return a[0]+a[1]*logity+a[2]*(y-0.5)*logity+a[3]*(y-0.5);
            break;
    default:if (GSL_IS_ODD(k)) {
                return Mk(k-1,y,a,n,param)+a[k-1]*gsl_pow_int(y-0.5,(k-1)/2);
            } else {
                return Mk(k-1,y,a,n,param)+a[k-1]*gsl_pow_int(y-0.5,k/2-1)*logity;
            }
            break;
    }
}

scalar MLogit(int k, scalar y, scalar *a, size_t n,scalar bl, scalar bu, sasfit_param * param) {
    scalar expM,yt;
    if (y<=0) return bl;
    if (y>=1) return bu;
    expM = exp(Mk(k,y,a,n,param));
    if (gsl_finite(expM)) {
        return (bl+bu*expM)/(1+expM);
        return bu+(bl-bu)/(1+expM);
    } else {
        return bu;
    }
}

scalar MLog(int k, scalar y, scalar *a, size_t n, scalar bl, sasfit_param * param) {
    scalar expM,yt;
    if (y==0) return bl;
    expM = exp(Mk(k,y,a,n,param));
    return bl+expM;

}

scalar MNLog(int k, scalar y, scalar *a, size_t n, scalar bu, sasfit_param * param) {
    scalar expmM,yt;
    if (y==1) return bu;
    expmM = exp(-Mk(k,y,a,n,param));
    return bu-expmM;

}

scalar mLogit(int k, scalar y, scalar *a, size_t n, scalar bl, scalar bu, sasfit_param * param) {
    scalar expM, yt;
    if (y<=0) return bl;
    if (y>=1) return bu;
    expM = exp(Mk(k,y,a,n,param));
    if (gsl_finite(expM)) {
        return mk(k,y,a,n,param)*gsl_pow_2(1+expM)/((bu-bl)*expM);
    } else {
        return 0;
    }

}

scalar mLog(int k, scalar y, scalar *a, size_t n, sasfit_param * param) {
    scalar expmM, yt;
    if (y<=0) return 0;
    expmM = exp(-Mk(k,y,a,n,param));
    return mk(k,y,a,n,param)*expmM;
}

scalar mNLog(int k, scalar y, scalar *a, size_t n, sasfit_param * param) {
    scalar expM, yt;
    if (y>=1) return 0;
    expM = exp(Mk(k,y,a,n,param));
    if (gsl_finite(expM)) {
        return mk(k,y,a,n,param)*expM;
    } else {
        return 0;
    }

}
