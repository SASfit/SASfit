#include <sasfit_common.h>
#define sasfit_common_DLLEXP SASFIT_LIB_IMPORT
#include <sasfit_plugin.h>
#include <tcl.h>
#include <sasfit_plugin_backend.h>
 
// make all form factor functions available
#include <sasfit_fuzzysphere.h>
extern sasfit_plugin_api_get_ver_t get_ver;
extern sasfit_plugin_api_do_init_t do_init;

int main()
{
    scalar result = -1.0;

    // init plugin properly
    sasfit_plugin_api_t plugin_api;
    sasfit_plugin_api_reset(&plugin_api);
    sasfit_plugin_api_set(&plugin_api, 0, &get_ver);
    sasfit_plugin_api_set(&plugin_api, 1, &do_init);
    const sasfit_plugin_info_t * plugin_exp = 0;
    int res = do_init(&plugin_exp, sasfit_common_stubs_ptr(),
	                  &sasfit_plugin_search);
    if ( !res )
    {
        printf("Could not initialize the plugin!");
        return -1;
    }

    sasfit_param param;     // configure parameters
    sasfit_init_param(&param);

    param.p[0] = 2.5;       // R
    param.p[1] = 0.8;       // sigma
    param.p[2] = 1.0;       // eta_sph
    param.p[3] = 0.0;       // eta_sol

    // call the form factor I(q) at multiple q positions
    scalar qvec[] = {0.0, 0.01, 0.1, 1.0, -1.0};

    for(size_t i=0; i < 5; i++)
    {
        param.errStatus = 0;
        result = sasfit_ff_fuzzysphere(qvec[i], &param);
        printf("The result at I(q=%f) = %f\n", qvec[i], result);
        printf(">ERR:%d< %s: %s\n", param.errStatus,
		       param.errStatus ? "True" : "False", param.errStr);
    }
    return 0;
}
