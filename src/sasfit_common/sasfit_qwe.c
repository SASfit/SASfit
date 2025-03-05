/*
%------------------------------------------------------------------%
% Copyright (c) 2012 by the Society of Exploration Geophysicists.  %
% For more information, go to http://software.seg.org/2012/0003 .  %
% You must read and accept usage terms at:                         %
% http://software.seg.org/disclaimer.txt before use.               %
%------------------------------------------------------------------%

function T = qwe(rtol,atol,nIntervalsMax,func,varargin)
%
% QWE - Quadrature with extrapolation
%
% Computes an infinite integral using the partial sum of quadrature terms
% accelerated by sequence extrapolation using the Shanks transformation
% implemented with Wynn's epsilon algorithm.
%
% Usage:
%
% T = qwe(rtol,atol,nIntervalsMax,func,varargin)
%
% Inputs:
%
% rtol,atol     - Relative and absolute tolerances for the extrapolation.
%                 Extrapolation for integral T stops when:
%                 absolute_error < rtol*abs(T) + atol
% nIntervalsMax - Maximum number of partial integral intervals
% func,varargin - Handle to the function that returns the
%                 quadarature sum for this interval. varargin
%                 are passed through to func to assist in the kernel eval.
%                 func has the form: func(interval,varargin), where the
%                 interger interval is an index to the precomputed absiccae
%                 and weights stored in the calling routine. func can
%                 return multiple related kernels, each of which will be
%                 integrated and extrapolated.
%
% Outputs:
%
% T - structure with fields described below. If func returns multiple
% related kernels (nKernels), then the T is an array of structures, ie
% T(1:nKernels).  The kernel integrations and subsequent transforms are
% applied to each kernel in parallel, but are stopped uniquely for each
% when tolerance has  been met.  For example, func might return the J0 and
% J1 kernels of electromagnetic fields Ey (nKernels=2), or from multiple
% components (nKernels=2*3 for Ex,Ey,Ez). This routine would then return
% T(1).n, T(2).n etc with the fields shown below:
%
% T.n             - Final order of the transform to reach rtol or atol.
% T.extrap(1:T.n) - Result from extrapolation at each order.  The final
%                   best answer is: T.extrap(T.n)
% T.absErr(1:T.n) - Estimated absolute error for each order n.
% T.relErr(1:T.n) - Estimated relative error for each order n.
%
%
% Written by:
%
% Kerry Key
% Scripps Institution of Oceanography
%
% History:
%
% June 8, 2011 - Moved quadrature outside.
% May 2011     - Implemented.
%
%--------------------------------------------------------------------------

%
% Adjustable parameters:
%
    nDelay = 1;  % must be >= 1, but 1 is usually sufficient.

%
% First compute nDelay partial integrals before starting
% the Shanks transformation iterations:
%
    prev = 0;
    for i = 1:nDelay
        prev    = prev + func(i,varargin{:}); % Call to func returns quadrature sum
    end

%
% Then initialize the T structure for the extrapolation results:
%
    nKernels = length(prev);  % func from nDelay loop above returns nKernels results, so now we
                              % know how many related kernels are being handled

    nTerms   = nIntervalsMax-nDelay-1;  % maximum number of terms in extrapolation

    rmin     = realmin;

    for i = 1:nKernels  % Preallocating arrays at maximum size for speed:
        T(i).S      = zeros(nTerms,1); % working array used for the recursion coefficients for the Epsilon algorithm
        T(i).extrap = zeros(nTerms,1); % extrapolated result for each order of the expansion
        T(i).relErr = zeros(nTerms,1); % relative error for each order
        T(i).absErr = zeros(nTerms,1); % absolute error for each order
        converged   = false(nKernels,1);
    end

%
% The extrapolation transformation loop:
%
    for i = nDelay+1:nTerms

        %
        % Step 1: Compute Guass quadrature of this interval:
        %
        f = func(i,varargin{:});

        %
        % Step 2: Compute Shanks transformation for each kernel function:
        %
        for j = 1:nKernels

            if  converged(j)  % skip this kernel since its done
                continue;
            end

            %
            % Insert components into the T structure:
            %
            n           = i-nDelay;
            T(j).n      = n;               % order of the expansion
            T(j).S(n+1) = T(j).S(n)+f(j);  % working array for transformation

            %
            % Compute the Shanks transform using the Epsilon algorithm:
            % Structured after Weniger (1989, p26)
            %
            aux2 = 0;
            for k = n+1:-1:2
                aux1 = aux2;
                aux2 = T(j).S(k-1);
                ddff = T(j).S(k) - aux2;
                if abs(ddff) < rmin
                    T(j).S(k-1) = realmax;
                else
                    T(j).S(k-1) = aux1 + 1/ddff;
                end
            end

            %
            % The extrapolated result plus the prev integration term:
            %
            T(j).extrap(n) = T(j).S(mod(n,2)+1)+prev(j);

            %
            % Step 3: Analyze for convergence:
            %
            if n > 1
                T(j).absErr(n) = abs( T(j).extrap(n) - T(j).extrap(n-1));
                T(j).relErr(n) = T(j).absErr(n) / abs(T(j).extrap(n)) ;
                converged(j)   = T(j).relErr(n) < rtol + atol/abs(T(j).extrap(n));
            end

        end % loop over nKernels

        if all(converged)
            break;
        end

    end % extrapolation loop

    % Clean up the T structure arrays:
    for j = 1:nKernels
        n = T(j).n;
        T(j).extrap = T(j).extrap(1:n);
        T(j).relErr = T(j).relErr(1:n);
        T(j).absErr = T(j).absErr(1:n);
    end

    % Goodbye my friend, returning to the calling function

end % function qwe
*/


#include "include/sasfit_common.h"
#include "include/sasfit_function.h"
#include <float.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_sf.h>

scalar FrJnu(scalar r, sasfit_param * param) {
    scalar Q,nu;
    nu = param->more_p[0];
    Q  = param->more_p[1];
    return r*gsl_sf_bessel_Jnu(nu,Q*r)*param->function(r,param->moreparam);
}

scalar sasfit_qwe(double nu, double (*f)(double, void *), double x, void *fparams, int nIntervalsMax, scalar rtol, scalar atol) {
	int nDelay;
	int nTerms, i,k,n;
	scalar *S, *extrap, *relErr, *absErr;
	scalar prev,amin,amax,rmin, Fi, aux2, aux1, ddff,res;
	sasfit_param param;
	bool converged;
	param.function = f;
	param.more_p[0] = nu;
	param.more_p[1] = x;
    param.moreparam=fparams;
	prev=0;
	nDelay = 1;
	amax=gsl_sf_bessel_zero_Jnu(nu,nDelay)/param.more_p[1];
	amin=DBL_MIN;
	amin=amax*sasfit_eps_get_nriq();
	prev = prev + sasfit_integrate(amin,amax,&FrJnu,&param);
	nTerms   = nIntervalsMax-nDelay-1;
	rmin     = DBL_MIN;
	S      = calloc(nTerms+1,sizeof(scalar));
	extrap = calloc(nTerms+1,sizeof(scalar));
	relErr = calloc(nTerms+1,sizeof(scalar));
	absErr = calloc(nTerms+1,sizeof(scalar));
    converged = FALSE;
	for (i=nDelay+1;i<=nTerms;i++) {
        amin=amax;
        amax=gsl_sf_bessel_zero_Jnu(nu,i)/param.more_p[1];
        Fi = sasfit_integrate(amin,amax,&FrJnu,&param);

        n = i-nDelay;
        S[n+1] = S[n]+Fi;
        aux2 = 0.0;
        for (k=n+1;k>=2;k--) {
            aux1=aux2;
            aux2=S[k-1];
            ddff=S[k]-aux2;
            if (fabs(ddff)<rmin) {
                S[k-1]=DBL_MAX;
            } else {
                S[k-1]=aux1+1./ddff;
            }
        }
        extrap[n] = S[(n % 2)+1]+prev;
        res = extrap[n];
        if (n > 1) {
                absErr[n] = fabs( extrap[n] - extrap[n-1]);
                relErr[n] = absErr[n] / fabs(extrap[n]) ;
                if (relErr[n] < rtol + atol/fabs(extrap[n]))
                    converged   = TRUE;
        }
        if (converged) break;
    }
	free(S);
	free(extrap);
	free(relErr);
	free(absErr);
	if (!converged) sasfit_err("QWE algorithm did not converged after maximum allowed intervals: %d\n",nIntervalsMax);
	return res;
}


/*
Copyright (c) 2018, Alan Chave
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are
met:

    * Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
    * Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in
      the documentation and/or other materials provided with the distribution
    * Neither the name of the Woods Hole Oceanographic Institution nor the names
      of its contributors may be used to endorse or promote products derived
      from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGE.
*/

scalar ZerojCos(int nzero, int sincos) {
    return M_PI_2+(nzero-1)*M_PI;
}
scalar ZerojSin(int nzero) {
    return (nzero)*M_PI;
}
scalar Zeroj(int nzero,scalar order) {
//%Computes the approximate zero of bessel function of the first kind of
//%order 0 or 1
//%   If nzero < 7 the algorithm in J. Comp. Phys., 42, 403-405, 1981 is used
//%   If nzero >= 7, McMahon's asymptotic expansion is used
//%
//%Input variables
//%nzero--index of the zero, integer >= 1
//%order--order of the bessel function, must be 0 or 1
//%
    scalar Zero,beta, t,b;
    if (nzero < 7) {
        beta = (nzero + order/2.0 - 0.25)*M_PI;
        if (order == 0) {
            t = 0.0682894897349453 + 0.131420807470708*beta*beta +
                0.0245988241803681 *gsl_pow_4(beta) +
                0.000813005721543268 *gsl_pow_6(beta);
            b = beta + 1.16837242570470*gsl_pow_3(beta) +
                0.200991122197811*gsl_pow_5(beta) + 0.00650404577261471*gsl_pow_7(beta);
            Zero = beta + t/b;
            return Zero;
        } else if (order == 1) {
            t = -0.362804405737084 + 0.120341279038597*gsl_pow_3(beta)+
                0.0439454547101171*gsl_pow_4(beta) + 0.00159340088474713*gsl_pow_6(beta);
            b = beta - 0.325641790801361*gsl_pow_3(beta) -
                0.117453445968927*gsl_pow_5(beta) - 0.424906902601794*gsl_pow_7(beta);
            Zero = beta + t/b;
            return Zero;
        }
    } else {
        if (order == 0) {
            beta = (nzero - 0.25)*M_PI;
            Zero = beta + 1.0/(8.0*beta) - 124.0/(1536.0*gsl_pow_3(beta)) +
                120928.0/(491520.0*gsl_pow_5(beta)) -
                401743168.0/(220200960.0*gsl_pow_7(beta));
            return Zero;
        } else if (order == 1) {
            beta = (nzero + 0.25)*M_PI;
            Zero = beta - 3.0/(8.0*beta) + 36.0/(1536.0*gsl_pow_3(beta)) -
                113184.0/(491520.0*gsl_pow_5(beta)) +
                1951209.0/(220200960.0*gsl_pow_7(beta));
            return Zero;
        }
    }
    return gsl_sf_bessel_zero_Jnu(order,nzero);
}


scalar Padesum(scalar *s, size_t n) {
//%Computes sum from 1 to n of s(i) using Pade approximant implemented with
//%continued fraction expansion; see Z. Naturforschung, 33a, 402-417, 1978.
//%
//%Input variable
//%s--series of values to be summed, may be complex
//%Output variable
//%Cf--sum of the series
//%
    scalar *D, *d, *x, *t;
    scalar Cf;
    int i,k,L;
    D = calloc(n+1,sizeof(scalar));
    x = calloc(n+1,sizeof(scalar));
    d = calloc(n+1,sizeof(scalar));
    t = calloc(n+1,sizeof(scalar));
    D[1] = s[1];
    d[1] = D[1];
    if (n == 1) {
        Cf = d[1];
        goto exitPadesum;
    }
    D[2] = s[2];
    d[2] = -D[2]/D[1];
    if (n == 2) {
        Cf = d[1]/(1 + d[2]);
        goto exitPadesum;
    }
    for (i=3;i<=n;i++) {
        L = 2*lround(floor((i-1.)/2.));
//%update x vector
        for (k=L;k>=4;k=k-2) {
            x[k] = x[k-1] + d[i-1]*x[k-2];
        }
        x[2] = x[1] + d[i - 1];
//% interchange odd and even parts
        for (k=1;k<=L-1;k=k+2) {
            t[k] = x[k];
            x[k] = x[k+1];
            x[k+1] = t[k];
        }

//% compute cf coefficient
        D[i] = s[i];
        for (k=0;k<=L/2-1;k++) {
            D[i] = D[i] + s[i-1-k]*x[1+2*k];
        }
//    D[i] = s[i] + s[i-1:-1:i-L/2]*x[1:2:L-1];
        d[i] = -D[i]/D[i-1];
    }
//%evaluate continued fraction
    Cf = 1;
    for (k=n;k>=2;k--) {
        Cf = 1 + d[k]/Cf;
    }

    Cf = d[1]/Cf;
exitPadesum:
    free(D);
    free(d);
    free(x);
    free(t);
    return Cf;
}

scalar sasfit_HankelChave(scalar order, double (*f)(double, void *), scalar r, void *fparams, int nIntervalsMax, scalar rerr, scalar aerr) {
//%Computes Hankel transform integral from 0 to inf J_sub_order(x*r)*f(x) by integration
//%between zero crossings of the Bessel function followed by summation
//%using Pade approximant to speed up convergence
//%
//%Input variables
//%order--order of the Bessel function, either 0 or 1
//%r--argument of the Hankel transform
//%f--function to compute kernel called as f(x,fparams)
//%rerr--relative error
//%aerr--absolute error
//%
//%Output variable
//%Sum--computed integral
//%
//%Functions required: Zeroj, Padesum
    scalar Sum,a,b,last,*s;
    int nzero,i;
	bool converged;
	sasfit_param param;
	converged = FALSE;
	param.function = f;
	param.more_p[0] = order;
	param.more_p[1] = r;
    param.moreparam=fparams;
    b=0;
    b=Zeroj(1,order)/r*rerr;
    last=0;
    s=calloc(nIntervalsMax+1,sizeof(scalar));
    for (nzero = 1;nzero<=nIntervalsMax;nzero++) { //%upper limit is arbitrary and should never be reached
        a = b;
        b = Zeroj(nzero,order)/r;
        s[nzero] = sasfit_integrate(a,b,&FrJnu,&param);
        Sum = Padesum(s,nzero);
        if (fabs(Sum - last) <= rerr*fabs(Sum) + aerr) {
            converged = TRUE;
            return Sum;
        }
        last=Sum;
    }
    free(s);
    if (!converged) sasfit_err("HankelChave algorithm did not converged after maximum allowed intervals: %d\n",nIntervalsMax);
    return Sum;
}
