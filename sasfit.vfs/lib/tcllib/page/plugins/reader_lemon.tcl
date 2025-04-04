# -*- tcl -*- 
# -- $Id: reader_lemon.tcl,v 1.1 2006/05/02 12:32:12 kohlbrecher Exp $ ---
#
# PAGE plugin - reader - LEMON ~ Grammar specification as understood
#                                by drh's lemon parser generator.
#

# ### ### ### ######### ######### #########
## Imported API

# -----------------+--
# page_read        | Access to the input stream.
# page_read_done   |
# page_eof         |
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
# page_rfeature    | Query for special plugin features page might wish to use.
# page_rtime       | Activate collection of timing statistics.
# page_rgettime    | Return the collected timing statistics.
# page_rlabel      | User readable label for the plugin.
# page_rhelp       | Doctools help text for plugin.
# page_roptions    | Options understood by plugin.
# page_rconfigure  | Option (re)configuration.
# page_rdata       | External access to processed input stream.
# page_rrun        | Process input stream per plugin configuration and hardwiring.
# -----------------+--

# ### ### ### ######### ######### #########
## Requisites

package require page::util::norm::lemon ; # Normalize AST generated by PE matcher.
package require page::parse::lemon      ; # Mengine based parser for Lemon grammars.
package require struct::tree            ; # Data structure.
package require grammar::me::util       ; # AST conversion

global usec
global timed
set    timed 0

global cline
global ccol

# ### ### ### ######### ######### #########
## Implementation of exported API

proc page_rlabel {} {
    return {Lemon specification}
}

proc page_rfeature {key} {
    return [string eq $key timeable]
}

proc page_rtime {} {
    global timed
    set    timed 1
    return
}

proc page_rgettime {} {
    global  usec
    return $usec
}

proc page_rhelp {} {
    return {}
}

proc page_roptions {} {
    return {}
}

proc page_rconfigure {option value} {
    return -code error "Cannot set value of unknown option \"$option\""
}

## proc page_rdata {} {}
## Created in 'Initialize'

proc page_rrun {} {
    global timed usec cline ccol
    page_log_info "reader/lemon/run/parse"

    set ast {}
    set err {}

    # Location of the next character to be read.
    set cline 1
    set ccol  0

    if {$timed} {
	set usec [lindex [time {
	    set ok [::page::parse::lemon::parse ::Next err ast]
	}] 0] ; #{}
    } else {
	set ok [::page::parse::lemon::parse ::Next err ast]
    }
    page_read_done
    page_log_info "reader/lemon/run/check-for-errors"

    if {!$ok} {
	foreach {olc   messages} $err     break
	foreach {offset linecol} $olc     break
	foreach {line       col} $linecol break

	set olc [string map {{ } _} \
		[format %5d $line]]@[string map {{ } _} \
		[format %3d $col]]/([format %5d $offset])

	foreach m $messages {
	    page_log_error "reader/lemon/run: $olc: $m"
	    page_error $m $linecol
	}

	page_log_info "reader/lemon/run/failed"
	return {}
    }

    page_log_info "reader/lemon/run/ast-conversion"

    struct::tree                        ::tree
    ::grammar::me::util::ast2etree $ast ::grammar::me::tcl ::tree
    ::page::util::norm::lemon           ::tree

    set ast [::tree serialize]
    ::tree destroy

    page_log_info "reader/lemon/run/ok"
    return $ast
}

# ### ### ### ######### ######### #########
## Internal helper code.

proc Next {} {
    global cline ccol

    if {[page_eof]} {return {}}

    set ch [page_read 1]

    if {$ch eq ""} {return {}}

    set tok [list $ch {} $cline $ccol]

    if {$ch eq "\n"} {
	incr cline ; set ccol 0
    } else {
	incr ccol
    }

    return $tok
}

# ### ### ### ######### ######### #########
## Initialization

package provide page::reader::lemon 0.1
