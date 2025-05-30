/*
 * src/sasfit_old/ei.c
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
 */

#include <math.h>
#include "include/SASFIT_nr.h"

float ei(float x)
{
	float chepolsum(int, float, float []);
	float pol(int, float, float []);
	float jfrac(int, float [], float []);
	float p[8],q[8];

	if (x > 24.0) {
		p[0]=  1.00000000000058;     q[1] =  1.99999999924131;
		p[1]=x-3.00000016782085;     q[2] = -2.99996432944446;
		p[2]=x-5.00140345515924;     q[3] = -7.90404992298926;
		p[3]=x-7.49289167792884;     q[4] = -4.31325836146628;
		p[4]=x-3.08336269051763e1;   q[5] =  2.95999399486831e2;
		p[5]=x-1.39381360364405;     q[6] = -6.74704580465832;
		p[6]=x+8.91263822573708;     q[7] =  1.04745362652468e3;
		p[7]=x-5.31686623494482e1;
		return exp(x)*(1.0+jfrac(7,q,p)/x)/x;
	} else if (x > 12.0) {
		p[0]=  9.99994296074708e-1;  q[1] =  1.00083867402639;
		p[1]=x-1.95022321289660;     q[2] = -3.43942266899870;
		p[2]=x+1.75656315469614;     q[3] =  2.89516727925135e1;
		p[3]=x+1.79601688769252e1;   q[4] =  7.60761148007735e2;
		p[4]=x-3.23467330305403e1;   q[5] =  2.57776384238440e1;
		p[5]=x-8.28561994140641;     q[6] =  5.72837193837324e1;
		p[6]=x-1.86545454883399e1;   q[7] =  6.95000655887434e1;
		p[7]=x-3.48334653602853;
		return exp(x)*jfrac(7,q,p)/x;
	} else if (x > 6.0) {
		p[0]=  1.00443109228078;     q[1] =  5.27468851962908e-1;
		p[1]=x-4.32531132878135e1;   q[2] =  2.73624119889328e3;
		p[2]=x+6.01217990830080e1;   q[3] =  1.43256738121938e1;
		p[3]=x-3.31842531997221e1;   q[4] =  1.00367439516726e3;
		p[4]=x+2.50762811293561e1;   q[5] = -6.25041161671876;
		p[5]=x+9.30816385662165;     q[6] =  3.00892648372915e2;
		p[6]=x-2.19010233854880e1;   q[7] =  3.93707701852715;
		p[7]=x-2.18086381520724;
		return exp(x)*jfrac(7,q,p)/x;
	} else if (x > 0.0) {
		float t,r,x0,xmx0;
		p[0]= -1.95773036904548e8;   q[0] = -8.26271498626055e7;
		p[1]=  3.89280421311201e6;   q[1] =  8.91925767575612e7;
		p[2]= -2.21744627758845e7;   q[2] = -2.49033375740540e7;
		p[3]= -1.19623669349247e5;   q[3] =  4.28559624611749e6;
		p[4]= -2.49301393458648e5;   q[4] = -4.83547436162164e5;
		p[5]= -4.21001615357070e3;   q[5] =  3.57300298058508e4;
		p[6]= -5.49142265521085e2;   q[6] = -1.60708926587221e3;
		p[7]= -8.66937339951070;     q[7] =  3.41718750000000e1;
		x0=0.372507410781367;
		t=x/3.0-1.0;
		r=chepolsum(7,t,p)/chepolsum(7,t,q);
		xmx0=(x-409576229586.0/1099511627776.0)-
				0.767177250199394e-12;
		if (fabs(xmx0) > 0.037)
			t=log(x/x0);
		else {
			float z,z2;
			p[0] =  0.837207933976075e1;
			p[1] = -0.652268740837103e1;
			p[2] =  0.569955700306720;
			q[0] =  0.418603966988037e1;
			q[1] = -0.465669026080814e1;
			q[2] =  0.1e1;
			z=xmx0/(x+x0);
			z2=z*z;
			t=z*pol(2,z2,p)/pol(2,z2,q);
		}
		return t+xmx0*r;
	} else if (x > -1.0) {
		float y;
		p[0] = -4.41785471728217e4;  q[0]=7.65373323337614e4;
		p[1] =  5.77217247139444e4;  q[1]=3.25971881290275e4;
		p[2] =  9.93831388962037e3;  q[2]=6.10610794245759e3;
		p[3] =  1.84211088668000e3;  q[3]=6.35419418378382e2;
		p[4] =  1.01093806161906e2;  q[4]=3.72298352833327e1;
		p[5] =  5.03416184097568;    q[5]=1.0;
		y = -x;
		return log(y)-pol(5,y,p)/pol(5,y,q);
	} else if (x > -4.0) {
		float y;
		p[0]=8.67745954838444e-8;  q[0]=1.0;
		p[1]=9.99995519301390e-1;  q[1]=1.28481935379157e1;
		p[2]=1.18483105554946e1;   q[2]=5.64433569561803e1;
		p[3]=4.55930644253390e1;   q[3]=1.06645183769914e2;
		p[4]=6.99279451291003e1;   q[4]=8.97311097125290e1;
		p[5]=4.25202034768841e1;   q[5]=3.14971849170441e1;
		p[6]=8.83671808803844;     q[6]=3.79559003762122;
		p[7]=4.01377664940665e-1;  q[7]=9.08804569188869e-2;
		y = -1.0/x;
		return -exp(x)*pol(7,y,p)/pol(5,y,q);
	} else {
		float y;
		p[0] = -9.99999999998447e-1;  q[0]=1.0;
		p[1] = -2.66271060431811e1;   q[1]=2.86271060422192e1;
		p[2] = -2.41055827097015e2;   q[2]=2.92310039388533e2;
		p[3] = -8.95927957772937e2;   q[3]=1.33278537748257e3;
		p[4] = -1.29885688756484e3;   q[4]=2.77761949509163e3;
		p[5] = -5.45374158883133e2;   q[5]=2.40401713225909e3;
		p[6] = -5.66575206533869;     q[6]=6.31657483280800e2;
		y = -1.0/x;
		return -exp(x)*y*(1.0+y*pol(6,y,p)/pol(5,y,q));
	}
}

