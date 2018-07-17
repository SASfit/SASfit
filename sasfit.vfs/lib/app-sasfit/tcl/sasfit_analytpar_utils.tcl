# sasfit.vfs/lib/app-sasfit/tcl/sasfit_analytpar_utils.tcl
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
#   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)
#   Ingo Bressler (ingo@cs.tu-berlin.de)


proc ap_update_param_count { analytpar type } {
	upvar $analytpar ap
	set new_count [llength [array names ap $type,*,label]]
	if { [llength [array names ap $type,param_count]] } {
		set ap($type,param_count_old) $ap($type,param_count)
	} else {
		set ap($type,param_count_old) 0
	}
	set ap($type,param_count) $new_count
}

proc ap_set_param_labels { analytpar type } {
	upvar $analytpar ap
	set p [get_old_param_name $type]
	array unset ap "$type,$p*,label"
	set i 1
	foreach param [get_param_names $ap($type,typestr)] {
		set ap($type,$p$i,label) "[lindex $param 0] ="
		incr i
	}
	if { $i == 1 } { return 0 
	} else { return 1 }
}

# return a list with $count $value
proc get_list_values {value count} {

	list templist
	for {set i 0} {$i < $count} {incr i} { lappend templist $value }
	return $templist
}

proc init_analytpar {AnalytPar isGlobal} {

	upvar $AnalytPar Par

	foreach type {SD SQ FF} {

		set Par($type,param_count)	{}
		set Par($type,err)	{}
		set Par($type,min)	{}
		set Par($type,max)	{}
		set Par($type,limits)	{}
		set Par($type,active)	{}
		set Par($type,typestr)	{}

		if { $isGlobal } {
			set Par($type,common)	{}
			set Par($type,factor)	{}
		}
	}
	set Par(SD,a)		{}
	set Par(SQ,s)		{}
	set Par(FF,l)		{}
	set Par(FF,distr)	{}
	set Par(SQ,how)		{}

	set Par(fitSDFF)	{}
	set Par(calcSDFF)	{}
	set Par(substrSDFF)	{}

	set Par(covar)		{}
	set Par(alpha)		{}

	set Par(<R^0>)	{}
	set Par(<R^1>)	{}
	set Par(<R^2>)	{}
	set Par(<R^3>)	{}
	set Par(<R^4>)	{}
	set Par(<R^5>)	{}
	set Par(<R^6>)	{}
	set Par(<R^7>)	{}
	set Par(<R^8>)	{}
	set Par(R_li)	{}
	set Par(R_lc)	{}
	set Par(R_Ac)	{}
	set Par(R_VP)	{}
	set Par(R_RG)	{}
	set Par(fp)	{}

	set Par(<sumR^0>) 0.0
	set Par(<sumR^1>) 0.0
	set Par(<sumR^2>) 0.0
	set Par(<sumR^3>) 0.0
	set Par(<sumR^4>) 0.0
	set Par(<sumR^5>) 0.0
	set Par(<sumR^6>) 0.0
	set Par(<sumR^7>) 0.0
	set Par(<sumR^8>) 0.0
	set Par(sumR_li)  0.0
	set Par(sumR_lc)  0.0
	set Par(sumR_Ac)  0.0
	set Par(sumR_VP)  0.0
	set Par(sumR_RG)  0.0
	set Par(sumfp)    0.0

	set Par(alambda)	-1.0
	set Par(error)		0
	set Par(fit)		yes
	
	# multiplication factor for changing the input
	# parameters by pressing the up or down button
        set Par(par_x_X)	1.1

	set Par(isGlobal)	0
	if { $isGlobal } {
		set Par(isGlobal)	1
		set Par(dataset)	{}
		set Par(datalabel)	{}
	}

	set Par(max_SD)		0
}

proc add_analytpar {AnalytPar isGlobal} {

	global MAXPAR
	upvar $AnalytPar Par

	foreach type {SD SQ FF} {

		lappend Par($type,param_count) 0

		# fit error of particle dimensions and scattering contrast
		lappend Par($type,err) [get_list_values 0.0 $MAXPAR]

		# fit range of l
		lappend Par($type,min) [get_list_values -1e+200 $MAXPAR]
		lappend Par($type,max) [get_list_values  1e+200 $MAXPAR]

		lappend Par($type,limits) [get_list_values  0 $MAXPAR]
		
		# 0 fixed fit parameter (l), 1 free fit parameter
		lappend Par($type,active) [get_list_values  0 $MAXPAR]

		if { $isGlobal } {

			lappend Par($type,common) [get_list_values NONE $MAXPAR]
			lappend Par($type,factor) [get_list_values 1 $MAXPAR]
		}

	}
	set_analytpar_sd Par
	set_analytpar_sq Par
	set_analytpar_ff Par

        # all active parameter of this scattering contribution
	# will be free if value is yes and fixed if value is no
	lappend Par(fitSDFF) yes
        # this scattering contribution will be skipped if value
        # is no and considerd if value is yes
	lappend Par(calcSDFF) yes
        # this scattering contribution will be substracted
        # from the experimental data if value is yes
	lappend Par(substrSDFF) no

	lappend Par(<R^0>) 0.0
	lappend Par(<R^1>) 0.0
	lappend Par(<R^2>) 0.0
	lappend Par(<R^3>) 0.0
	lappend Par(<R^4>) 0.0
	lappend Par(<R^5>) 0.0
	lappend Par(<R^6>) 0.0
	lappend Par(<R^7>) 0.0
	lappend Par(<R^8>) 0.0
	lappend Par(R_li)  0.0
	lappend Par(R_lc)  0.0
	lappend Par(R_Ac)  0.0
	lappend Par(R_VP)  0.0
	lappend Par(R_RG)  0.0
	lappend Par(fp)    0.0

	if { $isGlobal } {
		lappend Par(dataset)		$::ask4dataset_var(dataset)
		lappend Par(datalabel)		[lindex $::addsasfit(Nth,filelabel) [expr $::ask4dataset_var(dataset)-1]]
	}

	incr Par(max_SD)
}

proc set_analytpar_ff {AnalytPar} {

	upvar $AnalytPar Par

	# particle dimension in nm and scattering contrast
	# a temporary list is mandatory if the command is 'lappend' -> number of lists unkown
	lappend Par(FF,l) [get_list_values 0.0 $::MAXPAR]
	lset Par(FF,l) {end 0} 10.0
	lset Par(FF,l) {end 3} 1.0

	# 1 over this parameter the size distribution integration is done.
	# Only one of the values should be 1 and the other 0.
	lappend Par(FF,distr) [get_list_values  0 $::MAXPAR]
	lset Par(FF,distr) {end 0} 1

	# type of form factor
	lappend Par(FF,typestr) Sphere
}

proc set_analytpar_sd {AnalytPar} {

	upvar $AnalytPar Par

	# parameters of the size distribution
	lappend Par(SD,a) [get_list_values 0.0 $::MAXPAR]
	lset Par(SD,a) {end 0} 1.0
	lset Par(SD,a) {end 1} 0.05
	lset Par(SD,a) {end 2} 1.0
	lset Par(SD,a) {end 3} 10.0

	# type of size distribution
	lappend Par(SD,typestr) LogNorm
}

proc set_analytpar_sq {AnalytPar} {

	upvar $AnalytPar Par

	# parameters of the structure factor
	# a temporary list is mandatory if the command is 'lappend' -> number of lists unkown
	lappend Par(SQ,s) [get_list_values 0.0 $::MAXPAR]
	lset Par(SQ,s) {end 0} 100.0
	lset Par(SQ,s) {end 1} 0.2

	# type of structure factor
	lappend Par(SQ,typestr) None
	lappend Par(SQ,how)     0
}

proc print_analytpar {AnalytPar} {

	upvar $AnalytPar apar

	foreach type {SD SQ FF} {

		puts "Par($type,err): $apar($type,err)"
		puts "Par($type,min): $apar($type,min)"
		puts "Par($type,max): $apar($type,max)"
		puts "Par($type,limits): $apar($type,limits)"
		puts "Par($type,active): $apar($type,active)"
		if { [info exists apar($type,common)] } { puts "Par($type,common): $apar($type,common)" }
		if { [info exists apar($type,factor)] } { puts "Par($type,factor): $apar($type,factor)" }

	}

	puts "Par(FF,l): $apar(FF,l)"
	puts "Par(FF,distr): $apar(FF,distr)"
	puts "Par(FF,typestr): $apar(FF,typestr)"
	
	puts "Par(SD,a): $apar(SD,a)"
	puts "Par(SD,typestr): $apar(SD,typestr)"

	puts "Par(SQ,s): $apar(SQ,s)"
	puts "Par(SQ,typestr): $apar(SQ,typestr)"
	puts "Par(SQ,how): $apar(SQ,how)"

	puts "Par(fitSDFF): $apar(fitSDFF)"
	puts "Par(calcSDFF): $apar(calcSDFF)"
	puts "Par(substrSDFF): $apar(substrSDFF)"
	
	puts "==========================================================="
}

proc update_analytpar {AnalytPar actualAnalytPar ti} {

	upvar $AnalytPar Par
	upvar $actualAnalytPar actualPar

	foreach type {SD SQ FF} {

		foreach param_type [array names Par "$type,*"] {

			# get the name of the parameter after "[SD|SQ|FF],"
			set param_type [string range "$param_type" [expr {[string length $type] + 1}] end]

			# save the unmodified parameter name for setting of SD.a SQ.s FF.l arrays
			set param_type_orig $param_type

			# workaround for SD.a SQ.s FF.l -> [SD|SQ|FF].var
			if { [string length $param_type] == 1 } { set param_type "var" }


			# update the variable directly if it consists only of a single value
			if { [info exists actualPar($type,$param_type)] } {

				if { [llength $Par($type,$param_type)] <= $ti } {
					lappend Par($type,$param_type) $actualPar($type,$param_type)
				} else {
					lset Par($type,$param_type) $ti $actualPar($type,$param_type)
				}

			} else {
				# otherwise update only the values provided by the GUI

				foreach param_val_name [array names actualPar "$type,*$param_type"] {

					# extract the number out of the parameter name of the form 'FF,l4,distr' -> 4
					set idx [string range "$param_val_name" [expr {[string length $type] + 2}] end]
					set idx [string range "$idx" 0 [expr {[string length $idx] - [string length $param_type] - 2}] ]
					# convert it into a list index
					set idx [expr {$idx - 1}]

					lset Par($type,$param_type_orig) $ti $idx $actualPar($param_val_name)

				}
			}
		}
	}
	#puts "============================================"
#	print_analytpar Par
	#puts "--------------------------------------------"
}

proc cmd_analytpar_save { analytPar filename } {

	upvar $analytPar tmpGlobalAnalytPar
	raise .analytical
	focus .analytical
	set dirname [file dirname $filename]
	set ::sasfit(datadir) "$dirname"
	set tmpGlobalAnalytPar(file) $filename
	set result [write_sasfit_inp_file tmpGlobalAnalytPar $filename]
	if {$result} {
		tk_messageBox -type ok -icon error -parent .analytical \
			-title "Can not write file !" \
			-message "The attempt of writing the file failed !\n'$filename'\nDo you have write permissions ?"
		return 1
	}
	return 0
}

proc cmd_analytpar_save_as { analytPar } {

	upvar $analytPar tmpGlobalAnalytPar
	while 1 {
		set saveFile [tk_getSaveFile \
			-defaultextension ".par"  \
			-filetypes {{"Fit Parameter" ".par"} \
				{"All"          ".*"}}   \
			-initialdir "$::sasfit(datadir)" \
			-title "Save Fit Parameter as ..."   \
			-parent .analytical
			]
		if {![string length $saveFile]} { break }
		set result [cmd_analytpar_save tmpGlobalAnalytPar $saveFile]
		if {! $result} { break }
	}
	set_title_analytical $saveFile $tmpGlobalAnalytPar(isGlobal)
}

