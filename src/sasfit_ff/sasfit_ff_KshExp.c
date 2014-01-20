/*
 * src/sasfit_ff/sasfit_ff_KshExp.c
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
#include "include/sasfit_ff_utils.h"

scalar sasfit_kshexp1(scalar q, sasfit_param * param);
scalar sasfit_kshexp2(scalar q, sasfit_param * param);

/*
float KshExp(Tcl_Interp *interp,
			 float Q, 
			 float R1, 
			 float DR, 
			 float eta_core, 
			 float eta_shell,
		  float x_in_solvent,
		  float x_out_solvent,
		  float alpha,
		  float eta_solvent,
			 bool  *error)
 */
scalar sasfit_ff_KshExp(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param);

	if (param->p[6] < 0) // alpha < 0
	{
		 // return Kshexp1(interp,Q,R1,DR,eta_core,eta_shell,x_in_solvent,x_out_solvent,alpha,eta_solvent,error);
		 return sasfit_kshexp1(q, param);
	} else {
		 // return Kshexp2(interp,Q,R1,DR,eta_core,eta_shell,x_in_solvent,x_out_solvent,alpha,eta_solvent,error);
		 return sasfit_kshexp2(q, param);
	}
}

/*
float Kshexp1(Tcl_Interp *interp,
          float q, 
          float R1, 
          float DR, 
          float eta_c,
          float eta_sh,
		  float x_in,
		  float x_out,
		  float alpha,
		  float eta_sol,
          bool  *error)
*/
// form factor of a spherical shell with radii R1 and DR                      
//          contrast profile in shell:                                        
// alpha < 0
//               eta_sh_in : scattering length density at R1                  
//              eta_sh_out : scattering length density at R1+DR               
//                 eta_sol : scattering length density of solvent             
//                x_in_sol : amount of solvent at R1                          
//               x_out_sol : amount of solvent at R1+DR                       
//                  eta_sh : scattering length density of pure shell material 
scalar sasfit_kshexp1(scalar q, sasfit_param * param)
{
	scalar R1, DR, eta_c, eta_sh, x_in, x_out, alpha, eta_sol;
	scalar t0,t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t14,t15,t16,t17,t18,t19,t20,t21,
		t22,t23,t24,t26,t27,t28,t29,t30,t31,t32,t34,t35,t36,t37,t38,t39,t40,t41,
		t44,t45,t46,t48,t49,t50,t53,t54,t56,t57,t58,t59,t60,t63,t65,t66,t69,t70,
		t71,t72,t75,t76,t79,t81,t84,t85,t86,t87,t90,t92,t93,t94,t95,t98,t101,t102,
		t103,t107,t110,t112,t114,t115,t116,t117,t121,t124,t125,t126,t129,t130,t131,
		t134,t136,t139,t142,t144,t146,t147,t149,t150,t151,t152,t156,t157,t159,t161,
		t162,t164,t166,t167,t171,t172,t173,t176,t177,t178,t179,t182,t183,t184,t187,
		t188,t189,t190,t192,t194,t195,t197,t201,t204,t207,t208,t209,t210,t212,t215,
		t217,t218,t219,t222,t224,t225,t226,t228,t229,t230,t234,t237,t238,t239,t242,
		t244,t246,t250,t253,t254,t255,t256,t258,t261,t262,t264,t267,t270,t276,t277,
		t278,t285,t287,t290,t292,t295,t297,t298,t299,t303,t306,t312,t313,t317,t325,
		t328,t331,t334,t342,t345,t347,t350,t351,t352,t354,t355,t357,t359,t361,t367,
		t369,t372,t375,t380,t386,t387,t389,t392,t393,t396,t398,t402,t405,t408,t409,
		t414,t415,t418,t427,t435,t441,t442,t447,t453,t454,t461,t462,t465,t468,t470,
		t475,t477,t480,t481,t494,t497,t499,t504,t518,t523,t531,t532,t533,t538,t542,
		t545,t546,t550,t553,t562,t563,t579,t588,t590,t595,t602,t605,t606,t625,t629,
		t630,t640,t645,t664,t670,t699,t702,t709,t716,t732,t736,t739,t744,t770,t772,
		t778,t785,t808,t822;

	SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 8, &R1, &DR, &eta_c, &eta_sh, &x_in, &x_out, &alpha, &eta_sol);

	t0 = 4.0*0.3141592653589793E1/(q*q*q)*(eta_c-eta_sol)*(sin(R1*q)-
	cos(R1*q)*R1*q);

	if (DR == 0) return t0;

	t1 = x_in*eta_sh;
	t2 = q*t1;
	t3 = alpha*alpha;
	t4 = t3*t3;
	t5 = t4*t3;
	t6 = t5*DR;
	t7 = R1*q;
	t8 = cos(t7);
	t9 = q*DR;
	t10 = cos(t9);
	t11 = t10*t8;
	t12 = t11*t6;
	t14 = q*q;
	t15 = t14*t14;
	t16 = t15*t14;
	t17 = t16*t1;
	t18 = DR*DR;
	t19 = t18*t18;
	t20 = t19*t18;
	t21 = t20*alpha;
	t22 = sin(t9);
	t23 = t22*t8;
	t24 = t23*t21;
	t26 = t14*q;
	t27 = t15*t26;
	t28 = t27*eta_sol;
	t29 = t28*R1;
	t30 = sin(t7);
	t31 = t30*t20;
	t32 = t22*t31;
	t34 = eta_sh*x_out;
	t35 = R1*t15;
	t36 = t35*t34;
	t37 = t18*DR;
	t38 = t3*alpha;
	t39 = t38*t37;
	t40 = t10*t30;
	t41 = t40*t39;
	t44 = exp(alpha);
	t45 = t15*q;
	t46 = t45*t44;
	t48 = t8*t19;
	t49 = alpha*R1;
	t50 = t49*t48;
	t53 = t8*t20;
	t54 = t10*t53;
	t56 = x_out*eta_sol;
	t57 = t19*t45;
	t58 = t57*t56;
	t59 = t22*t30;
	t60 = t59*t49;
	t63 = t44*t1;
	t65 = t18*t38;
	t66 = t65*t30*t14;
	t69 = t30*t16;
	t70 = t19*DR;
	t71 = t70*R1;
	t72 = t71*t69;
	t75 = t15*t3*eta_sh;
	t76 = t22*t48;
	t79 = t45*t56;
	t81 = t10*t8*t70;
	t84 = t8*t1;
	t85 = R1*t26;
	t86 = t18*t4;
	t87 = t86*t85;
	t90 = t40*t21;
	t92 = t18*t26;
	t93 = t92*t1;
	t94 = t4*R1;
	t95 = t11*t94;
	t98 = t11*t49;
	t101 = t3*t1;
	t102 = t19*t15;
	t103 = t23*t102;
	t107 = t15*t3*eta_sol;
	t110 = t30*eta_sh;
	t112 = t30*eta_sol;
	t114 = -t12*t2+t24*t17+t32*t29-2.0*t41*t36+2.0*t50*t46*t34-t54*t29-2.0*
	t60*t58-2.0*t66*t63+t72*t63-3.0*t76*t75-2.0*t81*t79+3.0*t87*t84+t90*t17-2.0*t95
	*t93+2.0*t98*t58+3.0*t103*t101+3.0*t76*t107+t5*t110-t5*t112;
	t115 = t45*t34;
	t116 = alpha*t70;
	t117 = t59*t116;
	t121 = t10*t30*t19;
	t124 = t92*t56;
	t125 = R1*t38;
	t126 = t59*t125;
	t129 = t26*t1;
	t130 = t3*t37;
	t131 = t11*t130;
	t134 = t30*t1;
	t136 = t19*t15*t3;
	t139 = t59*t130;
	t142 = t44*t56;
	t144 = t40*t71;
	t146 = t16*eta_sh;
	t147 = t22*t53;
	t149 = q*eta_sol;
	t150 = DR*t149;
	t151 = t8*t5;
	t152 = t10*t151;
	t156 = x_in*eta_sol;
	t157 = t8*t156;
	t159 = t20*R1*t27;
	t161 = t16*eta_sol;
	t162 = t10*t31;
	t164 = t20*t69;
	t166 = t14*t34;
	t167 = t40*t65;
	t171 = t57*t34;
	t172 = R1*t3;
	t173 = t59*t172;
	t176 = t45*eta_sol;
	t177 = t70*t176;
	t178 = t8*t3;
	t179 = t10*t178;
	t182 = t15*t34;
	t183 = t19*alpha;
	t184 = t23*t183;
	t187 = R1*t14;
	t188 = t187*t34;
	t189 = t4*DR;
	t190 = t23*t189;
	t192 = 4.0*t117*t115-3.0*t121*t75-2.0*t126*t124+6.0*t131*t129-3.0*t136*
	t134-6.0*t139*t129+t72*t142-t144*t17-t147*t146-t152*t150+3.0*t121*t107-t159*
	t157+t162*t161-t164*t1-2.0*t167*t166-t162*t146+2.0*t173*t171-3.0*t179*t177+6.0*
	t184*t182-t190*t188;
	t194 = t27*t56;
	t195 = t19*t37;
	t197 = t10*t8*t195;
	t201 = t22*t30*t195;
	t204 = t22*t30*t3;
	t207 = t187*t156;
	t208 = t4*alpha;
	t209 = t208*DR;
	t210 = t23*t209;
	t212 = t40*t183;
	t215 = t16*t56;
	t217 = R1*t45;
	t218 = t19*t3;
	t219 = t218*t217;
	t222 = t40*t209;
	t224 = t45*t1;
	t225 = t3*t70;
	t226 = t59*t225;
	t228 = t15*t1;
	t229 = t38*t19;
	t230 = t23*t229;
	t234 = t40*t189;
	t237 = t14*t4*eta_sh;
	t238 = t8*t18;
	t239 = t22*t238;
	t242 = t44*t156;
	t244 = t187*t56;
	t246 = t92*t34;
	t250 = t92*t156;
	t253 = t197*t194+t159*t84-t201*t194+3.0*t204*t177-t210*t207+6.0*t212*t182
	+t24*t215+3.0*t219*t84-t222*t207+t226*t224+2.0*t230*t228+t201*t28-t234*t188-3.0
	*t239*t237-t72*t242+t222*t244-t95*t246-4.0*t117*t224+2.0*t126*t250;
	t254 = t26*t34;
	t255 = t4*t37;
	t256 = t59*t255;
	t258 = t40*t229;
	t261 = q*t156;
	t262 = t59*t6;
	t264 = t11*t116;
	t267 = t11*t255;
	t270 = t10*t30*t18;
	t276 = alpha*t16;
	t277 = t276*t1;
	t278 = t23*t71;
	t285 = t16*t156;
	t287 = t44*t34;
	t290 = t11*t225;
	t292 = t14*t44;
	t295 = t94*DR*t30;
	t297 = t14*t1;
	t298 = t208*t18;
	t299 = t23*t298;
	t303 = t276*t156;
	t306 = t256*t254+2.0*t258*t228-t262*t261-4.0*t264*t115-t267*t254-3.0*t270
	*t237+t90*t215+2.0*t60*t171+t278*t277-t210*t188-3.0*t87*t157-2.0*t230*t182-t24*
	t285+2.0*t66*t287-t290*t224+t295*t292*t34+t299*t297-2.0*t147*t215-t278*t303-
	t197*t28;
	t312 = t14*t156;
	t313 = t23*t65;
	t317 = t8*t70*t45;
	t325 = t40*t102;
	t328 = t276*t34;
	t331 = t18*t14*t4;
	t334 = t16*t34;
	t342 = t59*t39;
	t345 = t22*t151;
	t347 = t11*t39;
	t350 = -2.0*t258*t182-t222*t188-2.0*t313*t312+2.0*t317*t63+2.0*t256*t129+
	t144*t277+t190*t244-t144*t303+3.0*t325*t101-t144*t328-3.0*t331*t112-t90*t334-
	t24*t334+4.0*t264*t224+t12*t261-t295*t292*t56+4.0*t342*t254-t345*t156-4.0*t347*
	t254;
	t351 = t30*t5;
	t352 = t10*t351;
	t354 = t8*eta_sh;
	t355 = t5*t7;
	t357 = t22*t351;
	t359 = t8*eta_sol;
	t361 = t19*t172;
	t367 = t27*t34;
	t369 = t11*t172;
	t372 = t187*t1;
	t375 = t40*t298;
	t380 = t11*t125;
	t386 = t26*eta_sol;
	t387 = t37*t386;
	t389 = t22*t30*t4;
	t392 = q*eta_sh;
	t393 = DR*t392;
	t396 = -t352*t156-t355*t354+t357*t150+3.0*t361*t45*t359+t159*t359+t144*
	t285+t278*t285-t197*t367-2.0*t369*t171+t190*t372+t147*t161+t375*t297-2.0*t167*
	t312-t278*t328-2.0*t380*t250+t234*t244-2.0*t98*t171+3.0*t389*t387-t357*t393+
	t152*t393;
	t398 = t26*t56;
	t402 = t183*t30*t15;
	t405 = t26*t156;
	t408 = t14*t56;
	t409 = t23*t86;
	t414 = R1*t386;
	t415 = t11*t86;
	t418 = t30*t156;
	t427 = t15*t56;
	t435 = t59*t86;
	t441 = t26*eta_sh;
	t442 = t37*t441;
	t447 = -4.0*t342*t398+6.0*t402*t142-2.0*t256*t405+2.0*t409*t408-2.0*t126*
	t93-3.0*t415*t414+3.0*t331*t418+4.0*t342*t405-4.0*t342*t129+2.0*t317*t142+2.0*
	t230*t427-2.0*t66*t142+2.0*t380*t93+t299*t408+3.0*t435*t414-4.0*t347*t405+t234*
	t372-3.0*t389*t442-6.0*t184*t427;
	t453 = R1*t20;
	t454 = t59*t453;
	t461 = R1*t176;
	t462 = t11*t218;
	t465 = t59*t218;
	t468 = t11*t453;
	t470 = t40*t86;
	t475 = t26*t44;
	t477 = t125*t238;
	t480 = t45*eta_sh;
	t481 = R1*t480;
	t494 = t35*t1;
	t497 = t57*t1;
	t499 = -3.0*t331*t134-t72*t287+6.0*t139*t405-t454*t194+2.0*t147*t334-2.0*
	t50*t46*t56-3.0*t462*t461+3.0*t465*t461+t468*t194+2.0*t470*t408+2.0*t258*t427
	-2.0*t477*t475*t56+3.0*t462*t481-2.0*t267*t129+2.0*t226*t115+2.0*t162*t334-6.0*
	t212*t427+3.0*t136*t110+2.0*t41*t494+t173*t497;
	t504 = t10*t8*t4;
	t518 = t5*eta_sol;
	t523 = t5*eta_sh;
	t531 = t4*t156;
	t532 = t18*t14;
	t533 = t23*t532;
	t538 = R1*t441;
	t542 = t178*t37*t26;
	t545 = -3.0*t504*t387+2.0*t477*t475*t34-3.0*t465*t481-2.0*t290*t115-6.0*
	t131*t405-t299*t166-t278*t17+t40*t518-2.0*t60*t497+t23*t518-t40*t523+t164*t156+
	3.0*t136*t418+2.0*t50*t46*t156-5.0*t533*t531-t369*t497-t375*t166-3.0*t435*t538
	-6.0*t542*t63;
	t546 = t85*t156;
	t550 = t22*t30*t70;
	t553 = t20*t16;
	t562 = t5*R1;
	t563 = t59*t562;
	t579 = t217*t156;
	t588 = -2.0*t435*t546+2.0*t550*t224+t553*t110-t553*t112-t351*t1+t351*t156
	+3.0*t415*t538-6.0*t542*t142-t563*t261-t23*t523-2.0*t317*t242+4.0*t347*t129+2.0
	*t66*t242+2.0*t415*t546-2.0*t477*t475*t1-2.0*t81*t224-t465*t579+3.0*t504*t442
	-3.0*t361*t45*t354+4.0*t347*t398;
	t590 = t15*t156;
	t595 = t70*t480;
	t602 = t11*t562;
	t605 = t27*eta_sh;
	t606 = R1*t605;
	t625 = 6.0*t212*t590-2.0*t550*t115-3.0*t204*t595-2.0*t226*t79+2.0*t267*
	t405+t602*t261-t256*t398-t32*t606-6.0*t402*t287-6.0*t402*t242+6.0*t402*t63-t147
	*t17+t267*t398+2.0*t126*t246+6.0*t184*t590-t190*t207+2.0*t98*t497+t462*t579+t54
	*t606;
	t629 = t35*t56;
	t630 = t23*t39;
	t640 = t45*t156;
	t645 = R1*t149;
	t664 = -t295*t292*t1+t375*t408+2.0*t630*t629-2.0*t50*t46*t1+6.0*t139*t254
	+2.0*t630*t494+4.0*t117*t640+2.0*t313*t408+t357*t645-2.0*t380*t246-6.0*t131*
	t254-6.0*t139*t398-4.0*t264*t640+t222*t372+t210*t244+2.0*t41*t629-t299*t312+
	t262*t2+2.0*t290*t79+t278*t334;
	t670 = t14*t4*eta_sol;
	t699 = 2.0*t550*t79+3.0*t270*t670-t375*t312+t147*t285-t234*t207-t162*t17
	-2.0*t630*t36+6.0*t542*t242+3.0*t179*t595+t355*t84-2.0*t173*t58-t278*t215+2.0*
	t369*t58-3.0*t136*t112+t162*t285-6.0*t184*t228+6.0*t131*t398-t355*t157+t295*
	t292*t156;
	t702 = t18*t94;
	t709 = R1*t392;
	t716 = t40*t532;
	t732 = t201*t367-3.0*t702*t26*t354+t144*t334+t210*t372+t355*t359-t159*
	t354-t357*t709+t152*t709-t201*t605+2.0*t313*t297-t152*t645-5.0*t716*t531+t352*
	t1+t345*t1-t226*t640+2.0*t167*t297-t90*t285-2.0*t409*t166+2.0*t477*t475*t156+
	2.0*t380*t124;
	t736 = t3*t156;
	t739 = t4*t1;
	t744 = t276*t56;
	t770 = 4.0*t264*t79-3.0*t103*t736+5.0*t533*t739-t144*t215+t563*t2+t278*
	t744+t197*t605+2.0*t167*t408+t290*t640+3.0*t702*t26*t359-2.0*t550*t640+3.0*t239
	*t670+3.0*t331*t110-2.0*t313*t166-3.0*t325*t736-6.0*t212*t228-t602*t2+5.0*t716*
	t739-2.0*t470*t166;
	t772 = t59*t94;
	t778 = t57*t156;
	t785 = t35*t156;
	t808 = t144*t744+t772*t246-2.0*t230*t590-4.0*t117*t79+2.0*t60*t778+t454*
	t367+6.0*t542*t287-t468*t367-2.0*t630*t785-2.0*t162*t215-t772*t124-3.0*t219*
	t157+t95*t124-2.0*t317*t287-2.0*t258*t590+2.0*t81*t640+2.0*t81*t115-2.0*t41*
	t785-2.0*t98*t778+2.0*t772*t93;
	t822 = -4.0/(t5+3.0*t331+3.0*t136+t553)/t26*(t114+t192+t253+t306+t350+
	t396+t447+t499+t545+t588+t625+t664+t699+t732+t770+t808)*0.3141592653589793E1;

	return t0+t822;
}

/*
float Kshexp2(Tcl_Interp *interp,
          float q, 
          float R1, 
          float DR, 
          float eta_c, 
          float eta_sh,
		  float x_in,
		  float x_out,
		  float alpha,
		  float eta_sol,
          bool  *error)
	*/
// form factor of a spherical shell with radii R1 and DR                      
//          contrast profile in shell:                                        
// alpha >= 0
//               eta_sh_in : scattering length density at R1                  
//              eta_sh_out : scattering length density at R1+DR               
//             eta_solvent : scattering length density of solvent             
//            x_in_solvent : amount of solvent at R1                          
//           x_out_solvent : amount of solvent at R1+DR                       
//               eta_shell : scattering length density of pure shell material 
scalar sasfit_kshexp2(scalar q, sasfit_param * param)
{
	scalar R1, DR, eta_c, eta_sh, x_in, x_out, alpha, eta_sol;
	scalar t0,t1,t2,t3,t4,t5,t6,t7,t8,t10,t11,t12,t14,t15,t16,t17,t18,t20,t21,t22,
		t23,t24,t25,t26,t27,t29,t30,t31,t32,t33,t34,t35,t36,t38,t39,t40,t41,t42,t45,
		t46,t47,t48,t50,t51,t52,t53,t55,t56,t58,t59,t60,t65,t66,t67,t70,t71,t72,t73,
		t75,t78,t79,t81,t84,t85,t86,t87,t91,t94,t97,t99,t101,t102,t103,t104,t105,t108,
		t109,t110,t111,t114,t117,t118,t119,t121,t122,t125,t126,t128,t131,t132,t133,
		t135,t137,t138,t140,t141,t143,t144,t145,t146,t147,t150,t155,t156,t158,t159,
		t163,t164,t166,t167,t172,t174,t176,t178,t180,t182,t183,t186,t188,t191,t192,
		t193,t194,t195,t198,t199,t200,t201,t206,t207,t209,t210,t213,t216,t218,t219,
		t222,t224,t225,t226,t229,t231,t232,t233,t236,t237,t238,t241,t243,t245,t246,
		t248,t250,t254,t257,t259,t261,t262,t265,t268,t272,t276,t282,t283,t284,t285,
		t288,t291,t293,t294,t298,t300,t301,t303,t304,t306,t308,t309,t310,t314,t317,
		t318,t321,t323,t326,t330,t331,t335,t336,t341,t342,t346,t349,t350,t355,t358,
		t371,t373,t376,t377,t379,t382,t383,t385,t388,t389,t391,t392,t395,t396,t398,
		t399,t403,t404,t405,t411,t412,t416,t418,t420,t427,t432,t435,t437,t439,t441,
		t447,t448,t450,t454,t457,t459,t463,t466,t472,t476,t477,t480,t482,t485,t488,
		t492,t498,t499,t500,t504,t508,t518,t523,t526,t529,t537,t549,t555,t564,t567,
		t570,t573,t577,t578,t581,t591,t594,t597,t608,t611,t617,t622,t638,t644,t645,
		t650,t673,t683,t694,t698,t705,t709,t717,t735,t745,t757,t777,t790,t795,t798,
		t831,t837,t850;

	SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 8, &R1, &DR, &eta_c, &eta_sh, &x_in, &x_out, &alpha, &eta_sol);

	t0 = 4.0*0.3141592653589793E1/(q*q*q)*(eta_c-eta_sol)*(sin(R1*q)-
	cos(R1*q)*R1*q);

	if (DR == 0) return t0;

	t1 = q*q;
	t2 = t1*q;
	t3 = t1*t1;
	t4 = t3*t2;
	t5 = eta_sh*t4;
	t6 = DR*DR;
	t7 = t6*t6;
	t8 = t7*t6;
	t10 = t8*x_out*t5;
	t11 = R1*q;
	t12 = cos(t11);
	t14 = q*DR;
	t15 = cos(t14);
	t16 = exp(alpha);
	t17 = t16*t15;
	t18 = t17*t12*R1;
	t20 = x_out*eta_sol;
	t21 = t1*t20;
	t22 = alpha*alpha;
	t23 = t22*t22;
	t24 = t23*t6;
	t25 = sin(t14);
	t26 = t25*t12;
	t27 = t26*t24;
	t29 = eta_sh*x_out;
	t30 = t23*t22;
	t31 = t30*R1;
	t32 = t31*t29;
	t33 = sin(t11);
	t34 = t25*t33;
	t35 = q*t16;
	t36 = t35*t34;
	t38 = t3*t1;
	t39 = t8*t38;
	t40 = x_out*t39;
	t41 = t12*eta_sol;
	t42 = t16*t25;
	t45 = eta_sol*t4;
	t46 = R1*t45;
	t47 = t33*t8;
	t48 = t42*t47;
	t50 = R1*t1;
	t51 = t50*t20;
	t52 = t23*DR;
	t53 = t26*t52;
	t55 = t15*t33;
	t56 = t55*t52;
	t58 = t3*t20;
	t59 = t7*alpha;
	t60 = t26*t59;
	t65 = t22*eta_sol*x_out*t3;
	t66 = t33*t7;
	t67 = t17*t66;
	t70 = x_in*eta_sol;
	t71 = t16*t70;
	t72 = t3*q;
	t73 = t7*DR;
	t75 = t12*t73*t72;
	t78 = x_in*eta_sh;
	t79 = t72*t78;
	t81 = t25*t33*t73;
	t84 = eta_sh*t72;
	t85 = t73*t84;
	t86 = t12*t22;
	t87 = t17*t86;
	t91 = t30*DR*eta_sh;
	t94 = t15*t12*t73;
	t97 = t1*t29;
	t99 = t55*t24;
	t101 = t2*t20;
	t102 = t6*DR;
	t103 = t22*t102;
	t104 = t15*t12;
	t105 = t104*t103;
	t108 = t18*t10-t27*t21-t36*t32+t42*t41*t40-t48*t46-t53*t51-t56*t51+6.0*
	t60*t58+3.0*t67*t65+2.0*t75*t71-2.0*t81*t79-3.0*t87*t85+t36*t91+2.0*t94*t79+t27
	*t97+t99*t97-6.0*t105*t101;
	t109 = t72*t20;
	t110 = alpha*t73;
	t111 = t34*t110;
	t114 = t72*t29;
	t117 = t38*t78;
	t118 = t12*t8;
	t119 = t25*t118;
	t121 = eta_sh*t3;
	t122 = t22*t121;
	t125 = eta_sol*t72;
	t126 = t73*t125;
	t128 = t42*t33*t22;
	t131 = t7*t102;
	t132 = t131*t5;
	t133 = t16*t104;
	t135 = t16*t34;
	t137 = t30*DR;
	t138 = t137*t29;
	t140 = t137*t20;
	t141 = t35*t104;
	t143 = t6*t2;
	t144 = t143*t20;
	t145 = t22*alpha;
	t146 = R1*t145;
	t147 = t34*t146;
	t150 = t16*t78;
	t155 = t1*eta_sol*x_out*t16;
	t156 = DR*t33;
	t158 = t23*alpha*R1;
	t159 = t158*t156;
	t163 = x_in*t16;
	t164 = t41*t163;
	t166 = t7*t22;
	t167 = t166*R1*t72;
	t172 = t8*R1*t4*t12;
	t174 = eta_sh*t16;
	t176 = t12*x_in*t174;
	t178 = t24*R1*t2;
	t180 = t50*t78;
	t182 = t145*t102;
	t183 = t34*t182;
	t186 = 2.0*t111*t109+2.0*t81*t114+t119*t117+3.0*t67*t122-3.0*t128*t126-
	t133*t132+t135*t132-t36*t138-t141*t140+2.0*t147*t144-2.0*t75*t150-t159*t155+3.0
	*t87*t126+2.0*t167*t164+t172*t71-t178*t176-t53*t180+2.0*t183*t101;
	t188 = t104*t182;
	t191 = R1*t6;
	t192 = t191*t20;
	t193 = t12*t23;
	t194 = t2*t16;
	t195 = t194*t193;
	t198 = t23*eta_sol;
	t199 = t6*t198;
	t200 = t16*t33;
	t201 = t1*t200;
	t206 = DR*eta_sol;
	t207 = t30*t206;
	t209 = t12*t7;
	t210 = t42*t209;
	t213 = t72*t70;
	t216 = t102*t16;
	t218 = t33*t3;
	t219 = t146*t218;
	t222 = R1*t5;
	t224 = eta_sol*R1;
	t225 = t24*t224;
	t226 = t194*t34;
	t229 = x_out*t72;
	t231 = t7*eta_sol*t229;
	t232 = t22*R1;
	t233 = t133*t232;
	t236 = t23*t102;
	t237 = t236*t20;
	t238 = t194*t104;
	t241 = x_out*t5;
	t243 = t42*t33*t131;
	t245 = x_out*t4;
	t246 = eta_sol*t245;
	t248 = t33*t23;
	t250 = t2*t42*t248;
	t254 = t22*x_out*t121;
	t257 = t31*t20;
	t259 = -2.0*t188*t101+2.0*t195*t192+3.0*t201*t199-2.0*t94*t114-t36*t207+
	3.0*t210*t122-2.0*t94*t213-2.0*t219*t20*t216+t48*t222-3.0*t226*t225-3.0*t233*
	t231-3.0*t238*t237-t243*t241+t243*t246+3.0*t250*t192-3.0*t210*t254-t141*t257;
	t261 = t232*t84;
	t262 = t17*t209;
	t265 = t104*t110;
	t268 = t2*t78;
	t272 = t6*t23*eta_sh;
	t276 = t73*x_out*t84;
	t282 = t7*t72;
	t283 = t282*t78;
	t284 = alpha*R1;
	t285 = t104*t284;
	t288 = t50*t70;
	t291 = t17*t12*t131;
	t293 = t1*t16;
	t294 = t293*t55;
	t298 = t73*x_in;
	t300 = t33*t38;
	t301 = t284*t300;
	t303 = t1*t33;
	t304 = t303*t24;
	t306 = t72*t16;
	t308 = t73*eta_sol;
	t309 = t12*alpha;
	t310 = t309*t308;
	t314 = t284*t209;
	t317 = t143*t70;
	t318 = t104*t146;
	t321 = t16*t29;
	t323 = t86*t102*t2;
	t326 = t36*t257-3.0*t262*t261+2.0*t265*t114+2.0*t183*t268-3.0*t201*t272+
	3.0*t87*t276+t17*t33*eta_sol*t40-2.0*t285*t283+t53*t288-t291*t246-3.0*t294*t199
	+t36*t140-t301*t298*t174+t304*t71-2.0*t310*x_out*t306-2.0*t314*t306*t29+2.0*
	t318*t317-6.0*t323*t321;
	t330 = t7*x_out*t84;
	t331 = t135*t232;
	t335 = t73*R1;
	t336 = t55*t335;
	t341 = t102*eta_sol;
	t342 = t23*t341;
	t346 = t3*t70;
	t349 = t6*t145;
	t350 = t26*t349;
	t355 = t35*t12*t30;
	t358 = t22*eta_sol*t3;
	t371 = t158*t303;
	t373 = t143*t29;
	t376 = t38*t70;
	t377 = t26*t335;
	t379 = -3.0*t331*t330-t141*t91+t336*t117+2.0*t111*t79+t141*t207-3.0*t226*
	t342+t141*t32-6.0*t60*t346-2.0*t350*t21-t355*R1*t29-3.0*t67*t358-2.0*t285*t231+
	2.0*t309*t29*t73*t306+2.0*t219*t341*t163+t371*t206*t163+2.0*t318*t373-t377*t376
	;
	t382 = t30*t20;
	t383 = t16*t26;
	t385 = t55*t349;
	t388 = t30*eta_sh;
	t389 = t16*t55;
	t391 = R1*eta_sh;
	t392 = t30*t391;
	t395 = t38*t20;
	t396 = t15*t47;
	t398 = t16*t20;
	t399 = t335*t300;
	t403 = x_out*t6;
	t404 = t198*t403;
	t405 = t293*t26;
	t411 = t16*t12;
	t412 = t411*t166;
	t416 = t1*x_out*t174;
	t418 = t38*t16;
	t420 = t8*eta_sol;
	t427 = t23*t102*eta_sh;
	t432 = t236*t29;
	t435 = 3.0*t238*t225+t383*t382-2.0*t385*t21+t389*t388+t36*t392-t141*t392+
	t396*t395-t399*t398+2.0*t75*t321+3.0*t405*t404+3.0*t294*t272-3.0*t412*R1*t125+
	t159*t416-t33*t420*x_in*t418+2.0*t310*x_in*t306+3.0*t226*t427-3.0*t238*t427-3.0
	*t226*t432;
	t437 = t30*t29;
	t439 = eta_sh*t6;
	t441 = t293*t248;
	t447 = eta_sol*t38;
	t448 = t8*t447;
	t450 = t55*t59;
	t454 = t411*t232;
	t457 = t16*t33*t30;
	t459 = t16*t118;
	t463 = t34*t103;
	t466 = t3*t78;
	t472 = t34*t284;
	t476 = t12*t6;
	t477 = t194*t476;
	t480 = t30*eta_sol;
	t482 = t2*t70;
	t485 = -t389*t437+4.0*t441*x_out*t439-t371*x_in*DR*t174-t383*t448+6.0*
	t450*t58-t336*t376+t454*t231-t457*t20+t459*t222+2.0*t94*t109+6.0*t463*t268+6.0*
	t60*t466+6.0*t450*t466-t56*t180+2.0*t472*t231-3.0*t477*t23*t224+t200*t480-2.0*
	t183*t482;
	t488 = t349*t303;
	t492 = t23*R1*t156;
	t498 = t23*t70;
	t499 = t6*t1;
	t500 = t26*t499;
	t504 = t2*t12;
	t508 = t50*t29;
	t518 = t1*t70;
	t523 = t55*t499;
	t526 = t3*t29;
	t529 = 2.0*t188*t482-2.0*t488*t321+t492*t155-3.0*t210*t358-2.0*t472*t330+
	t500*t498+2.0*t504*t145*x_out*eta_sh*t216+t53*t508+t56*t508+3.0*t238*t342-2.0*
	t265*t109-2.0*t75*t398-t99*t21+2.0*t350*t518+2.0*t385*t518+t523*t498-t492*t416
	-6.0*t60*t526;
	t537 = t30*t224;
	t549 = t16*t66;
	t555 = t232*t125;
	t564 = t200*t166;
	t567 = t143*t78;
	t570 = t73*t16;
	t573 = -6.0*t450*t526+2.0*t488*t398-t36*t537+t141*t537+2.0*t350*t97+2.0*
	t385*t97-4.0*t441*eta_sol*t403+t355*R1*t20+3.0*t549*t358+3.0*t412*R1*t84+3.0*
	t262*t555-6.0*t463*t482+6.0*t105*t482-2.0*t167*t176-3.0*t564*t58-2.0*t318*t567-
	t301*t20*t570;
	t577 = eta_sh*t38;
	t578 = t8*t577;
	t581 = x_out*t577;
	t591 = t16*t47;
	t594 = t308*t229;
	t597 = t131*t45;
	t608 = t146*t476;
	t611 = t301*t308*t163-t200*t388+t389*t578-t389*t448-t42*t118*t581-t17*t33
	*x_out*t578+2.0*t285*t330+3.0*t564*t526+t591*t447-t119*t376-3.0*t87*t594-t135*
	t597+t377*t395+t336*t395+t119*t395+t178*t164-2.0*t309*t298*eta_sh*t306+2.0*t608
	*t194*t78;
	t617 = t420*t245;
	t622 = t2*t29;
	t638 = t1*t78;
	t644 = t492*t293*t78+t396*t117+t291*t241-t18*t617+t133*t597-t396*t376-
	t336*t581-6.0*t463*t622-2.0*t219*t102*x_in*t174+t399*t321+2.0*t314*t306*t20-2.0
	*t111*t213+2.0*t265*t213+t56*t288-2.0*t350*t638-2.0*t385*t638-t454*t330;
	t645 = q*t411;
	t650 = t191*t29;
	t673 = t42*t33*R1;
	t683 = t645*t392-t645*t537-3.0*t128*t276-3.0*t250*t650-2.0*t188*t268-2.0*
	t81*t109+6.0*t463*t101+2.0*t608*t194*t20+3.0*t128*t85+2.0*t314*t306*t78+6.0*
	t105*t622-t377*t581-2.0*t265*t79+t673*t617+3.0*t226*t237-t492*t293*t70-3.0*t67*
	t254+3.0*t405*t272;
	t694 = t59*t218;
	t698 = t504*t182;
	t705 = t2*t17*t193;
	t709 = t17*t118;
	t717 = -t383*t437-2.0*t147*t373-3.0*t405*t199-t399*t150+2.0*t488*t150-6.0
	*t694*t398-t304*t150-2.0*t698*t150-t591*t577-6.0*t450*t346+3.0*t705*t650-t459*
	t46-t709*t222+t383*t388+t389*t382+6.0*t694*t71-2.0*t318*t144;
	t735 = t282*t70;
	t745 = t23*t78;
	t757 = -t383*t480+6.0*t323*t398-6.0*t323*t71-2.0*t314*t306*t70-2.0*t488*
	t71-2.0*t504*t145*eta_sol*x_out*t216+2.0*t472*t283-2.0*t472*t735-2.0*t183*t622+
	2.0*t188*t622+3.0*t294*t404-t172*t150-t500*t745+2.0*t219*t29*t216+t301*t29*t570
	+2.0*t81*t213-t523*t745+2.0*t285*t735;
	t777 = t23*x_out*t439;
	t790 = t24*t391;
	t795 = -2.0*t147*t317-6.0*t105*t268-2.0*t608*t194*t70-2.0*t111*t114-3.0*
	t549*t122+t457*t29-t119*t581-t396*t581+t377*t117+3.0*t210*t65-3.0*t294*t777-2.0
	*t195*t650+3.0*t477*t23*t391-3.0*t705*t192+3.0*t233*t330-t673*t10+3.0*t226*t790
	-3.0*t238*t790;
	t798 = t42*t66;
	t831 = t383*t578+t141*t138-3.0*t798*t555-3.0*t405*t777+3.0*t798*t261+6.0*
	t694*t321-6.0*t694*t150+t709*t46+t399*t71+3.0*t238*t432+3.0*t331*t231-2.0*t608*
	t194*t29+6.0*t323*t150+2.0*t698*t71+t33*t8*x_in*eta_sh*t418+2.0*t147*t567+3.0*
	t128*t594-t389*t480;
	t837 = exp(-alpha);
	t850 = 4.0/(t30+3.0*t6*t1*t23+3.0*t7*t3*t22+t39)/t2*t837*(t108+t186+t259+
	t326+t379+t435+t485+t529+t573+t611+t644+t683+t717+t757+t795+t831)*
	0.3141592653589793E1;

	return t0+t850;
}

