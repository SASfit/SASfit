/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joahim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sd.h>
#include <float.h>

scalar mk(int k, scalar y, scalar *a, size_t n, sasfit_param * param) {
    SASFIT_CHECK_COND2((n < k), param, "n(%d) < k(%d)",n,k);
    SASFIT_CHECK_COND1((k < 2), param, "k(%d) < 2(%d)",k);
    scalar logity,y1my;
    if (y<=0) {
        return DBL_MAX;
        logity = -DBL_MAX;
        y1my = DBL_MIN;
    } else if (y>=1){
        return DBL_MAX;
        logity = DBL_MAX;
        y1my = DBL_MIN;
    } else {
        logity = log(y/(1-y));
        y1my = (y*(1-y));
    }
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
    if (y<=0) {
        logity = -DBL_MAX;
    } else if (y>=1){
        logity = DBL_MAX;
    } else {
        logity = log(y/(1-y));
    }
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
    if (y<=0) return bl;
    expM = exp(Mk(k,y,a,n,param));
    if (gsl_finite(expM)) {
        return bl+expM;
    } else {
        return DBL_MAX;
    }

}

scalar MNLog(int k, scalar y, scalar *a, size_t n, scalar bu, sasfit_param * param) {
    scalar expmM;
    if (y==1) return bu;
    expmM = exp(-Mk(k,y,a,n,param));
    if (gsl_finite(expmM)) {
        return bu-expmM;
    } else {
        return -DBL_MAX;
    }
}

scalar mLogit(int k, scalar y, scalar *a, size_t n, scalar bl, scalar bu, sasfit_param * param) {
    scalar expM;
    if (y<=0) return bl;
    if (y>=1) return bu;
    expM = exp(Mk(k,y,a,n,param));
    expM = mk(k,y,a,n,param)*gsl_pow_2(1+expM)/((bu-bl)*expM);
    if (gsl_finite(expM)) {
        return expM;
    } else {
        return DBL_MAX;
    }

}

scalar mLog(int k, scalar y, scalar *a, size_t n, sasfit_param * param) {
    scalar expmM;
    if (y<=0) return 0;
    expmM = exp(-Mk(k,y,a,n,param));
    expmM = mk(k,y,a,n,param)*expmM;
    if (gsl_finite(expmM)) {
        return expmM;
    } else {
        return DBL_MAX;
    }
}

scalar mNLog(int k, scalar y, scalar *a, size_t n, sasfit_param * param) {
    scalar expM, yt;
    if (y>=1) return DBL_MAX;
    expM = exp(Mk(k,y,a,n,param));
    expM = mk(k,y,a,n,param)*expM;
    if (gsl_finite(expM)) {
        return expM;
    } else {
        return DBL_MAX;
    }

}
