#ifndef RFA_PIECEWISE_H
#define RFA_PIECEWISE_H

#include <complex.h>

#ifdef __cplusplus
extern "C" {
#endif

void rfa_init(void);

typedef struct {
    int     n;
    double  eta;
    double *lambda;
    double *beps;
} PWPotential;

typedef struct {
    double complex s[3];
    double complex Sp[3];
    double A0, B0, S1, S2, S3;
} RFADerived;

typedef struct {
    int         n;
    double      eta;
    double     *lambda;
    double     *beps;
    double     *A;
    double     *B;
    RFADerived  d;
    int         converged;
    int         newton_iters;
} RFASolution;

int rfa1_solve(const PWPotential *pot, RFASolution *sol);
int rfa2_solve(const PWPotential *pot, RFASolution *sol);
int rfa1_solve_warm(const PWPotential *pot, const RFASolution *warm_start, RFASolution *sol);
int rfa2_solve_warm(const PWPotential *pot, const RFASolution *warm_start, RFASolution *sol);
void rfa_solution_free(RFASolution *sol);
double rfa_structure_factor(const RFASolution *sol, double q);
double rfa_compressibility_S0(const RFASolution *sol);
double rfa_rdf_first_shell(const RFASolution *sol, double r);
double rfa_contact_jump(const RFASolution *sol, int j);
int rfa_potential_init(PWPotential *pot, int n, double eta,
                        const double *lambda_steps, const double *beps_steps);
void rfa_potential_free(PWPotential *pot);
void rfa_solution_copy(const RFASolution *src, RFASolution *dst);

#ifdef __cplusplus
}
#endif
#endif /* RFA_PIECEWISE_H */
