/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>
#include <gsl/gsl_math.h>
#include <gsl/gsl_sf.h>

typedef enum
{
	PARAM0,
	PARAM1,
	V_BRUSH,
	ETA_CORE,
	ETA_BRUSH,
	ETA_SOLV,
	XSOLV_CORE,
	RG,
	D,
	EPSILON,
	RC,
	V_CORE,
	N_AGG,
	NAGG,
	R,
	Q
} variable_names;

#define VV       param->p

#define ELLIP_RWBRUSH_HEADER \
\
	scalar r_core, r_chain, p36; \
\
	SASFIT_ASSERT_PTR(param); \
\
	VV[Q] = q; \
\
	SASFIT_CHECK_COND1((VV[Q] < 0.0), param, "q(%lg) < 0",VV[Q]); \
	SASFIT_CHECK_COND1((VV[V_BRUSH] < 0.0), param, "V_brush(%lg) < 0",VV[V_BRUSH]); \
	SASFIT_CHECK_COND1((VV[RG] < 0.0), param, "Rg(%lg) < 0",VV[RG]); \
	SASFIT_CHECK_COND1((VV[XSOLV_CORE] == 1.0), param, "xsolv_core(%lg) == 1",VV[XSOLV_CORE]); \
	SASFIT_CHECK_COND1((VV[EPSILON] <= 0.0), param, "epsilon(%lg) <= 0",VV[EPSILON])


#define ELLIP_RWBRUSH_BODY \
\
        r_core  = VV[NAGG]*VV[V_CORE]  * (VV[ETA_CORE]  - VV[ETA_SOLV]); \
        r_chain =          VV[V_BRUSH] * (VV[ETA_BRUSH] - VV[ETA_SOLV]); \
\
        p36 =       pow(r_core,2.0)*sasfit_integrate(0.0, M_PI_2, sasfit_ff_ellip_rwbrush_fs_core, param); \
        p36 = p36 + VV[NAGG]*r_chain*r_chain*sasfit_ff_P18(VV[Q], VV[RG]); \
        p36 = p36 + VV[NAGG]*(VV[NAGG]-1.0)*r_chain*r_chain*sasfit_integrate(0.0, M_PI_2, sasfit_ff_ellip_rwbrush_scc_core, param)*((VV[NAGG] < 1) ?  0 : 1); \
        p36 = p36 + 2.0*VV[NAGG]*r_core*r_chain*sasfit_integrate(0.0, M_PI_2, sasfit_ff_ellip_rwbrush_ssc_core, param); \
\
        return p36

scalar XI(scalar Q, scalar R, scalar H, scalar alpha)
{
    scalar xR, xH;

    xR = Q*R*sin(alpha);
    xH = Q*H*cos(alpha)/2.0;

    if ((R+H) == 0.0) return 0.0;

   return R/(R+H) * 2.0*sasfit_jinc(xR) * cos(xH)
         +H/(R+H) * gsl_sf_bessel_J0(xR)*gsl_sf_bessel_j0(xH);
}

scalar sasfit_ff_P18(scalar Q, scalar Rg)
{
        scalar u;
        u = Q*Q * Rg*Rg;
        if ( u == 0 ) return 1.0;

        return 2*(exp(-u)+u-1.0) / (u*u);
}

scalar f1(scalar q, scalar r)
{
	scalar u = q*r;
    if (fabs(u)<1e-4) return (1 - gsl_pow_2(u)/10. + gsl_pow_4(u)/280. - gsl_pow_6(u)/15120.);
	return 3*(sin(u)-u*cos(u))/(u*u*u);
}

scalar r_ell(scalar r, scalar epsilon, scalar alpha)
{
	return r*gsl_hypot(sin(alpha),epsilon*cos(alpha));
}

scalar sasfit_ff_ellip_rwbrush_ssc_core(scalar alpha, sasfit_param *param)
{
    scalar w, u;

    SASFIT_ASSERT_PTR(param);

    w       = sasfit_rwbrush_w(VV[Q], VV[RG]);
	u 	= VV[Q] * (r_ell(VV[R],VV[EPSILON],alpha)+VV[D]*VV[RG]);

	return w*f1(VV[Q],r_ell(VV[R],VV[EPSILON],alpha))*gsl_sf_bessel_j0(u)*sin(alpha);
}

scalar sasfit_ff_ellip_rwbrush_scc_core(scalar alpha, sasfit_param *param)
{
    scalar w, u;

    SASFIT_ASSERT_PTR(param);

    w       = sasfit_rwbrush_w(VV[Q], VV[RG]);
	u 	= VV[Q] * (r_ell(VV[R],VV[EPSILON],alpha)+VV[D]*VV[RG]);
	return w*w * gsl_pow_2(gsl_sf_bessel_j0(u)) * sin(alpha);
}

scalar sasfit_ff_ellip_rwbrush_fs_core(scalar alpha, sasfit_param *param)
{
    SASFIT_ASSERT_PTR(param);

	return gsl_pow_2(f1(VV[Q],r_ell(VV[R],VV[EPSILON],alpha)))*sin(alpha);
}



scalar sasfit_ff_ellip_rwbrush(scalar q, sasfit_param * param)
{
	scalar s, ae, vol;
	ELLIP_RWBRUSH_HEADER;

	VV[RC]		= VV[PARAM0];
	VV[N_AGG]	= VV[PARAM1];
	vol = 4./3. * M_PI * pow(VV[RC],3.)*VV[EPSILON];

	if (VV[EPSILON] < 1.0)
	{
		ae = acos(VV[EPSILON]);
		s = 2.*M_PI*VV[RC]*VV[RC]*(1.+VV[EPSILON]*gsl_atanh(sin(ae))/sin(ae));
	}
	else if (VV[EPSILON] > 1.0)
	{
		ae = acos(1.0/VV[EPSILON]);
		s = 2.*M_PI*VV[RC]*VV[RC]*(1.0+VV[EPSILON]*ae/tan(ae));
	} else
	{
		s = 4.*M_PI*VV[RC]*VV[RC];
	}

	VV[R] = VV[RC];

	VV[NAGG] = VV[N_AGG]*s;
	if (SASFIT_EQUAL(VV[NAGG], 0.0))
	{
		VV[V_CORE] = 0.0;
	}
	else
	{
		VV[V_CORE] = vol * (1.0 - VV[XSOLV_CORE]) / VV[NAGG];
	}

	ELLIP_RWBRUSH_BODY;
}


scalar sasfit_ff_ellip_rwbrush_rc(scalar q, sasfit_param * param)
{
	ELLIP_RWBRUSH_HEADER;

	VV[RC]		= VV[PARAM0];
	VV[V_CORE]	= VV[PARAM1];
	SASFIT_CHECK_COND1((VV[V_CORE] < 0.0), param, "V_core(%lg) < 0",VV[V_CORE]);
	VV[R] = VV[RC];
	VV[NAGG] = 4./3.*M_PI*VV[R]*VV[R]*VV[R]*VV[EPSILON]*(1.0-VV[XSOLV_CORE])/VV[V_CORE];

	ELLIP_RWBRUSH_BODY;
}

scalar sasfit_ff_ellip_rwbrush_nagg(scalar q, sasfit_param * param)
{
	ELLIP_RWBRUSH_HEADER;

	VV[NAGG]	= VV[PARAM0];
	VV[V_CORE]	= VV[PARAM1];
	SASFIT_CHECK_COND1((VV[V_CORE] < 0.0), param, "V_core(%lg) < 0",VV[V_CORE]);
	VV[R]  = pow(fabs(VV[NAGG]*(VV[V_CORE]/(1.0-VV[XSOLV_CORE]))/(4/3*M_PI*VV[EPSILON])), 1.0/3.0);

	ELLIP_RWBRUSH_BODY;
}

scalar sasfit_ff_sphere_f(scalar q, sasfit_param * param)
{
	scalar L_ETA, L_R;
	L_R = param->p[0];
	L_ETA = param->p[3];
    SASFIT_ASSERT_PTR(param);

    SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
    SASFIT_CHECK_COND1((L_R < 0.0), param, "R(%lg) < 0",L_R);
    if (q*L_R<1e-4) return L_ETA*4.0/3.0*M_PI*gsl_pow_3(L_R)*(1 - gsl_pow_2(q*L_R)/10. + gsl_pow_4(q*L_R)/280. - gsl_pow_6(q*L_R)/15120.);
    return L_ETA*4.0*M_PI*(sin(q*L_R) - q*L_R*cos(q*L_R))/gsl_pow_3(q);
}


scalar sasfit_ff_sphere(scalar q, sasfit_param * param)
{
    scalar res;

    res = sasfit_ff_sphere_f(q, param);

    return gsl_pow_2(res);
}

scalar sasfit_ff_sphere_v(scalar x, sasfit_param * param, int distr)
{
    SASFIT_ASSERT_PTR(param);

    if ( distr != 0 ) x = param->p[0];

    return 4.0/3.0 * M_PI * gsl_pow_3(x);
}


scalar Fone(scalar q, scalar R)
{
	scalar F;
	if( fabs(q*R) <= 1e-4 )
	{
		F = (1 - gsl_pow_2(q*R)/10. + gsl_pow_4(q*R)/280. - gsl_pow_6(q*R)/15120.);
	} else {
		F = 3.0*(sin(q*R) - q*R*cos(q*R))/pow(q*R, 3.0);
	}
	return F;
}

scalar V(scalar R)
{
	return 4.0*M_PI*pow(R,3.0)/3.0;
}

scalar Sg(scalar q, scalar Rc, scalar s)
{
	scalar S;
	if(q == 0.0)
	{
		S = 1.0;
	}
	else if (Rc == 0.0)
	{
		S = exp(-pow(q*s,2.0)/2.0);
	}
	else if ((s == 0.0) && (q*Rc != 0.0))
	{
		S = sin(q*Rc)/(q*Rc);
	} else
	{
		S = pow(q*Rc*(4.*Rc*s+sqrt(2.*M_PI)*(pow(Rc,2.)+pow(s,2.))),-1.) *
			( 2.*Rc*s*sin(q*Rc)  +  sqrt(2*M_PI)*exp(-pow(q*s,2.)/2.) *
			  (q*Rc*pow(s,2.)*cos(q*Rc)+pow(Rc,2.)*sin(q*Rc)) +
			   2.*sqrt(2.)*gsl_sf_dawson(q*s/sqrt(2.)) *
			   (pow(Rc,2.)*cos(q*Rc)-q*Rc*pow(s,2.)*sin(q*Rc))
			);
	}
	return S;
}

scalar Ac(scalar q, scalar Rc, scalar R, scalar s)
{
	scalar Vzero, A;
	Vzero = 2.*M_PI*(4.*Rc*pow(s,2.)+sqrt(2.*M_PI)*(pow(Rc,2.)+pow(s,2.))*s);
	A = (Vzero*Sg(q, Rc,s)+V(Rc)*Fone(q,Rc)-V(R)*Fone(q,R))/(Vzero+V(Rc)-V(R));
	return A;
}

scalar Scc(scalar q, scalar Rc, scalar R, scalar s)
{
	scalar S;
	S = pow(Ac(q, Rc, R, s),2);
	return S;
}

scalar Ssc(scalar q, scalar Rc, scalar R, scalar s)
{
	scalar S = Fone(q,R) * Ac(q, Rc, R, s);
	return S;
}

scalar Ptwentyone(scalar x) // is identical to "F_Debye"
{
	scalar P;
	if(x == 0.0)
	{
		P = 1.0;
	} else {
		P = 2*(x-1+exp(-x))/pow(x,2);
	}
	return P;
}

scalar sasfit_Pthirtynine(scalar q, sasfit_param * param)
{
	scalar P, P_ch, P0_ch;
	scalar Nc, Rg, R, Rc, s, nu, Mthirtynine, rhos, rhoc, b, L;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 11, &Nc, &Rg, &R, &Rc, &s, &nu, &Mthirtynine, &rhos, &rhoc, &b, &L);

	sasfit_init_param( &subParam );
	subParam.p[0] = b;
	subParam.p[1] = L;
	subParam.p[2] = 0;

	//P_ch  = WormLikeChainEXV(interp,q,1.0,L,b,0.0,error)/(1.+nu*Ptwentyone(pow(q*Rg,2.)));
	P_ch  = sasfit_sq_p__q___worm_ps2_(q, &subParam)/(L*L)/(1.+nu*Ptwentyone(pow(q*Rg,2.)));

	SASFIT_CHECK_SUB_ERR(param, subParam);

	P0_ch = 1./(1.+nu);

	P =    pow(rhos,2)*pow(Fone(q,R),2)
		+  Nc*pow(rhoc,2)*P_ch
		+  Nc*(Nc-P0_ch)*pow(rhoc,2)*Scc(q, Rc, R, s)
		+  2*Nc*rhos*rhoc*Ssc(q, Rc, R, s)  ;
// The factor 1/pow(Mthirtynine,2) is skipped here compared to the paper from J.S. Pedersen as P39 shall be in absolute units

	return P;
}

scalar sasfit_ff_SphereWithGaussChains(scalar q, sasfit_param * param)
{
	scalar PHI,Fs,Fc,Scc,Ssc,I,w,sinc_x;
	scalar R, Rg, d, Nagg, rc, rs;

        SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 6, &R, &Rg, &d, &Nagg, &rc, &rs);

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((R < 0.0), param, "R(%lg) < 0",R);
	SASFIT_CHECK_COND1((Rg < 0.0), param, "Rg(%lg) < 0",Rg);

	if (q * R == 0)
	{
		PHI = 1;
	} else
	{
		PHI = 3 * ((sin(q * R) - q * R * cos(q * R)) / pow(q * R, 3));
	}

	Fs = PHI*PHI;
	w = sasfit_rwbrush_w(q, Rg);
	Fc = sasfit_gauss_fc(q, Rg);

	if (q * (R + d * Rg) == 0)
	{
		sinc_x = 1.0;
	} else
	{
		sinc_x = sin(q * (R + d * Rg)) / (q * (R + d * Rg));
	}
	Scc = pow(w*sinc_x, 2);
	Ssc = PHI * w * sinc_x;

	I = Nagg*Nagg*rs*rs*Fs
		+ Nagg*rc*rc*Fc
		+ Nagg*(Nagg-1)*((Nagg < 1) ?  0 : 1)*rc*rc*Scc
		+ 2*Nagg*Nagg*rs*rc*Ssc;
	return I;
}

scalar sasfit_ff_Sphere_RWbrush(scalar q, sasfit_param * param)
{
	scalar rc, rs, R, Nagg, S, V, res, V_core;
	scalar Rc, n_agg, V_brush, eta_core, eta_brush, eta_solv, xsolv_core, Rg, d;
	sasfit_param subParam;

        SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 9, EMPTY, EMPTY, &V_brush, &eta_core, &eta_brush, &eta_solv, &xsolv_core, &Rg, &d);

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((V_brush < 0.0), param, "V_brush(%lg) < 0",V_brush);
	SASFIT_CHECK_COND1((Rg < 0.0), param, "Rg(%lg) < 0",Rg);
	SASFIT_CHECK_COND((xsolv_core == 1.0), param, "xsolv_core == 1");

	switch( param->kernelSelector )
	{
		case SPHERE_RWBRUSH:
			Rc		= param->p[0];
			n_agg		= param->p[1];

			V = 4./3. * M_PI * pow(Rc,3.);
			S = 4.*M_PI*Rc*Rc;
			R = Rc;

			Nagg = n_agg*S;

			if (Rc == 0) return 0;
			if (Nagg == 0.0) return pow(V*(1.-xsolv_core)*(eta_core-eta_solv)*3.*(sin(q*R)-q*R*cos(q*R))/pow(q*R,3.),2.);

			rs = V/Nagg * (eta_core  - eta_solv)*(1.-xsolv_core);
			break;
		case SPHERE_RWBRUSH_RC:
			Rc		= param->p[0];
			V_core		= param->p[1];

			R = Rc;
			SASFIT_CHECK_COND1((V_core < 0.0), param, "V_core(%lg) < 0",V_core);
			Nagg = 4./3.*M_PI*R*R*R*(1-xsolv_core)/V_core;

			rs = V_core * (eta_core  - eta_solv);
			break;
		case SPHERE_RWBRUSH_NAGG:
			Nagg		= param->p[0];
			V_core		= param->p[1];

			SASFIT_CHECK_COND1((V_core < 0.0), param, "V_core(%lg) < 0",V_core);
			R = pow(fabs(Nagg*(V_core/(1.-xsolv_core))*3./4./M_PI),1./3.);

			rs = V_core * (eta_core - eta_solv);
			break;
		default:
			SASFIT_ERR_UNKNOWN_KERNEL(param, "kernel");
			break;
	}

	rc = V_brush * (eta_brush - eta_solv);

	sasfit_init_param( &subParam );
	subParam.p[0] = R;
	subParam.p[1] = Rg;
	subParam.p[2] = d;
	subParam.p[3] = Nagg;
	subParam.p[4] = rc;
	subParam.p[5] = rs;

//	return SphereWithGaussChains(interp,q,R,Rg,d,Nagg,rc,rs,error);
	res = sasfit_ff_SphereWithGaussChains(q, &subParam);
	SASFIT_CHECK_SUB_ERR(param, subParam);

	return res;
}

scalar sasfit_ff_Sphere_R_ma_Profile_ave_core(scalar x, sasfit_param * param)
{
	scalar Q,u,alpha;

        SASFIT_ASSERT_PTR(param);

	Q 	= param->p[MAXPAR-1];
        alpha 	= param->p[6];

        u = Q*x;

        if (u==0.0) return pow(x,-alpha)* 4.0 * M_PI * x*x;

        return pow(x,-alpha)* 4.0 * M_PI * x*x * sin(u)/u;
}


scalar sasfit_ff_Sphere_R_ma_Profile(scalar q, sasfit_param * param)
{
	scalar Plocal, Psh, Fsh, Fc, Pc, Vc, V, S, Nagg, b_c, b_sh, exvol;
	scalar smoothie = 1.0;
	scalar Rc, n_agg, Vsh, rho_c, rho_sh, rho_solv, alpha, t, Rm, L, b, xsolv_core, res1, res2;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 12, EMPTY, EMPTY, &Vsh, &rho_c, &rho_sh, &rho_solv, &alpha, &t, &L, &b, &xsolv_core,&exvol);

	switch( param->kernelSelector )
	{
		case SPHERE_RMA:
			Rc	= param->p[0];
			n_agg	= param->p[1];
			if (Rc == 0.0) return 0.0;
			Fc = sasfit_sphere_fc(q, Rc) * smoothie;
			Pc = Fc*Fc;
			V = 4./3. * M_PI * pow(Rc,3.);
			S = 4.*M_PI*Rc*Rc;
			Nagg = n_agg*S;
			if (Nagg == 0.0) return Pc;
			Vc = V / Nagg*(1.-xsolv_core);
			break;
		case SPHERE_2ND_RMA_RC:
			Rc	= param->p[0];
			Vc	= param->p[1]-fabs(Vsh);
			SASFIT_CHECK_COND1((Vc <= 0.0), param, "Vc(%lg) < 0",Vc);
			if (Rc == 0.0) return 0.0;
			Fc = sasfit_sphere_fc(q, Rc) * smoothie;
			Pc = Fc*Fc;
			V = 4./3. * M_PI * pow(Rc,3.)*(1-xsolv_core);
			Nagg = V / Vc;
			break;
		case SPHERE_RMA_RC:
		case SPHERE_RMA_RC_SMOOTH:
			Rc	= param->p[0];
			Vc	= param->p[1];
			SASFIT_CHECK_COND1((Vc <= 0.0), param, "Vc(%lg) < 0",Vc);
			if (Rc == 0.0) return 0.0;
			Fc = sasfit_sphere_fc(q, Rc) * smoothie;
			Pc = Fc*Fc;
			V = 4./3. * M_PI * pow(Rc,3.)*(1-xsolv_core);
			Nagg = V / Vc;
			break;
		case SPHERE_RMA_NAGG:
		case SPHERE_RMA_NAGG_SMOOTH:
			Nagg	= param->p[0];
			Vsh     = fabs(Vsh);
//			Vc	= param->p[1] - Vsh; // this was wrong Vc should be param->p[1]
			Vc	= fabs(param->p[1]);
			// I am using here the abs-value to avoid eventually negative value during a fit,
			// which can happen if the scattering curve is weakly dependent on this parameter or
			// if this function is not the right choice to describe the scattering curve
			SASFIT_CHECK_COND1((Nagg < 0.0), param, "Nagg(%lg) < 0",Nagg);
			V = Nagg*Vc/(1.-xsolv_core);
			Rc = pow(V*3./4./M_PI,1./3.);
			Fc = sasfit_sphere_fc(q, Rc) * smoothie;
			Pc = Fc*Fc;
			break;
		default:
			SASFIT_ERR_UNKNOWN_KERNEL(param, "kernel");
			break;
	}
	if ( param->kernelSelector == SPHERE_RMA_NAGG_SMOOTH ||
	     param->kernelSelector == SPHERE_RMA_RC_SMOOTH )
	{
		b_c  = Vc	* rho_c;	// Vc  * Drho_c
		b_sh = fabs(Vsh) * rho_sh;	// Vsh * Drho_sh
		smoothie = exp(-0.5 * q*q * rho_solv*rho_solv);	// rho_solv == sigma
	} else
	{
		b_c  = Vc	* (rho_c  - rho_solv);
		b_sh = fabs(Vsh) * (rho_sh - rho_solv);
	}

	if ((t == 0.0) || (b_sh == 0.0))
	{
		Fsh = Psh = 0.0;
	} else
	{
		SASFIT_CHECK_COND(((alpha == 3) && (Rc == 0)), param, "alpha==3 and Rc==0");

		param->p[MAXPAR-1] = q;
		Rm = Rc+t;
		//Fsh = SPHERE_R_ma_ave(interp,q,Rc,Rc+t,alpha,error);
		res1 = sasfit_integrate(Rc, Rc+t, sasfit_ff_Sphere_R_ma_Profile_ave_core, param);
		if (alpha == 3.0)
		{
			res2 = 4.*M_PI*(log(Rm/Rc));
		} else
		{
			res2 = 4./(3.-alpha)*M_PI*(pow(Rm,3.-alpha)-pow(Rc,3.-alpha));
		}
		Fsh = (res1/res2) * smoothie;
		Psh = Fsh*Fsh;
	}
	switch (lround(fabs(exvol))) {
	    case 0:
	        sasfit_init_param( &subParam );
            subParam.p[0] = b;
            subParam.p[1] = L;
            subParam.p[2] = 0.0;
            Plocal =  sasfit_sq_p__q___worm_ps3_(q, &subParam)/(L*L);
            break;
	    case 1:
	        sasfit_init_param( &subParam );
            subParam.p[0] = b;
            subParam.p[1] = L;
            subParam.p[2] = 1.0;
            Plocal =  sasfit_sq_p__q___worm_ps3_(q, &subParam)/(L*L);
            break;
        default:
            sasfit_init_param( &subParam );
            subParam.p[0] = L;
            subParam.p[1] = b;
            subParam.p[2] = 1.0;
            Plocal = sasfit_ff_generalized_gaussian_coil(q,&subParam);
	}

	if (Vsh<0) return Nagg * b_sh * b_sh * Plocal;
	return  Nagg*Nagg	* b_c  * b_c  * Pc +
		Nagg*(Nagg-1.0) * b_sh * b_sh * Psh * ((Nagg < 1) ?  0 : 1) +
		2.0*Nagg*Nagg	* b_c  * b_sh * Fc * Fsh +
		Nagg * b_sh * b_sh * Plocal;
}

scalar sasfit_ff_Sphere_R_ma_Profile_v(scalar q, sasfit_param * param, int distr)
{
	SASFIT_ASSERT_PTR(param);

	if ( param->kernelSelector == SPHERE_RMA_NAGG )
	{
        if (distr == 0) return q * param->p[1];
		else if (distr == 1) return param->p[0] * q;
		else                 return param->p[0] * param->p[1];
	} else
	{
        if ( distr == 0 ) q = q + param->p[7];
		else if ( distr == 7 ) q = param->p[0] + q;
		else                   q = param->p[0] + param->p[7];

		return sasfit_ff_sphere_v(q, param, 0);
	}
}


scalar sasfit_ff_Sphere_SAWbrush(scalar q, sasfit_param * param)
{
	scalar s, R, Mthirtynine, sigma, nu, Nc, rhos, rhoc, S, V, b, V_core, Nagg, res;
	scalar Rc, n_agg, V_brush, eta_core, eta_brush, eta_solv, xsolv_core, Rg, L;
	sasfit_param subParam;

        SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 10, EMPTY, EMPTY, &V_brush, &eta_core, &eta_brush, &eta_solv, &xsolv_core, &Rg, &L, &b);

	SASFIT_CHECK_COND1((L <= 0.0), param, "L(%lg) <= 0",L);
	SASFIT_CHECK_COND1((b < 0.0), param, "b(%lg) < 0",b);

	switch( param->kernelSelector )
	{
		case SPHERE_SAW:
			Rc		= param->p[0];
			n_agg		= param->p[1];

			SASFIT_CHECK_COND((xsolv_core == 1.0), param, "xsolv_core == 1");
			V = 4./3. * M_PI * pow(Rc,3.);
			S = 4.*M_PI*Rc*Rc;
			Nc = n_agg*S;
			rhos = V/(1.-xsolv_core) * (eta_core  - eta_solv);
			break;
		case SPHERE_SAW_RC:
			Rc		= param->p[0];
			V_core		= param->p[1];

			V = 4./3. * M_PI * pow(Rc,3.);
			Nc = V*(1.-xsolv_core)/V_core;
			rhos = Nc*V_core  * (eta_core  - eta_solv);
			break;
		case SPHERE_SAW_NAGG:
			Nagg		= param->p[0];
			V_core		= param->p[1];

			V = (Nagg*V_core)/(1.-xsolv_core);
			Rc = pow(V*3./4./M_PI,1./3.);
			Nc = Nagg;
			rhos = Nc*V_core  * (eta_core  - eta_solv);
			break;
		default:
			SASFIT_ERR_UNKNOWN_KERNEL(param, "kernel");
			break;
	}

	if (Nc == 0.0) return 0.0;

	rhoc = V_brush           *(eta_brush - eta_solv);

	s = Rg;
	R = Rc;
	Mthirtynine = rhos + Nc*rhoc;
/* ?? not used here ...
	if (Rg == 0.0)
	{
		sigma = Nc/4.;
	}
*/
	sigma = Nc*pow(Rg,2.)/(4.*pow(R+Rg,2.));
	nu = 1.4*pow(sigma,1.04);

	//return Pthirtynine(interp,q, Nc, Rg, R, Rc, s, nu, Mthirtynine, rhos, rhoc, b, L,error);
	sasfit_init_param( &subParam );
	subParam.p[0] = Nc;
	subParam.p[1] = Rg;
	subParam.p[2] = R;
	subParam.p[3] = Rc;
	subParam.p[4] = s;
	subParam.p[5] = nu;
	subParam.p[6] = Mthirtynine;
	subParam.p[7] = rhos;
	subParam.p[8] = rhoc;
	subParam.p[9] = b;
	subParam.p[10]= L;

//	return SphereWithGaussChains(interp,q,R,Rg,d,Nagg,rc,rs,error);
	res = sasfit_Pthirtynine(q, &subParam);
	SASFIT_CHECK_SUB_ERR(param, subParam);

	return res;
}
