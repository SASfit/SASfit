/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 */

double sasfit_KullbackLeiblerDivergency(double *, double *, int);
double sasfit_JensenShannonDivergency  (double *, double *, int);
double sasfit_RenyiDivergency          (double *, double *, int, double);
double sasfit_chi2Divergency           (double *, double *, int);
double sasfit_HellingerDistance        (double *, double *, int);
double sasfit_SquaredHellingerDistance (double *, double *, int);
double sasfit_Bhattacharyya_Distance   (double *, double *, int);
double sasfit_Itakura_Saito_Distance   (double *, double *, int);
double sasfit_HellingerSquaredDistance (double *, double *, int);
double sasfit_WassersteinDistance      (double *, double *, int);
