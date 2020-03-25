#!/bin/bash

if [ "x$TCLSH" = "x" ]; then
    TCLSH=tclsh
fi

SYSTEM_NAME="$(uname -s)";
SYSTEM_ARCH="$(uname -m)";

export CFLAGS="" # make sure it exists, later scripts reference it
if [ "$SYSTEM_NAME" = "Darwin" ]; then
    SYSTEM_ARCH="x86_64"
    # make sure XQuartz is installed, e.g. 'brew cask install xquartz'
    export CFLAGS="-arch $SYSTEM_ARCH -L/usr/X11/lib -I/usr/X11/include"
    # find optionally installed gcc package if not already defined
    if [ ! -f "$CC" ] || [ ! -f "$CXX" ]; then
        CC=$(which $(echo $PATH | tr ':' '\n' | xargs -n 1 ls -1 | egrep '^gcc-(mp-)?[0-9]+' | head -n1))
        CXX=$(which $(echo $PATH | tr ':' '\n' | xargs -n 1 ls -1 | egrep '^g\+\+-(mp-)?[0-9]+' | head -n1))
        [ -f "$CC" ] && export CC
        [ -f "$CXX" ] && export CXX
    fi
fi;
if uname -s | grep -q '^MINGW' ; then # do not replace backslashes in paths
    export CYGPATH=echo
fi
if [ -n "$CFLAGS" ]; then
    export LDFLAGS="$LDFLAGS $CFLAGS"
    export CPPFLAGS="$CPPFLAGS $CFLAGS"
fi;

if [ "x$TCLVERSION" = "x" ]; then
    TCLPATTERN='*.*'
else
    TCLPATTERN=$TCLVERSION
fi

if [ "x$NOUPX" = "x" ]; then
    UPX=best
else
    UPX=none
fi

if [ "x$NOEXPECT" = "x" ]; then
    EXPECT="-expect"
else
    EXPECT=""
fi

if [ "x$NOTCLX" = "x" ]; then
    TCLX="-tclx"
else
    TCLX=""
fi

if [ "x$NOTCLUDP" = "x" ]; then
    TCLUDP="-tcludp"
else
    TCLUDP=""
fi

if [ "x$THREADEDX11" = "x" ]; then
    TX="-no-threadedx11"
else
    TX="-threadedx11"
fi

if [ "x$HASMYSQLTCL" = "x" ]; then
    MYSQLTCL=""
else
    MYSQLTCL="-mysqltcl"
fi

if [ "x$HASPGTCL" = "x" ]; then
    PGTCL=""
else
    PGTCL="-pgtcl"
fi

# threaded
#$TCLSH gen/buildkit build -tcl=$TCLPATTERN -itcl -thread -sqlite -sqlite3 -tclcompiler -tbcload -tls $TCLX $EXPECT -tk=$TCLPATTERN -tile -itk -iwidgets -blt -tktable -tdom -tktreectrl $TCLUDP $MYSQLTCL $PGTCL -compression=$UPX -binary=all -resfile=current || exit 1
#$TCLSH gen/buildkit linkfinal -tcl=$TCLPATTERN -tk=$TCLPATTERN -tile -thread -sqlite -sqlite3 -tbcload -tls $TCLX $EXPECT -compression=$UPX -binary=gui -resfile=current || exit 1
#$TCLSH gen/buildkit compile res -thread -resfile=current || exit 1
#$TCLSH gen/buildkit linkfinal -tcl=$TCLPATTERN -itcl -thread -sqlite -sqlite3 -tclcompiler -tbcload -tls $TCLX $EXPECT -tdom -compression=$UPX -binary=console -resfile=current || exit 1
#$TCLSH gen/buildkit linkfinal -tcl=$TCLPATTERN -thread -compression=$UPX -binary=tiny -resfile=current || exit 1

TCLPATTERN=8.4*
# nonthreaded
$TCLSH gen/buildkit build -tcl=$TCLPATTERN -tk=$TCLPATTERN -blt -compression=none -binary=all -resfile=current || exit 1
#$TCLSH gen/buildkit linkfinal -tcl=$TCLPATTERN -tk=$TCLPATTERN -tile -sqlite -sqlite3 -tbcload -tls $TCLX $EXPECT -compression=$UPX -binary=gui $TX -resfile=current || exit 1
#$TCLSH gen/buildkit compile res -resfile=current || exit 1
#$TCLSH gen/buildkit linkfinal -tcl=$TCLPATTERN -itcl -sqlite -sqlite3 -tclcompiler -tbcload -tls $TCLX $EXPECT -tdom $TCLUDP $PGTCL -compression=$UPX -binary=console -resfile=current || exit 1
#$TCLSH gen/buildkit linkfinal -tcl=$TCLPATTERN -itcl -itk -tk=$TCLPATTERN -blt -compression=none -binary=tiny -resfile=current || exit 1

# threaded debug
#$TCLSH gen/buildkit compile res -thread -debug -resfile=current || exit 1
#$TCLSH gen/buildkit build -debug -tcl=$TCLPATTERN -itcl -thread -sqlite -sqlite3 -tbcload -tls $TCLX $EXPECT -tk=$TCLPATTERN -tile -itk -iwidgets -blt -tktable -tdom -tktreectrl $TCLUDP $PGTCL -compression=$UPX -binary=all -resfile=current || exit 1

