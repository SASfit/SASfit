# high experimental
proc assert_requisites {} {
	if {![there_is_data]} {
		tk_messageBox -type ok -icon error -title "no data" \
		  -message "There is no data to analyze !"
		return 0
	}
	if {![llength [array names ::actualAnalytPar w]]} {
		tk_messageBox -type ok -icon error -title "no settings" \
		  -message "Please select a form factor and parameter for distribution ! 
(open single-fit/simulate menu)"
		return 0 
	}
	return 1
}

proc assert_input {value type} {
	if {![string is $type -strict $value]} {
		tk_messageBox -type ok -icon error -title "errorneous input" \
			-message "Please enter data of type $type !\nOffending value: '$value'."
		return 0
	}
	return 1
}

proc sasfit_distrib_analysis {} {
	puts stderr "ingos test"
	set ::FitPrecision(int) yes
#	reset_main_menu
#	parray ::sasfit
#	clearGraph_el ::SDGraph
#	Put_Graph_el ::SDGraph {0 1 2 3 4 5} {1 2 3 1 4 6}
	if {![assert_requisites]} {return 0}

	# parsed by sasfit_sd_analysis
	set ::inv_data(param_min) 1.0
	set ::inv_data(param_max) 20.0
	set ::inv_data(param_num) 100
	set ::inv_data(param_num_auto) 0
	set ::inv_data(alpha) 500.0
	set ::inv_data(alpha_min) 2e-50
	set ::inv_data(alpha_max) 2e+50
	set ::inv_data(alpha_num) 500

	# set scattering vector boundaries
	# sets qmin, qmax and input data vector
	get_scattering_vector_boundaries ::inv_data
#	puts "input data: '$::inv_data(input)'"

	build_analysis_gui ::inv_data
}


proc build_analysis_gui {data_arr} {
	upvar $data_arr inv_data
	destroy .sdana
	set w [toplevel .sdana]
	wm title $w "parameter distribution analysis"
	label $w.head_lbl -justify center -text "select range for parameter distribution"
	
	label $w.param_min_lbl -justify right -text "minimum parameter value:"
	entry $w.param_min_val
	$w.param_min_val insert 0 $inv_data(param_min)
	set inv_data(widget_param_min) $w.param_min_val
	
	label $w.param_max_lbl -justify right -text "maximum parameter value:"
	entry $w.param_max_val
	$w.param_max_val insert 0 $inv_data(param_max)
	set inv_data(widget_param_max) $w.param_max_val

	label $w.param_num_lbl -justify right -text "parameter count:"
	entry $w.param_num_val
	$w.param_num_val insert 0 $inv_data(param_num)
	set inv_data(widget_param_num) $w.param_num_val

	checkbutton $w.param_num_mode -text "auto" -justify left \
		-command "set_auto_param_count inv_data"
	set inv_data(widget_param_num_mode) $w.param_num_mode
	$w.param_num_mode invoke
	
	label $w.alpha_lbl -justify right -text "alpha:"
	entry $w.alpha_val
	$w.alpha_val insert 0 $inv_data(alpha)
	set inv_data(widget_alpha) $w.alpha_val

	frame $w.input_data_info
	label $w.input_data_info.lbl -justify left -text "input data: "
	label $w.input_data_info.min_lbl -justify left -text "Qmin: "
	label $w.input_data_info.min -justify right -text "$inv_data(qmin)"
	label $w.input_data_info.max_lbl -justify left -text ", Qmax: "
	label $w.input_data_info.max -justify right -text "$inv_data(qmax)"
	label $w.input_data_info.cnt_lbl -justify left -text ", size: "
	label $w.input_data_info.cnt -justify right -text "[llength [lindex $inv_data(input) 0]]"
	grid $w.input_data_info.lbl $w.input_data_info.min_lbl $w.input_data_info.min \
	                            $w.input_data_info.max_lbl $w.input_data_info.max \
				    $w.input_data_info.cnt_lbl $w.input_data_info.cnt

	frame $w.eig_val_info
	label $w.eig_val_info.lbl -justify left -text "sys matrix eigenvalues: "
	label $w.eig_val_info.min_lbl -justify left -text "min: "
	label $w.eig_val_info.min -justify right -text "0"
	label $w.eig_val_info.max_lbl -justify left -text "max: "
	label $w.eig_val_info.max -justify right -text "0"
	grid $w.eig_val_info.lbl - 
	grid $w.eig_val_info.min_lbl $w.eig_val_info.min
	grid $w.eig_val_info.max_lbl $w.eig_val_info.max
	set inv_data(widget_eig_val_min) $w.eig_val_info.min
	set inv_data(widget_eig_val_max) $w.eig_val_info.max

	# distribution for matrix test/verification
	set ::ana_test_distrib {} ;# parsed by sasfit_sd_analysis
	set inv_data(mode) 0
	radiobutton $w.op_distrib -justify right -text "calculate distribution" \
				-value 0 -variable inv_data(mode) \
				-command "build_op_crit_menu inv_data"
	radiobutton $w.op_crit -justify right -text "alpha criteria" \
				-value 23 -variable inv_data(mode) \
				-command "build_op_crit_menu inv_data"
	radiobutton $w.op_crit_l -justify right -text "l-curve criterion" \
				 -value 4 -variable inv_data(mode) \
				 -command "build_op_crit_menu inv_data"
	radiobutton $w.op_test -justify right -text "test distribution" \
				-value 1 -variable inv_data(mode) \
				-command "build_op_crit_menu inv_data"
	frame $w.op_crit_frame
	set inv_data(widget_op_crit_frame) $w.op_crit_frame
	if {![llength [array names ::sasfit NR]]} {
		$w.op_test configure -state disable
	}
	button $w.run_btn -text "Run" -command "run_analysis inv_data"
	bind $w <KeyPress-Return> "$w.run_btn invoke"

	grid $w.head_lbl - -
	
	grid $w.param_min_lbl $w.param_min_val -
	grid configure $w.param_min_val -sticky w
	
	grid $w.param_max_lbl $w.param_max_val -
	grid configure $w.param_max_val -sticky w
	
	grid $w.param_num_lbl $w.param_num_val $w.param_num_mode
	grid configure $w.param_num_val -sticky w
	
	grid $w.alpha_lbl $w.alpha_val -
	grid configure $w.alpha_val -sticky w

	grid $w.op_distrib - -
	grid $w.op_crit - -
	grid $w.op_crit_frame - -
	grid $w.op_crit_l - -
	grid $w.op_test - -
	grid $w.input_data_info - -
	grid $w.eig_val_info - -
	grid $w.run_btn - -
	raise $w
	focus $w
}

proc build_op_crit_menu {data_arr} {
	upvar $data_arr inv_data
	set w $inv_data(widget_op_crit_frame)
	foreach child [winfo children $w] {
		destroy $child
	}

	$inv_data(widget_alpha) configure -state normal
	$inv_data(widget_param_min) configure -state normal
	$inv_data(widget_param_max) configure -state normal
	$inv_data(widget_alpha) configure -state normal
	if {$inv_data(mode) == 23 || $inv_data(mode) == 4} {
		label $w.min_lbl -justify right -text "minimum alpha:"
		entry $w.min -textvariable inv_data(alpha_min)
		
		label $w.max_lbl -justify right -text "maximum alpha:"
		entry $w.max -textvariable inv_data(alpha_max)

		label $w.num_lbl -justify right -text "alpha count:"
		entry $w.num -textvariable inv_data(alpha_num)

		grid $w.min_lbl $w.min
		grid $w.max_lbl $w.max
		grid $w.num_lbl $w.num
		$inv_data(widget_alpha) configure -state disabled
	}
	set_test_distrib inv_data
}

proc set_test_distrib {data_arr} {
	upvar $data_arr inv_data
	if {$inv_data(mode) == 1 && 
	    [llength [array names ::sasfit NR]]
	} {
		set ::ana_test_distrib [lindex $::sasfit(NR) 0 1]
		$inv_data(widget_param_min) delete 0 end
		$inv_data(widget_param_max) delete 0 end
		$inv_data(widget_param_min) insert 0 [lindex $::sasfit(NR) 0 0 0]
		$inv_data(widget_param_max) insert 0 [lindex $::sasfit(NR) 0 0 end]
		$inv_data(widget_param_min) configure -state disabled
		$inv_data(widget_param_max) configure -state disabled
		$inv_data(widget_alpha) configure -state disabled
	} elseif {$inv_data(mode) != 1} {
		set ::ana_test_distrib {}
	}
}

proc set_auto_param_count {data_arr} {
	upvar $data_arr inv_data

	if {$inv_data(param_num_auto)} {
		set inv_data(param_num_auto) 0
		$inv_data(widget_param_num_mode) deselect
		$inv_data(widget_param_num) configure -state normal
	} else {
		set inv_data(param_num_auto) 1
		# param distrib size is 90% of data length
		set inv_data(param_num) [llength [lindex $inv_data(input) 0]]
		$inv_data(widget_param_num) configure -state normal
		$inv_data(widget_param_num) delete 0 end
		$inv_data(widget_param_num) insert 0 $inv_data(param_num)
		$inv_data(widget_param_num) configure -state disabled
		$inv_data(widget_param_num_mode) select
	}
}

proc update_data {data_arr} {
	upvar $data_arr inv_data
	set_test_distrib inv_data
	get_scattering_vector_boundaries inv_data
	if {$inv_data(param_num_auto)} {
		set inv_data(param_num) [llength [lindex $inv_data(input) 0]]
	} else {
		set inv_data(param_num) [$inv_data(widget_param_num) get]
	}
	set inv_data(param_min) [$inv_data(widget_param_min) get]
	set inv_data(param_max) [$inv_data(widget_param_max) get]
	set inv_data(alpha)     [$inv_data(widget_alpha) get]
	if {![assert_input $inv_data(alpha_min) double]} { return 0 }
	if {![assert_input $inv_data(alpha_max) double]} { return 0 }
	if {![assert_input $inv_data(alpha_num) integer]} { return 0 }
	if {![assert_input $inv_data(param_min) double]} { return 0 }
	if {![assert_input $inv_data(param_max) double]} { return 0 }
	if {![assert_input $inv_data(param_num) integer]} { return 0 }
	if {![assert_input $inv_data(alpha) double]} { return 0 }
	return 1
}

proc update_gui {data_arr} {
	upvar $data_arr inv_data
	$inv_data(widget_param_min) delete 0 end
	$inv_data(widget_param_min) insert 0 $inv_data(param_min)
	$inv_data(widget_param_max) delete 0 end
	$inv_data(widget_param_max) insert 0 $inv_data(param_max)
	$inv_data(widget_alpha) delete 0 end
	$inv_data(widget_alpha) insert 0 $inv_data(alpha)
	if {[llength [array names inv_data sys_eig_min]]} {
		$inv_data(widget_eig_val_min) configure -text $inv_data(sys_eig_min)
	}
	if {[llength [array names inv_data sys_eig_max]]} {
		$inv_data(widget_eig_val_max) configure -text $inv_data(sys_eig_max)
	}
}

proc run_analysis {data_arr} {
	upvar $data_arr inv_data
	if {![assert_requisites]} { return 0 }
	if {![update_data inv_data]} { return 0 }

	# run the analysis
	save_AP ::tmpAnalytPar ::actualAnalytPar
	set res_data [sasfit_sd_analysis ::tmpAnalytPar $inv_data(input)]
	puts stderr "test: '$res_data' '[llength [lindex $res_data 0]]' '[llength [lindex $res_data 1]]'"

	update_gui inv_data

	if {[llength $res_data]} {
		# plot the result
		plot_result res_data inv_data
	}
}

proc plot_result {result data_arr} {
	upvar $result res
	upvar $data_arr inv_data

	set count [expr [llength $res] / 3]
	if {$inv_data(mode) == 0 && 
	    [llength [array names ::sasfit NR]] && 
	    [llength $::sasfit(NR)] 
	} {
		# show the first one (initial distribution)
		hide_all_elements ::SDGraph 1
	} else {
		# hide all in test mode and alpha criteria mode
		hide_all_elements ::SDGraph 0
	}
	if {$inv_data(mode) == 1} {
		add_graph ::SDGraph \
			[list $::sasfit(Q) $::sasfit(I) $::sasfit(DI)] \
			{0 0 0 Cyan square "intensity" "Q / nm^-1" "I(Q)"}
	}
	parray ::SDGraph

	foreach {type x_data y_data} $res {
		switch $type {
			0 {	add_graph ::SDGraph [list $x_data $y_data] \
					{0 0 0 Red circle "distribution" "param" {}}
			}
			1 {	add_graph ::SDGraph [list $x_data $y_data] \
					{0 0 0 Red circle "test intensity" "Q / nm^-1" "I(Q)"}
			}
			2 {	add_graph ::SDGraph [list $x_data $y_data] \
					{0 0 0 Blue none "derivation criterion" "alpha" {}}
			}
			3 {	add_graph ::SDGraph [list $x_data $y_data] \
					{0 0 0 Red none "quotient criterion" "alpha" {}}
			}
			4 {	add_graph ::SDGraph [list $x_data $y_data] \
					{0 0 0 Red none "l-curve criterion" "|| Ax - y ||" "|| Lx ||"}
			}
			default {}
		}
		if {$type == 0} {
			set ::SDGraph(x,type) [lindex $::sasfit(scale_types_x) 0]
			set ::SDGraph(y,type) [lindex $::sasfit(scale_types_y) 0]
		} else {
			set ::SDGraph(x,type) [lindex $::sasfit(scale_types_x) 4]
			set ::SDGraph(y,type) [lindex $::sasfit(scale_types_y) 9]
		}
	}

	RefreshGraph ::SDGraph
#	puts stderr "number of graphs: '$::SDGraph(e,element)'"
	#parray ::SDGraph
}

proc get_scattering_vector_boundaries { data_arr } {
	upvar $data_arr inv_data
#		if {$::sasfit(sim,Qmin) > 0} {    set min $::sasfit(sim,Qmin) }
#		if {$::sasfit(sim,Qmax) > $min} { set max $::sasfit(sim,Qmax) }
	set inv_data(qmin) [lindex $::sasfit(Q) 0]
	set inv_data(qmax) [lindex $::sasfit(Q) end]
	if {$::sasfit(lower,Q) > 0} {
		set inv_data(qmin) $::sasfit(lower,Q)
	}
	if {$::sasfit(upper,Q) > $inv_data(qmin)} {
		set inv_data(qmax) $::sasfit(upper,Q) 
	}
	# select data range
	set idx_start 0
	set idx_end [expr [llength $::sasfit(Q)] - 1]
	while {$idx_start < [llength $::sasfit(Q)] &&
	       $inv_data(qmin) > [lindex $::sasfit(Q) $idx_start]
	} {
		incr idx_start
	}
	while {$idx_end >= 0 &&
	       $inv_data(qmax) < [lindex $::sasfit(Q) $idx_end]
	} {
		set idx_end [expr $idx_end - 1]
	}
	set inv_data(input) [list [lrange $::sasfit(Q) $idx_start $idx_end] \
	                          [lrange $::sasfit(I) $idx_start $idx_end] \
	                          [lrange $::sasfit(DI) $idx_start $idx_end]]
}

# datalist(dl):   { <x values> <y values> <error values (optional)> }
# configlist(cl): { <show line> <show symbol> <show error> <linecolor> <symboltype> <legendtext> }
namespace eval idx {
	variable linehide 0
	variable symbolhide 1
	variable errorhide 2
	variable dashcolor 3
	variable symbol 4
	variable legendtext 5
	variable xtitle 6
	variable ytitle 7
}
proc config_avail { configlist idx } {
	if {[llength $configlist] < $idx} { return 0 }
	return 1
}
proc add_graph { graph_name dl cl } {
	upvar $graph_name graph
	switch [llength $dl] {
		2 { set res [Put_Graph_el graph [lindex $dl 0] [lindex $dl 1]] }
		3 { set res [Put_Graph_el graph [lindex $dl 0] [lindex $dl 1] [lindex $dl 2]] }
		default { return }
	}
	puts stderr "put res: '$res'"
	set elem [expr $graph(e,element) - 1]

#	set ::SDGraph(x,logscale) 0
#	set ::SDGraph(y,logscale) 0
        set ::SDGraph(y,type) y

	if {[config_avail $cl $idx::linehide]} {   lset graph(e,linehide)   $elem [inv_linehide [lindex $cl $idx::linehide]]}
	if {[config_avail $cl $idx::symbolhide]} { lset graph(e,symbolhide) $elem [lindex $cl $idx::symbolhide] }
	if {[config_avail $cl $idx::errorhide]} {  lset graph(e,errorhide)  $elem [lindex $cl $idx::errorhide] }
	if {[config_avail $cl $idx::dashcolor]} {  lset graph(e,dashcolor)  $elem [lindex $cl $idx::dashcolor] }
	if {[config_avail $cl $idx::symbol]} {     lset graph(e,symbol)     $elem [lindex $cl $idx::symbol] }
	if {[config_avail $cl $idx::legendtext]} { lset graph(l,legendtext) $elem [lindex $cl $idx::legendtext] }
	if {[config_avail $cl $idx::xtitle]} {     lset graph(x,title)      [lindex $cl $idx::xtitle] }
	if {[config_avail $cl $idx::ytitle]} {     lset graph(y,title)      [lindex $cl $idx::ytitle] }
	set graph(l,hide) 0
}
proc inv_linehide { value } {
	if { $value != 0 } { return 0 
	} else             { return 1 }
}
proc hide_all_elements { graph_name start } {
	upvar $graph_name graph
	set last_elem [expr $graph(e,element)]
	for {set i 0} {$i < $last_elem} {incr i} {
		if {$i >= $start} {
			set hide 1
		} else {
			set hide 0
		}
		lset graph(e,linehide)   $i [inv_linehide $hide]
		lset graph(e,symbolhide) $i $hide
		lset graph(e,errorhide)  $i $hide
	}
}

proc there_is_data {} {
	if {[llength [array names ::sasfit Q]] == 0 || [llength $::sasfit(Q)] == 0 ||
	    [llength [array names ::sasfit DI]] == 0 || [llength $::sasfit(DI)] == 0 ||
	    [llength [array names ::sasfit I]] == 0 || [llength $::sasfit(I)] == 0 
	}      { return 0 
	} else { return 1 }
}

proc reset_main_menu { } {
	set win .top
	foreach child [winfo children $win] {
		destroy $child
	}
	sasfit_build_menubar $win
}

