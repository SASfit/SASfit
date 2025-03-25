#ifndef HEADER_E7322657D9F13B16
#define HEADER_E7322657D9F13B16

scalar sasfit_GaussLobattoInt(sasfit_func_one_void f,
            scalar a, scalar b,
            scalar abstol,
            scalar reltol,
            size_t maxeval,
			scalar *error,
            void *param);

scalar GaussLobattoInt(sasfit_func_one_void f,
            scalar a, scalar b,
            scalar abstol,
            scalar reltol,
            size_t maxeval,
			scalar *error,
            void *param);



// Adaptive Lobatto Quadrature Parameters
typedef struct {
    double atol;
    double rtol;
    int parts;
    int maxFcnEvals;
    int maxParts;
} LobattoParams;

double adaptiveLobatto(sasfit_func_one_void f,
                       double a, double b,
                       LobattoParams params,
                       int *fcnEvals,
                       int *iter,
                       void *param);
#endif // header guard

