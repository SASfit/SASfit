/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joahim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_sd.h>
#include <gsl/gsl_roots.h>

#define MAXROOTITER 5000


scalar ylin (scalar y, void *pam) {
    return y;
}

scalar dylin (scalar y, void *pam) {
    return 1.0;
}

scalar yatan (scalar y, void *pam) {
    return (sin(y)+1.0)/2.;
    return (atan(y)+1.0)/2.;
}

scalar dyatan (scalar y, void *pam) {
    return cos(y)/2.;
    return 1./(2.*(1.+y*y));
}

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

scalar MLogit(int k, scalar y, scalar *a, size_t n, sasfit_param * param) {
    scalar bu,bl,expM,yt;
    metalog_param *mpara;
    mpara = (metalog_param *) param->moreparam;
    bu = mpara->bu;
    bl = mpara->bl;
//    yt = (mpara->ytrans)(y,param);
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

scalar MLog(int k, scalar y, scalar *a, size_t n, sasfit_param * param) {
    scalar bu,bl,expM,yt;
    metalog_param *mpara;
    mpara = (metalog_param *) param->moreparam;
    bu = mpara->bu;
    bl = mpara->bl;
//    yt = (mpara->ytrans)(y,param);
    if (y==0) return bl;
    expM = exp(Mk(k,y,a,n,param));
    return bl+expM;

}

scalar MNLog(int k, scalar y, scalar *a, size_t n, sasfit_param * param) {
    scalar bu,bl,expmM,yt;
    metalog_param *mpara;
    mpara = (metalog_param *) param->moreparam;
    bu = mpara->bu;
    bl = mpara->bl;
//    yt = (mpara->ytrans)(y,param);
//    if (yt==1) return bu;
    if (y==1) return bu;
    expmM = exp(-Mk(k,y,a,n,param));
    return bu-expmM;

}

scalar mLogit(int k, scalar y, scalar *a, size_t n, sasfit_param * param) {
    scalar bu,bl, expM, yt;
    metalog_param *mpara;
    mpara = (metalog_param *) param->moreparam;
    bu = mpara->bu;
    bl = mpara->bl;
//    yt = (mpara->ytrans)(y,param);
//    if (yt==0 || yt==1) return 0;
    if (y==0 || y==1) return 0;
    expM = exp(Mk(k,y,a,n,param));
    if (gsl_finite(expM)) {
        return mk(k,y,a,n,param)*gsl_pow_2(1+expM)/((bu-bl)*expM);
    } else {
        return 0;
    }

}

scalar mLog(int k, scalar y, scalar *a, size_t n, sasfit_param * param) {
    scalar bu,bl, expmM, yt;
    metalog_param *mpara;
    mpara = (metalog_param *) param->moreparam;
    bu = mpara->bu;
    bl = mpara->bl;
//    yt = (mpara->ytrans)(y,param);
//    if (yt==0) return 0;
    if (y==0) return 0;
    expmM = exp(-Mk(k,y,a,n,param));
    return mk(k,y,a,n,param)*expmM;
}

scalar mNLog(int k, scalar y, scalar *a, size_t n, sasfit_param * param) {
    scalar bu,bl, expM, yt;
    metalog_param *mpara;
    mpara = (metalog_param *) param->moreparam;
    bu = mpara->bu;
    bl = mpara->bl;
//    yt = (mpara->ytrans)(y,param);
//    if (yt==1) return 0;
    if (y==1) return 0;
    expM = exp(Mk(k,y,a,n,param));
    if (gsl_finite(expM)) {
        return mk(k,y,a,n,param)*expM;
    } else {
        return 0;
    }

}
double
root_metalog_f (double y, void *rootp)
{
  sasfit_param * param;
  metalog_param *mpara;
  scalar yt;
  param = (sasfit_param *) rootp ;
  mpara = (metalog_param *) param->moreparam;
  yt = (mpara->ytrans)(y,param);
  return Mk(mpara->n,yt,mpara->a,mpara->n,param)-mpara->x;
}

double
root_metalog_df (double y, void *rootp)
{
  sasfit_param * param;
  metalog_param *mpara;
  scalar yt;
  param = (sasfit_param *) rootp ;
  mpara = (metalog_param *) param->moreparam;
  yt = (mpara->ytrans)(y,param);

  return (mpara->dytrans)(y,param)/mk(mpara->n,yt,mpara->a,mpara->n,param);
}

void
root_metalog_fdf (double y, void *rootp,
               double *F, double *dF)
{
  sasfit_param * param;
  metalog_param *mpara;
  scalar yt;
  param = (sasfit_param *) rootp ;
  mpara = (metalog_param *) param->moreparam;
  yt = (mpara->ytrans)(y,param);
  *F = Mk(mpara->n,yt,mpara->a,mpara->n,param)-mpara->x;
  *dF = (mpara->dytrans)(y,param)/mk(mpara->n,yt,mpara->a,mpara->n,param);
}

double
root_metalog_Logit_f (double y, void *rootp)
{
  sasfit_param * param;
  metalog_param *mpara;
  scalar yt;
  param = (sasfit_param *) rootp ;
  mpara = (metalog_param *) param->moreparam;
  yt = (mpara->ytrans)(y,param);
  return MLogit(mpara->n,yt,mpara->a,mpara->n,param)-mpara->x;
}

double
root_metalog_Logit_df (double y, void *rootp)
{
  sasfit_param * param;
  metalog_param *mpara;
  scalar yt;
  param = (sasfit_param *) rootp ;
  mpara = (metalog_param *) param->moreparam;
  yt = (mpara->ytrans)(y,param);

  return (mpara->dytrans)(y,param)/mLogit(mpara->n,yt,mpara->a,mpara->n,param);
}

void
root_metalog_Logit_fdf (double y, void *rootp,
               double *F, double *dF)
{
  sasfit_param * param;
  metalog_param *mpara;
  scalar yt;
  param = (sasfit_param *) rootp ;
  mpara = (metalog_param *) param->moreparam;
  yt = (mpara->ytrans)(y,param);

  *F = MLogit(mpara->n,yt,mpara->a,mpara->n,param)-mpara->x;
  *dF =  (mpara->dytrans)(y,param)/mLogit(mpara->n,yt,mpara->a,mpara->n,param);
}

double
root_metalog_Log_f (double y, void *rootp)
{
  sasfit_param * param;
  metalog_param *mpara;
  scalar yt;
  param = (sasfit_param *) rootp ;
  mpara = (metalog_param *) param->moreparam;
  yt = (mpara->ytrans)(y,param);
  return MLog(mpara->n,yt,mpara->a,mpara->n,param)-mpara->x;
}

double
root_metalog_Log_df (double y, void *rootp)
{
  sasfit_param * param;
  metalog_param *mpara;
  scalar yt;
  param = (sasfit_param *) rootp ;
  mpara = (metalog_param *) param->moreparam;
  yt = (mpara->ytrans)(y,param);

  return (mpara->dytrans)(y,param)/mLog(mpara->n,yt,mpara->a,mpara->n,param);
}

void
root_metalog_Log_fdf (double y,  void *rootp,
                      double *F, double *dF  )
{
  sasfit_param * param;
  metalog_param *mpara;
  scalar yt;
  param = (sasfit_param *) rootp ;
  mpara = (metalog_param *) param->moreparam;
  yt = (mpara->ytrans)(y,param);

  *F  =   MLog(mpara->n,yt,mpara->a,mpara->n,param)-mpara->x;
  *dF =  (mpara->dytrans)(y,param)/mLog(mpara->n,yt,mpara->a,mpara->n,param);
}


double
root_metalog_NLog_f (double y, void *rootp)
{
  sasfit_param * param;
  metalog_param *mpara;
  scalar yt;
  param = (sasfit_param *) rootp ;
  mpara = (metalog_param *) param->moreparam;
  yt = (mpara->ytrans)(y,param);
  return MNLog(mpara->n,yt,mpara->a,mpara->n,param)-mpara->x;
}

double
root_metalog_NLog_df (double y, void *rootp)
{
  sasfit_param * param;
  metalog_param *mpara;
  scalar yt;
  param = (sasfit_param *) rootp ;
  mpara = (metalog_param *) param->moreparam;
  yt = (mpara->ytrans)(y,param);

  return (mpara->dytrans)(y,param)/mNLog(mpara->n,yt,mpara->a,mpara->n,param);
}

void
root_metalog_NLog_fdf (double y, void *rootp,
               double *F, double *dF)
{
  sasfit_param * param;
  metalog_param *mpara;
  scalar yt;
  param = (sasfit_param *) rootp ;
  mpara = (metalog_param *) param->moreparam;
  yt = (mpara->ytrans)(y,param);

  *F = MNLog(mpara->n,yt,mpara->a,mpara->n,param)-mpara->x;
  *dF =  (mpara->dytrans)(y,param)/mNLog(mpara->n,yt,mpara->a,mpara->n,param);
}


void assign_metalog_par(scalar x, metalog_param *mp, sasfit_param *param)
{
    mp->x = x;
    mp->n = 10;
    mp->a[0] = A1;
    mp->a[1] = A2;
    mp->a[2] = A3;
    mp->a[3] = A4;
    mp->a[4] = A5;
    mp->a[5] = A6;
    mp->a[6] = A7;
    mp->a[7] = A8;
    mp->a[8] = A9;
    mp->a[9] = A10;
    mp->bl=BL;
    mp->bu=BU;
}

scalar find_root_fdf_metalog(gsl_function_fdf *FDF) {
    const gsl_root_fdfsolver_type *T;
    gsl_root_fdfsolver *s;
    int status;
    size_t i, iter;
    scalar y0, y_lo, y_hi;
    static scalar yroot=0.5;
    sasfit_param * param;
    metalog_param *mpara;
    param = (sasfit_param *) FDF->params;
    mpara = (metalog_param *) param->moreparam;
    mpara->ytrans=&yatan;
	mpara->dytrans=&dyatan;
	yroot = gsl_max(yroot,1e-2);
	yroot = gsl_min(yroot,1-1e-2);
    gsl_set_error_handler_off ();
    switch (sasfit_get_rootalg()) {
        case ROOTALG_SECANT:     T = gsl_root_fdfsolver_secant;break;
        case ROOTALG_NEWTON:     T = gsl_root_fdfsolver_newton;break;
        case ROOTALG_STEFFENSON:
        default:                 T = gsl_root_fdfsolver_steffenson;
    }
    s = gsl_root_fdfsolver_alloc (T);
    gsl_root_fdfsolver_set (s, FDF, yroot);
    iter=0;
    do
    {
        iter++;
        status = gsl_root_fdfsolver_iterate (s);
        y0 = yroot;
        yroot = gsl_root_fdfsolver_root (s);
        status = gsl_root_test_delta (yroot, y0, 0, sasfit_eps_get_aniso());

    }
    while (status == GSL_CONTINUE && iter < MAXROOTITER);
    if (iter >= MAXROOTITER) {
        sasfit_err("could not find root within MAXROOTITER=%d iterations\n",MAXROOTITER);
    }
    gsl_root_fdfsolver_free (s);
    return (mpara->ytrans)(yroot,param);
}

scalar find_root_f_metalog(gsl_function *F) {
    const gsl_root_fsolver_type *T;
    gsl_root_fsolver *s;
    int status;
    size_t i, iter;
    scalar y0, y_lo, y_hi;
    static scalar yroot=0.5;
	yroot = gsl_max(yroot,1e-2);
	yroot = gsl_min(yroot,1-1e-2);
    gsl_set_error_handler_off ();
    sasfit_param * param;
    metalog_param *mpara;
    param = (sasfit_param *) F->params;
    mpara = (metalog_param *) param->moreparam;
    mpara->ytrans=&ylin;
	mpara->dytrans=&dylin;
    switch (sasfit_get_rootalg()) {
        case ROOTALG_BRENT:             T = gsl_root_fsolver_brent;break;
        case ROOTALG_FALSE_POSITION:    T = gsl_root_fsolver_falsepos;break;
        case ROOTALG_BISECTION:
        default:                        T = gsl_root_fsolver_bisection;
    }
    s = gsl_root_fsolver_alloc (T);
    gsl_root_fsolver_set (s, F, 2*gsl_max(DBL_EPSILON,sasfit_eps_get_res()), 1-2*gsl_max(DBL_EPSILON,sasfit_eps_get_res()));
    iter = 0;
    do
    {
        iter++;
        status = gsl_root_fsolver_iterate (s);
        yroot = gsl_root_fsolver_root (s);
        y_lo = gsl_root_fsolver_x_lower (s);
        y_hi = gsl_root_fsolver_x_upper (s);
        status = gsl_root_test_interval (y_lo, y_hi,
                                       0, sasfit_eps_get_aniso());
    }
    while (status == GSL_CONTINUE && iter < MAXROOTITER);
    if (iter >= MAXROOTITER) {
        sasfit_err("could not find root within MAXROOTITER=%d iterations\n",MAXROOTITER);
    }
    gsl_root_fsolver_free (s);
    return yroot;
}

scalar metalogPDF(scalar x, sasfit_param *param) {
    metalog_param mp;
    gsl_function F;
    gsl_function_fdf FDF;
    scalar y;
    int how;
    param->moreparam=&mp;

    assign_metalog_par(x, &mp,param);

//    if (x<=BL || x>=BU) return 0;
    F.function = &root_metalog_f;
    F.params=param;
    FDF.f   = &root_metalog_f;
    FDF.df  = &root_metalog_df;
    FDF.fdf = &root_metalog_fdf;
    FDF.params=param;

    if (sasfit_eps_get_robertus_p()<=3) {
        y = find_root_f_metalog(&F);
    } else {
        y = find_root_fdf_metalog(&FDF);
    }
    return mk(mp.n,y,mp.a,mp.n,param);
}


scalar metalogLogitPDF(scalar x, sasfit_param *param) {
    metalog_param mp;
    gsl_function F;
    scalar y;
    param->moreparam=&mp;
    assign_metalog_par(x, &mp,param);

    if (x<=BL || x>=BU) return 0;
 //   return metalogPDF(log(x-BL)-log(BU-x),param);

    F.function = &root_metalog_Logit_f;
    F.params=param;
    y = find_root_f_metalog(&F);
    return mLogit(mp.n,y,mp.a,mp.n,param);
}

scalar metalogLogPDF(scalar x, sasfit_param *param) {
    metalog_param mp;
    gsl_function F;
    scalar y;
    assign_metalog_par(x, &mp,param);
    param->moreparam=&mp;

    if (x<=BL || x>=BU) return 0;
    F.function = &root_metalog_Log_f;
    F.params=param;
    y = find_root_f_metalog(&F);
    return mLog(mp.n,y,mp.a,mp.n,param);
}

scalar metalogNLogPDF(scalar x, sasfit_param *param) {
    metalog_param mp;
    gsl_function F;
    scalar y;
    assign_metalog_par(x, &mp,param);
    param->moreparam=&mp;

    if (x<=BL || x>=BU) return 0;
    F.function = &root_metalog_NLog_f;
    F.params=param;
    y = find_root_f_metalog(&F);
    return mNLog(mp.n,y,mp.a,mp.n,param);
}
