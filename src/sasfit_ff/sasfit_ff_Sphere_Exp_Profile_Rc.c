/*
 * src/sasfit_ff/sasfit_ff_Sphere_Exp_Profile_Rc.c
 *
 * Copyright (c) 2008-2009, Paul Scherrer Institute (PSI)
 *
 * This file is part of SASfit.
 *
 * SASfit is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SASfit is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with SASfit.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 *   Ingo Bressler (ingo@cs.tu-berlin.de)
 */

#include <gsl/gsl_math.h>
#include <gsl/gsl_sf.h>
#include "include/sasfit_ff_utils.h"

/*
scalar sasfit_ff_Sphere_Exp1 (Tcl_Interp *interp,
	scalar q,
	scalar R1,
	scalar DR,
	  scalar x_out,
	  scalar alpha, // alpha < 0
	bool  *error) 
*/
scalar sasfit_ff_Sphere_Exp1(scalar q, sasfit_param * param)
{
	scalar R1, DR, x_out, alpha;
	scalar t1,t2,t3,t4,t5,t6,t7,t8,t9,t11,t12,t16,t17,t18,t22,t23,t24,t25,t26,
		t27,t28,t31,t32,t33,t34,t35,t39,t40,t44,t45,t46,t48,t49,t50,t59,t60,
		t63,t69,t70,t71,t72,t73,t76,t77,t80,t81,t92,t93,t94,t98,t99,t106,t107,
		t110,t113,t117,t118,t119,t122,t123,t126,t129,t134,t138,t141,t142,t143,
		t145,t146,t149,t150,t154,t155,t165,t167,t168,t175,t178,t179,t180,t185,
		t188,t189,t193,t194,t200,t202,t204,t210,t216,t222,t230,t235,t241,t248,
		t252,t260,t262,t263,t270,t271,t275,t298,t300,t315,t320,t325,t335,t349,
		t357,t360,t366,t386,t391;

	SASFIT_ASSERT_PTR(param);
	R1	= param->p[0];
	DR	= param->p[8];
	x_out	= param->p[9];
	alpha	= param->p[7];

	t1 = DR*DR;
	t2 = t1*t1;
	t3 = t2*t1;
	t4 = t3*x_out;
	t5 = q*q;
	t6 = t5*t5;
	t7 = t6*t5;
	t8 = q*R1;
	t9 = cos(t8);
	t11 = q*DR;
	t12 = sin(t11);
	t16 = sin(t8);
	t17 = t7*t16;
	t18 = cos(t11);
	t22 = t1*DR;
	t23 = x_out*t22;
	t24 = alpha*alpha;
	t25 = t23*t24;
	t26 = t9*t18;
	t27 = t5*q;
	t28 = t26*t27;
	t31 = t6*q;
	t32 = t2*DR;
	t33 = t31*t32;
	t34 = t33*x_out;
	t35 = alpha*t16;
	t39 = t7*x_out;
	t40 = t39*R1;
	t44 = t24*t24;
	t45 = t44*t24;
	t46 = t45*t9;
	t48 = exp(alpha);
	t49 = t48*x_out;
	t50 = t49*t27;
	t59 = t24*alpha;
	t60 = t23*t59;
	t63 = t45*t16;
	t69 = 2.0*t4*t7*t9*t12+2.0*t4*t17*t18-6.0*t25*t28+4.0*t34*t35*t12+t40*t32
		*t9*t12-t46*t12+6.0*t50*t22*t9*t24-2.0*t33*x_out*t16*t12-4.0*t60*t28-t63*t18+
		2.0*t33*x_out*t9*t18;
	t70 = t6*x_out;
	t71 = t70*alpha;
	t72 = t2*t9;
	t73 = t72*t12;
	t76 = t2*t16;
	t77 = t76*t18;
	t80 = t16*t12;
	t81 = t80*t27;
	t92 = x_out*t44*t5;
	t93 = t1*t9;
	t94 = t93*t12;
	t98 = t31*t2*x_out;
	t99 = alpha*R1;
	t106 = t1*t16;
	t107 = t106*t18;
	t110 = 6.0*t71*t73+6.0*t71*t77+4.0*t60*t81+6.0*t25*t81-4.0*t34*alpha*t9*
		t18+t17*t3-2.0*t92*t94-2.0*t98*t99*t26+t40*t32*t16*t18-2.0*t92*t107+t63;
	t113 = R1*t44;
	t117 = t27*t22;
	t118 = t44*t16;
	t119 = t118*t12;
	t122 = x_out*t27;
	t123 = t122*t22;
	t126 = t24*t9*t18;
	t129 = t70*t2;
	t134 = t59*t16;
	t138 = t44*t5;
	t141 = t6*t27;
	t142 = t2*t22;
	t143 = t141*t142;
	t145 = t44*t9;
	t146 = t145*t18;
	t149 = t24*t16;
	t150 = t149*t12;
	t154 = -3.0*t27*t9*t113*t1-3.0*t117*t119+t123*t119+3.0*t33*t126-2.0*t129*
		t59*t9*t12-2.0*t129*t134*t18-3.0*t138*t94-t143*t80+3.0*t117*t146-3.0*t33*t150-t123*t146;
	t155 = t24*t6;
	t165 = t63*t12;
	t167 = t7*t3;
	t168 = t9*t12;
	t175 = t122*t1;
	t178 = x_out*t5;
	t179 = t44*alpha;
	t180 = t178*t179;
	t185 = -3.0*t155*t73-3.0*t155*t77+3.0*t149*t6*t2-3.0*t138*t107-t8*t165-
		t167*t168+t48*DR*x_out*t5*t16*t113+t175*t113*t80-t180*t107+t143*t26-t175*t113*t26;
	t188 = x_out*R1;
	t189 = t188*DR;
	t193 = t141*R1;
	t194 = t3*t9;
	t200 = t16*t18;
	t202 = t46*t18;
	t204 = t3*t16;
	t210 = t39*alpha;
	t216 = x_out*t141;
	t222 = -t189*t118*t18*t5+t193*t194*t18+2.0*t98*t99*t80-t167*t200+t8*t202-
		t193*t204*t12-q*t9*R1*t45-t210*t194*t12-t141*t9*R1*t3-t216*t142*t9*t18-t210*
		t204*t18;
	t230 = R1*t24;
	t235 = t59*x_out*t1;
	t241 = t31*t9;
	t248 = t59*R1;
	t252 = R1*t32;
	t260 = 3.0*t118*t5*t1-t180*t94+t216*t142*t16*t12-2.0*t98*t230*t26-2.0*
		t235*t168*t5+2.0*t48*t2*x_out*t241*t99-2.0*t235*t200*t5+2.0*t50*t93*t248-t210*
		t252*t200-3.0*t241*t230*t2-t210*t252*t168;
	t262 = t70*R1;
	t263 = t22*t59;
	t270 = t178*R1;
	t271 = DR*t179;
	t275 = t31*R1*t24;
	t298 = -2.0*t262*t263*t168-2.0*t262*t263*t200-t270*t271*t200+3.0*t275*t72
		*t18-3.0*t275*t76*t12-t270*t271*t168+2.0*t98*t230*t80-6.0*t49*t6*t35*t2-2.0*t34
		*t126-t189*t145*t12*t5+2.0*t34*t150;
	t300 = t27*R1*t44;
	t315 = R1*t16;
	t320 = R1*t9;
	t325 = t216*t3;
	t335 = 3.0*t300*t93*t18-3.0*t300*t106*t12-t49*t16*t7*R1*t32+2.0*t49*t5*
		t134*t1+2.0*t235*t315*t12*t27-2.0*t235*t320*t18*t27-t325*t320*t18+t325*t315*t12
		+t11*t202-t11*t165-2.0*t49*t33*t9;
	t349 = R1*R1;
	t357 = t24*x_out;
	t360 = x_out*t1;
	t366 = x_out*DR;
	t386 = -3.0*t349*t24*x_out-3.0*t59*t349*x_out-3.0*t235-9.0*t357*t1-18.0*
		t360*alpha-18.0*t360+18.0*t360*t48-6.0*t248*t366-12.0*t230*t366-12.0*t188*DR*
		alpha+3.0*t48*t349*t357+12.0*t48*R1*t366*alpha-t44*t1-3.0*t44*t349-3.0*t113*DR;
	t391 = 3.0*(t69+t110+t154+t185+t222+t260+t298+t335)/t27/(t45+3.0*t138*t1+
		3.0*t155*t2+t167)/DR/t386*t44;

	return t391;
}

/*
	float F_Exp2 (Tcl_Interp *interp,
	float q,
	float R1,
	float DR,
	  float x_out,
	  float alpha, // alpha >= 0
	bool  *error) {
*/
scalar sasfit_ff_Sphere_Exp2(scalar q, sasfit_param * param)
{
	scalar R1, DR, x_out, alpha;
	scalar t1,t2,t3,t4,t5,t6,t8,t9,t10,t11,t14,t15,t16,t17,t18,t19,t21,t22,
		t23,t27,t32,t34,t37,t40,t44,t45,t46,t47,t50,t51,t52,t53,t56,t57,
		t59,t60,t64,t65,t66,t70,t74,t78,t81,t83,t84,t86,t88,t89,t90,t91,
		t92,t95,t96,t97,t98,t99,t102,t103,t107,t108,t112,t113,t114,t120,
		t121,t124,t132,t134,t137,t138,t139,t140,t142,t147,t151,t152,t157,
		t158,t167,t170,t171,t174,t177,t187,t192,t193,t204,t208,t218,t232,
		t235,t236,t240,t241,t242,t245,t246,t247,t258,t260,t261,t262,t267,
		t269,t270,t277,t278,t279,t282,t283,t288,t298,t305,t310,t319,t331,
		t335,t341,t364,t379,t391,t392,t399,t403,t436,t441;

	SASFIT_ASSERT_PTR(param);
	R1	= param->p[0];
	DR	= param->p[8];
	x_out	= param->p[9];
	alpha	= param->p[7];

	t1 = x_out*R1;
	t2 = t1*DR;
	t3 = alpha*alpha;
	t4 = t3*t3;
	t5 = q*R1;
	t6 = cos(t5);
	t8 = q*DR;
	t9 = sin(t8);
	t10 = q*q;
	t11 = t9*t10;
	t14 = exp(alpha);
	t15 = t14*x_out;
	t16 = sin(t5);
	t17 = t15*t16;
	t18 = t10*t10;
	t19 = t18*t10;
	t21 = DR*DR;
	t22 = t21*t21;
	t23 = t22*DR;
	t27 = t3*alpha;
	t32 = t14*t16;
	t34 = t3*t18*t22;
	t37 = t18*q;
	t40 = R1*t3;
	t44 = t22*t21;
	t45 = t19*t44;
	t46 = t6*t9;
	t47 = t46*t14;
	t50 = t37*R1*t3;
	t51 = t22*t16;
	t52 = t9*t14;
	t53 = t51*t52;
	t56 = t21*x_out;
	t57 = t56*t27;
	t59 = t10*q;
	t60 = t9*t59;
	t64 = R1*t6;
	t65 = cos(t8);
	t66 = t65*t59;
	t70 = t37*t23;
	t74 = t44*x_out;
	t78 = t19*t16;
	t81 = -t2*t4*t6*t11-t17*t19*R1*t23+2.0*t15*t10*t16*t27*t21+3.0*t32*t34
		-3.0*t14*t37*t6*t40*t22-t45*t47-3.0*t50*t53+2.0*t57*R1*t16*t60-2.0*t57*t64*t66
		-2.0*t15*t70*t6+t74*t19*t6*t9+t74*t78*t65;
	t83 = t4*t3;
	t84 = t64*t83;
	t86 = x_out*t83;
	t88 = t21*DR;
	t89 = x_out*t88;
	t90 = t89*t3;
	t91 = t6*t65;
	t92 = t91*t59;
	t95 = R1*t4;
	t96 = t95*t21;
	t97 = t16*t9;
	t98 = t59*t14;
	t99 = t97*t98;
	t102 = t70*x_out;
	t103 = alpha*t16;
	t107 = t15*t59;
	t108 = t88*t6;
	t112 = t22*t6;
	t113 = t65*t14;
	t114 = t112*t113;
	t120 = t19*x_out*R1;
	t121 = t23*t6;
	t124 = t88*t4;
	t132 = t15*q*t84+t86*t47-6.0*t90*t92-3.0*t96*t99+2.0*t102*t103*t9-2.0*
		t107*t108*t27+3.0*t50*t114-3.0*t17*t34+t120*t121*t9+3.0*t124*t6*t66*t14-t83*t6*
		t52+t32*t45;
	t134 = t16*t83;
	t137 = t18*t59;
	t138 = t22*t88;
	t139 = t137*t138;
	t140 = t91*t14;
	t142 = t14*q;
	t147 = t15*t37;
	t151 = t16*t65;
	t152 = t151*t14;
	t157 = t70*t3;
	t158 = t97*t14;
	t167 = t89*t27;
	t170 = -t15*t134-t134*t113+t139*t140-t142*t84-t14*t137*t64*t44-2.0*t147*
		t121*alpha+t86*t152+6.0*t107*t108*t3-3.0*t157*t158+3.0*t157*t140-2.0*t70*x_out*
		t16*t9-2.0*t167*t92;
	t171 = DR*t83;
	t174 = t65*q*t14;
	t177 = t137*R1*t44;
	t187 = t4*t21;
	t192 = t18*x_out;
	t193 = t192*alpha;
	t204 = t97*t59;
	t208 = t37*x_out*t40;
	t218 = t171*t6*t174-t177*t158-3.0*t102*t3*t6*t113+2.0*t70*x_out*t6*t65
		-3.0*t187*t6*t11*t14+6.0*t193*t112*t9+6.0*t193*t51*t65+3.0*t102*t3*t16*t52+2.0*
		t167*t204-3.0*t208*t114+6.0*t90*t204-2.0*t102*alpha*t6*t65-t45*t152;
	t232 = t9*q*t14;
	t235 = t37*t22*x_out;
	t236 = alpha*R1;
	t240 = x_out*t4;
	t241 = t240*t10;
	t242 = t21*t6;
	t245 = x_out*DR;
	t246 = t245*t83;
	t247 = t91*t142;
	t258 = t45*x_out;
	t260 = t32*t83-3.0*t34*t47-3.0*t34*t152+t120*t23*t16*t65-R1*t83*t16*t232
		-2.0*t235*t236*t91-t241*t242*t9-t246*t247-t171*t16*t232+2.0*t107*t64*t187-3.0*
		t124*t16*t60*t14+t258*t152;
	t261 = t137*x_out;
	t262 = t261*t138;
	t267 = t97*t142;
	t269 = t89*t4;
	t270 = t91*t98;
	t277 = t14*DR;
	t278 = t277*x_out;
	t279 = t10*t16;
	t282 = t240*t21;
	t283 = t10*t14;
	t288 = t65*t10;
	t298 = t1*t187;
	t305 = t262*t158-t241*t21*t16*t65+t246*t267-3.0*t269*t270+3.0*t269*t99+
		3.0*t96*t270+t278*t279*t95+3.0*t282*t151*t283-t2*t4*t16*t288+2.0*t235*t236*t97-
		t278*t279*R1*t4*alpha-3.0*t298*t270-t14*t23*x_out*t78*t236;
	t310 = R1*t27;
	t319 = t192*t3;
	t331 = t261*R1;
	t335 = t1*t83;
	t341 = -2.0*t14*t88*x_out*t18*t16*t310+3.0*t208*t53+t147*t64*t3*t22+3.0*
		t319*t112*t52+3.0*t319*t51*t113+3.0*t282*t46*t283-t139*t158+t258*t47+t331*t44*
		t16*t52+t335*t267-t335*t247-2.0*t57*t46*t10;
	t364 = t4*t10*t21;
	t379 = -2.0*t57*t151*t10-t331*t44*t6*t113-t262*t140+2.0*t14*t22*x_out*t37
		*t6*t236+3.0*t298*t99+2.0*t107*t242*t310-3.0*t98*t6*t96-4.0*t17*t364+3.0*t32*
		t364-6.0*t15*t18*t103*t22+t177*t140-3.0*t187*t16*t288*t14+t84*t174;
	t391 = R1*R1;
	t392 = t391*t4;
	t399 = t14*t391;
	t403 = t3*x_out;
	t436 = 3.0*t392*t15-3.0*t95*t277+t240*t21*t14-3.0*t399*t27*x_out+3.0*t399
		*t403-6.0*t15*t21*alpha-6.0*t40*t245-3.0*t391*t3*x_out-12.0*t1*DR*alpha+18.0*
		t56*t14-3.0*t392*t14-t187*t14-18.0*t56-3.0*t403*t21-12.0*t56*alpha+3.0*t95*t278
		-6.0*t14*t3*t2+12.0*t14*R1*t245*alpha;
	t441 = 3.0*(t81+t132+t170+t218+t260+t305+t341+t379)/t59/(t83+3.0*t364+3.0
		*t34+t45)/DR/t436*t4;

	return t441;
}

/*
	float F_Exp (Tcl_Interp *interp,
	float q,
	float R1,
	float DR,
	  float x_out,
	  float alpha,
	bool  *error) {
		*/
scalar sasfit_ff_Sphere_Exp(scalar q, sasfit_param * param)
{
	scalar R1, DR, x_out, alpha;
	scalar t1,t2,t3,t4,t5,t6,t7,t8,t11,t12,t14,t18,t20,t21,t24,t31,t34,t48,t60,t61,t66,t68,t80;

	SASFIT_ASSERT_PTR(param);
	R1	= param->p[0];
	DR	= param->p[8];
	x_out	= param->p[9];
	alpha	= param->p[7];

	if (alpha > 0) 
	{
		//return sasfit_ff_Sphere_Exp2(interp,q,R1,DR,x_out,alpha,error);
		return sasfit_ff_Sphere_Exp2(q, param);
	} 
	else if (alpha < 0) 
	{
		//return sasfit_ff_Sphere_Exp1(interp,q,R1,DR,x_out,alpha,error);
		return sasfit_ff_Sphere_Exp1(q, param);
	} else 
	{
		t1 = q*q;
		t2 = x_out*t1;
		t3 = t2*R1;
		t4 = R1*q;
		t5 = cos(t4);
		t6 = DR*t5;
		t7 = q*DR;
		t8 = cos(t7);
		t11 = t1*DR;
		t12 = t5*R1;
		t14 = x_out*t5;
		t18 = sin(t4);
		t20 = x_out*t18;
		t21 = sin(t7);
		t24 = DR*DR;
		t31 = DR*t18;
		t34 = t1*t24;
		t48 = x_out*q;
		t60 = -t3*t6*t8-t11*t12-2.0*t14+2.0*t14*t8+t7*t18-2.0*t20*t21-t2*t24*t5*
			t8+t2*t24*t18*t21+t3*t31*t21+t34*t5*t8+t4*t20*t8+t4*t14*t21-t7*t5*t21-t34*t18*
			t21-t7*t18*t8-t20*t4+2.0*t48*t31*t8+2.0*t48*t6*t21+t11*t12*t8-t11*R1*t18*t21;
		t61 = t1*t1;
		t66 = R1*R1;
		t68 = R1*DR;
		t80 = 12.0*t60/t61/t24/(-4.0*t24-12.0*t66-12.0*t68+3.0*t24*x_out+6.0*t66*x_out+8.0*t68*x_out);

		return t80;
	}
}


/*
float P_Sphere_Exp_Profile_Rc(Tcl_Interp *interp,
				float Q,
				  float Rc,
				float Vc,
				float Vsh,
				float rho_c,
			  float rho_sh,
			  float rho_solv,
			  float x_solv_c,
				float alpha,
				float t,
				float x_out,
				  bool  *error)
			*/
scalar sasfit_ff_Sphere_Exp_Profile_Rc(scalar q, sasfit_param * param)
{
	scalar Plocal, Psh, Fsh, Fc, Pc, V, S, Nagg, b_c, b_sh, u, tmin;
	scalar Rc, Vc, Vsh, rho_c, rho_sh, rho_solv, x_solv_c, alpha, t, x_out;

        SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 10, &Rc, &Vc, &Vsh, &rho_c, &rho_sh, &rho_solv, &x_solv_c, &alpha, &t, &x_out);

	SASFIT_CHECK_COND1((Vc <= 0.0), param, "Vc(%lg) <= 0",Vc);

	if (Rc == 0.0) return 0.0;

	V = 4./3. * M_PI * pow(Rc,3.);
	S = 4.*M_PI*Rc*Rc;

	Fc = sasfit_sphere_fc(q, Rc);
	Pc = Fc*Fc;

	tmin = t*x_solv_c;

//      Nagg = V * (1-x_solv_c) / Vc;
        Nagg = V / Vc;

        b_c  = Vc    * (rho_c  - rho_solv);
        b_sh = Vsh   * (rho_sh - rho_solv);

        if ((t == 0.0) || (b_sh==0.0)) 
	{
		Fsh = Psh = 0.0;
        } else 
	{
		//Fsh = sasfit_ff_Sphere_Exp(interp,q,Rc,t,x_out,alpha,error);
		Fsh = sasfit_ff_Sphere_Exp(q, param);
		Psh = Fsh*Fsh;
        }

//      return pow(Nagg*b_c*Fc+Nagg*b_sh*Fsh,2);

        Plocal = (2.0*gsl_sf_Si(q*tmin)/(q*tmin)-pow(sin(q*tmin*0.5)/(q*tmin*0.5),2.0));

	u = q*q*tmin*tmin;
	Plocal = 2.0*(u-1.0+exp(-u))/(u*u);

        return  Nagg*Nagg       * b_c  * b_c  * Pc +
		Nagg*(Nagg-1.0) * b_sh * b_sh * Psh * ((Nagg < 1) ?  0 : 1) +
		2.0*Nagg*Nagg   * b_c  * b_sh * Fc * Fsh +
		Nagg            * b_sh * b_sh * Plocal;
}
