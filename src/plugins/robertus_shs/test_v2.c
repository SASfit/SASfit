#include <stdio.h>
#include "sasfit_common.h"
#include "include/sasfit_robertus_shs.h"

int main(void)
{
    sasfit_param cs = {0};
    cs.p[0]=10.0; cs.p[1]=0.10; cs.p[2]=0.20; cs.p[3]=0.15; cs.p[4]=1; cs.p[5]=11;
    cs.p[6]=0.5; cs.p[7]=400.0; cs.p[8]=300.0; cs.p[9]=334.0;

    printf("q       ff_RobertusSHS_CoreShell(q)\n");
    for (double q = 0.02; q < 1.2; q += 0.1)
        printf("%5.3f   %16.4f\n", q, sasfit_ff_RobertusSHS_CoreShell(q, &cs));

    sasfit_param fz = {0};
    fz.p[0]=10.0; fz.p[1]=0.10; fz.p[2]=0.20; fz.p[3]=0.15; fz.p[4]=2; fz.p[5]=11;
    fz.p[6]=0.3; fz.p[7]=400.0; fz.p[8]=334.0;

    printf("\nq       ff_RobertusSHS_Fuzzy(q)\n");
    for (double q = 0.02; q < 1.2; q += 0.1)
        printf("%5.3f   %16.4f\n", q, sasfit_ff_RobertusSHS_Fuzzy(q, &fz));

    sasfit_param gd = {0};
    gd.p[0]=12.0; gd.p[1]=0.15; gd.p[2]=0.25; gd.p[3]=0.4; gd.p[4]=13; gd.p[5]=1.3;
    printf("\nq       ff_RobertusSHS_GenericDemo(q)\n");
    for (double q = 0.02; q < 1.2; q += 0.1)
        printf("%5.3f   %16.4f\n", q, sasfit_ff_RobertusSHS_GenericDemo(q, &gd));

    printf("\n_f/_v stubs: CoreShell_f=%.3f  CoreShell_v=%.3f (should both be 0.0)\n",
           sasfit_ff_RobertusSHS_CoreShell_f(0.5, &cs),
           sasfit_ff_RobertusSHS_CoreShell_v(0.5, &cs, 1));

    return 0;
}
