# sasfit.vfs/lib/app-sasfit/tcl/sasfit_fitparerror.tcl
#
# Copyright (c) 2008-2009, Paul Scherrer Institute (PSI)
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
#   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
#   Ingo Bressler (ingo@cs.tu-berlin.de)

# menu for showing errors of fit parameters
proc FitErrorCmd { analytpar actualap tmpactualap } {

	set ::fiterror(aap) $actualap
	set ::fiterror(taap) $tmpactualap
	set ::fiterror(ap) $analytpar
	set ::fiterror(color_bg_normal) "#d9d9d9"
	set ::fiterror(color_bg_active_column) "light blue"
	set ::fiterror(color_bg_active_row) "light green"
	upvar $analytpar ap

	# set&init some settings
	set ::fiterror(color_contrib_bg_current) gray95
	set ::fiterror(color_contrib_bg_other) gray88
	proc covar_make_font { bold } {
		set basefontname "tkconfixed"
		set family [font actual $basefontname -family]
		set slant [font actual $basefontname -slant]
		set size [font actual $basefontname -size]
		set weight [font actual $basefontname -weight]
		if {$bold} { set weight "bold" }
		set name [font create -family $family -slant $slant \
						-size $size -weight $weight \
						-underline 0 -overstrike 0]
		return $name
	}
	if {![info exists ::fiterror(font)]} { 
		set ::fiterror(font) [covar_make_font 0]
	}
	if {![info exists ::fiterror(fontbold)]} {
		set ::fiterror(fontbold) [covar_make_font 1]
	}
	

	cp_arr $actualap $tmpactualap
	if {[winfo exists .analytical.fiterror]} {destroy .analytical.fiterror}
	toplevel .analytical.fiterror
	set w .analytical.fiterror
	wm geometry $w
	wm title $w "confidence intervall of fit parameters ..."

	set taglist [bindtags .analytical.fiterror]
	lappend taglist WindowCloseTag
	bindtags .analytical.fiterror $taglist
	bind WindowCloseTag <Destroy> {
		dismissCmd
	}

	# label of columns
	frame $w.chisq
	pack $w.chisq
	label $w.chisq.disclaimer -text "Errors are related to the found local minimum.\nThese errors are not related to the experimental data!"
	label $w.chisq.txt -text "chisquare = "
	label $w.chisq.chi2 -textvariable ::stepfit(chisq)
	pack $w.chisq.disclaimer
	pack $w.chisq.txt $w.chisq.chi2 -side left
	# get the default background color
	set ::fiterror(color_bg_normal) [$w.chisq.chi2 cget -background]

	frame $w.modeltype
	pack $w.modeltype -fill x -side top -padx 10 -pady 10 
	button $w.modeltype.sd -text "parameter distribution" \
		-highlightthickness 0 -command { 
#			highlight_modeltype sd
		}
	button $w.modeltype.ff -text "form factor" \
		-highlightthickness 0 -command { 
#			highlight_modeltype ff
		}
	button $w.modeltype.sq -text "structure factor" \
		-highlightthickness 0 -command { 
#			highlight_modeltype sq
		}
#	pack $w.modeltype.sd $w.modeltype.ff $w.modeltype.sq \
#		-in $w.modeltype -side left

	frame $w.data
	pack $w.data

	label $w.helptext -text "Click on a parameter to highlight its column (left: column, right: row)"
	pack $w.helptext

	set wn [NoteBook $w.layer]
	pack $wn

	set SD_f [$wn insert 1 1  -text "size distribution"   -state normal ]
	set FF_f [$wn insert 2 2  -text "form factor"		 -state normal ]
	set SQ_f [$wn insert 3 3  -text "structure factor"	-state normal ]

	frame $w.layerl
	frame $w.layerm
	frame $w.layerr
	frame $w.tl
	frame $w.tm
	frame $w.tr

	pack $w.layerl -in $SD_f -fill y -expand 1 -padx 2m -pady 2m -side left
	pack $w.layerm -in $FF_f -fill y -expand 1 -padx 2m -pady 2m -side left
	pack $w.layerr -in $SQ_f -fill y -expand 1 -padx 2m -pady 2m -side left

	pack $w.tl -in $w.layerl -fill x -side top
	pack $w.tm -in $w.layerm -fill x -side top
	pack $w.tr -in $w.layerr -fill x -side top
	label $w.tl.text1 -text "value" -width 17
	label $w.tl.text2 -text " " -width 2
	label $w.tl.text3 -text "error" -width 17
	pack $w.tl.text3 $w.tl.text2 $w.tl.text1 -side right -fill x -in $w.tl
	label $w.tm.text1 -text "value" -width 17
	label $w.tm.text2 -text " " -width 2
	label $w.tm.text3 -text "error" -width 17
	pack $w.tm.text3 $w.tm.text2 $w.tm.text1 -side right -fill x -in $w.tm
	label $w.tr.text1 -text "value" -width 17
	label $w.tr.text2 -text " " -width 2
	label $w.tr.text3 -text "error" -width 17
	pack $w.tr.text3 $w.tr.text2 $w.tr.text1 -side right -fill x -in $w.tr
	#
	# entry fields for fit 
	#
	foreach p {a s l} type {SD SQ FF} layer {l r m} {
		for {set i 1} {$i <= [getfitap aap "$type,param_count"]} {incr i} {
			frame $w.$p$i
			pack $w.$p$i -in $w.layer$layer -fill x -side top
			label $w.$p$i.label -textvariable [format "%s%s" $tmpactualap ($type,$p$i,label)]
			label $w.$p$i.var -textvariable [format "%s%s" $tmpactualap ($type,$p$i,var)] \
				  -relief sunken -anchor w -width 17 -highlightthickness 0
			label $w.$p$i.pm -text "+-" -width 2 -highlightthickness 0
			label $w.$p$i.err -textvariable [format "%s%s" $tmpactualap ($type,$p$i,err)] \
				  -relief sunken -anchor w -width 17 -highlightthickness 0 
			pack $w.$p$i.err $w.$p$i.pm $w.$p$i.var $w.$p$i.label \
				 -side right -in $w.$p$i -padx 1m -pady 1m
		}
	}

	# buttons OK
	frame $w.buttons -relief groove -borderwidth 1
	pack $w.buttons -fill x -side bottom
	button $w.buttons.ok -text "Dismiss" -highlightthickness 0 -command dismissCmd
	pack $w.buttons.ok -in $w.buttons -padx 2m -pady 2m -side left
	$wn raise 1
	$wn compute_size

	fiterror_build_covar
	fiterror_update
}

proc dismissCmd { } {
	destroy .analytical.fiterror
	array unset ::fiterror
}

proc fiterror_active_list {} {

	set contrib_count [getfitap ap "max_SD"]
	set activeList {}
	for {set contrib 0} {$contrib < $contrib_count} {incr contrib} {
		set contriblist {}
		foreach t {SD FF SQ} {
			set pattern "${t},active"
			set active [lindex [getfitap ap $pattern] $contrib]
			set pcount [lindex [getfitap ap "${t},param_count"] $contrib]
			set paramlist {}
			for {set i 0} {$i < [llength $active] && $i < $pcount} {incr i} {
				if {[lindex $active $i]} {
					lappend paramlist $i
				}
			}
			lappend contriblist $paramlist
		}
		lappend activeList $contriblist
	}

	# reshape, TODO: let the C-code work with this !
	set alist {}
	set clen [llength $activeList]
	for {set c 0} {$c < $clen} {incr c} { ;# contributions
		set mlist [lindex $activeList $c]
		for {set m 0} {$m < [llength $mlist]} {incr m} { ;# models
			set plist [lindex $mlist $m]
			for {set p 0} {$p < [llength $plist]} {incr p} { ;# parameters
				lappend alist [list $c $m [lindex $plist $p]]
			}
		}
	}
	return [list $activeList $alist]
}

proc fiterror_build_covar {} {

	set w .analytical.fiterror
	if {![winfo exists $w]} { return }

	if {![info exists ::fiterror(aap)] || 
		![info exists ::fiterror(taap)] || 
		![info exists ::fiterror(ap)]} { return }

	set res [sasfit_covar]
	if {[lindex $res 0] != 3} { return }

	# build the matrix view

	set w $w.data
	# clear the frame first
	foreach child [winfo children $w] {
		destroy $child
	}

	set lst [fiterror_active_list]
	set activeList [lindex $lst 0]
	set alist [lindex $lst 1]

	set ::fiterror(activelist) $alist
	set rows [sasfit_covar $activeList]
	set width [llength $alist] ;# num of fitted parameters

	foreach row $rows ycoord $alist {
		set yname [join $ycoord "-"]
		set widgets {}
		set idx [expr $width-1]
		set contrib_old -1
		foreach xcoord $alist {
			set xname [join $xcoord "-"]
			set name "${xname}_${yname}"

			# draw vertical lines between contributions
			set contrib [lindex $xcoord 0]
			if {$contrib_old >= 0 && $contrib_old != $contrib} {
				lappend widgets [frame $w.frm_$name \
									-background black -height 10 -width 2]
			}
			set contrib_old $contrib

			# draw each data cell in a row
			if {$idx < [llength $row]} {
				# determine column/contrib color
				set val [lindex $row end-$idx]
				set is_diag_elem [expr ($idx + 1) == [llength $row]]
				set bgcolor [covar_background_color \
								$w $name $contrib $is_diag_elem $val]
				lappend widgets [label $w.$name -justify center -width 9 \
									-background $bgcolor -font $::fiterror(font) \
									-text [covar_format_val $val $is_diag_elem]]
			} else { ;# placeholder for empty columns
				lappend widgets "x"
			}
			incr idx -1
		}
		eval "grid $widgets"
	}
}

proc covar_contrib_is_current { contrib } {
	if {![string is integer -strict $contrib]} { return false }
	return [expr [$::nomenu index active] == $contrib]
}

# determine matrix element background color based on currently selected
# contribution and diagonal position
proc covar_background_color { w lname contrib is_diag_elem val
} {
	# return unchanged background color for other widgets than labels
	if {[winfo exists $w.$lname] && \
		![string equal [winfo class $w.$lname] "Label"]
	} {
		return [$w.$lname cget -background]
	}
	# get default background color based on current contribution
	set bgcolor $::fiterror(color_contrib_bg_other)
	if {[covar_contrib_is_current $contrib]} {
		set bgcolor $::fiterror(color_contrib_bg_current)
	}
	# get element value from label if not passed as argument
	if {![string is double -strict $val] && [winfo exists $w.$lname]} {
		set val [$w.$lname cget -text]
	}
	# set element color based on its value in range [0,1]
	if { !$is_diag_elem && [expr abs($val) <= 1]
	} {
		set bgcolor [color_shade $w $bgcolor 0 $val]
	}
	return $bgcolor
}

proc color_shade { w color channel amount
} {
    # get rgb values
    set rgb [winfo rgb [winfo parent $w] $color]
    # convert rgb values to range [0,255] (8 bit each)
    set hexcolor {}
    foreach c {0 1 2} {
        lset rgb $c [expr [lindex $rgb $c] * 255 / 65535]
        if {$c != $channel} {
            # shade the color according to element value
            lset rgb $c [expr int((1 - abs($amount)) * [lindex $rgb $c])]
        }
        # convert color to hex notation
        lappend hexcolor [format "%02X" [lindex $rgb $c]]
    }
    set color [format "\#%s" [join $hexcolor ""]]
    return $color
}

proc covar_format_val { val is_diag_elem
} {
	set res " "
	if { $is_diag_elem } {
		catch {set res [format "% 2.2e" $val] }
	} elseif { [expr abs($val) > 1] } {
		set res "n/a"
	} else {
		catch {set res [format "% 2.3f" $val] }
	}
	return $res
}

proc active_lists_equal { list1_var list2_var } {
	upvar $list1_var lst1
	upvar $list2_var lst2
	# lists are always sorted by contribution, model, parameter
	return [string equal [join [join $lst1] " "] [join [join $lst2] " "]]
}

proc fiterror_update {} {

	if {![info exists ::fiterror(aap)] || 
		![info exists ::fiterror(taap)] || 
		![info exists ::fiterror(ap)]} { return }

	set w .analytical.fiterror

	fiterror_update_widgets $w	

	set rebuild 0
	# see if errors changed
	set alambda [getfitap ap alambda]
	if {[info exists ::fiterror(alambda)] &&
		$::fiterror(alambda) != $alambda
	} {
		set rebuild 1
	}
	set ::fiterror(alambda) $alambda
	# see if list of fitted params changed
	if {! $rebuild && [info exists ::fiterror(activelist)]} {
		set lst [fiterror_active_list]
		set alist [lindex $lst 1]
		if {![active_lists_equal alist ::fiterror(activelist)]} {
			set rebuild 1
		}
	}

	if {$rebuild} {
		fiterror_build_covar
	} else {
		fiterror_update_covar $w
	}
}

proc getfitap { name pattern } {
	return [lindex [array get $::fiterror($name) $pattern] 1]
}

proc fiterror_update_widgets { w } {

	cp_arr $::fiterror(aap) $::fiterror(taap)

#	set ::fiterror(first_active_param) {}
	set contrib_current [$::nomenu index active]
	foreach p {a s l} type {SD SQ FF} {
		set param_count [getfitap taap "$type,param_count"]
		set model [get_model_id $type]
		for {set i 1} {$i <= $param_count} {incr i} {
			set active [getfitap taap "$type,$p$i,active"]
			set param [list $contrib_current $model $i]
			# remember the first active parameter for default selection
#			if {[llength $::fiterror(first_active_param)] == 0} {
#				set ::fiterror(first_active_param) [list $w.$p$i $type $i]
#			}
			foreach widget [list $w.$p$i.err $w.$p$i.label $w.$p$i.var] {
				if {$active} {
					$widget configure -state normal
					bind $widget <ButtonPress-1> "highlight_param $w.$p$i column {$param}"
					bind $widget <ButtonPress-2> "highlight_param $w.$p$i row {$param}"
					bind $widget <ButtonPress-3> "highlight_param $w.$p$i row {$param}"
				} else {
					$widget configure -state disabled
					bind $widget <ButtonPress-1> {}
					bind $widget <ButtonPress-2> {}
					bind $widget <ButtonPress-3> {}
				}
				$widget configure -background $::fiterror(color_bg_normal)
			}
		}
	}
	# restore selected parameter coloring for the selected contribution
	# -> _parameter_, not the covar element!
	foreach color {column row} {
		if {[info exists ::fiterror(selected_param_$color)]} {
			set param $::fiterror(selected_param_$color)
			set bgcolor $::fiterror(color_bg_normal)
			if {[covar_contrib_is_current [lindex $param 0]]} {
				set bgcolor $::fiterror(color_bg_active_$color)
			}
			foreach child [winfo children [lindex $param end]] {
				$child configure -background $bgcolor
			}
		}
	}
}

proc fiterror_update_covar { wname
} {
	set w $wname.data

	# change background according to currently selected contribution
	foreach child [winfo children $w] {
		set name [winfo name $child]
		$child configure -background [get_covar_background $w $name]
	}
	# change background according to selected row&column
	foreach color {column row} {
		if {![info exists ::fiterror(selected_covar_$color)]} { continue }
		set bgcolor $::fiterror(color_bg_active_$color)
		set channel [argmax [winfo rgb $w $bgcolor]]
		foreach lbl $::fiterror(selected_covar_$color) {
			if {![winfo exists $w.$lbl]} { continue }
			# shade the color which was set previously (above)
			set bgcolor [color_shade $w [$w.$lbl cget -background] $channel 0.1]
			$w.$lbl configure -background $bgcolor
		}
	}
}

# returns the background color of a diagonal cell according to the currently
# selected contribution,
# otherwise it returns the current unchanged background color otherwise
proc get_covar_background { w lname
} {
	set idx [string first "_" $lname]
	set xname [string range $lname 0 [expr $idx-1]]
	set yname [string range $lname [expr $idx+1] end]

	# do we have a diagonal cell? do nothing
	set is_diag_elem [string equal "$xname" "$yname"]

	# extract contrib number, emulate split()
	set idx [expr [string first "-" $lname] - 1]
	if {$idx < 0} { return $bg }
	set contrib [string range $lname 0 $idx]

	set bgcolor [covar_background_color $w $lname $contrib $is_diag_elem ""]
	return $bgcolor
}

proc get_model_id { str } {
	set str [string tolower $str]
	set m 0
	switch -- $str { ff { set m 1} sq { set m 2 } sd - default { set m 0 } }
	return $m
}

# reset previously selected parameter widgets
proc deselect_param { color } {
	set selected [array get ::fiterror selected_param_*]

	# check if we are at the valid contribution
	if {[llength $selected] < 2} { return }
	set contrib [lindex $selected 1 0]

	set lbl ""
	if {[llength $selected] == 2 &&
		[lindex $selected 0] == "selected_param_$color"
	} {
		set bgcolor $::fiterror(color_bg_normal)
		set lbl [lindex $selected 1 end]
	} elseif {[llength $selected] == 4} { ;# normal case, two colors selected
		set lbl [lindex $selected 1 end]
		set lbl2 [lindex $selected 3 end]
		set contrib2 [lindex $selected 3 0]
		if { $lbl == $lbl2 && $contrib == $contrib2 } { ;# special case: double selection
			# compare with the color it should have and chose the other
			set c $::fiterror(color_bg_active_$color)
			set bgcolor [lindex $selected 1 end-1]
			if {"$bgcolor" == "$c"} {
				set bgcolor [lindex $selected 3 end-1]
			}
		} else { ;# normal case
			set bgcolor $::fiterror(color_bg_normal)
			set lbl [lindex $::fiterror(selected_param_$color) end]
		}
	}
	if {[winfo exists $lbl]} {
		foreach child [winfo children $lbl] {
			$child configure -background $bgcolor
		}
	}
	array unset ::fiterror selected_param_$color
}

proc highlight_param { widget color param 
} {
	set otherc [covar_color_invert $color]
	if {[info exists ::fiterror(selected_param_$otherc)]} {
		set pprev $::fiterror(selected_param_$otherc)
		if {[covar_needs_swapping $color $param $pprev]
		} {
			highlight_param [lindex $pprev end] $color [lrange $pprev 0 2]
			set color $otherc
		}
	}
	# check if we are at the correct contribution
	if {[covar_contrib_is_current [lindex $param 0]]
	} {
		deselect_param $color
		set bgcolor $::fiterror(color_bg_active_$color)
		# set the new background
		foreach w [winfo children $widget] {
			$w configure -background $bgcolor
		}
		lappend param $bgcolor $widget
		set ::fiterror(selected_param_$color) $param
	}
	highlight_covar $color $param
}

proc deselect_covar { w color
} {
	if {![info exists ::fiterror(selected_covar_$color)]} { return }
	foreach lbl $::fiterror(selected_covar_$color) {
		if {![winfo exists $w.$lbl]} { continue }
		set bgcolor [get_covar_background $w $lbl] ;# unselected color
		set recolor 1
		if {[llength $::fiterror($lbl)] == 1} { ;# no double selection
			array unset ::fiterror $lbl
		} elseif {[llength $::fiterror($lbl)] > 1} {
			# if the (shaded) color it should have now, was backuped
			# we must not restore anything
			set thiscolor $::fiterror(color_bg_active_$color)
			set channel [argmax [winfo rgb $w $thiscolor]]
			set thiscolor [color_shade $w $bgcolor $channel 0.1]
			set idx [lsearch $::fiterror($lbl) $thiscolor]
			if {$idx >= 0} { ;# was backuped, delete that entry
				set recolor 0
				set ::fiterror($lbl) [lreplace $::fiterror($lbl) $idx $idx]
			} else {
				# not found, chose the last one and remove it (pop the stack)
				set bgcolor [lindex $::fiterror($lbl) end]
				set ::fiterror($lbl) [lreplace $::fiterror($lbl) end end]
			}
		}
		if {$recolor} {
			$w.$lbl configure -background $bgcolor
		}
		$w.$lbl configure -relief flat
	}
	array unset ::fiterror selected_covar_$color
}
# decides if highlighted column/row have to be swapped to get the overlapping
# field in the upper right half
proc covar_needs_swapping { color pcur pprev
} {
	set pcur_name [covar_get_param_label_name $pcur]
	set pprev_name [covar_get_param_label_name $pprev]
	if {![llength $pcur_name]} { return 0 }
	if {![llength $pprev_name]} { return 0 }
	if {"$color" == "row"} {
		if {[string compare $pcur_name $pprev_name] > 0} {
			return 1
		}
	} elseif {[string compare $pcur_name $pprev_name] < 0} {
		return 1
	}
	return 0
}
proc covar_color_invert { color } {
	if {"$color" == "row"} {
		return column
	}
	return row
}
proc covar_get_param_label_name { param_index_list } {
	set p1 [lrange $param_index_list 0 1]
	lappend p1 [expr [lindex $param_index_list 2]-1]
	return [join $p1 "-"]
}
# type: SD|FF|SQ, p: numerical parameter index, starts with 1
proc highlight_covar { color pcur } {
	if {![info exists ::fiterror(activelist)]} { return }
	set w .analytical.fiterror.data

	deselect_covar $w $color

	set pcur_name [covar_get_param_label_name $pcur]

	set prefix "${pcur_name}_"
	set suffix ""
	if {"$color" == "row"} {
		set prefix ""
		set suffix "_${pcur_name}"
	}
	# highlight the complete row/column 
	foreach pprev $::fiterror(activelist) {
		set pprev_name [join $pprev "-"]
		set lbl "$prefix${pprev_name}$suffix"
		if {![winfo exists $w.$lbl]} { continue }
		covar_colorize_cell $w $lbl $color
	}
}

# returns the index of the largest element in a list
proc argmax { lst } {
	set imax 0
	for {set i 0} {$i < [llength $lst]} {incr i} {
		if {[lindex $lst $i] > [lindex $lst $imax]} {
			set imax $i
		}
	}
	return $imax
}

proc covar_colorize_cell { w lbl color
} {
	covar_color_push $w $lbl $color
	set bgcolor $::fiterror(color_bg_active_$color)
	set channel [argmax [winfo rgb $w $bgcolor]]
	set bgcolor [color_shade $w [get_covar_background $w $lbl] $channel 0.1]
	$w.$lbl configure -background $bgcolor
	if {[llength $::fiterror($lbl)] > 1} {
		# an element selected multiple times is probably at intersection of
		# row and column, show it raised
		$w.$lbl configure -relief raised
	}
}

# stores previous color and row or column assignment of a covar elemnt
proc covar_color_push { w lbl color } {
	if {![winfo exists $w.$lbl]} { return }
	lappend ::fiterror($lbl) [$w.$lbl cget -background]
	lappend ::fiterror(selected_covar_$color) $lbl
}

# default selection of the first active parameter
proc covar_select_first_active {} {
	set fap $::fiterror(first_active_param)
	if {[llength $fap] == 3} { 
		highlight_param column [lindex $fap 0] [lindex $fap 1] [lindex $fap 2]
	}
}

# vim: set ts=4 sw=4 tw=0: 
