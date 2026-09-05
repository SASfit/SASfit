#include "f2c.h"
#include "signal1.h"
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif

 ftnint
#ifdef KR_headers
signal_(sigp, proc) integer *sigp; sig_pf proc;
#else
signal_(integer *sigp, sig_pf proc)
#endif
{
	int sig;
	sig = (int)*sigp;

	/* cast through intptr_t: signal() returns a function pointer, and on
	   64-bit platforms this is wider than ftnint. The narrowing to ftnint
	   here is intentional (matches historical f2c/Fortran ISIGNAL
	   behavior of returning the old handler as an integer). */
	return (ftnint)(intptr_t)signal(sig, proc);
	}
#ifdef __cplusplus
}
#endif
