#include <stdlib.h>
#include <math.h>
#include "hankel.h"

/* Ogata DE mapping */
static inline double psi(double t) {
    double sh = sinh(t);
    double u  = (M_PI / 2.0) * sh;
    return tanh(u);
}

static inline double dpsi(double t) {
    double ch = cosh(t);
    double sh = sinh(t);
    double u  = (M_PI / 2.0) * sh;
    double cu = cosh(u);
    return (M_PI / 2.0) * ch / (cu * cu);
}

hankel_ogata_table_t *hankel_ogata_table_create(double h, int Nmax)
{
    hankel_ogata_table_t *tab = malloc(sizeof(*tab));
    if (!tab) return NULL;

    tab->h = h;
    tab->Nmax = Nmax;

    int size = 2*Nmax + 1;
    tab->t = malloc(size*sizeof(double));
    tab->x = malloc(size*sizeof(double));
    tab->w = malloc(size*sizeof(double));

    if (!tab->t || !tab->x || !tab->w) {
        free(tab->t); free(tab->x); free(tab->w); free(tab);
        return NULL;
    }

    for (int i = 0; i < size; ++i) {
        int m = i - Nmax;
        double t = m * h;
        double ps = psi(t);
        double dps = dpsi(t);
        tab->t[i] = t;
        tab->x[i] = (M_PI / h) * ps;
        tab->w[i] = dps;
    }

    return tab;
}

void hankel_ogata_table_destroy(hankel_ogata_table_t *tab)
{
    if (!tab) return;
    free(tab->t);
    free(tab->x);
    free(tab->w);
    free(tab);
}
