// for printf
#include <stdio.h>

// for SASFIT_LIB_IMPORT
#include <sasfit_common.h>
 
// we want to import from sasfit-ff shared library
#define sasfit_set_msg_handler_DLLEXP SASFIT_LIB_IMPORT
 
// make all form factor functions available
#include <sasfit_fuzzysphere.h>
 
 		
void myprint(const char * format, va_list argptr)
{
	
	printf("beginMyprint\n");
	printf("endMyprint\n=================\n");
}

int main()
{
        scalar result = -1.0;
 
		
        sasfit_param param;     // configure parameters
		
sasfit_set_msg_handler(CHAN_INFO, 1, myprint);
sasfit_set_msg_handler(CHAN_ERR, 1, myprint);
		

        param.p[0] = 2.5;       // R
        param.p[1] = 0.8;       // sigma
        param.p[2] = 1.0;       // eta_sph
        param.p[3] = 0.0;       // eta_sol
		strcpy(param.errStr, "Hallo\n");
		
        // call the form factor for I(q) at q = 0.5
	
		param.errStatus = 0;
		strcpy(param.errStr, "HalloStat. I am the initial string!\n");
        result = sasfit_ff_fuzzysphere(0.00, &param);
        printf("The result at q=0.00 is %f\n", result);
		printf(">ERR:%d< %s: %s\n",param.errStatus, param.errStatus ? "True" : "False", param.errStr);
		
		param.errStatus = 0;
		strcpy(param.errStr, "HalloStat. I am the initial string!\n");
        result = sasfit_ff_fuzzysphere(0.01, &param);
        printf("The result at q=0.01 is %f\n", result);
		printf(">ERR:%d< %s: %s\n",param.errStatus, param.errStatus ? "True" : "False", param.errStr);
		
		param.errStatus = 0;
		strcpy(param.errStr, "HalloStat. I am the initial string!\n");
        result = sasfit_ff_fuzzysphere(0.10, &param);
        printf("The result at q=0.10 is %f\n", result);
		printf(">ERR:%d< %s: %s\n",param.errStatus, param.errStatus ? "True" : "False", param.errStr);
		
		param.errStatus = 0;
		strcpy(param.errStr, "HalloStat. I am the initial string!\n");
        result = sasfit_ff_fuzzysphere(1.00, &param);
        printf("The result at q=1.00 is %f\n", result);
		printf(">ERR:%d< %s: %s\n",param.errStatus, param.errStatus ? "True" : "False", param.errStr);
		
		param.errStatus = 0;
		strcpy(param.errStr, "HalloStat. I am the initial string!\n");
        result = sasfit_ff_fuzzysphere(-1.00, &param);
        printf("The result at q=-1.00 is %f\n", result);
		printf(">ERR:%d< %s: %s\n",param.errStatus, param.errStatus ? "True" : "False", param.errStr);
		
		param.errStatus = 0;
		strcpy(param.errStr, "HalloStat. I am the initial string!\n");
		result = sasfit_ff_fuzzysphere(1.00, &param);
        printf("The result at q=1.00 is %f\n", result);
		printf(">ERR:%d< %s: %s\n",param.errStatus, param.errStatus ? "True" : "False", param.errStr);
        return 0;
}