#include <math.h>
#include <stdexcept>
#include <vector>
#include <functional>
#include <limits>
#include <bestlime/Bessel_integrator.hpp>
#include <bestlime/Grid_types.hpp>
extern "C"
{
    #include "sasfit_constants.h"
    #include "sasfit_function.h"
    #include "sasfit_message.h"
    #include "sasfit_hankel.h"
}

struct sasfit_bestlime_params {
    double scale;
    double (*function)(double, void *);
    void *fparams;
};

scalar sasfit_bestlime(double nu, double (*f)(double, void *), double x, void *fparams) {
    if (f == NULL || !(x > 0.0) || (nu != 0.0 && nu != 1.0)) {
        return NAN;
    }

    try {
        const bestlime::index_vector n_points {128};
        const bestlime::vector_d z_limits {0.0, bestlime::Grid_1d::infinity};
        const bestlime::vector_d grid_params {1.0};
        bestlime::Bessel_integrator<bestlime::Exp_grid> integrator(
            n_points, z_limits, grid_params, 1.0, true
        );
        sasfit_bestlime_params params {x, f, fparams};
        std::function<double(double, void *)> integrand = [](double z, void *p) {
            const sasfit_bestlime_params *params =
                static_cast<const sasfit_bestlime_params *>(p);
            return z * params->function(z / params->scale, params->fparams)
                / (params->scale * params->scale);
        };
        const bestlime::grid_vector values = integrator.discretize(
            integrand, static_cast<void *>(&params));
        return nu == 0.0
            ? integrator.int_J_nu_minus_1(values, 1.0)
            : integrator.int_J_nu(values, 1.0);
    } catch (const std::exception& error) {
        sasfit_err("sasfit_bestlime: %s\n", error.what());
        return NAN;
    } catch (...) {
        sasfit_err("sasfit_bestlime: unknown exception\n");
        return NAN;
    }
}
