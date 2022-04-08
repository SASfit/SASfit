// test.c
// Example of a standalone program employing a precompiled SASfit plugin.

#include <stdio.h> // for printf
#include <string.h> // for strcpy

// for SASFIT_LIB_IMPORT
#include <sasfit_common.h>
#define sasfit_common_DLLEXP SASFIT_LIB_IMPORT

#include <sasfit_plugin.h>
#include <tcl.h>
#include <sasfit_plugin_backend.h>
 
// make all form factor functions available
#include <sasfit_fuzzysphere.h>
extern sasfit_plugin_api_get_ver_t get_ver;
extern sasfit_plugin_api_do_init_t do_init;
         
void myprint(const char * format, va_list argptr)
{
    printf(" ++ beginMyPrint ++\n");
    vprintf(format, argptr);
    printf(" ++ endMyPrint ++\n");
}

int main()
{
    scalar result = -1.0;

    // init plugin properly
    sasfit_plugin_api_t plugin_api;
    sasfit_plugin_api_reset(&plugin_api);
    sasfit_plugin_api_set(&plugin_api, 0, &get_ver);
    sasfit_plugin_api_set(&plugin_api, 1, &do_init);
    const sasfit_plugin_info_t * plugin_exp = 0;
    int res = do_init(&plugin_exp, sasfit_common_stubs_ptr(), &sasfit_plugin_search);
    if ( !res )
    {
        printf("Could not initialize the plugin!");
        return -1;
    }

    sasfit_param param;     // configure parameters
    sasfit_init_param(&param);

    // optional
    sasfit_set_msg_handler(CHAN_INFO, 0, myprint);
    sasfit_set_msg_handler(CHAN_ERR, 0, myprint);

    param.p[0] = 2.5;       // R
    param.p[1] = 0.8;       // sigma
    param.p[2] = 1.0;       // eta_sph
    param.p[3] = 0.0;       // eta_sol
//    strcpy(param.errStr, "Hallo\n");

    // call the form factor I(q) at multiple q positions
    scalar qvec[] = {0.0, 0.01, 0.1, 1.0, -1.0};

    for(size_t i=0; i < 5; i++)
    {
        printf("0\n");
        param.errStatus = 0;
        printf("1\n");
//        strcpy(param.errStr, "Hello. I am the initial err string!\n");
        printf("2\n");
        result = sasfit_ff_fuzzysphere(qvec[i], &param);
        printf("3\n");
        printf("The result at I(q=%f) = %f\n", qvec[i], result);
        printf("4\n");
        printf(">ERR:%d< %s: %s\n", param.errStatus, param.errStatus ? "True" : "False", param.errStr);
        printf("5\n");
    }

    return 0;
}
