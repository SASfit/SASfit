# sasfit.vfs/lib/app-sasfit/tcl/sasfit_plugin.tcl
#
# Copyright (c) 2008-2018, Paul Scherrer Institute (PSI)
#
# This file is part of SASfit.
#
# SASfit is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# SASfit is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with SASfit.  If not, see <http://www.gnu.org/licenses/>.

# Author(s) of this file:
#   Ingo Bressler (ingo@cs.tu-berlin.de)

# loads all valid plugins in given path
# - determines group definitions
# - parses header file, checks if referenced groups exist, breaks up on error
# - c-function 'sasfit_load_plugin' tries to load plugin (shared library)
#   breaks up on error and unloads library
#   returns list of association of function names to unique IDs on success
#
# notation: 'idx' means list index in global ::sasfit(group_defs)
#           'id' means function array index in sasfit_core/sasfit_plugin_db.c
#
proc sasfit_load_plugins { plugin_paths } {
	upvar ::sasfit(group_file) group_def_filename
	upvar ::sasfit(group_defs) groups

	set groups [get_group_defs $group_def_filename]
	set groups_num [llength $groups]
	puts "done. Loaded $groups_num group(s).\n"

	set lib_files {}
	foreach plugin_path $plugin_paths {
		puts "Searching for plugins in '$plugin_path'\n"
		foreach lib_file [glob -nocomplain -directory $plugin_path sasfit_*[sasfit_get_lib_suffix]] {
			lappend lib_files $lib_file
		}
	}
	if { ! [llength $lib_files] } {
		puts "no plugins found in '$plugin_paths'"
		return
	}

#	puts [time {
	set file_count [llength $lib_files]
	set file_count_max [expr $file_count*10]
	set filenr 0
	while { $filenr < [llength $lib_files]
	} {
		set lib_file [lindex $lib_files $filenr]
		incr filenr

		set basename [file rootname [file tail $lib_file]]
		set header_file [file join $plugin_path "$basename.h"]

		puts "-----\nparsing header file: $header_file"
		set func_info [parse_header groups $header_file $basename]
		set func_num [llength $func_info]
		if { $func_num <= 0 } { continue }

		puts "Trying to load '$lib_file' ... "

		# load plugin and get list of unique identifiers
		set id_list [sasfit_load_plugin $lib_file $func_num]
		if { ! [llength $id_list] 
		} { 
			puts stderr "Failed to load '$lib_file'!"
			continue 
		}
		if { [string equal [lindex $id_list 0] "dependency_error"] 
		} {
			if {[llength $lib_files] < $file_count_max &&
			    [llength $lib_files] != $filenr
			} {
				# add it at the end of the list for later retry
				puts "Dependency not found, postpone it ..."
				lappend lib_files $lib_file
			} else {
				puts stderr "Dependency not found, failed to load '$lib_file'!"
			}
			continue
		}
		# update tcl group/function list with unique IDs
		set index 0
		foreach {name id} $id_list {
			set index [lsearch $func_info "$name *"]
			if {$index < 0} { break }
			lset func_info $index $::idx_plgid $id
		}
		if {$index < 0} {
			puts stderr "'$name' was not found in $header_file!"
			puts stderr "Failed to load '$lib_file'!"
			continue 
		}
		# add the function records to the list of groups
		# update the children list of the parent group
		# currently they are stored in a list, 
		# we should use an array (later) for obviously faster access
		foreach func $func_info {
			lappend groups $func
			set cur_idx [expr {[llength $groups] - 1}]
			add_child groups [lindex $func $::idx_parents] $cur_idx
		}
		puts "success."
	}
#	}]
	set func_num [expr {[llength $groups] - $groups_num}]

	# convert the function names in field default_sd to list indices
	for {set idx 0} {$idx < [llength $groups]} {incr idx} {
		# foreach function record where the default sd was set
		if { [is_func_by_idx $idx] } {
			set def_sd_name [lindex $groups $idx $::idx_defsd]
			set sd_idx -1
			if { ! [string is integer -strict $def_sd_name] &&
			       [string length $def_sd_name] > 0
			} {
				set regexp "^[convert_to_nocase_regexp $def_sd_name] .*"
				set sd_idx [lsearch -regexp -start $sd_idx $groups $regexp]
				if { $sd_idx > [llength $groups] } { set sd_idx -1 }
				if { $sd_idx >= 0 } {
					set parent_head [get_top_level_parent [lindex $groups $sd_idx $::idx_name]]
					if { [string compare $parent_head "size_distrib"] != 0 } {
						set sd_idx -1
					}
				}
				# replace the name by its index
				lset groups $idx $::idx_defsd $sd_idx
			}
		}
	}
#	puts $groups
	puts ""
#	puts "test: '[get_functions_in_group size_distrib]' '[get_functions_in_group cyl_obj]'"
}

proc convert_to_nocase_regexp { str } {
	return [string map -nocase {
		a [aA] b [bB] c [cC] d [dD] e [eE] f [fF] g [gG] h [hH] i [iI]
		j [jJ] k [kK] l [lL] m [mM] n [nN] o [oO] p [pP] q [qQ] r [rR]
		s [sS] t [tT] u [uU] v [vV] w [wW] x [xX] y [yY] z [zZ]
	} $str]
}

#
# Access procedures to global datastructure
#

if {[info exists ::idx_name]} {puts stderr "::idx_name already exists in global context !!"}
# !! also used for parameter name
set ::idx_name		0
if {[info exists ::idx_label]} {puts stderr "::idx_label already exists in global context !!"}
# !! also used for parameter label
set ::idx_label		1
if {[info exists ::idx_parents]} {puts stderr "::idx_parents already exists in global context !!"}
set ::idx_parents	2
if {[info exists ::idx_children]} {puts stderr "::idx_children already exists in global context !!"}
set ::idx_children	3
if {[info exists ::idx_plgid]} {puts stderr "::idx_plgid already exists in global context !!"}
set ::idx_plgid		4
if {[info exists ::idx_defsd]} {puts stderr "::idx_defsd already exists in global context !!"}
set ::idx_defsd		5
if {[info exists ::idx_descr]} {puts stderr "::idx_descr already exists in global context !!"}
set ::idx_descr		6
if {[info exists ::idx_param]} {puts stderr "::idx_param already exists in global context !!"}
set ::idx_param		7
if {[info exists ::idx_bname]} {puts stderr "::idx_bname already exists in global context !!"}
set ::idx_bname		8
if {[info exists ::idx_max]} {puts stderr "::idx_max already exists in global context !!"}
set ::idx_max		$::idx_bname

#
# tests
# 

proc is_valid_idx { idx } {
	upvar ::sasfit(group_defs) groups
	return [expr { [string is integer -strict $idx] && 
			$idx >= 0 && 
			[llength [lindex $groups $idx]] >= 5 }]
}
# list index is a group if plugin function ID < 0 (does not exist)
proc is_group_by_idx { rec_idx } {
	upvar ::sasfit(group_defs) groups
	return [expr { [is_valid_idx $rec_idx] && 
			[llength [lindex $groups $rec_idx]] == 5 && 
			[lindex $groups $rec_idx $::idx_plgid] < 0 }]
}
# list index is a function if plugin function ID >= 0 (does exist)
proc is_func_by_idx { rec_idx } {
	upvar ::sasfit(group_defs) groups
	return [expr { [is_valid_idx $rec_idx] && 
			[llength [lindex $groups $rec_idx]] > $::idx_max && 
			[lindex $groups $rec_idx $::idx_plgid] >= 0 }]
}
# function record is valid if it has a complete set of info fields and
# there exists a plugin function ID
proc is_func_by_rec { rec } {
	return [expr { [llength $rec] > $::idx_max && 
			[lindex $rec $::idx_plgid] >= 0 }]
}

# check if a function has a default SD function to set
# (usually only for FF)
proc has_default_sd_by_idx { rec_idx } {
	upvar ::sasfit(group_defs) groups
	if { [is_func_by_idx $rec_idx] } {
		if { [lindex $groups $rec_idx $::idx_defsd] >= 0 } { return 1 }
	}
	return 0
}

proc name_exists { name } {
	upvar ::sasfit(group_defs) groups
	return [expr [lsearch $groups "$name *"] >= 0]
}

#
# utitility/access functions
#

proc get_all_func {} {
	upvar ::sasfit(group_defs) groups
	set l {}
	foreach rec $groups {
		if {[is_func_by_rec $rec]} {
			lappend l [lindex $rec $::idx_name]
		}
	}
	return $l
}

# returns name and title of the default SD function to select 
# (usually only for FF)
proc get_default_sd_name_by_idx { rec_idx } {
	upvar ::sasfit(group_defs) groups
	set sd_idx [lindex $groups $rec_idx $::idx_defsd]
	if {[string is integer -strict $sd_idx]} {
		return [lindex $groups $sd_idx $::idx_name]
	} else {
		return $sd_idx
	}
}
proc get_default_sd_title_by_idx { rec_idx } {
	upvar ::sasfit(group_defs) groups
	set sd_idx [lindex $groups $rec_idx $::idx_defsd]
	if {[string is integer -strict $sd_idx]} {
		return [lindex $groups $sd_idx $::idx_label]
	} else {
		return $sd_idx
	}
}

# some kind of tree traversal, recursive for now ...
#> not used anymore
proc get_all_group_idx { group_name } {
	upvar ::sasfit(group_defs) groups

	set this_idx [get_record_idx_by_name $group_name]
	if { ! [is_group_by_idx $this_idx] } { return }

	set result [list $this_idx]
	# get all children and take only groups
	foreach idx [lindex $groups $this_idx $::idx_children] {
		if { $idx < 0 } { continue }; # should not happen
		if { [lindex $groups $idx $::idx_plgid] > -1 } { continue }
		foreach elem [get_all_group_idx $idx] {
			lappend result $elem
		}
	}
	return $result
}

# get the list indices of all children
proc get_children_idx_by_idx { idx } {
	upvar ::sasfit(group_defs) groups
	if { ! [is_group_by_idx $idx] } { return }
	return [lindex $groups $idx $::idx_children]
}

proc get_children_idx_by_name { group_name } {
	upvar ::sasfit(group_defs) groups
	return [get_children_idx_by_idx [get_record_idx_by_name $group_name]]
}

proc get_children_names_by_name { group_name } {
	upvar ::sasfit(group_defs) groups
	set namelst {}
	foreach idx [get_children_idx_by_name $group_name] {
		if {[is_group_by_idx $idx]} {
			lappend namelst [get_name_by_idx $idx]
		}
	}
	return $namelst
}

proc get_basename_by_name { group_name } {
	upvar ::sasfit(group_defs) groups
	set idx [get_record_idx_by_name $group_name]
	if {[is_func_by_idx $idx]} {
		return [lindex $groups $idx $::idx_bname]
	} else { return {} }
}

# works for numerical indices as well as for names
proc get_func_idx_in_group { group_name } {
	upvar ::sasfit(group_defs) groups
	
	set rec [get_record_idx_by_name $group_name]
	if { ! [is_group_by_idx $rec] } { return }

	set result {}
	# get all children and take only functions
	foreach idx [lindex $groups $rec $::idx_children] {
		if { $idx < 0 } { continue }; # should not happen
		if { [lindex $groups $idx $::idx_plgid] < 0 } { continue }
		lappend result $idx
	}
	return $result
}

# works for numerical indices as well as for names
proc get_record_idx_by_name { name } {
	upvar ::sasfit(group_defs) groups
	set rec {}
	if { [is_valid_idx $name] } {
		set rec $name
	} else {
		set rec [lsearch $groups "$name *"]
	}
	return $rec
}

proc get_name_by_idx { idx } {
	upvar ::sasfit(group_defs) groups
	if { [is_valid_idx $idx] } { 
		return [lindex $groups $idx $::idx_name] 
	} else {
		return $idx
	}
}
proc get_name_by_id { func_id } {
	upvar ::sasfit(group_defs) groups
	if { ![string is integer -strict $func_id] || $func_id < 0 } { return $func_id }
	return [lindex [get_record_by_id $func_id] $::idx_name]
}
proc get_title_by_id { func_id } {
	upvar ::sasfit(group_defs) groups
	if { ![string is integer -strict $func_id] || $func_id < 0 } { return $func_id }
	return [lindex [get_record_by_id $func_id] $::idx_label]
}
proc get_title_by_idx { idx } {
	upvar ::sasfit(group_defs) groups
	if { [is_valid_idx $idx] } {
		return [lindex $groups $idx $::idx_label]
	} else {
		return $idx
	}
}
proc get_label_by_name { name } {
	upvar ::sasfit(group_defs) groups
	set idx [lsearch $groups "$name *"]
	if {$idx < 0} { return "" }
	return [lindex $groups $idx $::idx_label]
}
proc get_title { record } {
	upvar $record rec
	if { [is_func_by_rec $rec] } { return [lindex $rec $::idx_label] }
}

# returns the function index of the c-representation 
# by use of the function index in the tcl list
proc get_func_id_by_idx { idx } {
	upvar ::sasfit(group_defs) groups
	if { [is_func_by_idx $idx] } {
		return [lindex $groups $idx $::idx_plgid]
	}
}

proc get_func_id_by_name { name } {
	upvar ::sasfit(group_defs) groups
	set idx [lsearch $groups "$name *"]
	if { $idx < 0 || $idx > [llength $groups] } { return $name }
	return [get_func_id [lindex $groups $idx]]
}

proc get_func_id { rec } {
	if { [is_func_by_rec $rec] } { 
		return [lindex $rec $::idx_plgid] 
	}
}

proc get_param_names { func_id } {
	upvar ::actualAnalytPar ap
	if { ! [string is integer -strict $func_id] ||
		$func_id < 0 } { return }

	set record [get_record_by_id $func_id]
	if { [is_func_by_rec $record] } {
		return [lindex $record $::idx_param]
	}
}

# num == 0: general brief description of the function
# num > 0:  description of its parameters
# otherwise an empty string is returned
proc get_descr_by_id { func_id num } {
#	puts "get_descr_by_id: $func_id $num"
	if { ! [string is integer -strict $func_id] || $func_id < 0 ||
		! [string is integer -strict $num] || $num < 0 } { return "" }
	set record [get_record_by_id $func_id]
	if { ! [llength $record] } { return "" }
	
	if { $num == 0 } {
		return [lindex $record $::idx_descr]
	} elseif { $num <= [llength [lindex $record $::idx_param]] } {
		return [lindex $record $::idx_param [expr {$num-1}] $::idx_label]
	}
	return ""
}

proc get_function_type { record } {
	if { ! [is_func_by_rec $record] } { return "" }
	set parent [get_top_level_parent $record]
	if { ! [llength $parent] } { return "" }
	return [get_old_func_name [lindex $parent $::idx_name]]
}

proc get_old_func_name { parent_group_name } {
	switch -exact $parent_group_name {
		"form_fac" { return "FF" }
		"struct_fac" { return "SQ" }
		"size_distrib" { return "SD" }
		default { return "" }
	}
}

proc get_old_param_name { function_type } {
	switch -exact $function_type {
		"FF" { return "l" }
		"SQ" { return "s" }
		"SD" { return "a" }
		default { return "" }
	}
}

proc get_top_level_parent { record } {
	upvar ::sasfit(group_defs) groups

	# we get a list here (which should consist of only one entry)
	set parent_idx [lindex $record $::idx_parents]
	while { [llength $parent_idx] } {
		set record [lindex $groups [lindex $parent_idx 0]]
		set parent_idx [lindex $record $::idx_parents]
	}
	return $record
}

proc get_record_by_id { func_id } {
	upvar ::sasfit(group_defs) groups
	if { $func_id < 0 } { return }
	# get the function record by linear search, due to list structure atm
	foreach rec $groups {
		if { [lindex $rec $::idx_plgid] == $func_id } { return $rec }
	}
}

proc get_param_count { func_id } {
	set rec [get_record_by_id $func_id]
	if { [llength $rec] } {
		return [llength [lindex $rec $::idx_param]]
	}
}

#
# Implementation of internal helper functions for sasfit_load_plugin procedure
# [below]
#

# Reads group definition file and delivers its content to the parsing procedure
proc get_group_defs { group_files } {

	set fdata {}
	foreach group_file $group_files {
		if { ! [file exists $group_file] } { 
			puts stderr "Group definition file '$group_file' does not exist !"
			return
		}
		puts "Get group defintions from '$group_file' ..."
		set filehandler [open $group_file RDONLY]
		append fdata [read $filehandler]
		close $filehandler
	}

	return [parse_group_defs fdata]
}

# Parses a doxygen documentation file for group definitions and membership
proc parse_group_defs { data_in } {
	upvar $data_in fdata

	set allLines [split $fdata "\n"]
	set i 0

	set groups {}
	while 1 {
		search_comment_block allLines i
		set defgrp [find_word_down 1 allLines {\defgroup } i]
		if { ! [llength $defgrp] } {
			# search all comment blocks for a defgroup statement
			if { $i < [llength $allLines] } { 
				continue
			} else { 
				if { ! [llength $groups] } {
					puts stderr "Group definition not found !"
				}
				break 
			}
		}
		# add the group, 
		# 1. element is the unique name,
		# 2. element is an arbitrary title,
		# 3. element are parent indices,
		# 4. element are children indices
		# 5. elem: a unique ID, negative for groups, positive for valid functions
		set newelem [list [lindex $defgrp 0] [lrange $defgrp 1 end] {} {} -1]

		# see if the new group already exists
		set idx [lsearch $groups "[lindex $defgrp 0] *"]
		if { $idx < 0 } {
			# not found, just add a new group
			lappend groups $newelem
		} elseif { $idx < [llength $groups] } {
			# found, replace the old by the new one
			lset groups $idx $newelem
		}

		set ingrp [find_word_down 1 allLines {\ingroup } i]
		# this group is a member in an higher level group, add indices correctly
		if { [llength $ingrp] } {
			# search the parent group
			set this_idx [expr {[llength $groups] - 1}]
			foreach parent_grp $ingrp {
				add_parent groups $this_idx $parent_grp
			}
		}
	}
	# finally, resolve group names to list indices
	set num_records [llength $groups]
	for {set rec_idx 0} {$rec_idx < $num_records} {incr rec_idx} {
		set grp_name_list [lindex $groups $rec_idx $::idx_parents]
		set grp_name_list_new {}
		foreach grp_name $grp_name_list {
			set grp_idx [lsearch $groups "$grp_name *"]
			if {$grp_idx < 0} {
				puts stderr "Referenced group not found: $grp_name !"
			} else {
				lappend grp_name_list_new $grp_idx
				add_child groups $grp_idx $rec_idx
			}
		}
		lset groups $rec_idx $::idx_parents $grp_name_list_new
	}
	return $groups
}

# Adds a new value to a sublist of a group record
proc add_to_group_field { group_defs grp_idx field value } {
	upvar $group_defs groups
	set cur_val [lindex $groups $grp_idx $field]
	lappend cur_val $value
	lset groups $grp_idx $field $cur_val
}
# set parent indices in a group
proc add_parent { group_defs grp_idx pidx } {
	upvar $group_defs groups
	add_to_group_field groups $grp_idx $::idx_parents $pidx
}
# set children indices in a parent group
proc add_child { group_defs grp_idx_lst cidx } {
	upvar $group_defs groups
	add_to_group_field groups $grp_idx_lst $::idx_children $cidx
}

proc parse_header { group_defs filename basename } {
	upvar $group_defs groups

	if { ! [file exists $filename] } { 
		puts stderr "File does not exist !"
		return
	}
	set filehandler [open $filename RDONLY]
	set fdata [read $filehandler]
	close $filehandler

	parse_func_defs groups fdata $basename
}

proc remove_from_string_list { string_list word_list } {
	upvar $string_list line

	set newlist {}
	# remove control strings
	foreach elem $line {
		foreach {word word_len} $word_list {
			if { ! [string length $elem] } { break }
			set first_i [string first $word $elem]
			if { $first_i < 0 } { continue }
			set elem [string replace $elem $first_i [expr $first_i+$word_len]]
		}
		if { [string length $elem] } {
			lappend newlist $elem
		}
	}
	set line $newlist
}

# Assumes to get a string which starts with a doxygen command and an index
# pointing at the first character after that command.
# Returns a list containing all words after that index (aka 'arguments')
# Returns an empty list on error
proc get_arg_list { input_string word_i } {
	upvar $input_string line_str

	if { $word_i < 0 } { return }

	# split into a list, beginning with given position
	set line [split [string range $line_str $word_i end] " "]

	# list of strings to remove including their length-1
	remove_from_string_list line {\\f$ 2 \\n 1 <td> 3 </td> 4}

	return $line
}

proc find_word_down { multiline list_of_lines word start_idx } {
	upvar $list_of_lines allLines
	upvar $start_idx idx

	set arg_line ""
	set word_i -1
	# search the first $word statement within the current comment block
	foreach line [lrange $allLines $idx end] {
		set cur_line [string trim $line]
		# stop if we leave the current comment block
		if { ! [string equal -length 1 $cur_line "*"] } {
			break
		}
		set cur_line [string trimleft $cur_line "* " ]
		# word already found
		if { $word_i >= 0 } {
			# following line empty, or one we don't want ?
			if { $multiline == 0 ||
			     [string length $cur_line] == 0 ||
			     [string equal -length 1 $cur_line "/"] ||
			     [string equal -length 2 $cur_line "</"] ||
			     [string equal -length 1 $cur_line "\\"]
			} {
				break
			# enable multi-line statements
			} else {
				set arg_line "$arg_line $cur_line"
			}
		} else {
			# still not found, search for the word
			set word_i [string first $word $cur_line]
			if { $word_i >= 0 } {
				set arg_line $cur_line
			}
		}
		incr idx
	}
	if { [string length $arg_line] > 0 } {
		return [get_arg_list arg_line [expr { $word_i + [string length $word] }]]
	} 
}

proc search_comment_block { list_of_lines start_idx } {
	upvar $list_of_lines allLines
	upvar $start_idx idx

	# search the beginning phrase of a comment block
	while { $idx < [llength $allLines] } {
		if { [string first "/**" [string trim [lindex $allLines $idx]]] == 0 } { 
			incr idx
			return 1
		}
		incr idx
	}
	return 0
}

# parses to content of a doxygen documented header file for name, description
# and parameters of a function used to model a form factor, structure factor 
# or size distribution
#
# Some special doxygen commmands have to occur in this order:
# \defgroup, \ingroup, \brief, \note Def(ault (Size) Distribution:), 
# \par Req(uired Parameters) and \b (before each single parameter)
# otherwise, the parsing stops and continues with the next comment block
# The first command (defgroup) must not be in the same line with the opening
# token (/**).
#
# On success, we return a list of description records for each function/group
# which consists of the following fields:
# <name> 
# <title/label> 
# <parent group index list>
# <child group index list> 
# <unique function ID> 
# <brief description> 
# < <parameter name> <parameter description> >
#
# On error, we return an empty list
#
proc parse_func_defs { group_defs data_in basename } {
	upvar $group_defs groups
	upvar $data_in fdata

	set allLines [split $fdata "\n"]
	set i 0

	set functions {}
	set no_group_found_yet 1
#	set text [time {
	while 1 {
		search_comment_block allLines i
		set defgrp [find_word_down 1 allLines {\defgroup } i]
		if { ! [llength $defgrp] } {
			# search all comment blocks for a defgroup statement
			if { $i < [llength $allLines] } { 
				continue
			} else { 
				if { $no_group_found_yet } {
					puts stderr "No Group definition not found !"
				}
				break 
			}
		}
		set ingrp [find_word_down 1 allLines {\ingroup } i]
		if { ! [llength $ingrp] } {
			if { $no_group_found_yet } {
				puts stderr "Association of group '$defgrp' not found !"
			}
			break
		}
#		set temp_idx $i
#		set toplvlgrp [find_word_down 0 allLines {\memberof} temp_idx]
#	puts stderr "toplvlgrp: '[lindex [string trim [string trim $toplvlgrp {\{\}}]]]'"
		set brief_descr [find_word_down 1 allLines {\brief } i]
		if { ! [llength $brief_descr] } {
			if { $no_group_found_yet } {
				puts stderr "Brief description of the current function not found !"
			}
			break
		}
		# get default size distribution (optional)
		set old_i $i
		set default_sd [find_word_down 0 allLines {\note Def} i]
		if { [llength default_sd] <= 0 } {
			set default_sd -1
		} else {
			set tempval [lindex $default_sd end]
			set default_sd $tempval
		}
		set i $old_i

		# get parameter descriptions
		set param_descr [find_word_down 0 allLines {\par Req} i]
		if { ! [llength $param_descr] } {
			if { $no_group_found_yet } {
				puts stderr "'\\par Req' not found for group '$defgrp'"
			}
			break
		}
		set param_list {}
		while 1 {
			set param_descr [find_word_down 1 allLines {\b } i]
			if { ! [llength $param_descr] } {
				if { ! [llength $param_list] } {
					puts stderr "Parameter description not found for group '$ingrp'"
				}
				break
			}
			lappend param_list [list [lindex $param_descr 0] [lrange $param_descr 1 end]]
		}
		if { [llength param_list] > 0 } {
			# determine parent group index
			set ingrp_idx {}
			foreach parent_grp $ingrp {
				lappend ingrp_idx [lsearch $groups "$parent_grp *"]
			}
			if { $ingrp_idx < 0 } {
				puts stderr "Referenced parent group '[lindex $ingrp 0]' does not exist !"
			} else {
				# add this function with the described format
				lappend functions [list [lindex $defgrp 0] \
					[lrange $defgrp 1 end] $ingrp_idx {} -1 $default_sd $brief_descr $param_list $basename]
				# new function/group added successfully
				set no_group_found_yet 0
			}
		}
#		puts "cur($i): [lindex allLines $i]"
	}
#	}]
#	puts "$text"

	if { $no_group_found_yet } {
		puts stderr "Parsing of function documentation not completed !"
		return
	}
#	puts $functions
	return $functions
}

