/*
 * Author(s) of this file:
 *   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 *   27.10.2013
 */

/*
 * U_<PotName>       total potential
 * U_SR_<PotName>    short range contribution
 * U_LR_<PotName>    long range contribution
 * U_R_<PotName>     repulsive contributiom
 * U_A_<PotName>     attractive contribution
 * U_Ref_<PotName>   reference contributiom
 * U_Pert_<PotName>  pertubation contribution
 */
typedef double OZ_func_one_t (double, double, double *);

OZ_func_one_t U_Ionic_Microgel;
OZ_func_one_t U_Depletion;
OZ_func_one_t U_Sticky_Hard_Sphere;
OZ_func_one_t U_SR_Sticky_Hard_Sphere;
OZ_func_one_t U_LR_Sticky_Hard_Sphere;
OZ_func_one_t U_R_Sticky_Hard_Sphere;
OZ_func_one_t U_A_Sticky_Hard_Sphere;
OZ_func_one_t U_Ref_Sticky_Hard_Sphere;
OZ_func_one_t U_Pert_Sticky_Hard_Sphere;
OZ_func_one_t U_Soft_Sphere;
OZ_func_one_t U_Hard_Sphere;
OZ_func_one_t U_SR_Hard_Sphere;
OZ_func_one_t U_LR_Hard_Sphere;
OZ_func_one_t U_R_Hard_Sphere;
OZ_func_one_t U_A_Hard_Sphere;
OZ_func_one_t U_Lennard_Jones;
OZ_func_one_t U_SR_Lennard_Jones;
OZ_func_one_t U_LR_Lennard_Jones;
OZ_func_one_t U_GGCM_n;
OZ_func_one_t U_PSM;
OZ_func_one_t U_DLVO;

