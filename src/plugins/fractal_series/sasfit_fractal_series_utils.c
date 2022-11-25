/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>
#include <gsl/gsl_roots.h>

// define shortcuts for local parameters/variables

scalar fractalSD_single(scalar x, scalar a,scalar fd, scalar xmin, scalar xmax,sasfit_param * param) {
    return a*pow(x,-(1+fd))*fd/(pow(xmin,-fd)-pow(xmax,-fd));
}

scalar sc_ExpIntegralE(double n, double x) {
    if (x >=0) {
        return pow(x,n-1)*gsl_sf_gamma_inc(1-n,x);
    } else {
        return 0.0;
    }
}

scalar fractalFF_single(scalar Q, scalar a,scalar fd, scalar xmin, scalar xmax,sasfit_param * param) {

#define E exp(1)
#define Power pow
#define Sqrt sqrt
#define Gamma gsl_sf_gamma_inc

scalar xmin2,xmax2,Q2,xmin3,xmax3,Q3,xmax4,xmin4,Q4,xmin5,xmax5,Q5,xmax6,xmin6,Q6,xmax7,xmin7,Q7,xmax8,xmin8,Q8,xminfd,xmaxfd;
scalar tmp;

    xmin2=gsl_pow_2(xmin);
    xmax2=gsl_pow_2(xmax);
    Q2=gsl_pow_2(Q);
    xmin3=xmin2*xmin;
    xmax3=xmax2*xmax;
    Q3=Q2*Q;
    xmin4=gsl_pow_4(xmin);
    xmax4=gsl_pow_4(xmax);
    Q4=gsl_pow_4(Q);
    xmin5=xmin4*xmin;
    xmax5=xmax4*xmax;
    Q5=Q4*Q;
    xmin6=xmin4*xmin2;
    xmax6=xmax4*xmax2;
    Q6=Q4*Q2;
    xmin7=xmin6*xmin;
    xmax7=xmax6*xmax;
    Q7=Q6*Q;
    xmin8=xmin4*xmin4;
    xmax8=xmax4*xmax4;
    Q8=Q4*Q4;
    xminfd=pow(xmin,fd);
    xmaxfd=pow(xmax,fd);

   if (Q*xmax < 1e-3) {
     if (fd != 6) {
        return  (a*fd*(xmaxfd*xmin6 - xmax6*xminfd))/
                ((-6 + fd)*(xmaxfd - xminfd));
     } else {
         return (6*a*xmax6*xmin6*log(xmax/xmin))/(xmax6 - xmin6);
     }
   }

   if (Q*xmin > 100) {
     if (fd != 2) {
        return  (a*fd*(xmaxfd*xmin2 - xmax2*xminfd))/
                ((-2 + fd)*(xmaxfd - xminfd))/Q4;
     } else {
         return (2*a*xmax2*xmin2*log(xmax/xmin))/(xmax2 - xmin2)/Q4;
     }
   }
   if (fd == 1) {
        return (a*xmax*xmin*(Q*(xmax - xmin)*(3 + Q2*(2*xmax2 - xmax*xmin + 2*(1 + Q2*xmax2)*xmin2))
                     +3*(1 + Q2*xmax2)*(1 + Q2*xmin2)*(atan(Q*xmin)-atan(Q*xmax))))/
                (2.*Q5*(1 + Q2*xmax2)*(xmax - xmin)*(1 + Q2*xmin2));
    } else if (fd == 2) {
        return (a*(   1/(1 + Q2*xmax2)
                    - 1/(1 + Q2*xmin2)
                    + log(1 + Q2*xmax2) -
                      log(1 + Q2*xmin2)
                   )
                )/(Q4*(1/xmin2-1/xmax2));
    } else if (fd == 3) {
        return (3*a*(Q*(xmax - xmin)*(-1 + Q2*xmax*xmin)
                                + (1 + Q2*xmax2)*(1 + Q2*xmin2)*(atan(Q*xmax)-atan(Q*xmin))))/
                (2.*Q3*(1 + Q2*xmax2)*(1./xmin3-1./xmax3)*(1 + Q2*xmin2));
    } else if (fd == 4) {
        return (2*a*xmax4*xmin4)/((1 + Q2*xmax2)*(xmax2 + xmin2)*(1 + Q2*xmin2));
    } else if (fd == 5) {
        return (-5*a*(Q*(xmax - xmin)*(-1 + Q2*xmax*xmin)
               +(1 + Q2*xmax2)*(1 + Q2*xmin2)*(atan(Q*xmin)-atan(Q*xmax))))/
            (2.*Q*(1 + Q2*xmax2)*(1/xmin5-1/xmax5)*(1 + Q2*xmin2));
    } else if (fd == 6) {
        return (3*a*xmax6*xmin6*(Q2*(-xmax2 + xmin2 -
                (xmin2 + xmax2*(1 + Q2*xmin2))*
                    log(((1 + Q2*xmax2)*xmin2)/(xmax2*(1 + Q2*xmin2)))) +
                    log((xmax2*(1 + Q2*xmin2))/((1 + Q2*xmax2)*xmin2))))/
                ((1 + Q2*xmax2)*(1 + Q2*xmin2)*(xmax6 - xmin6));
    } else if (fd == 7) {
        return (7*a*xmax6*xmin6*((xmax - xmin)*
                    (2 + Q2*(-(xmax*xmin) + 2*xmin2 + xmax2*(2 + 3*Q2*xmin2)))
                +3*Q*xmax*(1 + Q2*pow(xmax,2))*xmin*(1 + Q2*xmin2)*(-atan(Q*xmax) + atan(Q*xmin))))/
            (2.*(1 + Q2*xmax2)*(1 + Q2*xmin2)*(xmax7 - xmin7));
    } else if (fd == 8) {
        return (4*a*xmax6*xmin6*((xmax - xmin)*(xmax + xmin)*
                    (1 + 2*Q4*xmax2*xmin2 + Q2*(xmax2 + xmin2)) +
                2*Q2*xmax2*(1 + Q2*xmax2)*xmin2*(1 + Q2*xmin2)*
                    log(((1 + Q2*xmax2)*xmin2)/(xmax2*(1 + Q2*xmin2)))))/
            ((1 + Q2*xmax2)*(1 + Q2*xmin2)*(xmax8 - xmin8));
    } else {
        if (Q2*xmax2 < 0.1) {
            tmp = ((-2 + fd)*Q2*(-(xmaxfd*xmin4) + xmax4*xminfd))/(-4 + fd)
                - ((-2 + fd)*Q4*(-(xmaxfd*xmin6) + xmax6*xminfd))/(-6 + fd) +
                  ((-2 + fd)*Q6*(-(xmaxfd*xmin8) + xmax8*xminfd))/(-8 + fd);
        } else if (Q2*xmin>3) {
            tmp =   -(xmaxfd*xmin2)
                    + xmax2*xminfd +
                    ((-2 + fd)*(xmaxfd - xminfd))/(fd*Q2) +
                    ((-2 + fd)*(-pow(xmax,2 + fd) + pow(xmin,2 + fd)))/
                            ((2 + fd)*Q4*xmax2*xmin2) +
                    ((-2 + fd)*(pow(xmax,4 + fd) - pow(xmin,4 + fd)))/
                            ((4 + fd)*Q6*xmax4*xmin4) +
                    ((-2 + fd)*(-pow(xmax,6 + fd) + pow(xmin,6 + fd)))/
                            ((6 + fd)*Q8*xmax6*xmin6);
        } else {
            tmp =
                xmax2*xminfd*
                sasfit_2f1(1.,(-2. + fd)/2.,fd/2.,-(1./(Q2*xmax2)),param) -
                xmaxfd*xmin2*
                sasfit_2f1(1.,(-2. + fd)/2.,fd/2.,-(1./(Q2*xmin2)),param);
        }
        return (a*(fd*Q2*(- (xmax4*xminfd)/(1 + Q2*xmax2)
                          + (xmaxfd*xmin4)/(1 + Q2*xmin2)
                         ) +
                ((-4 + fd)*fd*tmp)/(-2 + fd)))/(2.*Q4*(xmaxfd - xminfd));
    }
}
