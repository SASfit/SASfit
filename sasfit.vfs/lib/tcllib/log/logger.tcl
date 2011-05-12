# logger.tcl --
#
#   Tcl implementation of a general logging facility.
#
# Copyright (c) 2003 by David N. Welton <davidw@dedasys.com>
# Copyright (c) 2004,2005 by Michael Schlenker <mic42@users.sourceforge.net>
#
# See the file license.terms.

# The logger package provides an 'object oriented' log facility that
# lets you have trees of services, that inherit from one another.
# This is accomplished through the use of Tcl namespaces.


package require Tcl 8.2
package provide logger 0.6.1

namespace eval ::logger {
    namespace eval tree {}
    namespace export init enable disable services servicecmd import

    # The active services.
    variable services {}

    # The log 'levels'.
    variable levels [list debug info notice warn error critical]
}

# ::logger::_nsExists --
#
#   Workaround for missing namespace exists in Tcl 8.2 and 8.3.
#

if {[package vcompare [package provide Tcl] 8.4] < 0} {
    proc ::logger::_nsExists {ns} {
        expr {![catch {namespace parent $ns}]}
    }
} else {
    proc ::logger::_nsExists {ns} {
        namespace exists $ns
    }
}

# ::logger::_cmdPrefixExists --
#
# Utility function to check if a given callback prefix exists,
# this should catch all oddities in prefix names, including spaces, 
# glob patterns, non normalized namespaces etc.
#
# Arguments:
#   prefix - The command prefix to check
#   
# Results:
#   1 or 0 for yes or no
#
proc ::logger::_cmdPrefixExists {prefix} {
    set cmd [lindex $prefix 0]
    set full [namespace eval :: namespace which [list $cmd]]
    if {[string equal $full ""]} {return 0} else {return 1}
    # normalize namespaces
    set ns [namespace qualifiers $cmd]
    set cmd ${ns}::[namespace tail $cmd]
    set matches [::info command ${ns}::*]
    if {[lsearch -exact $matches $cmd] != -1} {return 1}
    return 0
}

# ::logger::walk --
#
#   Walk namespaces, starting in 'start', and evaluate 'code' in
#   them.
#
# Arguments:
#   start - namespace to start in.
#   code - code to execute in namespaces walked.
#
# Side Effects:
#   Side effects of code executed.
#
# Results:
#   None.

proc ::logger::walk { start code } {
    set children [namespace children $start]
    foreach c $children {
    logger::walk $c $code
    namespace eval $c $code
    }
}

proc ::logger::init {service} {
    variable levels
    variable services
        
    # We create a 'tree' namespace to house all the services, so
    # they are in a 'safe' namespace sandbox, and won't overwrite
    # any commands.
    namespace eval tree::${service} {
        variable service
        variable levels
        variable oldname 
    }

    lappend services $service

    set [namespace current]::tree::${service}::service $service
    set [namespace current]::tree::${service}::levels $levels
    set [namespace current]::tree::${service}::oldname $service
    
    namespace eval tree::${service} {
    # Defaults to 'debug' level - show everything.  I don't
    # want people to wonder where there debug messages are
    # going.  They can turn it off themselves.
    variable enabled "debug"

    # Callback to use when the service in question is shut down.
    variable delcallback [namespace current]::no-op

    # Callback when the loglevel is changed
    variable levelchangecallback [namespace current]::no-op
    
    # State variable to decide when to call levelcallback
    variable inSetLevel 0
    
    # The currently configured levelcommands
    variable lvlcmds 
    array set lvlcmds {}
    # We use this to disable a service completely.  In Tcl 8.4
    # or greater, by using this, disabled log calls are a
    # no-op!

    proc no-op args {}


    proc stdoutcmd {level text} {
        variable service
        puts "\[[clock format [clock seconds]]\] \[$service\] \[$level\] \'$text\'"
    }

    proc stderrcmd {level text} {
        variable service
        puts stderr "\[[clock format [clock seconds]]\] \[$service\] \[$level\] \'$text\'"
    }


    # setlevel --
    #
    #   This command differs from enable and disable in that
    #   it disables all the levels below that selected, and
    #   then enables all levels above it, which enable/disable
    #   do not do.
    #
    # Arguments:
    #   lv - the level, as defined in $levels.
    #
    # Side Effects:
    #   Runs disable for the level, and then enable, in order
    #   to ensure that all levels are set correctly.
    #
    # Results:
    #   None.


    proc setlevel {lv} {
        variable inSetLevel 1
        set oldlvl [currentloglevel]
        
        # do not allow enable and disable to do recursion
        if {[catch {
            disable $lv 0
            set newlvl [enable $lv 0]
        } msg] == 1} {
            return -code error -errorcode $::errorCode $msg
        }
        # do the recursion here
        logger::walk [namespace current] [list setlevel $lv]
        
        set inSetLevel 0
        lvlchangewrapper $oldlvl $newlvl
        return
    }

    # enable --
    #
    #   Enable a particular 'level', and above, for the
    #   service, and its 'children'.
    #
    # Arguments:
    #   lv - the level, as defined in $levels.
    #
    # Side Effects:
    #   Enables logging for the particular level, and all
    #   above it (those more important).  It also walks
    #   through all services that are 'children' and enables
    #   them at the same level or above.
    #
    # Results:
    #   None.

    proc enable {lv {recursion 1}} {
        variable levels
        set lvnum [lsearch -exact $levels $lv]
        if { $lvnum == -1 } {
        return -code error "Invalid level '$lv' - levels are $levels"
        }

        variable enabled
        set newlevel $enabled
        set elnum [lsearch -exact $levels $enabled]
        if {($elnum == -1) || ($elnum > $lvnum)} {
            set newlevel $lv
        }
                
        variable service
        while { $lvnum <  [llength $levels] } {
        interp alias {} [namespace current]::[lindex $levels $lvnum] \
            {} [namespace current]::[lindex $levels $lvnum]cmd
        incr lvnum
        }
        
        if {$recursion} {
            logger::walk [namespace current] [list enable $lv]
        }
        lvlchangewrapper $enabled $newlevel
        set enabled $newlevel
    }

    # disable --
    #
    #   Disable a particular 'level', and below, for the
    #   service, and its 'children'.
    #
    # Arguments:
    #   lv - the level, as defined in $levels.
    #
    # Side Effects:
    #   Disables logging for the particular level, and all
    #   below it (those less important).  It also walks
    #   through all services that are 'children' and disables
    #   them at the same level or below.
    #
    # Results:
    #   None.

    proc disable {lv {recursion 1}} {
        variable levels
        set lvnum [lsearch -exact $levels $lv]
        if { $lvnum == -1 } {
        return -code error "Invalid level '$lv' - levels are $levels"
        }

        variable enabled
        set newlevel $enabled
        set elnum [lsearch -exact $levels $enabled]
        if {($elnum > -1) && ($elnum <= $lvnum)} {
            if {$lvnum+1 >= [llength $levels]} {
                set newlevel "none"
            } else {
                set newlevel [lindex $levels [expr {$lvnum+1}]]
            }
        }
        
        while { $lvnum >= 0 } {
        
        interp alias {} [namespace current]::[lindex $levels $lvnum] {} \
            [namespace current]::no-op
        incr lvnum -1
        }
        if {$recursion} {
            logger::walk [namespace current] [list disable $lv]
        }
        lvlchangewrapper $enabled $newlevel
        set enabled $newlevel
    }

    # currentloglevel --
    #
    #   Get the currently enabled log level for this service.
    #
    # Arguments:
    #   none
    #
    # Side Effects:
    #   none
    #
    # Results:
    #   current log level
    #

    proc currentloglevel {} {
        variable enabled
        return $enabled
    }

    # lvlchangeproc --
    #
    #   Set or introspect a callback for when the logger instance 
    #   changes its loglevel.
    #
    # Arguments:
    #   cmd - the Tcl command to call, it is called with two parameters, old and new log level.
    #   or none for introspection
    #
    # Side Effects:
    #   None.
    #
    # Results:
    #   If no arguments are given return the current callback cmd.

    proc lvlchangeproc {args} {
        variable levelchangecallback
        
        switch -exact -- [llength [::info level 0]] {
                1   {return $levelchangecallback}
                2   {
                     if {[::logger::_cmdPrefixExists [lindex $args 0]]} {
                        set levelchangecallback [lindex $args 0]
                     } else {
                        return -code error "Invalid cmd '[lindex $args 0]' - does not exist"
                     }    
                    }
                default {
                    return -code error "Wrong # of arguments. Usage: \${log}::lvlchangeproc ?cmd?"
                }
        }
    }

    proc lvlchangewrapper {old new} {
        variable inSetLevel
        
        # we are called after disable and enable are finished 
        if {$inSetLevel} {return}
        
        # no action if level does not change
        if {[string equal $old $new]} {return}
        
        variable levelchangecallback
        # no action if levelchangecallback isn't a valid command
        if {[::logger::_cmdPrefixExists $levelchangecallback]} {
        catch {
            uplevel \#0 [linsert $levelchangecallback end $old $new]
        }
        }
    }
    
    # logproc --
    #
    #   Command used to create a procedure that is executed to
    #   perform the logging.  This could write to disk, out to
    #   the network, or something else.
    #   If two arguments are given, use an existing command.
    #   If three arguments are given, create a proc.
    #
    # Arguments:
    #   lv - the level to log, which must be one of $levels.
    #   args - either zero, one or two arguments.
    #          if zero this returns the current command registered 
    #          if one, this is a cmd name that is called for this level
    #          if two, these are an argument and proc body
    #
    # Side Effects:
    #   Creates a logging command to take care of the details
    #   of logging an event.
    #
    # Results:
    #   If called with zero length args, returns the name of the currently
    #   configured logging procedure.
    #   
    #

    proc logproc {lv args} {
        variable levels
        variable lvlcmds
        
        set lvnum [lsearch -exact $levels $lv]
        if { $lvnum == -1 } {
        return -code error "Invalid level '$lv' - levels are $levels"
        }
        switch -exact -- [llength $args] {
        0  {
            return $lvlcmds($lv)
           }
        1  {
            set cmd [lindex $args 0]
            if {[string equal "[namespace current]::${lv}cmd" $cmd]} {return} 
            if {[llength [::info commands $cmd]]} {
                proc ${lv}cmd {args} "uplevel 1 \[list $cmd \[lindex \$args end\]\]"
            } else {
                return -code error "Invalid cmd '$cmd' - does not exist"
            }
            set lvlcmds($lv) $cmd
        }
        2  {
            foreach {arg body} $args {break}
            proc ${lv}cmd {args} "_setservicename \$args; 
                                      set val \[${lv}customcmd \[lindex \$args end\]\] ; 
                                      _restoreservice; set val"
            proc ${lv}customcmd $arg $body
            set lvlcmds($lv) [namespace current]::${lv}customcmd
        }
        default {
            return -code error "Usage: \${log}::logproc level ?cmd?\nor \${log}::logproc level argname body"
        }
        }
    }


    # delproc --
    #
    #   Set or introspect a callback for when the logger instance 
    #   is deleted.
    #
    # Arguments:
    #   cmd - the Tcl command to call.
    #   or none for introspection
    #
    # Side Effects:
    #   None.
    #
    # Results:
    #   If no arguments are given return the current callback cmd.

    proc delproc {args} {
        variable delcallback
        
        switch -exact -- [llength [::info level 0]] {
                1   {return $delcallback}
                2   { if {[::logger::_cmdPrefixExists [lindex $args 0]]} {
                            set delcallback [lindex $args 0]
                      } else {
                        return -code error "Invalid cmd '[lindex $args 0]' - does not exist"                      
                      }
                    }
                default {
                    return -code error "Wrong # of arguments. Usage: \${log}::delproc ?cmd?"
                }
        }
    }


    # delete --
    #
    #   Delete the namespace and its children.

    proc delete {} {
        variable delcallback
        variable service
        
        logger::walk [namespace current] delete
        if {[::logger::_cmdPrefixExists $delcallback]} {
             uplevel \#0 [lrange $delcallback 0 end]  
        } 
        # clean up the global services list
        set idx [lsearch -exact [logger::services] $service]
        if {$idx !=-1} {
            set ::logger::services [lreplace [logger::services] $idx $idx]
        }
        
        namespace delete [namespace current]
        
    }

    # services --
    #
    #   Return all child services 
    
    proc services {} {
        variable service
        
        set children [list]
        foreach srv [logger::services] {
            if {[string match "${service}::*" $srv]} {
                lappend children $srv
            }
        }
        return $children
    }

    # servicename --
    #
    #   Return the name of the service
    
    proc servicename {} {
        variable service
        return $service
    }
    
    proc _setservicename {arg} {
        variable service
        variable oldname
        if {[llength $arg] <= 1} {
            return
        } else {
            set oldname $service
            set service [lindex $arg end-1]
        }
    }
        
    proc _restoreservice {} {
        variable service
        variable oldname
        set service $oldname
        return
    }
    
    # Walk the parent service namespaces to see first, if they
    # exist, and if any are enabled, and then, as a
    # consequence, enable this one
    # too.

    enable $enabled
    variable parent [namespace parent]
    while {[string compare $parent "::logger::tree"]} {
        # If the 'enabled' variable doesn't exist, create the
        # whole thing.
        if { ! [::info exists ${parent}::enabled] } {
        
        logger::init [string range $parent 16 end]
        }
        set enabled [set ${parent}::enabled]
        enable $enabled
        set parent [namespace parent $parent]
    }
    }

    # Now create the commands for different levels.

    namespace eval tree::${service} {
    set parent [namespace parent]

    # We 'inherit' the commands from the parents.  This
    # means that, if you want to share the same methods with
    # children, they should be instantiated after the parent's
    # methods have been defined.
    if {[string compare $parent "::logger::tree"]} {
        foreach lvl [::logger::levels] {
            # OPTIMIZE: do not allow multiple aliases in the hierarchy
            #           they can always be replaced by more efficient
            #           direct aliases to the target procs.
            interp alias {} [namespace current]::${lvl}cmd {} ${parent}::${lvl}cmd $service
        }
        # inherit the starting loglevel of the parent service
        setlevel [${parent}::currentloglevel]

    } else {
        foreach lvl [::logger::levels] {
            proc ${lvl}cmd {args} "_setservicename \$args ; 
                                   set val \[stdoutcmd $lvl \[lindex \$args end\]\] ; 
                                   _restoreservice; set val"
            set lvlcmds($lvl) [namespace current]::${lvl}cmd
        }
    }
    }
    
    
    return ::logger::tree::${service}
}

# ::logger::services --
#
#   Returns a list of all active services.
#
# Arguments:
#   None.
#
# Side Effects:
#   None.
#
# Results:
#   List of active services.

proc ::logger::services {} {
    variable services
    return $services
}

# ::logger::enable --
#
#   Global enable for a certain level.  NOTE - this implementation
#   isn't terribly effective at the moment, because it might hit
#   children before their parents, who will then walk down the
#   tree attempting to disable the children again.
#
# Arguments:
#   lv - level above which to enable logging.
#
# Side Effects:
#   Enables logging in a given level, and all higher levels.
#
# Results:
#   None.

proc ::logger::enable {lv} {
    variable services
    if {[catch {
        foreach sv $services {
        ::logger::tree::${sv}::enable $lv
        }
    } msg] == 1} {
        return -code error -errorcode $::errorCode $msg
    }
}

proc ::logger::disable {lv} {
    variable services
    if {[catch {
        foreach sv $services {
        ::logger::tree::${sv}::disable $lv
        }
    } msg] == 1} {
        return -code error -errorcode $::errorCode $msg
    }
}

proc ::logger::setlevel {lv} {
    variable services
    if {[catch {
        foreach sv $services {
        ::logger::tree::${sv}::setlevel $lv
        }
    } msg] == 1} {
        return -code error -errorcode $::errorCode $msg
    }
}

# ::logger::levels --
#
#   Introspect the available log levels.  Provided so a caller does
#   not need to know implementation details or code the list
#   himself.
#
# Arguments:
#   None.
#
# Side Effects:
#   None.
#
# Results:
#   levels - The list of valid log levels accepted by enable and disable

proc ::logger::levels {} {
    variable levels
    return $levels
}

# ::logger::servicecmd --
#
#   Get the command token for a given service name.
#
# Arguments:
#   service - name of the service.
#
# Side Effects:
#   none
#
# Results:
#   log - namespace token for this service

proc ::logger::servicecmd {service} {
    variable services
    if {[lsearch -exact $services $service] == -1} {
        return -code error "Service \"$service\" does not exist."
    }
    return "::logger::tree::${service}"
}

# ::logger::import --
#
#   Import the logging commands.
#
# Arguments:
#   service - name of the service.
#
# Side Effects:
#   creates aliases in the target namespace
#
# Results:
#   none

proc ::logger::import {args} {
    variable services
    
    if {[llength $args] == 0 || [llength $args] > 7} {
    return -code error "Wrong # of arguments: \"logger::import ?-all?\
                        ?-force?\
                        ?-prefix prefix? ?-namespace namespace? service\""
    }
    
    # process options
    #
    set import_all 0
    set force 0
    set prefix ""
    set ns [uplevel 1 namespace current]
    while {[llength $args] > 1} {
        set opt [lindex $args 0]
        set args [lrange $args 1 end]
        switch  -exact -- $opt {
            -all    { set import_all 1}
            -prefix { set prefix [lindex $args 0]
                      set args [lrange $args 1 end]        
                    }
            -namespace {
                      set ns [lindex $args 0]
                      set args [lrange $args 1 end]
            }
            -force {
                     set force 1
            }
            default {
                return -code error "Unknown argument: \"$opt\" :\nUsage:\
                \"logger::import ?-all? ?-force?\
                        ?-prefix prefix? ?-namespace namespace? service\""
            }
        }
    }
    
    #
    # build the list of commands to import
    #
    
    set cmds [logger::levels]
    if {$import_all} {
        lappend cmds setlevel enable disable logproc delproc services 
        lappend cmds servicename currentloglevel delete
    }
    
    #
    # check the service argument
    #
    
    set service [lindex $args 0]
    if {[lsearch -exact $services $service] == -1} {
            return -code error "Service \"$service\" does not exist."
    }

    #
    # setup the namespace for the import
    #

    set sourcens [logger::servicecmd $service]     
    set localns  [uplevel 1 namespace current]
    
    if {[string match ::* $ns]} {
        set importns $ns
    } else {
        set importns ${localns}::$ns
    }    

    # fake namespace exists for Tcl 8.2 - 8.3
    if {![_nsExists $importns]} {
        namespace eval $importns {}
    } 

    
    #
    # prepare the import
    #
    
    set imports ""
    foreach cmd $cmds {
        set cmdname ${importns}::${prefix}$cmd
        set collision [llength [info commands $cmdname]]
        if {$collision && !$force} {
            return -code error "can't import command \"$cmdname\": already exists"
        }
        lappend imports ${importns}::${prefix}$cmd ${sourcens}::${cmd}
    }
    
    #
    # and execute the aliasing after checking all is well
    #
    
    foreach {target source} $imports {
        proc $target {args} "uplevel 1 \[linsert \$args 0 $source \]"
    }
}

