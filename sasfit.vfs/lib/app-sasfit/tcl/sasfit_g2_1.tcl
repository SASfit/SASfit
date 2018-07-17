# sasfit.vfs/lib/app-sasfit/tcl/sasfit_g2_1.tcl
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

proc g2_1toR_PDI {} {
global G2ParData
if {[string compare $G2ParData(DLSmodel) "cumulant analysis (traditional)"] == 0 || [string compare $G2ParData(DLSmodel) "cumulant analysis (recommended)"] == 0} {
   set  k 1.3806505e-23
   set PI [expr atan(1)*4.0]
   set Q [expr 4e9*$PI*$G2ParData(refind)/$G2ParData(lambda) \
               * sin($G2ParData(Theta)*$PI/360.0)]
   set G2ParData(R_hyd) [expr 1e9*$k*$G2ParData(T)*$Q*$Q/$G2ParData(Par2)/(6.0e-3*$PI*$G2ParData(visc))]
   set G2ParData(PDI) [expr 1.0*$G2ParData(Par3)/($G2ParData(Par2)*$G2ParData(Par2))]
} else {
  set G2ParData(R_hyd) -1.0
  set G2ParData(PDI) -1.0
}
}

#------------------------------------------------------------------------------
#  
#
proc g2_1_ParFitCmd {} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^
global G2ParData 
global IQGraph
global sasfit

if {[winfo exists .g2_1]} {destroy .g2_1}
if {!$sasfit(I_enable)} {
    tk_messageBox -icon error -title ERROR \
            -message "No data loaded"
    return
} 
#else { RefreshG2_1ParFit {} } 
toplevel .g2_1
set w .g2_1
wm geometry $w
wm title $w "DLS fit"
raise $w
focus $w

#
# definition of the pull down menu
#
frame $w.menu -relief raised -bd 2
frame $w.input
frame $w.action
pack $w.menu $w.input $w.action -side top -fill x

menubutton $w.menu.file -text File -underline 0 \
           -menu $w.menu.file.menu
   menu $w.menu.file.menu
   $w.menu.file.menu add command -label "Quit" \
                     -command { 
                         if {[winfo exists .g2_1]} {destroy .g2_1}
                      }
menubutton $w.menu.options -text Options -underline 0 \
          -menu $w.menu.options.menu
   menu $w.menu.options.menu
   $w.menu.options.menu add cascade -label "Plot Axis" \
           -menu $w.menu.options.menu.plotaxis
   set w2 [menu $w.menu.options.menu.plotaxis -tearoff 0]
   $w2 add radio -label "LogLin" \
       -variable G2ParData(plottype) -value 0 \
       -command {
          global IQGraph
          set IQGraph(x,type) "log10(x)"
          set IQGraph(y,type) "y"
          RefreshGraph IQGraph
          raise .g2_1
          focus .g2_1
       }
   set IQGraph(x,type) "log10(x)"
   set IQGraph(y,type) "y"
   set IQGraph(tmp,x,title) $IQGraph(x,title)
   set IQGraph(tmp,y,title) $IQGraph(y,title)
   set IQGraph(x,title) "t / sec"
   set IQGraph(y,title) "g2(t)"
   RefreshGraph IQGraph
   set IQGraph(x,title) $IQGraph(tmp,x,title)
   set IQGraph(y,title) $IQGraph(tmp,y,title)
   $w2 add radio -label "LinLog" \
       -variable G2ParData(plottype) -value 1 \
       -comman {
          global IQGraph
          set IQGraph(x,type) "x"
          set IQGraph(y,type) "log10(y)"
          RefreshGraph IQGraph
          raise .g2_1
          focus .g2_1
       }
   $w2 add radio -label "LinLin" \
       -variable G2ParData(plottype) -value 2 \
       -command {
          global IQGraph
          set IQGraph(x,type) "x"
          set IQGraph(y,type) "y"
          RefreshGraph IQGraph
          raise .g2_1
          focus .g2_1
       }

sasfit_menubar_build_help $w.menu.help
pack $w.menu.file $w.menu.options -side left
pack $w.menu.help -side right

#
# input fields for parameters of DLS signal
#

set DLSpar [tk_optionMenu $w.input.model G2ParData(DLSmodel) \
                          "cumulant analysis (recommended)" \
                          "cumulant analysis (traditional)" \
                          "double decay cumulant analysis" \
                          "double stretched exponential"]
						  
$DLSpar entryconfigure 0 -command { 
        global G2ParData
        set G2ParData(DLSmodel) "cumulant analysis (recommended)"
        set i 1
        foreach ParNam [lindex $G2ParData(Par) 0] {
           set G2ParData([format %s%d%s Par $i lab]) $ParNam
	   if {[string length $ParNam] == 0} {
              set G2ParData([format %s%d%s Par $i active]) 0
           }
           incr i
	} 
        }
		
$DLSpar entryconfigure 1 -command { 
        global G2ParData
        set G2ParData(DLSmodel) "cumulant analysis (traditional)"
        set i 1
        foreach ParNam [lindex $G2ParData(Par) 1] {
           set G2ParData([format %s%d%s Par $i lab]) $ParNam
	   if {[string length $ParNam] == 0} {
              set G2ParData([format %s%d%s Par $i active]) 0
           }
           incr i
	} 
        }
		
$DLSpar entryconfigure 2 -command { 
        global G2ParData 
        set G2ParData(DLSmodel) "double decay cumulant analysis"
        set i 1
        foreach ParNam [lindex $G2ParData(Par) 2] {
           set G2ParData([format %s%d%s Par $i lab]) $ParNam
	   if {[string length $ParNam] == 0} {
              set G2ParData([format %s%d%s Par $i active]) 0
           }
           incr i
	} 
        }
		
$DLSpar entryconfigure 3 -command { 
        global G2ParData 
        set G2ParData(DLSmodel) "double stretched exponential"
        set i 1
        foreach ParNam [lindex $G2ParData(Par) 3] {
           set G2ParData([format %s%d%s Par $i lab]) $ParNam
	   if {[string length $ParNam] == 0} {
              set G2ParData([format %s%d%s Par $i active]) 0
           }
           incr i
	} 
        }


#set DLSerr [tk_optionMenu $w.input.error G2ParData(DLSerror) \
#                          "lin. error"  \
#                          "log. error"  ]
#$DLSerr entryconfigure 0 -command { 
#        global G2ParData
#        set G2ParData(DLSerror) "lin. error" 
#        }
#$DLSerr entryconfigure 1 -command { 
#        global G2ParData 
#        set G2ParData(DLSerror) "log. error"
#        }

label $w.input.iter_l -text "max iter. (>=0):"
entry $w.input.iter_v -textvariable G2ParData(iter) -width 4
label $w.input.iter_d -textvariable G2ParData(iter_d) -width 24 -anchor w

grid $w.input.model  -column 0 -columnspan 3 -row 0 -pady 2m -sticky w
grid $w.input.iter_l  -column 3 -columnspan 3 -row 0 -pady 2m -sticky w
grid $w.input.iter_v  -column 6 -columnspan 1 -row 0 -pady 2m -sticky w
grid $w.input.iter_d  -column 7 -columnspan 3 -row 0 -pady 2m -sticky w


label $w.input.par1lab -textvariable G2ParData(Par1lab)
label $w.input.par2lab -textvariable G2ParData(Par2lab)
label $w.input.par3lab -textvariable G2ParData(Par3lab)
label $w.input.par4lab -textvariable G2ParData(Par4lab)
label $w.input.par5lab -textvariable G2ParData(Par5lab)
label $w.input.par6lab -textvariable G2ParData(Par6lab)
label $w.input.par7lab -textvariable G2ParData(Par7lab)

label $w.input.par1err -width 16 -textvariable G2ParData(Par1err) -anchor w
label $w.input.par2err -width 16 -textvariable G2ParData(Par2err) -anchor w
label $w.input.par3err -width 16 -textvariable G2ParData(Par3err) -anchor w
label $w.input.par4err -width 16 -textvariable G2ParData(Par4err) -anchor w
label $w.input.par5err -width 16 -textvariable G2ParData(Par5err) -anchor w
label $w.input.par6err -width 16 -textvariable G2ParData(Par6err) -anchor w
label $w.input.par7err -width 16 -textvariable G2ParData(Par7err) -anchor w

label $w.input.eq_lab1 -text "="
label $w.input.eq_lab2 -text "="
label $w.input.eq_lab3 -text "="
label $w.input.eq_lab4 -text "="
label $w.input.eq_lab5 -text "="
label $w.input.eq_lab6 -text "="
label $w.input.eq_lab7 -text "="

label $w.input.pm_lab1 -text ±
label $w.input.pm_lab2 -text ±
label $w.input.pm_lab3 -text ±
label $w.input.pm_lab4 -text ±
label $w.input.pm_lab5 -text ±
label $w.input.pm_lab6 -text ±
label $w.input.pm_lab7 -text ±

grid  $w.input.par1lab -column 0 -row 2 -sticky e
grid  $w.input.par2lab -column 0 -row 3 -sticky e
grid  $w.input.par3lab -column 0 -row 4 -sticky e
grid  $w.input.par4lab -column 0 -row 5 -sticky e
grid  $w.input.par5lab -column 0 -row 6 -sticky e
grid  $w.input.par6lab -column 0 -row 7 -sticky e
grid  $w.input.par7lab -column 0 -row 8 -sticky e

entry $w.input.par1entry -width 12 -textvariable G2ParData(Par1)
entry $w.input.par2entry -width 12 -textvariable G2ParData(Par2)
entry $w.input.par3entry -width 12 -textvariable G2ParData(Par3)
entry $w.input.par4entry -width 12 -textvariable G2ParData(Par4)
entry $w.input.par5entry -width 12 -textvariable G2ParData(Par5)
entry $w.input.par6entry -width 12 -textvariable G2ParData(Par6)
entry $w.input.par7entry -width 12 -textvariable G2ParData(Par7)

grid  $w.input.eq_lab1 -column 1 -row 2 -sticky w
grid  $w.input.eq_lab2 -column 1 -row 3 -sticky w
grid  $w.input.eq_lab3 -column 1 -row 4 -sticky w
grid  $w.input.eq_lab4 -column 1 -row 5 -sticky w
grid  $w.input.eq_lab5 -column 1 -row 6 -sticky w
grid  $w.input.eq_lab6 -column 1 -row 7 -sticky w
grid  $w.input.eq_lab7 -column 1 -row 8 -sticky w

grid  $w.input.par1entry -column 2 -row 2 -sticky w
grid  $w.input.par2entry -column 2 -row 3 -sticky w
grid  $w.input.par3entry -column 2 -row 4 -sticky w
grid  $w.input.par4entry -column 2 -row 5 -sticky w
grid  $w.input.par5entry -column 2 -row 6 -sticky w
grid  $w.input.par6entry -column 2 -row 7 -sticky w
grid  $w.input.par7entry -column 2 -row 8 -sticky w

grid  $w.input.pm_lab1 -column 6 -row 2 -sticky w
grid  $w.input.pm_lab2 -column 6 -row 3 -sticky w
grid  $w.input.pm_lab3 -column 6 -row 4 -sticky w
grid  $w.input.pm_lab4 -column 6 -row 5 -sticky w
grid  $w.input.pm_lab5 -column 6 -row 6 -sticky w
grid  $w.input.pm_lab6 -column 6 -row 7 -sticky w
grid  $w.input.pm_lab7 -column 6 -row 8 -sticky w

grid  $w.input.par1err -column 7 -row 2 -sticky w
grid  $w.input.par2err -column 7 -row 3 -sticky w
grid  $w.input.par3err -column 7 -row 4 -sticky w
grid  $w.input.par4err -column 7 -row 5 -sticky w
grid  $w.input.par5err -column 7 -row 6 -sticky w
grid  $w.input.par6err -column 7 -row 7 -sticky w
grid  $w.input.par7err -column 7 -row 8 -sticky w

ArrowButton $w.input.par1up \
       -dir top -repeatdelay 300 -repeatinterval 100 \
       -highlightthickness 0 -width 22 -height 22 \
       -ipadx 2 -ipady 2 -clean 1 \
       -armcommand {set G2ParData(Par1) \
                       [expr $G2ParData(Par1)*$G2ParData(par_x_X)]
                    set G2ParData(Par1err) 0}
ArrowButton $w.input.par1down \
       -dir bottom -repeatdelay 300 -repeatinterval 100 \
       -highlightthickness 0 -width 22 -height 22 \
       -ipadx 2 -ipady 2 -clean 1 \
       -armcommand {set G2ParData(Par1) \
                       [expr $G2ParData(Par1)/$G2ParData(par_x_X)]
                    set G2ParData(Par1err) 0}

ArrowButton $w.input.par2up \
       -dir top -repeatdelay 300 -repeatinterval 100 \
       -highlightthickness 0 -width 22 -height 22 \
       -ipadx 2 -ipady 2 -clean 1 \
       -armcommand {set G2ParData(Par2) \
                       [expr $G2ParData(Par2)*$G2ParData(par_x_X)]
                    set G2ParData(Par2err) 0}
ArrowButton $w.input.par2down \
       -dir bottom -repeatdelay 300 -repeatinterval 100 \
       -highlightthickness 0 -width 22 -height 22 \
       -ipadx 2 -ipady 2 -clean 1 \
       -armcommand {set G2ParData(Par2) \
                       [expr $G2ParData(Par2)/$G2ParData(par_x_X)]
                    set G2ParData(Par2err) 0}

ArrowButton $w.input.par3up \
       -dir top -repeatdelay 300 -repeatinterval 100 \
       -highlightthickness 0 -width 22 -height 22 \
       -ipadx 2 -ipady 2 -clean 1 \
       -armcommand {set G2ParData(Par3) \
                       [expr $G2ParData(Par3)*$G2ParData(par_x_X)]
                    set G2ParData(Par3err) 0}
ArrowButton $w.input.par3down \
       -dir bottom -repeatdelay 300 -repeatinterval 100 \
       -highlightthickness 0 -width 22 -height 22 \
       -ipadx 2 -ipady 2 -clean 1 \
       -armcommand {set G2ParData(Par3) \
                       [expr $G2ParData(Par3)/$G2ParData(par_x_X)]
                    set G2ParData(Par3err) 0}

ArrowButton $w.input.par4up \
       -dir top -repeatdelay 300 -repeatinterval 100 \
       -highlightthickness 0 -width 22 -height 22 \
       -ipadx 2 -ipady 2 -clean 1 \
       -armcommand {set G2ParData(Par4) \
                       [expr $G2ParData(Par4)*$G2ParData(par_x_X)]
                    set G2ParData(Par4err) 0}
ArrowButton $w.input.par4down \
       -dir bottom -repeatdelay 300 -repeatinterval 100 \
       -highlightthickness 0 -width 22 -height 22 \
       -ipadx 2 -ipady 2 -clean 1 \
       -armcommand {set G2ParData(Par4) \
                       [expr $G2ParData(Par4)/$G2ParData(par_x_X)]
                    set G2ParData(Par4err) 0}

ArrowButton $w.input.par5up \
       -dir top -repeatdelay 300 -repeatinterval 100 \
       -highlightthickness 0 -width 22 -height 22 \
       -ipadx 2 -ipady 2 -clean 1 \
       -armcommand {set G2ParData(Par5) \
                       [expr $G2ParData(Par5)*$G2ParData(par_x_X)]
                    set G2ParData(Par5err) 0}
ArrowButton $w.input.par5down \
       -dir bottom -repeatdelay 300 -repeatinterval 100 \
       -highlightthickness 0 -width 22 -height 22 \
       -ipadx 2 -ipady 2 -clean 1 \
       -armcommand {set G2ParData(Par5) \
                       [expr $G2ParData(Par5)/$G2ParData(par_x_X)]
                    set G2ParData(Par5err) 0}

ArrowButton $w.input.par6up \
       -dir top -repeatdelay 300 -repeatinterval 100 \
       -highlightthickness 0 -width 22 -height 22 \
       -ipadx 2 -ipady 2 -clean 1 \
       -armcommand {set G2ParData(Par6) \
                       [expr $G2ParData(Par6)*$G2ParData(par_x_X)]
                    set G2ParData(Par6err) 0}
ArrowButton $w.input.par6down \
       -dir bottom -repeatdelay 300 -repeatinterval 100 \
       -highlightthickness 0 -width 22 -height 22 \
       -ipadx 2 -ipady 2 -clean 1 \
       -armcommand {set G2ParData(Par6) \
                       [expr $G2ParData(Par6)/$G2ParData(par_x_X)]
                    set G2ParData(Par6err) 0}

ArrowButton $w.input.par7up \
       -dir top -repeatdelay 300 -repeatinterval 100 \
       -highlightthickness 0 -width 22 -height 22 \
       -ipadx 2 -ipady 2 -clean 1 \
       -armcommand {set G2ParData(Par7) \
                       [expr $G2ParData(Par7)*$G2ParData(par_x_X)]
                    set G2ParData(Par7err) 0}
ArrowButton $w.input.par7down \
       -dir bottom -repeatdelay 300 -repeatinterval 100 \
       -highlightthickness 0 -width 22 -height 22 \
       -ipadx 2 -ipady 2 -clean 1 \
       -armcommand {set G2ParData(Par7) \
                       [expr $G2ParData(Par7)/$G2ParData(par_x_X)]
                    set G2ParData(Par7err) 0}

grid $w.input.par1up   -column 3 -row 2
grid $w.input.par1down -column 4 -row 2
grid $w.input.par2up   -column 3 -row 3
grid $w.input.par2down -column 4 -row 3
grid $w.input.par3up   -column 3 -row 4
grid $w.input.par3down -column 4 -row 4
grid $w.input.par4up   -column 3 -row 5
grid $w.input.par4down -column 4 -row 5
grid $w.input.par5up   -column 3 -row 6
grid $w.input.par5down -column 4 -row 6
grid $w.input.par6up   -column 3 -row 7
grid $w.input.par6down -column 4 -row 7
grid $w.input.par7up   -column 3 -row 8
grid $w.input.par7down -column 4 -row 8

checkbutton $w.input.par1active  -variable G2ParData(Par1active)
checkbutton $w.input.par2active  -variable G2ParData(Par2active)
checkbutton $w.input.par3active  -variable G2ParData(Par3active)
checkbutton $w.input.par4active  -variable G2ParData(Par4active)
checkbutton $w.input.par5active  -variable G2ParData(Par5active)
checkbutton $w.input.par6active  -variable G2ParData(Par6active)
checkbutton $w.input.par7active  -variable G2ParData(Par7active)

grid $w.input.par1active -column 5 -row 2
grid $w.input.par2active -column 5 -row 3
grid $w.input.par3active -column 5 -row 4
grid $w.input.par4active -column 5 -row 5
grid $w.input.par5active -column 5 -row 6
grid $w.input.par6active -column 5 -row 7
grid $w.input.par7active -column 5 -row 8 


label $w.input.empty -text ""
label $w.input.lambda_lab -text "wavelength (nm):"  -anchor w
label $w.input.temp_lab   -text "temperature (K):"  -anchor w
label $w.input.eta_lab    -text "viscosity (cP):"   -anchor w
label $w.input.refin_lab  -text "refraction index:" -anchor w
label $w.input.theta_lab  -text "Theta (degree):"   -anchor w
label $w.input.r_hyd_lab  -text "R_hyd:"            -anchor w
label $w.input.pdi_lab    -text "PDI:"              -anchor w

entry $w.input.lambda_val -width 12 -textvariable G2ParData(lambda)
entry $w.input.temp_val   -width 12 -textvariable G2ParData(T)
entry $w.input.eta_val    -width 12 -textvariable G2ParData(visc)
entry $w.input.refin_val  -width 12 -textvariable G2ParData(refind)
entry $w.input.theta_val  -width 12 -textvariable G2ParData(Theta)
entry $w.input.r_hyd_val  -width 16 -textvariable G2ParData(R_hyd)
entry $w.input.pdi_val    -width 16 -textvariable G2ParData(PDI)

grid $w.input.r_hyd_lab  -column 4 -columnspan 2 -row 10 -sticky e
grid $w.input.r_hyd_val  -column 6 -columnspan 2 -row 10 -sticky w
grid $w.input.pdi_lab    -column 4 -columnspan 2 -row 12 -sticky e
grid $w.input.pdi_val    -column 6 -columnspan 2 -row 12 -sticky w

grid $w.input.empty      -column 0 -row 9
grid $w.input.lambda_lab -column 0 -row 10  -sticky e
grid $w.input.temp_lab   -column 0 -row 11 -sticky e
grid $w.input.eta_lab    -column 0 -row 12 -sticky e
grid $w.input.refin_lab  -column 0 -row 13 -sticky e
grid $w.input.theta_lab  -column 0 -row 14 -sticky e

grid $w.input.lambda_val -column 2 -row 10 -sticky w
grid $w.input.temp_val   -column 2 -row 11
grid $w.input.eta_val    -column 2 -row 12 -sticky w
grid $w.input.refin_val  -column 2 -row 13 -sticky w
grid $w.input.theta_val  -column 2 -row 14 -sticky w

button $w.action.apply -text Apply -highlightthickness 0  \
   -padx 1m -pady 1m -width 8\
   -command { global G2ParData
              global sasfit
              if {([string length $sasfit(lower,Q)] > 0) && \
                  ([string length $sasfit(upper,Q)] > 0) } {
	         set Q {}
                 set I {}
                 set DI {}
                 set res {}
                 foreach x $sasfit(Q) y $sasfit(I) e $sasfit(DI) r $sasfit(res) {
		    if {($x>=$sasfit(lower,Q)) && ($x<=$sasfit(upper,Q))} {
                       lappend Q   $x
                       lappend I   $y
                       lappend DI  $e
                       lappend res $r
                    }
                 } 
	      } else  {
                 set Q  $sasfit(Q)
                 set I  $sasfit(I)
                 set DI $sasfit(DI)
                 set res $sasfit(res)
              }
              set DLSth [dls_CumulantCalc G2ParData [list $Q $I]]
              g2_1toR_PDI
              if {([string length $sasfit(lower,Q)] > 0) && \
                  ([string length $sasfit(upper,Q)] > 0) } {
                 set DI  {}
                 set nx  0
                 foreach x $sasfit(Q) {
		    if {($x>=$sasfit(lower,Q)) && ($x<=$sasfit(upper,Q))} {
                       lappend DI  [lindex [lindex $DLSth 1] $nx]
                       incr nx 
                    } else {
                       lappend DI  1e-5		    }
		} 
	      } else { 
                 set DI [lindex $DLSth 1]
              }
              clearGraph_el IQGraph
              Put_Graph_el IQGraph $sasfit(Q) $sasfit(I)
              Put_Graph_el IQGraph $Q [lindex $DLSth 0]
              set IQGraph(e,symbol) \
                         [lreplace $IQGraph(e,symbol)    1 1 none]
              set IQGraph(e,linehide) [lreplace $IQGraph(e,linehide) 1 1 1]
              set IQGraph(e,dashcolor) [lreplace $IQGraph(e,dashcolor) 1 1 red]
              set IQGraph(l,legendtext) [lreplace $IQGraph(l,legendtext) \
                                            0 0 \
                                            [file tail $sasfit(filename)]]
              set IQGraph(l,legendtext) [lreplace $IQGraph(l,legendtext) \
                                            1 1 Fit]
	      set IQGraph(tmp,x,title) $IQGraph(x,title)
	      set IQGraph(tmp,y,title) $IQGraph(y,title)
	      set IQGraph(x,title) "t / sec"
	      set IQGraph(y,title) "g2(t)"
	      RefreshGraph IQGraph
	      set IQGraph(x,title) $IQGraph(tmp,x,title)
	      set IQGraph(y,title) $IQGraph(tmp,y,title)
            }
button $w.action.fit -text Fit -highlightthickness 0  \
   -padx 1m -pady 1m -width 8\
   -command { global G2ParData
              global sasfit
              if {([string length $sasfit(lower,Q)] > 0) && \
                  ([string length $sasfit(upper,Q)] > 0) } {
	         set Q {}
                 set I {}
                 set DI {}
                 set res {}
                 foreach x $sasfit(Q) y $sasfit(I) e $sasfit(DI) r $sasfit(res) {
		    if {($x>=$sasfit(lower,Q)) && ($x<=$sasfit(upper,Q))} {
                       lappend Q   $x
                       lappend I   $y
                       lappend DI  $e
                       lappend res $r
                    }
                 } 
	      } else  {
                 set Q  $sasfit(Q)
                 set I  $sasfit(I)
                 set DI $sasfit(DI)
                 set res $sasfit(res)
              }
              set DLSth [dls_CumulantFit G2ParData [list $Q $I]]
#puts "> $DLSth <"
#puts  ">[lindex $DLSth 0]<"
#puts  ">[lindex $DLSth 1]<"
              g2_1toR_PDI
              if {([string length $sasfit(lower,Q)] > 0) && \
                  ([string length $sasfit(upper,Q)] > 0) } {
                 set DI  {}
                 set nx  0
                 foreach x $sasfit(Q) {
		    if {($x>=$sasfit(lower,Q)) && ($x<=$sasfit(upper,Q))} {
                       lappend DI  [lindex [lindex $DLSth 1] $nx]
                       incr nx 
                    } else {
                       lappend DI  1e-5		    }
		} 
	      } else { 
                 set DI [lindex $DLSth 1]
              }
              clearGraph_el IQGraph
              Put_Graph_el IQGraph $sasfit(Q) $sasfit(I) $DI
              Put_Graph_el IQGraph $Q [lindex $DLSth 0]
              set IQGraph(e,symbol) \
                         [lreplace $IQGraph(e,symbol)    1 1 none]
              set IQGraph(e,linehide) [lreplace $IQGraph(e,linehide) 1 1 1]
              set IQGraph(e,dashcolor) [lreplace $IQGraph(e,dashcolor) 1 1 red]
              set IQGraph(l,legendtext) [lreplace $IQGraph(l,legendtext) \
                                            0 0 \
                                            [file tail $sasfit(filename)]]
              set IQGraph(l,legendtext) [lreplace $IQGraph(l,legendtext) \
                                            1 1 Fit]
	      set IQGraph(tmp,x,title) $IQGraph(x,title)
	      set IQGraph(tmp,y,title) $IQGraph(y,title)
	      set IQGraph(x,title) "t / sec"
	      set IQGraph(y,title) "g2(t)"
	      RefreshGraph IQGraph
	      set IQGraph(x,title) $IQGraph(tmp,x,title)
	      set IQGraph(y,title) $IQGraph(tmp,y,title)
            }
label $w.action.chisqrlab -text "chi^2 =" \
      -anchor e -width 8
label $w.action.chisqrval -textvariable G2ParData(chisqr) \
      -width 16 -anchor w

pack $w.action.apply $w.action.fit \
     $w.action.chisqrlab $w.action.chisqrval \
     -side left
}
