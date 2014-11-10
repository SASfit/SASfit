# sasfit.vfs/lib/app-sasfit/tcl/sasfit_fitparerror.tcl
#
# Copyright (c) 2008-2014, Paul Scherrer Institute (PSI)
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
#   Ingo Bressler (ingo.bressler@bam.de)

# menu for showing errors of fit parameters
proc FitErrorCmd { analytpar actualap tmpactualap
} {
	set ::fiterror(aap) $actualap
	set ::fiterror(taap) $tmpactualap
	set ::fiterror(ap) $analytpar
	set ::fiterror(color_bg_normal) "#d9d9d9"
	set ::fiterror(color_bg_active_0) "light green"
	set ::fiterror(color_bg_active_1) "light blue"
	# threshold of correlation coefficient
	set ::fiterror(correlation_threshold) 0.5
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
	set ::fiterror(widget) $w
	wm geometry $w
	wm title $w "confidence interval of fit parameters ..."

	set taglist [bindtags .analytical.fiterror]
	lappend taglist WindowCloseTag
	bindtags .analytical.fiterror $taglist
	bind WindowCloseTag <Destroy> {
		dismissCmd
	}

	# label of columns
	frame $w.chisq
	pack $w.chisq
	label $w.chisq.disclaimer -wraplength 400 -justify left -text [join {\
		"Confidence values are given for the current local optimum under the "\
		"assumption that:\n"\
		"- the provided measurement errors are independent\n"\
		"- the provided measurement errors are normally distributed\n"\
		"- the parameters are not correlated"} ""]
	pack $w.chisq.disclaimer
	# get the default background color
	set ::fiterror(color_bg_normal) [$w.chisq.disclaimer cget -background]

	frame $w.data
	pack $w.data

	frame $w.correlated -relief sunken
	pack $w.correlated

	label $w.correlated.helptext -text \
		"Detected correlated parameter pairs:"
	pack $w.correlated.helptext
	canvas $w.correlated.canvas -highlightthickness 0 -height 70 \
								-yscrollcommand "$w.correlated.scrollbar set"
	scrollbar $w.correlated.scrollbar -command "$w.correlated.canvas yview"
	pack $w.correlated.canvas -side left -fill both -in $w.correlated
	pack $w.correlated.scrollbar -side left -fill y -in $w.correlated
	set corr_frm $w.correlated.canvas.frame
	pack [frame $corr_frm] -in $w.correlated.canvas
	$w.correlated.canvas create window 0 0 -anchor nw -window $corr_frm
	bind $corr_frm <Configure> "scrollform_resize $w.correlated.canvas"
	set ::fiterror(correlated_frame) $corr_frm
	::analytical_scroll_binds [winfo toplevel $corr_frm]

	label $w.helptext -text [join {\
		"Click on a parameter below to highlight its column or row:\n"
		"(left button: column, right button: row)"} ""]
	pack $w.helptext

	set wn [NoteBook $w.layer]
	pack $wn

	set SD_f [$wn insert 1 1  -text "size distribution" -state normal ]
	set FF_f [$wn insert 2 2  -text "form factor"	   -state normal ]
	set SQ_f [$wn insert 3 3  -text "structure factor"  -state normal ]

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

# returns lists of all active/fitted parameters in triples:
# (contribution, model, parameter)
proc fiterror_active_list {
} {
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

	foreach child [winfo children $::fiterror(correlated_frame)] {
		destroy $child }
	set w $w.data
	# clear the frame first
	foreach child [winfo children $w] { destroy $child }

	set lst [fiterror_active_list]
	set activeList [lindex $lst 0]
	set alist [lindex $lst 1]

	set ::fiterror(activelist) $alist
	set rows [sasfit_covar $activeList]
	set width [llength $alist] ;# num of fitted parameters
	set correlated_btns [list]

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
				lappend widgets [button $w.$name -justify center -width 9 \
							-relief flat -overrelief raised \
							-padx 0 -pady 0 -activebackground $bgcolor \
							-background $bgcolor -font $::fiterror(font) \
							-text [covar_format_val $val $is_diag_elem] \
							-command "deselect_param 0
								  deselect_param 1
								  highlight_param 0 {$xcoord}
								  highlight_param 1 {$ycoord}"
							]
				# create button to emphasize correlated parameter pair
				set val [expr abs($val)]
				if {[expr $val > $::fiterror(correlation_threshold)]} {
					# add the button widget and the corr.coeff. to a list
					# for sorting and packing later on
					set btn [list]
					lappend btn [covar_add_correlated_param $w $xcoord $ycoord \
									[color_shade $w white 0 $val]] ;# red base color
					lappend btn $val
					if {[winfo exists [lindex $btn 0]]} {
						lappend correlated_btns $btn
					}
				}
			} else { ;# placeholder for empty columns
				lappend widgets "x"
			}
			incr idx -1
		}
		eval "grid $widgets"
	}
	# finally, pack buttons of correlated parameter pairs
	# sorted by their corr.coeff.
	set correlated_btns [lsort -real -decreasing -index end $correlated_btns]
	foreach btn $correlated_btns {
		pack [lindex $btn 0] -fill x -in $::fiterror(correlated_frame)
	}
}

# creates a button for a given parameter pair, returns the full widget path
proc covar_add_correlated_param { w xcoord ycoord bgcolor
} {
	if {$xcoord == $ycoord} { return } ;# ignore diagonal elements
	set c0 [lindex $xcoord 0]
	set c1 [lindex $ycoord 0]
	set type0 ""; set type1 ""
	set p0 ""; set p1 ""
	foreach i {0 1 2} type {SD FF SQ} p {a l s} {
		if {$i == [lindex $xcoord 1]} { set type0 $type; set p0 $p }
		if {$i == [lindex $ycoord 1]} { set type1 $type; set p1 $p }
	}
	set model0 [lindex [getfitap ap $type0,typestr] $c0]
	set model1 [lindex [getfitap ap $type1,typestr] $c1]
	set p0 [fiterror_param_prefix $xcoord]
	set p1 [fiterror_param_prefix $ycoord]
	set param0 [getfitap taap $type0,$p0,label]
	set param0 [string trimright $param0 " ="]
	set param1 [getfitap taap $type1,$p1,label]
	set param1 [string trimright $param1 " ="]
	set w $::fiterror(correlated_frame)

	set idx [llength [winfo children $w]]
	button $w.lbl$idx -wraplength 400 -justify left -background "$bgcolor" \
		-text [join [list \
		"- $param0 of $model0 in contribution $c0 with\n" \
		"- $param1 of $model1 in contribution $c1"] ""] \
		-command "deselect_param 0
				  deselect_param 1
				  highlight_param 0 {$xcoord}
				  highlight_param 1 {$ycoord}"
	return $w.lbl$idx
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
	if {!$is_diag_elem && \
		[string is double -strict $val] && \
		[expr abs($val) <= 1]
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

proc fiterror_update {
} {
	if {![info exists ::fiterror(aap)] || 
		![info exists ::fiterror(taap)] || 
		![info exists ::fiterror(ap)]} { return }

	set w .analytical.fiterror

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
	}
	fiterror_update_widgets $w
}

proc getfitap { name pattern } {
	return [lindex [array get $::fiterror($name) $pattern] 1]
}

proc fiterror_update_widgets { w
} {
	cp_arr $::fiterror(aap) $::fiterror(taap)
	set contrib_current [$::nomenu index active]
	foreach p {a l s} type {SD FF SQ} {
		set param_count [getfitap taap "$type,param_count"]
		set model [get_model_id $type]
		for {set i 0} {$i < $param_count} {incr i} {
			set param [list $contrib_current $model $i]
			set prefix [fiterror_param_prefix $param]
			set active [getfitap taap "$type,$prefix,active"]
			foreach widget [list $w.$prefix.err $w.$prefix.label $w.$prefix.var] {
				if {$active} {
					$widget configure -state normal
					bind $widget <ButtonPress-1> "highlight_param 0 {$param}"
					bind $widget <ButtonPress-2> "highlight_param 1 {$param}"
					bind $widget <ButtonPress-3> "highlight_param 1 {$param}"
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
	set selection [list]
	# unselect all parameters incl. covar matrix
	foreach {pattern param} [array get ::fiterror selected_param_*] {
		set kind [string range $pattern end end]
		deselect_param $kind
		lappend selection $kind
		lappend selection $param
	}
	# change matrix background according to current contribution
	set w $::fiterror(widget).data
	foreach child [winfo children $w] {
		set name [winfo name $child]
		$child configure -background [get_covar_background $w $name]
	}
	# apply previous parameter selection
	foreach {kind param} $selection {
		highlight_param $kind $param
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
proc deselect_param { kind
} {
	if {![info exists ::fiterror(selected_param_$kind)]} { return }
	set param $::fiterror(selected_param_$kind)
	# unmark widgets only if at the current contrib
	set w $::fiterror(widget).[fiterror_param_prefix $param]
	if {[winfo exists $w]} {
		foreach child [winfo children $w] {
			$child configure -background $::fiterror(color_bg_normal)
		}
		$w configure -background $::fiterror(color_bg_normal)
	}
	deselect_covar $kind
	# mark the current kind unselected
	array unset ::fiterror selected_param_$kind
}

proc fiterror_param_prefix { param
} {
	foreach i {0 1 2} type {SD FF SQ} p {a l s} {
		if {$i == [lindex $param 1]} { return "$p[expr [lindex $param 2]+1]" }
	}
}

proc highlight_param { kind param
} {
	set widget $::fiterror(widget).[fiterror_param_prefix $param]
	if {[info exists ::fiterror(selected_param_$kind)]} {
		set param_prev $::fiterror(selected_param_$kind)
	}
	deselect_param $kind
	# reselect other parameter if any, fixes overlaps
	set kind_other [fiterror_invert_kind $kind]
	if {[info exists ::fiterror(selected_param_$kind_other)]} {
		set param_other $::fiterror(selected_param_$kind_other)
		deselect_param $kind_other
		highlight_param $kind_other $param_other
	}
	# prev. selection was identical, remove selection at all
	if {[info exists param_prev] &&
		[lrange $param 0 2] == [lrange $param_prev 0 2]} { return }
	# set the new background if we are at the correct contribution
	if {[covar_contrib_is_current [lindex $param 0]]} {
		foreach w [winfo children $widget] {
			$w configure -background $::fiterror(color_bg_active_$kind)
		}
		$widget configure -background $::fiterror(color_bg_active_$kind)
	}
	set ::fiterror(selected_param_$kind) $param
	highlight_covar $kind $param ""
}

proc deselect_covar { kind
} {
	if {![info exists ::fiterror(selected_covar_$kind)]} { return }
	set w $::fiterror(widget).data
	set kind_other [fiterror_invert_kind $kind]
	foreach lbl $::fiterror(selected_covar_$kind) {
		if {![winfo exists $w.$lbl]} { continue }
		if {[info exists ::fiterror(${lbl}_$kind)]} {
			# get the stored previous color, see covar_color_push()
			set bgcolor $::fiterror(${lbl}_$kind)
			$w.$lbl configure -background $bgcolor
			# purge any previous pushed color
			array unset ::fiterror "${lbl}_$kind"
			# fix the intersecting element of the other selection
			if {[info exists ::fiterror(${lbl}_${kind_other})]} {
				covar_colorize_cell $w $lbl $kind_other
			}
		}
		$w.$lbl configure -relief flat
	}
	array unset ::fiterror selected_covar_$kind
}

proc fiterror_invert_kind { kind } {
	if {"$kind" == "0"} { return 1
	}
	return 0
}

# type: SD|FF|SQ, p: numerical parameter index, starts with 1
proc highlight_covar { kind param orient
} {
	if {![info exists ::fiterror(activelist)]} { return }
	if {[info exists ::fiterror(selected_covar_$kind)]} { ;# already selected
		deselect_covar $kind
	}
	set w $::fiterror(widget).data
	set orient [covar_swap_orientation $kind $param $orient]
	set pname "[join $param "-"]"
	if {[string equal $orient row]} {
		set prefix ""
		set suffix "_${pname}"
	} else { ;# orient == column
		set prefix "${pname}_"
		set suffix ""
	}
	# highlight the complete row/column 
	foreach p $::fiterror(activelist) {
		set pname [join $p "-"]
		set lbl "$prefix${pname}$suffix"
		if {![winfo exists $w.$lbl]} { continue }
		covar_colorize_cell $w $lbl $kind
	}
}

# decides if highlighted column/row have to be swapped to get the overlapping
# field in the upper right half
proc covar_swap_orientation { kind param orient
} {
	if {![string length $orient]} { set orient row } ;# orient default
	set kind_other [fiterror_invert_kind $kind]
	if {![info exists ::fiterror(selected_param_$kind_other)] ||
		![info exists ::fiterror(selected_covar_$kind_other)]
	} { return $orient }

	set param_other $::fiterror(selected_param_$kind_other)
	set pname_other [join $param_other "-"]
	set orient_other column
	# find orientation of existing selection: row or column?
	foreach lname $::fiterror(selected_covar_$kind_other) {
		if {[string equal "$lname" "${pname_other}_$pname_other"]} {
			continue ;# ignore diagonal elements
		}
		if {[string first $pname_other $lname]} {
			# other parameter is last element: in a row
			set orient_other row
		}
	}
	if {[string compare "$param" "$param_other"] > 0} {
		set orient column
	}
	if {[string equal $orient $orient_other]} {
		# swap the other orientation
		deselect_covar $kind_other
		# swap other orientation
		if {[string equal $orient_other row]} {
			set orient_other column
		} else {
			set orient_other row
		}
		highlight_covar $kind_other $param_other $orient_other
	}
	return $orient
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

proc covar_colorize_cell { w lbl kind
} {
	covar_color_push $w $lbl $kind
	set bgcolor $::fiterror(color_bg_active_$kind)
	set channel [argmax [winfo rgb $w $bgcolor]]
	set bgcolor [color_shade $w [get_covar_background $w $lbl] $channel 0.25]
	$w.$lbl configure -background $bgcolor
	if {[info exists ::fiterror(${lbl}_0)] &&
		[info exists ::fiterror(${lbl}_1)]} {
		# an element selected multiple times is probably at intersection of
		# row and column, show it raised
		$w.$lbl configure -relief raised
	}
}

# stores previous color and row or column assignment of a covar elemnt
proc covar_color_push { w lbl kind
} {
	if {![winfo exists $w.$lbl]} { return }
	set kind_other [fiterror_invert_kind $kind]
	if {[info exists ::fiterror(${lbl}_$kind_other)]} {
		set ::fiterror(${lbl}_$kind) $::fiterror(${lbl}_$kind_other)
	} else {
		set ::fiterror(${lbl}_$kind) [$w.$lbl cget -background]
	}
	lappend ::fiterror(selected_covar_$kind) $lbl
}

# vim: set ts=4 sw=4 tw=0: 
