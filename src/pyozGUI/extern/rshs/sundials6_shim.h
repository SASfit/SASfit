/* Compatibility shim: robertus_shs_core.c targets SUNDIALS 7.x, this
 * container has 6.4.1. Only the error-handler API differs; the solver API
 * (SUNContext, KINSol, SPGMR) is the same from v6 on. Injected with
 * -include so the plugin source itself is used completely unmodified.
 *   - SUNErrCode                : v7 typedef, v6 uses plain int
 *   - SUN_COMM_NULL             : v7 macro; v6 SUNContext_Create takes void* comm
 *   - SUNContext_{Clear,Push}Err: v7-only; they merely silence KINSOL's own
 *                                 warning printouts, so no-ops here are
 *                                 semantically harmless.                    */
#ifndef RSHS_SUNDIALS6_SHIM_H
#define RSHS_SUNDIALS6_SHIM_H
#include <sundials/sundials_types.h>
typedef int SUNErrCode;
#define SUN_COMM_NULL NULL
#define SUNContext_ClearErrHandlers(ctx)        ((void)(ctx))
#define SUNContext_PushErrHandler(ctx, fn, d)   ((void)(ctx), (void)(fn), (void)(d))
#endif
