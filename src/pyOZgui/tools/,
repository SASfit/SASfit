# -*- coding: utf-8 -*-
"""
Two-Yukawa MSA structure factor -- ANALYTIC REFERENCE (not independent).

    from two_yukawa_reference import twoYukawa
    Sq = twoYukawa(q, radius, K1, K2, Z1, Z2, phi)

q is Q*(2*radius)-free: it is in the same inverse units as `radius`. K > 0 is
ATTRACTION, K < 0 repulsion, and Z_i are inverse screening lengths.

WHY THIS FILE EXISTS HERE.

The manuscript's validation table quotes agreement with jscatter's two-Yukawa
MSA at 0.15 to 0.28 per cent. Those numbers were measured where jscatter was
installed, and jscatter's dependency chain -- MDAnalysis, jupyterlab, nglview,
pdb2pqr, netcdf4 -- does not build under MinGW, so the check could not be
reproduced on the machine the paper is written on. A validation claim that its
own authors cannot re-run is not much of a claim.

This module is the two-Yukawa solver on its own, needing only numpy. It makes
that row reproducible.

PROVENANCE, which matters more than usual here.

  * the derivation is Liu, Chen & Chen, J. Chem. Phys. 122, 044507 (2005),
    doi:10.1063/1.1830433 -- the paper asks to be cited by anyone using
    results from this code, and that request is passed on;
  * the algorithm is Yun Liu's own MATLAB, converted to C by Marcus Hennig
    (2010), to Igor by S. R. Kline (2010), and to Python for jscatter by
    Ralf Biehl (2020);
  * the file is taken verbatim from jscatter.libs.Two_Yukawa, which descends
    from the NIST SANS & USANS reduction package, described as "freely
    available" in doi:10.1107/S0021889806035059.

LICENCE.

    This code may be redistributed and modified, provided that Liu, Chen &
    Chen, J. Chem. Phys. 122, 044507 (2005) is cited in any publication
    using results obtained from it.

Granted in writing by the authors on request, 2026. Citation-ware: the one
condition is the citation, and it is honoured in the module docstring, the
report, the manuscript and this header. Keep the correspondence with the
project records -- the grant is what lets a downstream packager ship the file,
and without it the default is all rights reserved however freely the code
circulates in practice.

If anyone later wants a GENUINELY independent check of the paper's equations,
this file cannot provide one: everything here traces to the authors' own
MATLAB (see the provenance above), so agreement with it tests our
discretisation and not their algebra. A fresh implementation from the
published equations would be the stronger validation, and is worth doing on
its own merits rather than for licensing reasons, which no longer apply.

CONSTRAINT FROM THE ORIGINAL AUTHORS, quoted because it is easy to violate:
Z1 > Z2 is REQUIRED. With Z1 < Z2 the calculation still returns something
that looks like a structure factor and is wrong -- the mathematics treats the
two screening lengths asymmetrically. The Igor version added an automatic
lower limit on Z1 for exactly this reason. Check it rather than trust it.

Returns 0 (a number, not an array) on failure to find a physical root; the
two-Yukawa MSA admits several, and the original code selects among them via
g(r), which is why a wide q range is needed.
"""

import inspect

import numpy as np

# shortcuts to prevent editing of later equations
sin = np.sin
exp = np.exp
cos = np.cos
pi = np.pi


class global_data(object):
    # class to store global parameters that are named TY_xxx in original Igor .ipf
    # we use her TY.xxx
    names = """TY_q22, TY_qa12, TY_qa21, TY_qa22, TY_qa23, TY_qa32,
    TY_qb12, TY_qb21, TY_qb22, TY_qb23, TY_qb32,
    TY_qc112, TY_qc121, TY_qc122, TY_qc123, TY_qc132,
    TY_qc212, TY_qc221, TY_qc222, TY_qc223, TY_qc232,
    TY_A12, TY_A21, TY_A22, TY_A23, TY_A32, TY_A41, TY_A42, TY_A43, TY_A52,
    TY_B12, TY_B14, TY_B21, TY_B22, TY_B23, TY_B24, TY_B25, TY_B32, TY_B34,
    TY_F14, TY_F16, TY_F18, TY_F23, TY_F24, TY_F25, TY_F26, TY_F27, TY_F28, 
    TY_F29, TY_F32, TY_F33, TY_F34, TY_F35, TY_F36, TY_F37, TY_F38, TY_F39, TY_F310,
    TY_G13, TY_G14, TY_G15, TY_G16, TY_G17, TY_G18, TY_G19, TY_G110, TY_G111, TY_G112, 
    TY_G113, TY_G22, TY_G23, TY_G24, TY_G25, TY_G26, TY_G27, TY_G28, TY_G29, 
    TY_G210, TY_G211, TY_G212, TY_G213, TY_G214"""

    def __init__(self):
        self.attr = [name.split('_')[1] for name in self.names.replace(',', '').split()]
        for attr in self.attr:
            setattr(self, attr, 0.)
        setattr(self, 'w', np.zeros(23))

    def __getattr__(self, attrib):
        return object.__getattr__(self, name)

    def __setattr__(self, name, value):
        object.__setattr__(self, name, value)

    @property
    def getall(self):
        return [getattr(self, attr) for attr in self.attr]


# initialize a global data stack
TY = global_data()


def twoYukawa(q, radius, K1, K2, Z1, Z2, phi):
    # assure float input parameters
    K1, K2, Z1, Z2, radius, phi = np.array([K1, K2, Z1, Z2, radius, phi], dtype=float)

    # make sure that none of the values are too close to zero
    # make them very small instead
    if abs(K1) < 0.001: K1 = 0.001
    if abs(K2) < 0.001: K2 = 0.001
    if abs(Z1) < 0.001: Z1 = 0.001
    if abs(Z2) < 0.001: Z2 = 0.001
    # Z1 == Z2 not allowed, this may not be enough of a correction
    # RB: added new correction that gives approximate close solution in some cases,
    #     at least better than Igor solution if that worked at all.
    if Z1 == Z2:
        Z1 *= 2  # arbitrary larger Z1 value
        K2 = (K1 + K2)
        K1 = 0.001 * K2  # with K1 small compared to K2

    # Z1 > Z2, otherwise swap (remark SRK Jan 2014 in original igor file)
    if Z1 < Z2:
        Z1, Z2 = Z2, Z1
        K1, K2 = K2, K1  # RB: This should make it more consistent for fits as only order changes not Zi,Ki relation.

    ok = TY_SolveEquations(Z1, Z2, K1, K2, phi)  # ok = (a,b,c1,c2,d1,d2) are returned
    # if OK then ok contains list of values (a, b, c1, c2, d1, d2) that evaluate to True, otherwise it is  (False)
    if isinstance(ok, str) or not ok:
        # error
        print('Output TY_SolveEquation: ', ok)
        return 0
    else:
        a, b, c1, c2, d1, d2 = ok
        # check returns 0 if all test are bad ??
        # this is a check that was not used in Igor, RB: I guess ist because of numerical inaccuracy (test ==0)
        # that it often fails
        # check = TY_CheckSolution( Z1, Z2, K1, K2, phi, a, b, c1, c2, d1, d2 )

        # just take best solution even if check is not true
        Sq = SqTwoYukawa(q * radius * 2, Z1, Z2, K1, K2, phi, [a, b, c1, c2, d1, d2])

    return Sq


##########/ converted procedures from c-code ##############
# there were two functions defined as TY_q: one as TY_Q and one as TY_q.
# I renamed the TY_Q function as TY_capQ, and left TY_q unchanged
# function TY_W change to TY_capW, since there is a wave named TY_w

def chop(x):
    # avoid small nonzero numbers
    return 0 if (abs(x) < 1e-6) else x


def pow(a, b):
    return a ** b


# /*
# ==================================================================================================
#
# The two-yukawa structure factor is uniquely determined by 6 parameters a, b, c1, c2, d1, d2,
# which are the solution of a system of 6 equations ( 4 linear, 2 nonlinear ). The solution can
# constructed by the roots of a polynomial of 22nd degree. For more details see attached
# Mathematica notebook, where a derivation is given
#
# ==================================================================================================
# */


def TY_sigma(s, Z1, Z2, a, b, c1, c2, d1, d2):
    return -(a / 2. + b + c1 * exp(-Z1) + c2 * exp(-Z2)) / s + a * pow(s, -3) + b * pow(s, -2) + (c1 + d1) * pow(s + Z1,
                                                                                                                 -1) + (
                       c2 + d2) * pow(s + Z2, -1)


def TY_tau(s, Z1, Z2, a, b, c1, c2):
    return b * pow(s, -2) + a * (pow(s, -3) + pow(s, -2)) - pow(s, -1) * (
                c1 * Z1 * exp(-Z1) * pow(s + Z1, -1) + c2 * Z2 * exp(-Z2) * pow(s + Z2, -1))


def TY_q(s, Z1, Z2, a, b, c1, c2, d1, d2):
    return TY_sigma(s, Z1, Z2, a, b, c1, c2, d1, d2) - exp(-s) * TY_tau(s, Z1, Z2, a, b, c1, c2)


def TY_g(s, phi, Z1, Z2, a, b, c1, c2, d1, d2):
    return s * TY_tau(s, Z1, Z2, a, b, c1, c2) * exp(-s) / (1 - 12 * phi * TY_q(s, Z1, Z2, a, b, c1, c2, d1, d2))


# /*
# ==================================================================================================
#
# Structure factor for the potential
#
# V(r) = -kB * T * ( K1 * exp[ -Z1 * (r - 1)] / r + K2 * exp[ -Z2 * (r - 1)] / r ) for r > 1
# V(r) = inf for r <= 1
#
# The structure factor is parametrized by (a, b, c1, c2, d1, d2)
# which depend on (K1, K2, Z1, Z2, phi).
#
# ==================================================================================================
# */

def TY_hq(qq, Z, K, v):
    result = np.ones_like(qq)

    result[qq == 0] = (exp(-2. * Z) * (v + (v * (-1. + Z) - 2. * K * Z) * exp(Z)) * (
                -(v * (1. + Z)) + (v + 2. * K * Z * (1. + Z)) * exp(Z)) * pow(K, -1) * pow(Z, -4)) / 4.

    # variable t1, t2, t3, t4
    q = qq[qq > 0]
    t1 = (1. - v / (2. * K * Z * exp(Z))) * ((1. - cos(q)) / (q * q) - 1. / (Z * Z + q * q))
    t2 = (v * v * (q * cos(q) - Z * sin(q))) / (4. * K * Z * Z * q * (Z * Z + q * q))
    t3 = (q * cos(q) + Z * sin(q)) / (q * (Z * Z + q * q))
    t4 = v / (Z * exp(Z)) - v * v / (4. * K * Z * Z * exp(2. * Z)) - K
    result[qq > 0] = v / Z * t1 - t2 + t3 * t4

    return result


def TY_pc(qq, Z1, Z2, K1, K2, phi, a, b, c1, c2, d1, d2):
    t1 = np.ones_like(qq)
    t2 = np.ones_like(qq)
    t3 = np.ones_like(qq)

    v1 = 24. * phi * K1 * exp(Z1) * TY_g(Z1, phi, Z1, Z2, a, b, c1, c2, d1, d2)
    v2 = 24. * phi * K2 * exp(Z2) * TY_g(Z2, phi, Z1, Z2, a, b, c1, c2, d1, d2)
    a0 = a * a
    b0 = -12. * phi * (pow(a + b, 2) / 2. + a * (c1 * exp(-Z1) + c2 * exp(-Z2)))

    t4 = TY_hq(qq, Z1, K1, v1) + TY_hq(qq, Z2, K2, v2)
    # variable t1, t2, t3
    t1[qq == 0] = a0 / 3.
    t2[qq == 0] = b0 / 4.
    t3[qq == 0] = a0 * phi / 12.

    q = qq[qq > 0]
    t1[qq > 0] = a0 * (sin(q) - q * cos(q)) / pow(q, 3)
    t2[qq > 0] = b0 * (2. * q * sin(q) - (q * q - 2.) * cos(q) - 2.) / pow(q, 4)
    t3[qq > 0] = a0 * phi * ((q * q - 6.) * 4. * q * sin(q) - (pow(q, 4) - 12. * q * q + 24.) * cos(q) + 24.) / (
                2. * pow(q, 6))

    return -24. * phi * (t1 + t2 + t3 + t4)


def SqTwoYukawa(q, Z1, Z2, K1, K2, phi, abc1c2d1d2):
    # for numpy q is an array and abc1c2d1d2 is a list of the values
    # we catch q==0 in the above functions
    a, b, c1, c2, d1, d2 = abc1c2d1d2
    if Z1 == Z2:
        # one-yukawa potential
        return 0
    else:
        # two-yukawa potential
        return 1. / (1. - TY_pc(q, Z1, Z2, K1, K2, phi, a, b, c1, c2, d1, d2))


# /*
# ==================================================================================================
#
# Non-linear equation system that determines the parameter for structure factor
#
# ==================================================================================================
# */

def TY_LinearEquation_1(Z1, Z2, K1, K2, phi, a, b, c1, c2, d1, d2):
    return b - 12. * phi * (-a / 8. - b / 6. + d1 * pow(Z1, -2) + c1 * (
                pow(Z1, -2) - exp(-Z1) * (0.5 + (1. + Z1) * pow(Z1, -2))) + d2 * pow(Z2, -2) + c2 * (
                                        pow(Z2, -2) - exp(-Z2) * (0.5 + (1. + Z2) * pow(Z2, -2))))


def TY_LinearEquation_2(Z1, Z2, K1, K2, phi, a, b, c1, c2, d1, d2):
    return 1. - a - 12. * phi * (-a / 3. - b / 2. + d1 * pow(Z1, -1) + c1 * (
                pow(Z1, -1) - (1. + Z1) * exp(-Z1) * pow(Z1, -1)) + d2 * pow(Z2, -1) + c2 * (
                                             pow(Z2, -1) - (1. + Z2) * exp(-Z2) * pow(Z2, -1)))


def TY_LinearEquation_3(Z1, Z2, K1, K2, phi, a, b, c1, c2, d1, d2):
    return K1 * exp(Z1) - d1 * Z1 * (1. - 12. * phi * TY_q(Z1, Z1, Z2, a, b, c1, c2, d1, d2))


def TY_LinearEquation_4(Z1, Z2, K1, K2, phi, a, b, c1, c2, d1, d2):
    return K2 * exp(Z2) - d2 * Z2 * (1. - 12. * phi * TY_q(Z2, Z1, Z2, a, b, c1, c2, d1, d2))


def TY_NonlinearEquation_1(Z1, Z2, K1, K2, phi, a, b, c1, c2, d1, d2):
    return c1 + d1 - 12. * phi * (
                (c1 + d1) * TY_sigma(Z1, Z1, Z2, a, b, c1, c2, d1, d2) - c1 * TY_tau(Z1, Z1, Z2, a, b, c1, c2) * exp(
            -Z1))


def TY_NonlinearEquation_2(Z1, Z2, K1, K2, phi, a, b, c1, c2, d1, d2):
    return c2 + d2 - 12. * phi * (
                (c2 + d2) * TY_sigma(Z2, Z1, Z2, a, b, c1, c2, d1, d2) - c2 * TY_tau(Z2, Z1, Z2, a, b, c1, c2) * exp(
            -Z2))


# Check the computed solutions satisfy the system of equations
def TY_CheckSolution(Z1, Z2, K1, K2, phi, a, b, c1, c2, d1, d2):
    eq_1 = chop(TY_LinearEquation_1(Z1, Z2, K1, K2, phi, a, b, c1, c2, d1, d2))
    eq_2 = chop(TY_LinearEquation_2(Z1, Z2, K1, K2, phi, a, b, c1, c2, d1, d2))
    eq_3 = chop(TY_LinearEquation_3(Z1, Z2, K1, K2, phi, a, b, c1, c2, d1, d2))
    eq_4 = chop(TY_LinearEquation_4(Z1, Z2, K1, K2, phi, a, b, c1, c2, d1, d2))
    eq_5 = chop(TY_NonlinearEquation_1(Z1, Z2, K1, K2, phi, a, b, c1, c2, d1, d2))
    eq_6 = chop(TY_NonlinearEquation_2(Z1, Z2, K1, K2, phi, a, b, c1, c2, d1, d2))

    # check if all equation are zero
    return (eq_1 == 0) & (eq_2 == 0) & (eq_3 == 0) & (eq_4 == 0) & (eq_5 == 0) & (eq_6 == 0)


def TY_ReduceNonlinearSystem(Z1, Z2, K1, K2, phi):
    #  solution of the 4 linear equations depending on d1 and d2, the solution is polynomial
    #  in d1, d2. We represent the solution as determinants obtained by Cramer's rule
    #  which can be expressed by their coefficient matrices
    #
    # Python : Results are passed through the global variable TY

    # use global
    global TY

    m11 = (3. * phi) / 2.
    m13 = 6. * phi * exp(-Z1) * (2. + Z1 * (2. + Z1) - 2. * exp(Z1)) * pow(Z1, -2)
    m14 = 6. * phi * exp(-Z2) * (2. + Z2 * (2. + Z2) - 2. * exp(Z2)) * pow(Z2, -2)
    m23 = -12. * phi * exp(-Z1) * (-1. - Z1 + exp(Z1)) * pow(Z1, -1)
    m24 = -12. * phi * exp(-Z2) * (-1. - Z2 + exp(Z2)) * pow(Z2, -1)
    m31 = -6. * phi * exp(-Z1) * pow(Z1, -2) * (2. * (1 + Z1) + exp(Z1) * (-2. + pow(Z1, 2)))
    m32 = -12. * phi * (-1. + Z1 + exp(-Z1)) * pow(Z1, -1)
    m33 = 6. * phi * exp(-2. * Z1) * pow(-1. + exp(Z1), 2)
    m34 = 12. * phi * exp(-Z1 - Z2) * (Z2 - (Z1 + Z2) * exp(Z1) + Z1 * exp(Z1 + Z2)) * pow(Z1 + Z2, -1)
    m41 = -6. * phi * exp(-Z2) * pow(Z2, -2) * (2. * (1. + Z2) + exp(Z2) * (-2. + pow(Z2, 2)))
    m42 = -12. * phi * (-1. + Z2 + exp(-Z2)) * pow(Z2, -1)
    m43 = 12. * phi * exp(-Z1 - Z2) * (Z1 - (Z1 + Z2 - Z2 * exp(Z1)) * exp(Z2)) * pow(Z1 + Z2, -1)
    m44 = 6. * phi * exp(-2 * Z2) * pow(-1. + exp(Z2), 2)

    #    /* determinant of the linear system expressed as coefficient matrix in d1, d2 */

    TY.q22 = m14 * (-(m33 * m42) + m23 * (m32 * m41 - m31 * m42) + m32 * m43 + (
                4. * m11 * (-3. * m33 * m41 + 2. * m33 * m42 + 3. * m31 * m43 - 2. * m32 * m43)) / 3.)
    TY.q22 += m13 * (m34 * m42 + m24 * (-(m32 * m41) + m31 * m42) - m32 * m44 + (
                4. * m11 * (3. * m34 * m41 - 2. * m34 * m42 - 3. * m31 * m44 + 2. * m32 * m44)) / 3.)
    TY.q22 += (3. * m24 * (m33 * (3. * m41 + 4. * m11 * m41 - 3. * m11 * m42) + (
                -3. * m31 - 4. * m11 * m31 + 3. * m11 * m32) * m43) + 3. * m23 * (
                           -3. * m34 * m41 - 4. * m11 * m34 * m41 + 3. * m11 * m34 * m42 + 3. * m31 * m44 + 4. * m11 * m31 * m44 - 3. * m11 * m32 * m44) - (
                           m34 * m43 - m33 * m44) * pow(3. - 2. * m11, 2)) / 9.

    #    /* Matrix representation of the determinant of the of the system where row referring to
    #     the variable a is replaced by solution vector */

    # Variable t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t11,t12,t13,t14,t15,t16,t17,t18,t19,t20

    TY.qa12 = (K1 * (
                3. * m14 * (m23 * m42 - 4. * m11 * m43) - 3. * m13 * (m24 * m42 - 4. * m11 * m44) + (3. + 4. * m11) * (
                    m24 * m43 - m23 * m44)) * exp(Z1)) / 3.

    TY.qa21 = -(K2 * (
                3. * m14 * (m23 * m32 - 4. * m11 * m33) - 3. * m13 * (m24 * m32 - 4. * m11 * m34) + (3. + 4. * m11) * (
                    m24 * m33 - m23 * m34)) * exp(Z2)) / 3.

    TY.qa22 = m14 * (-(m23 * m42 * Z1) + 4. * m11 * m43 * Z1 - m33 * (m42 + 4. * m11 * Z2) + m32 * (m43 + m23 * Z2)) + (
                3. * m13 * (
                    m24 * m42 * Z1 - 4. * m11 * m44 * Z1 + m34 * (m42 + 4. * m11 * Z2) - m32 * (m44 + m24 * Z2)) + (
                            3. + 4. * m11) * (
                            -(m24 * m43 * Z1) + m23 * m44 * Z1 - m34 * (m43 + m23 * Z2) + m33 * (m44 + m24 * Z2))) / 3.

    t1 = (2. * (-3. * m13 * m42 + 3. * m43 + 4. * m11 * m43) * Z1 * pow(Z2, 2) - m33 * (Z1 + Z2) * (
                6. * m42 + (3. + 4. * m11) * pow(Z2, 2)) + 3. * m32 * (Z1 + Z2) * (2. * m43 + m13 * pow(Z2, 2)))
    t2 = (2. * (3. * m14 * m42 - 3. * m44 - 4. * m11 * m44) * Z1 * pow(Z2, 2) + m34 * (Z1 + Z2) * (
                6. * m42 + (3. + 4. * m11) * pow(Z2, 2)) - 3. * m32 * (Z1 + Z2) * (2. * m44 + m14 * pow(Z2, 2)))
    t3 = (3. * (m14 * m33 * m42 - m13 * m34 * m42 - m14 * m32 * m43 + m34 * m43 + m13 * m32 * m44 - m33 * m44) * Z2 * (
                Z1 + Z2) + 2. * m11 * (6. * (-(m14 * m43) + m13 * m44) * Z1 * pow(Z2, 2) + m34 * (Z1 + Z2) * (
                2. * m43 * (-3. + Z2) - 3. * m13 * pow(Z2, 2)) + m33 * (Z1 + Z2) * (
                                                   6. * m44 - 2. * m44 * Z2 + 3. * m14 * pow(Z2, 2))))

    TY.qa23 = 2. * phi * pow(Z2, -2) * (m24 * t1 + m23 * t2 + 2. * t3) * pow(Z1 + Z2, -1)

    t1 = ((-3. * m13 * m42 + (3. + 4. * m11) * m43) * (Z1 + Z2) * pow(Z1, 2) - 2. * m33 * (
                3. * m42 * (Z1 + Z2) + (3. + 4. * m11) * Z2 * pow(Z1, 2)) + 6. * m32 * (
                      m43 * (Z1 + Z2) + m13 * Z2 * pow(Z1, 2)))
    t2 = ((3. * m14 * m42 - (3. + 4. * m11) * m44) * (Z1 + Z2) * pow(Z1, 2) + m34 * (
                6. * m42 * (Z1 + Z2) + 2. * (3. + 4. * m11) * Z2 * pow(Z1, 2)) - 6. * m32 * (
                      m44 * (Z1 + Z2) + m14 * Z2 * pow(Z1, 2)))
    t3 = (3. * (m14 * m33 * m42 - m13 * m34 * m42 - m14 * m32 * m43 + m34 * m43 + m13 * m32 * m44 - m33 * m44) * Z1 * (
                Z1 + Z2) + 2. * m11 * (-3. * (m14 * m43 - m13 * m44) * (Z1 + Z2) * pow(Z1, 2) + 2. * m34 * (
                m43 * (-3 + Z1) * (Z1 + Z2) - 3. * m13 * Z2 * pow(Z1, 2)) + m33 * (
                                                   -2. * m44 * (-3. + Z1) * (Z1 + Z2) + 6. * m14 * Z2 * pow(Z1, 2))))

    TY.qa32 = 2. * phi * pow(Z1, -2) * (m24 * t1 + m23 * t2 + 2. * t3) * pow(Z1 + Z2, -1)

    #    /* Matrix representation of the determinant of the of the system where row referring to
    #     the variable b is replaced by solution vector */

    TY.qb12 = (K1 * (-3. * m11 * m24 * m43 + m14 * (
                -3. * m23 * m41 + (-3. + 8. * m11) * m43) + 3. * m11 * m23 * m44 + m13 * (
                                 3. * m24 * m41 + 3. * m44 - 8. * m11 * m44)) * exp(Z1)) / 3.

    TY.qb21 = (K2 * (-3. * m13 * m24 * m31 + 3. * m11 * m24 * m33 + m14 * (3. * m23 * m31 + (
                3. - 8. * m11) * m33) - 3. * m13 * m34 + 8. * m11 * m13 * m34 - 3. * m11 * m23 * m34) * exp(Z2)) / 3.

    TY.qb22 = m13 * (m31 * m44 - m24 * m41 * Z1 - m44 * Z1 + (8. * m11 * m44 * Z1) / 3. + m24 * m31 * Z2 + m34 * (
                -m41 + Z2 - (8. * m11 * Z2) / 3.)) + m14 * (
                          m23 * m41 * Z1 + m43 * Z1 - (8. * m11 * m43 * Z1) / 3. + m33 * (
                              m41 - Z2 + (8. * m11 * Z2) / 3.) - m31 * (m43 + m23 * Z2)) + m11 * (
                          m24 * m43 * Z1 - m23 * m44 * Z1 + m34 * (m43 + m23 * Z2) - m33 * (m44 + m24 * Z2))

    t1 = (-(m14 * m33 * m41) + m13 * m34 * m41 + m14 * m31 * m43 - m11 * m34 * m43 - m13 * m31 * m44 + m11 * m33 * m44)
    t2 = (-3. * m11 * m24 * m43 + m14 * (-3. * m23 * m41 + (-3. + 8. * m11) * m43) + 3. * m11 * m23 * m44 + m13 * (
                3. * m24 * m41 + 3. * m44 - 8. * m11 * m44))
    t3 = (3. * m24 * (m33 * m41 - m31 * m43) + m23 * (-3. * m34 * m41 + 3. * m31 * m44) + (-3. + 8. * m11) * (
                m34 * m43 - m33 * m44))

    TY.qb23 = 2. * phi * (
                3. * m14 * m23 * m31 - 3. * m13 * m24 * m31 + 3. * m14 * m33 - 8. * m11 * m14 * m33 + 3. * m11 * m24 * m33 - 3. * m13 * m34 + 8. * m11 * m13 * m34 - 3. * m11 * m23 * m34 + 2. * t3 * pow(
            Z2, -2) + 6. * t1 * pow(Z2, -1) + 2. * t2 * Z1 * pow(Z1 + Z2, -1))

    t1 = (-(m34 * (m23 * m41 + m43)) + m24 * (m33 * m41 - m31 * m43) + (m23 * m31 + m33) * m44)
    t2 = (-(m14 * m33 * m41) + m13 * m34 * m41 + m14 * m31 * m43 - m13 * m31 * m44)
    t3 = (m14 * (2. * m23 * m31 + 2. * m33 - m23 * m41 - m43) + m13 * (-2. * m34 + m24 * (-2. * m31 + m41) + m44))
    t4 = (16. * m34 * m43 - 16. * m33 * m44 - 6. * m34 * m43 * Z1 + 6. * m33 * m44 * Z1 + (
                6. * m24 * m33 - 3. * m24 * m43 + 8. * m14 * (-2. * m33 + m43) + (8. * m13 - 3. * m23) * (
                    2. * m34 - m44)) * pow(Z1, 2))
    t5 = (2. * m34 * m43 * (8. - 3. * Z1) + 2. * m33 * m44 * (-8. + 3. * Z1) + (
                8. * m14 * m43 - 3. * m24 * m43 - 8. * m13 * m44 + 3. * m23 * m44) * pow(Z1, 2))

    TY.qb32 = 2. * phi * pow(Z1, -2) * (
                6. * t1 + 6. * t2 * Z1 + 3. * t3 * pow(Z1, 2) + (m11 * Z2 * t4 + m11 * Z1 * t5) * pow(Z1 + Z2,
                                                                                                      -1) + 6. * (
                            -(m14 * (m23 * m31 + m33)) + m13 * (m24 * m31 + m34)) * pow(Z1, 3) * pow(Z1 + Z2, -1))

    #    /* Matrix representation of the determinant of the of the system where row referring to
    #     the variable c1 is replaced by solution vector */

    TY.qc112 = -(K1 * exp(Z1) * (9. * m24 * m41 - 9. * m14 * m42 + 3. * m11 * (
                -12. * m14 * m41 + 4. * m24 * m41 + 8. * m14 * m42 - 3. * m24 * m42) + m44 * pow(3. - 2. * m11,
                                                                                                 2))) / 9.

    TY.qc121 = (K2 * exp(Z2) * (9. * m24 * m31 - 9. * m14 * m32 + 3. * m11 * (
                -12. * m14 * m31 + 4. * m24 * m31 + 8. * m14 * m32 - 3. * m24 * m32) + m34 * pow(3. - 2. * m11,
                                                                                                 2))) / 9.

    TY.qc122 = m14 * (-4. * m11 * m41 * Z1 - m42 * Z1 + (8. * m11 * m42 * Z1) / 3. + m32 * (
                -m41 + Z2 - (8. * m11 * Z2) / 3.) + m31 * (m42 + 4. * m11 * Z2)) + (3. * m34 * ((
                                                                                                            3. + 4. * m11) * m41 - 3. * m11 * m42) + 9. * m11 * m32 * m44 + 9. * m24 * m41 * Z1 + 12. * m11 * m24 * m41 * Z1 - 9. * m11 * m24 * m42 * Z1 + 9. * m44 * Z1 - 12. * m11 * m44 * Z1 + 9. * m11 * m24 * m32 * Z2 - 3. * (
                                                                                                3. + 4. * m11) * m31 * (
                                                                                                m44 + m24 * Z2) - m34 * Z2 * pow(
        3. - 2. * m11, 2) + 4. * m44 * Z1 * pow(m11, 2)) / 9.

    t1 = (m34 * (Z1 + Z2) * (2. * m42 + Z2 * (-2. * m41 + Z2)) - m32 * (Z1 + Z2) * (
                2. * m44 + m14 * Z2 * (-2. * m41 + Z2)) - 2. * (m14 * m42 - m44) * Z2 * (-(Z1 * Z2) + m31 * (Z1 + Z2)))
    t2 = (2. * (3. * m41 + 4. * m11 * m41 - 3. * m11 * m42) * Z1 * pow(Z2, 2) + 3. * m32 * (Z1 + Z2) * (
                2. * m41 + m11 * pow(Z2, 2)) - m31 * (Z1 + Z2) * (6. * m42 + (3. + 4. * m11) * pow(Z2, 2)))
    t3 = (8. * m42 + 4. * m41 * (-3. + Z2) - 3. * m42 * Z2 + 2. * pow(Z2, 2))
    t4 = (6. * m44 - 2. * m44 * Z2 + 3. * m14 * pow(Z2, 2))
    t5 = (-8. * m32 * m44 * Z1 + m32 * m44 * (-8. + 3. * Z1) * Z2 + (
                3. * m32 * m44 - 4. * (m14 * (m32 + 3. * m41 - 2. * m42) + m44) * Z1) * pow(Z2, 2) + m34 * (
                      Z1 + Z2) * t3 + 2. * m31 * (Z1 + Z2) * t4 - 4. * m14 * m32 * pow(Z2, 3))

    TY.qc123 = (2. * phi * pow(Z2, -2) * (9. * t1 + 4. * (-2. * m44 * Z1 + m34 * (Z1 + Z2)) * pow(m11, 2) * pow(Z2,
                                                                                                                2) - 3. * m24 * t2 - 6. * m11 * t5) * pow(
        Z1 + Z2, -1)) / 3.

    t1 = ((m14 * m42 - m44) * (2. * m31 - Z1) * Z1 * (Z1 + Z2) - 2. * m34 * (
                m42 * (Z1 + Z2) - Z1 * (-(Z1 * Z2) + m41 * (Z1 + Z2))) + 2. * m32 * (
                      m44 * (Z1 + Z2) - m14 * Z1 * (-(Z1 * Z2) + m41 * (Z1 + Z2))))
    t2 = (((3. + 4. * m11) * m41 - 3. * m11 * m42) * (Z1 + Z2) * pow(Z1, 2) + 6. * m32 * (
                m41 * (Z1 + Z2) + m11 * Z2 * pow(Z1, 2)) - 2. * m31 * (
                      3. * m42 * (Z1 + Z2) + (3. + 4. * m11) * Z2 * pow(Z1, 2)))
    t3 = (-8. * m32 * m44 + m34 * (m42 * (8. - 3. * Z1) + 4. * m41 * (-3. + Z1)) - 4. * m31 * m44 * (
                -3. + Z1) + 3. * m32 * m44 * Z1 - 2. * (3. * m14 * m41 - 2. * m14 * m42 + m44) * pow(Z1, 2))
    t4 = (4. * (3. * m31 - 2. * m32) * m44 + Z1 * (-4. * m31 * m44 + 3. * m32 * m44 - 2. * (
                m14 * (-6. * m31 + 4. * m32 + 3. * m41 - 2. * m42) + m44) * Z1) + m34 * (
                      m42 * (8. - 3. * Z1) + 4. * m41 * (-3. + Z1) + 4. * pow(Z1, 2)))

    TY.qc132 = (-2. * phi * pow(Z1, -2) * (9. * t1 + 4. * (-2. * m34 * Z2 + m44 * (Z1 + Z2)) * pow(m11, 2) * pow(Z1,
                                                                                                                 2) + 3. * m24 * t2 + 6. * m11 * (
                                                       Z1 * t3 + Z2 * t4)) * pow(Z1 + Z2, -1)) / 3.

    #    /* Matrix representation of the determinant of the of the system where row referring to
    #     the variable c1 is replaced by solution vector */
    TY.qc212 = (K1 * exp(Z1) * (9 * m23 * m41 - 9 * m13 * m42 + 3 * m11 * (
                -12 * m13 * m41 + 4 * m23 * m41 + 8 * m13 * m42 - 3 * m23 * m42) + m43 * pow(3 - 2 * m11, 2))) / 9.

    TY.qc221 = -(K2 * exp(Z2) * (9 * m23 * m31 - 9 * m13 * m32 + 3 * m11 * (
                -12 * m13 * m31 + 4 * m23 * m31 + 8 * m13 * m32 - 3 * m23 * m32) + m33 * pow(3 - 2 * m11, 2))) / 9.

    TY.qc222 = m13 * (4 * m11 * m41 * Z1 + m42 * Z1 - (8 * m11 * m42 * Z1) / 3. + m32 * (
                m41 - Z2 + (8 * m11 * Z2) / 3.) - m31 * (m42 + 4 * m11 * Z2)) + (
                           9 * m31 * m43 - 9 * (m23 * m41 + m43) * Z1 + 9 * m23 * m31 * Z2 + 3 * m11 * (
                               (-4 * m23 * m41 + 3 * m23 * m42 + 4 * m43) * Z1 + 4 * m31 * (
                                   m43 + m23 * Z2) - 3 * m32 * (m43 + m23 * Z2)) + m33 * (
                                       -3 * (3 + 4 * m11) * m41 + 9 * m11 * m42 + Z2 * pow(3 - 2 * m11,
                                                                                           2)) - 4 * m43 * Z1 * pow(m11,
                                                                                                                    2)) / 9.

    t1 = (-(m33 * (Z1 + Z2) * (2 * m42 + Z2 * (-2 * m41 + Z2))) + m32 * (Z1 + Z2) * (
                2 * m43 + m13 * Z2 * (-2 * m41 + Z2)) + 2 * (m13 * m42 - m43) * Z2 * (-(Z1 * Z2) + m31 * (Z1 + Z2)))
    t2 = (2 * (3 * m41 + 4 * m11 * m41 - 3 * m11 * m42) * Z1 * pow(Z2, 2) + 3 * m32 * (Z1 + Z2) * (
                2 * m41 + m11 * pow(Z2, 2)) - m31 * (Z1 + Z2) * (6 * m42 + (3 + 4 * m11) * pow(Z2, 2)))
    t3 = (-8 * m32 * m43 * Z1 + m32 * m43 * (-8 + 3 * Z1) * Z2 + (
                3 * m32 * m43 - 4 * (m13 * (m32 + 3 * m41 - 2 * m42) + m43) * Z1) * pow(Z2, 2) + m33 * (Z1 + Z2) * (
                      8 * m42 + 4 * m41 * (-3 + Z2) - 3 * m42 * Z2 + 2 * pow(Z2, 2)) + 2 * m31 * (Z1 + Z2) * (
                      6 * m43 - 2 * m43 * Z2 + 3 * m13 * pow(Z2, 2)) - 4 * m13 * m32 * pow(Z2, 3))

    TY.qc223 = (2 * phi * pow(Z2, -2) * (9 * t1 - 4 * (-2 * m43 * Z1 + m33 * (Z1 + Z2)) * pow(m11, 2) * pow(Z2,
                                                                                                            2) + 3 * m23 * t2 + 6 * m11 * t3) * pow(
        Z1 + Z2, -1)) / 3.

    t1 = ((m13 * m42 - m43) * (2 * m31 - Z1) * Z1 * (Z1 + Z2) - 2 * m33 * (
                m42 * (Z1 + Z2) - Z1 * (-(Z1 * Z2) + m41 * (Z1 + Z2))) + 2 * m32 * (
                      m43 * (Z1 + Z2) - m13 * Z1 * (-(Z1 * Z2) + m41 * (Z1 + Z2))))
    t2 = (((3 + 4 * m11) * m41 - 3 * m11 * m42) * (Z1 + Z2) * pow(Z1, 2) + 6 * m32 * (
                m41 * (Z1 + Z2) + m11 * Z2 * pow(Z1, 2)) - 2 * m31 * (
                      3 * m42 * (Z1 + Z2) + (3 + 4 * m11) * Z2 * pow(Z1, 2)))
    t3 = (-8 * m32 * m43 + m33 * (m42 * (8 - 3 * Z1) + 4 * m41 * (-3 + Z1)) - 4 * m31 * m43 * (
                -3 + Z1) + 3 * m32 * m43 * Z1 - 2 * (3 * m13 * m41 - 2 * m13 * m42 + m43) * pow(Z1, 2))
    t4 = (4 * (3 * m31 - 2 * m32) * m43 + Z1 * (-4 * m31 * m43 + 3 * m32 * m43 - 2 * (
                m13 * (-6 * m31 + 4 * m32 + 3 * m41 - 2 * m42) + m43) * Z1) + m33 * (
                      m42 * (8 - 3 * Z1) + 4 * m41 * (-3 + Z1) + 4 * pow(Z1, 2)))

    TY.qc232 = (2 * phi * pow(Z1, -2) * (
                9 * t1 + 4 * (-2 * m33 * Z2 + m43 * (Z1 + Z2)) * pow(m11, 2) * pow(Z1, 2) + 3 * m23 * t2 + 6 * m11 * (
                    Z1 * t3 + Z2 * t4)) * pow(Z1 + Z2, -1)) / 3.

    #    /* coefficient matrices of nonlinear equation 1 */
    t1 = (Z1 * (2 * TY.qb12 * (-1 + Z1) * (Z1 + Z2) - Z1 * (2 * TY.qc212 * Z1 + TY.qc112 * (Z1 + Z2))) + TY.qa12 * (
                Z1 + Z2) * (-2 + pow(Z1, 2)))
    t2 = (exp(2 * Z1) * t1 - TY.qc112 * (Z1 + Z2) * pow(Z1, 2) + 2 * (Z1 + Z2) * exp(Z1) * (
                TY.qa12 + (TY.qa12 + TY.qb12) * Z1 + TY.qc112 * pow(Z1, 2)))

    TY.A12 = 6 * phi * TY.qc112 * exp(-2 * Z1 - Z2) * pow(TY.q22, -2) * pow(Z1, -3) * (
                2 * TY.qc212 * exp(Z1) * (-Z2 + (Z1 + Z2) * exp(Z1)) * pow(Z1, 2) + exp(Z2) * t2) * pow(Z1 + Z2, -1)

    t1 = (2 * Z1 * (TY.qb21 * TY.qc112 * (-1 + Z1) * (Z1 + Z2) + TY.qb12 * TY.qc121 * (-1 + Z1) * (Z1 + Z2) - Z1 * (
                TY.qc121 * TY.qc212 * Z1 + TY.qc112 * (
                    TY.qc121 + TY.qc221) * Z1 + TY.qc112 * TY.qc121 * Z2)) + TY.qa21 * TY.qc112 * (Z1 + Z2) * (
                      -2 + pow(Z1, 2)) + TY.qa12 * TY.qc121 * (Z1 + Z2) * (-2 + pow(Z1, 2)))
    t2 = (TY.qb21 * TY.qc112 + TY.qc121 * (TY.qa12 + TY.qb12 + 2 * TY.qc112 * Z1))
    t3 = (2 * (TY.qa12 * TY.qc121 + TY.qa21 * TY.qc112 * (1 + Z1) + Z1 * t2) * (Z1 + Z2) * exp(Z1) + exp(
        2 * Z1) * t1 - 2 * TY.qc112 * TY.qc121 * (Z1 + Z2) * pow(Z1, 2))

    TY.A21 = 6 * phi * exp(-2 * Z1 - Z2) * pow(TY.q22, -2) * pow(Z1, -3) * (
                2 * (TY.qc121 * TY.qc212 + TY.qc112 * TY.qc221) * exp(Z1) * (-Z2 + (Z1 + Z2) * exp(Z1)) * pow(Z1,
                                                                                                              2) + exp(
            Z2) * t3) * pow(Z1 + Z2, -1)

    t1 = (TY.qb22 * TY.qc112 + TY.qc122 * (TY.qa12 + TY.qb12 + 2 * TY.qc112 * Z1))
    t2 = (2 * Z1 * (TY.qb22 * TY.qc112 * (-1 + Z1) * (Z1 + Z2) + TY.qb12 * TY.qc122 * (-1 + Z1) * (Z1 + Z2) - Z1 * (
                TY.qc122 * TY.qc212 * Z1 + TY.qc112 * (
                    TY.qc122 + TY.qc222) * Z1 + TY.qc112 * TY.qc122 * Z2)) + TY.qa22 * TY.qc112 * (Z1 + Z2) * (
                      -2 + pow(Z1, 2)) + TY.qa12 * TY.qc122 * (Z1 + Z2) * (-2 + pow(Z1, 2)))
    t3 = (12 * phi * (TY.qa12 * TY.qc122 + TY.qa22 * TY.qc112 * (1 + Z1) + Z1 * t1) * (Z1 + Z2) * exp(
        Z1) - 2 * phi * TY.qc112 * TY.qc122 * (Z1 + Z2) * pow(Z1, 2) + exp(2 * Z1) * (
                      6 * phi * t2 + TY.q22 * TY.qc112 * (Z1 + Z2) * pow(Z1, 3)))

    TY.A22 = exp(-2 * Z1 - Z2) * pow(TY.q22, -2) * pow(Z1, -3) * (
                12 * phi * (TY.qc122 * TY.qc212 + TY.qc112 * TY.qc222) * exp(Z1) * (-Z2 + (Z1 + Z2) * exp(Z1)) * pow(Z1,
                                                                                                                     2) + exp(
            Z2) * t3) * pow(Z1 + Z2, -1)

    t1 = ((TY.q22 * TY.qc112 + TY.qc123 * (TY.qc112 + TY.qc212) + TY.qc112 * TY.qc223) * Z1 + TY.qc112 * TY.qc123 * Z2)
    t2 = (TY.qa12 * TY.qc123 + TY.qa23 * TY.qc112 * (1 + Z1) + Z1 * (
                TY.qb23 * TY.qc112 + TY.qc123 * (TY.qa12 + TY.qb12 + 2 * TY.qc112 * Z1)))
    t3 = (2 * Z1 * (TY.qb23 * TY.qc112 * (-1 + Z1) * (Z1 + Z2) + TY.qb12 * TY.qc123 * (-1 + Z1) * (
                Z1 + Z2) - Z1 * t1) + TY.qa23 * TY.qc112 * (Z1 + Z2) * (-2 + pow(Z1, 2)) + TY.qa12 * TY.qc123 * (
                      Z1 + Z2) * (-2 + pow(Z1, 2)))

    TY.A23 = 6 * phi * exp(-2 * Z1 - Z2) * pow(TY.q22, -2) * pow(Z1, -3) * (
                2 * (TY.qc123 * TY.qc212 + TY.qc112 * TY.qc223) * exp(Z1) * (-Z2 + (Z1 + Z2) * exp(Z1)) * pow(Z1,
                                                                                                              2) + exp(
            Z2) * (2 * t2 * (Z1 + Z2) * exp(Z1) + exp(2 * Z1) * t3 - 2 * TY.qc112 * TY.qc123 * (Z1 + Z2) * pow(Z1,
                                                                                                               2))) * pow(
        Z1 + Z2, -1)

    t1 = (TY.qb32 * TY.qc112 + (TY.qa23 + TY.qb23) * TY.qc121 + (TY.qa21 + TY.qb21) * TY.qc123 + (
                TY.qa12 + TY.qb12) * TY.qc132 + TY.q22 * TY.qc112 * Z1 + 2 * (
                      TY.qc121 * TY.qc123 + TY.qc112 * TY.qc132) * Z1 + TY.qc122 * (TY.qa22 + TY.qb22 + TY.qc122 * Z1))
    t2 = (TY.qc132 * TY.qc212 + TY.qc123 * TY.qc221 + TY.qc122 * TY.qc222 + TY.qc121 * TY.qc223 + TY.qc112 * TY.qc232)
    t3 = ((
                      TY.q22 + TY.qc132) * TY.qc212 + TY.qc123 * TY.qc221 + TY.qc122 * TY.qc222 + TY.qc121 * TY.qc223 + TY.qc112 * TY.qc232)
    t4 = (2 * TY.qc121 * TY.qc123 + 2 * TY.qc112 * TY.qc132 + pow(TY.qc122, 2))
    t5 = (6 * phi * (2 * Z1 * (TY.qb12 * (-1 + Z1) * (Z1 + Z2) - Z1 * (
                (TY.qc112 + TY.qc121 + TY.qc212) * Z1 + TY.qc112 * Z2)) + TY.qa12 * (Z1 + Z2) * (
                                 -2 + pow(Z1, 2))) + TY.qc122 * (Z1 + Z2) * pow(Z1, 3))
    t6 = (-2 * (TY.qa22 * TY.qc122 + TY.qa21 * TY.qc123 + TY.qa12 * TY.qc132) - 2 * (
                TY.qb32 * TY.qc112 + TY.qb23 * TY.qc121 + TY.qb22 * TY.qc122 + TY.qb21 * TY.qc123 + TY.qb12 * TY.qc132) * Z1 + (
                      2 * TY.qb32 * TY.qc112 + 2 * TY.qb23 * TY.qc121 + (
                          TY.qa22 + 2 * TY.qb22 - TY.qc122) * TY.qc122 + (
                                  TY.qa21 + 2 * TY.qb21 - 2 * TY.qc121) * TY.qc123 + (
                                  TY.qa12 + 2 * TY.qb12 - 2 * TY.qc112) * TY.qc132) * pow(Z1, 2))
    t7 = -2 * TY.qa22 * TY.qc122 * Z1 - 2 * TY.qa21 * TY.qc123 * Z1 - 2 * TY.qa12 * TY.qc132 * Z1 + TY.qa32 * TY.qc112 * (
                Z1 + Z2) * (-2 + pow(Z1, 2)) + TY.qa23 * TY.qc121 * (Z1 + Z2) * (
                     -2 + pow(Z1, 2)) - 2 * TY.qb32 * TY.qc112 * pow(Z1, 2) - 2 * TY.qb23 * TY.qc121 * pow(Z1,
                                                                                                           2) - 2 * TY.qb22 * TY.qc122 * pow(
        Z1, 2) - 2 * TY.qb21 * TY.qc123 * pow(Z1, 2) - 2 * TY.qb12 * TY.qc132 * pow(Z1, 2)
    t8 = Z2 * t6 + 2 * TY.qb32 * TY.qc112 * pow(Z1, 3) + 2 * TY.qb23 * TY.qc121 * pow(Z1, 3) + TY.qa22 * TY.qc122 * pow(
        Z1, 3) + 2 * TY.qb22 * TY.qc122 * pow(Z1, 3) + TY.qa21 * TY.qc123 * pow(Z1, 3) + 2 * TY.qb21 * TY.qc123 * pow(
        Z1, 3) - 2 * TY.qc121 * TY.qc123 * pow(Z1, 3) + TY.qa12 * TY.qc132 * pow(Z1, 3)
    t9 = (t7 + t8 + 2 * TY.qb12 * TY.qc132 * pow(Z1, 3) - 2 * TY.qc112 * TY.qc132 * pow(Z1,
                                                                                        3) - 2 * TY.qc132 * TY.qc212 * pow(
        Z1, 3) - 2 * TY.qc123 * TY.qc221 * pow(Z1, 3) - 2 * TY.qc122 * TY.qc222 * pow(Z1,
                                                                                      3) - 2 * TY.qc121 * TY.qc223 * pow(
        Z1, 3) - 2 * TY.qc112 * TY.qc232 * pow(Z1, 3) - pow(TY.qc122, 2) * pow(Z1, 3))
    t10 = (12 * phi * (
                TY.qa23 * TY.qc121 + TY.qa22 * TY.qc122 + TY.qa21 * TY.qc123 + TY.qa12 * TY.qc132 + TY.qa32 * TY.qc112 * (
                    1 + Z1) + Z1 * t1) * (Z1 + Z2) * exp(Z1 + Z2) - 12 * phi * t2 * Z2 * exp(Z1) * pow(Z1,
                                                                                                       2) + 12 * phi * t3 * (
                       Z1 + Z2) * exp(2 * Z1) * pow(Z1, 2) - 6 * phi * (Z1 + Z2) * exp(Z2) * t4 * pow(Z1, 2) + exp(
        2 * Z1 + Z2) * (TY.q22 * t5 + 6 * phi * t9))

    TY.A32 = exp(-2 * Z1 - Z2) * pow(TY.q22, -2) * pow(Z1, -3) * t10 * pow(Z1 + Z2, -1)

    t1 = ((-(TY.qc132 * TY.qc221) - TY.qc121 * TY.qc232) * Z2 + (
                (TY.q22 + TY.qc132) * TY.qc221 + TY.qc121 * TY.qc232) * (Z1 + Z2) * exp(Z1))
    t2 = (TY.qa21 * TY.qc132 + TY.qa32 * TY.qc121 * (1 + Z1) + Z1 * (
                TY.qb32 * TY.qc121 + (TY.qa21 + TY.qb21) * TY.qc132 + TY.qc121 * (TY.q22 + 2 * TY.qc132) * Z1))
    t3 = (-2 * (TY.qa32 * TY.qc121 + TY.qa21 * (TY.q22 + TY.qc132)) - 2 * (
                TY.qb32 * TY.qc121 + TY.qb21 * (TY.q22 + TY.qc132)) * Z1 + (
                      TY.q22 * (TY.qa21 + 2 * TY.qb21 - 2 * TY.qc121) + TY.qc121 * (
                          TY.qa32 + 2 * TY.qb32 - 2 * TY.qc132) + (TY.qa21 + 2 * TY.qb21) * TY.qc132) * pow(Z1, 2))
    t4 = (-2 * (TY.qa32 * TY.qc121 + TY.qa21 * (TY.q22 + TY.qc132)) - 2 * (
                TY.qb32 * TY.qc121 + TY.qb21 * (TY.q22 + TY.qc132)) * Z1 + (
                      TY.qa32 * TY.qc121 + 2 * TY.qb32 * TY.qc121 + TY.qa21 * TY.qc132 + 2 * TY.qb21 * TY.qc132 - 2 * TY.qc121 * TY.qc132 + TY.q22 * (
                          TY.qa21 + 2 * TY.qb21 - 2 * TY.qc121 - 2 * TY.qc221) - 2 * TY.qc132 * TY.qc221 - 2 * TY.qc121 * TY.qc232) * pow(
        Z1, 2))

    TY.A41 = 6 * phi * exp(-2 * Z1 - Z2) * pow(TY.q22, -2) * pow(Z1, -3) * (2 * exp(Z1) * t1 * pow(Z1, 2) + exp(Z2) * (
                2 * t2 * (Z1 + Z2) * exp(Z1) - 2 * TY.qc121 * TY.qc132 * (Z1 + Z2) * pow(Z1, 2) + exp(2 * Z1) * (
                    Z2 * t3 + Z1 * t4))) * pow(Z1 + Z2, -1)

    t1 = (TY.qb32 * TY.qc122 + (TY.qa22 + TY.qb22) * TY.qc132 + TY.qc122 * (TY.q22 + 2 * TY.qc132) * Z1)
    t2 = (TY.qc132 * TY.qc222 + TY.qc122 * TY.qc232)
    t3 = ((TY.q22 + TY.qc132) * TY.qc222 + TY.qc122 * TY.qc232)
    t4 = (2 * Z1 * (TY.qb32 * TY.qc122 * (-1 + Z1) * (Z1 + Z2) + TY.qb22 * TY.qc132 * (-1 + Z1) * (Z1 + Z2) - Z1 * (
                TY.qc132 * TY.qc222 * Z1 + TY.qc122 * (
                    TY.qc132 + TY.qc232) * Z1 + TY.qc122 * TY.qc132 * Z2)) + TY.qa32 * TY.qc122 * (Z1 + Z2) * (
                      -2 + pow(Z1, 2)) + TY.qa22 * TY.qc132 * (Z1 + Z2) * (-2 + pow(Z1, 2)))
    t5 = (6 * phi * t4 + (Z1 + Z2) * pow(TY.q22, 2) * pow(Z1, 3) + TY.q22 * (6 * phi * (2 * Z1 * (
                TY.qb22 * (-1 + Z1) * (Z1 + Z2) - Z1 * ((TY.qc122 + TY.qc222) * Z1 + TY.qc122 * Z2)) + TY.qa22 * (
                                                                                                    Z1 + Z2) * (
                                                                                                    -2 + pow(Z1,
                                                                                                             2))) + TY.qc132 * (
                                                                                         Z1 + Z2) * pow(Z1, 3)))
    t6 = (12 * phi * (TY.qa22 * TY.qc132 + TY.qa32 * TY.qc122 * (1 + Z1) + Z1 * t1) * (Z1 + Z2) * exp(
        Z1 + Z2) - 12 * phi * t2 * Z2 * exp(Z1) * pow(Z1, 2) + 12 * phi * t3 * (Z1 + Z2) * exp(2 * Z1) * pow(Z1,
                                                                                                             2) - 12 * phi * TY.qc122 * TY.qc132 * (
                      Z1 + Z2) * exp(Z2) * pow(Z1, 2) + exp(2 * Z1 + Z2) * t5)

    TY.A42 = exp(-2 * Z1 - Z2) * pow(TY.q22, -2) * pow(Z1, -3) * t6 * pow(Z1 + Z2, -1)

    t1 = ((TY.qc132 * TY.qc223 + TY.qc123 * TY.qc232) * Z2 - ((TY.q22 + TY.qc132) * TY.qc223 + TY.qc123 * TY.qc232) * (
                Z1 + Z2) * exp(Z1))
    t2 = (TY.qa23 * TY.qc132 + TY.qa32 * TY.qc123 * (1 + Z1) + Z1 * (
                TY.qb32 * TY.qc123 + (TY.qa23 + TY.qb23) * TY.qc132 + TY.qc123 * (TY.q22 + 2 * TY.qc132) * Z1))
    t3 = (2 * TY.qa32 * TY.qc123 + 2 * TY.qa23 * (TY.q22 + TY.qc132) + 2 * (
                TY.qb32 * TY.qc123 + TY.qb23 * (TY.q22 + TY.qc132)) * Z1 - (
                      TY.q22 * (TY.qa23 + 2 * TY.qb23 - 2 * TY.qc123) + TY.qc123 * (
                          TY.qa32 + 2 * TY.qb32 - 2 * TY.qc132) + (TY.qa23 + 2 * TY.qb23) * TY.qc132) * pow(Z1, 2))
    t4 = (2 * TY.qa32 * TY.qc123 + 2 * TY.qa23 * (TY.q22 + TY.qc132) + 2 * (
                TY.qb32 * TY.qc123 + TY.qb23 * (TY.q22 + TY.qc132)) * Z1 + (
                      -(TY.qa32 * TY.qc123) - (TY.qa23 + 2 * TY.qb23) * TY.qc132 + TY.q22 * (
                          -TY.qa23 + 2 * (-TY.qb23 + TY.qc123 + TY.qc132 + TY.qc223)) + 2 * (
                                  -(TY.qb32 * TY.qc123) + TY.qc132 * (
                                      TY.qc123 + TY.qc223) + TY.qc123 * TY.qc232) + 2 * pow(TY.q22, 2)) * pow(Z1, 2))

    TY.A43 = -6 * phi * exp(-2 * Z1 - Z2) * pow(TY.q22, -2) * pow(Z1, -3) * (2 * exp(Z1) * t1 * pow(Z1, 2) + exp(Z2) * (
                -2 * t2 * (Z1 + Z2) * exp(Z1) + 2 * TY.qc123 * TY.qc132 * (Z1 + Z2) * pow(Z1, 2) + exp(2 * Z1) * (
                    Z2 * t3 + Z1 * t4))) * pow(Z1 + Z2, -1)

    t1 = (TY.qc132 * Z2 - (TY.q22 + TY.qc132) * (Z1 + Z2) * exp(Z1))
    t2 = (Z1 * (-2 * TY.qb32 * (-1 + Z1) * (Z1 + Z2) + Z1 * (
                (TY.q22 + TY.qc132 + 2 * TY.qc232) * Z1 + (TY.q22 + TY.qc132) * Z2)) - TY.qa32 * (Z1 + Z2) * (
                      -2 + pow(Z1, 2)))
    t3 = ((TY.q22 + TY.qc132) * exp(2 * Z1) * t2 + (Z1 + Z2) * pow(TY.qc132, 2) * pow(Z1, 2) - 2 * TY.qc132 * (
                Z1 + Z2) * exp(Z1) * (TY.qa32 + (TY.qa32 + TY.qb32) * Z1 + (TY.q22 + TY.qc132) * pow(Z1, 2)))

    TY.A52 = -6 * phi * exp(-2 * Z1 - Z2) * pow(TY.q22, -2) * pow(Z1, -3) * (
                2 * TY.qc232 * exp(Z1) * t1 * pow(Z1, 2) + exp(Z2) * t3) * pow(Z1 + Z2, -1)

    #    /* coefficient matrices of nonlinear equation 2 */

    t1 = (TY.qa12 * TY.qc221 + TY.qa21 * TY.qc212 * (1 + Z2) + Z2 * (
                TY.qb21 * TY.qc212 + TY.qc221 * (TY.qa12 + TY.qb12 + 2 * TY.qc212 * Z2)))
    t2 = (-(TY.qc121 * TY.qc212) - TY.qc112 * TY.qc221)
    t3 = (TY.qb21 * TY.qc212 * (-1 + Z2) * (Z1 + Z2) + TY.qb12 * TY.qc221 * (-1 + Z2) * (Z1 + Z2) - Z2 * (
                TY.qc212 * TY.qc221 * Z1 + TY.qc112 * TY.qc221 * Z2 + TY.qc212 * (TY.qc121 + TY.qc221) * Z2))
    t4 = (exp(Z1) * (
                2 * Z2 * t3 + TY.qa21 * TY.qc212 * (Z1 + Z2) * (-2 + pow(Z2, 2)) + TY.qa12 * TY.qc221 * (Z1 + Z2) * (
                    -2 + pow(Z2, 2))) + 2 * (TY.qc121 * TY.qc212 + TY.qc112 * TY.qc221) * (Z1 + Z2) * pow(Z2, 2))

    TY.B12 = 6 * phi * exp(-Z1 - 2 * Z2) * pow(TY.q22, -2) * pow(Z2, -3) * (
                -2 * TY.qc212 * TY.qc221 * (Z1 + Z2) * exp(Z1) * pow(Z2, 2) + 2 * exp(Z2) * (
                    (Z1 + Z2) * t1 * exp(Z1) + t2 * Z1 * pow(Z2, 2)) + exp(2 * Z2) * t4) * pow(Z1 + Z2, -1)

    t1 = ((Z1 + Z2) * (TY.qa12 * TY.qc223 + TY.qa23 * TY.qc212 * (1 + Z2) + Z2 * (
                TY.qb23 * TY.qc212 + (TY.qa12 + TY.qb12) * TY.qc223 + TY.qc212 * (TY.q22 + 2 * TY.qc223) * Z2)) * exp(
        Z1) + (-(TY.qc123 * TY.qc212) - TY.qc112 * TY.qc223) * Z1 * pow(Z2, 2))
    t2 = (TY.qc123 * TY.qc212 + TY.qc112 * (TY.q22 + TY.qc223))
    t3 = (TY.qa23 * TY.qc212 + TY.qa12 * (TY.q22 + TY.qc223))
    t4 = (TY.qa23 * TY.qc212 + TY.qa12 * (TY.q22 + TY.qc223) + (
                TY.qb23 * TY.qc212 + TY.qb12 * (TY.q22 + TY.qc223)) * Z1)
    t5 = (-2 * (TY.qb23 * TY.qc212 + TY.qb12 * (TY.q22 + TY.qc223)) + (
                TY.q22 * (TY.qa12 + 2 * TY.qb12 - 2 * TY.qc212) + TY.qc212 * (TY.qa23 + 2 * TY.qb23 - 2 * TY.qc223) + (
                    TY.qa12 + 2 * TY.qb12) * TY.qc223) * Z1)
    t6 = (TY.q22 * (TY.qa12 + 2 * TY.qb12 - 2 * TY.qc112 - 2 * TY.qc212) + TY.qc212 * (
                TY.qa23 + 2 * TY.qb23 - 2 * TY.qc123 - 2 * TY.qc223) + (
                      TY.qa12 + 2 * TY.qb12 - 2 * TY.qc112) * TY.qc223)

    TY.B14 = 6 * phi * exp(-Z1 - 2 * Z2) * pow(TY.q22, -2) * pow(Z2, -3) * (
                -2 * TY.qc212 * TY.qc223 * (Z1 + Z2) * exp(Z1) * pow(Z2, 2) + 2 * exp(Z2) * t1 + exp(2 * Z2) * (
                    2 * t2 * (Z1 + Z2) * pow(Z2, 2) + exp(Z1) * (
                        -2 * t3 * Z1 - 2 * t4 * Z2 + t5 * pow(Z2, 2) + t6 * pow(Z2, 3)))) * pow(Z1 + Z2, -1)

    t1 = (TY.qc221 * (Z1 + Z2) * exp(Z1) * pow(Z2, 2))
    t2 = (exp(Z1) * (Z2 * (
                2 * TY.qb21 * (-1 + Z2) * (Z1 + Z2) - Z2 * (2 * TY.qc121 * Z2 + TY.qc221 * (Z1 + Z2))) + TY.qa21 * (
                                 Z1 + Z2) * (-2 + pow(Z2, 2))) + 2 * TY.qc121 * (Z1 + Z2) * pow(Z2, 2))
    t3 = (-(TY.qc121 * Z1 * pow(Z2, 2)) + (Z1 + Z2) * exp(Z1) * (
                TY.qa21 + (TY.qa21 + TY.qb21) * Z2 + TY.qc221 * pow(Z2, 2)))

    TY.B21 = 6 * phi * TY.qc221 * exp(-Z1 - 2 * Z2) * pow(TY.q22, -2) * pow(Z2, -3) * (
                -t1 + exp(2 * Z2) * t2 + 2 * exp(Z2) * t3) * pow(Z1 + Z2, -1)

    t1 = (TY.qb22 * TY.qc221 + TY.qc222 * (TY.qa21 + TY.qb21 + 2 * TY.qc221 * Z2))
    t2 = ((Z1 + Z2) * (TY.qa21 * TY.qc222 + TY.qa22 * TY.qc221 * (1 + Z2) + Z2 * t1) * exp(Z1) + (
                -(TY.qc122 * TY.qc221) - TY.qc121 * TY.qc222) * Z1 * pow(Z2, 2))
    t3 = (TY.qc122 * TY.qc221 + TY.qc121 * TY.qc222)
    t4 = (TY.qb22 * TY.qc221 * (-1 + Z2) * (Z1 + Z2) + TY.qb21 * TY.qc222 * (-1 + Z2) * (Z1 + Z2) - Z2 * (
                TY.qc221 * TY.qc222 * Z1 + TY.qc121 * TY.qc222 * Z2 + TY.qc221 * (TY.qc122 + TY.qc222) * Z2))
    t5 = (6 * phi * (
                2 * Z2 * t4 + TY.qa22 * TY.qc221 * (Z1 + Z2) * (-2 + pow(Z2, 2)) + TY.qa21 * TY.qc222 * (Z1 + Z2) * (
                    -2 + pow(Z2, 2))) + TY.q22 * TY.qc221 * (Z1 + Z2) * pow(Z2, 3))

    TY.B22 = exp(-Z1 - 2 * Z2) * pow(TY.q22, -2) * pow(Z2, -3) * (
                -12 * phi * TY.qc221 * TY.qc222 * (Z1 + Z2) * exp(Z1) * pow(Z2, 2) + 12 * phi * exp(Z2) * t2 + exp(
            2 * Z2) * (12 * phi * t3 * (Z1 + Z2) * pow(Z2, 2) + exp(Z1) * t5)) * pow(Z1 + Z2, -1)

    t1 = (2 * TY.qc221 * TY.qc223 + 2 * TY.qc212 * TY.qc232 + pow(TY.qc222, 2))
    t2 = (TY.qb32 * TY.qc212 + (TY.qa23 + TY.qb23) * TY.qc221 + (TY.qa22 + TY.qb22) * TY.qc222 + (
                TY.qa21 + TY.qb21) * TY.qc223 + (TY.qa12 + TY.qb12) * TY.qc232 + Z2 * (
                      TY.q22 * TY.qc221 + 2 * TY.qc221 * TY.qc223 + 2 * TY.qc212 * TY.qc232 + pow(TY.qc222, 2)))
    t3 = (-(
                TY.qc132 * TY.qc212) - TY.qc123 * TY.qc221 - TY.qc122 * TY.qc222 - TY.qc121 * TY.qc223 - TY.qc112 * TY.qc232)
    t4 = (TY.qc132 * TY.qc212 + TY.qc123 * TY.qc221 + TY.qc122 * TY.qc222 + TY.qc121 * (
                TY.q22 + TY.qc223) + TY.qc112 * TY.qc232)
    t5 = (TY.qa32 * TY.qc212 + TY.qa23 * TY.qc221 + TY.qa22 * TY.qc222 + TY.qa21 * (
                TY.q22 + TY.qc223) + TY.qa12 * TY.qc232)
    t6 = (TY.qa32 * TY.qc212 + TY.qa23 * TY.qc221 + TY.qa22 * TY.qc222 + TY.qa21 * (
                TY.q22 + TY.qc223) + TY.qa12 * TY.qc232 + (
                      TY.qb32 * TY.qc212 + TY.qb23 * TY.qc221 + TY.qb22 * TY.qc222 + TY.qb21 * (
                          TY.q22 + TY.qc223) + TY.qb12 * TY.qc232) * Z1)
    t7 = (TY.qb32 * TY.qc212 + TY.qb23 * TY.qc221 + TY.qb22 * TY.qc222 + TY.qb21 * (
                TY.q22 + TY.qc223) + TY.qb12 * TY.qc232)
    t8 = (TY.q22 * (TY.qa21 + 2 * TY.qb21 - 2 * TY.qc221) + (TY.qa22 + 2 * TY.qb22 - TY.qc222) * TY.qc222 + TY.qc221 * (
                TY.qa23 + 2 * TY.qb23 - 2 * TY.qc223) + TY.qa21 * TY.qc223 + 2 * TY.qb21 * TY.qc223 + TY.qc212 * (
                      TY.qa32 + 2 * TY.qb32 - 2 * TY.qc232) + TY.qa12 * TY.qc232 + 2 * TY.qb12 * TY.qc232)
    t9 = (TY.qa21 + 2 * TY.qb21 - 2 * TY.qc121 - 2 * TY.qc212 - 2 * TY.qc221)
    t10 = (TY.qa22 + 2 * TY.qb22 - 2 * TY.qc122 - TY.qc222)
    t11 = (TY.qa23 + 2 * TY.qb23 - 2 * TY.qc123 - 2 * TY.qc223)
    t12 = (TY.qa32 + 2 * TY.qb32 - 2 * TY.qc132 - 2 * TY.qc232)
    t13 = ((Z1 + Z2) * exp(Z1) * (
                TY.qa23 * TY.qc221 + TY.qa22 * TY.qc222 + TY.qa21 * TY.qc223 + TY.qa12 * TY.qc232 + TY.qa32 * TY.qc212 * (
                    1 + Z2) + Z2 * t2) + t3 * Z1 * pow(Z2, 2))
    t14 = (TY.q22 * t9 + t10 * TY.qc222 + TY.qc221 * t11 + (
                TY.qa21 + 2 * TY.qb21 - 2 * TY.qc121) * TY.qc223 + TY.qc212 * t12 + (
                       TY.qa12 + 2 * TY.qb12 - 2 * TY.qc112) * TY.qc232)
    t15 = (-6 * phi * (Z1 + Z2) * exp(Z1) * t1 * pow(Z2, 2) + 12 * phi * exp(Z2) * t13 + exp(2 * Z2) * (
                12 * phi * t4 * (Z1 + Z2) * pow(Z2, 2) + exp(Z1) * (
                    TY.q22 * TY.qc222 * (Z1 + Z2) * pow(Z2, 3) - 6 * phi * (
                        2 * t5 * Z1 + 2 * t6 * Z2 - (-2 * t7 + t8 * Z1) * pow(Z2, 2) - t14 * pow(Z2, 3)))))

    TY.B23 = exp(-Z1 - 2 * Z2) * pow(TY.q22, -2) * pow(Z2, -3) * t15 * pow(Z1 + Z2, -1)

    t1 = (TY.qa22 * TY.qc223 + TY.qa23 * TY.qc222 * (1 + Z2) + Z2 * (
                TY.qb23 * TY.qc222 + (TY.qa22 + TY.qb22) * TY.qc223 + TY.qc222 * (TY.q22 + 2 * TY.qc223) * Z2))
    t2 = (TY.qc123 * TY.qc222 + TY.qc122 * TY.qc223)
    t3 = (TY.qc123 * TY.qc222 + TY.qc122 * (TY.q22 + TY.qc223))
    t4 = (2 * Z2 * (TY.qb23 * TY.qc222 * (-1 + Z2) * (Z1 + Z2) + TY.qb22 * TY.qc223 * (-1 + Z2) * (Z1 + Z2) - Z2 * (
                TY.qc222 * TY.qc223 * Z1 + TY.qc122 * TY.qc223 * Z2 + TY.qc222 * (
                    TY.qc123 + TY.qc223) * Z2)) + TY.qa23 * TY.qc222 * (Z1 + Z2) * (
                      -2 + pow(Z2, 2)) + TY.qa22 * TY.qc223 * (Z1 + Z2) * (-2 + pow(Z2, 2)))
    t5 = (6 * phi * t4 + (Z1 + Z2) * pow(TY.q22, 2) * pow(Z2, 3) + TY.q22 * (6 * phi * (2 * Z2 * (
                TY.qb22 * (-1 + Z2) * (Z1 + Z2) - Z2 * (TY.qc222 * Z1 + (TY.qc122 + TY.qc222) * Z2)) + TY.qa22 * (
                                                                                                    Z1 + Z2) * (
                                                                                                    -2 + pow(Z2,
                                                                                                             2))) + TY.qc223 * (
                                                                                         Z1 + Z2) * pow(Z2, 3)))
    t6 = (12 * phi * (Z1 + Z2) * t1 * exp(Z1 + Z2) - 12 * phi * TY.qc222 * TY.qc223 * (Z1 + Z2) * exp(Z1) * pow(Z2,
                                                                                                                2) - 12 * phi * t2 * Z1 * exp(
        Z2) * pow(Z2, 2) + 12 * phi * t3 * (Z1 + Z2) * exp(2 * Z2) * pow(Z2, 2) + exp(Z1 + 2 * Z2) * t5)

    TY.B24 = exp(-Z1 - 2 * Z2) * pow(TY.q22, -2) * pow(Z2, -3) * t6 * pow(Z1 + Z2, -1)

    t1 = (exp(Z1) * (Z2 * (-2 * TY.qb23 * (-1 + Z2) * (Z1 + Z2) + Z2 * (
                (TY.q22 + TY.qc223) * Z1 + (TY.q22 + 2 * TY.qc123 + TY.qc223) * Z2)) - TY.qa23 * (Z1 + Z2) * (
                                 -2 + pow(Z2, 2))) - 2 * TY.qc123 * (Z1 + Z2) * pow(Z2, 2))
    t2 = ((Z1 + Z2) * exp(Z1) * pow(TY.qc223, 2) * pow(Z2, 2) + (TY.q22 + TY.qc223) * exp(
        2 * Z2) * t1 + 2 * TY.qc223 * exp(Z2) * (TY.qc123 * Z1 * pow(Z2, 2) - (Z1 + Z2) * exp(Z1) * (
                TY.qa23 + (TY.qa23 + TY.qb23) * Z2 + (TY.q22 + TY.qc223) * pow(Z2, 2))))

    TY.B25 = -6 * phi * exp(-Z1 - 2 * Z2) * pow(TY.q22, -2) * pow(Z2, -3) * t2 * pow(Z1 + Z2, -1)

    t1 = (TY.qa21 * TY.qc232 + TY.qa32 * TY.qc221 * (1 + Z2) + Z2 * (
                TY.qb32 * TY.qc221 + TY.qc232 * (TY.qa21 + TY.qb21 + 2 * TY.qc221 * Z2)))
    t2 = (-(TY.qc132 * TY.qc221) - TY.qc121 * TY.qc232)
    t3 = (TY.qb32 * TY.qc221 * (-1 + Z2) * (Z1 + Z2) + TY.qb21 * TY.qc232 * (-1 + Z2) * (Z1 + Z2) - Z2 * (
                TY.qc221 * TY.qc232 * Z1 + TY.qc121 * TY.qc232 * Z2 + TY.qc221 * (TY.q22 + TY.qc132 + TY.qc232) * Z2))
    t4 = (exp(Z1) * (
                2 * Z2 * t3 + TY.qa32 * TY.qc221 * (Z1 + Z2) * (-2 + pow(Z2, 2)) + TY.qa21 * TY.qc232 * (Z1 + Z2) * (
                    -2 + pow(Z2, 2))) + 2 * (TY.qc132 * TY.qc221 + TY.qc121 * TY.qc232) * (Z1 + Z2) * pow(Z2, 2))

    TY.B32 = 6 * phi * exp(-Z1 - 2 * Z2) * pow(TY.q22, -2) * pow(Z2, -3) * (
                -2 * TY.qc221 * TY.qc232 * (Z1 + Z2) * exp(Z1) * pow(Z2, 2) + 2 * exp(Z2) * (
                    (Z1 + Z2) * t1 * exp(Z1) + t2 * Z1 * pow(Z2, 2)) + exp(2 * Z2) * t4) * pow(Z1 + Z2, -1)

    t1 = (-((Z1 + Z2) * (TY.qa23 * TY.qc232 + TY.qa32 * TY.qc223 * (1 + Z2) + Z2 * (
                TY.qb32 * TY.qc223 + TY.qc232 * (TY.qa23 + TY.qb23 + TY.q22 * Z2 + 2 * TY.qc223 * Z2))) * exp(Z1)) + (
                      TY.qc132 * TY.qc223 + TY.qc123 * TY.qc232) * Z1 * pow(Z2, 2))
    t2 = (TY.qc132 * (TY.q22 + TY.qc223) + TY.qc123 * TY.qc232)
    t3 = (TY.qa32 * (TY.q22 + TY.qc223) + TY.qa23 * TY.qc232)
    t4 = (TY.qa32 * (TY.q22 + TY.qc223) + TY.qa23 * TY.qc232 + (
                TY.qb32 * (TY.q22 + TY.qc223) + TY.qb23 * TY.qc232) * Z1)
    t5 = (-2 * (TY.qb32 * (TY.q22 + TY.qc223) + TY.qb23 * TY.qc232) + ((TY.qa32 + 2 * TY.qb32) * (TY.q22 + TY.qc223) + (
                -2 * TY.q22 + TY.qa23 + 2 * TY.qb23 - 2 * TY.qc223) * TY.qc232) * Z1)
    t6 = (2 * t3 * Z1 + 2 * t4 * Z2 - t5 * pow(Z2, 2) + (
                (2 * TY.q22 - TY.qa32 - 2 * TY.qb32 + 2 * TY.qc132) * (TY.q22 + TY.qc223) + (
                    2 * TY.q22 - TY.qa23 + 2 * (-TY.qb23 + TY.qc123 + TY.qc223)) * TY.qc232) * pow(Z2, 3))

    TY.B34 = -6 * phi * exp(-Z1 - 2 * Z2) * pow(TY.q22, -2) * pow(Z2, -3) * (
                2 * TY.qc223 * TY.qc232 * (Z1 + Z2) * exp(Z1) * pow(Z2, 2) + 2 * exp(Z2) * t1 + exp(2 * Z2) * (
                    -2 * t2 * (Z1 + Z2) * pow(Z2, 2) + exp(Z1) * t6)) * pow(Z1 + Z2, -1)

    #    /* decrease order of nonlinear equation 1 by means of equation 2 */

    TY.F14 = -(TY.A32 * TY.B12 * TY.B32) + TY.A52 * pow(TY.B12, 2) + TY.A12 * pow(TY.B32, 2)
    TY.F16 = 2 * TY.A52 * TY.B12 * TY.B14 - TY.A32 * TY.B14 * TY.B32 - TY.A32 * TY.B12 * TY.B34 + 2 * TY.A12 * TY.B32 * TY.B34
    TY.F18 = -(TY.A32 * TY.B14 * TY.B34) + TY.A52 * pow(TY.B14, 2) + TY.A12 * pow(TY.B34, 2)
    TY.F23 = 2 * TY.A52 * TY.B12 * TY.B21 - TY.A41 * TY.B12 * TY.B32 - TY.A32 * TY.B21 * TY.B32 + TY.A21 * pow(TY.B32,
                                                                                                               2)
    TY.F24 = 2 * TY.A52 * TY.B12 * TY.B22 - TY.A42 * TY.B12 * TY.B32 - TY.A32 * TY.B22 * TY.B32 + TY.A22 * pow(TY.B32,
                                                                                                               2)
    TY.F25 = 2 * TY.A52 * TY.B14 * TY.B21 + 2 * TY.A52 * TY.B12 * TY.B23 - TY.A43 * TY.B12 * TY.B32 - TY.A41 * TY.B14 * TY.B32 - TY.A32 * TY.B23 * TY.B32 - TY.A41 * TY.B12 * TY.B34 - TY.A32 * TY.B21 * TY.B34 + 2 * TY.A21 * TY.B32 * TY.B34 + TY.A23 * pow(
        TY.B32, 2)
    TY.F26 = 2 * TY.A52 * TY.B14 * TY.B22 + 2 * TY.A52 * TY.B12 * TY.B24 - TY.A42 * TY.B14 * TY.B32 - TY.A32 * TY.B24 * TY.B32 - TY.A42 * TY.B12 * TY.B34 - TY.A32 * TY.B22 * TY.B34 + 2 * TY.A22 * TY.B32 * TY.B34
    TY.F27 = 2 * TY.A52 * TY.B14 * TY.B23 + 2 * TY.A52 * TY.B12 * TY.B25 - TY.A43 * TY.B14 * TY.B32 - TY.A32 * TY.B25 * TY.B32 - TY.A43 * TY.B12 * TY.B34 - TY.A41 * TY.B14 * TY.B34 - TY.A32 * TY.B23 * TY.B34 + 2 * TY.A23 * TY.B32 * TY.B34 + TY.A21 * pow(
        TY.B34, 2)
    TY.F28 = 2 * TY.A52 * TY.B14 * TY.B24 - TY.A42 * TY.B14 * TY.B34 - TY.A32 * TY.B24 * TY.B34 + TY.A22 * pow(TY.B34,
                                                                                                               2)
    TY.F29 = 2 * TY.A52 * TY.B14 * TY.B25 - TY.A43 * TY.B14 * TY.B34 - TY.A32 * TY.B25 * TY.B34 + TY.A23 * pow(TY.B34,
                                                                                                               2)
    TY.F32 = -(TY.A41 * TY.B21 * TY.B32) + TY.A52 * pow(TY.B21, 2)
    TY.F33 = 2 * TY.A52 * TY.B21 * TY.B22 - TY.A42 * TY.B21 * TY.B32 - TY.A41 * TY.B22 * TY.B32
    TY.F34 = 2 * TY.A52 * TY.B21 * TY.B23 - TY.A43 * TY.B21 * TY.B32 - TY.A42 * TY.B22 * TY.B32 - TY.A41 * TY.B23 * TY.B32 - TY.A41 * TY.B21 * TY.B34 + TY.A52 * pow(
        TY.B22, 2)
    TY.F35 = 2 * TY.A52 * TY.B22 * TY.B23 + 2 * TY.A52 * TY.B21 * TY.B24 - TY.A43 * TY.B22 * TY.B32 - TY.A42 * TY.B23 * TY.B32 - TY.A41 * TY.B24 * TY.B32 - TY.A42 * TY.B21 * TY.B34 - TY.A41 * TY.B22 * TY.B34
    TY.F36 = 2 * TY.A52 * TY.B22 * TY.B24 + 2 * TY.A52 * TY.B21 * TY.B25 - TY.A43 * TY.B23 * TY.B32 - TY.A42 * TY.B24 * TY.B32 - TY.A41 * TY.B25 * TY.B32 - TY.A43 * TY.B21 * TY.B34 - TY.A42 * TY.B22 * TY.B34 - TY.A41 * TY.B23 * TY.B34 + TY.A52 * pow(
        TY.B23, 2)
    TY.F37 = 2 * TY.A52 * TY.B23 * TY.B24 + 2 * TY.A52 * TY.B22 * TY.B25 - TY.A43 * TY.B24 * TY.B32 - TY.A42 * TY.B25 * TY.B32 - TY.A43 * TY.B22 * TY.B34 - TY.A42 * TY.B23 * TY.B34 - TY.A41 * TY.B24 * TY.B34
    TY.F38 = 2 * TY.A52 * TY.B23 * TY.B25 - TY.A43 * TY.B25 * TY.B32 - TY.A43 * TY.B23 * TY.B34 - TY.A42 * TY.B24 * TY.B34 - TY.A41 * TY.B25 * TY.B34 + TY.A52 * pow(
        TY.B24, 2)
    TY.F39 = 2 * TY.A52 * TY.B24 * TY.B25 - TY.A43 * TY.B24 * TY.B34 - TY.A42 * TY.B25 * TY.B34
    TY.F310 = -(TY.A43 * TY.B25 * TY.B34) + TY.A52 * pow(TY.B25, 2)

    TY.G13 = -(TY.B12 * TY.F32)
    TY.G14 = -(TY.B12 * TY.F33)
    TY.G15 = TY.B32 * TY.F14 - TY.B14 * TY.F32 - TY.B12 * TY.F34
    TY.G16 = -(TY.B14 * TY.F33) - TY.B12 * TY.F35
    TY.G17 = TY.B34 * TY.F14 + TY.B32 * TY.F16 - TY.B14 * TY.F34 - TY.B12 * TY.F36
    TY.G18 = -(TY.B14 * TY.F35) - TY.B12 * TY.F37
    TY.G19 = TY.B34 * TY.F16 + TY.B32 * TY.F18 - TY.B14 * TY.F36 - TY.B12 * TY.F38
    TY.G110 = -(TY.B14 * TY.F37) - TY.B12 * TY.F39
    TY.G111 = TY.B34 * TY.F18 - TY.B12 * TY.F310 - TY.B14 * TY.F38
    TY.G112 = -(TY.B14 * TY.F39)
    TY.G113 = -(TY.B14 * TY.F310)
    TY.G22 = -(TY.B21 * TY.F32)
    TY.G23 = -(TY.B22 * TY.F32) - TY.B21 * TY.F33
    TY.G24 = TY.B32 * TY.F23 - TY.B23 * TY.F32 - TY.B22 * TY.F33 - TY.B21 * TY.F34
    TY.G25 = TY.B32 * TY.F24 - TY.B24 * TY.F32 - TY.B23 * TY.F33 - TY.B22 * TY.F34 - TY.B21 * TY.F35
    TY.G26 = TY.B34 * TY.F23 + TY.B32 * TY.F25 - TY.B25 * TY.F32 - TY.B24 * TY.F33 - TY.B23 * TY.F34 - TY.B22 * TY.F35 - TY.B21 * TY.F36
    TY.G27 = TY.B34 * TY.F24 + TY.B32 * TY.F26 - TY.B25 * TY.F33 - TY.B24 * TY.F34 - TY.B23 * TY.F35 - TY.B22 * TY.F36 - TY.B21 * TY.F37
    TY.G28 = TY.B34 * TY.F25 + TY.B32 * TY.F27 - TY.B25 * TY.F34 - TY.B24 * TY.F35 - TY.B23 * TY.F36 - TY.B22 * TY.F37 - TY.B21 * TY.F38
    TY.G29 = TY.B34 * TY.F26 + TY.B32 * TY.F28 - TY.B25 * TY.F35 - TY.B24 * TY.F36 - TY.B23 * TY.F37 - TY.B22 * TY.F38 - TY.B21 * TY.F39
    TY.G210 = TY.B34 * TY.F27 + TY.B32 * TY.F29 - TY.B21 * TY.F310 - TY.B25 * TY.F36 - TY.B24 * TY.F37 - TY.B23 * TY.F38 - TY.B22 * TY.F39
    TY.G211 = TY.B34 * TY.F28 - TY.B22 * TY.F310 - TY.B25 * TY.F37 - TY.B24 * TY.F38 - TY.B23 * TY.F39
    TY.G212 = TY.B34 * TY.F29 - TY.B23 * TY.F310 - TY.B25 * TY.F38 - TY.B24 * TY.F39
    TY.G213 = -(TY.B24 * TY.F310) - TY.B25 * TY.F39
    TY.G214 = -(TY.B25 * TY.F310)

    # Make/O/D/N=23 TY.w

    # coefficients for polynomial
    TY.w[0] = (-(TY.A21 * TY.B12) + TY.A12 * TY.B21) * (TY.A52 * TY.B21 - TY.A41 * TY.B32) * pow(TY.B21, 2) * pow(
        TY.B32, 3)

    TY.w[
        1] = 2 * TY.B32 * TY.G13 * TY.G14 - TY.B24 * TY.G13 * TY.G22 - TY.B23 * TY.G14 * TY.G22 - TY.B22 * TY.G15 * TY.G22 - TY.B21 * TY.G16 * TY.G22 - TY.B23 * TY.G13 * TY.G23 - TY.B22 * TY.G14 * TY.G23
    TY.w[
        1] += - TY.B21 * TY.G15 * TY.G23 + 2 * TY.B14 * TY.G22 * TY.G23 - TY.B22 * TY.G13 * TY.G24 - TY.B21 * TY.G14 * TY.G24 + 2 * TY.B12 * TY.G23 * TY.G24 - TY.B21 * TY.G13 * TY.G25 + 2 * TY.B12 * TY.G22 * TY.G25

    TY.w[2] = -(
                TY.B25 * TY.G13 * TY.G22) - TY.B24 * TY.G14 * TY.G22 - TY.B23 * TY.G15 * TY.G22 - TY.B22 * TY.G16 * TY.G22 - TY.B21 * TY.G17 * TY.G22 - TY.B24 * TY.G13 * TY.G23 - TY.B23 * TY.G14 * TY.G23 - TY.B22 * TY.G15 * TY.G23 - TY.B21 * TY.G16 * TY.G23
    TY.w[
        2] += -TY.B23 * TY.G13 * TY.G24 - TY.B22 * TY.G14 * TY.G24 - TY.B21 * TY.G15 * TY.G24 + 2 * TY.B14 * TY.G22 * TY.G24 - TY.B22 * TY.G13 * TY.G25 - TY.B21 * TY.G14 * TY.G25 + 2 * TY.B12 * TY.G23 * TY.G25 - TY.B21 * TY.G13 * TY.G26 + 2 * TY.B12 * TY.G22 * TY.G26
    TY.w[2] += +TY.B34 * pow(TY.G13, 2) + TY.B32 * (2 * TY.G13 * TY.G15 + pow(TY.G14, 2)) + TY.B14 * pow(TY.G23,
                                                                                                         2) + TY.B12 * pow(
        TY.G24, 2)

    TY.w[3] = 2 * TY.B34 * TY.G13 * TY.G14 + 2 * TY.B32 * (
                TY.G14 * TY.G15 + TY.G13 * TY.G16) - TY.B25 * TY.G14 * TY.G22 - TY.B24 * TY.G15 * TY.G22 - TY.B23 * TY.G16 * TY.G22 - TY.B22 * TY.G17 * TY.G22 - TY.B21 * TY.G18 * TY.G22 - TY.B25 * TY.G13 * TY.G23
    TY.w[
        3] += -TY.B24 * TY.G14 * TY.G23 - TY.B23 * TY.G15 * TY.G23 - TY.B22 * TY.G16 * TY.G23 - TY.B21 * TY.G17 * TY.G23 - TY.B24 * TY.G13 * TY.G24 - TY.B23 * TY.G14 * TY.G24 - TY.B22 * TY.G15 * TY.G24 - TY.B21 * TY.G16 * TY.G24 + 2 * TY.B14 * TY.G23 * TY.G24
    TY.w[
        3] += -TY.B23 * TY.G13 * TY.G25 - TY.B22 * TY.G14 * TY.G25 - TY.B21 * TY.G15 * TY.G25 + 2 * TY.B14 * TY.G22 * TY.G25 + 2 * TY.B12 * TY.G24 * TY.G25 - TY.B22 * TY.G13 * TY.G26 - TY.B21 * TY.G14 * TY.G26 + 2 * TY.B12 * TY.G23 * TY.G26 - TY.B21 * TY.G13 * TY.G27
    TY.w[3] += 2 * TY.B12 * TY.G22 * TY.G27

    TY.w[4] = -(
                TY.B25 * TY.G15 * TY.G22) - TY.B24 * TY.G16 * TY.G22 - TY.B23 * TY.G17 * TY.G22 - TY.B22 * TY.G18 * TY.G22 - TY.B21 * TY.G19 * TY.G22 - TY.B25 * TY.G14 * TY.G23 - TY.B24 * TY.G15 * TY.G23 - TY.B23 * TY.G16 * TY.G23 - TY.B22 * TY.G17 * TY.G23
    TY.w[
        4] += -TY.B21 * TY.G18 * TY.G23 - TY.B25 * TY.G13 * TY.G24 - TY.B24 * TY.G14 * TY.G24 - TY.B23 * TY.G15 * TY.G24 - TY.B22 * TY.G16 * TY.G24 - TY.B21 * TY.G17 * TY.G24 - TY.B24 * TY.G13 * TY.G25 - TY.B23 * TY.G14 * TY.G25 - TY.B22 * TY.G15 * TY.G25
    TY.w[
        4] += -TY.B21 * TY.G16 * TY.G25 + 2 * TY.B14 * TY.G23 * TY.G25 - TY.B23 * TY.G13 * TY.G26 - TY.B22 * TY.G14 * TY.G26 - TY.B21 * TY.G15 * TY.G26 + 2 * TY.B14 * TY.G22 * TY.G26 + 2 * TY.B12 * TY.G24 * TY.G26 - TY.B22 * TY.G13 * TY.G27 - TY.B21 * TY.G14 * TY.G27
    TY.w[4] += 2 * TY.B12 * TY.G23 * TY.G27 - TY.B21 * TY.G13 * TY.G28 + 2 * TY.B12 * TY.G22 * TY.G28 + TY.B34 * (
                2 * TY.G13 * TY.G15 + pow(TY.G14, 2)) + TY.B32 * (
                           2 * TY.G14 * TY.G16 + 2 * TY.G13 * TY.G17 + pow(TY.G15, 2)) + TY.B14 * pow(TY.G24, 2)
    TY.w[4] += TY.B12 * pow(TY.G25, 2)

    TY.w[5] = 2 * TY.B34 * (TY.G14 * TY.G15 + TY.G13 * TY.G16) + 2 * TY.B32 * (
                TY.G15 * TY.G16 + TY.G14 * TY.G17 + TY.G13 * TY.G18) - TY.B21 * TY.G110 * TY.G22 - TY.B25 * TY.G16 * TY.G22 - TY.B24 * TY.G17 * TY.G22 - TY.B23 * TY.G18 * TY.G22 - TY.B22 * TY.G19 * TY.G22
    TY.w[
        5] += -TY.B25 * TY.G15 * TY.G23 - TY.B24 * TY.G16 * TY.G23 - TY.B23 * TY.G17 * TY.G23 - TY.B22 * TY.G18 * TY.G23 - TY.B21 * TY.G19 * TY.G23 - TY.B25 * TY.G14 * TY.G24 - TY.B24 * TY.G15 * TY.G24 - TY.B23 * TY.G16 * TY.G24 - TY.B22 * TY.G17 * TY.G24
    TY.w[
        5] += -TY.B21 * TY.G18 * TY.G24 - TY.B25 * TY.G13 * TY.G25 - TY.B24 * TY.G14 * TY.G25 - TY.B23 * TY.G15 * TY.G25 - TY.B22 * TY.G16 * TY.G25 - TY.B21 * TY.G17 * TY.G25 + 2 * TY.B14 * TY.G24 * TY.G25 - TY.B24 * TY.G13 * TY.G26 - TY.B23 * TY.G14 * TY.G26
    TY.w[
        5] += -TY.B22 * TY.G15 * TY.G26 - TY.B21 * TY.G16 * TY.G26 + 2 * TY.B14 * TY.G23 * TY.G26 + 2 * TY.B12 * TY.G25 * TY.G26 - TY.B23 * TY.G13 * TY.G27 - TY.B22 * TY.G14 * TY.G27 - TY.B21 * TY.G15 * TY.G27 + 2 * TY.B14 * TY.G22 * TY.G27 + 2 * TY.B12 * TY.G24 * TY.G27
    TY.w[
        5] += -TY.B22 * TY.G13 * TY.G28 - TY.B21 * TY.G14 * TY.G28 + 2 * TY.B12 * TY.G23 * TY.G28 - TY.B21 * TY.G13 * TY.G29 + 2 * TY.B12 * TY.G22 * TY.G29

    TY.w[6] = -(
                TY.B22 * TY.G110 * TY.G22) - TY.B21 * TY.G111 * TY.G22 - TY.B25 * TY.G17 * TY.G22 - TY.B24 * TY.G18 * TY.G22 - TY.B23 * TY.G19 * TY.G22 + TY.G210 * (
                          -(
                                      TY.B21 * TY.G13) + 2 * TY.B12 * TY.G22) - TY.B21 * TY.G110 * TY.G23 - TY.B25 * TY.G16 * TY.G23
    TY.w[
        6] += -TY.B24 * TY.G17 * TY.G23 - TY.B23 * TY.G18 * TY.G23 - TY.B22 * TY.G19 * TY.G23 - TY.B25 * TY.G15 * TY.G24 - TY.B24 * TY.G16 * TY.G24 - TY.B23 * TY.G17 * TY.G24 - TY.B22 * TY.G18 * TY.G24 - TY.B21 * TY.G19 * TY.G24 - TY.B25 * TY.G14 * TY.G25
    TY.w[
        6] += -TY.B24 * TY.G15 * TY.G25 - TY.B23 * TY.G16 * TY.G25 - TY.B22 * TY.G17 * TY.G25 - TY.B21 * TY.G18 * TY.G25 - TY.B25 * TY.G13 * TY.G26 - TY.B24 * TY.G14 * TY.G26 - TY.B23 * TY.G15 * TY.G26 - TY.B22 * TY.G16 * TY.G26 - TY.B21 * TY.G17 * TY.G26
    TY.w[
        6] += 2 * TY.B14 * TY.G24 * TY.G26 - TY.B24 * TY.G13 * TY.G27 - TY.B23 * TY.G14 * TY.G27 - TY.B22 * TY.G15 * TY.G27 - TY.B21 * TY.G16 * TY.G27 + 2 * TY.B14 * TY.G23 * TY.G27 + 2 * TY.B12 * TY.G25 * TY.G27 - TY.B23 * TY.G13 * TY.G28 - TY.B22 * TY.G14 * TY.G28
    TY.w[
        6] += -TY.B21 * TY.G15 * TY.G28 + 2 * TY.B14 * TY.G22 * TY.G28 + 2 * TY.B12 * TY.G24 * TY.G28 - TY.B22 * TY.G13 * TY.G29 - TY.B21 * TY.G14 * TY.G29 + 2 * TY.B12 * TY.G23 * TY.G29 + TY.B34 * (
                2 * TY.G14 * TY.G16 + 2 * TY.G13 * TY.G17 + pow(TY.G15, 2))
    TY.w[6] += TY.B32 * (2 * (TY.G15 * TY.G17 + TY.G14 * TY.G18 + TY.G13 * TY.G19) + pow(TY.G16, 2)) + TY.B14 * pow(
        TY.G25, 2) + TY.B12 * pow(TY.G26, 2)

    TY.w[7] = 2 * TY.B34 * (TY.G15 * TY.G16 + TY.G14 * TY.G17 + TY.G13 * TY.G18) + 2 * TY.B32 * (
                TY.G110 * TY.G13 + TY.G16 * TY.G17 + TY.G15 * TY.G18 + TY.G14 * TY.G19) - TY.B22 * TY.G13 * TY.G210 - TY.B21 * TY.G14 * TY.G210 - TY.B23 * TY.G110 * TY.G22
    TY.w[
        7] += -TY.B22 * TY.G111 * TY.G22 - TY.B21 * TY.G112 * TY.G22 - TY.B25 * TY.G18 * TY.G22 - TY.B24 * TY.G19 * TY.G22 + TY.G211 * (
                -(
                            TY.B21 * TY.G13) + 2 * TY.B12 * TY.G22) - TY.B22 * TY.G110 * TY.G23 - TY.B21 * TY.G111 * TY.G23 - TY.B25 * TY.G17 * TY.G23
    TY.w[
        7] += -TY.B24 * TY.G18 * TY.G23 - TY.B23 * TY.G19 * TY.G23 + 2 * TY.B12 * TY.G210 * TY.G23 - TY.B21 * TY.G110 * TY.G24 - TY.B25 * TY.G16 * TY.G24 - TY.B24 * TY.G17 * TY.G24 - TY.B23 * TY.G18 * TY.G24 - TY.B22 * TY.G19 * TY.G24 - TY.B25 * TY.G15 * TY.G25
    TY.w[
        7] += -TY.B24 * TY.G16 * TY.G25 - TY.B23 * TY.G17 * TY.G25 - TY.B22 * TY.G18 * TY.G25 - TY.B21 * TY.G19 * TY.G25 - TY.B25 * TY.G14 * TY.G26 - TY.B24 * TY.G15 * TY.G26 - TY.B23 * TY.G16 * TY.G26 - TY.B22 * TY.G17 * TY.G26 - TY.B21 * TY.G18 * TY.G26
    TY.w[
        7] += 2 * TY.B14 * TY.G25 * TY.G26 - TY.B25 * TY.G13 * TY.G27 - TY.B24 * TY.G14 * TY.G27 - TY.B23 * TY.G15 * TY.G27 - TY.B22 * TY.G16 * TY.G27 - TY.B21 * TY.G17 * TY.G27 + 2 * TY.B14 * TY.G24 * TY.G27 + 2 * TY.B12 * TY.G26 * TY.G27 - TY.B24 * TY.G13 * TY.G28
    TY.w[
        7] += -TY.B23 * TY.G14 * TY.G28 - TY.B22 * TY.G15 * TY.G28 - TY.B21 * TY.G16 * TY.G28 + 2 * TY.B14 * TY.G23 * TY.G28 + 2 * TY.B12 * TY.G25 * TY.G28 - TY.B23 * TY.G13 * TY.G29 - TY.B22 * TY.G14 * TY.G29 - TY.B21 * TY.G15 * TY.G29 + 2 * TY.B14 * TY.G22 * TY.G29
    TY.w[7] += 2 * TY.B12 * TY.G24 * TY.G29

    TY.w[8] = -(
                TY.B23 * TY.G13 * TY.G210) - TY.B22 * TY.G14 * TY.G210 - TY.B21 * TY.G15 * TY.G210 - TY.B22 * TY.G13 * TY.G211 - TY.B21 * TY.G14 * TY.G211 - TY.B21 * TY.G13 * TY.G212 - TY.B24 * TY.G110 * TY.G22 - TY.B23 * TY.G111 * TY.G22 - TY.B22 * TY.G112 * TY.G22
    TY.w[
        8] += -TY.B21 * TY.G113 * TY.G22 - TY.B25 * TY.G19 * TY.G22 + 2 * TY.B14 * TY.G210 * TY.G22 + 2 * TY.B12 * TY.G212 * TY.G22 - TY.B23 * TY.G110 * TY.G23 - TY.B22 * TY.G111 * TY.G23 - TY.B21 * TY.G112 * TY.G23 - TY.B25 * TY.G18 * TY.G23 - TY.B24 * TY.G19 * TY.G23
    TY.w[
        8] += 2 * TY.B12 * TY.G211 * TY.G23 - TY.B22 * TY.G110 * TY.G24 - TY.B21 * TY.G111 * TY.G24 - TY.B25 * TY.G17 * TY.G24 - TY.B24 * TY.G18 * TY.G24 - TY.B23 * TY.G19 * TY.G24 + 2 * TY.B12 * TY.G210 * TY.G24 - TY.B21 * TY.G110 * TY.G25 - TY.B25 * TY.G16 * TY.G25
    TY.w[
        8] += -TY.B24 * TY.G17 * TY.G25 - TY.B23 * TY.G18 * TY.G25 - TY.B22 * TY.G19 * TY.G25 - TY.B25 * TY.G15 * TY.G26 - TY.B24 * TY.G16 * TY.G26 - TY.B23 * TY.G17 * TY.G26 - TY.B22 * TY.G18 * TY.G26 - TY.B21 * TY.G19 * TY.G26 - TY.B25 * TY.G14 * TY.G27
    TY.w[
        8] += -TY.B24 * TY.G15 * TY.G27 - TY.B23 * TY.G16 * TY.G27 - TY.B22 * TY.G17 * TY.G27 - TY.B21 * TY.G18 * TY.G27 + 2 * TY.B14 * TY.G25 * TY.G27 - TY.B25 * TY.G13 * TY.G28 - TY.B24 * TY.G14 * TY.G28 - TY.B23 * TY.G15 * TY.G28 - TY.B22 * TY.G16 * TY.G28
    TY.w[
        8] += -TY.B21 * TY.G17 * TY.G28 + 2 * TY.B14 * TY.G24 * TY.G28 + 2 * TY.B12 * TY.G26 * TY.G28 - TY.B24 * TY.G13 * TY.G29 - TY.B23 * TY.G14 * TY.G29 - TY.B22 * TY.G15 * TY.G29 - TY.B21 * TY.G16 * TY.G29 + 2 * TY.B14 * TY.G23 * TY.G29 + 2 * TY.B12 * TY.G25 * TY.G29
    TY.w[8] += TY.B34 * (2 * (TY.G15 * TY.G17 + TY.G14 * TY.G18 + TY.G13 * TY.G19) + pow(TY.G16, 2)) + TY.B32 * (
                2 * (TY.G111 * TY.G13 + TY.G110 * TY.G14 + TY.G16 * TY.G18 + TY.G15 * TY.G19) + pow(TY.G17,
                                                                                                    2)) + TY.B14 * pow(
        TY.G26, 2)
    TY.w[8] += TY.B12 * pow(TY.G27, 2)

    TY.w[9] = 2 * TY.B34 * (TY.G110 * TY.G13 + TY.G16 * TY.G17 + TY.G15 * TY.G18 + TY.G14 * TY.G19) + 2 * TY.B32 * (
                TY.G112 * TY.G13 + TY.G111 * TY.G14 + TY.G110 * TY.G15 + TY.G17 * TY.G18 + TY.G16 * TY.G19) - TY.B24 * TY.G13 * TY.G210 - TY.B23 * TY.G14 * TY.G210
    TY.w[
        9] += -TY.B22 * TY.G15 * TY.G210 - TY.B21 * TY.G16 * TY.G210 - TY.B23 * TY.G13 * TY.G211 - TY.B22 * TY.G14 * TY.G211 - TY.B21 * TY.G15 * TY.G211 - TY.B22 * TY.G13 * TY.G212 - TY.B21 * TY.G14 * TY.G212 - TY.B25 * TY.G110 * TY.G22 - TY.B24 * TY.G111 * TY.G22
    TY.w[9] += -TY.B23 * TY.G112 * TY.G22 - TY.B22 * TY.G113 * TY.G22 + 2 * TY.B14 * TY.G211 * TY.G22 + TY.G213 * (-(
                TY.B21 * TY.G13) + 2 * TY.B12 * TY.G22) - TY.B24 * TY.G110 * TY.G23 - TY.B23 * TY.G111 * TY.G23 - TY.B22 * TY.G112 * TY.G23
    TY.w[
        9] += -TY.B21 * TY.G113 * TY.G23 - TY.B25 * TY.G19 * TY.G23 + 2 * TY.B14 * TY.G210 * TY.G23 + 2 * TY.B12 * TY.G212 * TY.G23 - TY.B23 * TY.G110 * TY.G24 - TY.B22 * TY.G111 * TY.G24 - TY.B21 * TY.G112 * TY.G24 - TY.B25 * TY.G18 * TY.G24 - TY.B24 * TY.G19 * TY.G24
    TY.w[
        9] += 2 * TY.B12 * TY.G211 * TY.G24 - TY.B22 * TY.G110 * TY.G25 - TY.B21 * TY.G111 * TY.G25 - TY.B25 * TY.G17 * TY.G25 - TY.B24 * TY.G18 * TY.G25 - TY.B23 * TY.G19 * TY.G25 + 2 * TY.B12 * TY.G210 * TY.G25 - TY.B21 * TY.G110 * TY.G26 - TY.B25 * TY.G16 * TY.G26
    TY.w[
        9] += -TY.B24 * TY.G17 * TY.G26 - TY.B23 * TY.G18 * TY.G26 - TY.B22 * TY.G19 * TY.G26 - TY.B25 * TY.G15 * TY.G27 - TY.B24 * TY.G16 * TY.G27 - TY.B23 * TY.G17 * TY.G27 - TY.B22 * TY.G18 * TY.G27 - TY.B21 * TY.G19 * TY.G27 + 2 * TY.B14 * TY.G26 * TY.G27
    TY.w[
        9] += -TY.B25 * TY.G14 * TY.G28 - TY.B24 * TY.G15 * TY.G28 - TY.B23 * TY.G16 * TY.G28 - TY.B22 * TY.G17 * TY.G28 - TY.B21 * TY.G18 * TY.G28 + 2 * TY.B14 * TY.G25 * TY.G28 + 2 * TY.B12 * TY.G27 * TY.G28 - TY.B25 * TY.G13 * TY.G29 - TY.B24 * TY.G14 * TY.G29
    TY.w[
        9] += -TY.B23 * TY.G15 * TY.G29 - TY.B22 * TY.G16 * TY.G29 - TY.B21 * TY.G17 * TY.G29 + 2 * TY.B14 * TY.G24 * TY.G29 + 2 * TY.B12 * TY.G26 * TY.G29

    TY.w[10] = -(
                TY.B25 * TY.G13 * TY.G210) - TY.B24 * TY.G14 * TY.G210 - TY.B23 * TY.G15 * TY.G210 - TY.B22 * TY.G16 * TY.G210 - TY.B21 * TY.G17 * TY.G210 - TY.B24 * TY.G13 * TY.G211 - TY.B23 * TY.G14 * TY.G211 - TY.B22 * TY.G15 * TY.G211 - TY.B21 * TY.G16 * TY.G211
    TY.w[
        10] += -TY.B23 * TY.G13 * TY.G212 - TY.B22 * TY.G14 * TY.G212 - TY.B21 * TY.G15 * TY.G212 - TY.B22 * TY.G13 * TY.G213 - TY.B21 * TY.G14 * TY.G213 - TY.B21 * TY.G13 * TY.G214 - TY.B25 * TY.G111 * TY.G22 - TY.B24 * TY.G112 * TY.G22 - TY.B23 * TY.G113 * TY.G22
    TY.w[
        10] += 2 * TY.B14 * TY.G212 * TY.G22 + 2 * TY.B12 * TY.G214 * TY.G22 - TY.B25 * TY.G110 * TY.G23 - TY.B24 * TY.G111 * TY.G23 - TY.B23 * TY.G112 * TY.G23 - TY.B22 * TY.G113 * TY.G23 + 2 * TY.B14 * TY.G211 * TY.G23 + 2 * TY.B12 * TY.G213 * TY.G23
    TY.w[
        10] += -TY.B24 * TY.G110 * TY.G24 - TY.B23 * TY.G111 * TY.G24 - TY.B22 * TY.G112 * TY.G24 - TY.B21 * TY.G113 * TY.G24 - TY.B25 * TY.G19 * TY.G24 + 2 * TY.B14 * TY.G210 * TY.G24 + 2 * TY.B12 * TY.G212 * TY.G24 - TY.B23 * TY.G110 * TY.G25
    TY.w[
        10] += -TY.B22 * TY.G111 * TY.G25 - TY.B21 * TY.G112 * TY.G25 - TY.B25 * TY.G18 * TY.G25 - TY.B24 * TY.G19 * TY.G25 + 2 * TY.B12 * TY.G211 * TY.G25 - TY.B22 * TY.G110 * TY.G26 - TY.B21 * TY.G111 * TY.G26 - TY.B25 * TY.G17 * TY.G26 - TY.B24 * TY.G18 * TY.G26
    TY.w[
        10] += -TY.B23 * TY.G19 * TY.G26 + 2 * TY.B12 * TY.G210 * TY.G26 - TY.B21 * TY.G110 * TY.G27 - TY.B25 * TY.G16 * TY.G27 - TY.B24 * TY.G17 * TY.G27 - TY.B23 * TY.G18 * TY.G27 - TY.B22 * TY.G19 * TY.G27 - TY.B25 * TY.G15 * TY.G28 - TY.B24 * TY.G16 * TY.G28
    TY.w[
        10] += -TY.B23 * TY.G17 * TY.G28 - TY.B22 * TY.G18 * TY.G28 - TY.B21 * TY.G19 * TY.G28 + 2 * TY.B14 * TY.G26 * TY.G28 - TY.B25 * TY.G14 * TY.G29 - TY.B24 * TY.G15 * TY.G29 - TY.B23 * TY.G16 * TY.G29 - TY.B22 * TY.G17 * TY.G29 - TY.B21 * TY.G18 * TY.G29
    TY.w[10] += 2 * TY.B14 * TY.G25 * TY.G29 + 2 * TY.B12 * TY.G27 * TY.G29 + TY.B34 * (
                2 * (TY.G111 * TY.G13 + TY.G110 * TY.G14 + TY.G16 * TY.G18 + TY.G15 * TY.G19) + pow(TY.G17, 2))
    TY.w[10] += TY.B32 * (
                2 * (TY.G113 * TY.G13 + TY.G112 * TY.G14 + TY.G111 * TY.G15 + TY.G110 * TY.G16 + TY.G17 * TY.G19) + pow(
            TY.G18, 2)) + TY.B14 * pow(TY.G27, 2) + TY.B12 * pow(TY.G28, 2)

    TY.w[11] = 2 * TY.B34 * (
                TY.G112 * TY.G13 + TY.G111 * TY.G14 + TY.G110 * TY.G15 + TY.G17 * TY.G18 + TY.G16 * TY.G19) + 2 * TY.B32 * (
                           TY.G113 * TY.G14 + TY.G112 * TY.G15 + TY.G111 * TY.G16 + TY.G110 * TY.G17 + TY.G18 * TY.G19) - TY.B25 * TY.G14 * TY.G210
    TY.w[
        11] += -TY.B24 * TY.G15 * TY.G210 - TY.B23 * TY.G16 * TY.G210 - TY.B22 * TY.G17 * TY.G210 - TY.B21 * TY.G18 * TY.G210 - TY.B25 * TY.G13 * TY.G211 - TY.B24 * TY.G14 * TY.G211 - TY.B23 * TY.G15 * TY.G211 - TY.B22 * TY.G16 * TY.G211 - TY.B21 * TY.G17 * TY.G211
    TY.w[
        11] += -TY.B24 * TY.G13 * TY.G212 - TY.B23 * TY.G14 * TY.G212 - TY.B22 * TY.G15 * TY.G212 - TY.B21 * TY.G16 * TY.G212 - TY.B23 * TY.G13 * TY.G213 - TY.B22 * TY.G14 * TY.G213 - TY.B21 * TY.G15 * TY.G213 - TY.B25 * TY.G112 * TY.G22 - TY.B24 * TY.G113 * TY.G22
    TY.w[
        11] += 2 * TY.B14 * TY.G213 * TY.G22 - TY.B25 * TY.G111 * TY.G23 - TY.B24 * TY.G112 * TY.G23 - TY.B23 * TY.G113 * TY.G23 + 2 * TY.B14 * TY.G212 * TY.G23 - TY.G214 * (
                TY.B22 * TY.G13 + TY.B21 * TY.G14 - 2 * TY.B12 * TY.G23) - TY.B25 * TY.G110 * TY.G24
    TY.w[
        11] += -TY.B24 * TY.G111 * TY.G24 - TY.B23 * TY.G112 * TY.G24 - TY.B22 * TY.G113 * TY.G24 + 2 * TY.B14 * TY.G211 * TY.G24 + 2 * TY.B12 * TY.G213 * TY.G24 - TY.B24 * TY.G110 * TY.G25 - TY.B23 * TY.G111 * TY.G25 - TY.B22 * TY.G112 * TY.G25
    TY.w[
        11] += -TY.B21 * TY.G113 * TY.G25 - TY.B25 * TY.G19 * TY.G25 + 2 * TY.B14 * TY.G210 * TY.G25 + 2 * TY.B12 * TY.G212 * TY.G25 - TY.B23 * TY.G110 * TY.G26 - TY.B22 * TY.G111 * TY.G26 - TY.B21 * TY.G112 * TY.G26 - TY.B25 * TY.G18 * TY.G26 - TY.B24 * TY.G19 * TY.G26
    TY.w[
        11] += 2 * TY.B12 * TY.G211 * TY.G26 - TY.B22 * TY.G110 * TY.G27 - TY.B21 * TY.G111 * TY.G27 - TY.B25 * TY.G17 * TY.G27 - TY.B24 * TY.G18 * TY.G27 - TY.B23 * TY.G19 * TY.G27 + 2 * TY.B12 * TY.G210 * TY.G27 - TY.B21 * TY.G110 * TY.G28 - TY.B25 * TY.G16 * TY.G28
    TY.w[
        11] += -TY.B24 * TY.G17 * TY.G28 - TY.B23 * TY.G18 * TY.G28 - TY.B22 * TY.G19 * TY.G28 + 2 * TY.B14 * TY.G27 * TY.G28 - TY.B25 * TY.G15 * TY.G29 - TY.B24 * TY.G16 * TY.G29 - TY.B23 * TY.G17 * TY.G29 - TY.B22 * TY.G18 * TY.G29 - TY.B21 * TY.G19 * TY.G29
    TY.w[11] += 2 * TY.B14 * TY.G26 * TY.G29 + 2 * TY.B12 * TY.G28 * TY.G29

    TY.w[12] = -(
                TY.B25 * TY.G15 * TY.G210) - TY.B24 * TY.G16 * TY.G210 - TY.B23 * TY.G17 * TY.G210 - TY.B22 * TY.G18 * TY.G210 - TY.B21 * TY.G19 * TY.G210 - TY.B25 * TY.G14 * TY.G211 - TY.B24 * TY.G15 * TY.G211 - TY.B23 * TY.G16 * TY.G211 - TY.B22 * TY.G17 * TY.G211
    TY.w[
        12] += -TY.B21 * TY.G18 * TY.G211 - TY.B25 * TY.G13 * TY.G212 - TY.B24 * TY.G14 * TY.G212 - TY.B23 * TY.G15 * TY.G212 - TY.B22 * TY.G16 * TY.G212 - TY.B21 * TY.G17 * TY.G212 - TY.B24 * TY.G13 * TY.G213 - TY.B23 * TY.G14 * TY.G213 - TY.B22 * TY.G15 * TY.G213
    TY.w[
        12] += -TY.B21 * TY.G16 * TY.G213 - TY.B25 * TY.G113 * TY.G22 - TY.B25 * TY.G112 * TY.G23 - TY.B24 * TY.G113 * TY.G23 + 2 * TY.B14 * TY.G213 * TY.G23 - TY.B25 * TY.G111 * TY.G24 - TY.B24 * TY.G112 * TY.G24 - TY.B23 * TY.G113 * TY.G24
    TY.w[12] += 2 * TY.B14 * TY.G212 * TY.G24 - TY.G214 * (
                TY.B23 * TY.G13 + TY.B22 * TY.G14 + TY.B21 * TY.G15 - 2 * TY.B14 * TY.G22 - 2 * TY.B12 * TY.G24) - TY.B25 * TY.G110 * TY.G25 - TY.B24 * TY.G111 * TY.G25 - TY.B23 * TY.G112 * TY.G25
    TY.w[
        12] += -TY.B22 * TY.G113 * TY.G25 + 2 * TY.B14 * TY.G211 * TY.G25 + 2 * TY.B12 * TY.G213 * TY.G25 - TY.B24 * TY.G110 * TY.G26 - TY.B23 * TY.G111 * TY.G26 - TY.B22 * TY.G112 * TY.G26 - TY.B21 * TY.G113 * TY.G26 - TY.B25 * TY.G19 * TY.G26
    TY.w[
        12] += 2 * TY.B14 * TY.G210 * TY.G26 + 2 * TY.B12 * TY.G212 * TY.G26 - TY.B23 * TY.G110 * TY.G27 - TY.B22 * TY.G111 * TY.G27 - TY.B21 * TY.G112 * TY.G27 - TY.B25 * TY.G18 * TY.G27 - TY.B24 * TY.G19 * TY.G27 + 2 * TY.B12 * TY.G211 * TY.G27
    TY.w[
        12] += -TY.B22 * TY.G110 * TY.G28 - TY.B21 * TY.G111 * TY.G28 - TY.B25 * TY.G17 * TY.G28 - TY.B24 * TY.G18 * TY.G28 - TY.B23 * TY.G19 * TY.G28 + 2 * TY.B12 * TY.G210 * TY.G28 - TY.B21 * TY.G110 * TY.G29 - TY.B25 * TY.G16 * TY.G29 - TY.B24 * TY.G17 * TY.G29
    TY.w[12] += -TY.B23 * TY.G18 * TY.G29 - TY.B22 * TY.G19 * TY.G29 + 2 * TY.B14 * TY.G27 * TY.G29 + TY.B34 * (
                2 * (TY.G113 * TY.G13 + TY.G112 * TY.G14 + TY.G111 * TY.G15 + TY.G110 * TY.G16 + TY.G17 * TY.G19) + pow(
            TY.G18, 2))
    TY.w[12] += TY.B32 * (2 * (TY.G113 * TY.G15 + TY.G112 * TY.G16 + TY.G111 * TY.G17 + TY.G110 * TY.G18) + pow(TY.G19,
                                                                                                                2)) + TY.B14 * pow(
        TY.G28, 2) + TY.B12 * pow(TY.G29, 2)

    TY.w[13] = 2 * TY.B32 * (TY.G113 * TY.G16 + TY.G112 * TY.G17 + TY.G111 * TY.G18 + TY.G110 * TY.G19) + 2 * TY.B34 * (
                TY.G113 * TY.G14 + TY.G112 * TY.G15 + TY.G111 * TY.G16 + TY.G110 * TY.G17 + TY.G18 * TY.G19) - TY.B21 * TY.G110 * TY.G210
    TY.w[
        13] += -TY.B25 * TY.G16 * TY.G210 - TY.B24 * TY.G17 * TY.G210 - TY.B23 * TY.G18 * TY.G210 - TY.B22 * TY.G19 * TY.G210 - TY.B25 * TY.G15 * TY.G211 - TY.B24 * TY.G16 * TY.G211 - TY.B23 * TY.G17 * TY.G211 - TY.B22 * TY.G18 * TY.G211 - TY.B21 * TY.G19 * TY.G211
    TY.w[
        13] += -TY.B25 * TY.G14 * TY.G212 - TY.B24 * TY.G15 * TY.G212 - TY.B23 * TY.G16 * TY.G212 - TY.B22 * TY.G17 * TY.G212 - TY.B21 * TY.G18 * TY.G212 - TY.B25 * TY.G13 * TY.G213 - TY.B24 * TY.G14 * TY.G213 - TY.B23 * TY.G15 * TY.G213 - TY.B22 * TY.G16 * TY.G213
    TY.w[
        13] += -TY.B21 * TY.G17 * TY.G213 - TY.B25 * TY.G113 * TY.G23 - TY.B25 * TY.G112 * TY.G24 - TY.B24 * TY.G113 * TY.G24 + 2 * TY.B14 * TY.G213 * TY.G24 - TY.B25 * TY.G111 * TY.G25 - TY.B24 * TY.G112 * TY.G25 - TY.B23 * TY.G113 * TY.G25
    TY.w[13] += 2 * TY.B14 * TY.G212 * TY.G25 - TY.G214 * (
                TY.B24 * TY.G13 + TY.B23 * TY.G14 + TY.B22 * TY.G15 + TY.B21 * TY.G16 - 2 * TY.B14 * TY.G23 - 2 * TY.B12 * TY.G25) - TY.B25 * TY.G110 * TY.G26 - TY.B24 * TY.G111 * TY.G26 - TY.B23 * TY.G112 * TY.G26
    TY.w[
        13] += -TY.B22 * TY.G113 * TY.G26 + 2 * TY.B14 * TY.G211 * TY.G26 + 2 * TY.B12 * TY.G213 * TY.G26 - TY.B24 * TY.G110 * TY.G27 - TY.B23 * TY.G111 * TY.G27 - TY.B22 * TY.G112 * TY.G27 - TY.B21 * TY.G113 * TY.G27 - TY.B25 * TY.G19 * TY.G27
    TY.w[
        13] += 2 * TY.B14 * TY.G210 * TY.G27 + 2 * TY.B12 * TY.G212 * TY.G27 - TY.B23 * TY.G110 * TY.G28 - TY.B22 * TY.G111 * TY.G28 - TY.B21 * TY.G112 * TY.G28 - TY.B25 * TY.G18 * TY.G28 - TY.B24 * TY.G19 * TY.G28 + 2 * TY.B12 * TY.G211 * TY.G28
    TY.w[
        13] += -TY.B22 * TY.G110 * TY.G29 - TY.B21 * TY.G111 * TY.G29 - TY.B25 * TY.G17 * TY.G29 - TY.B24 * TY.G18 * TY.G29 - TY.B23 * TY.G19 * TY.G29 + 2 * TY.B12 * TY.G210 * TY.G29 + 2 * TY.B14 * TY.G28 * TY.G29

    TY.w[14] = -(
                TY.B22 * TY.G110 * TY.G210) - TY.B21 * TY.G111 * TY.G210 - TY.B25 * TY.G17 * TY.G210 - TY.B24 * TY.G18 * TY.G210 - TY.B23 * TY.G19 * TY.G210 - TY.B21 * TY.G110 * TY.G211 - TY.B25 * TY.G16 * TY.G211 - TY.B24 * TY.G17 * TY.G211
    TY.w[
        14] += -TY.B23 * TY.G18 * TY.G211 - TY.B22 * TY.G19 * TY.G211 - TY.B25 * TY.G15 * TY.G212 - TY.B24 * TY.G16 * TY.G212 - TY.B23 * TY.G17 * TY.G212 - TY.B22 * TY.G18 * TY.G212 - TY.B21 * TY.G19 * TY.G212 - TY.B25 * TY.G14 * TY.G213 - TY.B24 * TY.G15 * TY.G213
    TY.w[
        14] += -TY.B23 * TY.G16 * TY.G213 - TY.B22 * TY.G17 * TY.G213 - TY.B21 * TY.G18 * TY.G213 - TY.B25 * TY.G113 * TY.G24 - TY.B25 * TY.G112 * TY.G25 - TY.B24 * TY.G113 * TY.G25 + 2 * TY.B14 * TY.G213 * TY.G25 - TY.B25 * TY.G111 * TY.G26 - TY.B24 * TY.G112 * TY.G26
    TY.w[14] += -TY.B23 * TY.G113 * TY.G26 + 2 * TY.B14 * TY.G212 * TY.G26 - TY.G214 * (
                TY.B25 * TY.G13 + TY.B24 * TY.G14 + TY.B23 * TY.G15 + TY.B22 * TY.G16 + TY.B21 * TY.G17 - 2 * TY.B14 * TY.G24 - 2 * TY.B12 * TY.G26) - TY.B25 * TY.G110 * TY.G27
    TY.w[
        14] += -TY.B24 * TY.G111 * TY.G27 - TY.B23 * TY.G112 * TY.G27 - TY.B22 * TY.G113 * TY.G27 + 2 * TY.B14 * TY.G211 * TY.G27 + 2 * TY.B12 * TY.G213 * TY.G27 - TY.B24 * TY.G110 * TY.G28 - TY.B23 * TY.G111 * TY.G28 - TY.B22 * TY.G112 * TY.G28
    TY.w[
        14] += -TY.B21 * TY.G113 * TY.G28 - TY.B25 * TY.G19 * TY.G28 + 2 * TY.B14 * TY.G210 * TY.G28 + 2 * TY.B12 * TY.G212 * TY.G28 - TY.B23 * TY.G110 * TY.G29 - TY.B22 * TY.G111 * TY.G29 - TY.B21 * TY.G112 * TY.G29 - TY.B25 * TY.G18 * TY.G29 - TY.B24 * TY.G19 * TY.G29
    TY.w[14] += 2 * TY.B12 * TY.G211 * TY.G29 + TY.B32 * (
                2 * (TY.G113 * TY.G17 + TY.G112 * TY.G18 + TY.G111 * TY.G19) + pow(TY.G110, 2))
    TY.w[14] += TY.B34 * (2 * (TY.G113 * TY.G15 + TY.G112 * TY.G16 + TY.G111 * TY.G17 + TY.G110 * TY.G18) + pow(TY.G19,
                                                                                                                2)) + TY.B12 * pow(
        TY.G210, 2) + TY.B14 * pow(TY.G29, 2)

    TY.w[15] = 2 * TY.B34 * (TY.G113 * TY.G16 + TY.G112 * TY.G17 + TY.G111 * TY.G18 + TY.G110 * TY.G19) + 2 * TY.B32 * (
                TY.G110 * TY.G111 + TY.G113 * TY.G18 + TY.G112 * TY.G19) - TY.B23 * TY.G110 * TY.G210 - TY.B22 * TY.G111 * TY.G210
    TY.w[
        15] += -TY.B21 * TY.G112 * TY.G210 - TY.B25 * TY.G18 * TY.G210 - TY.B24 * TY.G19 * TY.G210 - TY.B22 * TY.G110 * TY.G211 - TY.B21 * TY.G111 * TY.G211 - TY.B25 * TY.G17 * TY.G211 - TY.B24 * TY.G18 * TY.G211 - TY.B23 * TY.G19 * TY.G211
    TY.w[
        15] += 2 * TY.B12 * TY.G210 * TY.G211 - TY.B21 * TY.G110 * TY.G212 - TY.B25 * TY.G16 * TY.G212 - TY.B24 * TY.G17 * TY.G212 - TY.B23 * TY.G18 * TY.G212 - TY.B22 * TY.G19 * TY.G212 - TY.B25 * TY.G15 * TY.G213 - TY.B24 * TY.G16 * TY.G213
    TY.w[
        15] += -TY.B23 * TY.G17 * TY.G213 - TY.B22 * TY.G18 * TY.G213 - TY.B21 * TY.G19 * TY.G213 - TY.B25 * TY.G113 * TY.G25 - TY.B25 * TY.G112 * TY.G26 - TY.B24 * TY.G113 * TY.G26 + 2 * TY.B14 * TY.G213 * TY.G26 - TY.B25 * TY.G111 * TY.G27 - TY.B24 * TY.G112 * TY.G27
    TY.w[15] += -TY.B23 * TY.G113 * TY.G27 + 2 * TY.B14 * TY.G212 * TY.G27 - TY.G214 * (
                TY.B25 * TY.G14 + TY.B24 * TY.G15 + TY.B23 * TY.G16 + TY.B22 * TY.G17 + TY.B21 * TY.G18 - 2 * TY.B14 * TY.G25 - 2 * TY.B12 * TY.G27) - TY.B25 * TY.G110 * TY.G28
    TY.w[
        15] += -TY.B24 * TY.G111 * TY.G28 - TY.B23 * TY.G112 * TY.G28 - TY.B22 * TY.G113 * TY.G28 + 2 * TY.B14 * TY.G211 * TY.G28 + 2 * TY.B12 * TY.G213 * TY.G28 - TY.B24 * TY.G110 * TY.G29 - TY.B23 * TY.G111 * TY.G29 - TY.B22 * TY.G112 * TY.G29
    TY.w[
        15] += -TY.B21 * TY.G113 * TY.G29 - TY.B25 * TY.G19 * TY.G29 + 2 * TY.B14 * TY.G210 * TY.G29 + 2 * TY.B12 * TY.G212 * TY.G29

    TY.w[16] = -(
                TY.B24 * TY.G110 * TY.G210) - TY.B23 * TY.G111 * TY.G210 - TY.B22 * TY.G112 * TY.G210 - TY.B21 * TY.G113 * TY.G210 - TY.B25 * TY.G19 * TY.G210 - TY.B23 * TY.G110 * TY.G211 - TY.B22 * TY.G111 * TY.G211 - TY.B21 * TY.G112 * TY.G211
    TY.w[
        16] += -TY.B25 * TY.G18 * TY.G211 - TY.B24 * TY.G19 * TY.G211 - TY.B22 * TY.G110 * TY.G212 - TY.B21 * TY.G111 * TY.G212 - TY.B25 * TY.G17 * TY.G212 - TY.B24 * TY.G18 * TY.G212 - TY.B23 * TY.G19 * TY.G212 + 2 * TY.B12 * TY.G210 * TY.G212
    TY.w[
        16] += -TY.B21 * TY.G110 * TY.G213 - TY.B25 * TY.G16 * TY.G213 - TY.B24 * TY.G17 * TY.G213 - TY.B23 * TY.G18 * TY.G213 - TY.B22 * TY.G19 * TY.G213 - TY.B25 * TY.G113 * TY.G26 - TY.B25 * TY.G112 * TY.G27 - TY.B24 * TY.G113 * TY.G27
    TY.w[
        16] += 2 * TY.B14 * TY.G213 * TY.G27 - TY.B25 * TY.G111 * TY.G28 - TY.B24 * TY.G112 * TY.G28 - TY.B23 * TY.G113 * TY.G28 + 2 * TY.B14 * TY.G212 * TY.G28
    TY.w[16] += -TY.G214 * (
                TY.B25 * TY.G15 + TY.B24 * TY.G16 + TY.B23 * TY.G17 + TY.B22 * TY.G18 + TY.B21 * TY.G19 - 2 * TY.B14 * TY.G26 - 2 * TY.B12 * TY.G28) - TY.B25 * TY.G110 * TY.G29 - TY.B24 * TY.G111 * TY.G29 - TY.B23 * TY.G112 * TY.G29
    TY.w[16] += -TY.B22 * TY.G113 * TY.G29 + 2 * TY.B14 * TY.G211 * TY.G29 + 2 * TY.B12 * TY.G213 * TY.G29 + TY.B34 * (
                2 * (TY.G113 * TY.G17 + TY.G112 * TY.G18 + TY.G111 * TY.G19) + pow(TY.G110, 2))
    TY.w[16] += TY.B32 * (2 * TY.G110 * TY.G112 + 2 * TY.G113 * TY.G19 + pow(TY.G111, 2)) + \
                TY.B14 * pow(TY.G210, 2) + TY.B12 * pow(TY.G211, 2)

    TY.w[17] = 2 * TY.B32 * (TY.G111 * TY.G112 + TY.G110 * TY.G113) + 2 * TY.B34 * (
                TY.G110 * TY.G111 + TY.G113 * TY.G18 + TY.G112 * TY.G19) - TY.B25 * TY.G110 * TY.G210 - TY.B24 * TY.G111 * TY.G210 - TY.B23 * TY.G112 * TY.G210 - TY.B22 * TY.G113 * TY.G210
    TY.w[
        17] += -TY.B24 * TY.G110 * TY.G211 - TY.B23 * TY.G111 * TY.G211 - TY.B22 * TY.G112 * TY.G211 - TY.B21 * TY.G113 * TY.G211 - TY.B25 * TY.G19 * TY.G211 + 2 * TY.B14 * TY.G210 * TY.G211 - TY.B23 * TY.G110 * TY.G212 - TY.B22 * TY.G111 * TY.G212
    TY.w[
        17] += -TY.B21 * TY.G112 * TY.G212 - TY.B25 * TY.G18 * TY.G212 - TY.B24 * TY.G19 * TY.G212 + 2 * TY.B12 * TY.G211 * TY.G212 - TY.B22 * TY.G110 * TY.G213 - TY.B21 * TY.G111 * TY.G213 - TY.B25 * TY.G17 * TY.G213 - TY.B24 * TY.G18 * TY.G213
    TY.w[
        17] += -TY.B23 * TY.G19 * TY.G213 + 2 * TY.B12 * TY.G210 * TY.G213 - TY.B25 * TY.G113 * TY.G27 - TY.B25 * TY.G112 * TY.G28 - TY.B24 * TY.G113 * TY.G28 + 2 * TY.B14 * TY.G213 * TY.G28 - TY.B25 * TY.G111 * TY.G29 - TY.B24 * TY.G112 * TY.G29
    TY.w[17] += -TY.B23 * TY.G113 * TY.G29 + 2 * TY.B14 * TY.G212 * TY.G29 - TY.G214 * (
                TY.B21 * TY.G110 + TY.B25 * TY.G16 + TY.B24 * TY.G17 + TY.B23 * TY.G18 + TY.B22 * TY.G19 - 2 * TY.B14 * TY.G27 - 2 * TY.B12 * TY.G29)

    TY.w[18] = -(
                TY.B25 * TY.G111 * TY.G210) - TY.B24 * TY.G112 * TY.G210 - TY.B23 * TY.G113 * TY.G210 - TY.B25 * TY.G110 * TY.G211 - TY.B24 * TY.G111 * TY.G211 - TY.B23 * TY.G112 * TY.G211 - TY.B22 * TY.G113 * TY.G211 - TY.B24 * TY.G110 * TY.G212
    TY.w[
        18] += -TY.B23 * TY.G111 * TY.G212 - TY.B22 * TY.G112 * TY.G212 - TY.B21 * TY.G113 * TY.G212 - TY.B25 * TY.G19 * TY.G212 + 2 * TY.B14 * TY.G210 * TY.G212 - TY.B23 * TY.G110 * TY.G213 - TY.B22 * TY.G111 * TY.G213 - TY.B21 * TY.G112 * TY.G213
    TY.w[
        18] += -TY.B25 * TY.G18 * TY.G213 - TY.B24 * TY.G19 * TY.G213 + 2 * TY.B12 * TY.G211 * TY.G213 - TY.B25 * TY.G113 * TY.G28
    TY.w[18] += -TY.G214 * (
                TY.B22 * TY.G110 + TY.B21 * TY.G111 + TY.B25 * TY.G17 + TY.B24 * TY.G18 + TY.B23 * TY.G19 - 2 * TY.B12 * TY.G210 - 2 * TY.B14 * TY.G28) - TY.B25 * TY.G112 * TY.G29 - TY.B24 * TY.G113 * TY.G29 + 2 * TY.B14 * TY.G213 * TY.G29
    TY.w[18] += TY.B34 * (2 * TY.G110 * TY.G112 + 2 * TY.G113 * TY.G19 + pow(TY.G111, 2)) + TY.B32 * (
                2 * TY.G111 * TY.G113 + pow(TY.G112, 2)) + TY.B14 * pow(TY.G211, 2) + TY.B12 * pow(TY.G212, 2)

    TY.w[19] = 2 * TY.B32 * TY.G112 * TY.G113 + 2 * TY.B34 * (
                TY.G111 * TY.G112 + TY.G110 * TY.G113) - TY.B25 * TY.G112 * TY.G210 - TY.B24 * TY.G113 * TY.G210 - TY.B25 * TY.G111 * TY.G211 - TY.B24 * TY.G112 * TY.G211 - TY.B23 * TY.G113 * TY.G211
    TY.w[
        19] += -TY.B25 * TY.G110 * TY.G212 - TY.B24 * TY.G111 * TY.G212 - TY.B23 * TY.G112 * TY.G212 - TY.B22 * TY.G113 * TY.G212 + 2 * TY.B14 * TY.G211 * TY.G212 - TY.B24 * TY.G110 * TY.G213 - TY.B23 * TY.G111 * TY.G213 - TY.B22 * TY.G112 * TY.G213
    TY.w[
        19] += -TY.B21 * TY.G113 * TY.G213 - TY.B25 * TY.G19 * TY.G213 + 2 * TY.B14 * TY.G210 * TY.G213 + 2 * TY.B12 * TY.G212 * TY.G213 - TY.B25 * TY.G113 * TY.G29
    TY.w[19] += -TY.G214 * (
                TY.B23 * TY.G110 + TY.B22 * TY.G111 + TY.B21 * TY.G112 + TY.B25 * TY.G18 + TY.B24 * TY.G19 - 2 * TY.B12 * TY.G211 - 2 * TY.B14 * TY.G29)

    TY.w[20] = -(
                TY.B25 * TY.G113 * TY.G210) - TY.B25 * TY.G112 * TY.G211 - TY.B24 * TY.G113 * TY.G211 - TY.B25 * TY.G111 * TY.G212 - TY.B24 * TY.G112 * TY.G212 - TY.B23 * TY.G113 * TY.G212 - TY.B25 * TY.G110 * TY.G213 - TY.B24 * TY.G111 * TY.G213
    TY.w[20] += -TY.B23 * TY.G112 * TY.G213 - TY.B22 * TY.G113 * TY.G213 + 2 * TY.B14 * TY.G211 * TY.G213 - (
                TY.B24 * TY.G110 + TY.B23 * TY.G111 + TY.B22 * TY.G112 + TY.B21 * TY.G113 + TY.B25 * TY.G19 - 2 * TY.B14 * TY.G210 - 2 * TY.B12 * TY.G212) * TY.G214
    TY.w[20] += TY.B34 * (2 * TY.G111 * TY.G113 + pow(TY.G112, 2)) + TY.B32 * pow(TY.G113, 2) + TY.B14 * pow(TY.G212,
                                                                                                             2) + TY.B12 * pow(
        TY.G213, 2)

    TY.w[21] = TY.B25 * (TY.A23 * TY.B14 * (
                -3 * TY.A52 * TY.B24 * TY.B25 + (2 * TY.A43 * TY.B24 + TY.A42 * TY.B25) * TY.B34) + TY.B25 * (
                                     TY.A22 * TY.B14 * (-(TY.A52 * TY.B25) + TY.A43 * TY.B34) + TY.A12 * (
                                         4 * TY.A52 * TY.B24 * TY.B25 - (
                                             3 * TY.A43 * TY.B24 + TY.A42 * TY.B25) * TY.B34))) * pow(TY.B34, 3)

    TY.w[22] = (-(TY.A23 * TY.B14) + TY.A12 * TY.B25) * (TY.A52 * TY.B25 - TY.A43 * TY.B34) * pow(TY.B25, 2) * pow(
        TY.B34, 3)

    # end function TY_ReduceNonlinearSystem( Z1, Z2,  K1,  K2,  phi, )
    # no return value


def TY_capQ(d2):
    global TY
    return d2 * TY.B32 + pow(d2, 3) * TY.B34


def TY_V(d2):
    global TY
    return -(pow(d2, 2) * TY.G13 + pow(d2, 3) * TY.G14 + pow(d2, 4) * TY.G15 + pow(d2, 5) * TY.G16 + pow(d2,
                                                                                                         6) * TY.G17 + pow(
        d2, 7) * TY.G18 + pow(d2, 8) * TY.G19 + pow(d2, 9) * TY.G110 + pow(d2, 10) * TY.G111 + pow(d2,
                                                                                                   11) * TY.G112 + pow(
        d2, 12) * TY.G113)


def TY_capW(d2):
    global TY

    tmp = d2 * TY.G22 + pow(d2, 2) * TY.G23 + pow(d2, 3) * TY.G24 + pow(d2, 4) * TY.G25 + pow(d2, 5) * TY.G26
    tmp += pow(d2, 6) * TY.G27 + pow(d2, 7) * TY.G28 + pow(d2, 8) * TY.G29 + pow(d2, 9) * TY.G210
    tmp += pow(d2, 10) * TY.G211 + pow(d2, 11) * TY.G212 + pow(d2, 12) * TY.G213 + pow(d2, 13) * TY.G214

    return tmp


def TY_X(d2):
    return TY_V(d2) / TY_capW(d2)


# solve the linear system depending on d1, d2 using Cramer's rule
# a,b,c1,c2 are  passed by reference and returned
def TY_SolveLinearEquations(d1, d2):
    # Return values will be a, b, c1, c2 to mimick the &a return-by-reference in igor
    global TY

    det = TY.q22 * d1 * d2
    det_a = TY.qa12 * d2 + TY.qa21 * d1 + TY.qa22 * d1 * d2 + TY.qa23 * d1 * pow(d2, 2) + TY.qa32 * pow(d1, 2) * d2
    det_b = TY.qb12 * d2 + TY.qb21 * d1 + TY.qb22 * d1 * d2 + TY.qb23 * d1 * pow(d2, 2) + TY.qb32 * pow(d1, 2) * d2
    det_c1 = TY.qc112 * d2 + TY.qc121 * d1 + TY.qc122 * d1 * d2 + TY.qc123 * d1 * pow(d2, 2) + TY.qc132 * pow(d1,
                                                                                                              2) * d2
    det_c2 = TY.qc212 * d2 + TY.qc221 * d1 + TY.qc222 * d1 * d2 + TY.qc223 * d1 * pow(d2, 2) + TY.qc232 * pow(d1,
                                                                                                              2) * d2

    a = det_a / det
    b = det_b / det
    c1 = det_c1 / det
    c2 = det_c2 / det

    return a, b, c1, c2


# Solve the system of linear and nonlinear equations for given Zi, Ki, phi which gives at
# most 22 solutions for the parameters a,b,ci,di. From the set of solutions choose the
# physical one (g(r<1) = 0) and return it.
def TY_SolveEquations(Z1, Z2, K1, K2, phi):
    global TY

    # reduce system to a polynomial from which all solution are extracted
    # by doing that a lot of global background variables are set found in TY
    # The polynominal coefficients are in TY.w
    TY_ReduceNonlinearSystem(Z1, Z2, K1, K2, phi)

    # we use np.roots, the algorithm seems to be similar to Igor (companion matrix <=> Jenkins-Traub algorithm in Igor)
    # FindRoots\p takes coefficients in reverse order than np.roots -> invert using [::-1]
    # catch roots resulting from overflow by isfinite
    w = TY.w[::-1]
    roots = np.roots(w[np.isfinite(w)])

    # select real roots and those satisfying Q(x) != 0 and W(x) != 0
    # Paper: Cluster formation in two-Yukawa Fluids, J. Chem. Phys. 122, 2005
    # The right set of (a, b, c1, c2, d1, d2) should have the following properties:
    # (1) a > 0
    # (2) d1, d2 are real
    # (3) vi/Ki > 0 <=> g(Zi) > 0
    # (4) if there is still more than root, calculate g(r) for each root
    #     and g(r) of the correct root should have the minimum average value
    #       inside the hardcore

    nroots = []  # collected new roots
    for ro in roots:
        x = np.real(ro)
        y = np.imag(ro)

        if (chop(y) == 0) & (TY_capW(x) != 0) & (TY_capQ(x) != 0):
            var_d1 = TY_X(x)
            var_d2 = x

            # solution of linear system for given d1, d2 to obtain a,b,ci,di
            # var_a, var_b, var_c1, var_c2 passed by reference in igor => changed to return values in python
            var_a, var_b, var_c1, var_c2 = TY_SolveLinearEquations(var_d1, var_d2)

            # select physical solutions, for details check paper: "Cluster formation in
            # two-Yukawa fluids", J. Chem. Phys. 122 (2005)
            # check for properties (1,2,3,4) above
            if ((var_a > 0) &
                    (TY_g(Z1, phi, Z1, Z2, var_a, var_b, var_c1, var_c2, var_d1, var_d2) > 0) &
                    (TY_g(Z2, phi, Z1, Z2, var_a, var_b, var_c1, var_c2, var_d1, var_d2) > 0)):
                nroots.append([var_a, var_b, var_c1, var_c2, var_d1, var_d2])

    # if there is still more than one root left, than choose the one with the minimum
    # average value inside the hardcore
    # RB: nevertheless we test this for single roots to throw away to strange roots g(r<1) >1

    # the number of q values should be a power of 2 in order to speed up the FFT
    n = 8192  # 2**13   # 16384  #2^14 points
    # the maximum q value should be large enough to enable a reasonable approximation of g(r)
    qmax = 16 * 10 * 2 * pi
    q = np.r_[0:qmax:n * 1j]

    if len(nroots) > 1:
        # loop over all remaining roots
        grincore = []
        for j in range(len(nroots)):
            # calculate structure factor for j root
            sq = SqTwoYukawa(q, Z1, Z2, K1, K2, phi, nroots[j])

            # calculate pair correlation function g(r) for given structure factor
            r, gr = Yuk_SqToGr_FFT(phi, q, sq)

            # determine sum inside the hardcore
            # 0 =< r < 1 of the pair-correlation function (as q is actually q*R r is a scaled function r/R )
            # RB: use 0.1<r<0.9 as edges are often increasing
            grincore.append(np.mean(np.abs(gr[(r < 0.9) & (r > 0.1)])))

        selected_root = np.argmin(grincore)  # index of smallest values
        # best physical solution was found
        a, b, c1, c2, d1, d2 = nroots[selected_root]
        return a, b, c1, c2, d1, d2

    elif len(nroots) == 1:
        # test for good gr
        sq = SqTwoYukawa(q, Z1, Z2, K1, K2, phi, nroots[0])
        r, gr = Yuk_SqToGr_FFT(phi, q, sq)
        grincore = np.mean(np.abs(gr[(r < 0.9) & (r > 0.1)]))
        if grincore > 1:
            return 'bad single root with g(r<1)>1'

        a, b, c1, c2, d1, d2 = nroots[0]
        return a, b, c1, c2, d1, d2
    else:
        # no solution was found
        return 'no root found'
    # end TY_SolveEquations


def Yuk_SqToGr_FFT(phi, q, sq):
    # fourier transform of sq to get gr, return (r,gr)
    n = len(sq)
    qmax = np.max(q)
    dr = 2 * pi / qmax

    p = np.r_[0.:n]  # should be float
    gr = np.zeros_like(p)
    temp = p * (sq - 1)
    alpha = n * pow(qmax / n, 3) / (24 * pi * pi * phi)

    W_FFT = np.fft.fft(temp)
    gr[1:] = 1 - alpha / p[1:] * np.imag(W_FFT)[1:]  # sin transform is -imag(fft)
    gr[0] = 0
    r = p * dr

    return r, gr

#  ###########end converted procedures #################
