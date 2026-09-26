/* TEST-ONLY SHIM - not the real SASfit header. Mirrors the real
 * signatures/values confirmed from src/sasfit_common/include/. */
#ifndef SASFIT_COMMON_H_SHIM
#define SASFIT_COMMON_H_SHIM
#include <stdio.h>
#include <stdarg.h>
typedef double scalar;
#define MAXPAR 50
typedef struct { double p[MAXPAR]; } sasfit_param;
#define DBGINFO(text) "%s:%d: " text ,__FILE__,__LINE__
#ifndef SASFIT_ERR_PREFIX
#define SASFIT_ERR_PREFIX "Error: "
#endif
#ifndef SASFIT_RETURNVAL_ON_ERROR
#define SASFIT_RETURNVAL_ON_ERROR 0.0
#endif
static void sasfit_param_set_err(sasfit_param *param, const char *fmt, ...)
{ char b[512]; va_list a; va_start(a,fmt); vsnprintf(b,sizeof(b),fmt,a); va_end(a);
  (void)param; fprintf(stderr,"[sasfit_param_set_err] %s", b); }
#define SASFIT_ASSERT_PTR(x) do { (void)(x); } while (0)
#define SASFIT_CHECK_COND1(cond,param,fmt,val) do { if (cond) { \
    sasfit_param_set_err(param, DBGINFO(SASFIT_ERR_PREFIX fmt " !\n"), (val)); \
    return SASFIT_RETURNVAL_ON_ERROR; } } while (0)
#endif
