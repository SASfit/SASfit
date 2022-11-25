/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

#include "include/private.h"
#include <sasfit_error_ff.h>

// define shortcuts for local parameters/variables
#define C param->p[0]
#define MCORE	param->p[1]
#define MBRUSH	param->p[2]
#define RHO_CORE	param->p[3]
#define RHO_BRUSH	param->p[4]
#define B_CORE	param->p[5]
#define B_BRUSH	param->p[6]
#define NAGG	param->p[7]
#define D1_PLUS	param->p[8]
#define PART23	param->p[9]
#define D2_PLUS	param->p[10]
#define D3_PLUS	param->p[11]
#define SIGMA1	param->p[12]
#define SIGMA2	param->p[13]
#define SIGMA3	param->p[14]
#define PARTSTAR	param->p[15]
#define GAMMA	param->p[16]
#define LPARABOL	param->p[17]
#define F_BRUSH	param->p[18]
#define F_CORE param->p[19]
#define RHOSOLV param->p[20]
#define Q	param->p[MAXPAR-1]



scalar sasfit_ff_jjuelichcoreshell(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((MCORE < 0.0), param, "Mcore(%lg) < 0",MCORE); // modify condition to your needs
	SASFIT_CHECK_COND1((MBRUSH < 0.0), param, "Mbrush(%lg) < 0",MBRUSH); // modify condition to your needs
	SASFIT_CHECK_COND1((RHO_CORE < 0.0), param, "rho_core(%lg) < 0",RHO_CORE); // modify condition to your needs
	SASFIT_CHECK_COND1((RHO_BRUSH < 0.0), param, "rho_brush(%lg) < 0",RHO_BRUSH); // modify condition to your needs
	SASFIT_CHECK_COND1((B_CORE < 0.0), param, "b_core(%lg) < 0",B_CORE); // modify condition to your needs
	SASFIT_CHECK_COND1((B_BRUSH < 0.0), param, "b_brush(%lg) < 0",B_BRUSH); // modify condition to your needs
	SASFIT_CHECK_COND1((NAGG < 0.0), param, "Nagg(%lg) < 0",NAGG); // modify condition to your needs
	SASFIT_CHECK_COND1((D1_PLUS < 0.0), param, "d1_plus(%lg) < 0",D1_PLUS); // modify condition to your needs
	SASFIT_CHECK_COND1((PART23 < 0.0), param, "part23(%lg) < 0",PART23); // modify condition to your needs
	SASFIT_CHECK_COND1((D2_PLUS < 0.0), param, "d2_plus(%lg) < 0",D2_PLUS); // modify condition to your needs
	SASFIT_CHECK_COND1((D3_PLUS < 0.0), param, "d3_plus(%lg) < 0",D3_PLUS); // modify condition to your needs
	SASFIT_CHECK_COND1((SIGMA1 < 0.0), param, "sigma1(%lg) < 0",SIGMA1); // modify condition to your needs
	SASFIT_CHECK_COND1((SIGMA2 < 0.0), param, "sigma2(%lg) < 0",SIGMA2); // modify condition to your needs
	SASFIT_CHECK_COND1((SIGMA3 < 0.0), param, "sigma3(%lg) < 0",SIGMA3); // modify condition to your needs
	SASFIT_CHECK_COND1((PARTSTAR < 0.0), param, "partstar(%lg) < 0",PARTSTAR); // modify condition to your needs
	SASFIT_CHECK_COND1((GAMMA < 0.0), param, "gamma(%lg) < 0",GAMMA); // modify condition to your needs
	SASFIT_CHECK_COND1((LPARABOL < 0.0), param, "lparabol(%lg) < 0",LPARABOL); // modify condition to your needs
	SASFIT_CHECK_COND1((F_BRUSH < 0.0), param, "f_brush(%lg) < 0",F_BRUSH); // modify condition to your needs
	SASFIT_CHECK_COND1((F_CORE < 0.0), param, "f_core(%lg) < 0",F_CORE); // modify condition to your needs

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_juelichcoreshell_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_ff_juelichcoreshell_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

// JuelichCoreShell (JCS) subParam layout:
typedef enum {
	JCS_Q		= 0,
	JCS_R1		= 1,
	JCS_R1_OLD	= 2,
	JCS_SIGMA1	= 3,
	JCS_SIGMA1_OLD	= 4,
	JCS_TOT_SWELL1	= 5,
	JCS_V1I		= 6,
	JCS_PHI01	= 7,
	JCS_R2		= 8,
	JCS_R2_OLD	= 9,
	JCS_SIGMA2	= 10,
	JCS_SIGMA2_OLD	= 11,
	JCS_TOT_SWELL2	= 12,
	JCS_V2I		= 13,
	JCS_PHI02	= 14,
	JCS_R3		= 15,
	JCS_R3_OLD	= 16,
	JCS_SIGMA3	= 17,
	JCS_SIGMA3_OLD	= 18,
	JCS_TOT_SWELL3	= 19,
	JCS_V3I		= 20,
	JCS_PHI03	= 21,
	JCS_GAMMA	= 22,
	JCS_FSTAR	= 23,
	JCS_LPARABOL	= 24
} jcs_param_layout_t;


static const scalar jcs_xunit = 1e-8;

/*
 * ---> fermi Funkktion 1/(1+exp(x/sigma)  ROBUST
 *
 *  implicit none
 *  double precision x, sigma, arg
 */
scalar fermi(scalar x, scalar sigma)
{
	scalar argu;
	if (sigma > 0.0)
	{
        	argu = x/sigma;
        	if (argu < -100.0)
		{
			return 1.0;
		}
		if (argu > 100.0)
		{
			return 0.0;
		}
		return 1.0/(1.0+exp(argu));
	} else {
		if (x < 0.0) {
			return 1.0;
		} else {
			return 0.0;
		}
	}
}

scalar parabol_brush(scalar x, scalar l)
{
	scalar yl;

	if (l < 1e-4*jcs_xunit)
	{
		yl = 1e-4*jcs_xunit;
	}
	else {
		yl = l;
	}

	if (x < 0.0)
	{
		return 1.0;
	}
	if (x > yl)
	{
		return 0.0;
	}

	return 1.0 - (x/yl)*(x/yl);
}

// for the first shot I do this with switches, may be improved later ..
scalar plugin_vol_ker(scalar r, sasfit_param * param)
{
	scalar phi, r_para;
	int idx_dist;

    	SASFIT_ASSERT_PTR( param );

	idx_dist = (JCS_R2 - JCS_R1) * (param->kernelSelector-1);

	switch( param->kernelSelector )	// represent the shell to calculate
	{
		case 1:
			phi = param->p[JCS_PHI01] * fermi( r - param->p[JCS_R1], param->p[JCS_SIGMA1] );
			break;
		case 2:
			// Volumefraction 2.Schale
			// Modellierung 2.Schale : hier einfache Fermi-Fktn
    			phi = param->p[JCS_PHI02] * (1.0 - fermi( r - param->p[JCS_R1], param->p[JCS_SIGMA1] )) *
							   fermi( r - param->p[JCS_R2], param->p[JCS_SIGMA2] );
			break;
		case 3:
			// Modellierung 3.Schale :  Fermi-Fktn * Parabol oder star
			if(r > param->p[JCS_R2])
			{
				r_para = r;
			} else
			{
				r_para = param->p[JCS_R2];
			}
			phi = param->p[JCS_PHI03] * (1.0 - fermi( r - param->p[JCS_R2], param->p[JCS_SIGMA2] )) *
							   fermi( r - param->p[JCS_R3], param->p[JCS_SIGMA3] )  *
				((1.0 - param->p[JCS_FSTAR]) * parabol_brush( r_para - param->p[JCS_R2], param->p[JCS_LPARABOL] ) +
				 param->p[JCS_FSTAR]/pow(r_para/jcs_xunit+(r_para>0 ? 0 : 1e-100), param->p[JCS_GAMMA]));
			break;
		default:
			SASFIT_ERR_UNKNOWN_KERNEL(param, "shell number");
			break;
	}

	if ( param->kernelSubSelector == JCS_INT_V || param->p[JCS_Q] == 0.0 )
	{
		return 4.0*M_PI * phi * r*r;	// former vol_kerX() function
	}
	else if ( param->kernelSubSelector == JCS_INT_FF && param->p[JCS_Q] != 0.0 )
	{
		// Streuintegration Schale X
		return 4.0*M_PI * phi * sin(param->p[JCS_Q]*r) * r/param->p[JCS_Q];
	}
	else
	{
		SASFIT_ERR_UNKNOWN_KERNEL(param, "integration method");
	}
}

scalar jcs_integrate(sasfit_param *param, int shellNr, int intType)
{
	const scalar VerrorVal = 1.007;
	scalar res, R, sigma;
	int idx_dist;

	SASFIT_ASSERT_PTR( param );
	SASFIT_CHECK_COND1((shellNr < 1 || shellNr > 3), param, "Invalid shell number (%d)",shellNr);
	SASFIT_CHECK_COND1((intType != JCS_INT_V && intType != JCS_INT_FF), param, "Invalid integration type (%d)",intType);

	param->kernelSelector = shellNr;
	param->kernelSubSelector = intType;

	// used to add to array indices to get the correct entry
	// JCS_R1+distance = JCS_R2 ..
	idx_dist = (JCS_R2 - JCS_R1) * (param->kernelSelector-1);

	R 	= param->p[ JCS_R1 + idx_dist ];
	sigma 	= param->p[ JCS_SIGMA1 + idx_dist ];

	res = sasfit_integrate(0.0, R+sigma*10.0, plugin_vol_ker, param);
	if ( param->errStatus != FALSE )
	{
		if ( param->kernelSubSelector == JCS_INT_V )	return VerrorVal;
		else 						return 0.0;	// t == JCS_INT_FF
	}

	return res;
}

/*
'amplitu'	concentration factor
'mcore'		molecular weight core (g/mol)
'mbrush'	molecular weight brush (g/mol)
'rhocore'	density core matter (g/cm**3)
'rhobrsh'	density brush matter (g/cm**3)
'bcore'		scattering length density core mat. (cm**-2)
'bbrush'	scattering length density brush mat.(cm**-2)
'naggr'		aggregation number real to be fitted
'd1+'		extra radius of shell1=core (compared to compact)
'part23'	relative distribution of shell amount in shell2:shell3 (0..inf)
'd2+'		extra radius of shell2 (compared to compact)
'd3+'		extra radius of shell3 (compared to compact)
'sigma1'	core smearing
'sigma2'	shell2 smearing
'sigma3'	shell3 smearing
'partstar'	relative distribution of parbolic:starlike profile in shell3;
		I usually put a very high value in order to consider only a star-like profile.
'gamma'		star-like exponent is 4/3, const=0
'lparabol'	thickness of parabolic brush (must fit in shell3!) I put it = 0
'f_brush'	scattering length density correction factor brush
'f_core'	scattering length density correction factor core
'rhosolv'   scattering length density of solvent

  dbc = bcore-rhosolv*(1-f_core)
  dbb = bbrush - rhosolv*(1-f_brush)

  dSigma/dOmega = amplitu/(VC+VB)*(dbc*F1+dbb*(F1+F2))^2
 */
scalar sasfit_ff_juelichcoreshell(scalar q, sasfit_param * param)
{
	scalar amplitu, mcore, mbrush,
		   rhocore, rhobrsh, bcore, bbrush,
		   naggr, d1p, part23, d2p, d3p, sigma1, sigma2, sigma3, partstar, gamma, lparabol, f_brush, f_core, rhosolv;
	scalar dbc, dbb,Nav,VC,RC0,R1,VCS,swellC,VB,fshell2,fshell3;
	scalar VS2, RC2, R2, VS2S, swellS2;
	scalar VS3, RC3, R3, VS3S, swellS3;
	scalar fstar;
	static scalar V_1, V_2, V_3;
	scalar f_1, f_2, f_3;
	scalar delta, alpha, a1,a2,a3,afactor;
//	const scalar unit = 1e-7;
	static scalar first=-1;
	sasfit_param subParam;

	SASFIT_ASSERT_PTR(param); // assert pointer param is valid
/*
	SASFIT_CHECK_COND1((q < 0.0), param, "q(%lg) < 0",q);
	SASFIT_CHECK_COND1((MCORE < 0.0), param, "Mcore(%lg) < 0",MCORE); // modify condition to your needs
	SASFIT_CHECK_COND1((MBRUSH < 0.0), param, "Mbrush(%lg) < 0",MBRUSH); // modify condition to your needs
	SASFIT_CHECK_COND1((RHO_CORE < 0.0), param, "rho_core(%lg) < 0",RHO_CORE); // modify condition to your needs
	SASFIT_CHECK_COND1((RHO_BRUSH < 0.0), param, "rho_brush(%lg) < 0",RHO_BRUSH); // modify condition to your needs
	SASFIT_CHECK_COND1((B_CORE < 0.0), param, "b_core(%lg) < 0",B_CORE); // modify condition to your needs
	SASFIT_CHECK_COND1((B_BRUSH < 0.0), param, "b_brush(%lg) < 0",B_BRUSH); // modify condition to your needs
	SASFIT_CHECK_COND1((NAGG < 0.0), param, "Nagg(%lg) < 0",NAGG); // modify condition to your needs
	SASFIT_CHECK_COND1((D1_PLUS < 0.0), param, "d1_plus(%lg) < 0",D1_PLUS); // modify condition to your needs
	SASFIT_CHECK_COND1((PART23 < 0.0), param, "part23(%lg) < 0",PART23); // modify condition to your needs
	SASFIT_CHECK_COND1((D2_PLUS < 0.0), param, "d2_plus(%lg) < 0",D2_PLUS); // modify condition to your needs
	SASFIT_CHECK_COND1((D3_PLUS < 0.0), param, "d3_plus(%lg) < 0",D3_PLUS); // modify condition to your needs
	SASFIT_CHECK_COND1((SIGMA1 < 0.0), param, "sigma1(%lg) < 0",SIGMA1); // modify condition to your needs
	SASFIT_CHECK_COND1((SIGMA2 < 0.0), param, "sigma2(%lg) < 0",SIGMA2); // modify condition to your needs
	SASFIT_CHECK_COND1((SIGMA3 < 0.0), param, "sigma3(%lg) < 0",SIGMA3); // modify condition to your needs
	SASFIT_CHECK_COND1((PARTSTAR < 0.0), param, "partstar(%lg) < 0",PARTSTAR); // modify condition to your needs
	SASFIT_CHECK_COND1((GAMMA < 0.0), param, "gamma(%lg) < 0",GAMMA); // modify condition to your needs
	SASFIT_CHECK_COND1((LPARABOL < 0.0), param, "lparabol(%lg) < 0",LPARABOL); // modify condition to your needs
	SASFIT_CHECK_COND1((F_BRUSH < 0.0), param, "f_brush(%lg) < 0",F_BRUSH); // modify condition to your needs
	SASFIT_CHECK_COND1((F_CORE < 0.0), param, "f_core(%lg) < 0",F_CORE); // modify condition to your needs
	SASFIT_CHECK_COND1((RHOSOLV < 0.0), param, "rhosolv(%lg) < 0",RHOSOLV); // modify condition to your needs
*/
	amplitu		= param->p[0];
	mcore		= fabs(	param->p[1] );
	mbrush		= fabs(	param->p[2] );
	rhocore		= fabs(	param->p[3] );
	rhobrsh		= fabs(	param->p[4] );
	bcore		= 	param->p[5];
	bbrush		= 	param->p[6];
	naggr		= fabs(	param->p[7] );
	d1p			= fabs(	param->p[8] )*jcs_xunit;
	part23		= param->p[9];
	d2p			= fabs(	param->p[10] )*jcs_xunit;
	d3p			= fabs(	param->p[11] )*jcs_xunit;
	sigma1		= fabs(	param->p[12] )*jcs_xunit;
	sigma2		= fabs(	param->p[13] )*jcs_xunit;
	sigma3		= fabs(	param->p[14] )*jcs_xunit;
	partstar	= fabs(	param->p[15] );
	gamma		= 	param->p[16];
	lparabol	= fabs(	param->p[17] )*jcs_xunit;
	f_brush		=	param->p[18];
	f_core		= 	param->p[19];
	rhosolv		= 	param->p[20];
	Q			= q/jcs_xunit;

	sasfit_init_param( &subParam );

	if (first != 1.123779053423241)
	{
		first = 1.123779053423241;
		/*
		params.R1_old = -1.0;
		params.sigma1_old = -1.0;
		params.R2_old = -1.0;
		params.sigma2_old = -1.0;
		params.R3_old = -1.0;
		params.sigma3_old = -1.0;
		*/
		subParam.p[JCS_R1_OLD] 		= -1.0;
		subParam.p[JCS_SIGMA1_OLD] 	= -1.0;
		subParam.p[JCS_R2_OLD] 		= -1.0;
		subParam.p[JCS_SIGMA2_OLD] 	= -1.0;
		subParam.p[JCS_R3_OLD] 		= -1.0;
		subParam.p[JCS_SIGMA3_OLD] 	= -1.0;
	}

	SASFIT_CHECK_COND1((Q < 0.0), param, "q(%lg) < 0",Q);

	dbc = bcore-rhosolv*(1.-f_core);
	dbb = bbrush-rhosolv*(1.-f_brush);
	Nav = 6.022045E23;

	VC = naggr*mcore/(rhocore*Nav);
	RC0 = pow(3.0/(4.0*M_PI)*VC, 1.0/3.0);
	R1 = RC0 + d1p;
	VCS = 4.0/3.0 * M_PI * R1*R1*R1;
	swellC = ((VC == 0.0) ?  1.0 : VCS/VC);

	VB = naggr*mbrush/(rhobrsh*Nav);
	fshell2 = part23/(1.0+part23)+1e-5;
	fshell3 = 1.0-fshell2;

	VS2 = fshell2*VB;
	RC2 = pow(3.0/(4.0*M_PI)*(VCS+VS2), 1.0/3.0);
	R2 = RC2+d2p;
	VS2S = 4.0/3.0 * M_PI *R2*R2*R2;
	swellS2 = ((VS2 == 0.0) ?  1.0 : (VS2S-VCS)/VS2);

	VS3 = fshell3*VB;
	RC3 = pow(3.0/(4.0*M_PI)*(VS3+VS2S), 1.0/3.0);
	R3 = RC3+d3p;
	VS3S = 4.0/3.0 * M_PI *R3*R3*R3;
	swellS3 = ((VS3 == 0.0) ?  1.0 : (VS3S-VS2S)/VS3) ;

	fstar = 2.0*atan(fabs(partstar))/M_PI;

	subParam.p[JCS_R1]	= R1;
	subParam.p[JCS_R2] 	= R2;
	subParam.p[JCS_R3] 	= R3;
	subParam.p[JCS_SIGMA1] 	= sigma1;
	subParam.p[JCS_SIGMA2] 	= sigma2;
	subParam.p[JCS_SIGMA3] 	= sigma3;
	subParam.p[JCS_Q]	= Q;

// as phi01, phi02, phi03 are anyway canceld due to the normalization F_i/V_i
// they could be set here to 1, but I will leave it as in the original
// fortran version from IFF

//   params.phi01 = 1.;
//   params.phi02 = 1.;
//   params.phi03 = 1.;

	subParam.p[JCS_TOT_SWELL1] = swellC;
	subParam.p[JCS_TOT_SWELL2] = swellS2;
	subParam.p[JCS_TOT_SWELL3] = swellS3;

	if (swellC  == 0.0) {
		subParam.p[JCS_PHI01] = 0.0;
	} else {
		subParam.p[JCS_PHI01] = 1.0/swellC;
	}
	if (swellS2 == 0.0) {
		subParam.p[JCS_PHI02] = 0.0;
	} else {
		subParam.p[JCS_PHI02] = 1.0/swellS2;
	}
	if (swellS3 == 0.0) {
		subParam.p[JCS_PHI03] = 0.0;
	} else {
		subParam.p[JCS_PHI03] = 1.0/swellS3;
	}

	subParam.p[JCS_GAMMA] 		= gamma;
	subParam.p[JCS_FSTAR] 		= fstar;
	subParam.p[JCS_LPARABOL] 	= lparabol;

	if ((subParam.p[JCS_R1] 	!= subParam.p[JCS_R1_OLD]) ||
	    (subParam.p[JCS_SIGMA1] 	!= subParam.p[JCS_SIGMA1_OLD]))
	{
		V_1 = jcs_integrate( &subParam, 1, JCS_INT_V );
		SASFIT_CHECK_SUB_ERR(param, subParam);

		V_2 = jcs_integrate( &subParam, 2, JCS_INT_V );
		SASFIT_CHECK_SUB_ERR(param, subParam);

		subParam.p[JCS_R1_OLD] 		= subParam.p[JCS_R1];
		subParam.p[JCS_SIGMA1_OLD] 	= subParam.p[JCS_SIGMA1];
		subParam.p[JCS_R2_OLD] 		= subParam.p[JCS_R2];
		subParam.p[JCS_SIGMA2_OLD] 	= subParam.p[JCS_SIGMA2];
	}
	if ((subParam.p[JCS_R2] 	!= subParam.p[JCS_R2_OLD]) ||
	    (subParam.p[JCS_SIGMA2] 	!= subParam.p[JCS_SIGMA2_OLD]))
	{
		V_2 = jcs_integrate( &subParam, 2, JCS_INT_V );
		SASFIT_CHECK_SUB_ERR(param, subParam);

		V_3 = jcs_integrate( &subParam, 3, JCS_INT_V );
		SASFIT_CHECK_SUB_ERR(param, subParam);

		subParam.p[JCS_R2_OLD] 		= subParam.p[JCS_R2];
		subParam.p[JCS_SIGMA2_OLD] 	= subParam.p[JCS_SIGMA2];
		subParam.p[JCS_R3_OLD] 		= subParam.p[JCS_R3];
		subParam.p[JCS_SIGMA3_OLD] 	= subParam.p[JCS_SIGMA3];
	}
	if ((subParam.p[JCS_R3] 	!= subParam.p[JCS_R3_OLD]) ||
	    (subParam.p[JCS_SIGMA3] 	!= subParam.p[JCS_SIGMA3_OLD]))
	{
		V_3 = jcs_integrate( &subParam, 3, JCS_INT_V );
		SASFIT_CHECK_SUB_ERR(param, subParam);

		subParam.p[JCS_R3_OLD] 		= subParam.p[JCS_R3];
		subParam.p[JCS_SIGMA3_OLD] 	= subParam.p[JCS_SIGMA3];
	}

	subParam.p[JCS_V1I] = V_1;
	subParam.p[JCS_V2I] = V_2;
	subParam.p[JCS_V3I] = V_3;

//! erstens: erzwinge Kompaktvolumenvehaeltnis 2te/3te Schale
	if (V_2 > 0.0)
	{
		delta = part23 * V_3 / V_2;
	} else
	{
		delta = 0.0;
	}
//! zweitens: erzwinge Kompaktvolumen der gesamten Schale
	alpha = VB / (V_3 * (1. + part23) );
//! reskaliere die Schalen, so dass die Volumina konserviert sind
	a1 = VC / V_1;
	a2 = delta * alpha;
	a3 = alpha;
	dbc = bcore-rhosolv * (1.0 - f_core);
	dbb = bbrush - rhosolv * (1.0 - f_brush);
	f_1 = a1 * jcs_integrate( &subParam, 1, JCS_INT_FF );
	SASFIT_CHECK_SUB_ERR(param, subParam);

	f_2 = a2 * jcs_integrate( &subParam, 2, JCS_INT_FF );
	SASFIT_CHECK_SUB_ERR(param, subParam);

	f_3 = a3 * jcs_integrate( &subParam, 3, JCS_INT_FF );
	SASFIT_CHECK_SUB_ERR(param, subParam);

	afactor = amplitu / (VC + VB);

/*
   if ((params.R1 != params.R1_old) || (params.sigma1 != params.sigma1_old))  {
	   IFFprint(params);
	   params.R1_old = params.R1;
	   params.sigma1_old = params.sigma1;
	   params.R2_old = params.R2;
	   params.sigma2_old = params.sigma2;
   }
*/

	return afactor*pow(dbc*f_1+dbb*(f_2+f_3),2.0);
}
