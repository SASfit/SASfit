# sasfit.vfs/lib/app-sasfit/tcl/sasfit_OZ_solver.tcl
#
# Copyright (c) 2008-2013, Paul Scherrer Institute (PSI)
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

proc StartOZsolver {} {
        global OZ ozSQGraph ozgrGraph ozcrGraph ozbetaUrGraph sasfit
        puts "Start OZ Solver"
        sasfit_oz_calc OZ

        if {$OZ(color_i) >= [llength $ozSQGraph(colorselection)]} {
            set OZ(color_i) 0
            incr OZ(symbol_i)
            if {$OZ(symbol_i) >= [llength $OZSQGraph(symbolselection)]} {set $OZ(symbol_i) 0}
        }
        set color_n  [lindex $ozSQGraph(colorselection)  $OZ(color_i)]
        set symbol_n [lindex $ozSQGraph(symbolselection) $OZ(symbol_i)]
        set count_n  $OZ(plottedgraphs)

        incr OZ(color_i)
        incr OZ(symbol_i)

        #clearGraph_el ozSQGraph
        set ozSQGraph(x,type) arcsinh(x)
        Put_Graph_el ozSQGraph $OZ(res,s,x) $OZ(res,s,y)
        set ozSQGraph(e,linehide)  [lreplace $ozSQGraph(e,linehide)  $count_n  $count_n 1]
        set ozSQGraph(e,dashcolor) [lreplace $ozSQGraph(e,dashcolor) $count_n  $count_n $color_n]
        set ozSQGraph(e,fill)      [lreplace $ozSQGraph(e,fill)      $count_n  $count_n $color_n]
        set ozSQGraph(e,outline)   [lreplace $ozSQGraph(e,outline)   $count_n  $count_n $color_n]
        set ozSQGraph(e,symbol)    [lreplace $ozSQGraph(e,symbol)    $count_n  $count_n $symbol_n]
        RefreshGraph ozSQGraph

        #clearGraph_el ozcrGraph 
        set ozcrGraph(x,type) arcsinh(x)
        Put_Graph_el ozcrGraph $OZ(res,c,x) $OZ(res,c,y)
        set ozcrGraph(e,linehide)  [lreplace $ozcrGraph(e,linehide)  $count_n  $count_n 1]
        set ozcrGraph(e,dashcolor) [lreplace $ozcrGraph(e,dashcolor) $count_n  $count_n $color_n]
        set ozcrGraph(e,fill)      [lreplace $ozcrGraph(e,fill)      $count_n  $count_n $color_n]
        set ozcrGraph(e,outline)   [lreplace $ozcrGraph(e,outline)   $count_n  $count_n $color_n]
        set ozcrGraph(e,symbol)    [lreplace $ozcrGraph(e,symbol)    $count_n  $count_n $symbol_n]
        RefreshGraph ozcrGraph

        #clearGraph_el ozgrGraph 
        set ozgrGraph(x,type) arcsinh(x)
        Put_Graph_el ozgrGraph $OZ(res,g,x) $OZ(res,g,y)
        set ozgrGraph(e,linehide)  [lreplace $ozgrGraph(e,linehide)  $count_n  $count_n 1]
        set ozgrGraph(e,dashcolor) [lreplace $ozgrGraph(e,dashcolor) $count_n  $count_n $color_n]
        set ozgrGraph(e,fill)      [lreplace $ozgrGraph(e,fill)      $count_n  $count_n $color_n]
        set ozgrGraph(e,outline)   [lreplace $ozgrGraph(e,outline)   $count_n  $count_n $color_n]
        set ozgrGraph(e,symbol)    [lreplace $ozgrGraph(e,symbol)    $count_n  $count_n $symbol_n]
        RefreshGraph ozgrGraph

        #clearGraph_el ozbetaUrGraph 
        set ozbetaUrGraph(x,type) arcsinh(x)
        Put_Graph_el ozbetaUrGraph $OZ(res,u,x) $OZ(res,u,y)
        set ozbetaUrGraph(e,linehide)  [lreplace $ozbetaUrGraph(e,linehide)  $count_n  $count_n 1]
        set ozbetaUrGraph(e,dashcolor) [lreplace $ozbetaUrGraph(e,dashcolor) $count_n  $count_n $color_n]
        set ozbetaUrGraph(e,fill)      [lreplace $ozbetaUrGraph(e,fill)      $count_n  $count_n $color_n]
        set ozbetaUrGraph(e,outline)   [lreplace $ozbetaUrGraph(e,outline)   $count_n  $count_n $color_n]
        set ozbetaUrGraph(e,symbol)    [lreplace $ozbetaUrGraph(e,symbol)    $count_n  $count_n $symbol_n]
        RefreshGraph ozbetaUrGraph

        incr  OZ(plottedgraphs)
}

proc ClearOZsolver {} {
    global ozSQGraph ozcrGraph ozgrGraph ozbetaUrGraph OZ
    clearGraph_el ozSQGraph
    clearGraph_el ozcrGraph 
    clearGraph_el ozgrGraph 
    clearGraph_el ozbetaUrGraph 
    set OZ(color_i) 0
    set OZ(symbol_i) 0
    set OZ(plottedgraphs) 0
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
			}
		StickyHardSphere {
			set OZ(p0,name) diameter
			set OZ(p1,name) tau
			set OZ(p2,name) delta
			set OZ(p3,name) ""
			set OZ(p4,name) ""
			set OZ(p5,name) ""
			}
		SoftSphere {
			set OZ(p0,name) diameter
			set OZ(p1,name) epsilon
			set OZ(p2,name) ""
			set OZ(p3,name) ""
			set OZ(p4,name) ""
			set OZ(p5,name) ""
			}
		LennardJones {
			set OZ(p0,name) diameter
			set OZ(p1,name) epsilon
			set OZ(p2,name) ""
			set OZ(p3,name) ""
			set OZ(p4,name) ""
			set OZ(p5,name) ""
			}
		Depletion {
			set OZ(p0,name) diameter
			set OZ(p1,name) "diam. (small)"
			set OZ(p2,name) "phi (small)"
			set OZ(p3,name) ""
			set OZ(p4,name) ""
			set OZ(p5,name) ""
			}
		IonicMicrogel {
			set OZ(p0,name) diameter
			set OZ(p1,name) Z
			set OZ(p2,name) ed
			set OZ(p3,name) kpi
			set OZ(p4,name) epsilon
			set OZ(p5,name) ""
			}
		PenetrableSphere {
			set OZ(p0,name) diameter
			set OZ(p1,name) epsilon
			set OZ(p2,name) ""
			set OZ(p3,name) ""
			set OZ(p4,name) ""
			set OZ(p5,name) ""
			}
		DLVO {
			set OZ(p0,name) diameter
			set OZ(p1,name) kappa
			set OZ(p2,name) Z
			set OZ(p3,name) LB
			set OZ(p4,name) ""
			set OZ(p5,name) ""
			}
		"GGCM-n" {
			set OZ(p0,name) diameter
			set OZ(p1,name) epsilon
			set OZ(p2,name) n
			set OZ(p3,name) alpha
			set OZ(p4,name) ""
			set OZ(p5,name) ""
			}
		default {
			set OZ(p0,name) ""
			set OZ(p1,name) ""
			set OZ(p2,name) ""
			set OZ(p3,name) ""
			set OZ(p4,name) ""
			set OZ(p5,name) ""
			}
	}
}

proc sasfit_OZ_solver {} {
    global sasfit ozSQGraph ozgrGraph ozcrGraph ozbetaUrGraph OZ
    set w .oztop
    if {[winfo exists $w]} {destroy $w}
    toplevel .oztop
    wm geometry $w
    wm title .oztop "Ornstein Zernike solver"
    raise $w

#------------------------------------------------------------------------------
#                               Create OZ tabs

#
#  creating tabset
#
    blt::tabset .oztop.tab -relief sunken -borderwidth 2
    frame $w.interface 
    frame $w.interface.param   
    frame $w.interface.action 
    pack .oztop.tab $w.interface -fill both -expand yes -side right
    pack  $w.interface.param  $w.interface.action  -fill both -expand yes
    
    button  $w.interface.action.calc -text calculate -command {StartOZsolver}
    button  $w.interface.action.clear -text clear -command {ClearOZsolver}
    pack $w.interface.action.calc $w.interface.action.clear -side left -fill x -expand yes
 
    set w .oztop.interface
    label $w.param.cltext -text "closure relation:"
    label $w.param.pottext -text "potential:"
    grid $w.param.cltext  -sticky e\
	    -column 0 -row 0
    grid $w.param.pottext  -sticky e\
	    -column 0 -row 1
    ComboBox $w.param.clvalue \
	    -values {PY HNC RY Verlet MS BPGG} \
	    -textvariable OZ(closure)
    grid  $w.param.clvalue\
	    -column 1 -row 0
    ComboBox $w.param.potvalue \
	    -values {"HardSphere" "StickyHardSphere" "SoftSphere" "StarPolymer" \
	    		"LennardJones" "Depletion" "IonicMicrogel"  \
	    		"PenetrableSphere" "DLVO" "GGCM-n"} \
	    -textvariable OZ(potential) \
	    -modifycmd {oz_input_names}
    grid  $w.param.potvalue\
	    -column 1 -row 1

    label $w.param.v0text -anchor e -justify right -textvariable OZ(p0,name)
    label $w.param.v1text -anchor e -justify right -textvariable OZ(p1,name)
    label $w.param.v2text -anchor e -justify right -textvariable OZ(p2,name)
    label $w.param.v3text -anchor e -justify right -textvariable OZ(p3,name)
    label $w.param.v4text -anchor e -justify right -textvariable OZ(p4,name)
    label $w.param.v5text -anchor e -justify right -textvariable OZ(p5,name)
    grid  $w.param.v0text -sticky e\
	    -column 0 -row 2  
    grid  $w.param.v1text -sticky e\
	    -column 0 -row 3  
    grid  $w.param.v2text -sticky e\
	    -column 0 -row 4  
    grid  $w.param.v3text -sticky e\
	    -column 0 -row 5  
    grid  $w.param.v4text -sticky e\
	    -column 0 -row 6  
    grid  $w.param.v5text -sticky e\
	    -column 0 -row 7

    entry $w.param.v0value -textvariable OZ(p0)
    entry $w.param.v1value -textvariable OZ(p1)
    entry $w.param.v2value -textvariable OZ(p2)
    entry $w.param.v3value -textvariable OZ(p3)
    entry $w.param.v4value -textvariable OZ(p4)
    entry $w.param.v5value -textvariable OZ(p5)  
    grid  $w.param.v0value\
	    -column 1 -row 2  
    grid  $w.param.v1value\
	    -column 1 -row 3  
    grid  $w.param.v2value\
	    -column 1 -row 4  
    grid  $w.param.v3value\
	    -column 1 -row 5  
    grid  $w.param.v4value\
	    -column 1 -row 6  
    grid  $w.param.v5value\
	    -column 1 -row 7

    label $w.param.phitext -text "volume fraction:"  
    entry $w.param.phivalue -textvariable OZ(phi)
    label $w.param.ttext -text "temperature \[K\]:"  
    entry $w.param.tvalue -textvariable OZ(T)
    grid  $w.param.phitext -sticky e\
	    -column 0 -row 8
    grid  $w.param.ttext -sticky e\
	    -column 0 -row 9
    grid  $w.param.phivalue\
	    -column 1 -row 8
    grid  $w.param.tvalue\
	    -column 1 -row 9

    label $w.param.gridtext -text "gridsize (n x 1024), n:"  
    entry $w.param.gridvalue -textvariable OZ(mult)
    label $w.param.mixtext -text "mixing parameter:"  
    entry $w.param.mixvalue -textvariable OZ(mix)
    label $w.param.ittext -text "max iterations:"  
    entry $w.param.itvalue -textvariable OZ(maxit)
    label $w.param.relepstext -text "rel. iteration precision:"  
    entry $w.param.relepsvalue -textvariable OZ(releps) 
    label $w.param.drdsigmatext -text "rel. grid step width:" 
    entry $w.param.drdsigmavalue -textvariable OZ(dr/dsigma)
    grid  $w.param.gridtext -sticky e\
	    -column 0 -row 10
    grid  $w.param.mixtext -sticky e\
	    -column 0 -row 11
    grid  $w.param.ittext -sticky e\
	    -column 0 -row 12
    grid  $w.param.relepstext -sticky e\
	    -column 0 -row 13
    grid  $w.param.drdsigmatext -sticky e\
	    -column 0 -row 14
    grid  $w.param.gridvalue \
	    -column 1 -row 10
    grid  $w.param.mixvalue \
	    -column 1 -row 11
    grid  $w.param.itvalue \
	    -column 1 -row 12
    grid  $w.param.relepsvalue \
	    -column 1 -row 13
    grid  $w.param.drdsigmavalue \
	    -column 1 -row 14
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
    pack $ozSQGraph(w) -in .oztop.tab.sq
    pack configure $ozSQGraph(w) -fill both -expand yes

   .oztop.tab insert 0 ozSQGraph
   .oztop.tab tab configure ozSQGraph -text "structure factor\nS(Q)"
   .oztop.tab tab configure ozSQGraph -fill both -padx 0.1i -pady 0.1i \
            -window .oztop.tab.sq -windowheight $sasfit(height) -windowwidth $sasfit(width) 
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
    set ozgrGraph(y,title) "g(r))"
    pack $ozgrGraph(w) -in .oztop.tab.gr
    pack configure $ozgrGraph(w) -fill both -expand yes

   .oztop.tab insert 1 ozgrGraph
   .oztop.tab tab configure ozgrGraph -text "radial distribution\nfunction g(r)"
   .oztop.tab tab configure ozgrGraph -fill both -padx 0.1i -pady 0.1i \
            -window .oztop.tab.gr -windowheight $sasfit(height) -windowwidth $sasfit(width)  
    Blt_ZoomStack $ozgrGraph(w)

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
    pack $ozcrGraph(w) -in .oztop.tab.cr
    pack configure $ozcrGraph(w) -fill both -expand yes

   .oztop.tab insert 2 ozcrGraph
   .oztop.tab tab configure ozcrGraph -text "direct correlation\nfunction c(r)"
   .oztop.tab tab configure ozcrGraph -fill both -padx 0.1i -pady 0.1i \
            -window .oztop.tab.cr -windowheight $sasfit(height) -windowwidth $sasfit(width) 
    Blt_ZoomStack $ozcrGraph(w)

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
    pack $ozbetaUrGraph(w) -in .oztop.tab.betaUr
    pack configure $ozbetaUrGraph(w) -fill both -expand yes

   .oztop.tab insert 3 ozbetaUrGraph
   .oztop.tab tab configure ozbetaUrGraph -text "interaction potential\nU(r)/(kB*T)"
   .oztop.tab tab configure ozbetaUrGraph -fill both -padx 0.1i -pady 0.1i \
            -window .oztop.tab.betaUr -windowheight $sasfit(height) -windowwidth $sasfit(width) 
    Blt_ZoomStack $ozbetaUrGraph(w)

    RefreshGraph ozSQGraph
    RefreshGraph ozgrGraph
    RefreshGraph ozcrGraph
    RefreshGraph ozbetaUrGraph
}
