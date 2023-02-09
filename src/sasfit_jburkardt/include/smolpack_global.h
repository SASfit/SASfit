#ifndef SASFIT_SMOLPACK_GLOBAL_H
#define SASFIT_SMOLPACK_GLOBAL_H

#define SP_uniw 1025    /* total # of nodes of quadrature formulae     */
#define SP_fn 12        /* # of different basic formulas                */
#define SP_gesfn 50     /* # of basic formulae  (incl. multiplicities) */

struct SPstruct {
    double* cubmin;
    double* cubmax;
    double x_ab[maxdim];
    double quafo;                         /* cubature result             */
    double x[maxdim];                     /* function argument           */
    double xnu[SP_fn][SP_uniw];
    double dnu[SP_fn][SP_uniw];   /* Delta-parameter             */
    double ftotal;
    double wsum;
    double summe;
    size_t d;
    int q;
    void *cubparam;                            /* cubature formula parameter  */
    int ninv[SP_fn];
    int sw[SP_gesfn];       /* working var's */
    int count;
    int wcount;       /* counter of f-calls and coefficient calls */

    int indices[maxdim];
    int argind[maxdim]; /* formula and nodal indices    */
    int indsum[maxdim][maxdim];     /* parameter for 'divide et conquer' */
    int n[SP_fn];
    int anzw[SP_uniw];
    int nodes[maxdim];
    int lookind[SP_fn][SP_uniw];
    int invlook[SP_fn][SP_uniw];
    int maxind;          /* tree parameter              */
    int wind[maxdim];      /* Parameter for slow coefficient calculation */
    double (*f) (double*, size_t, void *);
};

struct SPstruct SP;

                    /* function argument           */


#endif  //file
