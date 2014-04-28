/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include <stdio.h>
#include <stdlib.h>
#include <sasfit_oz.h>
#include <sasfit_gmres.h>
#include <bool.h>

scalar parab3p(double lambdac, double lambdam, double ff0, double ffc, double ffm) {
//% Apply three-point safeguarded parabolic model for a line search.
//%
//% C. T. Kelley, April 1, 2003
//%
//% This code comes with no guarantee or warranty of any kind.
//%
//% function lambdap = parab3p(lambdac, lambdam, ff0, ffc, ffm)
//%
//% input:
//%       lambdac = current steplength
//%       lambdam = previous steplength
//%       ff0 = value of \| F(x_c) \|^2
//%       ffc = value of \| F(x_c + \lambdac d) \|^2
//%       ffm = value of \| F(x_c + \lambdam d) \|^2
//%
//% output:
//%       lambdap = new value of lambda given parabolic model
//%
//% internal parameters:
//%       sigma0 = .1, sigma1 = .5, safeguarding bounds for the linesearch
//%
	double sigma0 = 0.1;
	double sigma1 = 0.5;
	double c1,c2,lambdap;
	c2 = lambdam*(ffc-ff0) - lambdac*(ffm-ff0);
	if (c2 >= 0) {
        lambdap = sigma1*lambdac;
        return lambdap;
	}
	c1 = gsl_pow_2(lambdac)*(ffm-ff0)
	    -gsl_pow_2(lambdam)-(ffc-ff0);
    lambdap = -0.5*c1/c2;
    if (lambdap < sigma0*lambdac) lambdap=sigma0*lambdac;
    if (lambdap > sigma1*lambdac) lambdap=sigma1*lambdac;
	return lambdap;
}


void givapp(scalar *c, scalar *s, scalar *vin, int k, int dim, scalar *vrot) {
// dim: length of array c,s,vin
// dim+1: length of array
}

#define MAXIT  parms->maxit
#define LMAXIT parms->lmaxit
#define LMETH  parms->lmeth
#define MAXIT  parms->maxit
#define ETAMAX parms->etamax
#define RESTARTLIMIT parms->restart_limit
#define N parms->n

scalar norm(scalar *f0, int n) {
    int i;
    scalar res;
    res = 0;
    for (i=0;i<n;i++) {
        res = res + gsl_pow_2(f0[i]);
    }
    return sqrt(res);
}

void cparr(scalar *xfrom, scalar *xto, int n) {
    int i;
    for (i=0;i<n;i++) {
        xto[i] = xfrom[i];
    }
}

void feval(void *ozfunct, scalar *x, scalar *xnew, int n) {
    cparr(x,xnew,n);
}

void gmres(scalar *f0,
           void *ozfunct,
           scalar *x,
           sasfit_oz_data *OZd,
           scalar *gmparms,
           scalar *step,
           scalar *errstep,
           int *total_iters) {
//% GMRES linear equation solver for use in Newton-GMRES solver
//%
//% C. T. Kelley, April 1, 2003
//%
//% This code comes with no guarantee or warranty of any kind.
//%
//% function [x, error, total_iters] = dgmres(f0, f, xc, params, xinit)
//%
//%
//% Input:  f0 = function at current point
//%         f = nonlinear function
//%              the format for f is  function fx = f(x)
//%              Note that for Newton-GMRES we incorporate any
//%              preconditioning into the function routine.
//%         xc = current point
//%         params = two dimensional vector to control iteration
//%              params(1) = relative residual reduction factor
//%              params(2) = max number of iterations
//%            params(3) (Optional) = reorthogonalization method
//%                   1 -- Brown/Hindmarsh condition (default)
//%                   2 -- Never reorthogonalize (not recommended)
//%                   3 -- Always reorthogonalize (not cheap!)
//%
//%         xinit = initial iterate. xinit = 0 is the default. This
//%              is a reasonable choice unless restarted GMRES
//%              will be used as the linear solver.
//%
//% Output: x = solution
//%         error = vector of residual norms for the history of
//%                 the iteration
//%         total_iters = number of iterations
//%
//% Requires givapp.m, dirder.m

//%
//% initialization
//%

}

void dkrylov(scalar *f0,
             void *ozfunct,
             scalar *x,
             sasfit_oz_data *OZd,
             scalar *params,
             int lmeth,
             scalar *step,
             scalar *errstep,
             int *total_iters,
             int *f_evals) {
//% Krylov linear equation solver for use in nsoli
//%
//% C. T. Kelley, April 1, 2003
//%
//%
//% This code comes with no guarantee or warranty of any kind.
//%
//% function [step, errstep, total_iters, f_evals]
//%                              = dkrylov(f0, f, x, params, lmeth)
//%
//%
//% Input:  f0 = function at current point
//%         f = nonlinear function
//%              the format for f is  function fx = f(x)
//%              Note that for Newton-GMRES we incorporate any
//%              preconditioning into the function routine.
//%         x = current point
//%         params = vector to control iteration
//%              params(1) = relative residual reduction factor
//%              params(2) = max number of iterations
//%              params(3) = max number of restarts for GMRES(m)
//%              params(4) (Optional) = reorthogonalization method in GMRES
//%                   1 -- Brown/Hindmarsh condition (default)
//%                   2 -- Never reorthogonalize (not recommended)
//%                   3 -- Always reorthogonalize (not cheap!)
//%
//%         lmeth = method choice
//%              1 GMRES without restarts (default)
//%              2 GMRES(m), m = params(2) and the maximum number
//%                   of restarts is params(3)
//%              3 Bi-CGSTAB
//%              4 TFQMR
//%
//% Output: x = solution
//%         errstep = vector of residual norms for the history of
//%                 the iteration
//%         total_iters = number of iterations
//%
//%

    int lmaxit, restart_limit=20.0;
    int kinn;
    double gmparms[3];

    lmaxit = params[1];
    restart_limit=params[2];
    if(lmeth ==1) restart_limit=0;
    gmparms[0] = params[0];
    gmparms[1] = params[1];
    gmparms[2] = params[3];
//%
//% linear iterative methods
//%
    switch (lmeth) {
        case 1:
        case 2:
            // if lmeth == 1 || lmeth == 2  % GMRES or GMRES(m)
//%
//% compute the step using a GMRES routine especially designed
//% for this purpose
//%
//            [step, errstep, total_iters] = dgmres(f0, f, x, gmparms);
            kinn = 0;
//%
//%   restart at most restart_limit times
//%
            while (    (*total_iters == lmaxit)
                    && (errstep[*total_iters] > gmparms[0]*norm(f0,OZd->Npoints))
                    && (kinn < restart_limit) ) {
                kinn = kinn+1;
//                [step, errstep, total_iters] = dgmres(f0, f, x, gmparms,step);
            }
            *total_iters = *total_iters+kinn*lmaxit;
            *f_evals = *total_iters+kinn;
            break;
//%
//% Bi-CGSTAB
//%
        case 3:
//            [step, errstep, total_iters] = dcgstab(f0, f, x, gmparms);
//            f_evals = 2*total_iters;
            break;
//%
//% TFQMR
//%
        case 4:
//            [step, errstep, total_iters] = dtfqmr(f0, f, x, gmparms);
//            f_evals = 2*total_iters;
            break;
    default :
            sasfit_err("lmeth error in fdkrylov\n");
    }
}

void nsoli(scalar *x,
           void *ozfunct,
           sasfit_oz_data *OZd,
           scalar *tol,
           nsolip *parms,
           scalar *sol,
           int *ierr) {
//% NSOLI  Newton-Krylov solver, globally convergent
//%        solver for f(x) = 0//
//%
//% Inexact-Newton-Armijo iteration
//%
//% Eisenstat-Walker forcing term
//%
//% Parabolic line search via three point interpolation.
//%
//% C. T. Kelley, April 27, 2001
//%
//% This code comes with no guarantee or warranty of any kind.
//%
//% function [sol, it_hist, ierr, x_hist] = nsoli(x,f,tol,parms)
//%
//% inputs:
//%        initial iterate = x
//%        function = f
//%        tol = [atol, rtol] relative/absolute
//%            error tolerances for the nonlinear iteration
//%        parms = [maxit, maxitl, etamax, lmeth, restart_limit]
//%            maxit = maxmium number of nonlinear iterations
//%                default = 40
//%            maxitl = maximum number of inner iterations before restart
//%                in GMRES(m), m = maxitl
//%                default = 40
//%
//%                For iterative methods other than GMRES(m) maxitl
//%                is the upper bound on linear iterations.
//%
//%            |etamax| = Maximum error tolerance for residual in inner
//%                iteration. The inner iteration terminates
//%                when the relative linear residual is
//%                smaller than eta*| F(x_c) |. eta is determined
//%                by the modified Eisenstat-Walker formula if etamax > 0.
//%                If etamax < 0, then eta = |etamax| for the entire
//%                iteration.
//%                default: etamax = .9
//%
//%            lmeth = choice of linear iterative method
//%                    1 (GMRES), 2 GMRES(m),
//%                    3 (BICGSTAB), 4 (TFQMR)
//%                 default = 1 (GMRES, no restarts)
//%
//%            restart_limit = max number of restarts for GMRES if
//%                    lmeth = 2
//%                  default = 20
//%
//% output:
//%        sol = solution
//%        it_hist(maxit,3) = l2 norms of nonlinear residuals
//%            for the iteration, number of function evaluations,
//%            and number of steplength reductions
//%        ierr = 0 upon successful termination
//%        ierr = 1 if after maxit iterations
//%             the termination criterion is not satsified
//%        ierr = 2 failure in the line search. The iteration
//%             is terminated if too many steplength reductions
//%             are taken.
//%
//%    x_hist = matrix of the entire interation history.
//%             The columns are the nonlinear iterates. This
//%             is useful for making movies, for example, but
//%             can consume way too much storage. This is an
//%             OPTIONAL argument. Storage is only allocated
//%             if x_hist is in the output argument list.
//%
//%
//%
//% internal parameters:
//%       debug = turns on/off iteration statistics display as
//%               the iteration progresses
//%
//%       alpha = 1.d-4, parameter to measure sufficient decrease
//%
//%       sigma0 = .1, sigma1 = .5, safeguarding bounds for the linesearch
//%
//%       maxarm = 20, maximum number of steplength reductions before
//%                    failure is reported
//%
//%
    scalar alpha  = 1.0e-4;
    scalar sigma0 = 0.1;
    scalar sigma1 = 0.5;
    int maxarm    = 20, inner_it_count = 0, inner_f_evals = 0;
    scalar gamma  = 0.9;
    scalar gmparms[4];
    scalar rtol, atol, *f0, *xt, *ft, *xold, *step, *errstep;
    scalar  nft, nf0, ff0, ffc, ffm, fnrm, fnrmo,rat, stop_tol;
    int itc,lmeth,i,j;
    scalar **it_histx;
    scalar lambda, lamm, lamc, etanew, etaold;
    int iarm;

    lmeth = LMETH;
    gmparms[0]=fabs(ETAMAX);
    gmparms[1]=LMAXIT;
    gmparms[2]=RESTARTLIMIT;
    gmparms[3]=1;
    fnrm = 1.0;

    it_histx = (scalar **) malloc (MAXIT*sizeof(scalar));
    for (i=0;i<MAXIT;i++) {
        it_histx[i] = (scalar *) malloc(3 * sizeof(scalar));
    }
    xold  = (scalar *)malloc((OZd->Npoints)*sizeof(scalar));
    xt = (scalar *)malloc((OZd->Npoints)*sizeof(scalar));
    ft = (scalar *)malloc((OZd->Npoints)*sizeof(scalar));
    f0 = (scalar *)malloc((OZd->Npoints)*sizeof(scalar));
    step = (scalar *)malloc((OZd->Npoints)*sizeof(scalar));
    errstep = (scalar *)malloc((OZd->Npoints)*sizeof(scalar));

    itc = 0;
    feval(ozfunct,x,f0,OZd->Npoints);
//    f0 = OZd->G;
    fnrm = norm(f0,OZd->Npoints);
    it_histx[itc][0] = fnrm;
    it_histx[itc][1] = 0;
    it_histx[itc][2] = 0;
    stop_tol = atol + rtol*fnrm;
    while(fnrm > stop_tol && itc < MAXIT) {
        rat = fnrm/fnrmo;
        fnrmo = fnrm;
        itc = itc+1;
////   [step, errstep, inner_it_count,inner_f_evals] = ...
////        dkrylov(f0, f, x, gmparms, lmeth);
        dkrylov(f0, ozfunct, x, OZd, gmparms, lmeth, step, errstep, &inner_it_count, &inner_f_evals);
//
//   The line search starts here.
//
        cparr(x,xold,OZd->Npoints);
        lambda = 1;
        lamm = 1;
        lamc = lambda;
        iarm = 0;
        for (i=0;i<OZd->Npoints;i++) {
            ////    xt = x + lambda*step;
            xt[i] = x[i]+lambda*step[i];
        }
        feval(ozfunct,xt,ft,OZd->Npoints);

        nft = norm(ft,OZd->Npoints);
        nf0 = norm(f0,OZd->Npoints);
        ff0 = nf0*nf0;
        ffc = nft*nft;
        ffm = nft*nft;

        while (nft >= (1 - alpha*lambda) * nf0) {
//
//   Apply the three point parabolic model.
//
        if (iarm == 0) {
            lambda = sigma1*lambda;
        } else {
            lambda = parab3p(lamc, lamm, ff0, ffc, ffm);
        }
//
// Update x; keep the books on lambda.
//
        for (i=0;i<OZd->Npoints;i++) {
            ////    xt = x + lambda*step;
            xt[i] = x[i]+lambda*step[i];
        }
        lamm = lamc;
        lamc = lambda;
//
// Keep the books on the function norms.
//
        feval(ozfunct,xt,ft,OZd->Npoints);
        nft = norm(ft,OZd->Npoints);
        ffm = ffc;
        ffc = nft*nft;
        iarm = iarm+1;

        if (iarm > maxarm) {
            fprintf(stderr,"Armijo failure, too many reductions ");
            *ierr = 2;
//            disp(outstat)
////            it_hist = it_histx(1:itc+1,:);
//        if nargout == 4, x_hist = [x_hist,x]; end
            cparr(xold,sol,OZd->Npoints);
            return;
        }

        }
        cparr(xt,x,OZd->Npoints);
        f0 = ft;
//
//   End of line search.
//
//    if nargout == 4, x_hist = [x_hist,x]; end
        fnrm = norm(f0,OZd->Npoints);
        it_histx[itc+1][0] = fnrm;
//
//   How many function evaluations did this iteration require?
//
        it_histx[itc+1][1] = it_histx[itc][1]+inner_f_evals+iarm+1;
        if (itc == 1) it_histx[itc+1][1] = it_histx[itc+1][2]+1;
        it_histx[itc+1][2] = iarm;
//
        rat = fnrm/fnrmo;
//
//   Adjust eta as per Eisenstat-Walker.
//

    if (ETAMAX > 0) {
        etaold = gmparms[0];
        etanew = gamma*rat*rat;
        if (gamma*etaold*etaold > .1) {
            etanew = max(etanew,gamma*etaold*etaold);
        }
        gmparms[0] = min(etanew,ETAMAX);
        gmparms[0] = max(gmparms[0],.5*stop_tol/fnrm);
    }
    }

    for (i=0;i<MAXIT;i++) {
        free(it_histx[i]);
    }
    free(it_histx);
    free(xold);
    free(xt);
    free(ft);
    free(step);
}

