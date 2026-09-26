/* TEST-ONLY SHIM - matches the real src/sasfit_common/include/sasfit_error_sq.h */
#ifndef SASFIT_ERROR_SQ_H_SHIM
#define SASFIT_ERROR_SQ_H_SHIM
#ifdef SASFIT_RETURNVAL_ON_ERROR
#undef SASFIT_RETURNVAL_ON_ERROR
#endif
#define SASFIT_RETURNVAL_ON_ERROR 1.0
#ifdef SASFIT_ERR_PREFIX
#undef SASFIT_ERR_PREFIX
#endif
#define SASFIT_ERR_PREFIX "Can not calculate structure factor because "
#endif
