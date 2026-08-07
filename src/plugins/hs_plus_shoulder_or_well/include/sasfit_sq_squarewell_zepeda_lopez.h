/* squarewell_zepeda_lopez.h
 * Zepeda-Lopez, Torres-Carbajal, Ramirez-Gonzalez, Medina-Noyola (2019),
 * arXiv:1906.09894, eq. (3.9): full PY hard-sphere reference
 * (Verlet-Weiss corrected) plus the exact nonlinear Boltzmann
 * perturbation term for the square well -- the ORPA closed form.
 *
 * Convention: eps_kT>0 = attractive well depth in units of kT;
 * eps_kT<0 for a repulsive square shoulder.
 */
#ifndef SQUAREWELL_ZEPEDA_LOPEZ_H
#define SQUAREWELL_ZEPEDA_LOPEZ_H

/* S(Q): sigma = hard-sphere diameter, eta = volume fraction,
 * eps_kT = well depth eps/kB/T, lam = well range (well extends to
 * r=lam*sigma). */
double sw_zepeda_lopez_S(double Q, double sigma, double eta, double eps_kT, double lam);

#endif
