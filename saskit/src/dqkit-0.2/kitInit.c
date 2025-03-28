/* 
 * tclAppInit.c --
 *
 *  Provides a default version of the main program and Tcl_AppInit
 *  procedure for Tcl applications (without Tk).  Note that this
 *  program must be built in Win32 console mode to work properly.
 *
 * Copyright (c) 1996-1997 by Sun Microsystems, Inc.
 * Copyright (c) 1998-1999 by Scriptics Corporation.
 * Copyright (c) 2000-2002 Jean-Claude Wippler <jcw@equi4.com>
 * Copyright (c) 2003-2004 Wojciech Kocjan <wojciech.kocjan@dq.pl>
 *
 * See the file "license.terms" for information on usage and redistribution
 * of this file, and for a DISCLAIMER OF ALL WARRANTIES.
 *
 */

#ifndef MAC_TCL

#ifdef KIT_INCLUDES_TK
#include <tk.h>
#else
#include <tcl.h>
#endif

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#undef WIN32_LEAN_AND_MEAN
#endif

#else

/* preprocessor hack to allow us to use our own main below */
#define main main_unused
#include "tkMacAppInit.c"
#undef main

#endif

#include "tclInt.h"

%DQKIT_EXTERN_CODE%
Tcl_AppInitProc	Mk4tcl_Init, Vfs_Init, Pwb_Init, Rechan_Init, Zlib_Init;
#ifdef _WIN32
Tcl_AppInitProc	Dde_Init, Registry_Init;
#endif

#ifdef KIT_DEBUG
#define KITDEBUG(str) Tcl_Eval(interp,"puts \"" str "\"");
#else
#define KITDEBUG(str)
#endif

    /*
     *  Attempt to load a "boot.tcl" entry from the embedded MetaKit file.
     *  If there isn't one, try to open a regular "setup.tcl" file instead.
     *  If that fails, this code will throw an error, using a message box.
     */

static char preInitCmd[] = 
#ifdef MAC_TCL
"proc tkInit {} {\n\
global tcl_library tk_library tk_version\n\
rename tkInit {}\n\
set tk_library [file join [file dirname $tcl_library] tk$tk_version]\n\
if {[catch {uplevel #0 [list source [file join $tk_library tk.tcl]]}]} {\n\
  set msg \"Can't find a usable tk.tcl file\"\n\
  append msg \" perhaps you need to install Tk or set your\"\n\
  append msg \" TK_LIBRARY environment variable?\"\n\
  error $msg\n\
  }\n\
}\n"
#endif
"proc tclKitInit {} {\n"
    "rename tclKitInit {}\n"
    "load {} Mk4tcl\n"
#ifdef KIT_USECUSTOMFILE
#ifdef KIT_DEBUG
    "puts \"Opening kit file " KIT_CUSTOMFILE "\"\n"
#endif
    "mk::file open exe \"" KIT_CUSTOMFILE "\" -readonly -shared\n"
    "set ::othernoe \"" KIT_CUSTOMFILE "\"\n"
//#elif KIT_DEBUG
//    "puts \"Opening [info nameofexecutable].kit file\"\n"
//    "mk::file open exe [info nameofexecutable].kit -readonly -shared\n"
#else
    "mk::file open exe [info nameofexecutable] -readonly -shared\n"
#endif
#ifdef KIT_DEBUG
    "set ::tclInitDebug 1\n"
    "puts \"Doing select\"\n"
#endif
    "set n [mk::select exe.dirs!0.files name boot.tcl]\n"
    "if {$n == \"\"} {\n"
        "mk::file close exe\n"
        "puts \"Opening [info nameofexecutable].kit file\"\n"
        "mk::file open exe [info nameofexecutable].kit -readonly -shared\n"
        "set n [mk::select exe.dirs!0.files name boot.tcl]\n"
    "}\n"
    "if {$n != \"\"} {\n"
        "set s [mk::get exe.dirs!0.files!$n contents]\n"
	"if {![string length $s]} { error \"empty boot.tcl\" }\n"
        "load {} zlib\n"
        "if {[mk::get exe.dirs!0.files!$n size] != [string length $s]} {\n"
	    "set s [zlib decompress $s]\n"
	"}\n"
    "} else {\n"
        "set f [open setup.tcl]\n"
        "set s [read $f]\n"
        "close $f\n"
    "}\n"
#ifdef KIT_DEBUG
    "puts \"Evaluating script\"\n"
#endif
    "uplevel #0 $s\n"
#ifdef _WIN32
    "package ifneeded dde 1.2.3 {load {} dde}\n"
    "package ifneeded registry 1.1.3 {load {} registry}\n"
#endif
"}\n"
"tclKitInit"
;
static char preInitCmd2[] = 
"load {} Vfs\n"
"load {} rechan\n"
"namespace eval ::vfs {}\n"
"proc ::vfs::_dummy_rechan_handler {args} {}\n"
"if {[llength [file channels]]==0} {\n"
"  rechan ::vfs::_dummy_rechan_handler 6\n"
"  rechan ::vfs::_dummy_rechan_handler 6\n"
"  rechan ::vfs::_dummy_rechan_handler 6\n"
"}\n"
"proc tclInit {} {\n"
"source [file join [info nameofexecutable] bootInit.tcl]\n"
"rename tclInit {}\n"
"}\n"
;

static const char initScript[] =
#ifdef MAC_TCL
"namespace eval ::tk::mac {\n\
proc OpenDocument {args} {\n\
    set v {}\n\
    foreach a $args {\n\
	lappend v [list source $a]\n\
    }\n\
    after idle [join $v \n]\n\
}}\n"
#endif
"if {[file isfile [file join [info nameofexe] main.tcl]]} {\n"
    "if {[info commands console] != {}} { console hide }\n"
    "set tcl_interactive 0\n"
    "incr argc\n"
    "set argv [linsert $argv 0 $argv0]\n"
    "set argv0 [file join [info nameofexe] main.tcl]\n"
"} else continue\n"
;

int 
TclKit_AppInit(Tcl_Interp *interp)
{
    char *oldCmd;
    KITDEBUG("Initializing static packages")
%DQKIT_INIT_CODE%
    Tcl_StaticPackage(0, "Mk4tcl", Mk4tcl_Init, NULL);
    Tcl_StaticPackage(0, "dqkitpwb", Pwb_Init, NULL);
    Tcl_StaticPackage(0, "rechan", Rechan_Init, NULL);
    Tcl_StaticPackage(0, "vfs", Vfs_Init, NULL);
    Tcl_StaticPackage(0, "zlib", Zlib_Init, NULL);
#ifdef _WIN32
    Tcl_StaticPackage(0, "dde", Dde_Init, NULL);
    Tcl_StaticPackage(0, "registry", Registry_Init, NULL);
#endif
#ifdef KIT_INCLUDES_TK
    Tcl_StaticPackage(0, "Tk", Tk_Init, Tk_SafeInit);
#endif

    /* the tcl_rcFileName variable only exists in the initial interpreter */
#ifdef _WIN32
    Tcl_SetVar(interp, "tcl_rcFileName", "~/tclkitrc.tcl", TCL_GLOBAL_ONLY);
#else
    Tcl_SetVar(interp, "tcl_rcFileName", "~/.tclkitrc", TCL_GLOBAL_ONLY);
#ifdef MAC_TCL
    Tcl_SetVar(interp, "tcl_rcRsrcName", "tclkitrc", TCL_GLOBAL_ONLY);
#endif
#endif

    KITDEBUG("TclSetPreInitScript()")
    oldCmd = TclSetPreInitScript(preInitCmd);
    KITDEBUG("Tcl_Init()")
    if (Tcl_Init(interp) == TCL_ERROR)
        goto error;
    KITDEBUG("Tcl_Init2()")
    TclSetPreInitScript(preInitCmd2);

#ifdef KIT_INCLUDES_TK
    KITDEBUG("Initializing Tk")
#if defined(_WIN32) || defined(MAC_TCL)
    if (Tk_Init(interp) == TCL_ERROR)
        goto error;
#ifdef _WIN32
    KITDEBUG("Initializing Tk console window")
    if (Tk_CreateConsoleWindow(interp) == TCL_ERROR)
        goto error;
#else
    KITDEBUG("Setting up main Tcl interp")
    SetupMainInterp(interp);
#endif
#endif
#endif

    KITDEBUG("Tcl_Eval(initScript)")
      /* messy because TclSetStartupScriptPath is called slightly too late */
    if (Tcl_Eval(interp, initScript) == TCL_OK) {
        Tcl_Obj* path = TclGetStartupScriptPath();
	TclSetStartupScriptPath(Tcl_GetObjResult(interp));
	if (path == NULL)
	  Tcl_Eval(interp, "incr argc -1; set argv [lrange $argv 1 end]");
    }

    KITDEBUG("returning")
    Tcl_SetVar(interp, "errorInfo", "", TCL_GLOBAL_ONLY);
    Tcl_ResetResult(interp);
    return TCL_OK;

error:
#ifdef KIT_INCLUDES_TK
#ifdef _WIN32
    MessageBeep(MB_ICONEXCLAMATION);
    MessageBox(NULL, Tcl_GetStringResult(interp), "Error in TclKit",
        MB_ICONSTOP | MB_OK | MB_TASKMODAL | MB_SETFOREGROUND);
    ExitProcess(1);
    /* we won't reach this, but we need the return */
#endif
#endif
    return TCL_ERROR;
}

#ifdef MAC_TCL
#ifndef DQKIT_BUILDAR
void
main( int argc, char **argv)
{
    char *newArgv[2];
	
    if (MacintoshInit()  != TCL_OK) {
    Tcl_Exit(1);
    }

    argc = 1;
    newArgv[0] = "TclKit";
    newArgv[1] = NULL;
    
    Tcl_FindExecutable(newArgv[0]);
	
    Tk_Main(argc, newArgv, TclKit_AppInit);
}
#endif
#endif
