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

OZ_func_one_t U_Sticky_Hard_Sphere;
OZ_func_one_t U_SR_Sticky_Hard_Sphere;
OZ_func_one_t U_LR_Sticky_Hard_Sphere;
OZ_func_one_t U_R_Sticky_Hard_Sphere;
OZ_func_one_t U_A_Sticky_Hard_Sphere;
OZ_func_one_t U_Ref_Sticky_Hard_Sphere;
OZ_func_one_t U_Pert_Sticky_Hard_Sphere;

OZ_func_one_t U_Square_Well_Sphere;
OZ_func_one_t U_SR_Square_Well_Sphere;
OZ_func_one_t U_LR_Square_Well_Sphere;
OZ_func_one_t U_R_Square_Well_Sphere;
OZ_func_one_t U_A_Square_Well_Sphere;
OZ_func_one_t U_Ref_Square_Well_Sphere;
OZ_func_one_t U_Pert_Square_Well_Sphere;
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
OZ_func_one_t U_Ref_DLVO;
OZ_func_one_t U_Pert_DLVO;
OZ_func_one_t U_SR_DLVO;
OZ_func_one_t U_LR_DLVO;
OZ_func_one_t U_R_DLVO;
OZ_func_one_t U_A_DLVO;

OZ_func_one_t U_DLVO_Hydra;
OZ_func_one_t U_Ref_DLVO_Hydra;
OZ_func_one_t U_Pert_DLVO_Hydra;
OZ_func_one_t U_SR_DLVO_Hydra;
OZ_func_one_t U_LR_DLVO_Hydra;
OZ_func_one_t U_R_DLVO_Hydra;
OZ_func_one_t U_A_DLVO_Hydra;

OZ_func_one_t U_ZERO;
OZ_func_one_t U_Star1;
OZ_func_one_t U_Star2;

OZ_func_one_t U_HS_3Yukawa;
OZ_func_one_t U_Ref_HS_3Yukawa;
OZ_func_one_t U_Pert_HS_3Yukawa;
OZ_func_one_t U_SR_HS_3Yukawa;
OZ_func_one_t U_LR_HS_3Yukawa;
OZ_func_one_t U_R_HS_3Yukawa;
OZ_func_one_t U_A_HS_3Yukawa;

OZ_func_one_t U_FDM;

OZ_func_one_t U_Depletion;
OZ_func_one_t U_DepletionOfSpheresBySpheres;
OZ_func_one_t U_R_DepletionOfSpheresBySpheres;
OZ_func_one_t U_A_DepletionOfSpheresBySpheres;
OZ_func_one_t U_SR_DepletionOfSpheresBySpheres;
OZ_func_one_t U_LR_DepletionOfSpheresBySpheres;
OZ_func_one_t U_Ref_DepletionOfSpheresBySpheres;
OZ_func_one_t U_Pert_DepletionOfSpheresBySpheres;

OZ_func_one_t U_DepletionOfSpheresByDiscs;
OZ_func_one_t U_R_DepletionOfSpheresByDiscs;
OZ_func_one_t U_A_DepletionOfSpheresByDiscs;
OZ_func_one_t U_SR_DepletionOfSpheresByDiscs;
OZ_func_one_t U_LR_DepletionOfSpheresByDiscs;
OZ_func_one_t U_Ref_DepletionOfSpheresByDiscs;
OZ_func_one_t U_Pert_DepletionOfSpheresByDiscs;

OZ_func_one_t U_DepletionOfSpheresByRods;
OZ_func_one_t U_R_DepletionOfSpheresByRods;
OZ_func_one_t U_A_DepletionOfSpheresByRods;
OZ_func_one_t U_SR_DepletionOfSpheresByRods;
OZ_func_one_t U_LR_DepletionOfSpheresByRods;
OZ_func_one_t U_Ref_DepletionOfSpheresByRods;
OZ_func_one_t U_Pert_DepletionOfSpheresByRods;

OZ_func_one_t U_Piecewise_Constant_HS;
OZ_func_one_t U_R_Piecewise_Constant_HS;
OZ_func_one_t U_A_Piecewise_Constant_HS;
OZ_func_one_t U_SR_Piecewise_Constant_HS;
OZ_func_one_t U_LR_Piecewise_Constant_HS;
OZ_func_one_t U_Ref_Piecewise_Constant_HS;
OZ_func_one_t U_Pert_Piecewise_Constant_HS;

