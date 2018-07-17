# sasfit.vfs/lib/app-sasfit/tcl/sasfit_plotData.tcl
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

proc Del_Square_el {GraphPar} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
	global sasfit addsasfit
	upvar $GraphPar Par
	catch "$Par(w) marker delete fitrange"
	if {[string compare $GraphPar IQGraph] == 0} {
		set sasfit(lower,Q) ""
		set sasfit(upper,Q) ""
	} elseif {[string compare $GraphPar GlobalFitIQGraph] == 0} {
		set addsasfit(lower,Q) ""
		set addsasfit(upper,Q) ""
	} else {
		set sasfit(lower,Q) ""
		set sasfit(upper,Q) ""
	}
	set Par(lower,x) ""
	set Par(upper,x) "" 
	set Par(fitrange) no
}

proc Make_Square_el {GraphPar ulx lrx} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
	upvar $GraphPar Par
	catch {$Par(w) marker delete fitrange}
	set xtrans $Par(x,type)
	if {[string compare $Par(x,type) "arcsinh(x)"] == 0 } {
		set xtrans "log(x+hypot(x,1))"
	}
	if {[string compare $Par(x,type) "log10(x)"] == 0 } {
		set xtrans "x"
	}
	regsub -all "x" $xtrans "\$x" xtrans
	set x $ulx
	set ulx [expr $xtrans]
	set x $lrx
	set lrx [expr $xtrans]
	set range [list $ulx -Inf $lrx -Inf $lrx Inf $ulx Inf]
	$Par(w) marker create polygon -name fitrange
	$Par(w) marker configure fitrange \
        	-coords $range -fill $Par(fit,background) -linewidth 0 -under yes
	$Par(w) marker after fitrange 
	set Par(fitrange) yes
}

#------------------------------------------------------------------------------
#   Removes the last curve from the plot "GraphPar
#
proc Pop_Graph_el {GraphPar} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
	upvar $GraphPar Par
	if { $Par(e,element) == 0 } {return 0}
	#
	# deleting tags of error bars
	#
	set last_el [incr Par(e,element) -1]
	foreach i [lindex $Par(e,errtags) $last_el] { catch "$Par(w) marker delete $i" }
	#
	# deleting last curve
	#
	catch "$Par(w) element delete [lindex $Par(e,elementname) $Par(e,element)]"
	#
	# deleting all element options corresponding to the curve to be deleted
	#
	incr last_el -1
	set Par(e,elementname)  [lrange $Par(e,elementname)  0 $last_el]

	set Par(e,linewidth)    [lrange $Par(e,linewidth)    0 $last_el]
	set Par(e,linehide)     [lrange $Par(e,linehide)     0 $last_el]
	set Par(e,dash)         [lrange $Par(e,dash)         0 $last_el]
	set Par(e,dashcolor)    [lrange $Par(e,dashcolor)    0 $last_el]
	set Par(e,offdashcolor) [lrange $Par(e,offdashcolor) 0 $last_el]
	set Par(e,smooth)       [lrange $Par(e,smooth)       0 $last_el] 

	set Par(e,symbol)       [lrange $Par(e,symbol)       0 $last_el]
	set Par(e,scale)        [lrange $Par(e,scale)        0 $last_el]
	set Par(e,pixels)       [lrange $Par(e,pixels)       0 $last_el]
	set Par(e,symbolhide)   [lrange $Par(e,symbolhide)   0 $last_el]
	set Par(e,fill)         [lrange $Par(e,fill)         0 $last_el]
	set Par(e,outline)      [lrange $Par(e,outline)      0 $last_el]
	set Par(e,outlinewidth) [lrange $Par(e,outlinewidth) 0 $last_el]

	set Par(e,errorhide)    [lrange $Par(e,errorhide)    0 $last_el]
	set Par(e,errlinewidth) [lrange $Par(e,errlinewidth) 0 $last_el]
	set Par(e,errdash)      [lrange $Par(e,errdash)      0 $last_el]
	set Par(e.errstipples)  [lrange $Par(e,errstipples)  0 $last_el]
	set Par(e,errfill)      [lrange $Par(e,errfill)      0 $last_el]
	set Par(e,erroutline)   [lrange $Par(e,erroutline)   0 $last_el]
	set Par(e,errtags)      [lrange $Par(e,errtags)      0 $last_el]
	
	set Par(e,xdata)        [lrange $Par(e,xdata)        0 $last_el]
	set Par(e,ydata)        [lrange $Par(e,ydata)        0 $last_el]
	set Par(e,resdata)      [lrange $Par(e,resdata)      0 $last_el]
	set Par(e,error)        [lrange $Par(e,error)        0 $last_el]

	set Par(e,reshide)      [lrange $Par(e,reshide)      0 $last_el]
	set Par(e,reslinewidth) [lrange $Par(e,reslinewidth) 0 $last_el]
	set Par(e,resdash)      [lrange $Par(e,resdash)      0 $last_el]
	set Par(e.resstipples)  [lrange $Par(e,resstipples)  0 $last_el]
	set Par(e,resfill)      [lrange $Par(e,resfill)      0 $last_el]
	set Par(e,resoutline)   [lrange $Par(e,resoutline)   0 $last_el]
	set Par(e,restags)      [lrange $Par(e,restags)      0 $last_el]
	
	set Par(l,legendtext)   [lrange $Par(l,legendtext)   0 $last_el]
	return 1
}


#------------------------------------------------------------------------------
#   Removes all curves from the plot "GraphPar
#
proc clearGraph_el {GraphPar} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
	upvar $GraphPar Par
	while {[Pop_Graph_el Par]} {}
	set par(fitrange) no
}


#------------------------------------------------------------------------------
#        Adds a new curve to GraphPar (GraphPar = e.g. IQGraph SDGraph ...)
#        procedure returns 0 if Put_Graph_el fails otherwise 1
#        after succesfull call of the procedure GraphPar is updated
#
proc Put_Graph_el {GraphPar xdata args } {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
	upvar $GraphPar Par
	if {[llength $args] > 3} {return 0}
	# $args may consist of up to 3 lists: '{ydata dydata resdata}'
	# $args is printed over $xdata
	set ydata   {}
	set dydata  {}
	set resdata {}
	set data    {ydata dydata resdata}
	set j 0

	# cycle through each list in $args and assign it to ydata, dydata and resdata
	foreach i $args {
		if { [llength $xdata] != [llength $i] } {
			return 0
		}
		set [lindex $data $j] $i
		incr j
	}

	# ensure graph datastructure is valid
	if { ![info exists Par]    } { return 0 }
	# ensure we have a window path
	if { ![info exists Par(w)] } { return 0 }

	#
	# if ydata not set than xdata becomes ydata, xdata becomes {1 2 3 ...}
	# and dydata becomes {-1.0 -1.0 ...} and resdata {}
	if { [llength $ydata] == 0 } {
		set ydata $xdata
		set xdata {}
		for {set i 1} {$i <= [llength $ydata]} {incr i} {
			lappend xdata $i
		}
	}
	if { [llength $dydata] == 0 } {
		set dydata {}
		for {set i 1} {$i <= [llength $ydata]} {incr i} {
			lappend dydata -1.0
		}
	}
	if { [llength $resdata] == 0 } {
		set resdata {}
		for {set i 1} {$i <= [llength $ydata]} {incr i} {
			lappend resdata 0.0
		}
	}
#
# check if all entries are numbers
#

	set tx {}
	set ty {}
	set tdy {}
	set tr {}
	foreach x $xdata y $ydata dy $dydata r $resdata {
	    if {[catch {expr $x}]==0 &&
                [catch {expr $y}]==0 &&
                [catch {expr $dy}]==0 &&
                [catch {expr $r}]==0 
	       } {
		   lappend tx $x
		   lappend ty $y
		   lappend tdy $dy
		   lappend tr $r
	       } else {
# puts "$x $y"
	       }
	}
	set xdata $tx
	set ydata $ty
	set dydata $tdy
	set resdata $tr

#
# multiplying with prefactor
#
	set Par(x,factor) [expr abs($Par(x,factor))]
	set Par(y,factor) [expr abs($Par(y,factor))]
	if {$Par(y,factor) != 1.0} {
	    set tmpdata {}
	    set tmpddata {}
	    foreach y $ydata e $dydata {
                if {$Par(y,factor) == 0} {
		    set factor 1.0
		} else {
		    set factor $Par(y,factor)
		}
		lappend tmpdata  [expr $y*$factor]
                if {$e != -1.0} {
                    # don't scale the uncertainty placeholder value
		    lappend tmpddata [expr $e*$factor]
                }
	    }
	    set ydata  $tmpdata
	    set dydata $tmpddata
	}
	if {$Par(x,factor) != 1.0} {
	    set tmpdata {}
	    set tmprdata {}
	    foreach x $xdata r $resdata {
                if {$Par(x,factor) == 0} {
		    set factor 1.0
		} else {
		    set factor $Par(x,factor)
		}
		lappend tmpdata  [expr $x*$factor]
		lappend tmprdata [expr $r*$factor]
	    }
	    set xdata   $tmpdata
	    set resdata $tmprdata
	}
#
# all data which are too small will be set to zero
#
	set tmpydata $ydata
	set ydata {}
	foreach y $tmpydata {
	    if {[catch {expr 0+$y}]} {
		lappend ydata 0.0
	    } else {
		lappend ydata $y
	    }
	}

	#
	# sorting {xdata ydata dydata} by increasing values of xdata
	#
	set ind [lsort_index $xdata]
	set xdata [lmindex $xdata $ind]
	set ydata [lmindex $ydata $ind]
	if { [llength $dydata] == [llength $ind] } {
		set dydata [lmindex $dydata $ind]
	}
	if { [llength $resdata] == [llength $ind] } {
		set resdata [lmindex $resdata $ind]
	}

	# 
	# create new element for displaying data
	# options are set to default values (see procedure CreateGraphPar)
	#
	CreateGraphPar tmpGP
	if { $Par(e,element) == 0 } { ;# neccessary to get a correct list
		set Par(e,elementname)  {}
		set Par(e,linewidth)    {}
		set Par(e,linehide)     {}
		set Par(e,dash)         {}
		set Par(e,dashcolor)    {}
		set Par(e,offdashcolor) {}
		set Par(e,smooth)       {}

		set Par(e,symbol)       {}
		set Par(e,scale)        {}
		set Par(e,pixels)       {}
		set Par(e,fill)         {}
		set Par(e,outline)      {}
		set Par(e,outlinewidth) {}
		set Par(e,symbolhide)   {}

		set Par(e,errorhide)    {}
		set Par(e,errlinewidth) {}
		set Par(e,errfill)      {}
		set Par(e,erroutline)   {}
		set Par(e,errdash)      {}
		set Par(e,errstipples)  {}
		
		set Par(e,reshide)      {}
		set Par(e,reslinewidth) {}
		set Par(e,resfill)      {}
		set Par(e,resoutline)   {}
		set Par(e,resdash)      {}
		set Par(e,resstipples)  {}	
		set Par(e,restags)      {}
		
		set Par(e,xdata)        {}
		set Par(e,ydata)        {}
		set Par(e,resdata)      {}
		set Par(e,error)        {}
		set Par(e,errtags)      {}

		set Par(l,legendtext)   {}
	}

	incr Par(e,element)
	$Par(w) element create graph$Par(e,element)
	$Par(w) element configure graph$Par(e,element) \
		-linewidth [lindex $tmpGP(e,linewidth) 0]

	lappend Par(e,elementname)  graph$Par(e,element)
	lappend Par(e,xdata)        $xdata
	lappend Par(e,ydata)        $ydata
	lappend Par(e,error)        $dydata
	lappend Par(e,resdata)      $resdata
	lappend Par(l,legendtext)   [lindex $tmpGP(l,legendtext)   0]

	lappend Par(e,linewidth)    [lindex $tmpGP(e,linewidth)    0]
	lappend Par(e,linehide)     [lindex $tmpGP(e,linehide)     0]
	lappend Par(e,dash)         [lindex $tmpGP(e,dash)         0]
	lappend Par(e,dashcolor)    [lindex $tmpGP(e,dashcolor)    0]
	lappend Par(e,offdashcolor) [lindex $tmpGP(e,offdashcolor) 0]
	lappend Par(e,smooth)       [lindex $tmpGP(e,smooth)       0]

	lappend Par(e,symbol)       [lindex $tmpGP(e,symbol)       0]
	lappend Par(e,symbolhide)   [lindex $tmpGP(e,symbolhide)   0]
	lappend Par(e,scale)        [lindex $tmpGP(e,scale)        0]
	lappend Par(e,pixels)       [lindex $tmpGP(e,pixels)       0]
	lappend Par(e,fill)         [lindex $tmpGP(e,fill)         0]
	lappend Par(e,outline)      [lindex $tmpGP(e,outline)      0]
	lappend Par(e,outlinewidth) [lindex $tmpGP(e,outlinewidth) 0]

	lappend Par(e,errorhide)    [lindex $tmpGP(e,errorhide)    0]
	lappend Par(e,errlinewidth) [lindex $tmpGP(e,errlinewidth) 0]
	lappend Par(e,errdash)      [lindex $tmpGP(e,errdash)      0]
	lappend Par(e,errstipples)  [lindex $tmpGP(e,errstipples)  0]
	lappend Par(e,errfill)      [lindex $tmpGP(e,errfill)      0]
	lappend Par(e,erroutline)   [lindex $tmpGP(e,erroutline)   0]

	lappend Par(e,reshide)      [lindex $tmpGP(e,reshide)      0]
	lappend Par(e,reslinewidth) [lindex $tmpGP(e,reslinewidth) 0]
	lappend Par(e,resdash)      [lindex $tmpGP(e,resdash)      0]
	lappend Par(e,resstipples)  [lindex $tmpGP(e,resstipples)  0]
	lappend Par(e,resfill)      [lindex $tmpGP(e,resfill)      0]
	lappend Par(e,resoutline)   [lindex $tmpGP(e,resoutline)   0]
	
	$Par(w) element configure graph$Par(e,element) \
	        -color [lindex $tmpGP(e,dashcolor) 0]
	$Par(w) element configure graph$Par(e,element) \
	        -offdash [lindex $tmpGP(e,offdashcolor) 0]
	$Par(w) element configure graph$Par(e,element) \
	        -linewidth [expr [  lindex $tmpGP(e,linewidth) 0] \
	                          * [lindex $tmpGP(e,linehide) 0] ]
	$Par(w) element configure graph$Par(e,element) \
	        -smooth [lindex $tmpGP(e,smooth) 0]
	$Par(w) element configure graph$Par(e,element) \
	        -dashes [lindex $tmpGP(e,dash) 0]

	$Par(w) element configure graph$Par(e,element) \
	        -symbol [lindex $tmpGP(e,symbol) 0]
	$Par(w) element configure graph$Par(e,element) \
	        -scalesymbols [lindex $tmpGP(e,scale) 0]
	$Par(w) element configure graph$Par(e,element) \
	        -pixels [lindex $tmpGP(e,pixels) 0]
	$Par(w) element configure graph$Par(e,element) \
	        -hide [lindex $tmpGP(e,symbolhide) 0]
	$Par(w) element configure graph$Par(e,element) \
	        -fill [lindex $tmpGP(e,fill) 0]
	$Par(w) element configure graph$Par(e,element) \
	        -outline [lindex $tmpGP(e,outline) 0]
	$Par(w) element configure graph$Par(e,element) \
	        -outlinewidth [lindex $tmpGP(e,outlinewidth) 0]

	$Par(w) element configure graph$Par(e,element) \
	        -xdata $xdata
	$Par(w) element configure graph$Par(e,element) \
	        -ydata $ydata

	#
	# default legend text will be graph$i
	#
	$Par(w) element configure graph$Par(e,element) -label "[lindex Par(l,legendtext) [expr $Par(e,element)-1]]"
	#lappend Par(l,legendtext) "graph$Par(e,element)"

	#
	# create error bars of point j in graph i, 
	# error bars they are drawn by tag lines from ydata-dydata to ydata+dydata 
	# and named errbar$i$j, error bars are only displayed together with the 
	# data in element graph$i
	#
	set errTags {}
	set errorbars yes
	for {set j 0} {$j < [llength $dydata]} {incr j} {
		if {[lindex $dydata $j] <= 0} {
			set errorbars no
		}
	}
	if { $errorbars } {
		set plot_y_limits [$Par(w) yaxis limits]
		for {set j 0} {$j < [llength $dydata]} {incr j} {
			set xerr  [lindex $xdata $j]
			if { [lindex $dydata $j] > 0 } {
				set dylow [expr [lindex $ydata $j]-[lindex $dydata $j] ]
				if { $Par(y,logscale) == 1 } {
					if {$dylow < [lindex $plot_y_limits 0]} {
						set dylow [lindex $plot_y_limits 0]
					}
				}
				set dyhi  [expr [lindex $ydata $j]+[lindex $dydata $j] ]
				lappend errTags [$Par(w) marker create line  \
				        -coords [list $xerr $dylow $xerr $dyhi ] ]
			}
		}
	}
	lappend Par(e,errtags) $errTags
	return 1
}


#------------------------------------------------------------------------------
proc CreateGraphPar {GraphPar} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
	upvar $GraphPar Par

	set Par(symbolselection)   [list square circle diamond plus cross splus scross triangle]
	set Par(colorselection)    [list black red green blue cyan magenta yellow violet purple orange pink gray]
	set Par(fitrange)          no
	set Par(dashpattern)       {{0} {2 2} {8 2 8 2} {2 2 8 2} \
	                            {2 2 2 2 8 2} {8 2 8 2 2 2} }
	set Par(stipples)          {bdiagonal1 bdiagonal2 checker2 checker3 \
	                            cross1 cross2 cross3 crossdiag \
	                            dot1 dot2 dot3 dot4 fdiagonal1 fdiagonal2 \
	                            hline1 hline2 lbottom ltop rbottom rtop \
	                            vline1 vline2}
	set Par(dashnames)         {{-} {..} {--} {.-} \
	                            {..-} {--.}}
	set Par(w)                 ""
	set Par(GraphName)         $GraphPar
	set Par(width)             768
	set Par(height)            512
	set Par(font)              {Helvetica 10}
	set Par(title)             unknown

	set Par(g,color)           black
	set Par(g,dash)            0
	set Par(g,hide)            yes
	set Par(g,linewidth)       1
	set Par(g,minor)           no

	set Par(c,color)           black
	set Par(c,hide)            yes
	set Par(c,dash)            0
	set Par(c,linewidth)       1

	set Par(graph,fg)          black
	set Par(graph,bg)          white
	set Par(graph,relief)      flat
	set Par(graph,borderwidth) 0

	set Par(plot,relief)       flat
	set Par(plot,borderwidth)  0
	set Par(plot,background)   gray90
	set Par(fit,background)    #cccccc

	set Par(margin,left)       0
	set Par(margin,right)      0
	set Par(margin,bottom)     0
	set Par(margin,top)        0
	set Par(margin,plotpadx)   {5 5}
	set Par(margin,plotpady)   {5 5}

	set Par(x,color)      Black
	set Par(x,tickcolor)  black
	set Par(x,titlecolor) black
	set Par(x,tickfont)   {Helvetica 10}
	set Par(x,titlefont)  {Helvetica 10}
	set Par(x,size)       12
	set Par(x,rot)        0
	set Par(x,linewidth)  2
	set Par(x,logscale)   1
	set Par(x,mapped)     1
	set Par(x,title)     "Q / nm^-1"
	set Par(x,min)       ""
	set Par(x,max)       ""
	set Par(x,type)      "log10(x)"
	set Par(x,loose)     "no"
	set Par(x,showticks) "yes"
	set Par(x,stepsize)  0
	set Par(x,subticks)  5
	set Par(x,ticklength) 10
	set Par(x,factor)     1.0

	set Par(y,color)      Black
	set Par(y,tickcolor)  black
	set Par(y,titlecolor) black
	set Par(y,tickfont)   {Helvetica 10}
	set Par(y,titlefont)  {Helvetica 10}
	set Par(y,size)       12
	set Par(y,rot)        0
	set Par(y,linewidth) 2
	set Par(y,logscale)  1
	set Par(y,loose)     no
	set Par(y,mapped)    1
	set Par(y,title)    "I(Q)"
	set Par(y,min)      ""
	set Par(y,max)      ""
	set Par(y,type)     "log10(y)"
	set Par(y,loose)     "no"
	set Par(y,showticks) "yes"
	set Par(y,stepsize)  0
	set Par(y,subticks)  5
	set Par(y,ticklength) 10
	set Par(y,factor)     1.0

	set Par(e,element)      0
	set Par(e,elementname)  {{}}

	set Par(e,linewidth)    {2}
	set Par(e,linehide)     {0}
	set Par(e,dash)         {0}
	set Par(e,dashcolor)    {Black}
	set Par(e,offdashcolor) {White}
	set Par(e,smooth)       {linear}

	set Par(e,symbol)       {circle}
	set Par(e,scale)        {no}
	set Par(e,pixels)       {4}
	set Par(e,fill)         {Black}
	set Par(e,outline)      {Black}
	set Par(e,outlinewidth) {1}
	set Par(e,symbolhide)   {no}

	set Par(e,xdata)        {{}}
	set Par(e,ydata)        {{}}
	set Par(e,resdata)      {{}}
	set Par(e,error)        {{}}
	
	set Par(e,errorhide)    {no}
	set Par(e,errlinewidth) {1}
	set Par(e,errdash)      {0}
	set Par(e,errstipples)  {hline1}
	set Par(e,errfill)      {Black}
	set Par(e,erroutline)   {blue}
	set Par(e,errtags)      {{}}
	
	
	set Par(e,reshide)      {no}
	set Par(e,reslinewidth) {1}
	set Par(e,resdash)      {0}
	set Par(e,resstipples)  {hline1}
	set Par(e,resfill)      {Black}
	set Par(e,resoutline)   {blue}
	set Par(e,restags)      {{}}

	set Par(l,legendtext)  {"unknown"}
	set Par(l,hide)        yes
	set Par(l,bg)          white
	set Par(l,fg)          black
	set Par(l,relief)      sunken
	set Par(l,borderwidth) 2
	set Par(l,font)        {Helvetica 8}
	set Par(l,padx)        {0 0}
	set Par(l,pady)        {0 0}
	set Par(l,ipadx)       {0 0}
	set Par(l,ipady)       {0 0}
	set Par(l,position)    right
	set Par(l,posx)        10
	set Par(l,posy)        10
	set Par(l,anchor)      n

	set Par(lower,x) ""
	set Par(lower,y) ""
	set Par(upper,x) ""
	set Par(upper,y) ""
}

#------------------------------------------------------------------------------
proc RefreshGraph {Graph} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^
	global sasfit
	upvar $Graph w
	if { ![info exists w] } { return 0}

	$w(w) configure -width  $w(width)
	$w(w) configure -height $w(height)
	$w(w) configure -font   $w(font)

	$w(w) grid configure -color     $w(g,color)
	$w(w) grid configure -dashes    $w(g,dash)
	$w(w) grid configure -hide      $w(g,hide)
	$w(w) grid configure -linewidth $w(g,linewidth)
	$w(w) grid configure -minor     $w(g,minor)

	$w(w) crosshairs configure -linewidth $w(c,linewidth)
	$w(w) crosshairs configure -color     $w(c,color)
	$w(w) crosshairs configure -hide      $w(c,hide)
	$w(w) crosshairs configure -dashes    $w(c,dash)

	set xmin $w(x,min)
	set xmax $w(x,max)
	set ymin $w(y,min)
	set ymax $w(y,max)
	if { ([string length $w(x,min)] > 0) && \
	     ([string length $w(x,max)] > 0) 
	} {
		if { $w(x,min) < $w(x,max) } { 
			set xmin $w(x,min) 
			set xmax $w(x,max)
		} elseif { $w(x,min) == $w(x,max) } {
			set xmin $w(x,min)
			set xmax [expr 2.0*($w(x,min)+0.1)]
		} else {
			set xmin $w(x,max)
			set xmax $w(x,min)
		}
	}

	if { ([string length $w(y,min)] > 0) && \
	     ([string length $w(y,max)] > 0) 
	} {
		if { $w(y,min) < $w(y,max) } { 
			set ymin $w(y,min) 
			set ymax $w(y,max)
		} elseif { $w(y,min) == $w(y,max) } {
			set ymin $w(y,min) 
			set ymax [expr 2.0*($w(y,min)+0.1)]
		} else {
			set ymin $w(y,max)
			set ymax $w(y,min)
		}
	} 

	set $w(x,min) $xmin
	set $w(x,max) $xmax
	set $w(y,min) $ymin
	set $w(y,max) $ymax
	$w(w) axis configure x -min $xmin
	$w(w) axis configure x -max $xmax
	$w(w) axis configure y -min $ymin
	$w(w) axis configure y -max $ymax

	#
	#  convert Q-data and I(Q)-data into x-data and y-data type coordinates
	#
	if { [string compare $w(x,type) "log10(x)"] == 0 } {
		set w(x,logscale) 1
	} else { set w(x,logscale) 0 }
	
	if { [string compare $w(y,type) "log10(y)"] == 0 } {
		set w(y,logscale) 1
	} else { set w(y,logscale) 0 }

	#
	#  set correct fontname
	#
	$w(w) axis configure x -titlefont $w(x,titlefont)
	$w(w) axis configure x -titlecolor $w(x,titlecolor)
	$w(w) axis configure x -tickfont $w(x,tickfont)
	$w(w) axis configure x -color $w(x,tickcolor)
	$w(w) axis configure x -rotate $w(x,rot)
	$w(w) axis configure x -linewidth $w(x,linewidth)
	$w(w) axis configure x -logscale $w(x,logscale)
	$w(w) axis configure x -loose $w(x,loose)

	set xtrans $w(x,type)

	if {([string compare $xtrans "log10(x)"] != 0) } {
	    if {$w(x,factor) != 1} {
			set xscale "* $w(x,factor)"
			regsub -all "x" $xtrans   "($w(x,title) $xscale)" xtrans
	    } else {
			regsub -all "x" $xtrans   "$w(x,title)" xtrans
	    }
	} else { set xtrans $w(x,title) }


	$w(w) axis configure x -title $xtrans
	$w(w) axis configure x -showticks $w(x,showticks)
	$w(w) axis configure x -stepsize $w(x,stepsize)
	$w(w) axis configure x -subdivisions $w(x,subticks)
	$w(w) axis configure x -ticklength $w(x,ticklength)

	$w(w) axis configure y -titlefont $w(y,titlefont)
	$w(w) axis configure y -titlecolor $w(y,titlecolor)
	$w(w) axis configure y -tickfont $w(y,tickfont)
	$w(w) axis configure y -color $w(y,tickcolor)
	$w(w) axis configure y -rotate $w(y,rot)
	$w(w) axis configure y -linewidth $w(y,linewidth)
	$w(w) axis configure y -logscale $w(y,logscale)
	$w(w) axis configure y -loose $w(y,loose)
	
	set ytrans $w(y,type)

	if {([string compare $ytrans "log10(y)"] != 0) } {
	    if {$w(y,factor) != 1} {
		set yscale "$w(y,factor) *"
		regsub -all "y" $ytrans   "($yscale $w(y,title))" ytrans
	    } else {
		regsub -all "y" $ytrans   "$w(y,title)" ytrans
	    }
	    if {$w(x,factor) != 1} {
		set xscale "* $w(x,factor)"
		regsub -all "x" $ytrans   "($w(x,title) $xscale))" ytrans
	    } else {
		regsub -all "x" $ytrans   "$w(x,title)" ytrans
	    }
	} else { set ytrans $w(y,title) }


	$w(w) axis configure y -title $ytrans
	$w(w) axis configure y -showticks $w(y,showticks)
	$w(w) axis configure y -stepsize $w(y,stepsize)
	$w(w) axis configure y -subdivisions $w(y,subticks)
	$w(w) axis configure y -ticklength $w(y,ticklength)

	$w(w) configure -title           $w(title)
	$w(w) configure -font            $w(font)
	$w(w) configure -plotbackground  $w(plot,background)
	$w(w) configure -plotrelief      $w(plot,relief)
	$w(w) configure -plotborderwidth $w(plot,borderwidth)
	$w(w) configure -foreground      $w(graph,fg)
	$w(w) configure -background      $w(graph,bg)
	$w(w) configure -relief          $w(graph,relief)
	$w(w) configure -borderwidth     $w(graph,borderwidth)
	$w(w) configure -bottommargin    $w(margin,bottom)
	$w(w) configure -topmargin       $w(margin,top)
	$w(w) configure -leftmargin      $w(margin,left)
	$w(w) configure -rightmargin     $w(margin,right)
	$w(w) configure -plotpadx        $w(margin,plotpadx)
	$w(w) configure -plotpady        $w(margin,plotpady)

	foreach etag [$w(w) marker names] {
		$w(w) marker delete $etag
	}
		
	for {set i 0} {$i < $w(e,element)} {incr i} {
		set graphname [lindex $w(e,elementname) $i]

		$w(w) element configure $graphname \
		      -linewidth [expr  [lindex $w(e,linewidth) $i] \
		                      * [lindex $w(e,linehide)  $i] ]
		$w(w) element configure $graphname \
		      -dashes [lindex $w(e,dash) $i]
		$w(w) element configure $graphname \
		      -color [lindex $w(e,dashcolor) $i]
		$w(w) element configure $graphname \
		      -offdash [lindex $w(e,offdashcolor) $i]
		$w(w) element configure $graphname \
		      -smooth [lindex $w(e,smooth) $i]

		$w(w) element configure $graphname -symbol \
		      [lindex $w(e,symbol)     $i]
		$w(w) element configure $graphname -hide \
		      [lindex $w(e,symbolhide)     $i]
		$w(w) element configure $graphname -scalesymbols \
		      [lindex $w(e,scale)      $i]
		$w(w) element configure $graphname -pixels \
		      [lindex $w(e,pixels)     $i]
		$w(w) element configure $graphname -fill \
		      [lindex $w(e,fill)       $i]
		$w(w) element configure $graphname -outline \
		      [lindex $w(e,outline)    $i]
		$w(w) element configure $graphname -outlinewidth \
		      [lindex $w(e,outlinewidth)    $i]

		$w(w) element configure $graphname -label   [lindex $w(l,legendtext) $i]
		$w(w) legend  configure -hide        $w(l,hide)
		$w(w) legend  configure -relief      $w(l,relief)
		$w(w) legend  configure -borderwidth $w(l,borderwidth)
		$w(w) legend  configure -font        $w(l,font)
		$w(w) legend  configure -foreground  $w(l,fg)
		$w(w) legend  configure -background  $w(l,bg)
		$w(w) legend  configure -padx        $w(l,padx)
		$w(w) legend  configure -pady        $w(l,pady)
		$w(w) legend  configure -ipadx       $w(l,ipadx)
		$w(w) legend  configure -ipady       $w(l,ipady)
		set legendxypos $w(l,position)
		regsub -all "x" $w(l,position) "$w(l,posx)" legendxypos
		regsub -all "y" $legendxypos   "$w(l,posy)" legendxypos
		$w(w) legend  configure -position    $legendxypos
		$w(w) legend  configure -anchor      $w(l,anchor)

		#
		# preparation for transformation of  xdata, ydata, error and errtags
		#
		set xdata     [lindex $w(e,xdata)   $i]
		set ydata     [lindex $w(e,ydata)   $i]
		set resdata   [lindex $w(e,resdata) $i]
		set error     [lindex $w(e,error)   $i]
		set errtags   [lindex $w(e,errtags) $i]

		# ignore negative values on certain scales
		if {[llength $ydata] && [llength $xdata] &&
			([string match "*log*" $w(y,type)] ||
			 [string match "*sqrt*" $w(y,type)])
		} {
			set xlist {}
			set ylist {}
			set reslist {}
			set errlist {}
			set etaglist {}
			foreach xval $xdata yval $ydata resval $resdata \
		       		errval $error etag $errtags {

				if {$yval > 0} {
					lappend xlist $xval
					lappend ylist $yval
					if {[llength $resdata]} { lappend reslist $resval }
					if {[llength $error]} { lappend errlist $errval }
					if {[llength $errtags]} { lappend etaglist $etag }
				}
			}
			set xdata $xlist
			set ydata $ylist
			set resdata $reslist
			set error $errlist
			set errtags $etaglist
		}

		regsub -all "x" $w(x,type) "\$x" xtrans
		regsub -all "y" $xtrans    "\$y" xtrans
		regsub -all "x" $w(y,type) "\$x" ytrans
		regsub -all "y" $ytrans    "\$y" ytrans
		if {[string compare $w(x,type) "arcsinh(x)"] == 0 } {
			set xtrans "log(\$x+hypot(\$x,1))"
		}
		if {[string compare $w(y,type) "arcsinh(y)"] == 0 } {
			set ytrans "log(\$y+hypot(\$y,1))"
		}

		#
		# real transformation of xdata
		#
		if { ([string compare $xtrans "\$x"] != 0) && 
		     ([string compare $xtrans "log10(\$x)"] != 0) 
		} {
			set xxdata {}
			foreach x $xdata y $ydata {
				lappend xxdata [expr $xtrans]
			}
		} else { set xxdata $xdata }

		#
		# real transformation of ydata
		#
		if { ([string compare $ytrans "\$y"] != 0) && 
		     ([string compare $ytrans "log10(\$y)"] != 0) 
		} {
			set yydata {}
			foreach x $xdata y $ydata {
				lappend yydata [expr $ytrans]
#				if {[catch {lappend yydata [expr $ytrans]}]} {
#					puts stderr "catch1: '$ytrans' '$y'"
#				}
			}
		} else { set yydata $ydata }

		$w(w) element configure $graphname -xdata  $xxdata
		$w(w) element configure $graphname -ydata  $yydata
		
		#
		# transformation of error data into dylow and dyhi values
		#
		set thereIsError 0
		foreach err $error {  
			if {$err != -1.0} { 
				set thereIsError 1
				break 
			}
		}
		set dylow {}
		set dyhi  {}
		set dxlow {}
		set dxhi  {}

		if { [llength $error] > 0 && $thereIsError } {
			foreach y $ydata e $error {
				lappend dylow [expr $y - 1.0*abs($e)]
				lappend dyhi  [expr $y + 1.0*abs($e)]
			}
			#
			# real transformation of dylow and dyhi
			#
			set limit_low [lindex [$w(w) yaxis limits] 0]
			if { ([string compare $ytrans "\$y"] != 0) && 
			     ([string compare $ytrans "log10(\$y)"] != 0) 
			} {
				set dyylow {}
				foreach x $xdata y $dylow {
					# avoid sqrt(negative) or log(negative)
					if {[catch {set val [expr $ytrans]}]} {
						set val $limit_low
					# avoid outside of plot area coords
					} elseif { $val < $limit_low } {
						set val $limit_low
					}
					lappend dyylow $val
				}
			} else { set dyylow $dylow }

			# avoid coords outside of plot area
			set dyylow_tmp {}
			foreach yval $dyylow {
				if {$yval < $limit_low} { set yval $limit_low }
				lappend dyylow_tmp $yval
			}
			set dyylow $dyylow_tmp

			if { ([string compare $ytrans "\$y"] != 0) &&
			     ([string compare $ytrans "log10(\$y)"] != 0)
			} {
				set dyyhi {}
				foreach x $xdata y $dyhi {
					lappend dyyhi [expr $ytrans]
				}
			} else { set dyyhi $dyhi }

			if {[lindex $w(e,errorhide) $i]} {
				$w(w) element configure $graphname -ylow {}
				$w(w) element configure $graphname -yhigh {}
			} else {
				$w(w) element configure $graphname -ylow $dyylow
				$w(w) element configure $graphname -yhigh $dyyhi
				$w(w) element configure $graphname -errorbarcolor [lindex $w(e,erroutline) $i]
				$w(w) element configure $graphname -errorbarwidth [lindex $w(e,errlinewidth) $i]
			}
		};# if { [llength $error] > 0 }
	
		set thereIsResdata 0
		foreach resd $resdata {  
			if {$resd > 0.0} { 
				set thereIsResdata 1
				break 
			}
		}
	
		if { [llength $resdata] > 0 && $thereIsResdata } {
			foreach x $xdata r $resdata {
				lappend dxlow [expr $x - 1.0*abs($r)]
				lappend dxhi  [expr $x + 1.0*abs($r)]
			}
			#
			# real transformation of dxlow and dxhi
			#
			set limit_low [lindex [$w(w) xaxis limits] 0]
			if { ([string compare $xtrans "\$x"] != 0) && 
			     ([string compare $xtrans "log10(\$x)"] != 0) 
			} {
				set dxxlow {}
				foreach x $dxlow y $ydata {
					# avoid sqrt(negative) or log(negative)
					if {[catch {set val [expr $xtrans]}]} {
						set val $limit_low
					# avoid outside of plot area coords
					} elseif { $val < $limit_low } {
						set val $limit_low
					}
					lappend dxxlow $val
				}
			} else { set dxxlow $dxlow }

			# avoid coords outside of plot area
			set dxxlow_tmp {}
			foreach xval $dxxlow {
				if {$xval < $limit_low} { set xval $limit_low }
				lappend dxxlow_tmp $xval
			}
			set dxxlow $dxxlow_tmp

			if { ([string compare $xtrans "\$x"] != 0) &&
			     ([string compare $xtrans "log10(\$x)"] != 0)
			} {
				set dxxhi {}
				foreach x $dxhi y $ydata {
					lappend dxxhi [expr $xtrans]
				}
			} else { set dxxhi $dxhi }

			if {[lindex $w(e,reshide) $i]} {
				$w(w) element configure $graphname -xlow {}
				$w(w) element configure $graphname -xhigh {}
			} else {
				$w(w) element configure $graphname -xlow $dxxlow
				$w(w) element configure $graphname -xhigh $dxxhi
				$w(w) element configure $graphname -errorbarcolor [lindex $w(e,erroutline) $i]
				$w(w) element configure $graphname -errorbarwidth [lindex $w(e,errlinewidth) $i]
			}
		};# if { [llength $error] > 0 }
	
	}

	if {[string compare $w(fitrange) yes] == 0} {
		Make_Square_el w $w(lower,x) $w(upper,x)
	}
	$w(w) element show
	return 1
}


#------------------------------------------------------------------------------
# 
proc axisformat {w label} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^
	return [format %g $label]
}

#------------------------------------------------------------------------------
# Creates a blt_graph and a corresponding array "Graph" containing the
# options and data of the Graph used in sasfittcl
#
proc CreateGraph {Graph} {
#^^^^^^^^^^^^^^^^^^^^^^^^^
	global sasfit
	upvar $Graph w
	CreateGraphPar $Graph
	blt::graph $w(w)
	$w(w) yaxis configure  -command axisformat
	$w(w) xaxis configure  -command axisformat
	RefreshGraph w
}
