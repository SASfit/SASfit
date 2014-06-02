# sasfit.vfs/lib/app-sasfit/tcl/sasfit_main.tcl
#
# Copyright (c) 2008-2009, Paul Scherrer Institute (PSI)
#
# This file is part of SASfit.
#
# SASfit is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#`
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

#------------------------------------------------------------------------------
#                            Global Variables
# some defaults
# see sasfit-common/include/sasfit_function.h for that
set MAXPAR 40
set ::sasfit(update_menu_during_fit) 1

proc fp n1 {
format %4g $n1
}

package require BLT

# global settings for GUI elements
set ::radio_check_button_prop "-indicatoron true"
set ::check_button_color_dis "#a3a3a3"
set ::check_button_color_en  "White"
set ::fitparamguiupdate yes
if {[string equal $::tcl_platform(os) Linux]} {
	set ::check_button_color_en  "#b03060"
}
set ::show_new_file_dialog 0

# prints the contents of a namespace
proc listns {{ns ::}} {
	puts [concat "namespace" $ns ": "]
	puts "vars:"
	foreach it [info vars ${ns}::*] {
		puts $it
	}
	puts "procs:"
	foreach it [info procs ${ns}::*] {
		puts $it
	}
	puts "commands:"
	foreach it [info commands ${ns}::*] {
		puts $it
	}
	puts [concat "namespace" $ns "done."]
}

global printer_vars
global SASFitVersionNo

if { [catch {source $sasfit(tcl)/sasfit_NTprinter.tcl} result] } {
	puts stderr "error while sourcing sasfit_NTprinter.tcl: $result"
}
if { [catch {source $sasfit(tcl)/sasfit_print.tcl} result] } {
	puts stderr "error while sourcing sasfit_print.tcl: $result"
}



set sasfit(Message) "??"

set sasfit(progname) "SASfit"
if [catch {set SASFitVersionNo $sasfit(svn_rev)}] {
	set SASFitVersionNo devel
}
set sasfit(version) "version $SASFitVersionNo"
set sasfit(author) "written by Joachim Kohlbrecher (LNS,PSI)"
set sasfit(authorname) ""

set sasfit(width) [expr [winfo vrootwidth .]*7/9]
if {$sasfit(width) > 780} {
   set sasfit(width) 780
}
set sasfit(height) [expr [winfo vrootheight .]*7/9]
if {$sasfit(height) > 460} {
   set sasfit(height) 460
} 

proc set_title_analytical {analytpar_file isGlobal
} {
	set prj_file ""
	if {[info exists ::sasfit(lastProjectFile)] &&
	    [file exists $::sasfit(lastProjectFile)]
	} {
		set prj_file "[file tail $::sasfit(lastProjectFile)],"
	}
	if {[file exists $analytpar_file]
	} {
		set analytpar_file "[file tail $analytpar_file],"
	}
	if {$isGlobal} {
	wm title .analytical \
"$prj_file $analytpar_file Input Parameter, Global Fit of Analytical Form Factors"
	} else {
		wm title .analytical \
"$prj_file $analytpar_file Input Parameter, Analytical Form Factor"
	}
}

#------------------------------------------------------------------------------
#                            Menu Commands
#
proc AboutCmd { is_init } {
#^^^^^^^^^^^^^^^^^
global sasfit
toplevel .about
set w .about
wm geometry $w
wm title .about [concat $sasfit(progname) "- About"]
raise $w
focus $w
image create photo LNSLogo -file $sasfit(tcl)/images/lns-logo_e.gif
button $w.logo -image LNSLogo  -command {destroy .about}
frame $w.frame
button $w.ok -text OK -command {destroy .about}
pack $w.logo $w.frame $w.ok

frame $w.frame.left 
frame $w.frame.right
pack  $w.frame.left $w.frame.right -side left -expand yes -fill both -pady 10
image create photo SASfitLogo -file $sasfit(tcl)/images/sasfit_logo.gif
button $w.frame.left.sasfitlogo -image SASfitLogo  -command "$w.ok invoke"
bind $w <KeyPress-Return> "$w.ok invoke"
if { $is_init } {
	# close all sasfit if the startup window is exited (only OK is valid)
	bind $w <KeyPress-Escape> exit_cmd
	wm protocol $w WM_DELETE_WINDOW exit_cmd
} else {
	bind $w <KeyPress-Escape> "$w.ok invoke"
}

frame $w.frame.right.prog
frame $w.frame.right.address
frame $w.frame.right.sasfit
frame $w.frame.right.notice
frame $w.frame.right.textbox
pack $w.frame.right.prog $w.frame.right.address $w.frame.right.sasfit \
	$w.frame.right.notice $w.frame.right.textbox -ipady 5

frame $w.frame.right.prog.info -relief solid -borderwidth 2
pack $w.frame.right.prog.info
label $w.frame.right.prog.info.l1 -text $sasfit(progname) -font {Helvetica 14}
label $w.frame.right.prog.info.l2 -text $sasfit(version) -font {Helvetica 10}
pack $w.frame.right.prog.info.l1 $w.frame.right.prog.info.l2

label $w.frame.right.address.line1 -text \
"Copyright (c) 2008-2009, Paul Scherrer Institute (PSI)
Laboratory for Neutron Scattering
CH-5232 Villigen PSI, Switzerland"
entry $w.frame.right.address.url
$w.frame.right.address.url insert end "http://kur.web.psi.ch/sans1/SANSSoft/sasfit.html"
$w.frame.right.address.url configure -state readonly -relief flat -width 0 -highlightthickness 0
pack  $w.frame.right.address.line1 $w.frame.right.address.url

frame $w.frame.right.sasfit.left 
frame $w.frame.right.sasfit.right
pack $w.frame.right.sasfit.left $w.frame.right.sasfit.right -side left

label $w.frame.right.sasfit.left.writtenby -text "written by:"
label $w.frame.right.sasfit.right.author1 -text "Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)"
label $w.frame.right.sasfit.left.contribs -text "contributions since March 2008 by:"
label $w.frame.right.sasfit.right.author2 -text "Ingo Bressler (ingo.bressler@bam.de)"

pack $w.frame.right.sasfit.left.writtenby $w.frame.right.sasfit.left.contribs -anchor e
pack $w.frame.right.sasfit.right.author1 $w.frame.right.sasfit.right.author2 -anchor w


#label $w.frame.right.notice.line5 -text "SASfit has been mainly developed for users analysing data taken at PSI"

#pack $w.frame.right.notice.line5

# get license information from file
text $w.frame.right.textbox.box -yscrollcommand "$w.frame.right.textbox.sb set"
set filehandler [open [file join $sasfit(tcl) .. COPYING.txt] RDONLY]
$w.frame.right.textbox.box insert end [read $filehandler]
close $filehandler
$w.frame.right.textbox.box configure -state disabled -wrap none -width 74 -height 16
scrollbar $w.frame.right.textbox.sb -orient vertical -command "$w.frame.right.textbox.box yview"

pack $w.frame.right.textbox.box $w.frame.right.textbox.sb -fill y -side left
}


lappend auto_path $sasfit(tcl)

source $sasfit(tcl)/sasfit_analytpar_utils.tcl
source $sasfit(tcl)/sasfit_utils.tcl
source $sasfit(tcl)/sasfit_readData.tcl
source $sasfit(tcl)/sasfit_plotData.tcl
source $sasfit(tcl)/sasfit_analytical.tcl
source $sasfit(tcl)/sasfit_globalfit_analytical.tcl
source $sasfit(tcl)/sasfit_structural.tcl
source $sasfit(tcl)/sasfit_fitrange.tcl
source $sasfit(tcl)/sasfit_globalfitrange.tcl
source $sasfit(tcl)/sasfit_fitparerror.tcl
source $sasfit(tcl)/sasfit_graphlayout.tcl
source $sasfit(tcl)/sasfit_resolution.tcl
source $sasfit(tcl)/sasfit_addfiles.tcl
source $sasfit(tcl)/sasfit_g2_1.tcl
source $sasfit(tcl)/sasfit_customize.tcl
source $sasfit(tcl)/sasfit_xsect.tcl
source $::sasfit(tcl)/sasfit_plugin.tcl
source $::sasfit(tcl)/sasfit_plugin_guide.tcl
source $::sasfit(tcl)/sasfit_batch.tcl
source $::sasfit(tcl)/sasfit_OZ_solver.tcl

sasfit_load_plugins $::sasfit(plugins)

create_ASCIIData CT
set CT(InputFormat) xye
#puts "reading jet color table ..."
read_Ascii $sasfit(tcl)/jet.rgb CT noerror
set ColorMap(jet) {}
foreach r $CT(Q) g $CT(I) b $CT(DI) {
   lappend ColorMap(jet) [format "#%.2x%.2x%.2x" [expr int($r)] [expr int($g)] [expr int($b)] ] 
}
#puts ...done
#puts "reading rainbow color table ..."
read_Ascii $sasfit(tcl)/rainbow.rgb CT noerror
set ColorMap(rainbow) {}
foreach r $CT(Q) g $CT(I) b $CT(DI) {
   lappend ColorMap(rainbow) [format "#%.2x%.2x%.2x" [expr int($r)] [expr int($g)] [expr int($b)] ] 
}
#puts ...done
#puts "reading black&white color table ..."
read_Ascii $sasfit(tcl)/bw.rgb CT noerror
set ColorMap(bw) {}
foreach r $CT(Q) g $CT(I) b $CT(DI) {
   lappend ColorMap(bw) [format "#%.2x%.2x%.2x" [expr int($r)] [expr int($g)] [expr int($b)] ] 
}
#puts ...done

set FitPrecision(epsNRIQ)         1e-6
set FitPrecision(epsanisotropic)  1e-5
set FitPrecision(epsresolution)   1e-3
set FitPrecision(h)               1e-2
set FitPrecision(JMAXNRIQ)        18
set FitPrecision(JMAXanisotropic) 18
set FitPrecision(JMAXresolution)  18
set FitPrecision(SQ_or_IQ)        1
set FitPrecision(SQ_or_IQ,label)  "calculate scattering intensity"
set FitPrecision(Robertus_p)      9
set FitPrecision(iter_4_MC)       1000
set FitPrecision(int)             no

global FitPrecision

set OZ(plugin_fct_names) {"SQ oz 1" 	"SQ oz 2" 	"SQ oz 3" \
			  "SQ oz 4" 	"SQ oz 5" 	"SQ oz 6"}
set OZ(plugin_C_names) {"sq_sq_oz_1" 	"sq_sq_oz_2" 	"sq_sq_oz_3" \
			"sq_sq_oz_4" 	"sq_sq_oz_5" 	"sq_sq_oz_6"}
set OZ(closure) Percus-Yevick
set OZ(potential) "HardSphere"
set OZ(closure) "Hypernetted-Chain"
set OZ(potential) "LennardJones"
#tkcon show
#set ::tkcon_visible 1
tkcon hide
set ::tkcon_visible 0

set OZ(label) unknown
set OZ(p0) 1.0
set OZ(p1) 0.1
set OZ(p2) 1.2
set OZ(p3) 1.3
set OZ(p4) 1.4
set OZ(p5) 1.5
set OZ(p6) 1.
set OZ(p7) 1.
set OZ(p8) 1.
set OZ(p9) 1.
set OZ(p10) 1.
set OZ(p11) 1.
set OZ(p12) 1.
set OZ(p13) 1.
set OZ(p14) 1.
set OZ(p15) 1.

set OZ(result,closure) {}
set OZ(result,potential) {}
set OZ(result,p) {}
set OZ(result,phi) {}
set OZ(result,T) {}
set OZ(result,mult) {}
set OZ(result,mix) {}
set OZ(result,dr/dsigma) {}
set OZ(result,releps) {}
set OZ(result,gridlength) {}
set OZ(result,q) {}
set OZ(result,Sq) {}
set OZ(result,r) {}
set OZ(result,cr) {}
set OZ(result,gr) {}
set OZ(result,u,x) {}
set OZ(result,Ur) {}
set OZ(result,label) {}

set OZ(algorithm) "CR iteration"
#set OZ(algorithm) "GMRES"
#set OZ(phi) 0.41887902047863909846168578443727
set OZ(phi) 0.3
set OZ(T) 300
set OZ(mindimOZ) 128
set OZ(mult) 4
set OZ(mix) 0.5
set OZ(maxit) 10000
set OZ(dr/dsigma) 0.0703125
set OZ(releps) 1e-12
set OZ(gridlength) [expr int($OZ(mindimOZ)*$OZ(mult))]
set OZ(plottedgraphs) 0
set OZ(symbol_i) 0
set OZ(color_i) 0
set OZ(p0,name) diameter
set OZ(p1,name) ""
set OZ(p2,name) ""
set OZ(p3,name) ""
set OZ(p4,name) ""
set OZ(p5,name) ""
set OZ(p6,name) ""
set OZ(p7,name) ""
set OZ(p8,name) ""
set OZ(p9,name) ""
set OZ(p10,name) ""
set OZ(p11,name) ""
set OZ(p12,name) ""
set OZ(p13,name) ""
set OZ(p14,name) ""
set OZ(p15,name) ""
global OZ

set sasfit(sim,Qmin) 1e-2
set sasfit(sim,Qmax) 1
set sasfit(sim,N) 100
set sasfit(hide)      no
set sasfit(Q)         {}
set sasfit(I)         {}
set sasfit(DI)        {}
set sasfit(res)       {}
set sasfit(res,file)  {}
set sasfit(res,calc)  {}
set sasfit(graph)  "123" 
switch $::tcl_platform(platform) {
      windows {
	  set sasfit(datatypes) { { Ascii  ".*"      } \
		                  { BerSANS ".*"     } \
		                  { All    "*"       } }
      }
      unix {
	  set sasfit(datatypes) { { Ascii  ".*"         } \
		                  { BerSANS ".\\[0-9\\]*" } \
                                  { All    "*"          } }
      }
   }
#set sasfit(datatypes) { { Ascii  ".*"      } \
#                        { BerSANS ".*" } \
#                        { All    "*"         } }
set sasfit(actualdatatype) Ascii
#set sasfit(filename)       [list "$sasfit(datadir)/test.dat"]
set sasfit(filename)       {}
set sasfit(filelabel)      $sasfit(filename)

set sasfit(scale_types_x) {"x" "pow(x,2)" "pow(x,3)" "pow(x,4)" "log10(x)" "log(x)" \
                           "sqrt(x)" "1/x" "1/sqrt(x)" "arcsinh(x)" }
set sasfit(scale_types_y) {"y" "pow(y,2)" "pow(y,3)" "pow(y,4)" "y*x" "y*pow(x,2)" \
                           "y*pow(x,3)" "y*pow(x,4)" "y*pow(x,6)" "log10(y)" "log(abs(y))" \
			   "log(abs(y*x))" "log(abs(y*pow(x,2)))" "log(abs(y*pow(x,4)))"
                           "sqrt(abs(y))" "1/y" "1/sqrt(abs(y))" "arcsinh(y)" }

proc clear_sasfit_config {sasfit_arr prefix suffixlist
} {
	if {[llength $suffixlist] == 0} { return }
	upvar $sasfit_arr sf
	foreach suffix $suffixlist {
		set sf($prefix$suffix) {}
	}
}

proc sasfit_arr_op { cmd sasfit_dst sasfit_src prefix_dst prefix_src index suffixlist 
} {
	if {[llength $suffixlist] == 0} { return }
	upvar $sasfit_dst dst
	upvar $sasfit_src src
	switch $cmd {
		delete {
			foreach suffix $suffixlist {
				if {![info exists src($prefix_src$suffix)]} { continue }
				set dst($prefix_dst$suffix) \
					[lreplace $src($prefix_src$suffix) $index $index]
			}
		}
		lset {
			foreach suffix $suffixlist {
				if {![info exists src($prefix_src$suffix)] || \
				    ![info exists dst($prefix_dst$suffix)]} { continue }
				eval {$cmd dst($prefix_dst$suffix) $index $src($prefix_src$suffix)}
			}
		}
		default {
			if {$index == -1} {
				foreach suffix $suffixlist {
					if {![info exists src($prefix_src$suffix)]} { continue }
					eval {$cmd dst($prefix_dst$suffix) $src($prefix_src$suffix)}
				}
			} else {
				foreach suffix $suffixlist {
					if {![info exists src($prefix_src$suffix)]} { continue }
					set value [lindex $src($prefix_src$suffix) $index]
					eval {$cmd dst($prefix_dst$suffix) $value}
				}
			}
		}
	}
}

proc clear_sasfit_file_config { sasfit_arr prefix
} {
	upvar $sasfit_arr sf
	set sf(${prefix}widcnt) 0
	clear_sasfit_config $sasfit_arr $prefix {name divisor firstskip \
		lastskip hide Q I DI res "res,calc" "res,file" widname r1 r2 lambda Dlambda l1 \
		l2 Dd d dr_by_count dr_percent \
		dr_loglogdist dr_mindist}
}

proc dr_default {fieldname} {
	switch $fieldname {
		dr_by_count   {return 1}
		dr_percent    {return 1.0}
		dr_loglogdist {return 1}
		dr_mindist    {return 0.02}
		default       {return}
	}
}

proc dr_default_op {cmd sasfit_arr prefix} {
	upvar $sasfit_arr sf
	foreach {name} {by_count percent loglogdist mindist
	} {
		set redu_name [format "%s%s" "dr_" $name]
		set fieldname [format "%s%s" $prefix $redu_name]
		eval $cmd sf($fieldname) [list [dr_default $redu_name]]
	}
}

proc dr_set_arr {sasfit_arr prefix index data_redu_arr
} {
	upvar $sasfit_arr sf
	upvar $data_redu_arr data_redu

	foreach {name} {by_count percent loglogdist mindist
	} {
		set redu_name [format "%s%s" "dr_" $name]
		set fieldname [format "%s%s" $prefix $redu_name]
		if {[info exists sf($fieldname)] &&
		    [llength $sf($fieldname)] >= $index
		} {
			set data_redu($index,$name) \
				[lindex $sf($fieldname) [expr $index-1]]
		} else {
			set data_redu($index,$name) [dr_default $redu_name]
		}
	}
}

proc arr_append_dr { sasfit_arr prefix index
} {
	upvar $sasfit_arr sf
	foreach suffix {name divisor firstskip lastskip hide} \
	        varname {::fn ::divisor ::fskip ::lskip ::hide
	} {
		lappend sf($prefix$suffix) [lindex [array get $varname $index] 1]
	}
	foreach {name} {by_count percent loglogdist mindist} {
		set redu_name [format "%s%s" "dr_" $name]
		set fieldname [format "%s%s" $prefix $redu_name]
		if {[info exists ::data_redu($index,$name)]
		} {
			lappend sf($fieldname) $::data_redu($index,$name)
		} else {
			lappend sf($fieldname) [dr_default $redu_name]
		}
	}
}

proc equal_length_sasfit_file_config { sasfit_arr prefix
} {
	upvar $sasfit_arr sf
	foreach suffix {hide firstskip lastskip divisor Q I DI res "res,calc" "res,file" r1 r2 l1 \
	                l2 lambda Dlambda d Dd widname
	} {
		if {[llength $sf(${prefix}name)] != [llength $sf($prefix$suffix)]} {
			return 0
		}
	}
	return 1
}

proc zero_sasfit {argsasfit} {
	upvar $argsasfit sasfit
	set sasfit(hide)      no
	set sasfit(Q)         {}
	set sasfit(I)         {}
	set sasfit(DI)        {}
	set sasfit(res)       {}
	set sasfit(res,file)  {}
	set sasfit(res,calc)  {}
	set sasfit(Comment)   {}
	set sasfit(npoints)   0

	set sasfit(file,n)         0
	clear_sasfit_file_config sasfit "file,"

	set sasfit(Nth,n)              0
	set sasfit(Nth,n,nonhidden)    0
	set sasfit(Nth,actual)         0
	set sasfit(Nth,actual_optionMenu) 0    
	set sasfit(Nth,file,n)         {}
	set sasfit(Nth,filelabel)      {}
	set sasfit(Nth,hide)           {}
	clear_sasfit_file_config sasfit "Nth,file"

	set sasfit(Nth,I)              {}
	set sasfit(Nth,Q)              {}
	set sasfit(Nth,DI)             {}
	set sasfit(Nth,res)            {}
	set sasfit(Nth,Qth)            {}
	set sasfit(Nth,sub,Ith)        {}
	set sasfit(Nth,sub,res)        {}

	set sasfit(batch,jobfile) ""
	set sasfit(batch,jobfid) ""
	set sasfit(batch,jobcontents) ""
	set sasfit(batch,jobprogress) ""
	set sasfit(batch,jobprogress,line) 0
}

zero_sasfit sasfit

set sasfit(export,actualdatatype) Ascii
set sasfit(export,filename) ""
set sasfit(export,blt) ""

set resolution(r1)      28.2       ;# half width of the neutron guide (in mm)
set resolution(r2)      5.0        ;# radius of sample aperture (in mm)
set resolution(d)       7.5        ;# detector resolution (in mm)
set resolution(Dd)      7.5        ;# step size of radial average (in mm)
set resolution(lambda)  1.0        ;# wavelength (in nm)
set resolution(Dlambda) 0.1        ;# wavelength resolution Dlambda
set resolution(l1)      18000      ;# collimation length (in mm)
set resolution(l2)      18000      ;# sample detector distance (in mm)
set resolution(Q)       {}
set sasfit(I_enable)   0
set sasfit(DI_enable)  0
set sasfit(res_enable) 0
set sasfit(print,printto) File
set sasfit(print,command) "lpr -Pc2pos_2"
set sasfit(print,file)               sasfit.ps
set sasfit(print,tmpps)              sasfit.ps
set sasfit(print,tmp2ps)             sasfit2.ps
set sasfit(print,IQGraph_ps)         IQ.eps
set sasfit(print,GlobalIQGraph_ps)   GlobalIQ.eps
set sasfit(print,ResIQGraph_ps)      ResIQ.ps
set sasfit(print,SDGraph_ps)         SDGraph.ps
set sasfit(print,Moments_ps)         Moments.ps
set sasfit(print,StructParData_ps)   StructPar.ps
set sasfit(print,AnalytPar_ps)       AnalytPar.ps
set sasfit(print,GlobalAnalytPar_ps) AnalytPar.ps
set sasfit(print,units) m
set sasfit(print,landscape) yes
set sasfit(print,colormode) color
set sasfit(print,decorations) no
set sasfit(print,center) yes
set sasfit(print,maxpect) yes
set sasfit(print,ipadx1)  0
set sasfit(print,ipadx2)  0
set sasfit(print,ipady1)  0
set sasfit(print,ipady2)  0
set sasfit(print,padx1)  10
set sasfit(print,padx2)  10
set sasfit(print,pady1)  10
set sasfit(print,pady2)  10
set sasfit(print,papername)  {A0       A1      A2     A3        A4        A5  \
                              Letter   Tabloid Legal  Statement Executive Folio \
                              Quarto   10x14   A      D         C         D       E} 
set sasfit(print,paperxsize) {841      594     420    297       210       148  \
                              215.9    279.4   215.9  139.7     190.5     215.9 \
                              215.9    254     279.4  431.8     558.8     863.6   1117.6}
set sasfit(print,paperysize) {1189     841     594    420       297       210 \
                              279.4    431.8   355.6  215.9     254       330.2 \
                              274.32   355.6   215.9  279.4     431.8     558.8   863.6}
set sasfit(print,padx) 5
set sasfit(print,pady) 5
set sasfit(print,width) 148
set sasfit(print,height) 105
set sasfit(print,paperheight) 296
set sasfit(print,paperwidth) 210
set sasfit(print,blt) " "
set sasfit(lower,Q) ""
set sasfit(upper,Q) ""
set sasfit(maxIterations) 10

global ask4dataset_var
set ask4dataset_var(dataset) 1
set ask4dataset_var(N_datasets) 1
set ask4dataser_var(simorfit) simulate

cp_arr sasfit  addsasfit

create_ASCIIData ASCIIData

CreateGraphPar IQGraph
CreateGraphPar GlobalFitIQGraph
CreateGraphPar ResIQGraph
CreateGraphPar Detector2DIQGraph
set Detector2DIQGraph(ct) jet
set Detector2DIQGraph(jet)     $ColorMap(jet)
set Detector2DIQGraph(rainbow) $ColorMap(rainbow)
set Detector2DIQGraph(bw)      $ColorMap(bw)
set Detector2DIQGraph(nPix)    64
set Detector2DIQGraph(pixres)  15
set Detector2DIQGraph(SD)      18
set Detector2DIQGraph(lambda)  0.6
set Detector2DIQGraph(auto)    auto
set Detector2DIQGraph(min)     0
set Detector2DIQGraph(max)     1
set Detector2DIQGraph(scale)   log(y)

set ResIQGraph(x,logscale) 0
set ResIQGraph(y,logscale) 0
set ResIQGraph(x,type)     x
set ResIQGraph(y,type)     y
set ResIQGraph(y,title)    (I(Q)-Ith(Q))/DI(Q)

CreateGraphPar SDGraph
set SDGraph(x,logscale) 0
set SDGraph(y,logscale) 0
set SDGraph(x,type)     x 
set SDGraph(y,type)     "y*pow(x,3)"
set SDGraph(x,title) "D / nm"
set SDGraph(y,title) "N(D)"

CreateGraphPar ozSQGraph
CreateGraphPar ozgrGraph
CreateGraphPar ozcrGraph
CreateGraphPar ozhrGraph
CreateGraphPar ozgammarGraph
CreateGraphPar ozbetaUrGraph
CreateGraphPar ozBrGraph
CreateGraphPar ozyrGraph
CreateGraphPar ozfrGraph

CreateGraphPar CollectIQGraph

CreateGraphPar CollectSDGraph
set CollectSDGraph(x,logscale) 0
set CollectSDGraph(y,logscale) 0
set CollectSDGraph(x,type)     x 
set CollectSDGraph(y,type)     y 
set CollectSDGraph(x,title) "D / nm"
set CollectSDGraph(y,title) "N(D)"
set CollectSDGraph(x,logscale) 0
set CollectSDGraph(y,logscale) 0

create_AnalytPar AnalytPar
create_GlobalAnalytPar GlobalAnalytPar

set StructParData(w)          "123"
set StructParData(GraphName)  "StructParData"
set StructParData(plottype)   -1
set StructParData(RG)         "-1.0";# Guinier radius
set StructParData(DRG)        "-1.0"
set StructParData(R2G)        "-1.0"
set StructParData(DR2G)       "-1.0"
set StructParData(I0)         "-1.0";# forward scattering intensity I(Q=0)
set StructParData(DI0)        "-1.0"
set StructParData(c4)         "-1.0";# Porod constant
set StructParData(Dc4)        "-1.0"
set StructParData(c0)         "-1.0";# background
set StructParData(Dc0)        "-1.0"
set StructParData(c1)         " 0.0";# linear background
set StructParData(Dc1)        "-1.0"
set StructParData(Invariant)  "-1.0";# scattering invariant
set StructParData(DInvariant) "-1.0"
set StructParData(Qexp)       "-1.0";# scattering invariant
set StructParData(DQexp)      "-1.0"
set StructParData(Qc4)        "-1.0";# scattering invariant
set StructParData(DQc4)       "-1.0"
set StructParData(Qq0)        "-1.0";# scattering invariant
set StructParData(DQq0)       "-1.0"
set StructParData(iI)         "-1.0";# integrated intensity
set StructParData(DiI)        "-1.0"
set StructParData(iIexp)      "-1.0";# integrated intensity
set StructParData(DiIexp)     "-1.0"
set StructParData(iIc4)       "-1.0";# integrated intensity
set StructParData(DiIc4)      "-1.0"
set StructParData(iIq0)       "-1.0";# integrated intensity
set StructParData(DiIq0)      "-1.0"
set StructParData(intI)       "-1.0";# integral intensity
set StructParData(DintI)      "-1.0"
set StructParData(intIexp)    "-1.0";# integral intensity
set StructParData(DintIexp)   "-1.0"
set StructParData(intIc4)     "-1.0";# integral intensity
set StructParData(DintIc4)    "-1.0"
set StructParData(intIq0)     "-1.0";# integral intensity
set StructParData(DintIq0)    "-1.0"
set StructParData(VP)         "-1.0";# Porod volume
set StructParData(DVP)        "-1.0"
set StructParData(RP)         "-1.0";# Porod radius
set StructParData(DRP)        "-1.0"
set StructParData(S/V)        "-1.0";# specific surface
set StructParData(DS/V)       "-1.0"
set StructParData(lc)         "-1.0";# correlation length
set StructParData(Dlc)        "-1.0"
set StructParData(Ac)         "-1.0";# correlation area
set StructParData(DAc)        "-1.0"
set StructParData(li)         "-1.0";# intersection area
set StructParData(Dli)        "-1.0"
set StructParData(PorodD)     "0"
set StructParData(DPorodD)    "-1.0"
set StructParData(Qmin)       "-1.0"
set StructParData(Qmax)       "-1.0"
set StructParData(Guiniernpoints) 7
set StructParData(Guinierfirst)   1
set StructParData(Porodnpoints)  25
set StructParData(Porodfirst)    1
set StructParData(lowQText)    "Guinier approx.:\nI(Q) = I0 exp(-Rg^2*Q^2/3)"
set StructParData(largeQText)  "Porod approx.:\nI(Q) = c0+c4*Q^(D-4)"
set StructParData(a)           {0.0 0.0 0.0 0.0}
set StructParData(aname)       {c0 c1 c4 D}
set StructParData(err)         {0.0 0.0 0.0 0.0}
set StructParData(active)      {yes no yes no}
set StructParData(porodDfit)   no
set StructParData(c0fit)       yes
set StructParData(order)       descending
set StructParData(first)       1
set StructParData(npoints)     3
set StructParData(ndata)       0
set StructParData(error)       0
set StructParData(errortype)   experimental
set StructParData(typestr)     Porod
set StructParData(I0typestr)   Guinier
set StructParData(chisq)       -1.0
set StructParData(Porodchisq)  -1.0
set StructParData(I0chisq)     -1.0
set StructParData(lowQFit)     "Guinier fit results:"
set StructParData(largeQFit)   "Porod fit results:"

set G2ParData(plottype) 0
set G2ParData(DLSmodel) "cumulant analysis"
set G2ParData(DLSerror) "lin. error"
set G2ParData(Par) {{A Gamma1 Gamma2 Gamma3 B "" ""} {A p Gamma_a1 Gamma_a2 Gamma_b1 Gamma_b2 B} {A BaseLine p tau1 gamma1 tau2 gamma2}}
set G2ParData(Par1lab) A
set G2ParData(Par2lab) Gamma1
set G2ParData(Par3lab) Gamma2
set G2ParData(Par4lab) Gamma3
set G2ParData(Par5lab) B
set G2ParData(Par6lab) ""
set G2ParData(Par7lab) ""
set G2ParData(iter)    0
set G2ParData(iter_d)  0
set G2ParData(Par1)    1.0
set G2ParData(Par2)    100.0
set G2ParData(Par3)    0.0
set G2ParData(Par4)    0.0
set G2ParData(Par5)    0.0
set G2ParData(Par6)    0.0
set G2ParData(Par7)    0.0
set G2ParData(Par1err)    0.0
set G2ParData(Par2err)    0.0
set G2ParData(Par3err)    0.0
set G2ParData(Par4err)    0.0
set G2ParData(Par5err)    0.0
set G2ParData(Par6err)    0.0
set G2ParData(Par7err)    0.0
set G2ParData(par_x_X)    1.001
set G2ParData(Par1active) 1
set G2ParData(Par2active) 1
set G2ParData(Par3active) 0
set G2ParData(Par4active) 0
set G2ParData(Par5active) 0
set G2ParData(Par6active) 0
set G2ParData(Par7active) 0
set G2ParData(lambda)     632.8
set G2ParData(visc)       1.002
set G2ParData(refind)     1.332
set G2ParData(Theta)      90
set G2ParData(chisqr)     0.0
set G2ParData(R_hyd)      0.0
set G2ParData(PDI)        0.0
set G2ParData(T)          293.15
global G2ParData

set nLengthTable(lambda) 0.6
set nLengthTable(xrayenergy) 10.0
set nLengthTable(compound) D2O
set nLengthTable(density) 1.1
global nLengthTable

global f_close
set f_close "yes"


proc window_to_clipboard { window } {
  raise [winfo toplevel $window]
  update
  set emfdc [wmf open]
  set size [gdi copybits $emfdc -window $window -calc]
  eval gdi copybits $emfdc -window $window -source [ list $size ] -scale 1.01
  set emf [wmf close $emfdc]
  wmf copy $emf
  wmf delete $emf
}

#------------------------------------------------------------------------------
#                            Plot New Data
proc NewDataCmd {sasfitarray} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
upvar $sasfitarray ssasfit
global SDGraph IQGraph ResIQGraph
   clearGraph_el SDGraph
   RefreshGraph SDGraph
   clearGraph_el ResIQGraph
   RefreshGraph ResIQGraph

   clearGraph_el IQGraph
#   RefreshGraph IQGraph
   Put_Graph_el IQGraph $ssasfit(Q) $ssasfit(I) $ssasfit(DI) $ssasfit(res)
   set IQGraph(title) $ssasfit(filename)
   set IQGraph(l,legendtext) [list [file tail $ssasfit(filename)] ]
   RefreshGraph IQGraph
   set ResIQGraph(title) $ssasfit(filename)
   set SDGraph(title)    $ssasfit(filename)
   RefreshGraph ResIQGraph
   RefreshGraph SDGraph
}

proc dr {redu_arr index sf num_data list_q list_i list_di list_res list_resfile list_rescalc
} {
	upvar $redu_arr red
	upvar $sf sasfit_arr

	set decim_by_count 1
	set percentage     1.0
	set loglogdist     1
	set mindist        0.0
	if {[info exists red($index,by_count)] &&
	    [info exists red($index,percent)] &&
	    [info exists red($index,loglogdist)] &&
	    [info exists red($index,mindist)]
	} {
		if {[string is boolean -strict $red($index,by_count)]} {
			set decim_by_count $red($index,by_count)
		}
		if {[string is double -strict $red($index,percent)]} {
			set percentage     $red($index,percent)
		}
		if {[string is boolean -strict $red($index,loglogdist)]} {
			set loglogdist     $red($index,loglogdist)
		}
		if {[string is double -strict $red($index,by_count)]} {
			set mindist        $red($index,mindist)
		}
	}

	if {$mindist < 0.0} {set mindist 0.0}
	if {$percentage > 1.0} {set percentage 1.0}
	if {$percentage <= 0.0} {set percentage 0.01}
	set num [expr $num_data * $percentage]
	set increment [expr $num_data/$num]
	set tmp_q {}
	set tmp_i {}
	set tmp_di {}
	set tmp_res {}
        set tmp_resfile {}
        set tmp_rescalc {}
	set i 0.0
	set idx 0
	set xval 0.0
	set xval_old 0.0
	set yval 0.0
	set yval_old 0.0
	set dist -1.0
	set xstatus 0
	set ystatus 0
	set overall_min_dist 10.0
	while {$idx < $num_data} {
		set xval [lindex $list_q $idx]
		set yval [lindex $list_i $idx]
		if {$idx > 0} {
			if {$loglogdist} {
				set xstatus [catch {set xdiff [expr log10(abs($xval/$xval_old))]}]
				set ystatus [catch {set ydiff [expr log10(abs($yval/$yval_old))]}]
			} else {
				set xdiff [expr $xval-$xval_old]
				set ydiff [expr $yval-$yval_old]
			}
			if {$xstatus || $ystatus} {
				set dist [expr $mindist+9999.9999]
			} else {
				set dist [expr sqrt($xdiff*$xdiff + $ydiff*$ydiff)]
			}
		}
		if {$decim_by_count || $dist < 0.0 || $dist > $mindist} {
			lappend tmp_q $xval
			lappend tmp_i $yval
			lappend tmp_di [lindex $list_di $idx]
			lappend tmp_res [lindex $list_res $idx]
			lappend tmp_resfile [lindex $list_resfile $idx]
			lappend tmp_rescalc [lindex $list_rescalc $idx]
			set xval_old $xval
			set yval_old $yval
			if {$dist > 0.0 && $overall_min_dist > $dist} {set overall_min_dist $dist}
		}
		if {$decim_by_count} {
			set i [expr $i+$increment]
			set idx [expr int(round($i))]
		} else {
			incr idx
		}
	}; # end while
	set sasfit_arr(Q)        $tmp_q
	set sasfit_arr(I)        $tmp_i
	set sasfit_arr(DI)       $tmp_di
	set sasfit_arr(res)      $tmp_res
	set sasfit_arr(res,file) $tmp_resfile
	set sasfit_arr(res,calc) $tmp_rescalc
	set sasfit_arr(npoints) [llength $tmp_q]
	puts "Loaded [llength $tmp_q] of $num_data data points, min distance: $overall_min_dist."
}

#------------------------------------------------------------------------------
#                            Read Data File
proc sfFileIsProject { filename } {
	return [string equal [file extension $filename] ".sas"]
}

proc ReadFileCmd {sasfitarray args} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#
# Read small angle scattering data file. Supported formats are
# ASCII, BerSANS
#
upvar $sasfitarray ssasfit
global ASCIIData
global IQGraph ResIQGraph SDGraph CollectIQGraph CollectSDGraph \
       StructParData GlobalFitIQGraph
#
# Reading data
#
puts "reading file $ssasfit(filename) in $ssasfit(actualdatatype) format"
set error yes
if {[sfFileIsProject $ssasfit(filename)]} { return yes }
if { [file isfile $ssasfit(filename) ] } {
   switch $ssasfit(actualdatatype) {
      Ascii {
             read_Ascii $ssasfit(filename) ASCIIData
	     if {$ASCIIData(npoints) > 0} {
		dr ::data_redu 0 ssasfit $ASCIIData(npoints) $ASCIIData(Q) $ASCIIData(I) $ASCIIData(DI) $ASCIIData(res) $ASCIIData(res) $ASCIIData(res)
                set error no
                set ssasfit(I_enable)   1
                set ssasfit(DI_enable)  $ASCIIData(error)
                set ssasfit(res_enable) $ASCIIData(res_available)
		set ssasfit(res,file)   $ASCIIData(res)
             }
	    }
      BerSANS   {
             read_HMI $ssasfit(filename) HMIData
             set Q_IQ_E [HMIgetItem HMIData Counts SANSDIso y]
             set npoints [llength [lindex $Q_IQ_E 0]]
	     if {$npoints > 0} {
                set ssasfit(res,file) [lindex $Q_IQ_E 3] 
		dr ::data_redu 0 ssasfit $npoints [lindex $Q_IQ_E 0] [lindex $Q_IQ_E 1] [lindex $Q_IQ_E 2] [lindex $Q_IQ_E 3] [lindex $Q_IQ_E 3] [lindex $Q_IQ_E 3]
		set error no
                set ssasfit(I_enable)   1
                set ssasfit(DI_enable)  1
                set ssasfit(res_enable) 0
	     }
            }
	}

if {[llength $args] == 0} {
#
# SDGraph ResIQGraph and IQGraph are first deleted 
# before the new data will be added to IQGraph
#
   NewDataCmd ssasfit

#set ret [catch {sasfit_GuinierFit StructParData \
#	                  [list $sasfit(Q) $sasfit(I) $sasfit(DI)]} I0extrapol]
} else {
   switch [lindex $args 0] {
       addfile {
                  clearGraph_el GlobalFitIQGraph
                  Put_Graph_el GlobalFitIQGraph $ssasfit(Q) $ssasfit(I) $ssasfit(DI) $ssasfit(res)
                  RefreshGraph GlobalFitIQGraph
                 }
       default {}

   }
}
}
return $error
}
#------------------------------------------------------------------------------
#                            Read Data File
proc ReadClipboardCmd {sasfitarray average args} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
#
# Read small angle scattering data from clipboard. Supported formats are
# only ASCII data
#
upvar $sasfitarray ssasfit
global ASCIIData
global IQGraph ResIQGraph SDGraph CollectIQGraph CollectSDGraph \
       StructParData GlobalFitIQGraph
#
# Reading data
#
set error yes

if {[string compare $average average] == 0} {
    readANDaverage_Clipboard ASCIIData
} else {
    read_Clipboard ASCIIData
}

if {$ASCIIData(npoints) > 0} {
    set error no
    set ssasfit(Q)          $ASCIIData(Q)
    set ssasfit(I)          $ASCIIData(I)
    set ssasfit(DI)         $ASCIIData(DI)
    set ssasfit(res)        $ASCIIData(res)
    set ssasfit(res,file)   $ASCIIData(res)
    set ssasfit(npoints)    $ASCIIData(npoints)
    set ssasfit(I_enable)   1
    set ssasfit(DI_enable)  $ASCIIData(error)
    set ssasfit(res_enable) $ASCIIData(res_available)
}

if {[llength $args] == 0} {
   NewDataCmd ssasfit
} else {
   switch [lindex $args 0] {
       addfile {
                  clearGraph_el GlobalFitIQGraph
                  Put_Graph_el GlobalFitIQGraph $ssasfit(Q) $ssasfit(I) $ssasfit(DI) $ssasfit(res)
                  RefreshGraph GlobalFitIQGraph
                 }
       default {}
   }
}

return $error
}


proc PasteClipboardData2IQGraph {average} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
global sasfit  resolution
cp_arr sasfit tmpsasfit
catch {destroy .openfile}
catch {destroy .addfile}
if {[catch {set ReadSuccess [ReadClipboardCmd tmpsasfit $average]} msg]} {
   tk_messageBox -icon error \
                 -message "no valid ASCII Data in Clipboard\n$msg"
} else {
if {[string compare $ReadSuccess no] == 0} {
   set tmpsasfit(file,Q)        {}
   set tmpsasfit(file,I)        {}
   set tmpsasfit(file,DI)       {}
   set tmpsasfit(file,res)      {}
   set tmpsasfit(file,res,file) {}
   set tmpsasfit(file,res,calc) {}
   lappend tmpsasfit(file,Q)        $tmpsasfit(Q)
   lappend tmpsasfit(file,I)        $tmpsasfit(I)
   lappend tmpsasfit(file,DI)       $tmpsasfit(DI)
   lappend tmpsasfit(file,res)      $tmpsasfit(res)
   lappend tmpsasfit(file,res,file) $tmpsasfit(res)
   set tmpsasfit(file,widcnt) 0
   set tmpsasfit(file,n) 1
   set tmpsasfit(file,name)  {}
   lappend tmpsasfit(file,name) "data from clipboard"
   set tmpsasfit(file,widcnt)     0
   set tmpsasfit(file,divisor)   {1}
   set tmpsasfit(file,firstskip) {0}
   set tmpsasfit(file,lastskip)  {0}
   set tmpsasfit(file,hide)      {no}
   dr_default_op set tmpsasfit "file,"
   set tmpsasfit(file,r1)        [list $resolution(r1)]
   set tmpsasfit(file,r2)        [list $resolution(r2)]
   set tmpsasfit(file,l1)        [list $resolution(l1)]
   set tmpsasfit(file,l2)        [list $resolution(l2)]
   set tmpsasfit(file,lambda)    [list $resolution(lambda)]
   set tmpsasfit(file,Dlambda)   [list $resolution(Dlambda)]
   set tmpsasfit(file,d)         [list $resolution(d)]
   set tmpsasfit(file,Dd)        [list $resolution(Dd)]
   set tmpsasfit(file,widname)   {{.addfile.lay2 0}}
   catch {destroy .openfile}
} else {
   tk_messageBox -icon error \
                 -message "no valid ASCII Data in Clipboard"
} 
          
cp_arr tmpsasfit sasfit
NewDataCmd sasfit
}
}


#------------------------------------------------------------------------------
#                    Browser for opening data files
#
proc BrowseCmd {parent filename datatype SaveReadData} {
#^^^^^^^^^^^^^^^^^^
upvar $filename Name
upvar $datatype Type
global sasfit
set datatyp {}
foreach t $sasfit(datatypes) {
   if { [lindex $t 0] == $Type } {
      set datatyp [linsert "$datatyp" 0 "$t"]
   } else { 
      lappend datatyp "$t"
   }
}

if {[string compare $SaveReadData ReadData] == 0} { 
    set command tk_getOpenFile 
    set mult "-multiple yes"
} else {
   set command tk_getSaveFile
   set mult ""
}

set fileok 0
while { ! $fileok } {
	set tmpName [eval "$command -parent $parent $mult \
                      -title \"Select Data File\"\
                      -initialdir \"$sasfit(datadir)\" \
                      -filetypes {$datatyp} "]
	if {[llength $tmpName] <= 0} { return }
	foreach fn $tmpName {
		if {[sfFileIsProject $fn]} {
			tk_messageBox -icon error -type ok -parent $parent \
				-title "Project file selected!" \
				-message "You try to open a SASfit project file (*.sas) as raw data file."
			set fileok 0
			break
		} else {
			set fileok 1
		}
	}
}

set Name $tmpName

if {[file exists [lindex $Name 0]]} {
	set sasfit(datadir) "[file dirname [lindex $Name 0]]"
}
#foreach t $sasfit(datatypes) {
#   if {  [string compare [lindex $t 0] All] != 0 } {
#      if { [string compare -nocase [lindex $t 1] [file extension $Name] ] } {
#         set Type [lindex $t 0]
#         set sasfit(actualdatatype) $Type
#         return
#      }
#   } else { 
#      set Type [lindex $t 0]
#      set sasfit(actualdatatype) $Type
#   }
#}
set sasfit(actualdatatype) $Type
}

#------------------------------------------------------------------------------
#                  apply options for reading ASCII data
#
proc applyAsciiOptionsCmd {tmpASCIIData parent} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
	global ASCIIData
	upvar $tmpASCIIData Data
	set Data(InputFormat) [string tolower $Data(InputFormat)]
	if { !([regexp x $Data(InputFormat)] && [regexp y $Data(InputFormat)]) } {
		tk_messageBox -parent $parent \
			-message "\"file format\" must contain \"x\" and \"y\""
		return 0
	}
	if { ![string is integer -strict $Data(LineSkip)] || $Data(LineSkip) < 0 
	} {
		tk_messageBox -parent $parent \
			-message "\"lines to skip\" must be an positive integer value"
		return 0
	}
	set ASCIIData(InputFormat) $Data(InputFormat)
	set ASCIIData(Ext) $Data(Ext)
	set ASCIIData(LineSkip) $Data(LineSkip)
	set ASCIIData(in_out) $Data(in_out)
	set ASCIIData(nonneg) $Data(nonneg)
	set ASCIIData(zero_int_chkb) $Data(zero_int_chkb)
	if {[string first "e" $ASCIIData(InputFormat)] < 0} {
		tk_messageBox -type ok -icon warning -parent $parent -message \
"No error bar was supplied. Hence, it will be estimated by taking the local smoothness of the data into account."
	}
	return 1
}

#------------------------------------------------------------------------------
#                option menu for reading ASCII data files 
#
proc AsciiOptionsCmd {} {
#^^^^^^^^^^^^^^^^^^^^^^^^
	global ASCIIData
	global tmpASCIIData
	cp_arr ASCIIData tmpASCIIData
	set w .asciioptions
	if {[winfo exists $w]} {destroy $w}
	toplevel $w
	wm geometry $w 
	wm title $w "ASCII Options"
	raise $w
	focus $w
	grab $w
	frame $w.lay1 -relief solid -borderwidth 1
	frame $w.lay1.a
	frame $w.lay1.b
	frame $w.lay2
	frame $w.lay1.a.if
	frame $w.lay1.a.ls
	frame $w.lay1.a.ext
	frame $w.lay1.a.uc
	frame $w.lay1.a.nonneg

	label $w.lay1.a.if.label -text "input format:" -highlightthickness 0
	entry $w.lay1.a.if.entry -width 6 -relief sunken \
					  -textvariable tmpASCIIData(InputFormat) \
					  -highlightthickness 0
	pack $w.lay1.a.if.entry $w.lay1.a.if.label -side right 

	label $w.lay1.a.ls.label -text "lines to skip:" -highlightthickness 0
	entry $w.lay1.a.ls.entry -width 6 -relief sunken \
					  -textvariable tmpASCIIData(LineSkip) \
					  -highlightthickness 0
	pack $w.lay1.a.ls.entry $w.lay1.a.ls.label -side right

	label $w.lay1.a.ext.label -text "file extension:" -highlightthickness 0
	entry $w.lay1.a.ext.entry -width 6 -relief sunken \
					  -textvariable tmpASCIIData(Ext) \
					  -highlightthickness 0
	pack $w.lay1.a.ext.entry $w.lay1.a.ext.label -side right

	label $w.lay1.a.uc.label -text "unit conversion:" -highlightthickness 0 
	tk_optionMenu $w.lay1.a.uc.inu  tmpASCIIData(in_out) "nm->nm" "nm->Ångstrøm" "Ångstrøm->nm" "Ångstrøm->Ångstrøm" "ms->s" "s->s" "ms->ms" "s->ms"
	pack $w.lay1.a.uc.label $w.lay1.a.uc.inu \
	     -side left

	checkbutton $w.lay1.a.nonneg.zero_int_chkb \
		-text "ignore y == 0 at the beginning" \
		-variable tmpASCIIData(zero_int_chkb) \
		-offvalue 0 \
		-onvalue 1
	set tmpASCIIData(zero_int_chkb) 1
	checkbutton $w.lay1.a.nonneg.checkbutton \
		-text "use only non-negative y-values" \
		-variable tmpASCIIData(nonneg) \
		-offvalue 0 \
		-onvalue 1

	label $w.lay1.b.tmp -text " "
	pack $w.lay1.b.tmp -side left -fill x
	pack $w.lay1.a.if $w.lay1.a.ls $w.lay1.a.ext $w.lay1.a.uc  $w.lay1.a.nonneg -fill x -pady 4
	pack $w.lay1.a $w.lay1.b -side left -fill x
	pack $w.lay1.a.nonneg.zero_int_chkb
	pack $w.lay1.a.nonneg.checkbutton

	button $w.lay2.ok -text OK -highlightthickness 0 \
			  -command "if {\[applyAsciiOptionsCmd tmpASCIIData $w\]} {
					destroy $w}"
	button $w.lay2.apply -text Apply -command "applyAsciiOptionsCmd tmpASCIIData $w" \
	       -highlightthickness 0
	button $w.lay2.cancel -text Cancel -command "destroy $w" \
	       -highlightthickness 0
	pack $w.lay2.ok $w.lay2.apply  $w.lay2.cancel \
	     -side left  -fill x -padx 4
	pack $w.lay1  $w.lay2  -pady 4
	data_redu_menu $w 0
	if {[winfo exists $w.lay11]} {
		pack $w.lay11 -after $w.lay1 -before $w.lay2
	}
}

proc HMIOptionsCmd {} {
	set w .hmioptions
	if {[winfo exists $w]} {destroy $w}
	toplevel $w
	wm geometry $w 
	wm title $w "HMI Options"
	raise $w
	focus $w
	grab $w
	frame $w.lay1 -relief solid -borderwidth 1
	frame $w.lay2
	pack $w.lay1  $w.lay2  -pady 4
}

proc data_redu_menu { w index } {
	if {[llength [array get ::data_redu $index,*]] == 0} { return }
	set ::data_redu($index,widgetpath) $w
	frame $w.lay11 -relief solid -borderwidth 1
	label $w.lay11.lbl -text "thinning out data points:"
	frame $w.lay11.mode
	radiobutton $w.lay11.mode.count \
		-text "by counting" \
		-variable ::data_redu($index,by_count) \
		-value 1 -command "data_redu_menu_by_count $w $index"
	radiobutton $w.lay11.mode.dist \
		-text "by relative 2D-distance" \
		-variable ::data_redu($index,by_count) \
		-value 0 -command "data_redu_menu_by_dist $w $index"

	frame $w.lay11.mode.op

	pack $w.lay11.lbl -anchor nw
	pack $w.lay11.mode
	pack $w.lay11.mode.count $w.lay11.mode.dist $w.lay11.mode.op -anchor w
	if {$::data_redu($index,by_count)} {
	    $w.lay11.mode.count invoke
	} else {
	    $w.lay11.mode.dist invoke
	}
}

proc data_redu_menu_by_dist {w index} {
	set w $::data_redu($index,widgetpath)
	foreach child [winfo children $w.lay11.mode.op] { destroy $child }
	checkbutton $w.lay11.mode.op.cb -text "use loglog distance (linear otherwise)" \
		-offvalue 0 -onvalue 1 \
		-variable ::data_redu($index,loglogdist)
	label $w.lay11.mode.op.lbl -text "min point distance: "
	spinbox $w.lay11.mode.op.ntr \
		-textvariable ::data_redu($index,mindist) \
		-format %1.4f -width 7 \
		-from 0.0 -to 1.0 -increment 0.01 \
		-validate key \
		-validatecommand \
	{
		set p %P
		if {[string is double -strict $p]
		} { return 1 } else { return 0 }
	}
	pack $w.lay11.mode.op.cb -anchor w
	pack $w.lay11.mode.op.lbl $w.lay11.mode.op.ntr -side left
}

proc data_redu_menu_by_count {w index} {
	set w $::data_redu($index,widgetpath)
	foreach child [winfo children $w.lay11.mode.op] { destroy $child }
	label $w.lay11.mode.op.lbl -text "percentage/100 to load: "
	spinbox $w.lay11.mode.op.ntr \
		-textvariable ::data_redu($index,percent) \
		-format %1.3f -width 6 \
		-from 0.0 -to 1.0 -increment 0.1 \
		-validate key \
		-validatecommand \
	{
		set p %P
		if {[string is double -strict $p]
		} { return 1 } else { return 0 }
	}
	pack $w.lay11.mode.op.lbl $w.lay11.mode.op.ntr -side left
}

proc show_dr_menu { index } {
	set w .dr_menu
	if {[winfo exists $w]} {destroy $w}
	toplevel $w
	wm geometry $w 
	wm title $w "Data Reduction Options"
	raise $w
	focus $w
	grab $w

	frame $w.lay1 -relief solid -borderwidth 1
	label $w.lay1.lbl -text "data cropping and scaling: "

	frame $w.lay1.div
	label $w.lay1.div.lbl -text "divisor: "
	entry $w.lay1.div.ntr \
		-textvariable ::divisor($index) -width 10

	frame $w.lay1.skipf
	label $w.lay1.skipf.lbl -text "skip first n points: "
	entry $w.lay1.skipf.ntr \
		-textvariable ::fskip($index) -width 10

	frame $w.lay1.skipl
	label $w.lay1.skipl.lbl -text "skip last n points: "
	entry $w.lay1.skipl.ntr \
		-textvariable ::lskip($index) -width 10

	pack $w.lay1.lbl -anchor nw
	pack $w.lay1.div $w.lay1.skipf $w.lay1.skipl -anchor e
	pack $w.lay1.div.lbl $w.lay1.div.ntr -side left
	pack $w.lay1.skipf.lbl $w.lay1.skipf.ntr -side left
	pack $w.lay1.skipl.lbl $w.lay1.skipl.ntr -side left
	data_redu_menu $w $index
	pack $w.lay1 $w.lay11 -pady 5 -padx 5
}

#------------------------------------------------------------------------------
#                opening option menu for reading data files 
#
proc ReadOptionsCmd {} {
#^^^^^^^^^^^^^^^^^^^^^^^
	global sasfit
	switch $sasfit(actualdatatype) {
		Ascii {AsciiOptionsCmd}
		BerSANS {HMIOptionsCmd}
	}
}

#------------------------------------------------------------------------------
#                     menu for reading data files
#
proc NewCmd {} {
#^^^^^^^^^^^^^^^^
	global sasfit
	if {[winfo exists .openfile]} {destroy .openfile}
	toplevel .openfile
	wm geometry .openfile
	wm title .openfile "New File"
	raise .openfile
	grab  .openfile
	focus .openfile

	set datatype $sasfit(actualdatatype)
	frame .openfile.layout1
	frame .openfile.layout2
	frame .openfile.layout3

	set format [tk_optionMenu .openfile.layout1.format sasfit(actualdatatype) Ascii BerSANS]

	.openfile.layout1.format configure -highlightthickness 0
	label .openfile.layout1.label -text "File Format:" -highlightthickness 0
	pack .openfile.layout1.label .openfile.layout1.format -side left -fill x

	label .openfile.layout2.label -text "  File Name:"  -highlightthickness 0
	entry .openfile.layout2.entry -width 40 -relief sunken -highlightthickness 0 \
                              -textvariable sasfit(filename)
	button .openfile.layout2.browse -text "Browse..." -highlightthickness 0 \
	       -command {BrowseCmd .openfile sasfit(filename) \
	                           sasfit(actualdatatype) ReadData }
	pack .openfile.layout2.label .openfile.layout2.entry \
	     .openfile.layout2.browse  -side left -padx 2

	button .openfile.layout3.read -text OK -command \
	{
		catch {destroy .openfile}
	} -highlightthickness 0
	button .openfile.layout3.option -text "Options..." -command ReadOptionsCmd \
	       -highlightthickness 0
	button .openfile.layout3.dismiss -text Cancel \
	       -highlightthickness 0 -command \
	{
		if {[winfo exist .asciioptions]} {
			destroy .asciioptions
		}
		destroy .openfile
		set sasfit(filename) ""
	}
	pack .openfile.layout3.read .openfile.layout3.option  \
	     .openfile.layout3.dismiss \
	     -side left -padx 6 -fill x -expand yes 

	pack .openfile.layout1 .openfile.layout2 .openfile.layout3 \
	     -fill x -pady 2 -pady 6
	
	bind .openfile <KeyPress-Return> ".openfile.layout3.read invoke"
	bind .openfile <KeyPress-Escape> ".openfile.layout3.dismiss invoke"
}

#------------------------------------------------------------------------------
#                     subroutine for merging data files
#
proc FileMergeCmd {ttmpsasfit} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
upvar $ttmpsasfit ssasfit
global tmpAnalytPar
set ssasfit(Q)        {}
set ssasfit(I)        {}
set ssasfit(DI)       {}
set ssasfit(res)      {}
set ssasfit(res,file) {}
set ssasfit(res,calc) {}
for {set i 0} {$i < $ssasfit(file,n)} {incr i} {
   set tmpwidname  [lindex $ssasfit(file,widname)   $i]
   set j [lindex $tmpwidname 1]
   set hide($j)    [lindex $ssasfit(file,hide)      $i]
   set fskip($j)   [lindex $ssasfit(file,firstskip) $i]
   set lskip($j)   [lindex $ssasfit(file,lastskip)  $i]
   set divisor($j) [lindex $ssasfit(file,divisor)   $i]
   set widname($j) [lindex $ssasfit(file,widname)   $i]
   dr_set_arr ssasfit "file," $j data_redu

   set Q           [lindex $ssasfit(file,Q)         $i]
   set I           [lindex $ssasfit(file,I)         $i]
   set DI          [lindex $ssasfit(file,DI)        $i]
   set res         [lindex $ssasfit(file,res)       $i]
   set resfile     [lindex $ssasfit(file,res,file)  $i]
   set rescalc     [lindex $ssasfit(file,res,calc)  $i]

   set firsti $fskip($j)
   set lasti [expr [llength $Q]-$lskip($j)-1]
   set tmparr(Q) [lrange $Q $firsti $lasti]
   set tmparr(I) [lrange $I $firsti $lasti]
   set tmparr(DI) [lrange $DI $firsti $lasti]
   set tmparr(res) [lrange $res $firsti $lasti]
   set tmparr(res,file) [lrange $resfile $firsti $lasti]
   set tmparr(res,calc) [lrange $rescalc $firsti $lasti]
puts "Hallo [llength $tmparr(Q)]"
   dr data_redu $j tmparr [llength $tmparr(Q)] \
      $tmparr(Q) $tmparr(I) $tmparr(DI) $tmparr(res) $tmparr(res,file) $tmparr(res,calc)

   set widcnt      $ssasfit(file,widcnt)
   if {[string compare $hide($j) no] == 0} {
      for {set k 0} {$k < [llength $tmparr(Q)]} {incr k} {
         set h        [lindex $tmparr(Q)   $k]
         set Ih       [lindex $tmparr(I)   $k]
         set DIh      [lindex $tmparr(DI)  $k]
         set resh     [lindex $tmparr(res) $k]
         set resfileh [lindex $tmparr(res,file) $k]
         set rescalch [lindex $tmparr(res,calc) $k]
         lappend ssasfit(Q)   $h
         lappend ssasfit(I)   [expr $Ih /($divisor($j)*1.0)]
         lappend ssasfit(DI)  [expr $DIh/($divisor($j)*1.0)]
         lappend ssasfit(res) $resh
         lappend ssasfit(res,file) $resfileh
         lappend ssasfit(res,calc) $rescalch
      } 
      set ind               [lsort_index $ssasfit(Q)]
      set ssasfit(Q)        [lmindex $ssasfit(Q)   $ind]
      set ssasfit(I)        [lmindex $ssasfit(I)   $ind]
      set ssasfit(res,file) [lmindex $ssasfit(res,file) $ind]
      set ssasfit(res,calc) [lmindex $ssasfit(res,calc) $ind]
      if {$tmpAnalytPar(geometrical/datafile)} {
	  set ssasfit(res) $ssasfit(res,calc)
      } else {
	  set ssasfit(res) $ssasfit(res,file)
      }
      if { [llength $ssasfit(DI)] != 0 } {
         set ssasfit(DI)  [lmindex $ssasfit(DI)  $ind]
      }
   }
}
set ssasfit(npoints) [llength $ssasfit(Q)]
}

#------------------------------------------------------------------------------
#         call back subroutine for defining resolution parameters
#
proc DefResParCmd {widcnt} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^
	global resolution tmpsasfit
	set index 0
	for {} {$index < $tmpsasfit(file,n)} {incr index} {
		set tmpwidname  [lindex $tmpsasfit(file,widname) $index]
		set j [lindex $tmpwidname 1]
		if {$j == $widcnt} { 
			set resolution(widcnt) $index
			break
		}
	}
	foreach suffix {r1 r2 l1 l2 lambda Dlambda d Dd Q
	} {
		set resolution($suffix) [lindex $tmpsasfit(file,$suffix) $index]
	}
	set_resolutionPar [lindex $tmpsasfit(file,name) $index]
}

proc destroyMergeFile {widcnt} {
    set wid .addfile.lay2
    destroy $wid.vportfile.frame.text$widcnt
    destroy $wid.vportdivisor.frame.entry$widcnt
    destroy $wid.vportsfirst.frame.entry$widcnt
    destroy $wid.vportslast.frame.entry$widcnt
    destroy $wid.vporthide.frame.checkbutton$widcnt
    destroy $wid.vportdel.frame.delbutton$widcnt
    destroy $wid.vportres.frame.resbutton$widcnt
}

#------------------------------------------------------------------------------
#         subroutine for removing files from merging list
#
proc rmMergeFileCmd {delwidcnt} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
	global tmpsasfit
	global fn hide fskip lskip divisor widname 
	global r1 r2 l1 l2 lambda Dlambda d Dd
	global widcnt sf
	
	if {$tmpsasfit(file,n) <= 1} {
	   blt::beep
	   return
	}
	
	set sf(file,n)         $tmpsasfit(file,n)
	clear_sasfit_file_config sf "file,"
	set sf(delwid)         no
	
	clear_sasfit_config tmpsasfit "file," {name divisor firstskip lastskip hide \
		dr_by_count dr_percent dr_loglogdist dr_mindist}
	
	for {set i 0} {$i < $tmpsasfit(file,n)} {incr i} {
	   set tmpwidname [lindex $tmpsasfit(file,widname) $i]
	   set j [lindex $tmpwidname 1]
	   arr_append_dr tmpsasfit "file," $j
	}
	
	if { ![equal_length_sasfit_file_config tmpsasfit "file,"]
	} {
		error "length of lists describing the data sets do not match !"
	}
	
	set newIndex  0
	set listIndex 0
	foreach twidname $tmpsasfit(file,widname) {
	
	    set w2 .addfile.lay2
	    set widcnt [lindex $twidname 1]
	    destroyMergeFile $widcnt
	
	    if {$widcnt == $delwidcnt} {
		set sf(file,n) [expr $sf(file,n)-1]
	    } else {
		sasfit_arr_op lappend sf tmpsasfit "file," "file," $listIndex \
			{name hide firstskip lastskip divisor widname \
			Q I DI res "res,file" "res,calc" r1 r2 l1 l2 lambda Dlambda d Dd \
			dr_by_count dr_percent dr_loglogdist dr_mindist}
	        lset sf(file,widname) $newIndex [lreplace [lindex $sf(file,widname) end] 1 1 [expr $newIndex+1]]
		incr newIndex
	    }
	    incr listIndex
	}
	sasfit_arr_op set tmpsasfit sf "file," "file," -1 {name hide firstskip lastskip divisor \
		widname Q I DI res "res,file" "res,calc" r1 r2 l1 l2 lambda Dlambda d Dd n \
		dr_by_count dr_percent dr_loglogdist dr_mindist}
	
	set tmpsasfit(file,widcnt) $newIndex
	# rebuild GUI
	set i 0
	foreach twidname $tmpsasfit(file,widname) {
		MergeFileCmd tmpsasfit $i
		incr i
	}
}


#------------------------------------------------------------------------------
#                     subroutine for merging data files
#
proc MergeFileCmd {ttmpsasfit args} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
upvar $ttmpsasfit ssasfit
global resolution
set w2 .addfile.lay2
   if {[llength $args] == 1} {
         set i [lindex $args 0]
         set tmpwidname [lindex $ssasfit(file,widname) $i]
         set j [lindex $tmpwidname 1] 
   } else {
      set i [expr $ssasfit(file,n) -1]
      incr ssasfit(file,widcnt)
      set j $ssasfit(file,widcnt)
      if {[string length [string trim $ssasfit(filename)]]} {
	      lappend ssasfit(file,name) $ssasfit(filename)
	      set ssasfit(filename) ""
      }
      lappend ssasfit(file,divisor)   1
      lappend ssasfit(file,firstskip) 0
      lappend ssasfit(file,lastskip)  0
      lappend ssasfit(file,hide)      no
      dr_default_op lappend ssasfit "file,"
      lappend ssasfit(file,widname)   [list $w2 $ssasfit(file,widcnt)]
      lappend ssasfit(file,r1)        $resolution(r1)
      lappend ssasfit(file,r2)        $resolution(r2)
      lappend ssasfit(file,lambda)    $resolution(lambda)
      lappend ssasfit(file,Dlambda)   $resolution(Dlambda)
      lappend ssasfit(file,l1)        $resolution(l1)
      lappend ssasfit(file,l2)        $resolution(l2)
      lappend ssasfit(file,Dd)        $resolution(Dd)
      lappend ssasfit(file,d)         $resolution(d)
   }

global fn hide fskip lskip divisor widname widcnt

   set fn($j)      [lindex $ssasfit(file,name)      $i]
   set hide($j)    [lindex $ssasfit(file,hide)      $i]
   set fskip($j)   [lindex $ssasfit(file,firstskip) $i]
   set lskip($j)   [lindex $ssasfit(file,lastskip)  $i]
   set divisor($j) [lindex $ssasfit(file,divisor)   $i]
   set widname($j) [lindex $ssasfit(file,widname)   $i]
   set widcnt [lindex $widname($j) 1]
   dr_set_arr ssasfit "file," $j ::data_redu

   button $w2.vportfile.frame.text$widcnt \
         -text [dispFname $j] -width 20 -highlightthickness 0 -anchor w \
	 -command "show_dr_menu $j"
   setTooltip $w2.vportfile.frame.text$widcnt [lindex $ssasfit(file,name) $i]
   set h 0
   set tmph [winfo reqheight $w2.vportfile.frame.text$widcnt]
   if {$h < $tmph} {set h $tmph}
   grid  $w2.vportfile.frame.text$widcnt \
         -column 0 -row $i

   entry $w2.vportdivisor.frame.entry$widcnt \
           -textvariable divisor($j) -width 10
   set tmph [winfo reqheight $w2.vportdivisor.frame.entry$widcnt]
   if {$h < $tmph} {set h $tmph}
   grid  $w2.vportdivisor.frame.entry$widcnt \
         -column 0 -row $i

   entry $w2.vportsfirst.frame.entry$widcnt \
           -textvariable fskip($j) -width 10
   set tmph [winfo reqheight $w2.vportsfirst.frame.entry$widcnt]
   if {$h < $tmph} {set h $tmph}
   grid  $w2.vportsfirst.frame.entry$widcnt \
         -column 0 -row $i

   entry $w2.vportslast.frame.entry$widcnt \
           -textvariable lskip($j) -width 10
   set tmph [winfo reqheight $w2.vportslast.frame.entry$widcnt]
   if {$h < $tmph} {set h $tmph}
   grid  $w2.vportslast.frame.entry$widcnt \
         -column 0 -row $i

checkbutton $w2.vporthide.frame.checkbutton$widcnt \
           -variable hide($j) -onvalue yes -offvalue no
   set tmph [winfo reqheight $w2.vporthide.frame.checkbutton$widcnt]
   if {$h < $tmph} {set h $tmph}
   grid  $w2.vporthide.frame.checkbutton$widcnt \
         -column 0 -row $i


button $w2.vportdel.frame.delbutton$widcnt \
          -text del \
          -command "[list rmMergeFileCmd $widcnt]"
   set tmph [winfo reqheight $w2.vportdel.frame.delbutton$widcnt]
   if {$h < $tmph} {set h $tmph}
   grid  $w2.vportdel.frame.delbutton$widcnt \
         -column 0 -row $i -sticky nsew

button $w2.vportres.frame.resbutton$widcnt \
          -text "resolution parameter" \
          -command "[list DefResParCmd $widcnt]"
   set tmph [winfo reqheight $w2.vportres.frame.resbutton$widcnt]
   if {$h < $tmph} {set h $tmph}
   grid  $w2.vportres.frame.resbutton$widcnt \
         -column 0 -row $i -sticky nsew


   grid rowconfigure $w2.vportfile.frame    $i -minsize $h
   grid rowconfigure $w2.vportdivisor.frame $i -minsize $h
   grid rowconfigure $w2.vportsfirst.frame  $i -minsize $h 
   grid rowconfigure $w2.vportslast.frame   $i -minsize $h
   grid rowconfigure $w2.vporthide.frame    $i -minsize $h
   grid rowconfigure $w2.vportdel.frame     $i -minsize $h
   grid rowconfigure $w2.vportres.frame     $i -minsize $h

   update
   hsetyview scroll  1 units
   hsetyview scroll -1 units
}
proc dispFname { idx } {
   set fnl [string length $::fn($idx)]
   if {$fnl > 20} {
      return "...[string range $::fn($idx) [expr $fnl-17] [expr $fnl-1]]"
   }
}

#------------------------------------------------------------------------------
#                     menu for merging data files
#
proc scrollform_resize {win} {
   set bbox [$win bbox all]
   if {[winfo exists $win.frame]} {
      set wid [winfo width $win.frame]
   } else {
      foreach child [winfo children $win] {
         if {"[winfo class $child]" == "Frame"} {
            set wid [winfo width $child]
         }
      }
   }
   $win configure -width $wid\
        -scrollregion $bbox -yscrollincrement 1
}
proc hsetyview {args} {
  eval .addfile.lay2.vportfile    yview $args
  eval .addfile.lay2.vportdivisor yview $args
  eval .addfile.lay2.vportsfirst  yview $args
  eval .addfile.lay2.vportslast   yview $args
  eval .addfile.lay2.vporthide    yview $args
  eval .addfile.lay2.vportdel     yview $args
  eval .addfile.lay2.vportres     yview $args
}


proc MergeCmd {} {
#^^^^^^^^^^^^^^^^
	global sasfit tmpsasfit tmpAnalytPar
	global fn hide fskip lskip divisor widname r1 r2 l1 l2 lambda Dlambda d Dd
	global addsasfit
	if {$sasfit(file,n) < 1} {
		NewCmd
		tkwait window .openfile
		if {![string length [string trim $sasfit(filename)]]} {return};# 'cancel' pressed
		# clear structures
		clear_sasfit_file_config sasfit "file,"
	}
	if {[winfo exists .addfile]} {destroy .addfile}
	toplevel .addfile
	wm geometry .addfile
	wm title .addfile "Merge Files"
	raise .addfile
	grab  .addfile
	focus .addfile

	cp_arr sasfit tmpsasfit
	set datatype $tmpsasfit(actualdatatype)
	frame .addfile.lay1 -relief ridge -borderwidth 2
	frame .addfile.lay2 -relief ridge -borderwidth 2
	frame .addfile.lay1.layout1
	frame .addfile.lay1.layout2
	set w .addfile.lay1
	pack  .addfile.lay1 -fill x
	pack  .addfile.lay2 -fill both -expand yes
	frame .addfile.layout3

	set format [tk_optionMenu $w.layout1.format tmpsasfit(actualdatatype) Ascii BerSANS]

	$w.layout1.format configure -highlightthickness 0
	label $w.layout1.label -text "File Format:" \
	      -width 12 -highlightthickness 0
	button $w.layout1.option -text "Options..." -command ReadOptionsCmd \
	       -highlightthickness 0 -pady 1m
	button $w.layout1.read -text "Read file" -command \
	{
               global tmpsasfit ASCIIData
               set tmpfnlist $tmpsasfit(filename)
               set errornessFiles {}
               foreach fin $tmpfnlist {
		  if {![file exists $fin]} { continue }
                  set tmpsasfit(filename) $fin
		  set tmpval [ReadFileCmd tmpsasfit /norefreshdata]
                  if {[string equal "$tmpval" "no"]
		  } {
                     lappend tmpsasfit(file,Q)        $tmpsasfit(Q)
                     lappend tmpsasfit(file,I)        $tmpsasfit(I)
                     lappend tmpsasfit(file,DI)       $tmpsasfit(DI)
                     lappend tmpsasfit(file,res,file) $tmpsasfit(res,file)
                     set tmpsasfit(res,calc) [sasfit_res $resolution(lambda)  \
			                             $resolution(Dlambda) \
						     $resolution(r1)      \
						     $resolution(l1)      \
						     $resolution(r2)      \
						     $resolution(l2)      \
						     $resolution(d)       \
						     $resolution(Dd)      \
						     $tmpsasfit(Q)       \
						 ]
                     lappend tmpsasfit(file,res,calc) $tmpsasfit(res,calc)

		     if {$tmpAnalytPar(geometrical/datafile)} {
			 lappend tmpsasfit(file,res) $tmpsasfit(res,calc)
		     } else {
			 lappend tmpsasfit(file,res) $tmpsasfit(res,file)
		     }
                     incr tmpsasfit(file,n)
                     MergeFileCmd tmpsasfit
	          } else {
                     lappend errornessFiles  $tmpsasfit(filename)
	          }
	       }
               if {[llength $errornessFiles] > 0} {
		   set msgtxt ""
                   set indx 1
                   foreach fin  $errornessFiles {
		       set msgtxt "$msgtxt\n$indx\. $fin"
		       incr indx
		   }
                   tk_messageBox -icon error \
                        -message "Could not read data files:\n$msgtxt\n\nPlease review the input format options !"
	           if {[winfo exists .addfile]} {destroy .addfile}
		   MergeCmd
	       }
	} -highlightthickness 0 -pady 1m
	pack $w.layout1.label $w.layout1.format -side left -fill x
	pack $w.layout1.option -side left  -fill x -padx 3m
	pack $w.layout1.read   -side right -fill x -padx 2m

	label $w.layout2.label -text "File Name:" \
	      -width 12 -highlightthickness 0
	entry $w.layout2.entry -width 30 -relief sunken -highlightthickness 0 \
	                              -textvariable tmpsasfit(filename)
	button $w.layout2.browse -text "Browse..." -highlightthickness 0 \
	       -command {BrowseCmd .addfile tmpsasfit(filename) \
	                           tmpsasfit(actualdatatype) ReadData }
	pack $w.layout2.label $w.layout2.entry -side left
	pack $w.layout2.browse  -side left -padx 2m

	set w2 .addfile.lay2
	label $w2.file -text "file name:" \
	      -highlightthickness 0 -justify left -anchor w
	grid  $w2.file -column 0 -row 0 -sticky w 
	label $w2.divisor -text "divisor" \
	      -highlightthickness 0 -justify left -anchor w
	grid  $w2.divisor -column 1 -row 0 -sticky w
	label $w2.skipfirst -text "skip first\nn points" \
	      -highlightthickness 0 -justify left -anchor w
	grid  $w2.skipfirst -column 2 -row 0 -sticky w
	label $w2.skiplast -text "skip last\npoints" \
	      -highlightthickness 0 -justify left -anchor w
	grid  $w2.skiplast -column 3 -row 0 -sticky w
	label $w2.hide -text "hide" \
	      -highlightthickness 0 -justify left -anchor w
	grid  $w2.hide -column 4 -row 0 -sticky w
	frame $w2.resframe
	grid  $w2.resframe -column 6 -row 0 -sticky w
	checkbutton $w2.resframe.geom -text "calc resolution\nfrom geometrical values (on)\ntake values from data file (off)" \
	      -variable tmpAnalytPar(geometrical/datafile) \
	      -onvalue yes -offvalue no \
	      -justify left
	grid $w2.resframe.geom  -column 0 -row 1 -sticky w

	set canvasheight 160

	scrollbar $w2.sbar -command "hsetyview"
	grid   $w2.sbar -column 7 -row 1 -sticky ns
	grid rowconfigure $w2 1 -weight 2 -minsize $canvasheight

	canvas $w2.vportfile -width 1  -height $canvasheight \
	       -highlightthickness 0 -yscrollcommand "$w2.sbar set"
	grid   $w2.vportfile -column 0 -row 1 
	frame  $w2.vportfile.frame 
	$w2.vportfile create window 0 0 -anchor nw -window $w2.vportfile.frame
	bind $w2.vportfile.frame <Configure> "scrollform_resize $w2.vportfile"

	canvas $w2.vportdivisor -width 1 -height $canvasheight \
	       -highlightthickness 0 -yscrollcommand "$w2.sbar set"
	grid   $w2.vportdivisor -column 1 -row 1
	frame  $w2.vportdivisor.frame
	$w2.vportdivisor create window 0 0 -anchor nw -window $w2.vportdivisor.frame
	bind $w2.vportdivisor.frame <Configure> "scrollform_resize $w2.vportdivisor"

	canvas $w2.vportsfirst -width 1 -height $canvasheight \
	       -highlightthickness 0 -yscrollcommand "$w2.sbar set"
	grid   $w2.vportsfirst -column 2 -row 1
	frame  $w2.vportsfirst.frame
	$w2.vportsfirst create window 0 0 -anchor nw -window $w2.vportsfirst.frame
	bind $w2.vportsfirst.frame <Configure> "scrollform_resize $w2.vportsfirst"

	canvas $w2.vportslast -width 1 -height $canvasheight \
	       -highlightthickness 0 -yscrollcommand "$w2.sbar set"
	grid   $w2.vportslast -column 3 -row 1
	frame  $w2.vportslast.frame
	$w2.vportslast create window 0 0 -anchor nw -window $w2.vportslast.frame
	bind $w2.vportslast.frame <Configure> "scrollform_resize $w2.vportslast"

	canvas $w2.vporthide -width 1 -height $canvasheight \
	       -highlightthickness 0 -yscrollcommand "$w2.sbar set"
	grid   $w2.vporthide -column 4 -row 1
	frame  $w2.vporthide.frame
	$w2.vporthide create window 0 0 -anchor nw -window $w2.vporthide.frame
	bind $w2.vporthide.frame <Configure> "scrollform_resize $w2.vporthide"

	canvas $w2.vportdel -width 1 -height $canvasheight \
	       -highlightthickness 0 -yscrollcommand "$w2.sbar set"
	grid   $w2.vportdel -column 5 -row 1
	frame  $w2.vportdel.frame
	$w2.vportdel create window 0 0 -anchor nw -window $w2.vportdel.frame
	bind $w2.vportdel.frame <Configure> "scrollform_resize $w2.vportdel"

	canvas $w2.vportres -width 1 -height $canvasheight \
	       -highlightthickness 0 -yscrollcommand "$w2.sbar set"
	grid   $w2.vportres -column 6 -row 1
	frame  $w2.vportres.frame
	$w2.vportres create window 0 0 -anchor nw -window $w2.vportres.frame
	bind $w2.vportres.frame <Configure> "scrollform_resize $w2.vportres"

	if {[llength $tmpsasfit(filename)] > 0} {
		$w.layout1.read invoke
	} else {
		for {set i 0} {$i < $tmpsasfit(file,n)} {incr i} {
			MergeFileCmd tmpsasfit $i
		}
	}
	if {![winfo exists .addfile] || [winfo exists .addfile.layout3.read]
	} {
		return
	}
	button .addfile.layout3.read -text OK -command \
	{
		destroy_ascii_options
		merge_cmd_apply tmpsasfit 0
		catch {destroy .addfile}
	} -highlightthickness 0
	button .addfile.layout3.apply -text Apply -command \
	{
		destroy_ascii_options
		merge_cmd_apply tmpsasfit 0
	} -highlightthickness 0
	button .addfile.layout3.dismiss -text Cancel -highlightthickness 0 -command \
	{
		destroy_ascii_options
		catch {destroy .addfile}
	}
	button .addfile.layout3.new -text "New..." -highlightthickness 0 -command \
	{
		set sasfit(file,n) 0
		MergeCmd
	}
        checkbutton .addfile.layout3.dores -text "use resolution" \
		-onvalue yes -offvalue no \
		-variable ::tmpAnalytPar(resolution)

	pack .addfile.layout3.read .addfile.layout3.apply  \
	     .addfile.layout3.dismiss .addfile.layout3.new \
	     .addfile.layout3.dores \
	     -side left -padx 6 -fill x 

	pack $w.layout1 $w.layout2 .addfile.layout3 -fill x -pady 6

	bind .addfile <KeyPress-Return> ".addfile.layout3.read invoke"
	bind .addfile <KeyPress-Escape> ".addfile.layout3.dismiss invoke"

	set taglist [bindtags .addfile]
	lappend taglist AddfileResizeTag
	bindtags .addfile $taglist

	# we need to wait until the window is fully populated
	update

	bind AddfileResizeTag <Configure> "addfileResizeCmd [winfo reqheight .addfile] $canvasheight %h"
}

proc addfileResizeCmd { origHeight canvasHeight newHeight } {
#	puts stderr "$origHeight $canvasHeight $newHeight"
	set w2 .addfile.lay2
	set canvasHeight [expr $canvasHeight + ($newHeight - $origHeight)]
	if {$canvasHeight < 0} { return }
	grid rowconfigure $w2 1 -minsize $canvasHeight
	foreach w [list $w2.vportfile $w2.vportdivisor $w2.vportsfirst $w2.vportslast \
		$w2.vporthide $w2.vportdel $w2.vportres \
	] {
		$w configure -height $canvasHeight
	}
}

proc destroy_ascii_options {} {
	if {[winfo exist .asciioptions]} {
		destroy .asciioptions
	}
}

proc merge_cmd_apply { sasfit_arr isGlobal
} {
	upvar $sasfit_arr localsasfit
	global ASCIIData IQGraph GlobalFitIQGraph
	global fn hide fskip lskip divisor widname

	set titleText "Q / $ASCIIData(unit)\^-1"
	if {$isGlobal} { set GlobalFitIQGraph(x,title) $titleText
	} else {         set IQGraph(x,title) $titleText }

	clear_sasfit_config localsasfit "file," {name divisor firstskip lastskip \
		hide dr_by_count dr_percent dr_loglogdist dr_mindist}

	set n_no_hide 0
	for {set i 0} {$i < $localsasfit(file,n)} {incr i} {
		set tmpwidname [lindex $localsasfit(file,widname) $i]
		set j [lindex $tmpwidname 1]
		arr_append_dr localsasfit "file," $j
		if {[string compare $hide($j) no] == 0} {
			incr n_no_hide
		}
	}
	# why 2 times ? (see 'save_actual_globalfit_data' in sasfit_addfiles.tcl)
	for {set i 0} {$i < 2} {incr i} {
		if {$isGlobal} { 
			replace_or_append_Nth_globalfit_data localsasfit $localsasfit(Nth,actual)
		}
		if {$n_no_hide > 0} {
			FileMergeCmd localsasfit
			if {$isGlobal} {
				global addsasfit
				NewGlobalFitDataCmd localsasfit
				cp_arr localsasfit addsasfit
			} else {
				global sasfit
				cp_arr localsasfit sasfit
				NewDataCmd sasfit
			}
		} else {
			error "at least one file should not be hidden"
		}
	}
}

#------------------------------------------------------------------------------
#                
proc LoadCmd { loadProj } {
#^^^^^^^^^^^^^^^^
   global sasfit
   global addsasfit
   global AnalytPar
   global tmpAnalytPar
   global actualAnalytPar
   global tmpactualAnalytPar
   global GlobalAnalytPar
   global tmpGlobalAnalytPar
   global actualGlobalAnalytPar
   global tmpactualGlobalAnalytPar
   global G2ParData
   global distr
   global nomenu
   global IQGraph GlobalFitIQGraph ResIQGraph SDGraph StructParData
   global OZ ozSQGraph ozgrGraph ozcrGraph ozhrGraph ozgammarGraph ozbetaUrGraph ozBrGraph ozyrGraph ozfrGraph
   global adjustvalue
   global FitPrecision
   
   if {[llength $loadProj] == 0} {
       set loadProj [tk_getOpenFile -defaultextension ".sas"  \
                       -filetypes {{"sasfit project" ".sas"} \
                                   {"All"            ".*"}}   \
                       -initialdir "$sasfit(datadir)" \
                       -title "Open Project .."   \
                    ]
   }

   if {[llength $loadProj] > 0} { 
       set Name $loadProj
   } else { return }
   if {[file exists [lindex $Name 0]]} {
       set sasfit(datadir) "[file dirname [lindex $Name 0]]"
   }

   if { [string length $loadProj] > 0} {
      wm title . "[file tail $loadProj], $sasfit(progname) $sasfit(version)"
      clearGraph_el GlobalFitIQGraph
      clearGraph_el IQGraph
      clearGraph_el ResIQGraph
      clearGraph_el SDGraph
      # prevent local settings from being overwritten
      set no_update_entries $::sasfit(no_update_entries)
      foreach var $no_update_entries {
	      if {![info exists ::sasfit($var)]} { continue }
	      set temparr($var) $::sasfit($var) 
      }
      set graphSettings {}
      foreach graph {GlobalFitIQGraph IQGraph ResIQGraph SDGraph} {
	      set graphList {}
	      foreach noup $::sasfit(no_update_entries_graph) {
			lappend graphList [array get $graph $noup]
	      }
	      lappend graphSettings [list $graph $graphList]
      }
      # load project file
      source $loadProj
      # restore previous settings
      foreach var $no_update_entries { 
	      if {![info exists temparr($var)]} { continue }
	      set ::sasfit($var) $temparr($var) 
      }
      foreach graphSet $graphSettings {
	      set graph [lindex $graphSet 0]
	      set graphList [lindex $graphSet 1]
	      foreach noup $graphList {
		      array set $graph $noup
	      }
      }
      foreach ap { ::GlobalAnalytPar \
	           ::tmpGlobalAnalytPar \
                   ::actualGlobalAnalytPar \
		   ::tmpactualGlobalAnalytPar \
		   ::AnalytPar \
		   ::tmpAnalytPar \
		   ::actualAnalytPar \
		   ::tmpactualAnalytPar
      } {
		# convert unique function (group) names to function plugin IDs
	      foreach type {FF SD SQ} {
		      set str {}
		      foreach typestr_lst [lindex [array get $ap $type,typestr] 1] {
			      set sublst {}
			      foreach typestr $typestr_lst {
				lappend sublst [get_func_id_by_name $typestr]
			      }
			      lappend str $sublst
		      }
		      array set $ap [list $type,typestr $str]
		      ap_update_param_count $ap $type
	      }
	      if {[string equal -length 6 [lindex [array get $ap isGlobal] end] "global"]
		      } { array set $ap {isGlobal 1}}
	      if {[string equal -length 9 [lindex [array get $ap isGlobal] end] "nonGlobal"]
		      } { array set $ap {isGlobal 0}}
      }
      set sasfit(lastProjectFile) $loadProj
      add_to_global_var ::actualGlobalAnalytPar common_names
      # delete probably invalid and obsolete filenames
      # filenames with spaces are not encapsulated in list elements
      set addsasfit(filename) {}
      clearGraph_el IQGraph
      clearGraph_el ResIQGraph
      clearGraph_el SDGraph
      if {$sasfit(file,n) >= 1} {
         Put_Graph_el IQGraph $sasfit(Q) $sasfit(I) $sasfit(DI)
      }
      RefreshGraph IQGraph
      NewGlobalFitDataCmd addsasfit 
      RefreshGraph GlobalFitIQGraph
      RefreshGraph ResIQGraph
      RefreshGraph SDGraph
      RefreshStructParDataTab
      RefreshAnalytParDataTab GlobalAnalytPar /Global
      catch {destroy .analytical}
   }
}

#------------------------------------------------------------------------------
proc array_elem_exists { array_name element_name } {
	upvar $array_name arr_name
	return [expr {[string equal [array names arr_name -exact $element_name] "$element_name"]}]
}
proc add_empty_to_prj_file { fid array_name elem_name } {
	if { ! [array_elem_exists $array_name $elem_name] } {
		puts $fid "set $array_name\($elem_name\) {}"
	}
}
proc add_to_global_var { array_name elem_name } {
	upvar $array_name arr_name
	if { ! [array_elem_exists $array_name $elem_name] } {
		set arr_name($elem_name) {}
	}
}
#                
proc SaveCmd {} {
#^^^^^^^^^^^^^^^^
   global sasfit
   global addsasfit
   global AnalytPar
   global tmpAnalytPar
   global actualAnalytPar
   global tmpactualAnalytPar
   global GlobalAnalytPar
   global tmpGlobalAnalytPar
   global actualGlobalAnalytPar
   global tmpactualGlobalAnalytPar
   global G2ParData
   global distr
   global nomenu
   global IQGraph GlobalFitIQGraph ResIQGraph SDGraph StructParData
   global OZ ozSQGraph ozgrGraph ozcrGraph ozhrGraph ozgammarGraph ozbetaUrGraph ozBrGraph ozyrGraph ozfrGraph
   global adjustvalue
   global FitPrecision

   # check if there is already a filename otherwise ask for one
   if { ! [array_elem_exists ::sasfit lastProjectFile] } {
	   SaveAsCmd
   } else {
	   set filename $::sasfit(lastProjectFile)
	   array unset ::sasfit lastProjectFile

	   if { [string length $filename] > 0} {
	      wm title . "[file tail $filename], $sasfit(progname) $sasfit(version)"
	      set fid [open $filename w]
	      puts_arr $fid sasfit
	      puts_arr $fid addsasfit
		puts_arr $fid ::ask4dataset_var
		puts_arr $fid ::GlobalAnalytPar
		puts_arr $fid ::tmpGlobalAnalytPar
		puts_arr $fid ::actualGlobalAnalytPar
		puts_arr $fid ::tmpactualGlobalAnalytPar
		puts_arr $fid ::AnalytPar
		puts_arr $fid ::tmpAnalytPar
		puts_arr $fid ::actualAnalytPar
		puts_arr $fid ::tmpactualAnalytPar

	      foreach ap { ::GlobalAnalytPar \
			   ::tmpGlobalAnalytPar \
			   ::actualGlobalAnalytPar \
			   ::tmpactualGlobalAnalytPar \
			   ::AnalytPar \
			   ::tmpAnalytPar \
			   ::actualAnalytPar \
			   ::tmpactualAnalytPar
	      } {
		      add_empty_to_prj_file $fid $ap common_names

			# convert plugin function IDs to unique function (group) names
		      foreach type {FF SD SQ} {
			      set str {}
			      if { [llength [array names $ap $type,typestr]] } {
				      foreach typestr [lindex [array get $ap $type,typestr] end] {
					      lappend str [get_name_by_id $typestr]
				      }
			      }
			      if { [llength $str] } {
				puts $fid "set $ap\($type,typestr\) { $str }"
			      }
		      }
	      }
	      puts_arr $fid IQGraph
	      puts_arr $fid GlobalFitIQGraph
	      puts_arr $fid ResIQGraph
	      puts_arr $fid SDGraph
	      puts_arr $fid StructParData
	      puts_arr $fid G2ParData
	      puts_arr $fid FitPrecision
	      puts_arr $fid OZ 
	      puts_arr $fid ozSQGraph 
	      puts_arr $fid ozgrGraph 
	      puts_arr $fid ozcrGraph 
	      puts_arr $fid ozhrGraph 
	      puts_arr $fid ozgammarGraph 
	      puts_arr $fid ozbetaUrGraph
	      puts_arr $fid ozBrGraph 
	      puts_arr $fid ozyrGraph 
	      puts_arr $fid ozfrGraph 
	      close $fid
	   }
	   set ::sasfit(lastProjectFile) $filename
   }
}

proc SaveAsCmd {} {
#^^^^^^^^^^^^^^^^
   set filename [tk_getSaveFile -defaultextension ".sas"  \
                       -filetypes {{"sasfit project" ".sas"} \
                                   {"All"            ".*"}}   \
                       -initialdir "$::sasfit(datadir)" \
                       -title "Save Project .."   \
                ]
   if { [string length $filename] > 0 } {
	   set ::sasfit(lastProjectFile) $filename
	   SaveCmd
   }
   if {[llength $filename] > 0} { 
       set Name $filename
   } else { return }
   if {[file exists [lindex $Name 0]]} {
       set sasfit(datadir) "[file dirname [lindex $Name 0]]"
   }

}

proc RefreshAnalytParDataTab {AAnalytPar args} {
	upvar $AAnalytPar ap

	analytpar_get_text ap
        catch {moments_get_text ap}

	set n $::sasfit(par_name)
	tab_text_update ap $n

	# reconfigure copy command
	$ap(${n}_w).popup entryconfigure 0 -command \
		"tab_text_copy2clipboard_cmd $AAnalytPar ${n}"
	$ap(${n}_w).popup entryconfigure 1 -command \
		"tab_text_copy2file_cmd $AAnalytPar ${n}"

	set n $::sasfit(mom_name)
	tab_text_update ap $n

	# reconfigure copy command
	$ap(${n}_w).popup entryconfigure 0 -command \
		"tab_text_copy2clipboard_cmd $AAnalytPar ${n}"
	$ap(${n}_w).popup entryconfigure 1 -command \
		"tab_text_copy2file_cmd $AAnalytPar ${n}"
}

# Adds a sub-list to an existing list. If sublist has less elements than
# $width, empty elements are added. Every call to add2list results in a 
# constant increase of list length by $width.
# The first element of the list is the max required column width.
proc add2list { list width sl } {
	upvar $list l
	for {set i 0} {$i < $width} {incr i} {
		if {$i < [llength $sl]} {
			set e [lindex $sl $i]
			lappend l $e
		} else {lappend l {}}
	}
}
# determines the common width of each column
proc get_column_width { list num } {
	upvar $list l
	set colw {}
	for {set n 0} {$n < $num} {incr n
	} {
		set oldlen 0
		# ignore the first line (header for csv)
		for {set i [expr $num+$n]} {$i < [llength $l]} {incr i $num
		} {
			set elem [lindex $l $i]
			foreach e [split $elem "\n"] {
				set len [string length $e]
				if {$len > $oldlen} { set oldlen $len }
			}
		}
		lappend colw $oldlen
	}
	return $colw
}
# generates text from supplied list data,
# assumes $num blocks of 'descr value error ...' with
# $width number of entries per block
# The first element of the list is the max required column width.
proc list2text { list width num } {
	upvar $list l
	set pm " ± "
	set disp ""
	set exp ""
	set colw [get_column_width l [expr $width*$num]]
	for {set k 0} {$k < [llength $l]} {incr k $width
	} {
		set col_num [expr $k % ($width*$num)]
		# prepend line separator or field separator
		if {$k > 0} {
			if {$col_num == 0} {
				# end of a row
				append exp  "\n"
				append disp "\n"
			} else {
				# end of a block inside a row
				append exp  ";"
				append disp " "
			}
		}
		# ignore the first line for display text
		if {$k == ($width*$num)} { set disp "" }
		# process blockwise ($width elements)
		for {set i 0} {$i < $width} {incr i
		} {
			set elem [lindex $l [expr $k+$i]]
			if {$i > 0} {;# general column delimiter
				append exp ";"
				#append disp " "
			}
			# prepend the second and third column with 
			# 3 character width prefix
			# prepend columns > 3 width single character width
			if {[string length $elem]
			} {
				# in front of the value
				if {$i == 1} { append disp " = " 
				} elseif {$i == 2} { append disp "$pm"
				} elseif {$i > 2} { append disp " " }
			} else {
				if {$i == 1 || $i == 2} { append disp "   "
				} elseif {$i > 2} { append disp " " }
			}
			# finally, append the data
			append exp [join [split "$elem" "\n"] { }]
			# pick the column width for the current column
			append disp [shows $elem [lindex $colw [expr $col_num+$i]]]
		}
	}
	return [list $exp $disp]
}
proc shows { elem colw } {
	set i [string last "\n" $elem]
	set res ""
	if {$i > 0} {
		set res "[string range $elem 0 [expr $i-1]]\n"
		set elem [string range $elem [expr $i+1] end]
	}
	append res [format %${colw}s $elem]
	return $res
}
proc showf { val } {
        set retval " "
        catch {set retval [format "%1.4e" $val] }
	return $retval
}
proc moments_get_text { analytpar } {
	upvar $analytpar ap
	set PI 3.141592654
	set disp ""
	set exp ""
	set num 2
	set block 2
	set cols [expr $block*$num]
	set dl {};# data list
	for {set i 0} {$i < $num} {incr i} {
		add2list dl $block "description value"
	}
	for {set i 1} {$i <= $ap(max_SD)} {incr i} {
		if {$ap(isGlobal)} { GlobalAPindex ap p $i
		} else {             APindex ap p $i }
		if {$i > 1} {; # empty line separates contributions
			add2list dl $cols {}
		}
		# set description and meta-data for each contribution
		foreach blockEntry [list \
			"{scattering contribution} $i" \
			"calculate $p(calcSDFF)" \
			"{[get_title_by_id $p(SD,typestr)]}" \
			"{[get_title_by_id $p(FF,typestr)]}" \
			"<R^1> [showf $p(<R^1>)]" "<R^5> [showf $p(<R^5>)]" \
			"<R^2> [showf $p(<R^2>)]" "<R^6> [showf $p(<R^6>)]" \
			"<R^3> [showf $p(<R^3>)]" "<R^7> [showf $p(<R^7>)]" \
			"<R^4> [showf $p(<R^4>)]" "<R^8> [showf $p(<R^8>)]" \
			"R_lc [showf $p(R_lc)]" \
			"lc [showf [expr 2.0/3.0*$p(R_lc)]]" \
			"R_li [showf $p(R_li)]" \
			"li [showf [expr 3.0/4.0*$p(R_li)]]" \
			"R_Ac [showf $p(R_Ac)]" \
			"Ac [showf [expr 5.0/(4.0*$PI)*$p(R_Ac)*$p(R_Ac)]]" \
			"R_VP [showf $p(R_VP)]" \
			"VP [showf [expr 3.0/(4.0*$PI)*pow($p(R_VP),3.0)]]" \
			"R_RG [showf $p(R_RG)]" \
			"RG [showf [expr sqrt(3.0/5.0)*$p(R_RG)]]" \
			"fp [showf $p(fp)]" "" \
		] {
			add2list dl $block $blockEntry
		}
	}
	add2list dl $cols {}
	foreach blockEntry [list \
			"{scattering contribution\nof all size distrib.}" \
			"{sum N_i}  [showf $p(<sumR^0>)]" \
			"<sumR^1> [showf $p(<sumR^1>)]" \
			"<sumR^5> [showf $p(<sumR^5>)]" \
			"<sumR^2> [showf $p(<sumR^2>)]" \
			"<sumR^6> [showf $p(<sumR^6>)]" \
			"<sumR^3> [showf $p(<sumR^3>)]" \
			"<sumR^7> [showf $p(<sumR^7>)]" \
			"<sumR^4> [showf $p(<sumR^4>)]" \
			"<sumR^8> [showf $p(<sumR^8>)]" \
			"sumR_lc [showf $p(sumR_lc)]" \
			"sumlc [showf [expr 3.0/2.0*$p(sumR_lc)]]" \
			"sumR_li [showf $p(sumR_li)]" \
			"sumli [showf [expr 4.0/3.0*$p(sumR_li)]]" \
			"sumR_Ac [showf $p(sumR_Ac)]" \
			"sumAc [showf [expr 4.0*$PI/5.0*$p(sumR_Ac)*$p(sumR_Ac)]]" \
			"sumR_VP [showf $p(sumR_VP)]" \
			"sumVP [showf [expr 4.0*$PI/3.0*pow($p(sumR_VP),3.0)]]" \
			"sumR_RG [showf $p(sumR_RG)]" \
			"sumRG [showf [expr sqrt(3.0/5.0)*$p(sumR_RG)]]" \
			"sumfp(Sph.) [showf $p(sumfp)]" "" \
	] {
		add2list dl $block $blockEntry
	}
	set res [list2text dl $block $num]
	set ap($::sasfit(mom_name)_text_export) [lindex $res 0]
	set ap($::sasfit(mom_name)_text_display) [lindex $res 1]	
}
proc analytpar_get_text { analytpar } {
	upvar $analytpar ap

	# local helper function
	proc getTail { apar k type } {
		upvar $apar p
		set tail {}
		if {$p($type,[get_old_param_name $type]$k,active)} {
			lappend tail $p($type,[get_old_param_name $type]$k,err)
		} else { lappend tail {} }
		if {[info exists p($type,[get_old_param_name $type]$k,factor)]
		} { 
			set fac $p($type,[get_old_param_name $type]$k,factor)
			if { $fac == 1.0 } { lappend tail {}
			} else {             lappend tail $fac }
		} else { lappend tail {} }
		if {[info exists p($type,[get_old_param_name $type]$k,common)]
		} {
			set com $p($type,[get_old_param_name $type]$k,common) 
			if { $com == "NONE" || $com == "NEW" } { lappend tail {}
			} else {                                 lappend tail $com }
		} else { lappend tail {} }
		return $tail
	}
	proc getApproxType { actualAP } {
		upvar $actualAP p
		switch $p(SQ,how) {
			0 { return "monodisperse approx." }
			1 { return "decoupling approach" }
			2 { return "local monodisperse approx." }
			3 { return "partial structure factor" }
			4 { return "scaling approx." }
			default { return $p(SQ,how) }
		}
	}

	set disp ""
	set exp ""
	# list with data, 3 functions * (descr,val,err) = 9 columns
	set num 3
	set block 3
	set dl {};# data list
	set exp_header "description value error"
	if {$ap(isGlobal)} { 
		set block 5;# factor&global params
		set exp_header "$exp_header factor global_param"
	}
	for {set i 0} {$i < $num} {incr i} {
		add2list dl $block $exp_header
	}
	set cols [expr $block*$num]
	for {set i 1} {$i <= $ap(max_SD)} {incr i} {
		if {$ap(isGlobal)} { GlobalAPindex ap p $i
		} else {             APindex ap p $i }

		if {$i > 1} {; # empty line separates contributions
			add2list dl $cols {}
		}
		# set description and meta-data for each contribution (header)
		set headerList [list \
			"{scattering contribution} $i" \
			"{approximation type} {[getApproxType p]}" \
			"" \
			"calculate $p(calcSDFF)" \
			"substract $p(substrSDFF)" \
			"fit $p(fitSDFF)" ]
		if {$ap(isGlobal)} {
			lappend headerList "dataset $p(dataset)" \
					"label {$p(datalabel)}" \
					"" 
		}
		lappend headerList "{[get_title_by_id $p(SD,typestr)]}" \
				"{[get_title_by_id $p(FF,typestr)]}" \
				"{[get_title_by_id $p(SQ,typestr)]}"

		foreach blockEntry $headerList {
			add2list dl $block $blockEntry
		}

		# set parameter data for each model function
		for {set k 1} {$k <= $p(SD,param_count) ||
		               $k <= $p(FF,param_count) ||
		               $k <= $p(SQ,param_count)} {incr k
		} {
			set row {}
			# check if this function has enough parameters
			if { $k <= $p(SD,param_count) && [string length $p(SD,a$k,label)] != 0
			} {
				add2list row $block "{[string range $p(SD,a$k,label) 0 end-2]} $p(SD,a$k,var) [getTail p $k SD]"
			} else {add2list row $block {} }
			if { $k <= $p(FF,param_count) && [string length $p(FF,l$k,label)] != 0
			} {
				set value $p(FF,l$k,var)
				if {$p(FF,l$k,distr)} {  set value "distr." }
				add2list row $block "{[string range $p(FF,l$k,label) 0 end-2]} $value [getTail p $k FF]"
			} else {add2list row $block {} }
			if { $k <= $p(SQ,param_count) && [string length $p(SQ,s$k,label)] != 0
			} {
				add2list row $block "{[string range $p(SQ,s$k,label) 0 end-2]} $p(SQ,s$k,var) [getTail p $k SQ]"
			} else {add2list row $block {} }
			# check this row for entries, ignore it otherwise
			set addCurrentList 0
			foreach elem $row {
				if {[llength $elem] > 0} { 
					set addCurrentList 1 
					break
				}
			}
			# finally, add this list if non-empty
			if {$addCurrentList} { add2list dl $cols $row }
		};# foreach parameter (row)
	};# foreach contribution

	set res [list2text dl $block $num]
	set ap($::sasfit(par_name)_text_export) [lindex $res 0]
	set ap($::sasfit(par_name)_text_display) [lindex $res 1]	

}
proc structpar_get_data_descr { StructParData } {
	upvar $StructParData spd
	set result [list "1st data point (ascending)" \
	"number of points" "Q_min" "Q_max" "Guinier radius RG" \
	"forward scattering I(0)" "chisq($spd(I0typestr))"]
	if {[llength [array names spd guinier_lin*]]} {
		set result [concat $result [list \
			"linear Guinier approx I(0)" \
			"linear Guinier approx RG" \
			"linear Guinier approx RG^2" \
			"linear Guinier approx chisq"]]
	}
	set result [concat $result [list \
	"1st data point (descending)" "number of points" \
	"Q_min" "Q_max" "background c_0" "Porod constant c_4" \
	"potential law D" "chisq(Porod)" "correlation length lc" \
	"correlation area Ac" "intersection length li" "Porod volume VP" \
	"Porod radius RP" "specific surface S/V" "scattering invariant Inv" \
	"integrated intensity iI = int(I(Q)*Q)dQ" \
	"integral intensity intI = int(I(Q))dQ" "Q_min" "Q_max"]]
	return $result
}
# The value list contains additionally an error flag. If there is an error 
# value associated with it and at which position in the error list to find.
# This is required to avoid missing/vanished error values to be interpreted 
# as not existing/associated/required.
proc structpar_get_data_value { StructParData } {
	upvar $StructParData spd
	set result [list "{$spd(Guinierfirst)} -1" "{$spd(Guiniernpoints)} -1" \
	"{[lindex $::sasfit(Q) [expr $spd(Guinierfirst)-1]]} -1" \
	"{[lindex $::sasfit(Q) [expr $spd(Guinierfirst)+$spd(Guiniernpoints)-2]]} -1" \
	"{[showf $spd(RG)]} 0" "{[showf $spd(I0)]} 1" "{$spd(I0chisq)} -1"]
	if {[llength [array names spd guinier_lin*]]} {
		set result [concat $result [list \
			"{[showf $spd(guinier_lin_i0)]} 14" \
			"{[showf $spd(guinier_lin_rg)]} 15" \
			"{[showf $spd(guinier_lin_rg2)]} 16" \
			"{[showf $spd(guinier_lin_chisq)]} -1"]]
	}
	set result [concat $result [list \
	"{$spd(Porodfirst)} -1" "{$spd(Porodnpoints)} -1" \
	"{[lindex $::sasfit(Q) end-[expr $spd(Porodfirst)+$spd(Porodnpoints)-2]]} -1" \
	"{[lindex $::sasfit(Q) end-[expr $spd(Porodfirst)-1]]} -1" \
	"{[showf $spd(c0)]} 2" "{[showf $spd(c4)]} 3" "{[showf $spd(PorodD)]} 4" \
	"{$spd(Porodchisq)} -1" "{[showf $spd(lc)]} 5" "{[showf $spd(Ac)]} 6" \
	"{[showf $spd(li)]} 7" "{[showf $spd(VP)]} 8" "{[showf $spd(RP)]} 9" \
	"{[showf $spd(S/V)]} 10" "{[showf $spd(Invariant)]} 11" "{[showf $spd(iI)]} 12" \
	"{[showf $spd(intI)]} 13" "{$spd(Qmin)} -1" "{$spd(Qmax)} -1" ]]
	return $result
}
proc structpar_get_data_error { StructParData } {
	upvar $StructParData spd
	set result [list "[showf $spd(DRG)]" "[showf $spd(DI0)]" "[showf $spd(Dc0)]" \
	"[showf $spd(Dc4)]" "[showf $spd(DPorodD)]" "[showf $spd(Dlc)]" \
	"[showf $spd(DAc)]" "[showf $spd(Dli)]" "[showf $spd(DVP)]" \
	"[showf $spd(DRP)]" "[showf $spd(DS/V)]" "[showf $spd(DInvariant)]" \
	"[showf $spd(DiI)]" "[showf $spd(DintI)]" ]
	if {[llength [array names spd guinier_lin*]]} {
		lappend result "[showf $spd(guinier_lin_i0_err)]"
		lappend result "[showf $spd(guinier_lin_rg_err)]" 
		lappend result "[showf $spd(guinier_lin_rg2_err)]"
	}
	return $result
}

proc structpar_get_text { StructParData 
} {
	proc addData { datalist l0 l1 l2 start end 
	} {
		upvar $datalist dl $l0 c0 $l1 c1 $l2 c2
		for {set i $start} {$i < $end} {incr i} {
			set val_err [lindex $c1 $i]
			set errIdx [lindex $val_err 1]
			set err ""
			if {$errIdx >= 0} { 
				set err [lindex $c2 $errIdx]
			}
			add2list dl 3 "{[lindex $c0 $i]} {[lindex $val_err 0]} $err"
		}
	}
	upvar $StructParData spd
	# get lists for description, value and error
	set dl {};# max entry length
	set c0 [structpar_get_data_descr spd]
	set c1 [structpar_get_data_value spd]
	set c2 [structpar_get_data_error spd]
	if {[llength $c0] != [llength $c1] 
	} {
		puts stderr "Data source lists do not match !"
		puts stderr "'[llength $c0]' != '[llength $c1]'"
	}
	foreach row [list "description value error" "{$spd(lowQText)}"] {
		add2list dl 3 $row
	}
	set numGuinier 7
	if {[llength [array names spd guinier_lin*]]} { set numGuinier 11 }
	set numPorod 8
	addData dl c0 c1 c2 0 $numGuinier
	foreach row [list "" "{$spd(largeQText)}"] {
		add2list dl 3 $row
	}
	addData dl c0 c1 c2 $numGuinier [expr $numGuinier+$numPorod]
	foreach row [list "" "{integral structural parameters:}"] {
		add2list dl 3 $row
	}
	addData dl c0 c1 c2 [expr $numGuinier+$numPorod] [llength $c0]
	set res [list2text dl 3 1]
	set spd($::sasfit(isp_name)_text_export) [lindex $res 0]
	set spd($::sasfit(isp_name)_text_display) [lindex $res 1]
}

proc RefreshStructParDataTab {
} {
	structpar_get_text ::StructParData

	set n $::sasfit(isp_name)
	tab_text_update ::StructParData $n
}

#------------------------------------------------------------------------------
#                  Menu for setting Fit Range
#
proc Set_FitRange {XYGraph} {
#^^^^^^^^^^^^^^^^^^^^^
global sasfit addsasfit sf
global IQGraph GlobalFitIQGraph
if {[winfo exists .setfitrange]} {destroy .setfitrange}
toplevel .setfitrange
set w .setfitrange
wm geometry $w
wm title $w "Set Fit Range"
raise $w
focus $w
grab $w
frame $w.lay1
frame $w.lay2
frame $w.lay3
pack $w.lay1 $w.lay2 $w.lay3 -padx 2m -pady 2m -fill both

if {[string compare $XYGraph GlobalFitIQGraph] == 0} {
   cp_arr addsasfit sf
} else {
   cp_arr sasfit sf
}

set sf(graph) $XYGraph
label $w.lay1.lQ -text "lower Q:" \
      -width 7 -anchor w -highlightthickness 0
entry $w.lay1.lQvalue -textvariable sf(lower,Q) \
      -width 16 -relief sunken -highlightthickness 0
label $w.lay1.uQ -text "upper Q:" \
      -width 7 -anchor w -highlightthickness 0
entry $w.lay1.uQvalue -textvariable sf(upper,Q) \
      -width 16 -relief sunken -highlightthickness 0

grid $w.lay1.lQ      -column 0 -row 0
grid $w.lay1.lQvalue -column 1 -row 0
grid $w.lay1.uQ      -column 3 -row 0
grid $w.lay1.uQvalue -column 4 -row 0
grid columnconfigure $w.lay1 2 -minsize 2m
button $w.lay2.ok -text Ok -highlightthickness 0 \
       -command {
          global sf sasfit IQGraph GlobalFitIQGraph
          if {[string compare $sf(graph) GlobalFitIQGraph] == 0} {
             cp_arr sf addsasfit
             set $sf(graph)(lower,x) $addsasfit(lower,Q)
             set $sf(graph)(upper,x) $addsasfit(upper,Q)
             set sasfit(fitrange) yes
             Make_Square_el $sf(graph) $addsasfit(lower,Q) $addsasfit(upper,Q)
	  } else {
             cp_arr sf sasfit
             set $sf(graph)(lower,x) $sasfit(lower,Q)
             set $sf(graph)(upper,x) $sasfit(upper,Q)
             set sasfit(fitrange) yes
             Make_Square_el $sf(graph) $sasfit(lower,Q) $sasfit(upper,Q)
	  }
          catch {destroy .setfitrange}
          RefreshGraph $sf(graph)
      }
button $w.lay2.apply -text Apply -highlightthickness 0 \
       -command {
          global sf sasfit IQGraph GlobalFitIQGraph
          if {[string compare $sf(graph) GlobalFitIQGraph] == 0} {
             cp_arr sf addsasfit
             set $sf(graph)(lower,x) $addsasfit(lower,Q)
             set $sf(graph)(upper,x) $addsasfit(upper,Q)
             set addsasfit(fitrange) yes
             Make_Square_el $sf(graph) $addsasfit(lower,Q) $addsasfit(upper,Q)
	  } else {
             cp_arr sf sasfit
             set $sf(graph)(lower,x) $sasfit(lower,Q)
             set $sf(graph)(upper,x) $sasfit(upper,Q)
             set sasfit(fitrange) yes
             Make_Square_el $sf(graph) $sasfit(lower,Q) $sasfit(upper,Q)
	  }
          RefreshGraph $sf(graph)
      }
button $w.lay2.cancel -text Cancel -highlightthickness 0 \
       -command {
          global sf
          unset sf
          catch {destroy .setfitrange}
       }
pack $w.lay2.ok $w.lay2.apply $w.lay2.cancel \
     -side left -fill both -padx 2m
label $w.lay3.info -text "fit range can also be set by pressing\n<Shift-MouseLeft> button on the \"intensity\" plot" -justify left -anchor w
pack $w.lay3.info -side left
#set sasfit(lower,Q)
#set sasfit(upper,Q) 
#set IQGraph(lower,x)
#set IQGraph(upper,x)


}

proc sasfit_menubar_build { p } {

   menubutton $p.file -text File -underline 0 \
              -menu $p.file.menu
      menu $p.file.menu
      $p.file.menu add command -label "Single Data Set..." \
                                 -underline 1 \
                                 -command MergeCmd
#      setTooltip $p.file.menu -index "Open..." "Reads a data file\n(ASCII,BerSANS)"

         $p.file.menu add command -label "Multiple Data Sets..." \
                                    -underline 0 \
                                    -command {
					    set ::show_new_file_dialog 0
					    AddCmd
					    }
#         setTooltip $p.file.menu -index "Add..." "Adding data set for global fitting"
       
      $p.file.menu add command -label "Load project ..." \
                                 -underline 0 \
                                 -command {LoadCmd {}}
      $p.file.menu add command -label "Save project" \
                                 -underline 0 \
                                 -command SaveCmd
      $p.file.menu add command -label "Save project as ..." \
                                 -underline 1 \
                                 -command SaveAsCmd

#
# print menu bar cascade
#
      $p.file.menu add cascade -label "Print..." \
                -menu $p.file.menu.print
      [menu $p.file.menu.print -tearoff 0] add command -label "I(Q)..." \
                                 -underline 0 \
                                 -command {
                                    create_ps_and_send_it_to_printer \
					    print_IQGraph
                                 }
      $p.file.menu.print add command -label "Global I(Q)..." \
                                 -underline 0 \
                                 -command {
                                    create_ps_and_send_it_to_printer \
					    print_GlobalFitIQGraph
                                 }
      $p.file.menu.print add command -label "residuum..." \
                                 -underline 0 \
                                 -command {
                                    create_ps_and_send_it_to_printer \
					    print_ResIQGraph
                                 }
      $p.file.menu.print add command -label "N(R)..." \
                                 -underline 0 \
                                 -command {
                                    create_ps_and_send_it_to_printer \
					    print_SDGraph
                                 }
#
# export menu bar cascade
#
      $p.file.menu add cascade -label "Export" \
                -menu $p.file.menu.export
#      setTooltip $p.file.menu -index Export "Export Data"
      [menu $p.file.menu.export -tearoff 0] add command -label "I(Q) ..." \
                                 -underline 0 \
                                 -command {
                                    export_blt_graph IQGraph
                                 }
      $p.file.menu.export add command -label "I(Q) global ..." \
                                 -underline 0 \
                                 -command {
                                    export_blt_graph GlobalFitIQGraph
                                 }
#      setTooltip $p.file.menu.export -index "I(Q)..." \
#                                "Export data of scattering\nintensity I(Q) plot"

      $p.file.menu.export add command -label "Residuum ..." \
                                 -underline 0 \
                                 -command {
                                    export_blt_graph ResIQGraph
                                 }
#      setTooltip $p.file.menu.export -index "Residuum..." \
#         "Export data of residual intensity\n(I_exp(Q)-I_th(Q))/Delta I(Q) plot"
      $p.file.menu.export add command -label "N(R) ..." \
                                 -underline 0 \
                                 -command {
                                    export_blt_graph SDGraph
                                 }
#      setTooltip $p.file.menu.export -index "N(R)..." \
#                                  "Export data of\nsize distribution N(R) plot"

#
#
#
      $p.file.menu add command -label "Exit" \
                                 -underline 0 \
                                 -command exit_cmd
#      setTooltip $p.file.menu -index "Exit" "Exit program"


   menubutton $p.fit -text Calc -underline 0 \
              -menu $p.fit.menu
      menu $p.fit.menu
      $p.fit.menu add command -label "integral parameters ..." \
      				-underline 0 \
                                -command structuralParFitCmd
#      setTooltip $p.fit.menu -index "integral parameters ..." \
#                            "fitting structural integral parameters to data set"

      $p.fit.menu add command -label "DLS ..." \
      				-underline 0 \
                                -command g2_1_ParFitCmd
#      setTooltip $p.fit.menu -index "DLS ..." \
#                               "fitting DLS curves"

      $p.fit.menu add cascade -label "Single Data Set" \
	      -menu $p.fit.menu.single \
	      -underline 0
      [menu $p.fit.menu.single -tearoff 0] add command \
	      -label  "fit..." \
	      -underline 0 \
	      -command {analyticalSDCmd fit}
      $p.fit.menu.single add command \
	      -label  "simulate..." \
	      -underline 0 \
	      -command {analyticalSDCmd sim}

       $p.fit.menu add cascade -label "Multiple Data Sets" \
           -menu $p.fit.menu.multiple \
	   -underline 0
       [menu $p.fit.menu.multiple -tearoff 0] add command \
   	       -label  "fit..." \
	       -underline 0 \
	       -command {analyticalGlobalSDCmd fit}
       $p.fit.menu.multiple add command \
	       -label  "simulate..." \
	       -underline 0 \
	       -command {analyticalGlobalSDCmd simulate}
       
      $p.fit.menu add command -label "Ornstein Zernike solver" \
	      -command {sasfit_OZ_solver} \
	       -underline 0 
   menubutton $p.tools -text Tools -underline 0 \
              -menu $p.tools.menu
      menu $p.tools.menu
      $p.tools.menu add command -label "SLD calculator" \
	      -command {SLDCalculator} -underline 0
      $p.tools.menu add command -label "toggle console" -underline 0 \
	      -command {
		      if {[info exists ::tkcon_visible] && $::tkcon_visible} {
			      tkcon hide
			      set ::tkcon_visible 0
		      } else {
			      tkcon show
			      set ::tkcon_visible 1
		      }
		}
      $p.tools.menu add command -label "create new plugin" \
	      -command "sasfit_plugin_guide $p" -underline 0

   sasfit_menubar_build_help $p.help

   pack $p.file $p.fit $p.tools -side left 
   pack $p.help -side right 
}

proc sasfit_menubar_build_help { path } {
	menubutton $path -text Help -underline 0 -menu $path.menu
	menu $path.menu
	$path.menu add command -label "Documentation" \
		-command {$::sasfit(webbrowser) "file://$sasfit(docdir)/manual/sasfit.pdf" }
	$path.menu add command -label "About" -command "AboutCmd 0"
	return $path.menu
}

#------------------------------------------------------------------------------
#                    Create main menu bar with drawing area
#
proc sasfitmenu {} {
#^^^^^^^^^^^^^^^
   global IQGraph
   global SDGraph
   global collectIQGraph
   global collectSDGraph
   global sasfit
   global ASCIIData
   global IQGraph ResIQGraph SDGraph CollectIQGraph CollectSDGraph \
          StructParData GlobalFitIQGraph Detector2DIQGraph
   global AnalytPar GlobalAnalytPar
   global FitPrecision
   global zoomMod

   # Blt_ZoomStack modifier, see
   # saskit/kit/lib/blt2.4/graph.tcl, line 85
   set zoomMod "Control-"
   proc zoomstack { graph } {
       Blt_ZoomStack $graph
   }

   frame .top -relief raised -bd 2 
   pack .top -fill x -side top
   sasfit_menubar_build .top

frame .obW 
   pack .obW -fill both -expand yes 
#------------------------------------------------------------------------------
#                           Creat quick message bar

frame .messageframe -relief sunken -borderwidth 2 -height 5m
   pack .messageframe -side bottom -fill x
   pack propagate .messageframe yes
frame .messageframe.m -height 5m -width 70m
   pack .messageframe.m  -fill x -expand yes
   message .quickmessage -text "" \
           -justify left -aspect 3000 -anchor w 
   message .xycoordinates -text "    " \
           -justify right -aspect 3000 -anchor e 
   pack .quickmessage  -side left -fill x -expand yes -in .messageframe.m
   pack .xycoordinates -side left -fill x -in .messageframe.m
   
#------------------------------------------------------------------------------
#                               Create plot tabs

#
#  creating tabset
#
   blt::tabset .obW.tab -relief sunken -borderwidth 2 
   pack .obW.tab -fill both -expand yes
#
#  create scattering intesity plot "IQGraph"
#
frame .obW.tab.iq -relief groove -borderwidth 2 
   set IQGraph(w) .obW.tab.iq.draw
   set IQGraph(e,element) 0
   CreateGraph IQGraph
   pack $IQGraph(w) -in .obW.tab.iq
   pack configure $IQGraph(w) -fill both -expand yes

   .obW.tab insert 0 IQGraph
   .obW.tab tab configure IQGraph -text "fit of single\ndata set"
   .obW.tab tab configure IQGraph -fill both -padx 0.1i -pady 0.1i \
            -window .obW.tab.iq -windowheight $sasfit(height) -windowwidth $sasfit(width) 

##
## creating IQ popup menu
##
menu .obW.tab.iq.popup -tearoff 0
.obW.tab.iq.popup add command -label "Print..." -un 0 -command {
     global IQGraph
     create_ps_and_send_it_to_printer print_IQGraph
     }
.obW.tab.iq.popup add command -label "copy to clipboard (wmf)" -un 0 -command {
     window_to_clipboard .obW.tab.iq
     }
.obW.tab.iq.popup add command -label "copy to clipboard (ascii)" -un 0 -command {
     global IQGraph
     export_clipboard_data IQGraph
     }
.obW.tab.iq.popup add command -label "paste data from clipboard (ascii)" -un 0 -command {
     PasteClipboardData2IQGraph single
     }
.obW.tab.iq.popup add command -label "paste&average  data from clipboard (ascii)" -un 0 -command {
     PasteClipboardData2IQGraph average
     }

.obW.tab.iq.popup add cascade -label "Graph" -un 0 -menu .obW.tab.iq.popup.graph
set m3 [menu .obW.tab.iq.popup.graph -tearoff 1]
$m3 add command -label "autoscale" \
    -command { 
       set IQGraph(x,min) ""
       set IQGraph(x,max) ""
       set IQGraph(y,min) ""
       set IQGraph(y,max) ""
       RefreshGraph IQGraph
    }

$m3 add command -label "set fit range" \
    -command { Set_FitRange IQGraph }

$m3 add command -label "delete fit range" \
    -command { Del_Square_el IQGraph }

$m3 add command -label "x-axis..." -command {
                                      global IQGraph
                                      set_xaxis_layout IQGraph
                                   }
$m3 add command -label "y-axis..." -command {
                                      set_yaxis_layout IQGraph
                                   }
$m3 add command -label "graph layout..." -command {
                                           set_graph_layout IQGraph
                                         }
$m3 add command -label "plot layout..." -command {
                                           set_plot_layout IQGraph
                                        }
.obW.tab.iq.popup add command -label "Export Data..." \
    -command { export_blt_graph IQGraph }
bind .obW.tab.iq.draw <ButtonPress-3> {tk_popup .obW.tab.iq.popup %X %Y }
bind .obW.tab.iq.draw <Double-ButtonPress-1> {tk_popup .obW.tab.iq.popup %X %Y }
zoomstack $IQGraph(w)



#
#  create scattering 2D detector intesity plot "2DdetectorIQGraph"
#

#   frame .obW.tab.detector2Diq -relief groove -borderwidth 2 -background white  

   if {$sasfit(width) < $sasfit(height)} {
      set width2D $sasfit(width)
   } else {
      set width2D $sasfit(height)
   }
   set Detector2DIQGraph(sw) [ScrolledWindow .obW.tab.detector2Diq -auto none]
   set Detector2DIQGraph(fw) [ScrollableFrame $Detector2DIQGraph(sw).sf -width $width2D -height $width2D]
   set Detector2DIQGraph(w) [$Detector2DIQGraph(fw) getframe]
   $Detector2DIQGraph(sw) setwidget $Detector2DIQGraph(fw)

   .obW.tab insert 6 Detector2DIQGraph
   .obW.tab tab configure Detector2DIQGraph \
            -text "2D detector\nintensity"
   if {$sasfit(width) < $sasfit(height)} {
      set width2D $sasfit(width)
   } else {
      set width2D $sasfit(height)
   }
   .obW.tab tab configure Detector2DIQGraph -fill both \
            -padx 0.1i -pady 0.1i \
            -window $Detector2DIQGraph(sw)  \
            -windowheight $width2D \
            -windowwidth $width2D 

   set Detector2DIQGraph(cw) [ canvas $Detector2DIQGraph(w).c \
                                      -background white \
                                      -width $width2D \
                                      -height $width2D]

   pack $Detector2DIQGraph(cw) 
##
## creating Detector2DIQGraph popup menu
##
menu $Detector2DIQGraph(w).popup -tearoff 0
$Detector2DIQGraph(w).popup add command -label "copy to clipboard" -un 0 -command {
     window_to_clipboard $Detector2DIQGraph(cw)
     }
bind $Detector2DIQGraph(w) <ButtonPress-3> {tk_popup $Detector2DIQGraph(w).popup %X %Y }
bind $Detector2DIQGraph(w) <Double-ButtonPress-1> {tk_popup $Detector2DIQGraph(w).popup %X %Y }

   set Detector2DIQGraph(nPix) 64
   set s [::Plotchart::createIsometricPlot $Detector2DIQGraph(cw) \
               [list 0.0 $Detector2DIQGraph(nPix)] \
               [list 0.0 $Detector2DIQGraph(nPix)] noaxes] 
   set Detector2DIQGraph(s) $s


#
#  create scattering intesity plot "GlobalFitIQGraph"
#
frame .obW.tab.globaliq -relief groove -borderwidth 2 
   set GlobalFitIQGraph(w) .obW.tab.globaliq.draw
   set GlobalFitIQGraph(e,element) 0
   CreateGraph GlobalFitIQGraph
   pack $GlobalFitIQGraph(w) -in .obW.tab.globaliq
   pack configure $GlobalFitIQGraph(w) -fill both -expand yes

   .obW.tab insert 1 GlobalFitIQGraph
   .obW.tab tab configure GlobalFitIQGraph -text "multiple data sets\nfor global fitting"
   .obW.tab tab configure GlobalFitIQGraph -fill both -padx 0.1i -pady 0.1i \
            -window .obW.tab.globaliq -windowheight $sasfit(height) -windowwidth $sasfit(width) 

##
## creating GlobalFitIQ popup menu
##
menu .obW.tab.globaliq.popup -tearoff 0
.obW.tab.globaliq.popup add command -label "Print..." -un 0 -command {
     global GlobalFitIQGraph
     create_ps_and_send_it_to_printer print_GlobalFitIQGraph
     }
.obW.tab.globaliq.popup add command -label "copy to clipboard (wmf)" -un 0 -command {
     window_to_clipboard .obW.tab.globaliq
     }
.obW.tab.globaliq.popup add command -label "copy to clipboard (ascii)" -un 0 -command {
     global GlobalFitIQGraph
     export_clipboard_data GlobalFitIQGraph
     }
.obW.tab.globaliq.popup add command -label "paste data from clipboard (ascii)" -un 0 -command {
     PasteClipboardData2GlobalIQGraph single
     }
.obW.tab.globaliq.popup add command -label "paste&average data from clipboard (ascii)" -un 0 -command {
     PasteClipboardData2GlobalIQGraph average
     }

.obW.tab.globaliq.popup add cascade -label "Graph" -un 0 -menu .obW.tab.globaliq.popup.graph
set m3 [menu .obW.tab.globaliq.popup.graph -tearoff 1]
$m3 add command -label "autoscale" \
    -command { 
       set GlobalFitIQGraph(x,min) ""
       set GlobalFitIQGraph(x,max) ""
       set GlobalFitIQGraph(y,min) ""
       set GlobalFitIQGraph(y,max) ""
       RefreshGraph GlobalFitIQGraph
    }

$m3 add command -label "set fit range" \
    -command { Set_FitRange GlobalFitIQGraph }

$m3 add command -label "delete fit range" \
    -command { Del_Square_el GlobalFitIQGraph }



$m3 add command -label "x-axis..." -command {
                                      global GlobalFitIQGraph
                                      set_xaxis_layout GlobalFitIQGraph
                                   }
$m3 add command -label "y-axis..." -command {
                                      set_yaxis_layout GlobalFitIQGraph
                                   }
$m3 add command -label "graph layout..." -command {
                                           set_graph_layout GlobalFitIQGraph
                                         }
$m3 add command -label "plot layout..." -command {
                                           set_plot_layout GlobalFitIQGraph
                                        }
.obW.tab.globaliq.popup add command -label "Export Data..." \
    -command { export_blt_graph GlobalFitIQGraph }
bind .obW.tab.globaliq.draw <ButtonPress-3> {tk_popup .obW.tab.globaliq.popup %X %Y }
bind .obW.tab.globaliq.draw <Double-ButtonPress-1> {tk_popup .obW.tab.globaliq.popup %X %Y }
zoomstack $GlobalFitIQGraph(w)





##
##  create residual plot "ResIQGraph"
##
frame .obW.tab.residual -relief groove -borderwidth 2 
   set ResIQGraph(w) .obW.tab.residual.draw
   set ResIQGraph(e,element) 0
   CreateGraph ResIQGraph
   set ResIQGraph(x,logscale) 0
   set ResIQGraph(y,logscale) 0
   set ResIQGraph(x,type)     x
   set ResIQGraph(y,type)     y
   pack $ResIQGraph(w) -in .obW.tab.residual
   pack configure $ResIQGraph(w) -fill both -expand yes
   .obW.tab insert 2 ResIQGraph
   .obW.tab tab configure ResIQGraph -text residuum
   .obW.tab tab configure ResIQGraph -fill both -padx 0.1i -pady 0.1i \
            -window .obW.tab.residual -windowheight $sasfit(height)  -windowwidth $sasfit(width) 

#
# creating ResIQ popup menu
#
menu .obW.tab.residual.popup -tearoff 0
.obW.tab.residual.popup add command -label "Print..." -un 0 -command {
     global ResIQGraph
     create_ps_and_send_it_to_printer print_ResIQGraph
     }
.obW.tab.residual.popup add command -label "copy to clipboard (wmf)" -un 0 -command {
     window_to_clipboard .obW.tab.residual
     }
.obW.tab.residual.popup add command -label "copy to clipboard (ascii)" -un 0 -command {
     global ResIQGraph
     export_clipboard_data ResIQGraph
     }
.obW.tab.residual.popup add cascade -label "Graph" -un 0 \
                    -menu .obW.tab.residual.popup.graph
set m3 [menu .obW.tab.residual.popup.graph -tearoff 1]
$m3 add command -label "autoscale" \
    -command {
       set ResIQGraph(x,min) ""
       set ResIQGraph(x,max) ""
       set ResIQGraph(y,min) ""
       set ResIQGraph(y,max) ""
       RefreshGraph ResIQGraph
    }

$m3 add command -label "x-axis..." -command {
                                      global ResIQGraph
                                      set_xaxis_layout ResIQGraph
                                   }
$m3 add command -label "y-axis..." -command {
                                      set_yaxis_layout ResIQGraph
                                   }
$m3 add command -label "graph layout..." -command {
                                            set_graph_layout ResIQGraph
                                         }
$m3 add command -label "plot layout..." -command {
                                           set_plot_layout ResIQGraph
                                        }
.obW.tab.residual.popup add command -label "Export Data..." \
    -command { export_blt_graph ResIQGraph }
bind .obW.tab.residual.draw <ButtonPress-3> {tk_popup .obW.tab.residual.popup %X %Y }
bind .obW.tab.residual.draw <Double-ButtonPress-1> \
     {tk_popup .obW.tab.residual.popup %X %Y }
zoomstack $ResIQGraph(w)


#
#  create size distribution plot "SDGraph"
#
frame .obW.tab.sizedistr -relief groove -borderwidth 2 
   set SDGraph(w) .obW.tab.sizedistr.draw
   set SDGraph(e,element) 0
   CreateGraph SDGraph
   set SDGraph(x,logscale) 0
   set SDGraph(y,logscale) 0
   set SDGraph(x,type)     x
   set SDGraph(y,type)     "y*pow(x,3)"
   set SDGraph(x,title) "D / nm"
   set SDGraph(y,title) "N(D)"
   pack $SDGraph(w) -in .obW.tab.sizedistr
   pack configure $SDGraph(w) -fill both -expand yes
#   .obW.tab manage [list "size distrib." \
#            .obW.tab.sizedistr {-padx 10 -pady 10}]
#
   .obW.tab insert 3 SDGraph
   .obW.tab tab configure SDGraph -text "size distrib."
   .obW.tab tab configure SDGraph -fill both -padx 0.1i -pady 0.1i \
            -window .obW.tab.sizedistr -windowheight $sasfit(height) -windowwidth $sasfit(width) 
#
# creating SD popup menu
#
menu .obW.tab.sizedistr.popup -tearoff 0
.obW.tab.sizedistr.popup add command -label "Print..." -un 0 -command {
     global SDGraph
     create_ps_and_send_it_to_printer print_SDGraph
     }
.obW.tab.sizedistr.popup add command -label "copy to clipboard (wmf)" -un 0 -command {
     window_to_clipboard .obW.tab.sizedistr
     }
.obW.tab.sizedistr.popup add command -label "copy to clipboard (ascii)" -un 0 -command {
     global SDGraph
     export_clipboard_data SDGraph
     }
.obW.tab.sizedistr.popup add cascade -label "Graph" -un 0 \
                     -menu .obW.tab.sizedistr.popup.graph
set m3 [menu .obW.tab.sizedistr.popup.graph -tearoff 1]
$m3 add command -label "autoscale" \
    -command {
       set SDGraph(x,min) ""
       set SDGraph(x,max) ""
       set SDGraph(y,min) ""
       set SDGraph(y,max) ""
       RefreshGraph SDGraph
    }

$m3 add command -label "x-axis..." -command {
                                      global SDGraph
                                      set_xaxis_layout SDGraph
                                   }
$m3 add command -label "y-axis..." -command {
                                      set_yaxis_layout SDGraph
                                   }
$m3 add command -label "graph layout..." -command {
                                            set_graph_layout SDGraph
                                         }
$m3 add command -label "plot layout..." -command {
                                           set_plot_layout SDGraph
                                        }
.obW.tab.sizedistr.popup add command -label "Export Data..." \
    -command { export_blt_graph SDGraph }
bind .obW.tab.sizedistr.draw <ButtonPress-3> \
     {tk_popup .obW.tab.sizedistr.popup %X %Y }
bind .obW.tab.sizedistr.draw <Double-ButtonPress-1> \
     {tk_popup .obW.tab.sizedistr.popup %X %Y }
zoomstack $SDGraph(w)

#
#  create info page about moments of analytical size distributions
#
	set ::sasfit(isp_name) "isp"
	set ::sasfit(par_name) "par"
	set ::sasfit(mom_name) "mom"

	create_tab_text analytmoments $::sasfit(mom_name) \
		"moments of_analytical_size distrib." \
		::AnalytPar ::GlobalAnalytPar

	create_tab_text analytpar $::sasfit(par_name) \
		"parameters of_analytical_size distrib." \
		::AnalytPar ::GlobalAnalytPar
#
#  create structural parameter info page
#

	create_tab_text structpar $::sasfit(isp_name) \
		"integral_structural_parameters" \
		::StructParData

	RefreshStructParDataTab
	RefreshAnalytParDataTab AnalytPar


bind $IQGraph(w) <Enter> \
        {
         .quickmessage configure -text "scattering intensity I(Q)"
         .xycoordinates configure -text ""
        }
.obW.tab bind IQGraph <Enter> \
        {
         .quickmessage configure -text "scattering intensity I(Q)"
         .xycoordinates configure -text ""
        }

bind $IQGraph(w) <Motion> \
        {
         set xcoord [expr [winfo pointerx $IQGraph(w)] \
                         -[winfo rootx    $IQGraph(w)] ] 
         set ycoord [expr [winfo pointery $IQGraph(w)] \
                         -[winfo rooty    $IQGraph(w)] ]

         set IQGraph(c,x) $xcoord
         set IQGraph(c,y) $ycoord
         
         $IQGraph(w) crosshairs configure -position @$IQGraph(c,x),$IQGraph(c,y)

         if { $IQGraph(e,element) > 0 } {
            set xycoord [$IQGraph(w) invtransform $xcoord $ycoord]
            .xycoordinates configure \
                     -text "([fp [lindex $xycoord 0]],[fp [lindex $xycoord 1]])"
         }
        }
bind $IQGraph(w) <Shift-ButtonPress-1> \
        {
         global IQGraph 
         set xcoord [expr [winfo pointerx $IQGraph(w)] \
                         -[winfo rootx $IQGraph(w)]]
         set ycoord [expr [winfo pointery $IQGraph(w)] \
                         -[winfo rooty $IQGraph(w)]]
         if { $IQGraph(e,element) > 0 } {
            set xycoord [$IQGraph(w) invtransform $xcoord $ycoord]
            set IQGraph(lower,x) [lindex $xycoord 0]
            set IQGraph(upper,x) [lindex $xycoord 0]
            set ulx [invxaxistrans $IQGraph(x,type) $IQGraph(upper,x)]
            set lrx [invxaxistrans $IQGraph(x,type) $IQGraph(lower,x)]
            set IQGraph(lower,x) $lrx
            set IQGraph(upper,x) $ulx
            Make_Square_el IQGraph $lrx $ulx
         }
         bind $IQGraph(w) <Shift-Motion> \
              {
               global IQGraph
               set xcoord [expr [winfo pointerx $IQGraph(w)] \
                               -[winfo rootx $IQGraph(w)]]
               set ycoord [expr [winfo pointery $IQGraph(w)] \
                               -[winfo rooty $IQGraph(w)]]
               if { $IQGraph(e,element) > 0 } {
                  set xycoord [$IQGraph(w) invtransform $xcoord $ycoord]
                  set lrx $IQGraph(lower,x)
                  Del_Square_el IQGraph
                  set IQGraph(upper,x) [lindex $xycoord 0]
                  set ulx [invxaxistrans $IQGraph(x,type) $IQGraph(upper,x)]
                  set IQGraph(upper,x) $ulx
                  set IQGraph(lower,x) $lrx
                  Make_Square_el IQGraph $IQGraph(lower,x) $ulx
                  .xycoordinates configure \
                     -text "([fp [lindex $xycoord 0]],[fp [lindex $xycoord 1]])"
               }
              }
         bind $IQGraph(w) <ButtonRelease-1> \
              { 
               global IQGraph
               set xcoord [expr [winfo pointerx $IQGraph(w)] \
                               -[winfo rootx $IQGraph(w)]]
               set ycoord [expr [winfo pointery $IQGraph(w)] \
                               -[winfo rooty $IQGraph(w)]]
               if { $IQGraph(e,element) > 0 } {
                  set xycoord [$IQGraph(w) invtransform $xcoord $ycoord]
                  set lrx $IQGraph(lower,x)
                  Del_Square_el IQGraph
                  set IQGraph(upper,x) [lindex $xycoord 0]
                  set ulx [invxaxistrans $IQGraph(x,type) $IQGraph(upper,x)]
                  set IQGraph(upper,x) $ulx
                  set IQGraph(lower,x) $lrx
                  Make_Square_el IQGraph $IQGraph(lower,x) $ulx
                  .xycoordinates configure \
                     -text "([fp [lindex $xycoord 0]],[fp [lindex $xycoord 1]])"
               }
               bind $IQGraph(w) <ButtonRelease-1> ""
               bind $IQGraph(w) <Shift-Motion> ""
               if {$IQGraph(lower,x) > $IQGraph(upper,x)} {
                  set sasfit(upper,Q) $IQGraph(lower,x)
                  set sasfit(lower,Q) $IQGraph(upper,x)
               } else {
                  set sasfit(upper,Q) $IQGraph(upper,x)
                  set sasfit(lower,Q) $IQGraph(lower,x)
               }
               if {$IQGraph(lower,x) == $IQGraph(upper,x)} {
                  set sasfit(upper,Q) ""
                  set sasfit(lower,Q) ""
               }
              }
        }


bind $GlobalFitIQGraph(w) <Enter> \
        {
         .quickmessage configure -text "scattering intensity I(Q)"
         .xycoordinates configure -text ""
        }
.obW.tab bind GlobalFitIQGraph <Enter> \
        {
         .quickmessage configure -text "scattering intensity I(Q)"
         .xycoordinates configure -text ""
        }

bind $GlobalFitIQGraph(w) <Motion> \
        {
         set xcoord [expr [winfo pointerx $GlobalFitIQGraph(w)] \
                         -[winfo rootx    $GlobalFitIQGraph(w)] ] 
         set ycoord [expr [winfo pointery $GlobalFitIQGraph(w)] \
                         -[winfo rooty    $GlobalFitIQGraph(w)] ]

         set GlobalFitIQGraph(c,x) $xcoord
         set GlobalFitIQGraph(c,y) $ycoord
         
         $GlobalFitIQGraph(w) crosshairs configure -position @$GlobalFitIQGraph(c,x),$GlobalFitIQGraph(c,y)

         if { $GlobalFitIQGraph(e,element) > 0 } {
            set xycoord [$GlobalFitIQGraph(w) invtransform $xcoord $ycoord]
            .xycoordinates configure \
                     -text "([fp [lindex $xycoord 0]],[fp [lindex $xycoord 1]])"
         }
        }
bind $GlobalFitIQGraph(w) <Shift-ButtonPress-1> \
        {
         global GlobalFitIQGraph 
         set xcoord [expr [winfo pointerx $GlobalFitIQGraph(w)] \
                         -[winfo rootx $GlobalFitIQGraph(w)]]
         set ycoord [expr [winfo pointery $GlobalFitIQGraph(w)] \
                         -[winfo rooty $GlobalFitIQGraph(w)]]
         if { $GlobalFitIQGraph(e,element) > 0 } {
            set xycoord [$GlobalFitIQGraph(w) invtransform $xcoord $ycoord]
            set GlobalFitIQGraph(lower,x) [lindex $xycoord 0]
            set GlobalFitIQGraph(upper,x) [lindex $xycoord 0]
            set ulx [invxaxistrans $GlobalFitIQGraph(x,type) $GlobalFitIQGraph(upper,x)]
            set lrx [invxaxistrans $GlobalFitIQGraph(x,type) $GlobalFitIQGraph(lower,x)]
            set GlobalFitIQGraph(lower,x) $lrx
            set GlobalFitIQGraph(upper,x) $ulx
            Make_Square_el GlobalFitIQGraph $lrx $ulx
         }
         bind $GlobalFitIQGraph(w) <Shift-Motion> \
              {
               global GlobalFitIQGraph
               set xcoord [expr [winfo pointerx $GlobalFitIQGraph(w)] \
                               -[winfo rootx $GlobalFitIQGraph(w)]]
               set ycoord [expr [winfo pointery $GlobalFitIQGraph(w)] \
                               -[winfo rooty $GlobalFitIQGraph(w)]]
               if { $GlobalFitIQGraph(e,element) > 0 } {
                  set xycoord [$GlobalFitIQGraph(w) invtransform $xcoord $ycoord]
                  set lrx $GlobalFitIQGraph(lower,x)
                  Del_Square_el GlobalFitIQGraph
                  set GlobalFitIQGraph(upper,x) [lindex $xycoord 0]
                  set ulx [invxaxistrans $GlobalFitIQGraph(x,type) $GlobalFitIQGraph(upper,x)]
                  set GlobalFitIQGraph(upper,x) $ulx
                  set GlobalFitIQGraph(lower,x) $lrx
                  Make_Square_el GlobalFitIQGraph $GlobalFitIQGraph(lower,x) $ulx
                  .xycoordinates configure \
                     -text "([fp [lindex $xycoord 0]],[fp [lindex $xycoord 1]])"
               }
              }
         bind $GlobalFitIQGraph(w) <ButtonRelease-1> \
              { 
               global GlobalFitIQGraph
               global addsasfit
               set xcoord [expr [winfo pointerx $GlobalFitIQGraph(w)] \
                               -[winfo rootx $GlobalFitIQGraph(w)]]
               set ycoord [expr [winfo pointery $GlobalFitIQGraph(w)] \
                               -[winfo rooty $GlobalFitIQGraph(w)]]
               if { $GlobalFitIQGraph(e,element) > 0 } {
                  set xycoord [$GlobalFitIQGraph(w) invtransform $xcoord $ycoord]
                  set lrx $GlobalFitIQGraph(lower,x)
                  Del_Square_el GlobalFitIQGraph
                  set GlobalFitIQGraph(upper,x) [lindex $xycoord 0]
                  set ulx [invxaxistrans $GlobalFitIQGraph(x,type) $GlobalFitIQGraph(upper,x)]
                  set GlobalFitIQGraph(upper,x) $ulx
                  set GlobalFitIQGraph(lower,x) $lrx
                  Make_Square_el GlobalFitIQGraph $GlobalFitIQGraph(lower,x) $ulx
                  .xycoordinates configure \
                     -text "([fp [lindex $xycoord 0]],[fp [lindex $xycoord 1]])"
               }
               bind $GlobalFitIQGraph(w) <ButtonRelease-1> ""
               bind $GlobalFitIQGraph(w) <Shift-Motion> ""
               if {$GlobalFitIQGraph(lower,x) > $GlobalFitIQGraph(upper,x)} {
                  set addsasfit(upper,Q) $GlobalFitIQGraph(lower,x)
                  set addsasfit(lower,Q) $GlobalFitIQGraph(upper,x)
               } else {
                  set addsasfit(upper,Q) $GlobalFitIQGraph(upper,x)
                  set addsasfit(lower,Q) $GlobalFitIQGraph(lower,x)
               }
               if {$GlobalFitIQGraph(lower,x) == $GlobalFitIQGraph(upper,x)} {
                  set addsasfit(upper,Q) ""
                  set addsasfit(lower,Q) ""
               }
              }
        }


.obW.tab bind ResIQGraph <Enter> \
        {
         .quickmessage configure -text \
                "residual intensity (I_exp(Q) - I_th(Q))/Delta I_exp(Q)"
         .xycoordinates configure -text ""
        }
bind .obW.tab.residual <Enter> \
        {
         .quickmessage configure -text \
                "residual intensity (I_exp(Q) - I_th(Q))/Delta I_exp(Q)"
         .xycoordinates configure -text ""
        }
bind $ResIQGraph(w) <Motion> \
        {
         set xcoord [expr [winfo pointerx $ResIQGraph(w)] \
                         -[winfo rootx $ResIQGraph(w)]]
         set ycoord [expr [winfo pointery $ResIQGraph(w)] \
                         -[winfo rooty $ResIQGraph(w)]]
         set ResIQGraph(c,x) $xcoord
         set ResIQGraph(c,y) $ycoord
         
         $ResIQGraph(w) crosshairs configure -position @$ResIQGraph(c,x),$ResIQGraph(c,y)
         if { $ResIQGraph(e,element) > 0 } {
            set xycoord [$ResIQGraph(w) invtransform $xcoord $ycoord]
            .xycoordinates configure \
                     -text "([fp [lindex $xycoord 0]],[fp [lindex $xycoord 1]])"
         }
        }

.obW.tab bind SDGraph <Enter> \
        {.quickmessage configure -text \
                "size distribution N(D), V N(D) or V^2 N(D)"
         .xycoordinates configure -text ""
        }
bind $SDGraph(w) <Enter> \
        {.quickmessage configure -text \
                "size distribution N(D), V N(D) or V^2 N(D)"}
bind $SDGraph(w) <Motion> \
        {
         set xcoord [expr [winfo pointerx $SDGraph(w)] \
                         -[winfo rootx $SDGraph(w)]]
         set ycoord [expr [winfo pointery $SDGraph(w)] \
                         -[winfo rooty $SDGraph(w)]]
         if { $SDGraph(e,element) > 0 } {
            set xycoord [$SDGraph(w) invtransform $xcoord $ycoord]
            .xycoordinates configure \
                     -text "([fp [lindex $xycoord 0]],[fp [lindex $xycoord 1]])"
         }
        }

.obW.tab bind StructParData <Enter> \
        {
         .quickmessage configure -text \
               "overview of different integral structural parameters"
         .xycoordinates configure -text ""
        }
bind .obW.tab.structpar <Enter> \
        {
         .quickmessage configure -text \
               "overview of different integral structural parameters"
         .xycoordinates configure -text ""
        }

   wm title . "UNKNOWN, $sasfit(progname) $sasfit(version)"
}

proc create_tab_text { widgetname n tabtext analytpar args} {
	upvar $analytpar ap
	set ap(${n}_w) .obW.tab.$widgetname
	frame $ap(${n}_w) -relief sunken -borderwidth 1 -background white
	.obW.tab insert 4 $widgetname
	.obW.tab tab configure $widgetname -text [join [split "$tabtext" {_}] "\n"] \
		-fill both -padx 0.1i -pady 0.1i \
		-window $ap(${n}_w) \
		-windowheight $::sasfit(height) -windowwidth $::sasfit(width) 
	text $ap(${n}_w).textbox -font tkconfixed -borderwidth 0 -background white -wrap none \
		-yscrollcommand "$ap(${n}_w).scrolly set" \
		-xscrollcommand "$ap(${n}_w).scrollx set"
	scrollbar $ap(${n}_w).scrolly -orient vertical \
		-command "$ap(${n}_w).textbox yview"
	scrollbar $ap(${n}_w).scrollx -orient horizontal \
		-command "$ap(${n}_w).textbox xview"
	pack $ap(${n}_w).textbox -expand true -fill both
	pack $ap(${n}_w).scrolly -side right -fill both -before $ap(${n}_w).textbox
	pack $ap(${n}_w).scrollx -side bottom -fill both -before $ap(${n}_w).textbox

## creating popup menu
	set ap(${n}_descr)  "$tabtext"
	if {[llength $args]} {
		upvar [lindex $args 0] gap
		set gap(${n}_w)     $ap(${n}_w)
		set gap(${n}_descr) $ap(${n}_descr)
	}
	menu $ap(${n}_w).popup -tearoff 0
	$ap(${n}_w).popup add command -label "copy all (csv)" \
		-command "tab_text_copy2clipboard_cmd $analytpar $n"
	$ap(${n}_w).popup add command -label "save to file (csv)" \
		-command "tab_text_copy2file_cmd $analytpar $n"
	bind $ap(${n}_w).textbox <ButtonPress-3> "tk_popup $ap(${n}_w).popup %X %Y"
	bind $ap(${n}_w).textbox <Double-ButtonPress-1> "tk_popup $ap(${n}_w).popup %X %Y"
}
proc tab_text_copy2clipboard_cmd { analytpar n } {
	upvar $analytpar ap
	clipboard clear
	clipboard append $ap(${n}_text_export)
}
proc tab_text_copy2file_cmd { analytpar n } {
	upvar $analytpar ap
	set fname $n
	set dname $::sasfit(datadir)
	if {[info exists ::sasfit(lastProjectFile)]} {
		set fname [file rootname [file tail $::sasfit(lastProjectFile)]]
		set fname "${fname}_$n"
		set dname [file dirname $::sasfit(lastProjectFile)]
	}
	set fname [tk_getSaveFile -initialfile "${fname}.csv" -initialdir "$dname" \
		-parent $ap(${n}_w) \
		-title "Select a file to store the exported data."]
	if {![llength $fname]} { return }
	puts "Writing [join [split \"$ap(${n}_descr)\" {_}] " "] to: '$fname'"
	set fh [open "$fname" w+]
	puts $fh $ap(${n}_text_export)
	close $fh
}
proc tab_text_update { analytpar n } {
	upvar $analytpar ap
	$ap(${n}_w).textbox configure -state normal
	$ap(${n}_w).textbox delete 1.0 end
	$ap(${n}_w).textbox insert end \
		$ap(${n}_text_display)
	$ap(${n}_w).textbox configure -state disabled
}

proc get_config_from_file {} {
	proc cleanup { fh } {
		::ini::close $fh
		return -code return
	}
	set fn $::sasfit(cfgfile)
	if {![file exists $fn]} { return }
	set cfg [::ini::open $fn r]
	# we have only one section atm
	set section [lindex [::ini::sections $cfg] 0]
	# global section is mandatory
	if {![string equal $section "global"]} { cleanup $cfg }
	foreach key $::sasfit(no_update_entries) {
		set val {}
		if {[catch {set val [::ini::value $cfg "global" $key]}]} {
			continue
		}
		puts "setting ::sasfit($key) = '$val'"
		set ::sasfit($key) $val
	}
	cleanup $cfg
}

#------------------------------------------------------------------------------
#                               M A I N                    
#tk_bisque
catch {get_config_from_file}
sasfitmenu

if {![info exists ::sasfit(hide_about_win)] || !$::sasfit(hide_about_win)
} {
	AboutCmd 1
	raise .about
	focus .about
} else {
	raise .top
	focus .top
}


proc exit_cmd {} {
	exit
}
