# sasfit.vfs/lib/app-sasfit/tcl/sasfit_plugin_guide.tcl
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

# provides a GUI for creation of a skeleton plugin file structure

# names for general type selection
set ::name_ff "Form Factor"
set ::name_sd "Size Distribution"
set ::name_sq "Structure Factor"
set ::name_peak "Peak"
set ::name_back_entry "<back>"

# paths to required files and directories
set ::path_plugins_src [file join $::sasfit(basedir) src plugins]
set ::path_common_src  [file join $::sasfit(basedir) src sasfit_common]
set ::path_common_file [file join $path_common_src include sasfit_common.h]
set ::path_common_plg  [file join $path_common_src include sasfit_plugin.h]

proc sasfit_plugin_guide { parent } {

	if {![source_dir_present]} {
		tk_messageBox -parent $parent -icon error -type ok \
			-title "Source directories not found !" \
			-message "The required source directories were not found !\n\nPlease extract the source package of SASfit to the current basedir '$::sasfit(basedir)' and try again."
		return
	}

	set w .plg_guide
	catch {destroy $w}
	toplevel $w
	wm title $w "Plugin Creation Guide"
	raise $w
	focus $w

	frame $w.text
	pack $w.text
	frame $w.text.gui -padx 5 -pady 5
	frame $w.text.btn
	button $w.text.btn.ok -text "Create Directory" -command "extract_data $w.text.gui"
	button $w.text.btn.done -text "Close" -command "destroy $w"
	bind $w <KeyPress-Return> "$w.text.btn.ok invoke"
	bind $w <KeyPress-Escape> "destroy $w"
	pack $w.text.gui $w.text.btn
	pack $w.text.btn.ok $w.text.btn.done -side left 
	build_plg_gui $w.text.gui
}

proc build_plg_gui { r } {
	frame $r.com
	frame $r.func -pady 10
	pack $r.com $r.func

	frame $r.com.l -padx 5
	frame $r.com.r -padx 5 -pady 5 -relief solid -borderwidth 1
	pack $r.com.l $r.com.r -side left -anchor n

	label $r.com.l.lbl -justify left -borderwidth 1 -relief solid -padx 5 -pady 5 -text \
"This interface creates a proper plugin directory structure
and generates the required files based on the information
you provide here"
	frame $r.com.l.plg_name
	frame $r.com.l.func_sel
	frame $r.com.l.n_funcs
	pack $r.com.l.lbl $r.com.l.plg_name $r.com.l.n_funcs $r.com.l.func_sel -pady 2 -anchor w

	set p $r.com.l
	label $p.plg_name.lbl -text "Name of the plugin: "
	entry $p.plg_name.ntr
	bind $p.plg_name.ntr <FocusOut> "validate_cmd $p.plg_name.ntr {} {}"
	bind $p.plg_name.ntr <KeyRelease> "validate_cmd $p.plg_name.ntr {} {}"
	pack $p.plg_name.lbl $p.plg_name.ntr -side left

	label $p.func_sel.lbl -text "Function to configure: "
	set $p.func_sel.ddl.menu [dropdownl_create $p.func_sel.ddl]
	pack $p.func_sel.lbl $p.func_sel.ddl -side left

	label $p.n_funcs.lbl -text "Number of functions: "
	spinbox $p.n_funcs.spn -increment 1 -from 1 -to 1000 -width 5 -command "func_menu_all $p %d"
	bind $p.n_funcs.spn <KeyRelease> "func_menu_all $p none"
	pack $p.n_funcs.lbl $p.n_funcs.spn -side left

	$p.n_funcs.spn set 0
	$p.n_funcs.spn invoke buttonup
	sel_ddl $p.func_sel.ddl 0

	set p $r.com.r

	label $p.imptext -text "Select plugin functions to import"
	label $p.imptext_in -text "import:"
	label $p.imptext_out -text "available:"
	listbox $p.impin -height 15 -selectmode multiple -yscrollcommand "$p.impin_yscr set" -xscrollcommand "$p.impin_xscr set"
	scrollbar $p.impin_yscr -command "$p.impin yview"
	scrollbar $p.impin_xscr -command "$p.impin xview" -orient horizontal
	listbox $p.impout -height 15 -selectmode multiple -yscrollcommand "$p.impout_yscr set" -xscrollcommand "$p.impout_xscr set"
	scrollbar $p.impout_yscr -command "$p.impout yview"
	scrollbar $p.impout_xscr -command "$p.impout xview" -orient horizontal
	foreach f [lsort -dictionary [get_all_func]] {
		$p.impout insert end $f
	}
	frame $p.impbtn
	button $p.impbtn.in -text "<-" -command "btn_in_cmd $p.impin $p.impout"
	button $p.impbtn.out -text "->" -command "btn_in_cmd $p.impout $p.impin"
	pack $p.impbtn.in $p.impbtn.out

	grid $p.imptext - -
	grid $p.imptext_in x x $p.imptext_out
	grid $p.impin $p.impin_yscr $p.impbtn $p.impout $p.impout_yscr -sticky ns
	grid $p.impin_xscr x x $p.impout_xscr -sticky ew
}

proc validate_cmd { entrypath str labelpath } {
	if {![string length $str]} {
		set str [$entrypath get]
	}
	if {![string length $str]} { return 0 }
	if {[string match "*param*name*" $entrypath] || 
	    [string match "*param*descr*" $entrypath]
	} {
		regsub -all -- {[\}\{]|</?tr>|</?td>} $str {_} str
		if {[string match "*param*name*" $entrypath]} {
			regsub -all -- {[ ]} $str {_} str
		}
	} else {
		regsub -all -- {[^A-Za-z0-9]|^[0-9]} [string tolower $str] {_} str
		if {[string match "*func*name*" $entrypath]} {
			set type_str [type_ddl_get_prefix [winfo parent [winfo parent $entrypath]].type.err.ddl]
			append type_str "_" $str
			set str $type_str
		}
	}
	if {[string length $labelpath]} {
		$labelpath configure -text $str
	} else {
		# save the current selection
		set beg -1
		set end -1
		set curpos [$entrypath index insert]
		set xv [lindex [$entrypath xview] 0]
		if {[$entrypath selection present]} {
			set beg [$entrypath index sel.first]
			set end [$entrypath index sel.last]
		}
		# rewrite the contents
		$entrypath delete 0 end
		$entrypath insert end $str
		$entrypath icursor $curpos
		$entrypath xview moveto $xv
		# restore the old selection
		if {$beg >= 0 && $end > $beg} {
			$entrypath xview $beg
			$entrypath selection range $beg $end
		}
	}
	return 1
}

proc func_menu { pframe idx } {
	set root [frame $pframe.$idx -relief solid -borderwidth 1]
	frame $root.l -padx 5
	frame $root.r -padx 5
	pack $root.l $root.r -side left -anchor n -padx 5
	
	set f $root.l
	frame $f.name
	frame $f.name_int
	frame $f.type
	pack $f.name $f.name_int $f.type -ipady 2 -anchor w

	frame $f.type.err
	frame $f.type.gui
	pack $f.type.err $f.type.gui -side left

	label $f.name.lbl -text "display name of the function: "
	entry $f.name.ntr -validate key -validatecommand "validate_cmd %W %P $f.name_int.preview"
	bind $f.name.ntr <FocusOut> "func_menu_label $pframe $idx $f.name.ntr"
	bind $f.name.ntr <KeyRelease> "func_menu_label $pframe $idx $f.name.ntr"
	pack $f.name.lbl $f.name.ntr -side left

	label $f.name_int.lbl -text "preview of internal name: "
	label $f.name_int.preview -text ""
	pack $f.name_int.lbl $f.name_int.preview -side left

	label $f.type.err.lbl -text "Error type: "
	set $f.type.err.ddl.menu [dropdownl_create $f.type.err.ddl]
	$f.type.err.ddl.menu add command -label $::name_ff -command "cmd_ddl $f.type.err.ddl 0"
	$f.type.err.ddl.menu add command -label $::name_sd -command "cmd_ddl $f.type.err.ddl 1"
	$f.type.err.ddl.menu add command -label $::name_sq -command "cmd_ddl $f.type.err.ddl 2"
	$f.type.err.ddl.menu add command -label $::name_peak -command "cmd_ddl $f.type.err.ddl 3"
	sel_ddl $f.type.err.ddl 0
	pack $f.type.err.lbl $f.type.err.ddl

	label $f.type.gui.lbl -text "Select GUI group: \n(double-click for subgroups)"
	listbox $f.type.gui.box -height 20
	$f.type.gui.box insert end $::name_back_entry
	$f.type.gui.box insert end "form_fac"
	$f.type.gui.box insert end "size_distrib"
	$f.type.gui.box insert end "struct_fac"
	bind $f.type.gui.box <Double-Button-1> "sel_gui_subtype $f.type.gui.box $f.type.gui.lbl_sel"
	bind $f.type.gui.box <Button-1> "sel_gui_type $f.type.gui.box $f.type.gui.lbl_sel"
	label $f.type.gui.lbl_sel
	pack $f.type.gui.lbl $f.type.gui.box $f.type.gui.lbl_sel
	set ::sf_plg_gui_type_lst(0) {}
	$f.type.gui.box activate 1
	$f.type.gui.box selection set 1
	$f.type.gui.lbl_sel configure -text \
		"Selected: '[get_label_by_name [$f.type.gui.box get [$f.type.gui.box index active]]]'"

	set f $root.r
	frame $f.n_param -pady 2
	frame $f.param
	pack  $f.n_param $f.param -ipady 2 -anchor w

	label $f.n_param.lbl -text "Number of parameters: "
	spinbox $f.n_param.spn -increment 1 -from 1 -to $::MAXPAR -width 3 -command "param_menu_all $f %d"
	bind $f.n_param.spn <KeyRelease> "param_menu_all $f none"
	pack $f.n_param.lbl $f.n_param.spn -side left

	$f.n_param.spn set 0
	$f.n_param.spn invoke buttonup
}

# adds the selected item of one list to the other and removes them from the
# former list.
proc btn_in_cmd { inpath outpath } {
	set inlist [$inpath get 0 end]
	set outlist [$outpath get 0 end]
	foreach idx [$outpath curselection] {
		lappend inlist [$outpath get $idx]
		set outlist [lreplace $outlist $idx $idx]
	}
	$inpath delete 0 end
	foreach elem [lsort -dictionary $inlist] { $inpath insert end $elem }
	$outpath delete 0 end
	foreach elem [lsort -dictionary $outlist] { $outpath insert end $elem }
}

proc type_ddl_get_prefix { path } {
	set lbl [$path cget -text]
	# set label of the underlying menubutton
	if {[string equal $lbl $::name_ff]} {
		return "ff"
	} elseif {[string equal $lbl $::name_sd]} {
		return "sd"
	} elseif {[string equal $lbl $::name_sq]} {
		return "sq"
	} elseif {[string equal $lbl $::name_peak]} {
		return "peaks"
	} else {
		return ""
	}
}

proc sel_gui_type { boxpath lblpath } {
	if {![string length $lblpath] || ![string length $boxpath]} { return }
	# 'get active' doesn't work here
	set idx [$boxpath nearest [expr [winfo pointery $boxpath]-[winfo rooty $boxpath]]]
	set labelstr [get_label_by_name [$boxpath get $idx]]
	if {[string length $labelstr]} {
		set labelstr "Selected: '$labelstr'"
	}
	$lblpath configure -text "$labelstr"
	$boxpath activate $idx
	if {![llength $::sf_plg_gui_type_lst($::sf_plg_gui_type_lst(current))] && $idx > 0} {
		sel_ddl [winfo parent [winfo parent $boxpath]].err.ddl [expr $idx-1]
	}
}

# ::sf_plg_gui_type_lst contains previously shown GUI groups for navigation
# through model function groups. Groups in parent level are not known
# otherwise when going back. Now, on a per function basis via array.
# see cmd_ddl_func for initialization code ...

proc sel_gui_subtype { boxpath lblpath } {
	if {![string length $lblpath] || ![string length $boxpath]} { return }
	set group_name [$boxpath get [$boxpath index active]]
	if {[string equal $group_name $::name_back_entry]} {
		set new_names [lindex $::sf_plg_gui_type_lst($::sf_plg_gui_type_lst(current)) end]
		set tmp_list [lrange $::sf_plg_gui_type_lst($::sf_plg_gui_type_lst(current)) 0 end-1]
		set ::sf_plg_gui_type_lst($::sf_plg_gui_type_lst(current)) $tmp_list
	} else {
		if {[string equal [$boxpath get 0] $::name_back_entry]} {
			set first_idx 1
		} else {
			set first_idx 0
		}
		set new_names [get_children_names_by_name $group_name]
		if {[llength $new_names]} {
			lappend ::sf_plg_gui_type_lst($::sf_plg_gui_type_lst(current)) [$boxpath get $first_idx end]
		}
	}
	if {[llength $new_names]} {
		$boxpath delete 0 end
		$boxpath insert 0 $::name_back_entry
		foreach gname $new_names {
			$boxpath insert end "$gname"
		}
	}
	$boxpath activate 0
	sel_gui_type $boxpath $lblpath
}

# direction of spinbox (up|down), parent frame of all function forms
# increases number of entries in dropdownlist, also increases number of numbered sub-frames
proc func_menu_all { root dir } {
	set path $root.func_sel.ddl
	set spn $root.n_funcs.spn
	set pframe [winfo parent [winfo parent $root]].func
	set num [$spn get]
	if { "$dir" == "down" } {
		set idx [$path.menu index end]
		if {$idx > 0} {
			# remove the last entry
			$path.menu delete end
			destroy $pframe.$idx
			sel_ddl $path end
		}
	} elseif { "$dir" == "up" } {
		# add a new entry at the back
		$path.menu add command -label "$num" -command "cmd_ddl_func $path [expr $num-1]"
		func_menu $pframe [expr $num-1]
		sel_ddl $path end
	} else {
		# delete all and rebuild the list
		$path.menu delete 0 end
		foreach slave [winfo children $pframe] { 
			destroy $slave }
		for {set i 1} {$i <= $num} {incr i} {
			$path.menu add command -label "$i" -command "cmd_ddl_func $path [expr $i-1]"
			func_menu $pframe [expr $i-1]
		}
		sel_ddl $path 0
	}
}

proc func_menu_label { pframe idx entrypath } {
	set newtext [$entrypath get]
	if {[string length $newtext]} {
		set ddlpath [winfo parent $pframe].com.l.func_sel.ddl
		$ddlpath.menu entryconfigure $idx -label $newtext

		# update the menu_button_ only if the entry which causes 
		# this is still visible
		if {[winfo ismapped $entrypath]} {
			$ddlpath configure -text $newtext
		}
	}
}

# gets path to menubutton (dropdownlist)
proc sel_ddl { path idx } {
	$path.menu activate $idx
	$path.menu invoke $idx
}

# dropdown list command
proc cmd_ddl { path idx } {
#	set idx [$path.menu index active]
	# set label of the underlying menubutton
	$path configure -text [$path.menu entrycget $idx -label]
	if {[string match "*.type.err.ddl" $path]} {
		[winfo parent [winfo parent [winfo parent $path]]].name.ntr validate
	}
	return $idx
}

proc cmd_ddl_func { path idx } {
	set func_frame "[winfo parent [winfo parent [winfo parent [winfo parent $path]]]].func"
	cmd_ddl $path $idx
	foreach slave [pack slaves $func_frame] { pack forget $slave }
	pack $func_frame.$idx
	# set current index of GUI menu entries, initialise list if non-existent
	set ::sf_plg_gui_type_lst(current) $idx
	if {![info exists ::sf_plg_gui_type_lst($idx)]} {
		set ::sf_plg_gui_type_lst($idx) {}
	}
}

proc param_menu_all { root dir } {
	set spn $root.n_param.spn
	set pframe $root.param
	set num [$spn get]
	if { "$dir" == "down" } {
		if {$num > 0} {
			# remove the last entry
			destroy $pframe.$num
		}
	} elseif { "$dir" == "up" } {
		# add a new entry at the back
		param_menu $pframe [expr $num-1]
	} else {
		# delete all and rebuild the list
		foreach slave [winfo children $pframe] { 
			destroy $slave }
		for {set i 0} {$i < $num} {incr i} {
			param_menu $pframe $i
		}
	}
}

proc param_menu { pframe idx } {
	set f [frame $pframe.$idx -borderwidth 1 -relief sunken]
	frame $f.name
	frame $f.descr
	pack $f.name $f.descr -anchor e

	label $f.name.lbl -text "[expr $idx+1]. parameter name: "
	entry $f.name.ntr
	bind $f.name.ntr <FocusOut> "validate_cmd $f.name.ntr {} {}"
	bind $f.name.ntr <KeyRelease> "validate_cmd $f.name.ntr {} {}"

	label $f.descr.lbl -text "description: "
	entry $f.descr.ntr
	bind $f.descr.ntr <FocusOut> "validate_cmd $f.descr.ntr {} {}"
	bind $f.descr.ntr <KeyRelease> "validate_cmd $f.descr.ntr {} {}"
	pack $f.name.lbl $f.name.ntr -side left
	pack $f.descr.lbl $f.descr.ntr -side left

	pack $f -pady 2
}

# tests if all required directories are present
proc source_dir_present {} {
	# required directories
	if { [file isdirectory $::path_plugins_src] &&
	     [file isdirectory $::path_common_src] &&
	     [file exists $::path_common_file] &&
	     [file exists $::path_common_plg]
	} {
		return 1
	} else {
		return 0
	}
}

# gets the root path of the menu structure
proc extract_data { p } {
	set plg_name [$p.com.l.plg_name.ntr get]
	if { ![string length $plg_name] } {
		tk_messageBox -parent $p -icon error -type ok \
			-title "No Plugin name specified !" \
			-message "Please specify a name for the plugin to create !"
		return
	}
	set dir_name [file join $::path_plugins_src $plg_name]
	set importlist {}
	foreach impf [$p.com.r.impin get 0 end] {
		lappend importlist [list $impf [get_basename_by_name $impf]]
	}
	set fl {}
	foreach func [winfo children $p.func] {
		set name [$func.l.name.ntr get]
		set name_int [$func.l.name_int.preview cget -text]
		set ingroup [$func.l.type.gui.box get [$func.l.type.gui.box index active]]
		set type [type_ddl_get_prefix $func.l.type.err.ddl]
		set pl {}
		foreach param [winfo children $func.r.param] {
			set pname [$param.name.ntr get]
			set pdescr [$param.descr.ntr get]
			lappend pl [list $pname $pdescr]
			if {![string length $pname]} { 
				tk_messageBox -parent $p -icon error -type ok \
					-title "Unnamed Parameter !" \
					-message "Please specify a name for parameter [llength $pl] in function [expr [llength $fl]+1] !"
				return
			}
		}
		set n_p [llength $pl]
		lappend fl [list $name $name_int $ingroup $type $pl]
		if {![string length $name]} {
			tk_messageBox -parent $p -icon error -type ok \
				-title "Unnamed Function !" \
				-message "Please specify a name for function [llength $fl] !"
			return
		}
	}
	set n_f [llength $fl]
	plugin_gui_create_files $p $plg_name $fl $importlist
}

proc plugin_gui_create_files { p plg_name funclist importlist } {
	set access [list RDWR CREAT]
	set dir [file join $::path_plugins_src $plg_name]
	set inc_dir [file join $dir include]
	
	if {[file isdirectory $dir]} {
		set overwrite [tk_messageBox -parent $p -icon error -type yesno -default no \
			-title "Directory already exists !" \
			-message "Directory '$dir' already exists.\n\nDelete the existing directory ?"]
		if { $overwrite == "yes" } { file delete -force "$dir"
		} else {                     return }
	}
	if {[catch {file mkdir $dir} res]} {
		tk_messageBox -parent $p -icon error -type ok \
			-title "Could not create directory !" \
			-message "Could not create plugin directory '$dir' ! Message:\n\n$res"
		return
	}
	if {[catch {file mkdir $inc_dir} res]} {
		tk_messageBox -parent $p -icon error -type ok \
			-title "Could not create directory !" \
			-message "Could not create include directory '$inc_dir' ! Message:\n\n$res"
		file delete -force "$dir"
		return
	}

	if {[catch {
		set fh [open [file join $dir "interface.c"] $access]
		puts $fh [gen_interface $funclist $importlist]
		close $fh

		set fh [open [file join $dir "CMakeLists.txt"] $access]
		puts $fh [gen_cmakelists $plg_name $funclist]
		close $fh

		set fh [open [file join $inc_dir "private.h"] $access]
		puts $fh [gen_private_header $plg_name $importlist]
		close $fh

		set fh [open [file join $inc_dir "sasfit_${plg_name}.h"] $access]
		puts $fh [gen_public_header $plg_name $funclist]
		close $fh

		foreach func $funclist {
			set fh [open [file join $dir "sasfit_[lindex $func 1].c"] $access]
			puts $fh [gen_single_function $func]
			close $fh
		}
		} res]
	} {
		tk_messageBox -parent $p -icon error -type ok \
			-title "Could not write to directory !" \
			-message "Could not write files to generated directory ! Message:\n\n$res"
		file delete -force "$dir"
		return
	}
	tk_messageBox -parent $p -icon info -type ok \
		-title "Plugin created successfully" \
		-message \
"Plugin directory written successfully !\n
To build the plugin, you need to have CMake (cmake.org) and a build environment installed (e.g. GCC, MinGW on Windows).

In a terminal window, running CMake again followed by make will build this new plugin:

> cd \"$::sasfit_basedir/build\"
> cmake ../src
> make

On Windows, you may want to run cmake this way:

> cmake ../src -G \"MSYS Makefiles\"
"
}

############################## navigation marker #########################
# gets a list of functions from GUI
proc gen_interface { fl impl } {
	if {![llength $fl]} { return }
	set str \
"/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifdef MAKE_SASFIT_PLUGIN
#include \"include/private.h\"

// functions to mark for export
SASFIT_PLUGIN_EXP_BEGIN([llength $fl])\n"
	foreach f $fl {
		append str "SASFIT_PLUGIN_EXP_ADD( [lindex $f 1] )\n"
	}
	append str \
"
SASFIT_PLUGIN_EXP_END

// import functions from other plugins
"
	if {![llength $impl]} {
		append str "SASFIT_PLUGIN_IMP_NONE\n"
	} else {
		append str "SASFIT_PLUGIN_IMP_BEGIN( [llength $impl] )\n"
		foreach impf $impl {
			append str "SASFIT_PLUGIN_IMP_ADD( [lindex $impf 0] )\n"
		}
		append str "SASFIT_PLUGIN_IMP_END\n"
	}
	append str \
"
SASFIT_PLUGIN_INTERFACE

void do_at_init(void)
{
	// insert custom init code here (optional)
}

#endif\n"
	return $str
}

############################## navigation marker #########################
proc gen_cmakelists { plg_name fl } {
	if {![string length $plg_name] || ![llength $fl]} { return }
	set func_files ""
	foreach f $fl {
		append func_files "\tsasfit_[lindex $f 1].c\n"
	}
	set str \
"cmake_minimum_required(VERSION 3.0)

# the project name
set(PRJ_NAME sasfit_$plg_name)

# required external libs which are installed on the system
set(LIBS_EXT
\tGSL
)

# source files
set(SOURCE_\${PRJ_NAME}
$func_files\tinterface.c
)

# add plugin path for finding 'Find*.cmake' files for new <LIBS_EXT>
set(REL_CMAKE_MODULE_PATH .)

#                                                       #
### no adjustments required below this line (usually) ###
#                                                       #

project(\${PRJ_NAME} C)

set(LIB_IS_PLUGIN true)

set(SRC_DIR \${\${PRJ_NAME}_SOURCE_DIR})
set(CMAKE_MODULE_PATH \"\${CMAKE_MODULE_PATH};\${SRC_DIR}/\${MODULE_PATH}\")

include(SasfitCmakeUtils)
sasfit_cmake_plugin()"
	return $str
}

############################## navigation marker #########################
proc gen_single_function { f } {
	if {![llength $f]} { return }
	set intname [lindex $f 1]
	set type [lindex $f 3]
	set str \
"/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#include \"include/private.h\"
#include <sasfit_error_$type.h>

// define shortcuts for local parameters/variables\n"

	set idx 0
	foreach p [lindex $f 4] {
		append str \
"#define [string toupper [lindex $p 0]]\tparam->p\[$idx\]\n"
		incr idx
	}
	append str \
"
scalar sasfit_${intname}(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	SASFIT_CHECK_COND1((q < 0.0), param, \"q(%lg) < 0\",q);"

	foreach p [lindex $f 4] {
		append str \
"
	SASFIT_CHECK_COND1(([string toupper [lindex $p 0]] < 0.0), param, \"[lindex $p 0](%lg) < 0\",[string toupper [lindex $p 0]]); // modify condition to your needs"
	}
	append str \
"\n
	// insert your code here
	return 0.0;
}

scalar sasfit_${intname}_f(scalar q, sasfit_param * param)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}

scalar sasfit_${intname}_v(scalar q, sasfit_param * param, int dist)
{
	SASFIT_ASSERT_PTR(param); // assert pointer param is valid

	// insert your code here
	return 0.0;
}
"
	return $str
}

############################## navigation marker #########################
proc gen_private_header { plg_name impl } {
	if {![string length $plg_name]} { return }
	set upper_name [string toupper $plg_name]
	set str \
"/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef ${upper_name}_PRIVATE_H
#define ${upper_name}_PRIVATE_H

/*
 * Header file for the implementation of the form factor itself.
 */

// optional, depends on form factor implementation
#include <gsl/gsl_math.h>
#include <gsl/gsl_sf.h>

// mandatory, no adjustments necessary
#include <sasfit_common.h>

// mandatory, no adjustments necessary
#ifdef MAKE_SASFIT_PLUGIN
  // mandatory, no adjustments necessary
  #include <sasfit_plugin.h>

  SASFIT_PLUGIN_INFO_DECL;\n\n"

	set supp_includes ""
	set idx 0
	foreach impf $impl {
		append str "  // use lookup table for [lindex $impf 0](q, param)\n"
		append str "  #define sasfit_[lindex $impf 0](q,p) SASFIT_PLUGIN_IMPORTS()->functions\[${idx}\].func((q),(p))\n"
		append str "  #define sasfit_[lindex $impf 0]_f(q,p) SASFIT_PLUGIN_IMPORTS()->functions\[${idx}\].func_f((q),(p))\n"
		append str "  #define sasfit_[lindex $impf 0]_v(q,p,d) SASFIT_PLUGIN_IMPORTS()->functions\[${idx}\].func_v((q),(p),(d))\n\n"
		append supp_includes "  #include <[lindex $impf 1].h>\n"
		incr idx
	}
	append str \
"#else\n
$supp_includes
#endif

// adjust according to the plugins name
#include \"sasfit_${plg_name}.h\"

//
// add local defines here:
// #define P0 param->p\[0\]
//

#endif // end of file\n"
	return $str
}

############################## navigation marker #########################
proc gen_public_header { plg_name fl } {
	if {![string length $plg_name] || ![llength $fl]} { return }
	set upper_name [string toupper $plg_name]
	set str \
"/*
 * Author(s) of this file:
 *   <your name> (<email address>)
 */

#ifndef SASFIT_PLUGIN_${upper_name}_H
#define SASFIT_PLUGIN_${upper_name}_H

#include <sasfit_common_shared_exports.h>

/**
 * \\file sasfit_${plg_name}.h
 * Public available functions and descriptions of the $plg_name plugin.
 */

/**
 * \\def sasfit_${plg_name}_DLLEXP
 * \\copydoc sasfit_common_DLLEXP
 */

// adjust the project name below
// *_EXPORTS is set by cmake if build as shared library
#if defined(sasfit_${plg_name}_EXPORTS)
	#ifdef sasfit_${plg_name}_DLLEXP
	#undef sasfit_${plg_name}_DLLEXP
	#endif
	#define sasfit_${plg_name}_DLLEXP SASFIT_LIB_EXPORT
#elif !defined(sasfit_${plg_name}_DLLEXP)
	// is set somewhere else for export as non-plugin
	#define sasfit_${plg_name}_DLLEXP SASFIT_LIB_IMPORT
#endif

// general information about the form factor here
// & info used in the GUI:
// - group definition & uplevel group this one is in
// - brief description
// - description of parameters in HTML table-style\n"

	foreach f $fl {
		set fname [lindex $f 0]
		set fintname [lindex $f 1]
		set ingrp [lindex $f 2]
		set type [lindex $f 3]
		append str \
"
/* ################ start $fintname ################ */
/** 
 * \\defgroup $fintname $fname
 * \\ingroup $ingrp
 *
 * \\brief \\<some brief description of $fname function\\>
 *
 * <more detailed documentation, see 'doxygen' docs>
 *\n"
		if {[string equal "[lindex $f 3]" "ff"]} {
			append str \
" * \\note Default (Size) Distribution: \\ref delta\n"
		}
		append str \
" *
 * \\par Required parameters:
 *      <table border=\"0\">"
		foreach p [lindex $f 4] {
			append str \
"<tr>
 *       <td>\\b [lindex $p 0]</td>
 *       <td>[string trim [lrange $p 1 end] \}\{]</td>
 *      </tr>"
		}
		append str "</table>
 */

/**
 * \\ingroup $fintname
 *
 * \\sa sasfit_${plg_name}.h, $ingrp
 */
sasfit_${plg_name}_DLLEXP scalar sasfit_${fintname}(scalar q, sasfit_param * p);

/**
 * \\ingroup $fintname
 *
 * \\sa sasfit_${plg_name}.h, $ingrp
 */
sasfit_${plg_name}_DLLEXP scalar sasfit_${fintname}_f(scalar q, sasfit_param * p);

/**
 * \\ingroup $fintname
 *
 * \\sa sasfit_${plg_name}.h, $ingrp
 */
sasfit_${plg_name}_DLLEXP scalar sasfit_${fintname}_v(scalar q, sasfit_param * p, int dist);
/* ################ stop $fintname ################ */\n"

	}
	append str "\n\n#endif // this file\n"
	return $str
}
