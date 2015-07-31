# docidx.tcl --
#
#	Implementation of docidx objects for Tcl.
#
# Copyright (c) 2003-2005 Andreas Kupries <andreas_kupries@sourceforge.net>
#
# See the file "license.terms" for information on usage and redistribution
# of this file, and for a DISCLAIMER OF ALL WARRANTIES.
# 
# RCS: @(#) $Id: docidx.tcl,v 1.1 2006/05/02 08:24:37 kohlbrecher Exp $

package require Tcl 8.2
package require textutil::expander

namespace eval ::doctools {}
namespace eval ::doctools::idx {
    # Data storage in the doctools::idx module
    # -------------------------------
    #
    # One namespace per object, containing
    #  1) A list of additional search paths for format definition files.
    #     This list extends the list of standard paths known to the module.
    #     The paths in the list are searched before the standard paths.
    #  2) Configuration information
    #     a) string:  The format to use when converting the input.
    #  4) Name of the interpreter used to perform the syntax check of the
    #     input (= allowed order of formatting commands).
    #  5) Name of the interpreter containing the code coming from the format
    #     definition file.
    #  6) Name of the expander object used to interpret the input to convert.

    # commands is the list of subcommands recognized by the docidx objects
    variable commands [list		\
	    "cget"			\
	    "configure"			\
	    "destroy"			\
	    "format"			\
	    "map"			\
	    "search"			\
	    "warnings"                  \
	    "parameters"                \
	    "setparam"                  \
	    ]

    # Only export the toplevel commands
    namespace export new search help

    # Global data

    #  1) List of standard paths to look at when searching for a format
    #     definition. Extensible.
    #  2) Location of this file in the filesystem

    variable paths [list]
    variable here [file dirname [info script]]
}

# ::doctools::idx::search --
#
#	Extend the list of paths used when searching for format definition files.
#
# Arguments:
#	path	Path to add to the list. The path has to exist, has to be a
#               directory, and has to be readable.
#
# Results:
#	None.
#
# Sideeffects:
#	The specified path is added to the front of the list of search
#	paths. This means that the new path is search before the
#	standard paths set at module initialization time.

proc ::doctools::idx::search {path} {
    variable paths

    if {![file exists      $path]} {return -code error "doctools::idx::search: path does not exist"}
    if {![file isdirectory $path]} {return -code error "doctools::idx::search: path is not a directory"}
    if {![file readable    $path]} {return -code error "doctools::idx::search: path cannot be read"}

    set paths [linsert $paths 0 $path]
    return
}

# ::doctools::idx::help --
#
#	Return a string containing short help
#	regarding the existing formatting commands.
#
# Arguments:
#	None.
#
# Results:
#	A string.

proc ::doctools::idx::help {} {
    return "formatting commands\n\
	    * index_begin      - begin of index\n\
	    * index_end        - end of index\n\
	    * key              - begin of references for key\n\
	    * manpage          - index reference to manpage\n\
	    * url              - index reference to url\n\
	    * vset             - set/get variable values\n\
	    * include          - insert external file\n\
	    * lb, rb           - left/right brackets\n\
	    "
}

# ::doctools::idx::new --
#
#	Create a new docidx object with a given name. May configure the object.
#
# Arguments:
#	name	Name of the docidx object.
#	args	Options configuring the new object.
#
# Results:
#	name	Name of the doctools created

proc ::doctools::idx::new {name args} {
        if { [llength [info commands ::$name]] } {
	return -code error "command \"$name\" already exists, unable to create docidx object"
    }
    if {[llength $args] % 2 == 1} {
	return -code error "wrong # args: doctools::new name ?opt val...??"
    }

    # The arguments seem to be ok, setup the namespace for the object

    namespace eval ::doctools::idx::docidx$name {
	variable paths      [list]
	variable file       ""
	variable format     ""
	variable formatfile ""
	variable format_ip  ""
	variable chk_ip     ""
	variable expander   "[namespace current]::ex"
	variable ex_ok      0
	variable msg        [list]
	variable map ;      array set map {}
	variable param      [list]
    }

    # Create the command to manipulate the object
    #                 $name -> ::doctools::idx::DocIdxProc $name
    interp alias {} ::$name {} ::doctools::idx::DocIdxProc $name

    # If the name was followed by arguments use them to configure the
    # object before returning its handle to the caller.

    if {[llength $args] > 1} {
	# Use linsert trick to make the command a pure list.
	eval [linsert $args 0 _configure $name]
    }
    return $name
}

##########################
# Private functions follow

# ::doctools::idx::DocIdxProc --
#
#	Command that processes all docidx object commands.
#	Dispatches any object command to the appropriate internal
#	command implementing its functionality.
#
# Arguments:
#	name	Name of the docidx object to manipulate.
#	cmd	Subcommand to invoke.
#	args	Arguments for subcommand.
#
# Results:
#	Varies based on command to perform

proc ::doctools::idx::DocIdxProc {name {cmd ""} args} {
    # Do minimal args checks here
    if { [llength [info level 0]] == 2 } {
	error "wrong # args: should be \"$name option ?arg arg ...?\""
    }
    
    # Split the args into command and args components

    if { [llength [info commands ::doctools::idx::_$cmd]] == 0 } {
	variable commands
	set optlist [join $commands ", "]
	set optlist [linsert $optlist "end-1" "or"]
	return -code error "bad option \"$cmd\": must be $optlist"
    }
    return [eval [list ::doctools::idx::_$cmd $name] $args]
}

##########################
# Method implementations follow (these are also private commands)

# ::doctools::idx::_cget --
#
#	Retrieve the current value of a particular option
#
# Arguments:
#	name	Name of the docidx object to query
#	option	Name of the option whose value we are asking for.
#
# Results:
#	The value of the option

proc ::doctools::idx::_cget {name option} {
    _configure $name $option
}

# ::doctools::idx::_configure --
#
#	Configure a docidx object, or query its configuration.
#
# Arguments:
#	name	Name of the docidx object to configure
#	args	Options and their values.
#
# Results:
#	None if configuring the object.
#	A list of all options and their values if called without arguments.
#	The value of one particular option if called with a single argument.

proc ::doctools::idx::_configure {name args} {
    if {[llength $args] == 0} {
	# Retrieve the current configuration.

	upvar ::doctools::idx::docidx${name}::file    file
	upvar ::doctools::idx::docidx${name}::format     format

	set     res [list]
	lappend res -file       $file
	lappend res -format     $format
	return $res

    } elseif {[llength $args] == 1} {
	# Query the value of one particular option.

	switch -exact -- [lindex $args 0] {
	    -file {
		upvar ::doctools::idx::docidx${name}::file file
		return $file
	    }
	    -format {
		upvar ::doctools::idx::docidx${name}::format format
		return $format
	    }
	    default {
		return -code error \
			"doctools::idx::_configure: Unknown option \"[lindex $args 0]\", expected\
			-file, or -format"
	    }
	}
    } else {
	# Reconfigure the object.

	if {[llength $args] % 2 == 1} {
	    return -code error "wrong # args: doctools::idx::_configure name ?opt val...??"
	}

	foreach {option value} $args {
	    switch -exact -- $option {
		-file {
		    upvar ::doctools::idx::docidx${name}::file file
		    set file $value
		}
		-format {
		    if {[catch {
			set fmtfile [LookupFormat $name $value]
			SetupFormatter $name $fmtfile
			upvar ::doctools::idx::docidx${name}::format format
			set format $value
		    } msg]} {
			return -code error "doctools::idx::_configure: -format: $msg"
		    }
		}
		default {
		    return -code error \
			    "doctools::idx::_configure: Unknown option \"$option\", expected\
			    -file, or -format"
		}
	    }
	}
    }
    return ""
}

# ::doctools::idx::_destroy --
#
#	Destroy a docidx object, including its associated command and data storage.
#
# Arguments:
#	name	Name of the docidx object to destroy.
#
# Results:
#	None.

proc ::doctools::idx::_destroy {name} {
    # Check the object for sub objects which have to destroyed before
    # the namespace is torn down.
    namespace eval ::doctools::idx::docidx$name {
	if {$format_ip != ""} {interp delete $format_ip}
	if {$chk_ip    != ""} {interp delete $chk_ip}

	# Expander objects have no delete/destroy method. This would
	# be a leak if not for the fact that an expander object is a
	# namespace, and we have arranged to make it a sub namespace of
	# the docidx object. Therefore tearing down our object namespace
	# also cleans up the expander object.
	# if {$expander != ""} {$expander destroy}

    }
    namespace delete ::doctools::idx::docidx$name
    interp alias {} ::$name {}
    return
}

# ::doctools::idx::_map --
#
#	Add a mapping from symbolic to actual filename to the object.
#
# Arguments:
#	name	Name of the docidx object to use
#	sfname	Symbolic filename to map
#	afname	Actual filename
#
# Results:
#	None.

proc ::doctools::idx::_map {name sfname afname} {
    upvar ::doctools::idx::docidx${name}::map map
    set map($sfname) $afname
    return
}

# ::doctools::idx::_format --
#
#	Convert some text in doctools format
#	according to the configuration in the object.
#
# Arguments:
#	name	Name of the docidx object to use
#	text	Text to convert.
#
# Results:
#	The conversion result.

proc ::doctools::idx::_format {name text} {
    upvar ::doctools::idx::docidx${name}::format format
    if {$format == ""} {
	return -code error "$name: No format was specified"
    }

    upvar ::doctools::idx::docidx${name}::format_ip format_ip
    upvar ::doctools::idx::docidx${name}::chk_ip    chk_ip
    upvar ::doctools::idx::docidx${name}::ex_ok     ex_ok
    upvar ::doctools::idx::docidx${name}::expander  expander
    upvar ::doctools::idx::docidx${name}::passes    passes
    upvar ::doctools::idx::docidx${name}::msg       warnings

    if {!$ex_ok}       {SetupExpander  $name}
    if {$chk_ip == ""} {SetupChecker   $name}
    # assert (format_ip != "")

    set warnings [list]
    if {[catch {$format_ip eval idx_initialize}]} {
	return -code error "Could not initialize engine"
    }
    set result ""

    for {
	set p $passes ; set n 1
    } {
	$p > 0
    } {
	incr p -1 ; incr n
    } {
	if {[catch {$format_ip eval [list idx_setup $n]}]} {
	    catch {$format_ip eval idx_shutdown}
	    return -code error "Could not initialize pass $n of engine"
	}
	$chk_ip eval ck_initialize

	if {[catch {set result [$expander expand $text]} msg]} {
	    catch {$format_ip eval idx_shutdown}
	    # Filter for checker errors and reduce them to the essential message.

	    if {![regexp {^Error in} $msg]}          {return -code error $msg}
	    #set msg [join [lrange [split $msg \n] 2 end]]

	    if {![regexp {^--> \(FmtError\) } $msg]} {return -code error "Docidx $msg"}
	    set msg [lindex [split $msg \n] 0]
	    regsub {^--> \(FmtError\) } $msg {} msg

	    return -code error $msg
	}

	$chk_ip eval ck_complete
    }

    if {[catch {set result [$format_ip eval [list idx_postprocess $result]]}]} {
	return -code error "Unable to post process final result"
    }
    if {[catch {$format_ip eval idx_shutdown}]} {
	return -code error "Could not shut engine down"
    }
    return $result

}

# ::doctools::idx::_search --
#
#	Add a search path to the object.
#
# Arguments:
#	name	Name of the docidx object to extend
#	path	Search path to add.
#
# Results:
#	None.

proc ::doctools::idx::_search {name path} {
    if {![file exists      $path]} {return -code error "$name search: path does not exist"}
    if {![file isdirectory $path]} {return -code error "$name search: path is not a directory"}
    if {![file readable    $path]} {return -code error "$name search: path cannot be read"}

    upvar ::doctools::idx::docidx${name}::paths paths
    set paths [linsert $paths 0 $path]
    return
}

# ::doctools::idx::_warnings --
#
#	Return the warning accumulated during the last invocation of 'format'.
#
# Arguments:
#	name	Name of the docidx object to query
#
# Results:
#	A list of warnings.

proc ::doctools::idx::_warnings {name} {
    upvar ::doctools::idx::docidx${name}::msg msg
    return $msg
}

# ::doctools::_parameters --
#
#	Returns a list containing the parameters provided
#	by the selected formatting engine.
#
# Arguments:
#	name	Name of the doctools object to query
#
# Results:
#	A list of parameter names

proc ::doctools::idx::_parameters {name} {
    upvar ::doctools::idx::docidx${name}::param param
    return $param
}

# ::doctools::_setparam --
#
#	Set a named engine parameter to a value.
#
# Arguments:
#	name	Name of the doctools object to query
#	param	Name of the parameter to set.
#	value	Value to set the parameter to.
#
# Results:
#	None.

proc ::doctools::idx::_setparam {name param value} {
    upvar ::doctools::idx::docidx${name}::format_ip format_ip

    if {$format_ip == {}} {
	return -code error \
		"Unable to set parameters without a valid format"
    }

    $format_ip eval [list idx_varset $param $value]
    return
}

##########################
# Support commands

# ::doctools::idx::LookupFormat --
#
#	Search a format definition file based upon its name
#
# Arguments:
#	name	Name of the docidx object to use
#	format	Name of the format to look for.
#
# Results:
#	The file containing the format definition

proc ::doctools::idx::LookupFormat {name format} {
    # Order of searching
    # 1) Is the name of the format an existing file ?
    #    If yes, take this file.
    # 2) Look for the file in the directories given to the object itself..
    # 3) Look for the file in the standard directories of this package.

    if {[file exists $format]} {
	return $format
    }

    upvar ::doctools::idx::docidx${name}::paths opaths
    foreach path $opaths {
	set f [file join $path idx.$format]
	if {[file exists $f]} {
	    return $f
	}
    }

    variable paths
    foreach path $paths {
	set f [file join $path idx.$format]
	if {[file exists $f]} {
	    return $f
	}
    }

    return -code error "Unknown format \"$format\""
}

# ::doctools::idx::SetupFormatter --
#
#	Create and initializes an interpreter containing a
#	formatting engine
#
# Arguments:
#	name	Name of the docidx object to manipulate
#	format	Name of file containing the code of the engine
#
# Results:
#	None.

proc ::doctools::idx::SetupFormatter {name format} {

    # Create and initialize the interpreter first.
    # Use a transient variable. Interrogate the
    # engine and check its response. Bail out in
    # case of errors. Only if we pass the checks
    # we tear down the old engine and make the new
    # one official.

    variable here
    set mpip [interp create -safe] ; # interpreter for the formatting engine
    #set mpip [interp create] ; # interpreter for the formatting engine

    $mpip invokehidden source [file join $here api_idx.tcl]
    #$mpip eval [list source [file join $here api_idx.tcl]]
    interp alias $mpip dt_source   {} ::doctools::idx::Source $mpip [file dirname $format]
    interp alias $mpip dt_package  {} ::doctools::Package $mpip
    interp alias $mpip file        {} ::doctools::FileOp  $mpip
    interp alias $mpip puts_stderr {} ::puts stderr
    $mpip invokehidden source $format
    #$mpip eval [list source $format]

    # Check the engine for useability in doctools.

    foreach api {
	idx_numpasses
	idx_initialize
	idx_setup
	idx_postprocess
	idx_shutdown
	idx_listvariables
	idx_varset
    } {
	if {[$mpip eval [list info commands $api]] == {}} {
	    interp delete $mpip
	    error "$format error: API incomplete, cannot use this engine"
	}
    }
    if {[catch {
	set passes [$mpip eval idx_numpasses]
    }]} {
	interp delete $mpip
	error "$format error: Unable to query for number of passes"
    }
    if {![string is integer $passes] || ($passes < 1)} {
	interp delete $mpip
	error "$format error: illegal number of passes \"$passes\""
    }
    if {[catch {
	set parameters [$mpip eval idx_listvariables]
    }]} {
	interp delete $mpip
	error "$format error: Unable to query for list of parameters"
    }

    # Passed the tests. Tear down existing engine,
    # and checker. The latter is destroyed because
    # of its aliases into the formatter, which are
    # now invalid. It will be recreated during the
    # next call of 'format'.

    upvar ::doctools::idx::docidx${name}::formatfile formatfile
    upvar ::doctools::idx::docidx${name}::format_ip  format_ip
    upvar ::doctools::idx::docidx${name}::chk_ip     chk_ip
    upvar ::doctools::idx::docidx${name}::expander   expander
    upvar ::doctools::idx::docidx${name}::passes     xpasses
    upvar ::doctools::idx::docidx${name}::param      xparam

    if {$chk_ip != {}}    {interp delete $chk_ip}
    if {$format_ip != {}} {interp delete $format_ip}

    set chk_ip    ""
    set format_ip ""

    # Now link engine API into it.

    interp alias $mpip dt_format    {} ::doctools::idx::GetFormat    $name
    interp alias $mpip dt_user      {} ::doctools::idx::GetUser      $name
    interp alias $mpip dt_fmap      {} ::doctools::idx::MapFile      $name

    foreach cmd {cappend cget cis cname cpop cpush cset lb rb} {
	interp alias $mpip ex_$cmd {} $expander $cmd
    }

    set format_ip  $mpip
    set formatfile $format
    set xpasses    $passes
    set xparam     $parameters
    return
}

# ::doctools::idx::SetupChecker --
#
#	Create and initializes an interpreter for checking the usage of
#	docidx formatting commands
#
# Arguments:
#	name	Name of the docidx object to manipulate
#
# Results:
#	None.

proc ::doctools::idx::SetupChecker {name} {
    # Create an interpreter for checking the usage of docidx formatting commands
    # and initialize it: Link it to the interpreter doing the formatting, the
    # expander object and the configuration information. All of which
    # is accessible through the token/handle (name of state/object array).

    variable here

    upvar ::doctools::idx::docidx${name}::chk_ip    chk_ip
    if {$chk_ip != ""} {return}

    upvar ::doctools::idx::docidx${name}::expander  expander
    upvar ::doctools::idx::docidx${name}::format_ip format_ip

    set chk_ip [interp create] ; # interpreter hosting the formal format checker

    # Make configuration available through command, then load the code base.

    foreach {cmd ckcmd} {
	dt_search     SearchPaths
	dt_error      FmtError
	dt_warning    FmtWarning
    } {
	interp alias $chk_ip $cmd {} ::doctools::idx::$ckcmd $name
    }
    $chk_ip eval [list source [file join $here checker_idx.tcl]]

    # Simple expander commands are directly routed back into it, no
    # checking required.

    foreach cmd {cappend cget cis cname cpop cpush cset lb rb} {
	interp alias $chk_ip $cmd {} $expander $cmd
    }

    # Link the formatter commands into the checker. We use the prefix
    # 'fmt_' to distinguish them from the checking commands.

    foreach cmd {
	index_begin index_end key manpage url comment plain_text
    } {
	interp alias $chk_ip fmt_$cmd $format_ip fmt_$cmd
    }
    return
}

# ::doctools::idx::SetupExpander --
#
#	Create and initializes the expander for input
#
# Arguments:
#	name	Name of the docidx object to manipulate
#
# Results:
#	None.

proc ::doctools::idx::SetupExpander {name} {
    upvar ::doctools::idx::docidx${name}::ex_ok    ex_ok
    if {$ex_ok} {return}

    upvar ::doctools::idx::docidx${name}::expander expander
    ::textutil::expander $expander
    $expander evalcmd [list ::doctools::idx::Eval $name]
    $expander textcmd plain_text
    set ex_ok 1
    return
}

# ::doctools::idx::SearchPaths --
#
#	API for checker. Returns list of search paths for format
#	definitions. Used to look for message catalogs as well.
#
# Arguments:
#	name	Name of the docidx object to query.
#
# Results:
#	None.

proc ::doctools::idx::SearchPaths {name} {
    upvar ::doctools::idx::docidx${name}::paths opaths
    variable paths

    set p $opaths
    foreach s $paths {lappend p $s}
    return $p
}

# ::doctools::idx::FmtError --
#
#	API for checker. Called when an error occurred.
#
# Arguments:
#	name	Name of the docidx object to query.
#	text	Error message
#
# Results:
#	None.

proc ::doctools::idx::FmtError {name text} {
    return -code error "(FmtError) $text"
}

# ::doctools::idx::FmtWarning --
#
#	API for checker. Called when a warning was generated
#
# Arguments:
#	name	Name of the docidx object
#	text	Warning message
#
# Results:
#	None.

proc ::doctools::idx::FmtWarning {name text} {
    upvar ::doctools::idx::docidx${name}::msg msg
    lappend msg $text
    return
}

# ::doctools::idx::Eval --
#
#	API for expander. Routes the macro invocations
#	into the checker interpreter
#
# Arguments:
#	name	Name of the docidx object to query.
#
# Results:
#	None.

proc ::doctools::idx::Eval {name macro} {
    upvar ::doctools::idx::docidx${name}::chk_ip chk_ip

    # Handle the [include] command directly
    if {[string match include* $macro]} {
	foreach {cmd filename} $macro break
	return [ExpandInclude $name $filename]
    }

    return [$chk_ip eval $macro]
}

# ::doctools::idx::ExpandInclude --
#
#	Handle inclusion of files.
#
# Arguments:
#	name	Name of the docidx object to query.
#	path	Name of file to include and expand.
#
# Results:
#	None.

proc ::doctools::idx::ExpandInclude {name path} {
    upvar ::doctools::idx::docidx${name}::file file

    set ipath [file join [file dirname $file] $path]
    if {![file exists $ipath]} {
	set ipath $path
	if {![file exists $ipath]} {
	    return -code error "Unable to fine include file \"$path\""
	}
    }

    set    chan [open $ipath r]
    set    text [read $chan]
    close $chan

    upvar ::doctools::idx::docidx${name}::expander  expander

    return [$expander expand $text]
}

# ::doctools::idx::GetUser --
#
#	API for formatter. Returns name of current user
#
# Arguments:
#	name	Name of the docidx object to query.
#
# Results:
#	String, name of current user.

proc ::doctools::idx::GetUser {name} {
    global  tcl_platform
    return $tcl_platform(user)
}

# ::doctools::idx::GetFormat --
#
#	API for formatter. Returns format information
#
# Arguments:
#	name	Name of the docidx object to query.
#
# Results:
#	Format information

proc ::doctools::idx::GetFormat {name} {
    upvar ::doctools::idx::docidx${name}::format format
    return $format
}

# ::doctools::idx::MapFile --
#
#	API for formatter. Maps symbolic to actual filename in an
#	index element. If no mapping is found it is assumed that
#	the symbolic name is also the actual name.
#
# Arguments:
#	name	Name of the docidx object to query.
#	fname	Symbolic name of the file.
#
# Results:
#	Actual name of the file.

proc ::doctools::idx::MapFile {name fname} {
    upvar ::doctools::idx::docidx${name}::map map
    if {[info exists map($fname)]} {
	return $map($fname)
    }
    return $fname
}

# ::doctools::idx::Source --
#
#	API for formatter. Used by engine to ask for
#	additional script files support it.
#
# Arguments:
#	name	Name of the docidx object to change.
#
# Results:
#	Boolean flag.

proc ::doctools::idx::Source {ip path file} {
    $ip invokehidden source [file join $path [file tail $file]]
    #$ip eval [list source [file join $path [file tail $file]]]
    return
}

#------------------------------------
# Module initialization

namespace eval ::doctools::idx {
    # Reverse order of searching. First to search is specified last.

    # FOO/docidx.tcl
    # => FOO/mpformats

    #catch {search [file join $here                lib doctools mpformats]}
    #catch {search [file join [file dirname $here] lib doctools mpformats]}
    catch {search [file join $here                             mpformats]}
}

package provide doctools::idx 0.2.1
