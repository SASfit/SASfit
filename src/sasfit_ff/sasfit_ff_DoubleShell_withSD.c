/*
 * src/sasfit_ff/sasfit_ff_DoubleShell_withSD.c
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
 *   Ingo Bressler (ingo.bressler@bam.de)
 */

#include <gsl/gsl_math.h>
#include "include/sasfit_ff_utils.h"

/*
scalar DoubleShell_withSD(Tcl_Interp *interp,
			 scalar q,
			 scalar R,
			 scalar s,
		  scalar c1,
		  scalar c2,
		  scalar c3,
		  scalar cm,
			 scalar d1,
			 scalar d2,
			 bool  *error)
*/

scalar sasfit_ff_DoubleShell_withSD(scalar q, sasfit_param * param)
{
	scalar R, s, c1, c2, c3, cm, d1, d2;

	scalar t2;
	scalar t3;
	scalar t4;
	scalar t5;
	scalar t6;
	scalar t8;
	scalar t10;
	scalar t11;
	scalar t12;
	scalar t14;
	scalar t15;
	scalar t16;
	scalar t17;
	scalar t18;
	scalar t19;
	scalar t20;
	scalar t21;
	scalar t22;
	scalar t23;
	scalar t24;
	scalar t25;
	scalar t28;
	scalar t29;
	scalar t30;
	scalar t31;
	scalar t32;
	scalar t35;
	scalar t36;
	scalar t40;
	scalar t41;
	scalar t42;
	scalar t43;
	scalar t44;
	scalar t45;
	scalar t46;
	scalar t47;
	scalar t48;
	scalar t49;
	scalar t50;
	scalar t51;
	scalar t53;
	scalar t54;
	scalar t55;
	scalar t56;
	scalar t57;
	scalar t60;
	scalar t61;
	scalar t62;
	scalar t67;
	scalar t68;
	scalar t69;
	scalar t70;
	scalar t72;
	scalar t73;
	scalar t74;
	scalar t75;
	scalar t76;
	scalar t77;
	scalar t82;
	scalar t83;
	scalar t84;
	scalar t85;
	scalar t86;
	scalar t87;
	scalar t88;
	scalar t89;
	scalar t90;
	scalar t93;
	scalar t94;
	scalar t95;
	scalar t100;
	scalar t101;
	scalar t102;
	scalar t103;
	scalar t104;
	scalar t107;
	scalar t108;
	scalar t109;
	scalar t112;
	scalar t113;
	scalar t116;
	scalar t119;
	scalar t121;
	scalar t123;
	scalar t124;
	scalar t126;
	scalar t127;
	scalar t128;
	scalar t132;
	scalar t134;
	scalar t141;
	scalar t142;
	scalar t143;
	scalar t144;
	scalar t145;
	scalar t146;
	scalar t147;
	scalar t150;
	scalar t151;
	scalar t153;
	scalar t155;
	scalar t156;
	scalar t157;
	scalar t159;
	scalar t161;
	scalar t163;
	scalar t164;
	scalar t165;
	scalar t166;
	scalar t167;
	scalar t170;
	scalar t172;
	scalar t173;
	scalar t177;
	scalar t178;
	scalar t180;
	scalar t183;
	scalar t184;
	scalar t185;
	scalar t186;
	scalar t187;
	scalar t189;
	scalar t192;
	scalar t193;
	scalar t194;
	scalar t195;
	scalar t197;
	scalar t198;
	scalar t201;
	scalar t202;
	scalar t203;
	scalar t204;
	scalar t205;
	scalar t206;
	scalar t209;
	scalar t210;
	scalar t211;
	scalar t212;
	scalar t213;
	scalar t214;
	scalar t215;
	scalar t218;
	scalar t219;
	scalar t221;
	scalar t222;
	scalar t223;
	scalar t226;
	scalar t227;
	scalar t228;
	scalar t229;
	scalar t230;
	scalar t233;
	scalar t234;
	scalar t235;
	scalar t238;
	scalar t239;
	scalar t240;
	scalar t243;
	scalar t244;
	scalar t249;
	scalar t250;
	scalar t252;
	scalar t255;
	scalar t256;
	scalar t258;
	scalar t261;
	scalar t262;
	scalar t263;
	scalar t264;
	scalar t267;
	scalar t268;
	scalar t272;
	scalar t273;
	scalar t275;
	scalar t276;
	scalar t277;
	scalar t279;
	scalar t282;
	scalar t283;
	scalar t284;
	scalar t287;
	scalar t288;
	scalar t293;
	scalar t294;
	scalar t295;
	scalar t301;
	scalar t302;
	scalar t303;
	scalar t304;
	scalar t305;
	scalar t308;
	scalar t311;
	scalar t312;
	scalar t313;
	scalar t314;
	scalar t317;
	scalar t318;
	scalar t320;
	scalar t321;
	scalar t322;
	scalar t323;
	scalar t327;
	scalar t328;
	scalar t330;
	scalar t331;
	scalar t336;
	scalar t339;
	scalar t341;
	scalar t342;
	scalar t343;
	scalar t344;
	scalar t350;
	scalar t351;
	scalar t352;
	scalar t355;
	scalar t357;
	scalar t358;
	scalar t361;
	scalar t362;
	scalar t366;
	scalar t367;
	scalar t373;
	scalar t374;
	scalar t376;
	scalar t377;
	scalar t380;
	scalar t381;
	scalar t384;
	scalar t385;
	scalar t386;
	scalar t389;
	scalar t394;
	scalar t397;
	scalar t398;
	scalar t399;
	scalar t400;
	scalar t407;
	scalar t412;
	scalar t413;
	scalar t414;
	scalar t419;
	scalar t422;
	scalar t424;
	scalar t427;
	scalar t430;
	scalar t431;
	scalar t432;
	scalar t433;
	scalar t437;
	scalar t442;
	scalar t445;
	scalar t446;
	scalar t447;
	scalar t448;
	scalar t451;
	scalar t452;
	scalar t453;
	scalar t456;
	scalar t457;
	scalar t458;
	scalar t461;
	scalar t462;
	scalar t463;
	scalar t466;
	scalar t471;
	scalar t472;
	scalar t475;
	scalar t480;
	scalar t483;
	scalar t485;
	scalar t486;
	scalar t487;
	scalar t490;
	scalar t493;
	scalar t503;
	scalar t506;
	scalar t511;
	scalar t515;
	scalar t516;
	scalar t519;
	scalar t521;
	scalar t522;
	scalar t523;
	scalar t525;
	scalar t528;
	scalar t529;
	scalar t530;
	scalar t533;
	scalar t534;
	scalar t535;
	scalar t538;
	scalar t540;
	scalar t543;
	scalar t544;
	scalar t545;
	scalar t546;
	scalar t547;
	scalar t550;
	scalar t551;
	scalar t552;
	scalar t555;
	scalar t556;
	scalar t559;
	scalar t560;
	scalar t561;
	scalar t564;
	scalar t565;
	scalar t566;
	scalar t573;
	scalar t576;
	scalar t577;
	scalar t582;
	scalar t584;
	scalar t587;
	scalar t588;
	scalar t591;
	scalar t594;
	scalar t597;
	scalar t598;
	scalar t601;
	scalar t603;
	scalar t605;
	scalar t610;
	scalar t611;
	scalar t612;
	scalar t615;
	scalar t616;
	scalar t620;
	scalar t621;
	scalar t623;
	scalar t626;
	scalar t627;
	scalar t628;
	scalar t631;
	scalar t639;
	scalar t643;
	scalar t647;
	scalar t650;
	scalar t651;
	scalar t654;
	scalar t655;
	scalar t659;
	scalar t660;
	scalar t663;
	scalar t665;
	scalar t668;
	scalar t673;
	scalar t678;
	scalar t681;
	scalar t682;
	scalar t683;
	scalar t684;
	scalar t687;
	scalar t689;
	scalar t695;
	scalar t700;
	scalar t705;
	scalar t708;
	scalar t709;
	scalar t710;
	scalar t713;
	scalar t714;
	scalar t715;
	scalar t720;
	scalar t722;
	scalar t728;
	scalar t730;
	scalar t735;
	scalar t737;
	scalar t740;
	scalar t744;
	scalar t745;
	scalar t746;
	scalar t749;
	scalar t751;
	scalar t752;
	scalar t753;
	scalar t756;
	scalar t759;
	scalar t764;
	scalar t767;
	scalar t769;
	scalar t774;
	scalar t780;
	scalar t786;
	scalar t788;
	scalar t791;
	scalar t792;
	scalar t795;
	scalar t796;
	scalar t800;
	scalar t803;
	scalar t806;
	scalar t807;
	scalar t810;
	scalar t812;
	scalar t815;
	scalar t818;
	scalar t819;
	scalar t825;
	scalar t826;
	scalar t827;
	scalar t830;
	scalar t836;
	scalar t837;
	scalar t842;
	scalar t847;
	scalar t848;
	scalar t851;
	scalar t859;
	scalar t865;
	scalar t866;
	scalar t867;
	scalar t870;
	scalar t871;
	scalar t874;
	scalar t877;
	scalar t880;
	scalar t884;
	scalar t887;
	scalar t888;
	scalar t894;
	scalar t900;
	scalar t902;
	scalar t908;
	scalar t909;
	scalar t910;
	scalar t914;
	scalar t917;
	scalar t919;
	scalar t923;
	scalar t927;
	scalar t928;
	scalar t929;
	scalar t930;
	scalar t933;
	scalar t935;
	scalar t938;
	scalar t941;
	scalar t942;
	scalar t947;
	scalar t952;
	scalar t953;
	scalar t962;
	scalar t967;
	scalar t970;
	scalar t974;
	scalar t978;
	scalar t988;
	scalar t989;
	scalar t994;
	scalar t996;
	scalar t1000;
	scalar t1003;
	scalar t1006;
	scalar t1007;
	scalar t1014;
	scalar t1015;
	scalar t1016;
	scalar t1019;
	scalar t1020;
	scalar t1023;
	scalar t1028;
	scalar t1029;
	scalar t1031;
	scalar t1043;
	scalar t1049;
	scalar t1054;
	scalar t1057;
	scalar t1058;
	scalar t1061;
	scalar t1064;
	scalar t1072;
	scalar t1083;
	scalar t1084;
	scalar t1087;
	scalar t1090;
	scalar t1091;
	scalar t1092;
	scalar t1099;
	scalar t1102;
	scalar t1106;
	scalar t1110;
	scalar t1113;
	scalar t1114;
	scalar t1117;
	scalar t1118;
	scalar t1123;
	scalar t1126;
	scalar t1129;
	scalar t1131;
	scalar t1139;
	scalar t1142;
	scalar t1148;
	scalar t1149;
	scalar t1158;
	scalar t1159;
	scalar t1160;
	scalar t1163;
	scalar t1165;
	scalar t1168;
	scalar t1169;
	scalar t1174;
	scalar t1178;
	scalar t1186;
	scalar t1201;
	scalar t1212;
	scalar t1214;
	scalar t1227;
	scalar t1230;
	scalar t1235;
	scalar t1238;
	scalar t1240;
	scalar t1241;
	scalar t1244;
	scalar t1249;
	scalar t1251;
	scalar t1255;
	scalar t1258;
	scalar t1274;
	scalar t1277;
	scalar t1282;
	scalar t1289;
	scalar t1292;
	scalar t1293;
	scalar t1297;
	scalar t1302;
	scalar t1303;
	scalar t1304;
	scalar t1311;
	scalar t1312;
	scalar t1315;
	scalar t1316;
	scalar t1321;
	scalar t1326;
	scalar t1331;
	scalar t1333;
	scalar t1338;
	scalar t1343;
	scalar t1345;
	scalar t1346;
	scalar t1347;
	scalar t1348;
	scalar t1357;
	scalar t1364;
	scalar t1371;
	scalar t1374;
	scalar t1375;
	scalar t1377;
	scalar t1380;
	scalar t1386;
	scalar t1389;
	scalar t1394;
	scalar t1396;
	scalar t1401;
	scalar t1404;
	scalar t1405;
	scalar t1410;
	scalar t1415;
	scalar t1416;
	scalar t1419;
	scalar t1422;
	scalar t1427;
	scalar t1428;
	scalar t1429;
	scalar t1432;
	scalar t1433;
	scalar t1436;
	scalar t1447;
	scalar t1450;
	scalar t1457;
	scalar t1465;
	scalar t1472;
	scalar t1477;
	scalar t1480;
	scalar t1481;
	scalar t1484;
	scalar t1487;
	scalar t1490;
	scalar t1495;
	scalar t1500;
	scalar t1501;
	scalar t1506;
	scalar t1513;
	scalar t1520;
	scalar t1527;
	scalar t1528;
	scalar t1538;
	scalar t1544;
	scalar t1545;
	scalar t1550;
	scalar t1554;
	scalar t1561;
	scalar t1567;
	scalar t1575;
	scalar t1581;
	scalar t1585;
	scalar t1586;
	scalar t1587;
	scalar t1590;
	scalar t1593;
	scalar t1602;
	scalar t1614;
	scalar t1615;
	scalar t1620;
	scalar t1630;
	scalar t1633;
	scalar t1636;
	scalar t1637;
	scalar t1642;
	scalar t1643;
	scalar t1648;
	scalar t1657;
	scalar t1660;
	scalar t1679;
	scalar t1685;
	scalar t1688;
	scalar t1690;
	scalar t1694;
	scalar t1701;
	scalar t1702;
	scalar t1716;
	scalar t1722;
	scalar t1733;
	scalar t1734;
	scalar t1737;
	scalar t1738;
	scalar t1755;
	scalar t1762;
	scalar t1763;
	scalar t1766;
	scalar t1769;
	scalar t1774;
	scalar t1784;
	scalar t1789;
	scalar t1798;
	scalar t1799;
	scalar t1808;
	scalar t1823;
	scalar t1826;
	scalar t1831;
	scalar t1844;
	scalar t1853;
	scalar t1856;
	scalar t1857;
	scalar t1858;
	scalar t1867;
	scalar t1871;
	scalar t1882;
	scalar t1890;
	scalar t1899;
	scalar t1908;
	scalar t1914;
	scalar t1921;
	scalar t1929;
	scalar t1932;
	scalar t1934;
	scalar t1937;
	scalar t1954;
	scalar t1959;
	scalar t1970;
	scalar t1974;
	scalar t1979;
	scalar t1980;
	scalar t1986;
	scalar t1989;
	scalar t1999;
	scalar t2003;
	scalar t2005;
	scalar t2024;
	scalar t2029;
	scalar t2043;
	scalar t2046;
	scalar t2051;
	scalar t2052;
	scalar t2057;
	scalar t2071;
	scalar t2080;
	scalar t2122;
	scalar t2123;
	scalar t2128;
	scalar t2133;
	scalar t2139;
	scalar t2142;
	scalar t2151;
	scalar t2160;
	scalar t2161;
	scalar t2162;
	scalar t2173;
	scalar t2179;
	scalar t2197;
	scalar t2204;
	scalar t2215;
	scalar t2218;
	scalar t2221;
	scalar t2222;
	scalar t2225;
	scalar t2243;
	scalar t2251;
	scalar t2268;
	scalar t2294;
	scalar t2298;
	scalar t2312;
	scalar t2328;
	scalar t2332;
	scalar t2339;
	scalar t2342;
	scalar t2354;
	scalar t2357;
	scalar t2361;
	scalar t2364;
	scalar t2368;
	scalar t2371;
	scalar t2389;
	scalar t2407;
	scalar t2424;
	scalar t2431;
	scalar t2434;
	scalar t2451;
	scalar t2475;
	scalar t2478;
	scalar t2489;
	scalar t2494;
	scalar t2510;
	scalar t2539;
	scalar t2553;
	scalar t2569;
	scalar t2587;
	scalar t2592;
	scalar t2593;
	scalar t2602;
	scalar t2614;
	scalar t2625;
	scalar t2637;
	scalar t2640;
	scalar t2642;
	scalar t2670;
	scalar t2682;
	scalar t2717;
	scalar t2724;
	scalar t2743;
	scalar t2755;
	scalar t2758;
	scalar t2768;
	scalar t2808;
	scalar t2811;
	scalar t2838;
	scalar t2841;
	scalar t2844;
	scalar t2850;
	scalar t2854;
	scalar t2857;
	scalar t2858;
	scalar t2859;
	scalar t2903;
	scalar t2918;
	scalar t2941;
	scalar t2946;
	scalar t2987;
	scalar t3007;
	scalar t3026;
	scalar t3030;
	scalar t3035;
	scalar t3075;
	scalar t3116;
	scalar t3160;
	scalar t3204;
	scalar t3205;
	scalar t3215;
	scalar t3249;
	scalar t3292;
	scalar t3333;
	scalar t3374;
	scalar t3415;
	scalar t3455;
	scalar t3495;
	scalar t3535;
	scalar t3575;
	scalar t3592;

	SASFIT_ASSERT_PTR(param);

	sasfit_get_param(param, 8, &R, &s, &c1, &c2, &c3, &cm, &d1, &d2);
    d1 = fabs(d1);
    d2 = fabs(d2);

	t2 =  ( R *  R);
	t3 =  q *  q;
	t4 =  (s * s);
	t5 =  t4 *  t4;
	t6 =  (t3 * t5);
	t8 = t2 + 4 * t6;
	t10 = t8 / t2;
	t11 = t2 + t4;
	t12 = 1 / t4;
	t14 =  (t11 * t12) / 0.2e1;
	t15 = pow( t10, -t14);
	t16 = cm * cm;
	t17 = t15 * t16;
	t18 = t3 *  q;
	t19 = t17 * t18;
	t20 = pow( t10, t14);
	t21 = t5 * t20;
	t22 = d1 *  q;
	t23 = sin(t22);
	t24 = d1 * t23;
	t25 = t21 * t24;
	t28 = c2 * c2;
	t29 = t15 * t28;
	t30 = t2 * t2;
	t31 =  t30 * t20;
	t32 = t31 * t3;
	t35 = sqrt( t10);
	t36 = q * t4;
	t40 = atan( (2 * t36 / R));
	t41 =  t2 * t40;
	t42 = t41 *  t12;
	t43 = sin(t42);
	t44 = t35 * t43;
	t45 = t17 * t44;
	t46 = d1 * d1;
	t47 =  t2 * t46;
	t48 = 0.2e1 * t22;
	t49 = sin(t48);
	t50 = t3 * t49;
	t51 = t47 * t50;
	t53 = t15 * cm;
	t54 = c3 * t35;
	t55 = t53 * t54;
	t56 = cos(t42);
	t57 = t56 *  t2;
	t60 = (0.2e1 * d1 + d2) *  q;
	t61 = cos(t60);
	t62 = t61 * t46;
	t67 = t15 * c2;
	t68 = cm * t18;
	t69 = t67 * t68;
	t70 = t5 *  R;
	t72 =  t11 * t40 *  t12;
	t73 = cos(t72);
	t74 = t22 / 0.2e1;
	t75 = sin(t74);
	t76 = t75 * t75;
	t77 = t73 * t76;
	t82 = c2 * cm;
	t83 = t2 * t12;
	t84 =  t83 / 0.2e1;
	t85 = pow( t10, -t84);
	t86 = t85 * t43;
	t87 = t82 * t86;
	t88 =  q * t61;
	t89 = d1 *  t2;
	t90 = t88 * t89;
	t93 = t3 * t3;
	t94 = cm * t93;
	t95 = t67 * t94;
	t100 = c3 * t93;
	t101 = t53 * t100;
	t102 =  t36 * d1;
	t103 =  t36 * d2;
	t104 = t40 *  t4;
	t107 = cos((t102 + t103 + t41 + t104) *  t12);
	t108 = t107 * d1;
	t109 = t70 * t108;
	t112 =  pow( R,  t83);
	t113 = t3 *  t112;
	t116 = t2 + 2 * t4;
	t119 = pow( t8, - (t116 * t12) / 0.2e1);
	t121 = d2 *  q;
	t123 = sin(t121 / 0.2e1);
	t124 = t30 * t2;
	t126 = 0.3e1 * t102;
	t127 = 0.2e1 * t41;
	t128 = 0.4e1 * t104;
	t132 = cos((t126 + t103 + t127 + t128) *  t12 / 0.2e1);
	t134 = t119 * t75 * t123 *  t124 * t132;
	t141 = c2 * c3;
	t142 = t18 *  R;
	t143 = t141 * t142;
	t144 = sin(t72);
	t145 = t15 * t144;
	t146 = t61 * t5;
	t147 = t145 * t146;
	t150 = t35 * t56;
	t151 = t150 *  t2;
	t153 = t93 * t3;
	t155 = t153 *  t112 * t119;
	t156 = t82 * t155;
	t157 = t75 * t123;
	t159 = pow( t10, t84);
	t161 = (d1 + d2) *  q;
	t163 = cos(t161 / 0.2e1);
	t164 = t159 * t163;
	t165 = t5 * t5;
	t166 = t164 * t165;
	t167 = t157 *  t2 * t166;
	t170 = t15 * c1;
	t172 = t170 * c2 * t35;
	t173 = t3 *  t2;
	t177 = sin((t102 + t127 + t128) *  t12 / 0.2e1);
	t178 = t75 * t177;
	t180 = t173 * t178 *  t4;
	t183 = cm * c3;
	t184 = t183 *  q;
	t185 = t2 * R;
	t186 =  t185 * t15;
	t187 = cos(t22);
	t189 = t186 * t144 * t187;
	t192 = t85 * t159;
	t193 = t82 * t192;
	t194 = cos(t121);
	t195 = t194 * d1;
	t197 = t93 * d2 * t5;
	t198 = t195 * t197;
	t201 = c3 * c3;
	t202 = t15 * t201;
	t203 =  t2 * t20;
	t204 = t202 * t203;
	t205 = t3 * d2;
	t206 = t195 * t205;
	t209 = t93 *  R;
	t210 = t183 * t209;
	t211 = t15 * t73;
	t212 = cos(t161);
	t213 = t212 * d2;
	t214 = t213 * t5;
	t215 = t211 * t214;
	t218 = 0.8e1 * t19 * t25 - 0.2e1 * t29 * t32 + t45 * t51 - 0.4e1 * t55
		* t57 * t62 * t3 + 0.64e2 * t69 * t70 * t77 * t23 - 0.4e1 * t87 * t90
		- 0.32e2 * t95 * t70 * t77 * d1 + 0.8e1 * t101 * t109 - 0.8e1 * t82
		* t113 * t134 - 0.4e1 * t55 * t57 * t22 * t49 + 0.16e2 * t143 * t147
		+ t29 * t151 + 0.128e3 * t156 * t167 + 0.4e1 * t172 * t180 - 0.4e1
		* t184 * t189 + 0.8e1 * t193 * t198 + 0.2e1 * t204 * t206 - 0.8e1 * t210 * t215;
	t219 = t35 *  t2;
	t221 = (t102 + t41) *  t12;
	t222 = cos(t221);
	t223 = t219 * t222;
	t226 = t17 * t20;
	t227 = t18 * t5;
	t228 = sin(t161);
	t229 = t228 * d1;
	t230 = t227 * t229;
	t233 = t93 * t5;
	t234 = t212 *  t2;
	t235 = t233 * t234;
	t238 = t212 * d1;
	t239 = t238 * t5;
	t240 = t211 * t239;
	t243 = t16 * t85;
	t244 = t243 * t43;
	t249 = cm * t35;
	t250 = t170 * t249;
	t252 = t57 * t22 * t23;
	t255 = c3 * t20;
	t256 = t170 * t255;
	t258 = t233 * t238 *  R;
	t261 = sin(t60);
	t262 = t261 * d1;
	t263 = t262 * d2;
	t264 = t173 * t263;
	t267 = t76 * t144;
	t268 = t70 * t267;
	t272 = t67 * t249 * t3;
	t273 =  t2 * t187;
	t275 =  t116 * t40 *  t12;
	t276 = cos(t275);
	t277 = t276 * t76;
	t279 = t273 * t277 *  t4;
	t282 = t183 * t85;
	t283 = t56 * t212;
	t284 = t283 *  t6;
	t287 = t43 *  t2;
	t288 = t46 * t3;
	t293 = 0.2e1 * t161;
	t294 = cos(t293);
	t295 =  q * t294;
	t301 = t53 * t54 * t56;
	t302 =  t2 * t61;
	t303 = d1 * t3;
	t304 = t303 * d2;
	t305 = t302 * t304;
	t308 = t233 * t263;
	t311 = t2 * q;
	t312 = cos(t48);
	t313 = d1 * t312;
	t314 =  t311 * t313;
	t317 = t202 * t93;
	t318 = 0.2e1 * t102;
	t320 = (t318 + t103 + t41 + t104) *  t12;
	t321 = cos(t320);
	t322 = t321 * d2;
	t323 = t70 * t322;
	t327 = t93 *  t112 * t119;
	t328 = t82 * t327;
	t330 = t164 * t5;
	t331 = t157 *  t30 * t330;
	t336 = t233 * t212 *  R * d2;
	t339 = -0.4e1 * t17 * t223 - 0.8e1 * t226 * t230 - 0.8e1 * t226 * t235
		- 0.8e1 * t210 * t240 - 0.8e1 * t244 * t23 * t3 * t5 - 0.2e1 * t250
		* t252 - 0.8e1 * t256 * t258 - 0.2e1 * t45 * t264 + 0.16e2 * t19 * t268
		- 0.8e1 * t272 * t279 + 0.8e1 * t282 * t284 - 0.2e1 * t55 * t287
		* t288 * t49 - 0.4e1 * t55 * t287 * t295 * d2 - 0.4e1 * t301 * t305 -
		0.8e1 * t45 * t308 + 0.2e1 * t45 * t314 + 0.8e1 * t317 * t323 + 0.64e2
		* t328 * t331 - 0.8e1 * t256 * t336;
	t341 = t85 * t56;
	t342 = t183 * t341;
	t343 = t61 * d1;
	t344 = t343 * t197;
	t350 = t170 * cm * t20;
	t351 =  t185 * t212;
	t352 = t351 * t303;
	t355 = t5 *  t4;
	t357 = t164 *  t2;
	t358 = t157 * t355 * t357;
	t361 = t202 * t20;
	t362 =  t185 * t194;
	t366 = t67 * t249;
	t367 = t3 *  t30;
	t373 = t113 * t119;
	t374 = t82 * t373;
	t376 = t164 *  t30;
	t377 = t157 *  t4 * t376;
	t380 = d2 * t228;
	t381 =  t311 * t380;
	t384 = t82 * t341;
	t385 = t312 * t46;
	t386 = t385 * t173;
	t389 = t141 * t341;
	t394 = t183 * t373;
	t397 = t141 * t209;
	t398 = d1 * t49;
	t399 = t398 * t5;
	t400 = t145 * t399;
	t407 = t227 * t380;
	t412 = sin(t293);
	t413 = t3 * t412;
	t414 = d2 * d2;
	t419 = t183 * t327;
	t422 = 0.8e1 * t342 * t344 + 0.2e1 * t342 * t305 + 0.2e1 * t350 * t352
		+ 0.64e2 * t328 * t358 + 0.2e1 * t361 * t362 * t205 - 0.8e1 * t366
		* t367 * t187 * t276 * t76 + 0.8e1 * t374 * t377 - 0.2e1 * t226 * t381
		- 0.2e1 * t384 * t386 - 0.8e1 * t389 * t344 - 0.2e1 * t389 * t305
		+ 0.8e1 * t394 * t377 - 0.16e2 * t397 * t400 - 0.4e1 * t55 * t287 * t22
		* t312 - 0.8e1 * t256 * t407 - 0.8e1 * t256 * t230 - 0.2e1 * t55
		* t287 * t413 * t414 + 0.64e2 * t419 * t331;
	t424 = t195 * t205 *  t2;
	t427 = t351 * t205;
	t430 = t16 * t3;
	t431 = t430 *  t185;
	t432 = t261 * d2;
	t433 = t145 * t432;
	t437 = t43 * t261 *  t6;
	t442 =  t311 * t229;
	t445 = t3 *  t185;
	t446 = t141 * t445;
	t447 = t61 * d2;
	t448 = t211 * t447;
	t451 = t202 * t44;
	t452 = t412 * t414;
	t453 = t233 * t452;
	t456 = t170 * cm;
	t457 = t107 * d2;
	t458 = t445 * t457;
	t461 = t53 * c3;
	t462 = t187 * t3;
	t463 = t31 * t462;
	t466 = t385 * t233;
	t471 = t261 * t5;
	t472 = t211 * t471;
	t475 =  q * t412;
	t480 = t89 * t205;
	t483 = t16 * t93;
	t485 = t15 * t20;
	t486 = d1 * t5;
	t487 = t485 * t486;
	t490 = t202 * t35;
	t493 =  q * t261;
	t503 = 0.2e1 * t193 * t424 - 0.2e1 * t256 * t427 + 0.2e1 * t431 * t433
		+ 0.8e1 * t282 * t437 + 0.2e1 * t350 * t427 - 0.2e1 * t226 * t442
		- 0.2e1 * t446 * t448 + 0.4e1 * t451 * t453 + 0.2e1 * t456 * t458
		- 0.2e1 * t461 * t463 - 0.8e1 * t384 * t466 + 0.64e2 * t419 * t358
		+ 0.16e2 * t143 * t472 - 0.4e1 * t55 * t57 * t475 * d1 - 0.2e1 * t361
		* t480 - 0.8e1 * t483 *  R * t487 + 0.8e1 * t490 * t437 + 0.4e1
		* t55 * t57 * t493 * d2 + 0.2e1 * t55 * t57 * t294 * t3 * t414;
	t506 = t294 * t46;
	t511 = c3 *  t2;
	t515 = t17 * t35;
	t516 = t367 * t178;
	t519 = cos(t74);
	t521 = t76 * t75;
	t522 = sin(t275);
	t523 = t521 * t522;
	t525 =  t2 * t519 * t523 *  t4;
	t528 = t67 * cm;
	t529 = t20 * t46;
	t530 = t233 * t529;
	t533 = t170 * c3;
	t534 = t35 * t3;
	t535 = 0.2e1 * t104;
	t538 = cos((t102 + t103 + t41 + t535) *  t12);
	t540 = t534 *  t30 * t538;
	t543 = cm *  t2;
	t544 = t67 * t543;
	t545 = t20 * d1;
	t546 =  q * t23;
	t547 = t545 * t546;
	t550 = t20 * t187;
	t551 = t3 *  t4;
	t552 = t550 * t551;
	t555 = t43 * t228;
	t556 = t555 *  t2;
	t559 = t56 * t3;
	t560 = t5 * t312;
	t561 = t559 * t560;
	t564 = t412 * d2;
	t565 = t564 * d1;
	t566 = t233 * t565;
	t573 = t183 * t155;
	t576 = t183 * t445;
	t577 = t211 * t343;
	t582 = cos((t318 + t103 + t41 + t535) *  t12);
	t584 = t173 * t582 *  t4;
	t587 = t202 * t150;
	t588 = t227 * t432;
	t591 = t141 *  q;
	t594 = t211 * t213;
	t597 = 0.2e1 * t55 * t57 * t506 * t3 + 0.4e1 * t53 * t511 * t20 +
		+ 0.4e1 * t515 * t516 + 0.16e2 * t272 * t525 + 0.8e1 * t528 * t530
		- 0.2e1 * t533 * t540 - 0.2e1 * t544 * t547 - 0.2e1 * t544 * t552
		- 0.2e1 * t282 * t556 + 0.4e1 * t490 * t561 + 0.8e1 * t451 * t566
		- 0.4e1 * t55 * t287 * t295 * d1 + 0.128e3 * t573 * t167 + 0.4e1 * t576
		* t577 + 0.2e1 * t515 * t584 - 0.8e1 * t587 * t588 + 0.4e1 * t591
		* t189 - 0.2e1 * t576 * t594;
	t598 = 0.2e1 * t103;
	t601 = cos((t318 + t598 + t41 + t535) *  t12);
	t603 = t173 * t601 *  t4;
	t605 = t555 *  t6;
	t610 = cos((t102 + t41 + t104) *  t12);
	t611 = d1 * t610;
	t612 = t445 * t611;
	t615 =  t2 * t294;
	t616 = t615 * t304;
	t620 = (t102 + t103 + t41) *  t12;
	t621 = cos(t620);
	t623 = t534 * t5 * t621;
	t626 = t82 * t85;
	t627 = t159 * t46;
	t628 = t627 * t233;
	t631 = t145 * t229;
	t639 = t627 * t173;
	t643 = t485 * t195 * t5;
	t647 = t550 * t3;
	t650 =  t2 * t194;
	t651 = t650 * t288;
	t654 = d1 * t187;
	t655 = t654 * t5;
	t659 = t343 * t5;
	t660 = t211 * t659;
	t663 = sin(t221);
	t665 = t227 * d1 * t663;
	t668 = t243 * t56;
	t673 = t56 * t93;
	t678 =  t6 * t20;
	t681 = t29 *  t185;
	t682 = t3 * t73;
	t683 = t76 * d1;
	t684 = t682 * t683;
	t687 = -t515 * t603 - 0.8e1 * t282 * t605 + 0.2e1 * t528 * t612
		+ 0.4e1 * t301 * t616 + 0.8e1 * t533 * t623 - 0.8e1 * t626 * t628
		- 0.2e1 * t446 * t631 + 0.16e2 * t366 * t367 * t519 * t521 * t522
		- 0.2e1 * t626 * t639 - 0.16e2 * t397 * t643 + 0.2e1 * t17
		* t30 * t647 + 0.2e1 * t361 * t651 - 0.8e1 * t397 * t485 * t655 - 0.16e2
		* t397 * t660 + 0.8e1 * t250 * t665 - 0.8e1 * t668 * t312 * t3 * t5
		+ 0.8e1 * t55 * t673 * t560 * t46 - 0.8e1 * t202 * t678 + 0.4e1 * t681 * t684;
	t689 = t53 * t511;
	t695 = t233 * t261 * t46;
	t700 = t157 * t355 *  t2 * t132;
	t705 = t157 *  t4 *  t30 * t132;
	t708 = t170 * c2;
	t709 = t93 * t355;
	t710 = t709 * t550;
	t713 = sin(t620);
	t714 = t713 * d1;
	t715 = t227 * t714;
	t720 = cos((t102 + t41 + t535) *  t12);
	t722 = t534 *  t30 * t720;
	t728 = cos((t126 + t127) *  t12 / 0.2e1);
	t730 = t75 * d1;
	t735 = t20 * t93;
	t737 = t735 * t355 * t212;
	t740 = q * t185;
	t744 = cos((t102 + t127 + t535) *  t12 / 0.2e1);
	t745 = t744 * t75;
	t746 =  t740 * t745;
	t749 = t202 * t18;
	t751 = (t318 + t598 + t41 + t104) *  t12;
	t752 = sin(t751);
	t753 = t70 * t752;
	t756 =  t740 * t752;
	t759 = t287 * t49;
	t764 = t186 * t73 * t23;
	t767 = t53 * t255;
	t769 = t233 * t194 * t46;
	t774 = t157 *  t30 * t132 * t5;
	t780 = -0.2e1 * t689 * t547 - 0.2e1 * t689 * t552 - 0.8e1 * t451
		* t695 - 0.32e2 * t419 * t700 - 0.8e1 * t394 * t705 - 0.8e1 * t708
		* t710 - 0.8e1 * t250 * t715 - 0.2e1 * t456 * t722 - 0.8e1 * t366
		* t2 * t728 * t730 *  q + 0.8e1 * t461 * t737 - 0.8e1
		* t528 * t746 + 0.8e1 * t749 * t753 + 0.2e1 * t17 * t756 - t490 * t759
		- 0.8e1 * t87 * t695 - 0.4e1 * t184 * t764 - 0.16e2 * t767 * t769
		- 0.32e2 * t419 * t774 + 0.4e1 * t361 * t362 * t303;
	t786 = t17 *  t185;
	t788 =  q * t76 * t144;
	t791 = t713 * d2;
	t792 = t227 * t791;
	t795 = t159 * t187;
	t796 = t795 *  t6;
	t800 = t145 * t229 * t5;
	t803 = t62 * t173;
	t806 = t49 * t5;
	t807 = t211 * t806;
	t810 = sin(t121);
	t812 = t227 * d2 * t810;
	t815 = t145 * t380;
	t818 = t5 * t294;
	t819 = t559 * t818;
	t825 = cos((t318 + t103 + t127 + t535) *  t12 / 0.2e1);
	t826 = t825 * t123;
	t827 =  t740 * t826;
	t830 = t227 * t447;
	t836 = t76 * t23;
	t837 =  q * t73 * t836;
	t842 =  t6 * t550;
	t847 = t412 * d1;
	t848 = t227 * t847;
	t851 = -0.8e1 * t183 * t113 * t134 + 0.16e2 * t461 * t530 + 0.4e1
		* t786 * t788 - 0.8e1 * t250 * t792 + 0.8e1 * t282 * t796 - 0.8e1 * t397
		* t800 + 0.2e1 * t384 * t803 - 0.16e2 * t143 * t807 + 0.8e1 * t226
		* t812 - 0.2e1 * t446 * t815 + 0.4e1 * t490 * t819 - 0.8e1 * t456 * t827
		- 0.8e1 * t45 * t830 - 0.4e1 * t451 * t90 - 0.8e1 * t681 * t837
		- 0.8e1 * t528 * t710 - 0.8e1 * t528 * t842 - 0.2e1 * t461 * t722
		+ 0.8e1 * t587 * t848;
	t859 = cos((t318 + t41) *  t12);
	t865 = t5 * t194;
	t866 = d1 * d2;
	t867 = t865 * t866;
	t870 = t29 * t93;
	t871 = t21 * t46;
	t874 = t29 *  t2;
	t877 = t203 * t212;
	t880 = t62 * t233;
	t884 =  t30 * t194 * t3;
	t887 = t35 * t18;
	t888 = t17 * t887;
	t894 = t534 * t5 * t222;
	t900 = t82 *  q;
	t902 = t186 * t73 * t49;
	t908 = t183 * t86;
	t909 = t46 * t93;
	t910 = t909 * t806;
	t914 = t89 *  q * t49;
	t917 = t43 * t3;
	t919 = d2 *  t2;
	t923 = t243 * t159;
	t927 = t17 * t3;
	t928 = cos(t751);
	t929 =  t185 * t928;
	t930 = t929 * d1;
	t933 = -0.8e1 * t451 * t830 - 0.8e1 * t528 * t534 * t5 * t859 + 0.8e1
		* t17 * t735 * t867 - 0.4e1 * t870 * t871 + 0.2e1 * t874 * t552
		+ 0.2e1 * t461 * t877 + 0.8e1 * t384 * t880 + 0.2e1 * t361 * t884
		+ 0.8e1 * t888 * t5 * t43 * t654 + 0.16e2 * t528 * t894 - 0.2e1 * t431
		* t485 * d1 + 0.4e1 * t900 * t902 + 0.8e1 * t244 * t50 * t5 + 0.8e1
		* t908 * t910 + 0.2e1 * t587 * t914 + 0.2e1 * t243 * t917 * t262 * t919
		+ 0.2e1 * t923 * t288 *  t2 - 0.2e1 * t927 * t930;
	t935 = t919 *  q * t810;
	t938 = t650 * t551;
	t941 = t412 * t46;
	t942 = t233 * t941;
	t947 = t928 * d1;
	t952 = (t318 + t41 + t104) *  t12;
	t953 = sin(t952);
	t962 = t485 * t213;
	t967 =  t311 * t791;
	t970 =  t311 * t714;
	t974 = t89 *  q * t663;
	t978 = t16 *  q *  t185;
	t988 = t56 * t61;
	t989 = t988 *  t6;
	t994 = 0.2e1 * t361 * t935 + 0.2e1 * t361 * t938 + 0.4e1 * t451 * t942
		- 0.2e1 * t193 * t381 + 0.4e1 * t461 * t445 * t947 - 0.16e2 * t461
		* t142 * t5 * t953 - 0.8e1 * t226 * t407 - 0.8e1 * t515 * t665 + 0.2e1
		* t446 * t962 + 0.4e1 * t451 * t910 + 0.2e1 * t515 * t967 + 0.2e1 * t515
		* t970 - 0.2e1 * t515 * t974 - 0.4e1 * t978 * t211 * t228 - 0.2e1
		* t923 * t651 + 0.8e1 * t342 * t230 + 0.4e1 * t786 * t684 - 0.8e1 * t282
		* t989 + 0.2e1 * t342 * t442;
	t996 = t170 * t94;
	t1000 = t186 * t545;
	t1003 = t17 * t150;
	t1006 = cos(t952);
	t1007 = d1 * t1006;
	t1014 = t82 * t209;
	t1015 = t447 * t5;
	t1016 = t211 * t1015;
	t1019 = t18 * d1;
	t1020 = t1019 * t560;
	t1023 =  t311 * t847;
	t1028 = c1 * c1;
	t1029 = t15 * t1028;
	t1031 = t17 * t534;
	t1043 = t173 * t565;
	t1049 = t22 * t23 *  t2;
	t1054 = t22 * t273;
	t1057 = 0.8e1 * t996 * t109 + 0.4e1 * t141 * t3 * t1000 + 0.8e1
		* t1003 * t344 + 0.4e1 * t461 * t445 * t1007 + 0.8e1 * t923 * t909 * t5
		+ 0.8e1 * t1014 * t1016 + 0.16e2 * t87 * t1020 + 0.2e1 * t587 * t1023
		+ 0.8e1 * t515 * t715 - t1029 * t32 + 0.4e1 * t1031 *  t2
		* t75 * t177 *  t4 - 0.8e1 * t668 * t407 - 0.2e1 * t668 * t381
		- 0.4e1 * t250 * t180 + 0.2e1 * t451 * t1043 + 0.8e1 * t767 * t235
		- 0.2e1 * t384 * t1049 - 0.12e2 * t17 * t678 - 0.2e1 * t87 * t1054;
	t1058 = t70 * t745;
	t1061 = t227 * t262;
	t1064 = t70 * t611;
	t1072 = t43 * t49 *  t6;
	t1083 = t43 * t23;
	t1084 = t1083 *  t2;
	t1087 = t227 * t564;
	t1090 = t20 * t3;
	t1091 = t5 * t212;
	t1092 = t1090 * t1091;
	t1099 = t5 * t728 * t730;
	t1102 = t203 * t187;
	t1106 = t173 * t538 *  t4;
	t1110 = t173 * t720 *  t4;
	t1113 = t23 * t5;
	t1114 = t1019 * t1113;
	t1117 = t43 * t93;
	t1118 = t5 * t412;
	t1123 =  t311 * t564;
	t1126 = -0.32e2 * t69 * t1058 - 0.16e2 * t1003 * t1061 + 0.8e1 * t95
		* t1064 + 0.8e1 * t668 * t61 * t3 * t5 - 0.8e1 * t282 * t1072 + 0.8e1
		* t456 * t737 - 0.8e1 * t256 * t235 - 0.16e2 * t55 * t673 * t146 * t46
		+ 0.2e1 * t282 * t1084 + 0.8e1 * t1003 * t1087 + 0.8e1 * t456 * t1092
		+ 0.32e2 * t19 * t1058 + 0.16e2 * t29 * t887 * t1099 + 0.2e1 * t17
		* t1102 - 0.2e1 * t515 * t1106 + 0.2e1 * t515 * t1110 - 0.8e1 * t384
		* t1114 - 0.8e1 * t55 * t1117 * t1118 * t414 + 0.2e1 * t587 * t1123;
	t1129 = t294 * d1;
	t1131 = t233 * t1129 * d2;
	t1139 = t186 * t73 * t228;
	t1142 = t56 * t18;
	t1148 = t29 * t35;
	t1149 =  t6 * t222;
	t1158 = t82 * t445;
	t1159 = t194 * d2;
	t1160 = t485 * t1159;
	t1163 = t56 * d1;
	t1165 = t227 * t1163 * t23;
	t1168 = d1 *  R;
	t1169 = t21 * t1168;
	t1174 = t917 * t1118;
	t1178 = t1090 *  t4;
	t1186 = -0.8e1 * t1003 * t1131 + 0.8e1 * t515 * t792 + 0.2e1 * t923
		* t381 + 0.4e1 * t184 * t1139 - 0.16e2 * t55 * t1142 * t399 - 0.8e1
		* t923 * t769 - 0.8e1 * t1148 * t1149 + 0.8e1 * t533 * t842 - 0.8e1
		* t786 * t837 - 0.8e1 * t193 * t230 + 0.2e1 * t1158 * t1160 - 0.8e1
		* t250 * t1165 - 0.16e2 * t317 * t1169 + 0.4e1 * t45 * t942 + 0.8e1
		* t55 * t1174 - t1029 *  t2 * t1178 + 0.8e1 * t767 * t230 + 0.8e1 * t244 * t228 * t3 * t5;
	t1201 = t709 * t194;
	t1212 = t82 * t142;
	t1214 = t211 * t228 * t5;
	t1227 = t211 * t1113;
	t1230 = t57 * t187;
	t1235 = t141 * t192;
	t1238 = 0.2e1 * t1003 * t803 + 0.8e1 * t461 * t735 * t5 * t414 + 0.8e1
		* t342 * t407 + 0.2e1 * t342 * t381 - 0.8e1 * t55 * t1117 * t1118
		* t46 + 0.8e1 * t226 * t1201 - 0.4e1 * t900 * t764 - 0.8e1 * t193 * t407
		- 0.2e1 * t461 * t150 *  t2 * t312 + 0.16e2 * t1212 * t1214
		- 0.4e1 * t591 * t1139 + 0.4e1 * t184 * t902 - 0.4e1 * t366 * t180
		- 0.2e1 * t342 * t386 + 0.8e1 * t361 * t769 + 0.16e2 * t143 * t1227
		+ 0.2e1 * t515 * t1230 - 0.2e1 * t202 * t32 - 0.8e1 * t1235 * t198;
	t1240 = t56 * t187;
	t1241 = t1240 *  t6;
	t1244 = sin(t320);
	t1249 = t1029 * t3;
	t1251 =  t30 * t35 * t276;
	t1255 = t211 * t655;
	t1258 = t211 * t654;
	t1274 = t550 * t303;
	t1277 = t145 * t24;
	t1282 = t493 * t89;
	t1289 = t233 * t650;
	t1292 = t159 * t194;
	t1293 = t1292 *  t2;
	t1297 = t233 * t294 * t414;
	t1302 = -0.8e1 * t626 * t1241 + 0.32e2 * t461 * t142 * t5 * t1244
		- t1249 * t1251 - 0.2e1 * t342 * t1049 - 0.8e1 * t397 * t1255 - 0.2e1
		* t446 * t1258 - 0.8e1 * t366 * t1165 - 0.16e2 * t461 * t142 * t5 * t752
		- 0.16e2 * t342 * t1061 - 0.16e2 * t461 * t1090 * t865 + 0.2e1 * t170
		* c3 *  t185 * t1274 - 0.2e1 * t1158 * t1277 - 0.16e2 * t1212
		* t472 - 0.4e1 * t342 * t1282 - 0.16e2 * t461 * t735 * t355 * t194
		+ 0.8e1 * t226 * t1289 - 0.2e1 * t243 * t1293 - 0.4e1 * t1003 * t1297
		+ 0.8e1 * t45 * t566;
	t1303 = t187 * t5;
	t1304 = t1019 * t1303;
	t1311 = t18 * t212;
	t1312 = t1311 * t486;
	t1315 =  q * t212;
	t1316 = t1315 * t89;
	t1321 = cos((t318 + t41 + t535) *  t12);
	t1326 = t234 * t551;
	t1331 = t173 * t452;
	t1333 = t1315 * t919;
	t1338 = t367 * t1321;
	t1343 = t67 * cm *  t185;
	t1345 = t144 * d1;
	t1346 = 0.3e1 / 0.2e1 * t22;
	t1347 = cos(t1346);
	t1348 = t1345 * t1347;
	t1357 = t186 * t144 * t61;
	t1364 = -0.8e1 * t87 * t1304 - 0.16e2 * t143 * t1214 + 0.2e1 * t767
		* t442 - 0.8e1 * t244 * t1312 - 0.2e1 * t244 * t1316 + 0.2e1 * t461
		* t534 *  t30 * t1321 + 0.2e1 * t767 * t1326 - 0.2e1 * t515
		* t1084 + t45 * t1331 - 0.2e1 * t244 * t1333 - 0.8e1 * t533 * t737
		- t490 * t1338 + 0.2e1 * t576 * t1258 - 0.8e1 * t1343 * t3 * t75 * t1348
		+ 0.2e1 * t533 * t463 - 0.8e1 * t389 * t230 - 0.4e1 * t900 * t1357
		+ 0.8e1 * t533 * t710 - 0.8e1 * t923 * t1114;
	t1371 = t170 * t511;
	t1374 = t29 * t445;
	t1375 = t75 * t144;
	t1377 = t1375 * d1 * t1347;
	t1380 =  t6 * t1083;
	t1386 = t287 * t22 * t187;
	t1389 = t141 * t86;
	t1394 = t43 * d1;
	t1396 = t227 * t1394 * t187;
	t1401 = t17 * t93;
	t1404 = t321 * d1;
	t1405 = t70 * t1404;
	t1410 = sin(t1346);
	t1415 = t928 * d2;
	t1416 = t70 * t1415;
	t1419 = t70 * t947;
	t1422 = t70 * t1007;
	t1427 = -0.2e1 * t923 * t1049 + 0.2e1 * t1371 * t552 + 0.4e1 * t1374
		* t1377 - 0.8e1 * t172 * t1380 + 0.8e1 * t172 * t1241 - 0.2e1 * t250
		* t1386 - 0.2e1 * t1389 * t51 - 0.4e1 * t900 * t189 - 0.8e1 * t250
		* t1396 - 0.16e2 * t1389 * t1020 + 0.8e1 * t1401 * t323 + 0.16e2 * t1401
		* t1405 - 0.4e1 * t515 * t1072 + 0.32e2 * t69 * t70 * t1375 * t1410
		- 0.8e1 * t1401 * t1416 - 0.8e1 * t1401 * t1419 - 0.8e1 * t1401 * t1422
		- 0.2e1 * t389 * t381;
	t1428 = d2 * t5;
	t1429 = t1311 * t1428;
	t1432 = t3 * t261;
	t1433 = t1432 * t47;
	t1436 = t141 * t155;
	t1447 = t31 * t212 * t3;
	t1450 = t17 *  q;
	t1457 = t445 * t108;
	t1465 = t186 * t73 * t261;
	t1472 = t170 * t100;
	t1477 =  t311 * t447;
	t1480 = c3 * t18;
	t1481 = t170 * t1480;
	t1484 = -0.8e1 * t244 * t1429 + 0.2e1 * t1389 * t1433 - 0.128e3
		* t1436 * t167 - 0.8e1 * t708 * t150 *  t6 + 0.2e1 * t1158
		* t815 - 0.8e1 * t1389 * t910 - 0.2e1 * t533 * t1447 + 0.8e1 * t1450
		* t185 * t744 * t75 - 0.8e1 * t101 * t1064 + 0.2e1 * t456
		* t1457 + 0.4e1 * t515 * t819 + 0.4e1 * t767 * t480 + 0.4e1 * t591
		* t1465 - 0.4e1 * t184 * t1465 + 0.2e1 * t193 * t651 + 0.8e1 * t1472
		* t1064 - 0.8e1 * t515 * t989 - 0.2e1 * t451 * t1477 + 0.8e1 * t1481 * t25;
	t1487 = t21 * t654 *  R;
	t1490 = t70 * t1244;
	t1495 = t283 *  t2;
	t1500 = t17 * t445;
	t1501 = t77 * t654;
	t1506 = t728 * t75;
	t1513 = t145 * t1303;
	t1520 = t57 * t312;
	t1527 = t483 *  R * t15;
	t1528 = t73 * t61;
	t1538 = 0.8e1 * t1472 * t1487 - 0.16e2 * t749 * t1490 + 0.8e1 * t193
		* t769 + 0.2e1 * t282 * t1495 + 0.2e1 * t243 * t556 + 0.8e1 * t1500
		* t1501 - 0.2e1 * t908 * t1433 - 0.32e2 * t366 * t227 * t1506 * d1
		+ 0.4e1 * t515 * t561 - 0.16e2 * t1212 * t1513 - 0.16e2 * t210 * t643
		- 0.2e1 * t87 * t1477 + t515 * t1520 + 0.8e1 * t1003 * t880 + 0.4e1
		* t591 * t764 - 0.8e1 * t1527 * t1528 * t1428 + 0.2e1 * t431 * t1277
		- 0.4e1 * t515 * t1174 + 0.2e1 * t533 * t612;
	t1544 = t144 * t1410;
	t1545 =  q * t75 * t1544;
	t1550 = t144 * t261;
	t1554 =  t6 * t194;
	t1561 = t227 * t343;
	t1567 = t170 * c2 * t93;
	t1575 =  t30 * t187 * t277;
	t1581 = t173 * t1321 *  t4;
	t1585 = t202 * t3;
	t1586 =  t185 * t321;
	t1587 = t1586 * d2;
	t1590 = t183 * t142;
	t1593 = 0.8e1 * t282 * t1380 - 0.16e2 * t19 * t1490 - 0.4e1 * t681
		* t1545 - 0.2e1 * t461 * t612 + 0.8e1 * t1527 * t1550 * t1428 + 0.8e1
		* t361 * t1554 - 0.2e1 * t1235 * t1049 - 0.8e1 * t908 * t695 - 0.16e2
		* t45 * t1561 + 0.8e1 * t1343 * t1545 - 0.8e1 * t1567 * t1487 + 0.2e1
		* t193 * t1049 - 0.8e1 * t533 * t894 + 0.4e1 * t1031 * t1575 + 0.2e1
		* t1003 * t914 - t490 * t1581 + 0.8e1 * t226 * t1554 + 0.2e1 * t1585
		* t1587 - 0.16e2 * t1590 * t1513;
	t1602 = t21 * t273;
	t1614 = t159 * t212;
	t1615 = t1614 *  t6;
	t1620 = t988 *  t2;
	t1630 = t1019 * t806;
	t1633 = t740 * t1244;
	t1636 = R * d2;
	t1637 = t233 * t1636;
	t1642 = 0.2e1 * t668 * t386 - 0.32e2 * t69 * t268 - 0.8e1 * t69 * t25
		- 0.8e1 * t1567 * t1602 + 0.4e1 * t1031 * t279 - 0.8e1 * t95 * t1602
		- 0.2e1 * t431 * t1258 + 0.8e1 * t141 * t113 * t134 - 0.8e1 * t626
		* t1615 + 0.4e1 * t384 * t914 - 0.2e1 * t282 * t1620 - 0.2e1 * t446
		* t485 * t654 + 0.2e1 * t767 * t381 - 0.2e1 * t366 * t1386 + 0.16e2
		* t384 * t1630 + 0.8e1 * t461 * t1633 - 0.8e1 * t226 * t1637 + 0.2e1 * t172 * t252;
	t1643 = t145 * t1091;
	t1648 = t145 * t560;
	t1657 = t740 * t144;
	t1660 = t740 * t953;
	t1679 = t43 * t18;
	t1685 = t559 * t5;
	t1688 = t615 * t288;
	t1690 = 0.16e2 * t1590 * t1643 + 0.8e1 * t668 * t466 + 0.16e2 * t1590
		* t1648 - 0.2e1 * t908 * t264 - 0.8e1 * t397 * t1016 + 0.8e1 * t19
		* t753 - 0.4e1 * t456 * t1657 + 0.2e1 * t202 * t1660 + 0.16e2 * t101
		* t1422 + 0.16e2 * t101 * t1419 + 0.2e1 * t1003 * t1023 + 0.2e1 * t172
		* t1386 + 0.4e1 * t244 * t90 + 0.16e2 * t908 * t1020 - 0.4e1 * t244
		* t314 - 0.8e1 * t908 * t308 - 0.16e2 * t55 * t1679 * t818 * d2 + 0.4e1
		* t1029 * t35 * t1685 - t1003 * t1688;
	t1694 = t493 * t919;
	t1701 = t3 * t414;
	t1702 = t615 * t1701;
	t1716 = t141 * t85;
	t1722 = t16 * t18 *  R;
	t1733 = t53 * t1480;
	t1734 = t70 * t826;
	t1737 = 0.2e1 * t668 * t1049 - 0.2e1 * t342 * t1694 + 0.8e1 * t908
		* t1312 + 0.4e1 * t1148 * t1685 - t1003 * t1702 + 0.4e1 * t87 * t314
		+ 0.16e2 * t767 * t1637 + 0.2e1 * t908 * t1316 - 0.8e1 * t342 * t588
		- 0.8e1 * t456 * t623 + 0.16e2 * t210 * t660 + 0.8e1 * t1716 * t1241
		- 0.2e1 * t1716 * t1520 + 0.16e2 * t1722 * t472 - 0.16e2 * t87 * t1561
		+ 0.2e1 * t1716 * t1230 + 0.4e1 * t389 * t1282 - 0.8e1 * t389 * t407
		- 0.32e2 * t1733 * t1734;
	t1738 = t70 * t457;
	t1755 = t211 * t238;
	t1762 = t313 * t5;
	t1763 = t211 * t1762;
	t1766 = t211 * t313;
	t1769 = t203 * t551;
	t1774 = c2 *  t2;
	t1784 = 0.8e1 * t101 * t1738 - 0.16e2 * t1590 * t472 + 0.16e2 * t1590
		* t1214 + 0.16e2 * t1590 * t807 + 0.8e1 * t1716 * t1072 + 0.16e2
		* t389 * t1061 + 0.2e1 * t384 * t305 - 0.2e1 * t55 * t967 + 0.2e1 * t446
		* t1755 + 0.2e1 * t389 * t1694 + 0.8e1 * t515 * t1241 + 0.16e2 * t397
		* t1763 + 0.4e1 * t446 * t1766 + 0.2e1 * t708 * t1769 + 0.8e1 * t384
		* t344 + 0.2e1 * t170 * t1774 * t20 + 0.8e1 * t397 * t240 - 0.2e1 * t87
		* t264 - 0.8e1 * t1389 * t1312;
	t1789 = t145 * t380 * t5;
	t1798 = t294 * d2;
	t1799 = t227 * t1798;
	t1808 = t183 * t192;
	t1823 = t227 * t1129;
	t1826 = t367 * t601;
	t1831 = -0.8e1 * t397 * t1789 - 0.2e1 * t1389 * t1316 + 0.8e1 * t389
		* t588 - 0.2e1 * t490 * t1620 + 0.8e1 * t451 * t1799 - 0.4e1 * t184
		* t1357 + 0.8e1 * t226 * t769 - 0.4e1 * t490 * t1072 + 0.2e1 * t1808
		* t651 - 0.4e1 * t461 * t1660 + 0.4e1 * t908 * t314 - 0.2e1 * t1389
		* t1333 - 0.16e2 * t1212 * t147 - 0.8e1 * t317 * t1422 - 0.16e2 * t244
		* t1020 + 0.8e1 * t451 * t1823 - t490 * t1826 + 0.4e1 * t978 * t145 * t187;
	t1844 = t485 * t1159 * t5;
	t1853 = t145 * t262;
	t1856 = t29 * t233;
	t1857 =  R * t75;
	t1858 = t1857 * t1348;
	t1867 = t471 * d1;
	t1871 = t445 * d2;
	t1882 = 0.16e2 * t1722 * t1513 - 0.4e1 * t978 * t145 * t312 - 0.4e1
		* t250 * t516 + 0.2e1 * t446 * t1277 + 0.2e1 * t908 * t51 - 0.8e1 * t397
		* t1844 - 0.8e1 * t244 * t910 - 0.8e1 * t87 * t308 - 0.16e2 * t1590
		* t147 + 0.4e1 * t446 * t1853 + 0.16e2 * t1856 * t1858 + 0.2e1 * t202
		* t756 + 0.8e1 * t45 * t1020 - 0.2e1 * t244 * t51 + 0.32e2 * t55 * t1142
		* t1867 - 0.2e1 * t361 * t1871 + 0.16e2 * t1014 * t400 + 0.8e1 * t996
		* t1738 - 0.2e1 * t226 *  t30 * t212 * t3;
	t1890 =  t6 * t212;
	t1899 = t287 * t261;
	t1908 = t145 * t1867;
	t1914 = t219 * t859;
	t1921 = t219 * t621;
	t1929 = t145 * t24 * t5;
	t1932 = -0.8e1 * t1733 * t25 + 0.4e1 * t1500 * t1377 - 0.2e1 * t576
		* t1277 - 0.8e1 * t226 * t1890 - 0.4e1 * t461 * t44 *  t2 * t261
		+ 0.8e1 * t515 * t437 + 0.2e1 * t515 * t1899 + 0.4e1 * t342 * t914
		+ 0.16e2 * t143 * t1513 + 0.2e1 * t1158 * t631 + 0.16e2 * t397 * t1908
		+ 0.8e1 * t1527 * t1345 * t1113 + t17 * t1914 - 0.8e1 * t226 * t709
		* t212 + 0.2e1 * t384 * t442 - 0.2e1 * t461 * t1921 + 0.8e1 * t55 * t665
		+ 0.8e1 * t461 * t894 - 0.8e1 * t210 * t1929;
	t1934 = t70 * t144;
	t1937 = t170 * t68;
	t1954 = t709 * t20;
	t1959 = t145 * t398;
	t1970 =  t311 * t1129;
	t1974 = t186 * t144 * t312;
	t1979 = t29 * t227;
	t1980 = t1857 * t1544;
	t1986 = 0.8e1 * t1029 * t18 * t1934 - 0.32e2 * t1937 * t1734 + 0.8e1
		* t384 * t230 + 0.2e1 * t461 * t44 *  t2 * t49 - 0.8e1 * t515
		* t1380 + 0.8e1 * t55 * t287 * t88 * d1 - 0.8e1 * t317 * t1419 - 0.8e1
		* t29 * t1954 - 0.2e1 * t461 * t1102 + 0.4e1 * t576 * t1959 - 0.2e1
		* t45 * t1433 + 0.2e1 * t55 * t57 * t385 * t3 + 0.8e1 * t668 * t1114
		+ 0.2e1 * t45 * t1970 - 0.4e1 * t591 * t1974 + 0.16e2 * t342 * t1630
		- 0.16e2 * t1979 * t1980 - 0.8e1 * t1003 * t588 + t490 * t1520;
	t1989 = t529 * t3;
	t1999 = t173 * t941;
	t2003 = t17 * t203;
	t2005 = t1292 *  t6;
	t2024 = t233 * t866;
	t2029 = -t874 * t1989 - 0.2e1 * t243 * t1899 + 0.8e1 * t1716 * t989
		+ 0.8e1 * t451 * t1020 - 0.2e1 * t451 * t1433 + t45 * t1999 + 0.8e1
		* t767 * t407 - 0.3e1 * t2003 + 0.8e1 * t282 * t2005 - 0.8e1 * t1235
		* t1114 + 0.16e2 * t210 * t400 + 0.2e1 * t461 * t44 *  t2 * t412
		- 0.8e1 * t29 * t678 - 0.8e1 * t1567 * t1064 - 0.2e1 * t55 * t1110
		+ 0.4e1 * t900 * t1974 - 0.8e1 * t361 * t2024 - 0.8e1 * t55 * t819;
	t2043 = t233 * t203;
	t2046 = t17 *  t2;
	t2051 = t16 * t153;
	t2052 =  t112 * t119;
	t2057 = t157 * t165 *  t4 * t159 * t163;
	t2071 =  t2 * t859 * t288;
	t2080 = -0.16e2 * t55 * t1679 * t1762 - 0.12e2 * t1401 * t871 - 0.2e1
		* t528 * t1914 - 0.8e1 * t587 * t1131 + 0.2e1 * t461 * t540 + 0.2e1
		* t528 * t1769 + 0.8e1 * t528 * t2043 + 0.2e1 * t2046 * t552 + 0.2e1
		* t244 * t1054 - 0.128e3 * t2051 * t2052 * t2057 - 0.16e2 * t143 * t1643
		- 0.2e1 * t668 * t803 - 0.16e2 * t515 * t1149 + 0.2e1 * t626 * t1495
		+ 0.8e1 * t1014 * t800 - t515 * t2071 + 0.8e1 * t456 * t894 + 0.32e2
		* t55 * t1679 * t659 - 0.8e1 * t374 * t705;
	t2122 = -0.2e1 * t1235 * t651 + 0.2e1 * t515 * t367 * t720 + 0.16e2
		* t1212 * t1648 + 0.16e2 * t55 * t989 + 0.2e1 * t2046 * t547 - 0.4e1
		* t431 * t1959 - 0.16e2 * t1527 * t1345 * t806 - 0.8e1 * t55 * t715
		- 0.2e1 * t431 * t631 - 0.8e1 * t461 * t623 + 0.2e1 * t576 * t448
		+ 0.8e1 * t1235 * t407 - 0.16e2 * t1722 * t1648 + 0.8e1 * t1003 * t848
		- 0.4e1 * t202 * t1633 + 0.2e1 * t1808 * t1049 + 0.8e1 * t244 * t1304
		+ 0.2e1 * t55 * t1106 - 0.32e2 * t328 * t700;
	t2123 = t219 * t1240;
	t2128 = t929 * d2;
	t2133 = t2052 * t75;
	t2139 = t219 * t1083;
	t2142 = t485 * t214;
	t2151 = t170 * t54;
	t2160 =  R * t73;
	t2161 = t683 * t187;
	t2162 = t2160 * t2161;
	t2173 = -0.2e1 * t528 * t2123 + 0.2e1 * t244 * t1433 - 0.2e1 * t927
		* t2128 + 0.2e1 * t1716 * t1620 - 0.128e3 * t2051 * t2133 * t123
		* t2 * t166 - 0.2e1 * t708 * t2139 - 0.8e1 * t1014 * t2142
		+ 0.2e1 * t243 * t1230 + 0.8e1 * t1808 * t1114 - 0.2e1 * t576 * t1160
		+ 0.2e1 * t2151 * t1110 - 0.8e1 * t668 * t880 + 0.8e1 * t1808 * t769
		+ 0.2e1 * t908 * t1333 + 0.32e2 * t1856 * t2162 + 0.2e1 * t1235 * t442
		+ 0.2e1 * t708 * t32 - 0.16e2 * t767 * t198 - 0.2e1 * t2151 * t974;
	t2179 = t144 * t228;
	t2197 = t1586 * d1;
	t2204 = t485 * t239;
	t2215 = t485 * t238;
	t2218 = t462 * t5;
	t2221 = -0.8e1 * t1527 * t2179 * t486 - 0.2e1 * t2151 * t1106 + 0.8e1
		* t1235 * t230 + 0.2e1 * t1158 * t1258 - 0.2e1 * t17 * t877 + 0.8e1
		* t1716 * t1615 + 0.16e2 * t244 * t1561 + 0.8e1 * t210 * t1016 + 0.4e1
		* t927 * t2197 - 0.16e2 * t143 * t1648 - 0.8e1 * t210 * t1844 - 0.8e1
		* t1014 * t2204 - 0.32e2 * t328 * t774 + 0.2e1 * t1158 * t448 - 0.2e1
		* t1808 * t381 + 0.4e1 * t184 * t1974 + 0.2e1 * t446 * t2215 + 0.8e1 * t668 * t2218;
	t2222 = t243 * t1292;
	t2225 = t1614 *  t2;
	t2243 = t159 * t3 * t5;
	t2251 = t430 * t2052;
	t2268 = -0.8e1 * t2222 * t2024 - 0.2e1 * t626 * t2225 + 0.8e1 * t389
		* t466 + 0.4e1 * t55 * t287 * t1432 * t46 + 0.16e2 * t1014 * t643
		+ 0.2e1 * t1716 * t556 - 0.8e1 * t1716 * t561 + 0.128e3 * t573 * t2057
		+ 0.8e1 * t243 * t2243 - t515 * t1581 + 0.2e1 * t927 * t1587 + 0.2e1
		* t446 * t594 + 0.8e1 * t2251 * t157 *  t124 * t132 + 0.16e2
		* t397 * t487 + 0.2e1 * t366 * t2071 - 0.2e1 * t389 * t803 + 0.2e1 * t55
		* t603 + 0.8e1 * t1716 * t2243 + 0.8e1 * t361 * t812;
	t2294 = t202 * t735;
	t2298 = t186 * t144 * t212;
	t2312 = 0.8e1 * t45 * t1799 + 0.8e1 * t55 * t1072 - 0.2e1 * t431
		* t815 + 0.2e1 * t55 * t974 - 0.8e1 * t1808 * t407 + 0.2e1 * t431
		* t1755 - 0.2e1 * t2222 * t480 + 0.8e1 * t244 * t695 + 0.8e1 * t923
		* t1890 - 0.8e1 * t389 * t880 - 0.8e1 * t1235 * t769 + 0.8e1 * t397
		* t215 + 0.8e1 * t2294 * t867 + 0.4e1 * t184 * t2298 - 0.4e1 * t461
		* t534 * t30 * t582 - 0.8e1 * t317 * t1416 - 0.8e1 * t1527
		* t2179 * t1428 + 0.4e1 * t461 * t1769;
	t2328 = t485 * t195;
	t2332 = t67 * t94 * t5;
	t2339 = t192 *  t2;
	t2342 = t170 * t1774;
	t2354 = t21 *  t2;
	t2357 = t70 * t953;
	t2361 = t170 * c2 * t18;
	t2364 = -0.2e1 * t282 * t639 + 0.4e1 * t431 * t1853 + 0.4e1 * t45
		* t910 - 0.2e1 * t256 * t442 - 0.2e1 * t256 * t1326 - 0.8e1 * t870
		* t1169 + 0.16e2 * t2294 * t865 * t1168 - 0.4e1 * t446 * t2328 - 0.64e2
		* t2332 * t2162 + 0.8e1 * t243 * t1117 * t262 * t1428 - 0.2e1 * t183
		* t2339 - 0.2e1 * t2342 * t552 - 0.2e1 * t874 * t1178 - 0.8e1 * t244
		* t1432 * t5 - 0.2e1 * t708 * t151 - 0.4e1 * t461 * t756 - 0.8e1 * t870
		* t2354 + 0.8e1 * t19 * t2357 + 0.32e2 * t2361 * t1058;
	t2368 =  t185 * d1 * t1006;
	t2371 = t795 *  t2;
	t2389 = t73 * t212;
	t2407 = -0.2e1 * t1585 * t2368 + 0.2e1 * t282 * t2371 + 0.8e1 * t172
		* t1165 + 0.8e1 * t172 * t1396 - t515 * t759 - 0.2e1 * t256 * t381
		- 0.2e1 * t1716 * t1495 - 0.4e1 * t384 * t1282 + 0.8e1 * t587 * t344
		- 0.2e1 * t708 * t612 + 0.8e1 * t1527 * t2389 * t486 - 0.4e1 * t446
		* t577 + 0.8e1 * t2251 * t705 + 0.2e1 * t431 * t594 - 0.8e1 * t282
		* t628 + 0.2e1 * t1808 * t424 - 0.2e1 * t256 * t352 + 0.8e1 * t361 * t1201;
	t2424 = t141 * t373;
	t2431 = t483 * t2052;
	t2434 = t141 * t327;
	t2451 = 0.2e1 * t626 * t1520 + 0.8e1 * t708 * t2043 + 0.8e1 * t55
		* t673 * t818 * t46 - 0.8e1 * t626 * t605 + 0.8e1 * t350 * t235 + 0.8e1
		* t908 * t1429 - 0.4e1 * t591 * t902 - 0.8e1 * t2424 * t377 - 0.4e1
		* t908 * t90 - 0.2e1 * t243 * t2371 + 0.32e2 * t2431 * t700 + 0.32e2
		* t2434 * t774 + 0.16e2 * t461 * t2043 - 0.16e2 * t908 * t1561 + 0.8e1
		* t708 * t746 + 0.8e1 * t626 * t284 - 0.4e1 * t576 * t1766 + 0.8e1
		* t1014 * t1255 - 0.2e1 * t1716 * t1293;
	t2475 = t203 * t194;
	t2478 = t203 * t288;
	t2489 = t233 * t414;
	t2494 = -0.8e1 * t1716 * t2005 - t515 * t1338 - 0.2e1 * t2342 * t547
		- 0.8e1 * t708 * t842 - 0.2e1 * t282 * t1230 - 0.16e2 * t767 * t1289
		- 0.16e2 * t767 * t812 + 0.4e1 * t461 * t32 + 0.32e2 * t2434 * t700
		+ 0.8e1 * t1527 * t2389 * t1428 + 0.2e1 * t461 * t1457 - 0.4e1 * t461
		* t2475 + 0.4e1 * t461 * t2478 + 0.2e1 * t350 * t1326 + 0.8e1 * t350
		* t336 - 0.8e1 * t626 * t2243 - 0.8e1 * t342 * t1114 - 0.4e1 * t361
		* t2489 + 0.4e1 * t1585 * t2197;
	t2510 = t57 * t294;
	t2539 = -0.2e1 * t533 * t877 + 0.8e1 * t749 * t2357 + 0.8e1 * t587
		* t1630 + 0.2e1 * t244 * t1477 - 0.4e1 * t978 * t211 * t49 - 0.4e1
		* t1029 * t678 + 0.2e1 * t490 * t584 + t490 * t2510 - 0.2e1 * t626
		* t1620 - 0.8e1 * t626 * t989 - 0.32e2 * t2332 * t1858 + 0.2e1 * t384
		* t381 - 0.4e1 * t55 * t584 + 0.4e1 * t461 * t150 * t302 + 0.2e1 * t626
		* t1084 + 0.2e1 * t17 * t2475 - 0.16e2 * t55 * t1142 * t1118 * d2
		- 0.4e1 * t55 * t57 * t475 * d2 + 0.8e1 * t282 * t561;
	t2553 = t73 * d1;
	t2569 =  t6 * t859;
	t2587 = 0.8e1 * t626 * t1380 - 0.2e1 * t29 * t203 - 0.2e1 * t204
		+ 0.16e2 * t767 * t2024 + 0.2e1 * t1808 * t935 - t1029 * t203 - 0.8e1
		* t1527 * t2553 * t1303 + 0.2e1 * t243 * t159 *  t2 - 0.2e1
		* t1003 * t616 - 0.4e1 * t1029 * t93 * t2354 - 0.2e1 * t29 * t223
		+ 0.8e1 * t1014 * t1844 + 0.4e1 * t515 * t2569 - 0.8e1 * t626 * t1072
		- 0.8e1 * t923 * t812 + 0.8e1 * t55 * t673 * t818 * t414 - 0.8e1 * t55
		* t1117 * t5 * t46 * t49 + 0.8e1 * t528 * t678;
	t2592 = t17 * t233;
	t2593 = t2160 * t683;
	t2602 = t21 * t187;
	t2614 =  t185 * t107;
	t2625 = t219 * t276 *  t4;
	t2637 = t157 *  t124 * t159 * t163;
	t2640 = -0.4e1 * t1029 * t1954 - 0.16e2 * t1212 * t1227 + 0.16e2
		* t2592 * t2593 - 0.16e2 * t384 * t1061 + 0.8e1 * t2424 * t705 + 0.2e1
		* t528 * t2478 + 0.8e1 * t927 * t2602 - 0.2e1 * t1158 * t962 + 0.2e1
		* t243 * t2225 - 0.8e1 * t55 * t561 + t29 * t1914 + 0.2e1 * t29 * t1102
		- 0.2e1 * t927 * t2614 * d1 + 0.2e1 * t456 * t367 * t35 * t276 + 0.2e1
		* t170 * cm * t3 * t2625 + 0.4e1 * t528 * t223 - 0.16e2 * t1343 * t682
		* t2161 - 0.2e1 * t533 * t223 + 0.8e1 * t394 * t2637;
	t2642 = t367 * t582;
	t2670 = t1090 * d1;
	t2682 = 0.2e1 * t515 * t2642 - 0.4e1 * t576 * t1853 + t1029 * t151
		+ 0.8e1 * t384 * t407 + 0.8e1 * t461 * t1092 + 0.2e1 * t389 * t1049
		- 0.2e1 * t243 * t1084 - 0.8e1 * t202 * t1954 - 0.2e1 * t533 * t1457
		- 0.2e1 * t528 * t1102 - 0.8e1 * t1343 * t684 - 0.2e1 * t1158 * t594
		- 0.2e1 * t1585 * t930 + 0.8e1 * t244 * t830 - 0.2e1 * t681 * t2670
		+ 0.8e1 * t1808 * t812 - 0.2e1 * t1808 * t442 - 0.2e1 * t927 * t2614
		* d2 + 0.2e1 * t250 * t974;
	t2717 = t202 *  t2;
	t2724 = 0.2e1 * t29 *  t30 * t647 + 0.2e1 * t533 * t1102
		+ 0.8e1 * t1014 * t1789 + 0.8e1 * t397 * t1929 - 0.4e1 * t431 * t577
		+ 0.4e1 * t978 * t211 * t261 - 0.4e1 * t767 * t938 - 0.4e1 * t1003
		* t466 + 0.8e1 * t626 * t437 - 0.8e1 * t282 * t1241 - 0.2e1 * t626
		* t759 - 0.2e1 * t1003 * t1694 - 0.16e2 * t210 * t1908 - 0.8e1 * t1808
		* t230 + 0.2e1 * t587 * t305 + 0.4e1 * t431 * t1766 - 0.2e1 * t2717
		* t1178 - 0.4e1 * t1158 * t1853 - 0.8e1 * t384 * t588;
	t2743 = t483 * t2133;
	t2755 = t355 * t20 * t187;
	t2758 = t173 * t414;
	t2768 = 0.32e2 * t1481 * t1734 - 0.2e1 * t384 * t1694 - 0.4e1 * t767
		* t935 - 0.4e1 * t767 * t424 - 0.16e2 * t1014 * t1908 - 0.2e1 * t1158
		* t433 - 0.4e1 * t587 * t1282 + 0.2e1 * t87 * t1316 - 0.64e2 * t2743
		* t123 * t355 * t357 - 0.2e1 * t1716 * t2371 - 0.8e1 * t1716 * t796
		+ 0.4e1 * t681 * t788 + 0.8e1 * t1401 * t2755 - t361 * t2758 + 0.2e1
		* t17 * t1921 - 0.4e1 * t1158 * t1766 + 0.2e1 * t576 * t631 + 0.4e1 * t591 * t1357;
	t2808 = t233 * t859 * t46;
	t2811 = 0.2e1 * t451 * t314 + t451 * t51 + 0.8e1 * t210 * t1255 +
		+ 0.8e1 * t1450 *  t185 * t825 * t123 - 0.2e1 * t82 * t2339 -
		- 0.4e1 * t767 * t651 - t927 * t1251 - 0.8e1 * t1014 * t215 + 0.2e1
		* t451 * t1970 + 0.32e2 * t2592 * t2162 + 0.8e1 * t1401 * t1602 + 0.8e1
		* t1716 * t605 - 0.4e1 * t668 * t914 - 0.64e2 * t2434 * t358 - 0.4e1
		* t461 * t31 * t194 * t3 + 0.16e2 * t1856 * t2593 - 0.4e1 * t446 * t1959
		+ 0.2e1 * t626 * t2371 + 0.8e1 * t366 * t2808;
	t2838 = t53 * t54 * t43;
	t2841 =  t311 * t1798;
	t2844 = t287 * t412;
	t2850 = t29 * t534;
	t2854 = t233 * t506;
	t2857 = -0.2e1 * t282 * t2225 + 0.16e2 * t55 * t1117 * t471 * t46
		+ 0.2e1 * t490 * t2642 + 0.2e1 * t1029 * t1657 - 0.2e1 * t576 * t433
		+ 0.8e1 * t389 * t1114 + 0.8e1 * t626 * t796 + 0.8e1 * t1374 * t1501
		- 0.8e1 * t282 * t1615 + 0.16e2 * t668 * t1061 - 0.8e1 * t490 * t989
		+ 0.4e1 * t2838 * t264 + 0.2e1 * t451 * t2841 - t490 * t2844 + 0.2e1
		* t446 * t433 - 0.8e1 * t1401 * t109 + 0.4e1 * t2850 * t279 - t927
		* t2625 - 0.4e1 * t1003 * t2854;
	t2858 = t432 * t5;
	t2859 = t145 * t2858;
	t2903 = 0.8e1 * t397 * t2859 + 0.2e1 * t681 * t1274 + 0.16e2 * t1527
		* t2553 * t560 - 0.8e1 * t87 * t830 - 0.8e1 * t210 * t2859 - 0.16e2
		* t1014 * t1763 - 0.2e1 * t533 * t458 - 0.8e1 * t1389 * t1429 + 0.8e1
		* t55 * t57 * t493 * d1 - 0.2e1 * t1716 * t1899 + 0.8e1 * t888 * t5
		* t56 * t24 - 0.2e1 * t250 * t970 + 0.4e1 * t1158 * t577 - 0.16e2 * t210
		* t1763 - 0.8e1 * t317 * t2354 + 0.8e1 * t923 * t407 - 0.8e1 * t1401
		* t1738 + 0.16e2 * t1389 * t1561 + 0.4e1 * t1389 * t90;
	t2918 = t29 * t18;
	t2941 = t243 * t988;
	t2946 = 0.2e1 * t1389 * t1477 + 0.2e1 * t461 * t223 + 0.4e1 * t1158
		* t2328 - 0.2e1 * t250 * t1110 + 0.8e1 * t870 * t1602 + 0.8e1 * t2918
		* t25 - 0.8e1 * t668 * t1890 + 0.16e2 * t1527 * t1550 * t486 - t515
		* t2844 - 0.2e1 * t1158 * t1755 + 0.2e1 * t1003 * t1123 - 0.8e1 * t226
		* t2024 + 0.8e1 * t587 * t1087 + 0.8e1 * t708 * t1954 - 0.16e2 * t668
		* t1630 + 0.2e1 * t141 * t2339 - 0.2e1 * t2941 * t480 - 0.2e1 * t1158 * t2215;
	t2987 = 0.4e1 * t978 * t211 * t23 + 0.8e1 * t29 * t3 * t2602 + 0.8e1
		* t361 * t1289 - t587 * t1688 - 0.8e1 * t2251 * t2637 - 0.2e1 * t908
		* t1477 + 0.4e1 * t183 * t3 * t1000 + 0.8e1 * t350 * t230 + 0.2e1 * t923
		* t442 + 0.8e1 * t923 * t230 + 0.8e1 * t533 * t827 - 0.8e1 * t908
		* t830 - 0.12e2 * t1401 * t2354 - 0.2e1 * t226 * t1326 + 0.16e2 * t461
		* t1954 + 0.16e2 * t461 * t678 + 0.32e2 * t19 * t1734 + 0.2e1 * t250
		* t1106 + 0.8e1 * t870 * t2755;
	t3007 = t17 * t219;
	t3026 =  t30 * t519 * t523;
	t3030 = t1506 * t22;
	t3035 = 0.16e2 * t2838 * t308 + 0.2e1 * t461 * t458 + 0.4e1 * t45
		* t453 - 0.3e1 * t2046 * t1178 - 0.2e1 * t626 * t1230 - 0.2e1 * t908
		* t1054 - 0.8e1 * t45 * t695 + 0.16e2 * t1014 * t660 - 0.2e1 * t1585
		* t2128 + 0.2e1 * t3007 * t1163 * t546 + 0.8e1 * t1856 * t550 * t1168
		- t587 * t1702 + 0.2e1 * t389 * t386 - 0.2e1 * t515 * t1620 - 0.8e1
		* t908 * t1304 - 0.64e2 * t2743 * t123 *  t30 * t330 - 0.8e1
		* t2850 * t3026 + 0.4e1 * t29 * t219 * t3030 + 0.2e1 * t456 * t1447;
	t3075 = -0.2e1 * t587 * t616 - 0.2e1 * t626 * t556 - 0.8e1 * t250
		* t1241 + 0.2e1 * t626 * t1899 + 0.16e2 * t210 * t487 + 0.8e1 * t708
		* t678 - 0.4e1 * t226 * t2489 - 0.8e1 * t282 * t2243 + 0.2e1 * t668
		* t1694 - 0.8e1 * t1014 * t240 - 0.2e1 * t515 * t367 * t538 + 0.8e1
		* t1389 * t830 + 0.16e2 * t1212 * t807 - 0.4e1 * t786 * t1545 + 0.4e1
		* t1148 * t2569 - 0.2e1 * t250 * t967 - 0.8e1 * t366 * t1241 - 0.8e1
		* t461 * t842 - 0.4e1 * t515 * t2808;
	t3116 = 0.8e1 * t2151 * t715 + 0.2e1 * t3007 * t1394 *  q
		* t187 + 0.2e1 * t226 * t884 - 0.2e1 * t461 * t150 * t615 - 0.2e1
		* t1235 * t935 - 0.2e1 * t927 * t2368 + 0.8e1 * t2151 * t792 + 0.2e1
		* t2151 * t970 - 0.2e1 * t226 * t1871 + 0.8e1 * t350 * t258 - 0.16e2
		* t587 * t1061 + 0.2e1 * t2151 * t967 - 0.8e1 * t461 * t710 - 0.8e1
		* t2151 * t665 - 0.8e1 * t1235 * t812 + 0.8e1 * t87 * t1312 - 0.4e1
		* t490 * t1174 + t451 * t1331;
	t3160 = -0.12e2 * t17 * t1954 + 0.8e1 * t210 * t800 + 0.16e2 * t888
		* t1099 + 0.4e1 * t2850 * t1575 + 0.2e1 * t67 * t543 * t20 - 0.2e1 * t55
		* t287 * t413 * t46 - 0.4e1 * t978 * t145 * t212 - 0.2e1 * t2717
		* t1989 + 0.2e1 * t461 * t534 *  t30 * t601 + 0.2e1 * t243
		* t1620 + 0.2e1 * t626 * t1293 + 0.8e1 * t626 * t2005 - 0.2e1 * t923
		* t935 + 0.8e1 * t87 * t1429 + 0.2e1 * t87 * t1333 + 0.16e2 * t101
		* t1416 + 0.2e1 * t55 * t1581 + 0.8e1 * t350 * t407 - t1003 * t386;
	t3204 = 0.2e1 * t1389 * t1054 + 0.2e1 * t45 * t1043 + 0.16e2 * t55
		* t1679 * t1015 + 0.4e1 * t172 * t516 - 0.2e1 * t226 * t480 - 0.8e1
		* t430 * t2133 * t123 *  t4 * t376 + 0.2e1 * t226 * t938 + 0.8e1
		* t1389 * t1304 + 0.2e1 * t576 * t815 + 0.8e1 * t2294 * t865 * t1636
		- t226 * t2758 - 0.4e1 * t1389 * t314 + 0.2e1 * t461 * t1447 + 0.16e2
		* t2592 * t1858 + 0.8e1 * t528 * t1954 - 0.2e1 * t587 * t1694 - 0.2e1
		* t668 * t442 + 0.8e1 * t193 * t1114 + 0.8e1 * t668 * t588;
	t3205 = t17 * t227;
	t3215 = t2160 * t836;
	t3249 = -0.16e2 * t3205 * t1980 - 0.4e1 * t202 *  t185 * t2670
		- 0.4e1 * t2838 * t1043 - 0.8e1 * t1472 * t109 - 0.32e2 * t3205
		* t3215 + 0.2e1 * t1716 * t759 - 0.4e1 * t45 * t90 + 0.8e1 * t210
		* t1789 + 0.16e2 * t2918 * t268 - 0.2e1 * t45 * t1477 - 0.8e1 * t1014
		* t2859 - 0.8e1 * t55 * t792 + 0.2e1 * t490 * t1899 + 0.8e1 * t19
		* t1934 - 0.16e2 * t456 * t227 *  R * t144 - 0.8e1 * t461 * t445
		* t1404 + 0.8e1 * t626 * t561 + 0.2e1 * t193 * t935 - 0.8e1 * t2941 * t2024;
	t3292 = -0.2e1 * t1716 * t1084 + 0.16e2 * t317 * t1405 + 0.2e1 * t17
		* t1660 - 0.2e1 * t708 * t1102 + 0.2e1 * t533 * t1921 + 0.4e1 * t55
		* t287 * t88 * d2 + 0.2e1 * t202 * t2475 - 0.2e1 * t389 * t442 + 0.4e1
		* t900 * t1139 - 0.8e1 * t461 * t827 + 0.8e1 * t250 * t1380 - 0.16e2
		* t1722 * t1643 - 0.4e1 * t900 * t1465 + 0.8e1 * t193 * t812 - 0.8e1
		* t1716 * t1380 + 0.2e1 * t874 * t547 + 0.8e1 * t515 *  t6
		* t621 - 0.16e2 * t2838 * t566;
	t3333 = -t1249 * t2625 + 0.2e1 * t226 * t651 + 0.2e1 * t17 * t1657
		- 0.2e1 * t170 * c2 *  t185 * t1274 - 0.2e1 * t576 * t1755
		- 0.2e1 * t193 * t442 + 0.2e1 * t708 * t2123 - 0.32e2 * t1979 * t3215
		- 0.8e1 * t1716 * t437 - 0.2e1 * t456 * t1921 - 0.3e1 * t17 * t32
		+ 0.8e1 * t1389 * t695 + t451 * t1999 + 0.2e1 * t87 * t51 - 0.2e1 * t451
		* t264 - 0.4e1 * t587 * t1297 + 0.8e1 * t374 * t2637 + 0.2e1 * t461
		* t203 * t1701 + 0.16e2 * t55 * t1142 * t2858;
	t3374 = -0.2e1 * t243 * t1495 - 0.16e2 * t55 * t437 + 0.2e1 * t456
		* t223 - t515 * t1826 + 0.8e1 * t587 * t880 + 0.2e1 * t1003 * t305
		+ 0.8e1 * t45 * t1823 - 0.2e1 * t708 * t463 - 0.16e2 * t55 * t1679
		* t818 * d1 - 0.8e1 * t923 * t1554 + 0.2e1 * t587 * t803 - 0.8e1 * t533
		* t1092 + 0.2e1 * t282 * t1293 + 0.2e1 * t1389 * t264 + 0.4e1 * t978
		* t145 * t61 + 0.8e1 * t1808 * t198 - 0.16e2 * t451 * t1561 + t515
		* t2510 - 0.32e2 * t101 * t1405;
	t3415 = -0.16e2 * t101 * t323 + 0.2e1 * t1716 * t2225 - 0.4e1 * t587
		* t466 + 0.128e3 * t156 * t2057 - 0.8e1 * t668 * t230 + 0.16e2 * t301
		* t1131 - 0.8e1 * t1031 * t3026 + 0.2e1 * t1716 * t639 + 0.8e1 * t397
		* t2142 - 0.2e1 * t456 * t2123 - 0.8e1 * t1014 * t1929 + 0.4e1 * t1158
		* t1959 + 0.8e1 * t87 * t910 - 0.16e2 * t55 * t1142 * t1118 * d1 + 0.4e1
		* t668 * t1282 + 0.32e2 * t2431 * t774 + 0.2e1 * t2003 * t206 - 0.8e1
		* t317 * t871 - 0.4e1 * t389 * t914;
	t3455 = -0.16e2 * t389 * t1630 - 0.8e1 * t923 * t2218 + 0.2e1 * t282
		* t1899 - 0.2e1 * t528 * t463 + 0.8e1 * t397 * t2204 - 0.8e1 * t342
		* t466 + 0.2e1 * t1371 * t547 + 0.2e1 * t226 * t935 - 0.8e1 * t451
		* t308 - 0.4e1 * t587 * t2854 - 0.2e1 * t446 * t1160 + 0.8e1 * t1716
		* t628 + 0.2e1 * t456 * t540 - 0.4e1 * t1003 * t1282 - 0.16e2 * t1527
		* t1528 * t486 - 0.8e1 * t1472 * t1738 + 0.8e1 * t1389 * t308 - 0.8e1 * t1031 * t525;
	t3495 = -t587 * t386 - 0.2e1 * t243 * t1520 + 0.2e1 * t456 * t877
		+ 0.16e2 * t1722 * t147 + 0.16e2 * t1722 * t1227 - 0.8e1 * t2361 * t25
		- 0.8e1 * t361 * t1637 - 0.2e1 * t1235 * t424 - 0.2e1 * t431 * t448
		+ 0.4e1 * t461 * t20 *  t185 * t205 - 0.16e2 * t1722 * t807
		+ 0.8e1 * t1472 * t1602 + 0.2e1 * t1235 * t381 + 0.2e1 * t282 * t1520
		+ 0.2e1 * t243 * t759 + 0.4e1 * t461 * t445 * t1415 - t1148 * t2071
		- 0.8e1 * t101 * t1602 - 0.2e1 * t87 * t1433;
	t3535 = 0.8e1 * t1003 * t1630 + 0.16e2 * t1343 * t837 + 0.2e1 * t528
		* t32 - t490 * t603 - 0.128e3 * t1436 * t2057 + 0.2e1 * t350 * t442
		+ 0.2e1 * t456 * t2139 - 0.16e2 * t301 * t344 - 0.32e2 * t1937 * t1058
		+ 0.4e1 * t3007 * t3030 - 0.8e1 * t366 * t1396 - 0.3e1 * t2046 * t1989
		- 0.2e1 * t282 * t759 + 0.2e1 * t342 * t803 - 0.4e1 * t17 * t1633
		- 0.8e1 * t1716 * t284 - 0.8e1 * t528 *  t740 * t267 + 0.2e1
		* t350 * t381 - 0.8e1 * t2850 * t525;
	t3575 = -0.4e1 * t1148 * t2808 - 0.16e2 * t1590 * t1227 - 0.4e1 * t576
		* t2328 + 0.8e1 * t366 * t1380 - 0.8e1 * t456 * t746 - 0.16e2 * t1722
		* t1214 - 0.8e1 * t2424 * t2637 - 0.64e2 * t2434 * t331 + 0.4e1 * t900
		* t2298 + 0.2e1 * t528 * t2139 + 0.8e1 * t342 * t880 - 0.4e1 * t591
		* t2298 - 0.4e1 * t366 * t516 - 0.2e1 * t366 * t252 - 0.4e1 * t461
		* t445 * t322 + 0.2e1 * t45 * t2841 + 0.16e2 * t1212 * t1643 - 0.2e1
		* t55 * t970 + 0.2e1 * t533 * t722;
	t3592 = -0.6e1 * 0.3141592654e1 *  R * (t2987 + t2494 + t2221
		+ t1737 + t1784 + t422 + t2682 + t3415 + t687 + t3075 + t1538 + t1690
		+ t339 + t851 + t3249 + t1427 + t2724 + t2587 + t3455 + t1484 + t2768
		+ t2407 + t1831 + t3495 + t597 + t2029 + t3035 + t2451 + t2811 + t994
		+ t218 + t2080 + t1642 + t3116 + t1186 + t3160 + t3374 + t933 + t2857
		+ t1882 + t3333 + t1364 + t1986 + t1593 + t503 + t2173 + t2312 + t3575
		+ t3204 + t1238 + t1302 + t2539 + t2903 + t2268 + t2364 + t3535
		+ t2640 + t780 + t1126 + t1057 + t3292 + t1932 + t2122 + t2946)
		/ t153 / t11 /  t116 /  t8;

	return 4./3.*M_PI*(R*R+s*s)*(R*R+2.*s*s)/R* t3592;
	// The analytical formular for ??
}

