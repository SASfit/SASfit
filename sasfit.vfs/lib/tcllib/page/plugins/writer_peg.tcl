# -*- tcl -*- 
# -- $Id: writer_peg.tcl,v 1.1 2006/05/02 12:32:12 kohlbrecher Exp $ ---
#
# PAGE plugin - writer - PEG ~ Canonical PE Grammar
#

# ### ### ### ######### ######### #########
## Imported API

# -----------------+--
# page_wdata       | Access to processed input stream.
# -----------------+--
# page_info        | Reporting to the user.
# page_warning     |
# page_error       |
# -----------------+--
# page_log_error   | Reporting of internals.
# page_log_warning |
# page_log_info    |
# -----------------+--

# ### ### ### ######### ######### #########
## Exported API

# -----------------+--
# page_wfeature    | Query for special plugin features page might wish to use.
# page_wtime       | Activate collection of timing statistics.
# page_wgettime    | Return the collected timing statistics.
# page_wlabel      | User readable label for the plugin.
# page_whelp       | Doctools help text for plugin.
# page_woptions    | Options understood by plugin.
# page_wconfigure  | Option (re)configuration.
# page_wrun        | Generate output from data per plugin configuration and hardwiring.
# -----------------+--

# ### ### ### ######### ######### #########
## Requisites

package require page::gen::peg::canon
package require struct::tree         ; # Data structure.

global usec
global timed
set    timed 0

# ### ### ### ######### ######### #########
## Implementation of exported API

proc page_wlabel {} {
    return {Canonical PEG}
}

proc page_wfeature {key} {
    return [string eq $key timeable]
}

proc page_wtime {} {
    global timed
    set    timed 1
    return
}

proc page_wgettime {} {
    global  usec
    return $usec
}

proc page_whelp {} {
    return {}
}

proc page_woptions {} {
    return {}
}

proc page_wconfigure {option value} {
    return -code error "Cannot set value of unknown option \"$option\""
}

proc page_wrun {chan data} {
    global timed usec
    page_log_info "writer/peg/run/"

    if {$timed} {
	set usec [lindex [time {
	    ::struct::tree        ::tree deserialize $data
	    page::gen::peg::canon ::tree             $chan
	}] 0] ; #{}
    } else {
	::struct::tree        ::tree deserialize $data
	page::gen::peg::canon ::tree             $chan
    }

    page_log_info "writer/peg/run/ok"

    ::tree destroy
    return
}

# ### ### ### ######### ######### #########
## Internal helper code.

# ### ### ### ######### ######### #########
## Initialization

package provide page::writer::peg 0.1
