#ifndef UUTIL_H
#define UUITL_H 1

#include <tcl.h>

void uu_mount_setup(Tcl_Interp *);
void uu_loop_setup(Tcl_Interp *);
void uu_execv_setup(Tcl_Interp *);

#endif

