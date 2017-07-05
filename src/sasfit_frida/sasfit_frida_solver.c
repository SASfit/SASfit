/*
 * Author(s) of this file:
 *  by Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
 *   07.01.2017
 */
#include <sasfit_frida.h>

void SASFITfridaGlobal(interp,x,y,res,sig,yfit,ysub,ndata,max_SD,GlobalAP,GCP,error_type,
                        SASFIT_CData,chisq,funcs,alamda,error)
Tcl_Interp *interp;
float **x,**y,**sig,**res,**yfit,**ysub,*chisq,*alamda;
int   *ndata,max_SD,error_type;
bool  *error;
sasfit_analytpar *GlobalAP;
sasfit_commonpar *GCP;
struct sasfit_CData SASFIT_CData[];
void  (*funcs)();
{
}


