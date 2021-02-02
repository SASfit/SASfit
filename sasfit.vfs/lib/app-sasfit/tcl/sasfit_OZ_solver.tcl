# sasfit.vfs/lib/app-sasfit/tcl/sasfit_OZ_solver.tcl
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

proc put_OZ_res {} {
	global OZ
	lappend OZ(result,closure) 	$OZ(closure)
	lappend OZ(result,potential) 	$OZ(potential)
	lappend OZ(result,mixstrategy) 	$OZ(mixstrategy)
	lappend OZ(result,p) 		[list $OZ(p0) $OZ(p1) $OZ(p2) $OZ(p3) $OZ(p4) $OZ(p5) $OZ(p6) $OZ(p7) $OZ(p8) $OZ(p9) $OZ(p10) $OZ(p11) $OZ(p12) $OZ(p13) $OZ(p14) $OZ(p15)]
	lappend OZ(result,algorithm)	$OZ(algorithm)
	lappend OZ(result,phi)		$OZ(phi)
	lappend OZ(result,T) 		$OZ(T)
	
	lappend OZ(result,KINSetMAA) $OZ(KINSetMAA)
	lappend OZ(result,KINSetFuncNormTol) $OZ(KINSetFuncNormTol)
	lappend OZ(result,KINSetScaledSteptol) $OZ(KINSetScaledSteptol)
	lappend OZ(result,KINSetNumMaxIters) $OZ(KINSetNumMaxIters)
	lappend OZ(result,KINSetPrintLevel) $OZ(KINSetPrintLevel)
	lappend OZ(result,KINSetEtaForm) $OZ(KINSetEtaForm)
	lappend OZ(result,KINSpilsSetMaxRestarts) $OZ(KINSpilsSetMaxRestarts)
	lappend OZ(result,KINSolStrategy) $OZ(KINSolStrategy)
	lappend OZ(result,KINSetMaxNewtonStep) $OZ(KINSetMaxNewtonStep)
	
    	lappend OZ(result,mindimOZ)	$OZ(mindimOZ)
    	lappend OZ(result,mult) 	$OZ(mult)
    	lappend OZ(result,mix) 		$OZ(mix)
    	lappend OZ(result,dr/dsigma) 	$OZ(dr/dsigma)
    	lappend OZ(result,releps)  	$OZ(releps)
    	lappend OZ(result,gridlength)  	$OZ(gridlength)
    	lappend OZ(result,q)  		$OZ(res,s,x)
    	lappend OZ(result,Sq)  		$OZ(res,s,y)
    	lappend OZ(result,r)  		$OZ(res,c,x)
    	lappend OZ(result,cr)  		$OZ(res,c,y)
    	lappend OZ(result,gr)  		$OZ(res,g,y)
    	lappend OZ(result,gr2)  		$OZ(res,g2,y)
    	lappend OZ(result,gr3)  		$OZ(res,g3,y)
    	lappend OZ(result,gr4)  		$OZ(res,g4,y)
    	lappend OZ(result,hr)  		$OZ(res,h,y)
    	lappend OZ(result,gammar) 	$OZ(res,gamma,y)
    	lappend OZ(result,u,x)          $OZ(res,u,x)
    	lappend OZ(result,Ur)  		$OZ(res,u,y)
    	lappend OZ(result,Br)  		$OZ(res,B,y)
    	lappend OZ(result,yr)  		$OZ(res,y,y)
    	lappend OZ(result,fr)  		$OZ(res,f,y)
    	lappend OZ(result,label)	"[llength $OZ(result,closure)]: $OZ(label)"
    	.oztop.interface.assigning.assign configure -values $OZ(result,label)
    	.oztop.interface.assigning.assign setvalue last
}

proc pop_OZ_res {} {
	global OZ ozSQGraph ozgrGraph ozcrGraph ozhrGraph ozgammarGraph ozbetaUrGraph ozBrGraph ozyrGraph ozfrGraph
	set OZ(result,closure) 		[lrange $OZ(result,closure) 	0 [expr [llength $OZ(result,closure)]	-2]]
	set OZ(result,potential) 	[lrange $OZ(result,potential) 	0 [expr [llength $OZ(result,potential)]	-2]]
	set OZ(result,p) 		[lrange $OZ(result,p) 		0 [expr [llength $OZ(result,p)]		-2]]
	set OZ(result,algorithm)		[lrange $OZ(result,algorithm) 	0 [expr [llength $OZ(result,algorithm)]	-2]]
	
	set OZ(result,KINSetMAA) [lrange $OZ(result,KINSetMAA) 		0 [expr [llength $OZ(result,KINSetMAA)]		-2]]
	set OZ(result,KINSetFuncNormTol) [lrange $OZ(result,KINSetFuncNormTol) 		0 [expr [llength $OZ(result,KINSetFuncNormTol)]		-2]]
	set OZ(result,KINSetScaledSteptol) [lrange $OZ(result,KINSetScaledSteptol) 		0 [expr [llength $OZ(result,KINSetScaledSteptol)]		-2]]
	set OZ(result,KINSetNumMaxIters) [lrange $OZ(result,KINSetNumMaxIters) 		0 [expr [llength $OZ(result,KINSetNumMaxIters)]		-2]]
	set OZ(result,KINSetPrintLevel) [lrange $OZ(result,KINSetPrintLevel) 		0 [expr [llength $OZ(result,KINSetPrintLevel)]		-2]]
	set OZ(result,KINSetEtaForm) [lrange $OZ(result,KINSetEtaForm) 		0 [expr [llength $OZ(result,KINSetEtaForm)]		-2]]
	set OZ(result,KINSolStrategy) [lrange $OZ(result,KINSolStrategy) 		0 [expr [llength $OZ(result,KINSolStrategy)]		-2]]
	set OZ(result,KINSetMaxNewtonStep) [lrange $OZ(result,KINSetMaxNewtonStep) 		0 [expr [llength $OZ(result,KINSetMaxNewtonStep)]		-2]]

	set OZ(result,KINSpilsSetMaxRestarts) [lrange $OZ(result,KINSpilsSetMaxRestarts) 		0 [expr [llength $OZ(result,KINSpilsSetMaxRestarts)]		-2]]

	set OZ(result,phi)		[lrange $OZ(result,phi) 	0 [expr [llength $OZ(result,phi)]	-2]]
	set OZ(result,T) 		[lrange $OZ(result,T) 		0 [expr [llength $OZ(result,T)]		-2]]
    	set OZ(result,mindimOZ)		[lrange $OZ(result,mindimOZ) 	0 [expr [llength $OZ(result,mindimOZ)]	-2]]
    	set OZ(result,mult) 		[lrange $OZ(result,mult) 	0 [expr [llength $OZ(result,mult)]	-2]]
    	set OZ(result,mix) 		[lrange $OZ(result,mix) 	0 [expr [llength $OZ(result,mix)]	-2]]
    	set OZ(result,dr/dsigma) 	[lrange $OZ(result,dr/dsigma) 	0 [expr [llength $OZ(result,dr/dsigma)]	-2]]
    	set OZ(result,releps)  		[lrange $OZ(result,releps)	0 [expr [llength $OZ(result,releps)]	-2]]
    	set OZ(result,gridlength)  	[lrange $OZ(result,gridlength) 	0 [expr [llength $OZ(result,gridlength)] -2]]
    	set OZ(result,q)  		[lrange $OZ(result,q) 		0 [expr [llength $OZ(result,q)]		-2]]
    	set OZ(result,Sq)  		[lrange $OZ(result,Sq) 		0 [expr [llength $OZ(result,Sq)]	-2]]
    	set OZ(result,r)  		[lrange $OZ(result,r) 		0 [expr [llength $OZ(result,r)]		-2]]
    	set OZ(result,cr)  		[lrange $OZ(result,cr) 		0 [expr [llength $OZ(result,cr)]	-2]]
    	set OZ(result,gr)  		[lrange $OZ(result,gr) 		0 [expr [llength $OZ(result,gr)]	-2]]
    	set OZ(result,gr2)  		[lrange $OZ(result,gr2) 		0 [expr [llength $OZ(result,gr2)]	-2]]
    	set OZ(result,gr3)  		[lrange $OZ(result,gr3) 		0 [expr [llength $OZ(result,gr3)]	-2]]
    	set OZ(result,gr4)  		[lrange $OZ(result,gr4) 		0 [expr [llength $OZ(result,gr4)]	-2]]
    	set OZ(result,hr)  		[lrange $OZ(result,hr) 		0 [expr [llength $OZ(result,hr)]	-2]]
    	set OZ(result,gammar)  		[lrange $OZ(result,gammar) 	0 [expr [llength $OZ(result,gammar)]	-2]]
    	set OZ(result,u,x)          	[lrange $OZ(result,u,x) 	0 [expr [llength $OZ(result,u,x)]	-2]]
    	set OZ(result,Ur)  		[lrange $OZ(result,Ur) 		0 [expr [llength $OZ(result,Ur)]	-2]]
    	set OZ(result,Br)  		[lrange $OZ(result,Br) 		0 [expr [llength $OZ(result,Br)]	-2]]
    	set OZ(result,yr)  		[lrange $OZ(result,yr) 		0 [expr [llength $OZ(result,yr)]	-2]]
    	set OZ(result,fr)  		[lrange $OZ(result,fr) 		0 [expr [llength $OZ(result,fr)]	-2]]
    	set OZ(result,label)		[lrange $OZ(result,label) 	0 [expr [llength $OZ(result,label)]	-2]]
    	Pop_Graph_el ozSQGraph
    	Pop_Graph_el ozgrGraph
    	Pop_Graph_el ozcrGraph
    	Pop_Graph_el ozhrGraph
    	Pop_Graph_el ozgammarGraph
    	Pop_Graph_el ozbetaUrGraph
    	Pop_Graph_el ozBrGraph
    	Pop_Graph_el ozyrGraph
    	Pop_Graph_el ozfrGraph
    	incr OZ(plottedgraphs) -1
    	.oztop.interface.assigning.assign configure -values $OZ(result,label)
    	.oztop.interface.assigning.assign setvalue last
}

proc InterruptOZsolver {} {
        global OZ ozSQGraph ozgrGraph ozcrGraph ozhrGraph ozgammarGraph ozbetaUrGraph ozBrGraph ozyrGraph ozfrGraph sasfit
	set OZ(interrupt) 1
}

proc StartOZsolver {} {
        global OZ ozSQGraph ozgrGraph ozcrGraph ozhrGraph ozgammarGraph ozbetaUrGraph ozBrGraph ozyrGraph ozfrGraph sasfit
        sasfit_timer_start "Start OZ Solver"
		if {[catch {
			set OZ(interrupt) 0
			sasfit_oz_calc OZ
		} msg] } {
		    bgerror $msg
			set ::sasfit(busy) false
			return
		}
	sasfit_timer_stop "OZ solver" "calculation finished" "."
	
	sasfit_timer_start "Start plotting OZ Solver results"
        if {$OZ(color_i) >= [llength $ozSQGraph(colorselection)]} {
            set OZ(color_i) 0
            incr OZ(symbol_i)
            if {$OZ(symbol_i) >= [llength $ozSQGraph(symbolselection)]} {set $OZ(symbol_i) 0}
        }
        set color_n  [lindex $ozSQGraph(colorselection)  $OZ(color_i)]
        set symbol_n [lindex $ozSQGraph(symbolselection) $OZ(symbol_i)]
        set count_n  $OZ(plottedgraphs)

        incr OZ(color_i)

        #clearGraph_el ozSQGraph
        set ozSQGraph(x,type) arcsinh(x)
        Put_Graph_el ozSQGraph $OZ(res,s,x) $OZ(res,s,y)

        set ozSQGraph(e,linehide)   [lreplace $ozSQGraph(e,linehide)   $count_n  $count_n 1]
        set ozSQGraph(e,dashcolor)  [lreplace $ozSQGraph(e,dashcolor)  $count_n  $count_n $color_n]
        set ozSQGraph(e,fill)       [lreplace $ozSQGraph(e,fill)       $count_n  $count_n $color_n]
        set ozSQGraph(e,outline)    [lreplace $ozSQGraph(e,outline)    $count_n  $count_n $color_n]
        set ozSQGraph(e,symbol)     [lreplace $ozSQGraph(e,symbol)     $count_n  $count_n $symbol_n]
        set ozSQGraph(l,legendtext) [lreplace $ozSQGraph(l,legendtext) $count_n  $count_n $OZ(label)]
        RefreshGraph ozSQGraph

        #clearGraph_el ozcrGraph 
        set ozcrGraph(x,type) arcsinh(x)
        Put_Graph_el ozcrGraph $OZ(res,c,x) $OZ(res,c,y)
        set ozcrGraph(e,linehide)   [lreplace $ozcrGraph(e,linehide)   $count_n  $count_n 1]
        set ozcrGraph(e,dashcolor)  [lreplace $ozcrGraph(e,dashcolor)  $count_n  $count_n $color_n]
        set ozcrGraph(e,fill)       [lreplace $ozcrGraph(e,fill)       $count_n  $count_n $color_n]
        set ozcrGraph(e,outline)    [lreplace $ozcrGraph(e,outline)    $count_n  $count_n $color_n]
        set ozcrGraph(l,legendtext) [lreplace $ozcrGraph(l,legendtext) $count_n  $count_n $OZ(label)]
        set ozcrGraph(e,symbol)     [lreplace $ozcrGraph(e,symbol)     $count_n  $count_n $symbol_n]
        RefreshGraph ozcrGraph
        
        #clearGraph_el ozgammarGraph 
	set ozgammarGraph(x,type) arcsinh(x)
	Put_Graph_el ozgammarGraph $OZ(res,gamma,x) $OZ(res,gamma,y)
	set ozgammarGraph(e,linehide)   [lreplace $ozgammarGraph(e,linehide)   $count_n  $count_n 1]
	set ozgammarGraph(e,dashcolor)  [lreplace $ozgammarGraph(e,dashcolor)  $count_n  $count_n $color_n]
	set ozgammarGraph(e,fill)       [lreplace $ozgammarGraph(e,fill)       $count_n  $count_n $color_n]
	set ozgammarGraph(e,outline)    [lreplace $ozgammarGraph(e,outline)    $count_n  $count_n $color_n]
	set ozgammarGraph(l,legendtext) [lreplace $ozgammarGraph(l,legendtext) $count_n  $count_n $OZ(label)]
	set ozgammarGraph(e,symbol)     [lreplace $ozgammarGraph(e,symbol)     $count_n  $count_n $symbol_n]
        RefreshGraph ozgammarGraph

        #clearGraph_el ozgrGraph 
        set ozgrGraph(x,type) arcsinh(x)
        Put_Graph_el ozgrGraph $OZ(res,g,x) $OZ(res,g,y)
        set ozgrGraph(e,linehide)   [lreplace $ozgrGraph(e,linehide)   $count_n  $count_n 1]
        set ozgrGraph(e,dashcolor)  [lreplace $ozgrGraph(e,dashcolor)  $count_n  $count_n $color_n]
        set ozgrGraph(e,fill)       [lreplace $ozgrGraph(e,fill)       $count_n  $count_n $color_n]
        set ozgrGraph(e,outline)    [lreplace $ozgrGraph(e,outline)    $count_n  $count_n $color_n]
        set ozgrGraph(l,legendtext) [lreplace $ozgrGraph(l,legendtext) $count_n  $count_n $OZ(label)]
        set ozgrGraph(e,symbol)     [lreplace $ozgrGraph(e,symbol)     $count_n  $count_n $symbol_n]
        RefreshGraph ozgrGraph
        
        #clearGraph_el ozhrGraph 
        set ozhrGraph(x,type) arcsinh(x)
        Put_Graph_el ozhrGraph $OZ(res,h,x) $OZ(res,h,y)
        set ozhrGraph(e,linehide)   [lreplace $ozhrGraph(e,linehide)   $count_n  $count_n 1]
        set ozhrGraph(e,dashcolor)  [lreplace $ozhrGraph(e,dashcolor)  $count_n  $count_n $color_n]
        set ozhrGraph(e,fill)       [lreplace $ozhrGraph(e,fill)       $count_n  $count_n $color_n]
        set ozhrGraph(e,outline)    [lreplace $ozhrGraph(e,outline)    $count_n  $count_n $color_n]
        set ozhrGraph(l,legendtext) [lreplace $ozhrGraph(l,legendtext) $count_n  $count_n $OZ(label)]
        set ozhrGraph(e,symbol)     [lreplace $ozhrGraph(e,symbol)     $count_n  $count_n $symbol_n]
        RefreshGraph ozhrGraph

        #clearGraph_el ozbetaUrGraph 
        set ozbetaUrGraph(x,type) arcsinh(x)
        Put_Graph_el ozbetaUrGraph $OZ(res,u,x) $OZ(res,u,y)
        set ozbetaUrGraph(e,linehide)   [lreplace $ozbetaUrGraph(e,linehide)   $count_n  $count_n 1]
        set ozbetaUrGraph(e,dashcolor)  [lreplace $ozbetaUrGraph(e,dashcolor)  $count_n  $count_n $color_n]
        set ozbetaUrGraph(e,fill)       [lreplace $ozbetaUrGraph(e,fill)       $count_n  $count_n $color_n]
        set ozbetaUrGraph(e,outline)    [lreplace $ozbetaUrGraph(e,outline)    $count_n  $count_n $color_n]
        set ozbetaUrGraph(l,legendtext) [lreplace $ozbetaUrGraph(l,legendtext) $count_n  $count_n $OZ(label)]
        set ozbetaUrGraph(e,symbol)     [lreplace $ozbetaUrGraph(e,symbol)     $count_n  $count_n $symbol_n]
        RefreshGraph ozbetaUrGraph
        
        #clearGraph_el ozBrGraph 
	set ozBrGraph(x,type) arcsinh(x)
	Put_Graph_el ozBrGraph $OZ(res,B,x) $OZ(res,B,y)
	set ozBrGraph(e,linehide)   [lreplace $ozBrGraph(e,linehide)   $count_n  $count_n 1]
	set ozBrGraph(e,dashcolor)  [lreplace $ozBrGraph(e,dashcolor)  $count_n  $count_n $color_n]
	set ozBrGraph(e,fill)       [lreplace $ozBrGraph(e,fill)       $count_n  $count_n $color_n]
	set ozBrGraph(e,outline)    [lreplace $ozBrGraph(e,outline)    $count_n  $count_n $color_n]
	set ozBrGraph(l,legendtext) [lreplace $ozBrGraph(l,legendtext) $count_n  $count_n $OZ(label)]
	set ozBrGraph(e,symbol)     [lreplace $ozBrGraph(e,symbol)     $count_n  $count_n $symbol_n]
        RefreshGraph ozBrGraph
        
        #clearGraph_el ozyrGraph 
	set ozyrGraph(x,type) arcsinh(x)
	Put_Graph_el ozyrGraph $OZ(res,y,x) $OZ(res,y,y)
	set ozyrGraph(e,linehide)   [lreplace $ozyrGraph(e,linehide)   $count_n  $count_n 1]
	set ozyrGraph(e,dashcolor)  [lreplace $ozyrGraph(e,dashcolor)  $count_n  $count_n $color_n]
	set ozyrGraph(e,fill)       [lreplace $ozyrGraph(e,fill)       $count_n  $count_n $color_n]
	set ozyrGraph(e,outline)    [lreplace $ozyrGraph(e,outline)    $count_n  $count_n $color_n]
	set ozyrGraph(l,legendtext) [lreplace $ozyrGraph(l,legendtext) $count_n  $count_n $OZ(label)]
	set ozyrGraph(e,symbol)     [lreplace $ozyrGraph(e,symbol)     $count_n  $count_n $symbol_n]
        RefreshGraph ozyrGraph
        
        #clearGraph_el ozfrGraph 
	set ozfrGraph(x,type) arcsinh(x)
	Put_Graph_el ozfrGraph $OZ(res,f,x) $OZ(res,f,y)
	set ozfrGraph(e,linehide)   [lreplace $ozfrGraph(e,linehide)   $count_n  $count_n 1]
	set ozfrGraph(e,dashcolor)  [lreplace $ozfrGraph(e,dashcolor)  $count_n  $count_n $color_n]
	set ozfrGraph(e,fill)       [lreplace $ozfrGraph(e,fill)       $count_n  $count_n $color_n]
	set ozfrGraph(e,outline)    [lreplace $ozfrGraph(e,outline)    $count_n  $count_n $color_n]
	set ozfrGraph(l,legendtext) [lreplace $ozfrGraph(l,legendtext) $count_n  $count_n $OZ(label)]
	set ozfrGraph(e,symbol)     [lreplace $ozfrGraph(e,symbol)     $count_n  $count_n $symbol_n]
        RefreshGraph ozfrGraph
        
        incr  OZ(plottedgraphs)
        put_OZ_res
        sasfit_timer_stop "Plotting OZ solver results" "plotting finished" "."
        set OZ(progressbar) 0
}


proc ReplotOZsolver {} {
   global OZ ozSQGraph ozgrGraph ozcrGraph ozhrGraph ozgammarGraph ozbetaUrGraph ozBrGraph ozyrGraph ozfrGraph sasfit
   set OZ(color_i) 0
   set OZ(symbol_i) 0
   set OZ(plottedgraphs) 0
   set OZ(progressbar) 1
	
   for {set i 0} {$i < [llength $OZ(result,q)]} {incr i} {
	sasfit_timer_start "Start replotting OZ Solver result $i"
	set OZ(res,s,x) [lindex $OZ(result,q)  $i]
	set OZ(res,s,y) [lindex $OZ(result,Sq) $i]
	set OZ(res,c,x) [lindex $OZ(result,r)  $i]
	set OZ(res,c,y) [lindex $OZ(result,cr) $i]
	set OZ(res,c,x) [lindex $OZ(result,r)  $i]
	set OZ(res,gamma,y) [lindex $OZ(result,gammar) $i]
	set OZ(res,g,x) [lindex $OZ(result,r)  $i]
	set OZ(res,g,y) [lindex $OZ(result,gr) $i]
	set OZ(res,g2,x) [lindex $OZ(result,r)  $i]
	set OZ(res,g2,y) [lindex $OZ(result,gr2) $i]
	set OZ(res,g3,x) [lindex $OZ(result,r)  $i]
	set OZ(res,g3,y) [lindex $OZ(result,gr3) $i]
	set OZ(res,g4,x) [lindex $OZ(result,r)  $i]
	set OZ(res,g4,y) [lindex $OZ(result,gr4) $i]
	set OZ(res,h,x) [lindex $OZ(result,r)  $i]
	set OZ(res,h,y) [lindex $OZ(result,hr) $i]
	set OZ(res,u,x) [lindex $OZ(result,u,x) $i]
	set OZ(res,u,y) [lindex $OZ(result,Ur) $i]
	set OZ(res,B,x) [lindex $OZ(result,u,x) $i]
	set OZ(res,B,y) [lindex $OZ(result,Br) $i]
	set OZ(res,y,x) [lindex $OZ(result,u,x) $i]
	set OZ(res,y,y) [lindex $OZ(result,yr) $i]
	set OZ(res,f,x) [lindex $OZ(result,u,x) $i]
	set OZ(res,f,y) [lindex $OZ(result,fr) $i]
	
	set OZ(label)   [lindex $OZ(result,label) $i]
        if {$OZ(color_i) >= [llength $ozSQGraph(colorselection)]} {
            set OZ(color_i) 0
            incr OZ(symbol_i)
            if {$OZ(symbol_i) >= [llength $ozSQGraph(symbolselection)]} {set $OZ(symbol_i) 0}
        }
        set color_n  [lindex $ozSQGraph(colorselection)  $OZ(color_i)]
        set symbol_n [lindex $ozSQGraph(symbolselection) $OZ(symbol_i)]
        set count_n  $OZ(plottedgraphs)

        incr OZ(color_i)

        #clearGraph_el ozSQGraph
        set ozSQGraph(x,type) arcsinh(x)
        Put_Graph_el ozSQGraph $OZ(res,s,x) $OZ(res,s,y)
        set ozSQGraph(e,linehide)   [lreplace $ozSQGraph(e,linehide)   $count_n  $count_n 1]
        set ozSQGraph(e,dashcolor)  [lreplace $ozSQGraph(e,dashcolor)  $count_n  $count_n $color_n]
        set ozSQGraph(e,fill)       [lreplace $ozSQGraph(e,fill)       $count_n  $count_n $color_n]
        set ozSQGraph(e,outline)    [lreplace $ozSQGraph(e,outline)    $count_n  $count_n $color_n]
        set ozSQGraph(e,symbol)     [lreplace $ozSQGraph(e,symbol)     $count_n  $count_n $symbol_n]
        set ozSQGraph(l,legendtext) [lreplace $ozSQGraph(l,legendtext) $count_n  $count_n $OZ(label)]
        RefreshGraph ozSQGraph
        incr OZ(progressbar) 

        #clearGraph_el ozcrGraph 
        set ozcrGraph(x,type) arcsinh(x)
        Put_Graph_el ozcrGraph $OZ(res,c,x) $OZ(res,c,y)
        set ozcrGraph(e,linehide)   [lreplace $ozcrGraph(e,linehide)   $count_n  $count_n 1]
        set ozcrGraph(e,dashcolor)  [lreplace $ozcrGraph(e,dashcolor)  $count_n  $count_n $color_n]
        set ozcrGraph(e,fill)       [lreplace $ozcrGraph(e,fill)       $count_n  $count_n $color_n]
        set ozcrGraph(e,outline)    [lreplace $ozcrGraph(e,outline)    $count_n  $count_n $color_n]
        set ozcrGraph(l,legendtext) [lreplace $ozcrGraph(l,legendtext) $count_n  $count_n $OZ(label)]
        set ozcrGraph(e,symbol)     [lreplace $ozcrGraph(e,symbol)     $count_n  $count_n $symbol_n]
        RefreshGraph ozcrGraph
        incr OZ(progressbar) 

        #clearGraph_el ozgammarGraph 
        set ozgammarGraph(x,type) arcsinh(x)
        Put_Graph_el ozgammarGraph $OZ(res,gamma,x) $OZ(res,gamma,y)
        set ozgammarGraph(e,linehide)   [lreplace $ozgammarGraph(e,linehide)   $count_n  $count_n 1]
        set ozgammarGraph(e,dashcolor)  [lreplace $ozgammarGraph(e,dashcolor)  $count_n  $count_n $color_n]
        set ozgammarGraph(e,fill)       [lreplace $ozgammarGraph(e,fill)       $count_n  $count_n $color_n]
        set ozgammarGraph(e,outline)    [lreplace $ozgammarGraph(e,outline)    $count_n  $count_n $color_n]
        set ozgammarGraph(l,legendtext) [lreplace $ozgammarGraph(l,legendtext) $count_n  $count_n $OZ(label)]
        set ozgammarGraph(e,symbol)     [lreplace $ozgammarGraph(e,symbol)     $count_n  $count_n $symbol_n]
        RefreshGraph ozgammarGraph
        incr OZ(progressbar) 
        
        #clearGraph_el ozgrGraph 
        set ozgrGraph(x,type) arcsinh(x)
        Put_Graph_el ozgrGraph $OZ(res,g,x) $OZ(res,g,y)
        set ozgrGraph(e,linehide)   [lreplace $ozgrGraph(e,linehide)   $count_n  $count_n 1]
        set ozgrGraph(e,dashcolor)  [lreplace $ozgrGraph(e,dashcolor)  $count_n  $count_n $color_n]
        set ozgrGraph(e,fill)       [lreplace $ozgrGraph(e,fill)       $count_n  $count_n $color_n]
        set ozgrGraph(e,outline)    [lreplace $ozgrGraph(e,outline)    $count_n  $count_n $color_n]
        set ozgrGraph(l,legendtext) [lreplace $ozgrGraph(l,legendtext) $count_n  $count_n $OZ(label)]
        set ozgrGraph(e,symbol)     [lreplace $ozgrGraph(e,symbol)     $count_n  $count_n $symbol_n]
        RefreshGraph ozgrGraph
        incr OZ(progressbar) 
	     
        #clearGraph_el ozhrGraph 
        set ozhrGraph(x,type) arcsinh(x)
        Put_Graph_el ozhrGraph $OZ(res,h,x) $OZ(res,h,y)
        set ozhrGraph(e,linehide)   [lreplace $ozhrGraph(e,linehide)   $count_n  $count_n 1]
        set ozhrGraph(e,dashcolor)  [lreplace $ozhrGraph(e,dashcolor)  $count_n  $count_n $color_n]
        set ozhrGraph(e,fill)       [lreplace $ozhrGraph(e,fill)       $count_n  $count_n $color_n]
        set ozhrGraph(e,outline)    [lreplace $ozhrGraph(e,outline)    $count_n  $count_n $color_n]
        set ozhrGraph(l,legendtext) [lreplace $ozhrGraph(l,legendtext) $count_n  $count_n $OZ(label)]
        set ozhrGraph(e,symbol)     [lreplace $ozhrGraph(e,symbol)     $count_n  $count_n $symbol_n]
        RefreshGraph ozhrGraph
        incr OZ(progressbar) 
		
        #clearGraph_el ozbetaUrGraph 
        set ozbetaUrGraph(x,type) arcsinh(x)
        Put_Graph_el ozbetaUrGraph $OZ(res,u,x) $OZ(res,u,y)
        set ozbetaUrGraph(e,linehide)   [lreplace $ozbetaUrGraph(e,linehide)   $count_n  $count_n 1]
        set ozbetaUrGraph(e,dashcolor)  [lreplace $ozbetaUrGraph(e,dashcolor)  $count_n  $count_n $color_n]
        set ozbetaUrGraph(e,fill)       [lreplace $ozbetaUrGraph(e,fill)       $count_n  $count_n $color_n]
        set ozbetaUrGraph(e,outline)    [lreplace $ozbetaUrGraph(e,outline)    $count_n  $count_n $color_n]
        set ozbetaUrGraph(l,legendtext) [lreplace $ozbetaUrGraph(l,legendtext) $count_n  $count_n $OZ(label)]
        set ozbetaUrGraph(e,symbol)     [lreplace $ozbetaUrGraph(e,symbol)     $count_n  $count_n $symbol_n]
        RefreshGraph ozbetaUrGraph
        incr OZ(progressbar) 

	#clearGraph_el ozBrGraph 
	set ozBrGraph(x,type) arcsinh(x)
	Put_Graph_el ozBrGraph $OZ(res,B,x) $OZ(res,B,y)
	set ozBrGraph(e,linehide)   [lreplace $ozBrGraph(e,linehide)   $count_n  $count_n 1]
	set ozBrGraph(e,dashcolor)  [lreplace $ozBrGraph(e,dashcolor)  $count_n  $count_n $color_n]
	set ozBrGraph(e,fill)       [lreplace $ozBrGraph(e,fill)       $count_n  $count_n $color_n]
	set ozBrGraph(e,outline)    [lreplace $ozBrGraph(e,outline)    $count_n  $count_n $color_n]
	set ozBrGraph(l,legendtext) [lreplace $ozBrGraph(l,legendtext) $count_n  $count_n $OZ(label)]
	set ozBrGraph(e,symbol)     [lreplace $ozBrGraph(e,symbol)     $count_n  $count_n $symbol_n]
        RefreshGraph ozBrGraph
        incr OZ(progressbar) 
        
        #clearGraph_el ozyrGraph 
	set ozyrGraph(x,type) arcsinh(x)
	Put_Graph_el ozyrGraph $OZ(res,y,x) $OZ(res,y,y)
	set ozyrGraph(e,linehide)   [lreplace $ozyrGraph(e,linehide)   $count_n  $count_n 1]
	set ozyrGraph(e,dashcolor)  [lreplace $ozyrGraph(e,dashcolor)  $count_n  $count_n $color_n]
	set ozyrGraph(e,fill)       [lreplace $ozyrGraph(e,fill)       $count_n  $count_n $color_n]
	set ozyrGraph(e,outline)    [lreplace $ozyrGraph(e,outline)    $count_n  $count_n $color_n]
	set ozyrGraph(l,legendtext) [lreplace $ozyrGraph(l,legendtext) $count_n  $count_n $OZ(label)]
	set ozyrGraph(e,symbol)     [lreplace $ozyrGraph(e,symbol)     $count_n  $count_n $symbol_n]
        RefreshGraph ozyrGraph
        incr OZ(progressbar) 
        
        #clearGraph_el ozfrGraph 
	set ozfrGraph(x,type) arcsinh(x)
	Put_Graph_el ozfrGraph $OZ(res,f,x) $OZ(res,f,y)
	set ozfrGraph(e,linehide)   [lreplace $ozfrGraph(e,linehide)   $count_n  $count_n 1]
	set ozfrGraph(e,dashcolor)  [lreplace $ozfrGraph(e,dashcolor)  $count_n  $count_n $color_n]
	set ozfrGraph(e,fill)       [lreplace $ozfrGraph(e,fill)       $count_n  $count_n $color_n]
	set ozfrGraph(e,outline)    [lreplace $ozfrGraph(e,outline)    $count_n  $count_n $color_n]
	set ozfrGraph(l,legendtext) [lreplace $ozfrGraph(l,legendtext) $count_n  $count_n $OZ(label)]
	set ozfrGraph(e,symbol)     [lreplace $ozfrGraph(e,symbol)     $count_n  $count_n $symbol_n]
        RefreshGraph ozfrGraph
        incr OZ(progressbar) 
        
        sasfit_timer_stop "Relotting $i-th OZ solver result" "plotting finished" "."
        incr OZ(progressbar) 
        incr OZ(plottedgraphs)
    }
    set OZ(progressbar) 0
}

proc ClearOZsolver {} {
    global ozSQGraph ozcrGraph ozgrGraph ozhrGraph ozgammarGraph ozbetaUrGraph ozBrGraph ozyrGraph ozfrGraph OZ
    clearGraph_el ozSQGraph
    clearGraph_el ozcrGraph 
    clearGraph_el ozgammarGraph 
    clearGraph_el ozgrGraph 
    clearGraph_el ozhrGraph 
    clearGraph_el ozbetaUrGraph 
    clearGraph_el ozBrGraph 
    clearGraph_el ozfrGraph 
    clearGraph_el ozyrGraph 
    set OZ(color_i) 0
    set OZ(symbol_i) 0
    set OZ(plottedgraphs) 0
    
    set OZ(result,closure) {}
    set OZ(result,potential) {}
    set OZ(result,p) {}
    set OZ(result,algorithm) {}
		
	set OZ(result,KINSetMAA) {}
	set OZ(result,KINSetFuncNormTol) {}
	set OZ(result,KINSetScaledSteptol) {}
	set OZ(result,KINSetNumMaxIters) {}
	set OZ(result,KINSetPrintLevel) {}
	set OZ(result,KINSetEtaForm) {}
	set OZ(result,KINSpilsSetMaxRestarts) {}
	set OZ(result,KINSolStrategy) {}
	set OZ(result,KINSetMaxNewtonStep) {}
	
    set OZ(result,phi) {}
    set OZ(result,T) {}
    set OZ(result,mindimOZ) {}
    set OZ(result,mult) {}
    set OZ(result,mix) {}
    set OZ(result,dr/dsigma) {}
    set OZ(result,releps) {}
    set OZ(result,gridlength) {}
    set OZ(result,q) {}
    set OZ(result,Sq) {}
    set OZ(result,r) {}
    set OZ(result,cr) {}
    set OZ(result,gammar) {}
    set OZ(result,gr) {}
    set OZ(result,hr) {}
    set OZ(result,u,x) {}
    set OZ(result,Ur) {}
    set OZ(result,Br) {}
    set OZ(result,yr) {}
    set OZ(result,fr) {}
    set OZ(result,label) {}
	set OZ(label) unknown
    .oztop.interface.assigning.assign configure -text ""
    .oztop.interface.assigning.assign configure -values $OZ(result,label)
    .oztop.interface.assigning.assign setvalue last
}

proc oz_input_names {} {
	global OZ
	switch $OZ(potential) {
		HardSphere {
			set OZ(p0,name) diameter
			set OZ(p1,name) ""
			set OZ(p2,name) ""
			set OZ(p3,name) ""
			set OZ(p4,name) ""
			set OZ(p5,name) ""
			set OZ(p6,name) ""
			}
		StickyHardSphere {
			set OZ(p0,name) diameter
			set OZ(p1,name) tau
			set OZ(p2,name) delta
			set OZ(p3,name) ""
			set OZ(p4,name) ""
			set OZ(p5,name) ""
			set OZ(p6,name) ""
			}
		SquareWell {
			set OZ(p0,name) diameter
			set OZ(p1,name) epsilon
			set OZ(p2,name) delta
			set OZ(p3,name) ""
			set OZ(p4,name) ""
			set OZ(p5,name) ""
			set OZ(p6,name) ""
			}
		SoftSphere {
			set OZ(p0,name) diameter
			set OZ(p1,name) epsilon
			set OZ(p2,name) "stiffness n"
			set OZ(p3,name) ""
			set OZ(p4,name) ""
			set OZ(p5,name) ""
			set OZ(p6,name) ""
			}
		Fermi {
			set OZ(p0,name) diameter
			set OZ(p1,name) epsilon
			set OZ(p2,name) xi
			set OZ(p3,name) ""
			set OZ(p4,name) ""
			set OZ(p5,name) ""
			set OZ(p6,name) ""
			}
		LennardJones {
			set OZ(p0,name) diameter
			set OZ(p1,name) epsilon
			set OZ(p2,name) ""
			set OZ(p3,name) ""
			set OZ(p4,name) ""
			set OZ(p5,name) ""
			set OZ(p6,name) ""
			}
		"Depl-Sph-Sph" {
			set OZ(p0,name) "diam. (large)"
			set OZ(p1,name) "diam. (small)"
			set OZ(p2,name) "n (numb. dens.)"
			set OZ(p3,name) ""
			set OZ(p4,name) ""
			set OZ(p5,name) ""
			set OZ(p6,name) ""
			}
		"Depl-Sph-Discs" {
			set OZ(p0,name) "diam. (sphere)"
			set OZ(p1,name) "diam. (disc)"
			set OZ(p2,name) "n (numb. dens.)"
			set OZ(p3,name) ""
			set OZ(p4,name) ""
			set OZ(p5,name) ""
			set OZ(p6,name) ""
			}
		"Depl-Sph-Rods" {
			set OZ(p0,name) "diam. (sphere)"
			set OZ(p1,name) "length (rods)"
			set OZ(p2,name) "n (numb. dens.)"
			set OZ(p3,name) ""
			set OZ(p4,name) ""
			set OZ(p5,name) ""
			set OZ(p6,name) ""
			}
		IonicMicrogel {
			set OZ(p0,name) diameter
			set OZ(p1,name) Z
			set OZ(p2,name) ed
			set OZ(p3,name) kpi
			set OZ(p4,name) epsilon
			set OZ(p5,name) ""
			set OZ(p6,name) ""
			}
		PenetrableSphere {
			set OZ(p0,name) diameter
			set OZ(p1,name) epsilon
			set OZ(p2,name) ""
			set OZ(p3,name) ""
			set OZ(p4,name) ""
			set OZ(p5,name) ""
			set OZ(p6,name) ""
			}
		DLVO {
			set OZ(p0,name) diameter
			set OZ(p1,name) kappa
			set OZ(p2,name) Z
			set OZ(p3,name) LB
			set OZ(p4,name) A
			set OZ(p5,name) ""
			set OZ(p6,name) ""
			}
		"GGCM-n" {
			set OZ(p0,name) diameter
			set OZ(p1,name) epsilon
			set OZ(p2,name) n
			set OZ(p3,name) alpha
			set OZ(p4,name) ""
			set OZ(p5,name) ""
			set OZ(p6,name) ""
			}
		"StarPolymer (f>10)" {
			set OZ(p0,name) diameter
			set OZ(p1,name) functionality
			set OZ(p2,name) ""
			set OZ(p3,name) ""
			set OZ(p4,name) ""
			set OZ(p5,name) ""
			set OZ(p6,name) ""
			}
		"StarPolymer (f<10)" {
			set OZ(p0,name) diameter
			set OZ(p1,name) functionality
			set OZ(p2,name) ""
			set OZ(p3,name) ""
			set OZ(p4,name) ""
			set OZ(p5,name) ""
			set OZ(p6,name) ""
			}
		"HS 3Yukawa" {
			set OZ(p0,name) diameter
			set OZ(p1,name) K1
			set OZ(p2,name) lambda1
			set OZ(p3,name) K2
			set OZ(p4,name) lambda2
			set OZ(p5,name) K3
			set OZ(p6,name) lambda3
			}
		"DLVO Hydra" {
			set OZ(p0,name) diameter
			set OZ(p1,name) kappa
			set OZ(p2,name) Z
			set OZ(p3,name) LB
			set OZ(p4,name) A
			set OZ(p5,name) gHy
			set OZ(p6,name) DH
			}
		default {
			set OZ(p0,name) ""
			set OZ(p1,name) ""
			set OZ(p2,name) ""
			set OZ(p3,name) ""
			set OZ(p4,name) ""
			set OZ(p5,name) ""
			set OZ(p6,name) ""
			}
	}
}

proc update_ozmenu {} {
    global OZ
    set w .oztop.interface.assigning.assign
    set idx [$w getvalue]
    if {$idx >=0} {
       set OZ(closure)	 [lindex $OZ(result,closure) $idx]
       set OZ(potential) [lindex $OZ(result,potential) $idx]
       set OZ(mixstrategy) [lindex $OZ(result,mixstrategy) $idx]
       oz_input_names
       set OZ(p)	 [lindex $OZ(result,p) $idx]
       for {set i 0} {$i < 16} {incr i} {
           set OZ(p$i) [lindex $OZ(p) $i]
       }      
       set OZ(algorithm) [lindex $OZ(result,algorithm) $idx]
	   
	   set OZ(KINSetMAA) [lindex OZ(result,KINSetMAA) $idx]
	   set OZ(KINSetFuncNormTol) [lindex OZ(result,KINSetFuncNormTol)  $idx]
	   set OZ(KINSetScaledSteptol) [lindex OZ(result,KINSetScaledSteptol) $idx]
	   set OZ(KINSetNumMaxIters) [lindex OZ(result,KINSetNumMaxIters)  $idx]
	   set OZ(KINSetPrintLevel) [lindex OZ(result,KINSetPrintLevel)  $idx]
	   set OZ(KINSetEtaForm) [lindex OZ(result,KINSetEtaForm)  $idx]
	   set OZ(KINSolStrategy) [lindex OZ(result,KINSolStrategy)  $idx
	   set OZ(KINSetMaxNewtonStep) [lindex OZ(result,KINSetMaxNewtonStep)  $idx
	   set OZ(KINSpilsSetMaxRestarts) [lindex OZ(result,KINSpilsSetMaxRestarts) $idx] 

       set OZ(phi)	[lindex $OZ(result,phi) $idx]
       set OZ(T)	[lindex $OZ(result,T) $idx]
       set OZ(mindimOZ)	[lindex $OZ(result,mindimOZ) $idx]
       set OZ(mult)	[lindex $OZ(result,mult) $idx]
       set OZ(mix)	[lindex $OZ(result,mix) $idx]
       set OZ(dr/dsigma) [lindex $OZ(result,dr/dsigma) $idx]
       set OZ(releps)	[lindex $OZ(result,releps) $idx]
       set OZ(gridlength)	[lindex $OZ(result,gridlength) $idx]
#       set [lindex $OZ(result,q) $idx]
#       set [lindex $OZ(result,Sq) $idx]
#       set [lindex $OZ(result,r) $idx]
#       set [lindex $OZ(result,cr) $idx]
#       set [lindex $OZ(result,gr) $idx]
#       set [lindex $OZ(result,Ur) $idx]
       set OZ(label)	[lindex $OZ(result,label) $idx]
    }
    puts "index: $idx"
	
}

proc configOZalgorithm {} {
	global OZ
	set w .ozconfig
    if {[winfo exists $w]} {destroy $w}
    toplevel $w
    wm geometry $w
    wm title $w "configuration of Ornstein Zernike solver algorithm"
    raise $w
	
    label $w.algorithmtext -text "algorithm:"  
    ComboBox $w.algorithmvalue \
    		-values {"Picard iteration" "Mann iteration" \
    		         "Ishikawa iteration" "Noor iteration" \
    		         "SP iteration" "S iteration"	\
    		         "CR iteration" "Picard-S iteration" \
    		         "PMH iteration" "Mann II iteration" \
    		         "Krasnoselskij iteration" \
    		         "S* iteration" \
				 "dNewton" "Hybrid" \
	    		 "Hybrids (int. sc.)" "Broyden" "Biggs_Andrews"\
				 "Anderson mixing" "KINSOL_FP" "GMRES" "Bi-CGStab" "TFQMR" "FGMRES" 
			} \
	    -textvariable OZ(algorithm) -width 19  -editable 0
    
    grid  $w.algorithmtext -sticky e\
	    -column 0 -row 15
    grid  $w.algorithmvalue\
	    -column 1 -row 15


    label $w.gridtext -text "gridsize (n x 128), n:"  
    entry $w.gridvalue -textvariable OZ(mult)
	ComboBox $w.mixtext \
	    -values {"mixing parameter (const)" "mixing parameter (err)" "mixing parameter (reward/penalty)"} \
	    -textvariable OZ(mixstrategy) -editable 0
    entry $w.mixvalue -textvariable OZ(mix)
    label $w.ittext -text "max iterations:"  
    entry $w.itvalue -textvariable OZ(maxit)
    label $w.relepstext -text "iteration precision:"  
    entry $w.relepsvalue -textvariable OZ(releps) 
    label $w.drdsigmatext -text "rel. grid step width:" 
    entry $w.drdsigmavalue -textvariable OZ(dr/dsigma)
    

    grid  $w.gridtext -sticky e\
	    -column 0 -row 16
    grid  $w.mixtext -sticky e\
	    -column 0 -row 17
    grid  $w.ittext -sticky e\
	    -column 0 -row 18
    grid  $w.relepstext -sticky e\
	    -column 0 -row 19
    grid  $w.drdsigmatext -sticky e\
	    -column 0 -row 20
    grid  $w.gridvalue \
	    -column 1 -row 16
    grid  $w.mixvalue \
	    -column 1 -row 17
    grid  $w.itvalue \
	    -column 1 -row 18
    grid  $w.relepsvalue \
	    -column 1 -row 19
    grid  $w.drdsigmavalue \
	    -column 1 -row 20
		
	label $w.kinsolMMAtext -text KINSetMAA
    entry $w.kinsolMMAvalue -textvariable OZ(KINSetMAA)

	label $w.kinsolFuncNormToltext -text KINSetFuncNormTol
    entry $w.kinsolFuncNormTolvalue -textvariable OZ(KINSetFuncNormTol)

	label $w.kinsolScaledSteptoltext -text KINSetScaledSteptol
	entry $w.kinsolScaledSteptolvalue -textvariable OZ(KINSetScaledSteptol)

    label $w.kinsolNumMaxIterstext -text KINSetNumMaxIters
	entry $w.kinsolNumMaxItersvalue -textvariable OZ(KINSetNumMaxIters)

    label $w.kinsolPrintLeveltext -text KINSetPrintLevel
	entry $w.kinsolPrintLevelvalue -textvariable OZ(KINSetPrintLevel)

    label $w.kinsolEtaFormtext -text KINSetEtaForm
	ComboBox $w.kinsolEtaFormvalue \
	    -values {"KIN_ETACHOICE1" "KIN_ETACHOICE2" "KIN_ETACONSTANT"} \
	    -textvariable OZ(KINSetEtaForm) -editable 0
		
    label $w.kinsolMaxRestartstext -text KINSpilsSetMaxRestarts
	entry $w.kinsolMaxRestartsvalue -textvariable OZ(KINSpilsSetMaxRestarts)
	
    label $w.kinsolStrategytext -text KINSolStrategy
	ComboBox $w.kinsolStrategyvalue \
	    -values {"KIN_NONE" "KIN_LINESEARCH"} \
	    -textvariable OZ(KINSolStrategy) -editable 0

	label $w.kinsolMaxNewtonSteptext -text KINSetMaxNewtonStep
	entry $w.kinsolMaxNewtonStepvalue -textvariable OZ(KINSetMaxNewtonStep)

    grid  $w.kinsolMMAtext  -column 0 -row 21 -sticky e
    grid  $w.kinsolMMAvalue -column 1 -row 21
    grid  $w.kinsolFuncNormToltext  -column 0 -row 22 -sticky e
    grid  $w.kinsolFuncNormTolvalue -column 1 -row 22
    grid  $w.kinsolScaledSteptoltext  -column 0 -row 23 -sticky e
    grid  $w.kinsolScaledSteptolvalue -column 1 -row 23
    grid  $w.kinsolNumMaxIterstext  -column 0 -row 24 -sticky e
    grid  $w.kinsolNumMaxItersvalue -column 1 -row 24
    grid  $w.kinsolPrintLeveltext  -column 0 -row 25 -sticky e
    grid  $w.kinsolPrintLevelvalue -column 1 -row 25
    grid  $w.kinsolEtaFormtext  -column 0 -row 26 -sticky e
    grid  $w.kinsolEtaFormvalue -column 1 -row 26
    grid  $w.kinsolMaxRestartstext  -column 0 -row 27 -sticky e
    grid  $w.kinsolMaxRestartsvalue -column 1 -row 27
    grid  $w.kinsolStrategytext  -column 0 -row 28 -sticky e
    grid  $w.kinsolStrategyvalue -column 1 -row 28
    grid  $w.kinsolMaxNewtonSteptext  -column 0 -row 29 -sticky e
    grid  $w.kinsolMaxNewtonStepvalue -column 1 -row 29

}
proc sasfit_OZ_solver {} {
    global sasfit ozSQGraph ozgrGraph ozcrGraph ozhrGraph ozgammarGraph ozbetaUrGraph ozBrGraph ozyrGraph ozfrGraph OZ
    set w .oztop
    if {[winfo exists $w]} {destroy $w}
    toplevel $w
    wm geometry $w
    wm title $w "Ornstein Zernike solver"
    raise $w
    bind $w <KeyPress-Return> StartOZsolver
    bind $w <Alt-c> ClearOZsolver
    bind $w <Alt-d> pop_OZ_res
#------------------------------------------------------------------------------
#                               Create OZ tabs

#
#  creating tabset
#
    blt::tabset .oztop.tab -relief sunken -borderwidth 2 
    frame $w.interface 
    frame $w.interface.param   
    frame $w.interface.action   
    frame $w.interface.assigning 
    frame $w.interface.progressbar
    
    set OZ(progressbar) 0
    
	frame $w.messageframe -relief sunken -borderwidth 2 -height 5m
	pack $w.messageframe -side bottom -fill x
	pack propagate $w.messageframe yes
	frame $w.messageframe.m -height 5m -width 70m
	pack $w.messageframe.m  -fill x -expand yes
	message $w.quickmessage -text "" \
           -justify left -aspect 3000 -anchor w 
	message $w.xycoordinates -text "    " \
           -justify right -aspect 3000 -anchor e 
	pack $w.quickmessage  -side left -fill x -expand yes -in $w.messageframe.m
	pack $w.xycoordinates -side left -fill x -in $w.messageframe.m
   
    checkbutton $w.interface.progressbar.label -variable OZ(PrintProgress) -text "progress:"
    ProgressBar $w.interface.progressbar.value \
    		-maximum 100 -width 40m\
    		-type infinite -variable OZ(progressbar)
    pack $w.interface.progressbar.label $w.interface.progressbar.value \
    		-fill both  -expand yes -side left -pady 2m
    
    pack .oztop.tab  -fill both  -expand yes -side right
    pack $w.interface -side left
    pack  $w.interface.param $w.interface.action $w.interface.assigning $w.interface.progressbar -fill y
    
    button  $w.interface.action.calc -text calculate -command {
		StartOZsolver
	}
    button  $w.interface.action.interrupt -text interrupt -bg red -command {
		InterruptOZsolver
		set ::sasfit(busy) false
	}
    button  $w.interface.action.clear -text clear -command {
		ClearOZsolver
		set ::sasfit(busy) false
	}
    button  $w.interface.action.del -text "del last" -command {
		pop_OZ_res
	}
    ComboBox $w.interface.assigning.sqplugin \
	    -values $OZ(plugin_fct_names) \
            -text "SQ oz 1" -editable 0 -width 8
    ComboBox $w.interface.assigning.assign -editable 0 -width 14 \
    	-modifycmd {
    		update_ozmenu
    	}
    .oztop.interface.assigning.assign configure -values $OZ(result,label)
    .oztop.interface.assigning.assign setvalue first
    button   $w.interface.assigning.doassign -text assign \
    	-command {
    		set w .oztop.interface.assigning.assign
    		set plugin_fct [.oztop.interface.assigning.sqplugin getvalue]
			set data_set [.oztop.interface.assigning.assign getvalue]
			if {[expr $data_set+1] > [llength $OZ(result,q)] || $data_set < 0} {
				tk_messageBox -icon error -message "no structure factor curves are available\nstart the OZ solver first!"
				return
			}
    		set q  [lindex $OZ(result,q)  $data_set]
			set Sq [lindex $OZ(result,Sq) $data_set]
			set p  [lindex $OZ(result,p)  $data_set]
			set sigma [lindex $p 0]
			set qsigma {}
			foreach Q $q {
				set QSIGMA [expr $Q*$sigma]
				lappend qsigma $QSIGMA
			}
    		sasfit_oz_assign_data [lindex $OZ(plugin_C_names) $plugin_fct] $qsigma $Sq
    	}
    label    $w.interface.assigning.to -text to

    pack $w.interface.action.interrupt $w.interface.action.calc $w.interface.action.clear $w.interface.action.del \
    	-side left  -padx 2mm -pady 4mm
    pack $w.interface.assigning.doassign \
	 $w.interface.assigning.assign \
	 $w.interface.assigning.to \
         $w.interface.assigning.sqplugin  -side left
 
    set w .oztop.interface
    label $w.param.cltext -text "closure relation:"
    label $w.param.pottext -text "potential:"
    grid $w.param.cltext  -sticky e\
	    -column 0 -row 0
    grid $w.param.pottext  -sticky e\
	    -column 0 -row 1
    ComboBox $w.param.clvalue \
	    -values {"Percus\-Yevick" "Hypernetted\-Chain" "Reference HNC" "modified HNC" PLHNC MSA RMSA mMSA SMSA HMSA \
	             "Rogers\-Young" Verlet MS DH "Vompe\-Martynov" BB BPGG CJVM "Choudhury\-Gosh"} \
	    -textvariable OZ(closure) -editable 0
    grid  $w.param.clvalue\
	    -column 1 -row 0
    ComboBox $w.param.potvalue \
	    -values {"HardSphere" "StickyHardSphere" "SquareWell" "SoftSphere" \
	    		"StarPolymer (f>10)" "StarPolymer (f<10)" "HS 3Yukawa"\
	    		"LennardJones" "Depl-Sph-Sph" "Depl-Sph-Discs" "Depl-Sph-Rods" \
	    		"IonicMicrogel"  \
	    		"PenetrableSphere" "Fermi" "DLVO" "DLVO Hydra" "GGCM-n"} \
	    -textvariable OZ(potential) \
	    -modifycmd {oz_input_names}  -editable 0
    grid  $w.param.potvalue\
	    -column 1 -row 1

    label $w.param.empty1 -text "input parameters for u(r):" -anchor w -justify left -font "Arial 10 bold underline"
    label $w.param.v0text -anchor e -justify right -textvariable OZ(p0,name)
    label $w.param.v1text -anchor e -justify right -textvariable OZ(p1,name)
    label $w.param.v2text -anchor e -justify right -textvariable OZ(p2,name)
    label $w.param.v3text -anchor e -justify right -textvariable OZ(p3,name)
    label $w.param.v4text -anchor e -justify right -textvariable OZ(p4,name)
    label $w.param.v5text -anchor e -justify right -textvariable OZ(p5,name)
    label $w.param.v6text -anchor e -justify right -textvariable OZ(p6,name)
    
    grid  $w.param.empty1 \
	    -column 0 -row 2 -columnspan 2 -sticky w
    grid  $w.param.v0text -sticky e\
	    -column 0 -row 3  
    grid  $w.param.v1text -sticky e\
	    -column 0 -row 4  
    grid  $w.param.v2text -sticky e\
	    -column 0 -row 5  
    grid  $w.param.v3text -sticky e\
	    -column 0 -row 6  
    grid  $w.param.v4text -sticky e\
	    -column 0 -row 7  
    grid  $w.param.v5text -sticky e\
	    -column 0 -row 8
    grid  $w.param.v6text -sticky e\
	    -column 0 -row 9

    entry $w.param.v0value -textvariable OZ(p0)
    entry $w.param.v1value -textvariable OZ(p1)
    entry $w.param.v2value -textvariable OZ(p2)
    entry $w.param.v3value -textvariable OZ(p3)
    entry $w.param.v4value -textvariable OZ(p4)
    entry $w.param.v5value -textvariable OZ(p5)
    entry $w.param.v6value -textvariable OZ(p6)  
    grid  $w.param.v0value\
	    -column 1 -row 3  
    grid  $w.param.v1value\
	    -column 1 -row 4  
    grid  $w.param.v2value\
	    -column 1 -row 5  
    grid  $w.param.v3value\
	    -column 1 -row 6  
    grid  $w.param.v4value\
	    -column 1 -row 7  
    grid  $w.param.v5value\
	    -column 1 -row 8
    grid  $w.param.v6value\
	    -column 1 -row 9

    label $w.param.empty2 -text "parameters for OZ solver:" -font "Arial 10 bold underline" 
    grid  $w.param.empty2 -sticky w\
	    -column 0 -row 10 -columnspan 2

    label $w.param.phitext -text "volume fraction:"  
    entry $w.param.phivalue -textvariable OZ(phi)
    label $w.param.ttext -text "temperature \[K\]:"  
    entry $w.param.tvalue -textvariable OZ(T)
    grid  $w.param.phitext -sticky e\
	    -column 0 -row 11
    grid  $w.param.ttext -sticky e\
	    -column 0 -row 12
    grid  $w.param.phivalue\
	    -column 1 -row 11
    grid  $w.param.tvalue\
	    -column 1 -row 12
	
    label $w.param.empty3 -text " "
	grid $w.param.empty3 \
	    -column 0 -row 13
	
    button  $w.param.algorithmconfig -text "configure OZ solver algorithm" \
	        -font "Arial 10 bold" -bg "light grey" \
			-command {configOZalgorithm}
	grid  $w.param.algorithmconfig -sticky w\
	    -column 0 -row 14 -columnspan 2

    label $w.param.empty4 -text " "
    label $w.param.labeltext -text "label:"  
    entry $w.param.labelvalue -textvariable OZ(label)
    grid $w.param.empty4 \
	    -column 0 -row 15
    grid  $w.param.labeltext -sticky e\
	    -column 0 -row 16
    grid  $w.param.labelvalue \
	    -column 1 -row 16

#
#  create "ozSQGraph"
#
    frame .oztop.tab.sq -relief groove -borderwidth 2 
    set ozSQGraph(w) .oztop.tab.sq.draw
    set ozSQGraph(e,element) 0
    CreateGraph ozSQGraph
    set ozSQGraph(x,logscale) 0
    set ozSQGraph(y,logscale) 0
    set ozSQGraph(x,type)     x
    set ozSQGraph(y,type)     y
    set ozSQGraph(x,title) "Q / nm^-1"
    set ozSQGraph(y,title) "S(Q)"
    set ozSQGraph(l,hide)  no
    pack $ozSQGraph(w) -in .oztop.tab.sq
    pack configure $ozSQGraph(w) -fill both -expand yes

   .oztop.tab insert 0 ozSQGraph
   .oztop.tab tab configure ozSQGraph -text "structure\nfactor\nS(Q)"
   .oztop.tab tab configure ozSQGraph -fill both -padx 0.1i -pady 0.1i \
            -window .oztop.tab.sq -windowheight $sasfit(height) -windowwidth $sasfit(width) 
            

##
## creating ozSQ popup menu
##
    menu .oztop.tab.sq.popup -tearoff 0
    .oztop.tab.sq.popup add command -label "copy to clipboard (wmf)" -un 0 -command {
	window_to_clipboard .oztop.tab.sq
    }
    .oztop.tab.sq.popup add command -label "copy to clipboard (ascii)" -un 0 -command {
     	global ozSQGraph
     	export_clipboard_data ozSQGraph
    }
    .oztop.tab.sq.popup add cascade -label "Graph" -un 0 -menu .oztop.tab.sq.popup.graph
    set m3 [menu .oztop.tab.sq.popup.graph -tearoff 1]
    $m3 add command -label "autoscale" \
    	-command { 
    	   global ozSQGraph
    	   set ozSQGraph(x,min) ""
    	   set ozSQGraph(x,max) ""
    	   set ozSQGraph(y,min) ""
    	   set ozSQGraph(y,max) ""
    	   RefreshGraph ozSQGraph
    	}
    $m3 add command -label "x-axis..." -command {
                                          global ozSQGraph
                                          set_xaxis_layout ozSQGraph
                                       }
    $m3 add command -label "y-axis..." -command {
                                          set_yaxis_layout ozSQGraph
                                       }
    $m3 add command -label "graph layout..." -command {
                                               set_graph_layout ozSQGraph
                                             }
    $m3 add command -label "plot layout..." -command {
                                               set_plot_layout ozSQGraph
                                            }
    .oztop.tab.sq.popup add command -label "Export Data..." \
    	-command { export_blt_graph ozSQGraph }
    bind .oztop.tab.sq.draw <ButtonPress-3> {tk_popup .oztop.tab.sq.popup %X %Y }
    bind .oztop.tab.sq.draw <Double-ButtonPress-1> {tk_popup .oztop.tab.sq.popup %X %Y }
    Blt_ZoomStack $ozSQGraph(w)
#
#  create "ozgrGraph"
#

    frame .oztop.tab.gr -relief groove -borderwidth 2 
    set ozgrGraph(w) .oztop.tab.gr.draw
    set ozgrGraph(e,element) 0
    CreateGraph ozgrGraph
    set ozgrGraph(x,logscale) 0
    set ozgrGraph(y,logscale) 0
    set ozgrGraph(x,type)     x
    set ozgrGraph(y,type)     y
    set ozgrGraph(x,title) "r / nm"
    set ozgrGraph(y,title) "g(r)"
    set ozgrGraph(l,hide)  no
    pack $ozgrGraph(w) -in .oztop.tab.gr
    pack configure $ozgrGraph(w) -fill both -expand yes

   .oztop.tab insert 1 ozgrGraph
   .oztop.tab tab configure ozgrGraph -text "radial\ndistribution\nfunction g(r)"
   .oztop.tab tab configure ozgrGraph -fill both -padx 0.1i -pady 0.1i \
            -window .oztop.tab.gr -windowheight $sasfit(height) -windowwidth $sasfit(width)
            
##
## creating ozgr popup menu
##
    menu .oztop.tab.gr.popup -tearoff 0
    .oztop.tab.gr.popup add command -label "copy to clipboard (wmf)" -un 0 -command {
	window_to_clipboard .oztop.tab.gr
    }
    .oztop.tab.gr.popup add command -label "copy to clipboard (ascii)" -un 0 -command {
     	global ozgrGraph
     	export_clipboard_data ozgrGraph
    }
    .oztop.tab.gr.popup add cascade -label "Graph" -un 0 -menu .oztop.tab.gr.popup.graph
    set m3 [menu .oztop.tab.gr.popup.graph -tearoff 1]
    $m3 add command -label "autoscale" \
    	-command { 
    	   global ozgrGraph
    	   set ozgrGraph(x,min) ""
    	   set ozgrGraph(x,max) ""
    	   set ozgrGraph(y,min) ""
    	   set ozgrGraph(y,max) ""
    	   RefreshGraph ozgrGraph
    	}
    $m3 add command -label "x-axis..." -command {
                                          global ozgrGraph
                                          set_xaxis_layout ozgrGraph
                                       }
    $m3 add command -label "y-axis..." -command {
                                          set_yaxis_layout ozgrGraph
                                       }
    $m3 add command -label "graph layout..." -command {
                                               set_graph_layout ozgrGraph
                                             }
    $m3 add command -label "plot layout..." -command {
                                               set_plot_layout ozgrGraph
                                            }
    .oztop.tab.gr.popup add command -label "Export Data..." \
    	-command { export_blt_graph ozgrGraph }
    bind .oztop.tab.gr.draw <ButtonPress-3> {tk_popup .oztop.tab.gr.popup %X %Y }
    bind .oztop.tab.gr.draw <Double-ButtonPress-1> {tk_popup .oztop.tab.gr.popup %X %Y }
    Blt_ZoomStack $ozgrGraph(w)
	bind .oztop.tab.gr.draw <Motion> \
        {
         set xcoord [expr [winfo pointerx $ozgrGraph(w)] \
                         -[winfo rootx    $ozgrGraph(w)] ] 
         set ycoord [expr [winfo pointery $ozgrGraph(w)] \
                         -[winfo rooty    $ozgrGraph(w)] ]

         set ozgrGraph(c,x) $xcoord
         set ozgrGraph(c,y) $ycoord
         
         $ozgrGraph(w) crosshairs configure -position @$ozgrGraph(c,x),$ozgrGraph(c,y)

         if { $ozgrGraph(e,element) > 0 } {
            set xycoord [$ozgrGraph(w) invtransform $xcoord $ycoord]
			switch $ozgrGraph(x,type) {
				"arcsinh(x)"  {
					set xc [expr sinh([lindex $xycoord 0]) ]
				}
				"pow(x,2)"	{
					set xc [expr pow([lindex $xycoord 0],1./2.) ]
				}
				"pow(x,3)" {
					set xc [expr pow([lindex $xycoord 0],1./3.) ]
				}
				"pow(x,4)" {
					set xc [expr pow([lindex $xycoord 0],1/4.) ]
				} 
				"log(x)" {
					set xc [expr exp([lindex $xycoord 0] ]
				}
				"sqrt(x)" {
					set xc [expr pow([lindex $xycoord 0],2) ]
				}
				"1/x" {
					set xc [expr 1./([lindex $xycoord 0]) ]
				}
				"1/sqrt(x)" {
					set xc [expr 1./pow([lindex $xycoord 0],2.) ]
				}
				default {
					set xc [lindex $xycoord 0]
				}
			}
			set xname cursor_x
			set yname cursor_y
			switch $ozgrGraph(y,type) {
				arcsinh(y)  {
					set yc [expr sinh([lindex $xycoord 1]) ]
				}
				pow(y,2)  {
					set yc [expr pow([lindex $xycoord 1],1./2.) ]
				}
				pow(y,3)  {
					set yc [expr pow([lindex $xycoord 1],1./3.) ]
				}
				pow(y,4)  {
					set yc [expr pow([lindex $xycoord 1],1./4.) ]
				}
				x*y {
					set yc [expr [lindex $xycoord 1]/$xc ]
				}
				y*pow(x,2) {
					set yc [expr [lindex $xycoord 1]/($xc*$xc) ]
				}
				y*pow(x,3) {
					set yc [expr [lindex $xycoord 1]/($xc*$xc*$xc) ]
				}
				y*pow(x,4) {
					set yc [expr [lindex $xycoord 1]/($xc*$xc*$xc*$xc) ]
				}
				y*pow(x,6) {
					set yc [expr [lindex $xycoord 1]/($xc*$xc*$xc*$xc*$xc*$xc) ]
				}
				y*pow(x,-2) {
					set yc [expr [lindex $xycoord 1]*($xc*$xc) ]
				}
				y*pow(x,-3) {
					set yc [expr [lindex $xycoord 1]*($xc*$xc*xc) ]
				}
				y*pow(x,-4) {
					set yc [expr [lindex $xycoord 1]*($xc*$xc*$xc*$xc ]
				}
				y*pow(x,-6) {
					set yc [expr [lindex $xycoord 1]*($xc*$xc*$xc*$xc*$xc*$xc) ]
				}
				1/y {
					set yc [expr 1./[lindex $xycoord 1] ]
				}
				log(abs(y))	 {
					set yc [expr exp([lindex $xycoord 1]) ]
				}
				log(abs(y*x)) {
					set yc [expr exp([lindex $xycoord 1])/$xc ]
				}
				log(abs(y*pow(x,2))) {
					set yc [expr exp([lindex $xycoord 1])/($xc*$xc) ]
				}
				log(abs(y*pow(x,4))) {
					set yc [expr exp([lindex $xycoord 1])/($xc*$xc*$xc*$xc) ]
				}
				sqrt(abs(y)) {
					set yc [expr pow([lindex $xycoord 1],2.0) ]
				}
				1/sqrt(abs(y))  {
					set yc [expr 1./pow([lindex $xycoord 1],2.0) ]
				}
				default {
					set yc [lindex $xycoord 1]
				}
			}
			set idx [bisect [lindex $OZ(result,r) 0] $xc]
			set CN [lindex [lindex $OZ(result,gr2) 0] $idx]
			if {[catch {
				set Dcc [expr [lindex [lindex $OZ(result,gr3) 0] $idx]/$CN]
			} msg] } {
				set Dcc 0.0
			}
			if {[catch {
				set Dcc2 [expr [lindex [lindex $OZ(result,gr4) 0] $idx]/$CN-$Dcc*$Dcc]
			} msg] } {
				set Dcc2 0.0
			}
            .oztop.xycoordinates configure \
                     -text "<D>=$Dcc +- [expr sqrt(abs($Dcc2))]\t\t($xname,$yname)=([fp $xc],[fp $yc])"
         }
        }
#
#  create "ozcrGraph"
#

    frame .oztop.tab.cr -relief groove -borderwidth 2 
    set ozcrGraph(w) .oztop.tab.cr.draw
    set ozcrGraph(e,element) 0
    CreateGraph ozcrGraph
    set ozcrGraph(x,logscale) 0
    set ozcrGraph(y,logscale) 0
    set ozcrGraph(x,type)     x
    set ozcrGraph(y,type)     y
    set ozcrGraph(x,title) "r / nm"
    set ozcrGraph(y,title) "c(r)"
    set ozcrGraph(l,hide)  no
    pack $ozcrGraph(w) -in .oztop.tab.cr
    pack configure $ozcrGraph(w) -fill both -expand yes

   .oztop.tab insert 2 ozcrGraph
   .oztop.tab tab configure ozcrGraph -text "direct\ncorrelation\nfunction c(r)"
   .oztop.tab tab configure ozcrGraph -fill both -padx 0.1i -pady 0.1i \
            -window .oztop.tab.cr -windowheight $sasfit(height) -windowwidth $sasfit(width)
            
##
## creating ozcr popup menu
##
    menu .oztop.tab.cr.popup -tearoff 0
    .oztop.tab.cr.popup add command -label "copy to clipboard (wmf)" -un 0 -command {
	window_to_clipboard .oztop.tab.cr
    }
    .oztop.tab.cr.popup add command -label "copy to clipboard (ascii)" -un 0 -command {
     	global ozcrGraph
     	export_clipboard_data ozcrGraph
    }
    .oztop.tab.cr.popup add cascade -label "Graph" -un 0 -menu .oztop.tab.cr.popup.graph
    set m3 [menu .oztop.tab.cr.popup.graph -tearoff 1]
    $m3 add command -label "autoscale" \
    	-command { 
    	   global ozcrGraph
    	   set ozcrGraph(x,min) ""
    	   set ozcrGraph(x,max) ""
    	   set ozcrGraph(y,min) ""
    	   set ozcrGraph(y,max) ""
    	   RefreshGraph ozcrGraph
    	}
    $m3 add command -label "x-axis..." -command {
                                          global ozcrGraph
                                          set_xaxis_layout ozcrGraph
                                       }
    $m3 add command -label "y-axis..." -command {
                                          set_yaxis_layout ozcrGraph
                                       }
    $m3 add command -label "graph layout..." -command {
                                               set_graph_layout ozcrGraph
                                             }
    $m3 add command -label "plot layout..." -command {
                                               set_plot_layout ozcrGraph
                                            }
    .oztop.tab.cr.popup add command -label "Export Data..." \
    	-command { export_blt_graph ozcrGraph }
    bind .oztop.tab.cr.draw <ButtonPress-3> {tk_popup .oztop.tab.cr.popup %X %Y }
    bind .oztop.tab.cr.draw <Double-ButtonPress-1> {tk_popup .oztop.tab.cr.popup %X %Y }
    Blt_ZoomStack $ozcrGraph(w)

#
#  create "ozgammarGraph"
#

    frame .oztop.tab.gammar -relief groove -borderwidth 2 
    set ozgammarGraph(w) .oztop.tab.gammar.draw
    set ozgammarGraph(e,element) 0
    CreateGraph ozgammarGraph
    set ozgammarGraph(x,logscale) 0
    set ozgammarGraph(y,logscale) 0
    set ozgammarGraph(x,type)     x
    set ozgammarGraph(y,type)     y
    set ozgammarGraph(x,title) "r / nm"
    set ozgammarGraph(y,title) "gamma(r)"
    set ozgammarGraph(l,hide)  no
    pack $ozgammarGraph(w) -in .oztop.tab.gammar
    pack configure $ozgammarGraph(w) -fill both -expand yes

   .oztop.tab insert 3 ozgammarGraph
   .oztop.tab tab configure ozgammarGraph -text "indirect\ncorrelation\nfunct. gamma(r)"
   .oztop.tab tab configure ozgammarGraph -fill both -padx 0.1i -pady 0.1i \
            -window .oztop.tab.gammar -windowheight $sasfit(height) -windowwidth $sasfit(width)
            
##
## creating ozgammar popup menu
##
    menu .oztop.tab.gammar.popup -tearoff 0
    .oztop.tab.gammar.popup add command -label "copy to clipboard (wmf)" -un 0 -command {
	window_to_clipboard .oztop.tab.gammar
    }
    .oztop.tab.gammar.popup add command -label "copy to clipboard (ascii)" -un 0 -command {
     	global ozgammarGraph
     	export_clipboard_data ozgammarGraph
    }
    .oztop.tab.gammar.popup add cascade -label "Graph" -un 0 -menu .oztop.tab.gammar.popup.graph
    set m3 [menu .oztop.tab.gammar.popup.graph -tearoff 1]
    $m3 add command -label "autoscale" \
    	-command { 
    	   global ozgammarGraph
    	   set ozgammarGraph(x,min) ""
    	   set ozgammarGraph(x,max) ""
    	   set ozgammarGraph(y,min) ""
    	   set ozgammarGraph(y,max) ""
    	   RefreshGraph ozgammarGraph
    	}
    $m3 add command -label "x-axis..." -command {
                                          global ozgammarGraph
                                          set_xaxis_layout ozgammarGraph
                                       }
    $m3 add command -label "y-axis..." -command {
                                          set_yaxis_layout ozgammarGraph
                                       }
    $m3 add command -label "graph layout..." -command {
                                               set_graph_layout ozgammarGraph
                                             }
    $m3 add command -label "plot layout..." -command {
                                               set_plot_layout ozgammarGraph
                                            }
    .oztop.tab.gammar.popup add command -label "Export Data..." \
    	-command { export_blt_graph ozgammarGraph }
    bind .oztop.tab.gammar.draw <ButtonPress-3> {tk_popup .oztop.tab.gammar.popup %X %Y }
    bind .oztop.tab.gammar.draw <Double-ButtonPress-1> {tk_popup .oztop.tab.gammar.popup %X %Y }
    Blt_ZoomStack $ozgammarGraph(w)

#
#  create "ozhrGraph"
#

    frame .oztop.tab.hr -relief groove -borderwidth 2 
    set ozhrGraph(w) .oztop.tab.hr.draw
    set ozhrGraph(e,element) 0
    CreateGraph ozhrGraph
    set ozhrGraph(x,logscale) 0
    set ozhrGraph(y,logscale) 0
    set ozhrGraph(x,type)     x
    set ozhrGraph(y,type)     y
    set ozhrGraph(x,title) "r / nm"
    set ozhrGraph(y,title) "h(r)"
    set ozhrGraph(l,hide)  no
    pack $ozhrGraph(w) -in .oztop.tab.hr
    pack configure $ozhrGraph(w) -fill both -expand yes

   .oztop.tab insert 3 ozhrGraph
   .oztop.tab tab configure ozhrGraph -text "total\ncorrelation\nfunct. h(r)"
   .oztop.tab tab configure ozhrGraph -fill both -padx 0.1i -pady 0.1i \
            -window .oztop.tab.hr -windowheight $sasfit(height) -windowwidth $sasfit(width)
            
##
## creating ozhr popup menu
##
    menu .oztop.tab.hr.popup -tearoff 0
    .oztop.tab.hr.popup add command -label "copy to clipboard (wmf)" -un 0 -command {
	window_to_clipboard .oztop.tab.hr
    }
    .oztop.tab.hr.popup add command -label "copy to clipboard (ascii)" -un 0 -command {
     	global ozhrGraph
     	export_clipboard_data ozhrGraph
    }
    .oztop.tab.hr.popup add cascade -label "Graph" -un 0 -menu .oztop.tab.hr.popup.graph
    set m3 [menu .oztop.tab.hr.popup.graph -tearoff 1]
    $m3 add command -label "autoscale" \
    	-command { 
    	   global ozhrGraph
    	   set ozhrGraph(x,min) ""
    	   set ozhrGraph(x,max) ""
    	   set ozhrGraph(y,min) ""
    	   set ozhrGraph(y,max) ""
    	   RefreshGraph ozhrGraph
    	}
    $m3 add command -label "x-axis..." -command {
                                          global ozhrGraph
                                          set_xaxis_layout ozhrGraph
                                       }
    $m3 add command -label "y-axis..." -command {
                                          set_yaxis_layout ozhrGraph
                                       }
    $m3 add command -label "graph layout..." -command {
                                               set_graph_layout ozhrGraph
                                             }
    $m3 add command -label "plot layout..." -command {
                                               set_plot_layout ozhrGraph
                                            }
    .oztop.tab.hr.popup add command -label "Export Data..." \
    	-command { export_blt_graph ozhrGraph }
    bind .oztop.tab.hr.draw <ButtonPress-3> {tk_popup .oztop.tab.hr.popup %X %Y }
    bind .oztop.tab.hr.draw <Double-ButtonPress-1> {tk_popup .oztop.tab.hr.popup %X %Y }
    Blt_ZoomStack $ozhrGraph(w)

#
#  create "ozbetaUrGraph"
#

    frame .oztop.tab.betaUr -relief groove -borderwidth 2 
    set ozbetaUrGraph(w) .oztop.tab.betaUr.draw
    set ozbetaUrGraph(e,element) 0
    CreateGraph ozbetaUrGraph
    set oznetaUrGraph(x,logscale) 0
    set ozbetaUrGraph(y,logscale) 0
    set ozbetaUrGraph(x,type)     x
    set ozbetaUrGraph(y,type)     y
    set ozbetaUrGraph(x,title) "r / nm"
    set ozbetaUrGraph(y,title) "U(r)/(kB*T)"
    set ozbetaUrGraph(l,hide)  no
    pack $ozbetaUrGraph(w) -in .oztop.tab.betaUr
    pack configure $ozbetaUrGraph(w) -fill both -expand yes

   .oztop.tab insert 4 ozbetaUrGraph
   .oztop.tab tab configure ozbetaUrGraph -text "interaction\npotential\nU(r)/(kB*T)"
   .oztop.tab tab configure ozbetaUrGraph -fill both -padx 0.1i -pady 0.1i \
            -window .oztop.tab.betaUr -windowheight $sasfit(height) -windowwidth $sasfit(width) 
 ##
 ## creating ozbetaUr popup menu
 ##
     menu .oztop.tab.betaUr.popup -tearoff 0
     .oztop.tab.betaUr.popup add command -label "copy to clipboard (wmf)" -un 0 -command {
 	window_to_clipboard .oztop.tab.betaUr
     }
     .oztop.tab.betaUr.popup add command -label "copy to clipboard (ascii)" -un 0 -command {
      	global ozbetaUrGraph
      	export_clipboard_data ozbetaUrGraph
     }
     .oztop.tab.betaUr.popup add cascade -label "Graph" -un 0 -menu .oztop.tab.betaUr.popup.graph
     set m3 [menu .oztop.tab.betaUr.popup.graph -tearoff 1]
     $m3 add command -label "autoscale" \
     	-command {
     	   global ozbetaUrGraph
     	   set ozbetaUrGraph(x,min) ""
     	   set ozbetaUrGraph(x,max) ""
     	   set ozbetaUrGraph(y,min) ""
     	   set ozbetaUrGraph(y,max) ""
     	   RefreshGraph ozbetaUrGraph
     	}
     $m3 add command -label "x-axis..." -command {
                                           global ozbetaUrGraph
                                           set_xaxis_layout ozbetaUrGraph
                                        }
     $m3 add command -label "y-axis..." -command {
                                           set_yaxis_layout ozbetaUrGraph
                                        }
     $m3 add command -label "graph layout..." -command {
                                                set_graph_layout ozbetaUrGraph
                                              }
     $m3 add command -label "plot layout..." -command {
                                                set_plot_layout ozbetaUrGraph
                                             }
     .oztop.tab.betaUr.popup add command -label "Export Data..." \
     	-command { export_blt_graph ozbetaUrGraph }
     bind .oztop.tab.betaUr.draw <ButtonPress-3> {tk_popup .oztop.tab.betaUr.popup %X %Y }
     bind .oztop.tab.betaUr.draw <Double-ButtonPress-1> {tk_popup .oztop.tab.betaUr.popup %X %Y }
   
    Blt_ZoomStack $ozbetaUrGraph(w)

#
#  create "ozfrGraph"
#

    frame .oztop.tab.fr -relief groove -borderwidth 2 
    set ozfrGraph(w) .oztop.tab.fr.draw
    set ozfrGraph(e,element) 0
    CreateGraph ozfrGraph
    set ozfrGraph(x,logscale) 0
    set ozfrGraph(y,logscale) 0
    set ozfrGraph(x,type)     x
    set ozfrGraph(y,type)     y
    set ozfrGraph(x,title) "r / nm"
    set ozfrGraph(y,title) "f(r)"
    set ozfrGraph(l,hide)  no
    pack $ozfrGraph(w) -in .oztop.tab.fr
    pack configure $ozfrGraph(w) -fill both -expand yes

   .oztop.tab insert 5 ozfrGraph
   .oztop.tab tab configure ozfrGraph -text "Mayer-f\nfunction\nf(r)"
   .oztop.tab tab configure ozfrGraph -fill both -padx 0.1i -pady 0.1i \
            -window .oztop.tab.fr -windowheight $sasfit(height) -windowwidth $sasfit(width) 
 ##
 ## creating ozfr popup menu
 ##
     menu .oztop.tab.fr.popup -tearoff 0
     .oztop.tab.fr.popup add command -label "copy to clipboard (wmf)" -un 0 -command {
 	window_to_clipboard .oztop.tab.fr
     }
     .oztop.tab.fr.popup add command -label "copy to clipboard (ascii)" -un 0 -command {
      	global ozfrGraph
      	export_clipboard_data ozfrGraph
     }
     .oztop.tab.fr.popup add cascade -label "Graph" -un 0 -menu .oztop.tab.fr.popup.graph
     set m3 [menu .oztop.tab.fr.popup.graph -tearoff 1]
     $m3 add command -label "autoscale" \
     	-command {
     	   global ozfrGraph
     	   set ozfrGraph(x,min) ""
     	   set ozfrGraph(x,max) ""
     	   set ozfrGraph(y,min) ""
     	   set ozfrGraph(y,max) ""
     	   RefreshGraph ozfrGraph
     	}
     $m3 add command -label "x-axis..." -command {
                                           global ozfrGraph
                                           set_xaxis_layout ozfrGraph
                                        }
     $m3 add command -label "y-axis..." -command {
                                           set_yaxis_layout ozfrGraph
                                        }
     $m3 add command -label "graph layout..." -command {
                                                set_graph_layout ozfrGraph
                                              }
     $m3 add command -label "plot layout..." -command {
                                                set_plot_layout ozfrGraph
                                             }
     .oztop.tab.fr.popup add command -label "Export Data..." \
     	-command { export_blt_graph ozfrGraph }
     bind .oztop.tab.fr.draw <ButtonPress-3> {tk_popup .oztop.tab.fr.popup %X %Y }
     bind .oztop.tab.fr.draw <Double-ButtonPress-1> {tk_popup .oztop.tab.fr.popup %X %Y }
   
    Blt_ZoomStack $ozfrGraph(w)


#
#  create "ozBrGraph"
#

    frame .oztop.tab.br -relief groove -borderwidth 2 
    set ozBrGraph(w) .oztop.tab.br.draw
    set ozBrGraph(e,element) 0
    CreateGraph ozBrGraph
    set ozBrGraph(x,logscale) 0
    set ozBrGraph(y,logscale) 0
    set ozBrGraph(x,type)     x
    set ozBrGraph(y,type)     y
    set ozBrGraph(x,title) "r / nm"
    set ozBrGraph(y,title) "B(r)"
    set ozBrGraph(l,hide)  no
    pack $ozBrGraph(w) -in .oztop.tab.br
    pack configure $ozBrGraph(w) -fill both -expand yes

   .oztop.tab insert 6 ozBrGraph
   .oztop.tab tab configure ozBrGraph -text "bridge\nfunction\nB(r)"
   .oztop.tab tab configure ozBrGraph -fill both -padx 0.1i -pady 0.1i \
            -window .oztop.tab.br -windowheight $sasfit(height) -windowwidth $sasfit(width) 
 ##
 ## creating ozBr popup menu
 ##
     menu .oztop.tab.br.popup -tearoff 0
     .oztop.tab.br.popup add command -label "copy to clipboard (wmf)" -un 0 -command {
 	window_to_clipboard .oztop.tab.br
     }
     .oztop.tab.br.popup add command -label "copy to clipboard (ascii)" -un 0 -command {
      	global ozBrGraph
      	export_clipboard_data ozBrGraph
     }
     .oztop.tab.br.popup add cascade -label "Graph" -un 0 -menu .oztop.tab.br.popup.graph
     set m3 [menu .oztop.tab.br.popup.graph -tearoff 1]
     $m3 add command -label "autoscale" \
     	-command {
     	   global ozBrGraph
     	   set ozBrGraph(x,min) ""
     	   set ozBrGraph(x,max) ""
     	   set ozBrGraph(y,min) ""
     	   set ozBrGraph(y,max) ""
     	   RefreshGraph ozBrGraph
     	}
     $m3 add command -label "x-axis..." -command {
                                           global ozBrGraph
                                           set_xaxis_layout ozBrGraph
                                        }
     $m3 add command -label "y-axis..." -command {
                                           set_yaxis_layout ozBrGraph
                                        }
     $m3 add command -label "graph layout..." -command {
                                                set_graph_layout ozBrGraph
                                              }
     $m3 add command -label "plot layout..." -command {
                                                set_plot_layout ozBrGraph
                                             }
     .oztop.tab.br.popup add command -label "Export Data..." \
     	-command { export_blt_graph ozBrGraph }
     bind .oztop.tab.br.draw <ButtonPress-3> {tk_popup .oztop.tab.br.popup %X %Y }
     bind .oztop.tab.br.draw <Double-ButtonPress-1> {tk_popup .oztop.tab.br.popup %X %Y }
   
    Blt_ZoomStack $ozBrGraph(w)

#
#  create "ozyrGraph"
#

    frame .oztop.tab.yr -relief groove -borderwidth 2 
    set ozyrGraph(w) .oztop.tab.yr.draw
    set ozyrGraph(e,element) 0
    CreateGraph ozyrGraph
    set ozyrGraph(x,logscale) 0
    set ozyrGraph(y,logscale) 0
    set ozyrGraph(x,type)     x
    set ozyrGraph(y,type)     y
    set ozyrGraph(x,title) "r / nm"
    set ozyrGraph(y,title) "y(r)"
    set ozyrGraph(l,hide)  no
    pack $ozyrGraph(w) -in .oztop.tab.yr
    pack configure $ozyrGraph(w) -fill both -expand yes

   .oztop.tab insert 7 ozyrGraph
   .oztop.tab tab configure ozyrGraph -text "cavity\nfunction\ny(r)"
   .oztop.tab tab configure ozyrGraph -fill both -padx 0.1i -pady 0.1i \
            -window .oztop.tab.yr -windowheight $sasfit(height) -windowwidth $sasfit(width) 
 ##
 ## creating ozyr popup menu
 ##
     menu .oztop.tab.yr.popup -tearoff 0
     .oztop.tab.yr.popup add command -label "copy to clipboard (wmf)" -un 0 -command {
 	window_to_clipboard .oztop.tab.yr
     }
     .oztop.tab.yr.popup add command -label "copy to clipboard (ascii)" -un 0 -command {
      	global ozyrGraph
      	export_clipboard_data ozyrGraph
     }
     .oztop.tab.yr.popup add cascade -label "Graph" -un 0 -menu .oztop.tab.yr.popup.graph
     set m3 [menu .oztop.tab.yr.popup.graph -tearoff 1]
     $m3 add command -label "autoscale" \
     	-command {
     	   global ozyrGraph
     	   set ozyrGraph(x,min) ""
     	   set ozyrGraph(x,max) ""
     	   set ozyrGraph(y,min) ""
     	   set ozyrGraph(y,max) ""
     	   RefreshGraph ozyrGraph
     	}
     $m3 add command -label "x-axis..." -command {
                                           global ozyrGraph
                                           set_xaxis_layout ozyrGraph
                                        }
     $m3 add command -label "y-axis..." -command {
                                           set_yaxis_layout ozyrGraph
                                        }
     $m3 add command -label "graph layout..." -command {
                                                set_graph_layout ozyrGraph
                                              }
     $m3 add command -label "plot layout..." -command {
                                                set_plot_layout ozyrGraph
                                             }
     .oztop.tab.yr.popup add command -label "Export Data..." \
     	-command { export_blt_graph ozyrGraph }
     bind .oztop.tab.yr.draw <ButtonPress-3> {tk_popup .oztop.tab.yr.popup %X %Y }
     bind .oztop.tab.yr.draw <Double-ButtonPress-1> {tk_popup .oztop.tab.yr.popup %X %Y }
   
    Blt_ZoomStack $ozyrGraph(w)


    oz_input_names
    ReplotOZsolver
    RefreshGraph ozSQGraph
    RefreshGraph ozgrGraph
    RefreshGraph ozcrGraph
    RefreshGraph ozhrGraph
    RefreshGraph ozgammarGraph
    RefreshGraph ozbetaUrGraph
    RefreshGraph ozBrGraph
    RefreshGraph ozyrGraph
    RefreshGraph ozfrGraph
}
