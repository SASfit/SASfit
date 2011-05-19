# sasfit.vfs/lib/app-sasfit/tcl/sasfit_MaxEnt.tcl
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

#------------------------------------------------------------------------------
#  save the parameters in file and run the program "sizes" from Pete Jemian
#  (URL: http://www.uni.aps.anl.gov/~jemian/pete/sas.html)
#  read and display the calculated size distribution
#
proc MaxEntFit {} {
#^^^^^^^^^^^^^^^^^^
global MaxEntPar IQGraph ResIQGraph SDGraph StructParData sasfit
write_sizes_inp_file
write_sizes_expdata_file
#
# create a window for the output of the program sizes
#
if { [winfo exists .sizesoutput] } {destroy .sizesoutput}
toplevel .sizesoutput
set w .sizesoutput
wm geometry $w
wm title $w "Output -- sizes"
frame $w.f
pack $w.f -side top -fill both -expand 1
global  execm
set execm [text $w.t -setgrid 1 -wrap word -width 80 -height 20\
                      -yscrollcommand "$w.sy set"]
scrollbar $w.sy -orient vert -command "$w.t yview"
pack $w.sy -side right -fill y -in $w.f
pack $w.t -side left -fill both -expand 1 -in $w.f
global f
global f_close
global tmp
set f_close "no"
set f [open "|$MaxEntPar(SizesRunCmd) $MaxEntPar(CmdInputFileName)" r]
set tmp "?"
fileevent $f readable { 
   global f
   global execm
   global f_close
   global tmp
   if {![eof $f]} {
      gets $f execmessage
      if {[string compare $execmessage "NO SOLUTION"]==0}\
         {
            set tmp $execmessage
            close $f
            catch {set f_close yes}
      } 
      $execm insert end [append execmessage "\n"]
      update idletasks
   } else { 
        close $f 
        catch {set f_close yes}
   }
}

trace variable f_close w  {
global MaxEntPar IQGraph ResIQGraph SDGraph StructParData sasfit
global f_close
global tmp
#
# check if fit converged, then read and display fit result
#
if { !([string compare $tmp  \
                       "NO SOLUTION"] == 0) } {
#
#  reading and displaying intensity data
#
puts "MaxEnt found a solution"
#   read_sizes_fit_file $sasfit(datadir)/$MaxEntPar(ProjectName).fit Q SAS DSAS Fit Z StructPar
   read_sizes_fit_file $MaxEntPar(ProjectName).fit Q SAS DSAS Fit Z StructPar
   clearGraph_el IQGraph 
   Put_Graph_el IQGraph $Q $SAS $DSAS
   Put_Graph_el IQGraph $Q $Fit
   set IQGraph(e,symbol)     [lreplace $IQGraph(e,symbol)    1 1 ""]
   set IQGraph(e,linehide)     [lreplace $IQGraph(e,linehide)    1 1 1]
   set IQGraph(e,linewidth)     [lreplace $IQGraph(e,linewidth)    1 1 2]
   set IQGraph(e,dashcolor)     [lreplace $IQGraph(e,dashcolor)    1 1 red]

#   set IQGraph(e,linewidth)  [lreplace $IQGraph(e,linewidth) 0 0 0]
   set IQGraph(l,legendtext) [lreplace $IQGraph(l,legendtext) 0 0 \
                                       [file tail $sasfit(filename)]]
   set IQGraph(l,legendtext) [lreplace $IQGraph(l,legendtext) 1 1 Fit]
   RefreshGraph IQGraph
#
# reading and displaying residual intensity data
#
   clearGraph_el ResIQGraph
   Put_Graph_el ResIQGraph $Q $Z
#   set ResIQGraph(x,logscale)   [lreplace $ResIQGraph(x,logscale)  0 0 0]
#   set ResIQGraph(y,logscale)   [lreplace $ResIQGraph(y,logscale)  0 0 0]
   set ResIQGraph(e,linehide)  [lreplace $ResIQGraph(e,linehide) 0 0 0]
   set ResIQGraph(l,legendtext) [lreplace ResIQGraph(l,legendtext) 0 0 \
                                       [file tail $sasfit(filename)]]
#   set ResIQGraph(l,mapped) 0
   RefreshGraph ResIQGraph

#
# reading and displaying calculated size distribution
#
   switch $MaxEntPar(vPower) {
      0 { set sdfname $sasfit(tcl)/$MaxEntPar(ProjectName).N-D
          set sdfname $MaxEntPar(ProjectName).N-D
          set ytitle N(D) }
      1 { set sdfname $sasfit(tcl)/$MaxEntPar(ProjectName).f-D
          set sdfname $MaxEntPar(ProjectName).f-D 
          set ytitle N(D)*V }
      2 { set sdfname $sasfit(tcl)/$MaxEntPar(ProjectName).i-D
          set sdfname $MaxEntPar(ProjectName).i-D
          set ytitle N(D)*V^2 }
   }
   set xtitle "D / nm"

   create_ASCIIData Data
   set Data(InputFormat) xy
   set Data(LineSkip) 2
   read_Ascii $sdfname Data 
   clearGraph_el SDGraph
   Put_Graph_el SDGraph $Data(Q) $Data(I)
#   set SDGraph(x,logscale)  [lreplace $SDGraph(x,logscale)  0 0 0]
#   set SDGraph(y,logscale)  [lreplace $SDGraph(y,logscale)  0 0 0]
   set SDGraph(x,title)     $xtitle 
   set SDGraph(y,title)     $ytitle 
   set SDGraph(e,symbol)    [lreplace $SDGraph(e,symbol)    0 0 ""]
#   set SDGraph(e,linewidth)    [lreplace $SDGraph(e,linewidth)  0 0 2]
   set SDGraph(e,linehide)    [lreplace $SDGraph(e,linehide)  0 0 1]
   set SDGraph(l,mapped) 0
   RefreshGraph SDGraph
} else {
puts "MaxEnt did not found a solution"
   clearGraph_el ResIQGraph
   RefreshGraph ResIQGraph
   clearGraph_el SDGraph
   RefreshGraph SDGraph
   clearGraph_el IQGraph
   Put_Graph_el IQGraph $sasfit(Q) $sasfit(I) $sasfit(DI)
   set IQGraph(e,linewidth) {0}
   set IQGraph(l,legendtext) [list [file tail $sasfit(filename)] ]
   RefreshGraph IQGraph
}
catch {unset f_close}
}
}


#------------------------------------------------------------------------------
#   test, if the contents of the input fields make sense
#
proc applyMaxEntOptionsCmd {tmpMaxEntPar} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
global MaxEntPar
upvar $tmpMaxEntPar Par
cp_arr Par MaxEntPar
return 1
}

#------------------------------------------------------------------------------
#
proc MaxEntFitCmd {} {
#^^^^^^^^^^^^^^^^^^^^^
global sasfit
global MaxEntPar
global tmpMaxEntPar

cp_arr MaxEntPar tmpMaxEntPar

if {[winfo exists .maxent]} {destroy .maxent}
toplevel .maxent
set w .maxent
wm geometry $w
wm title $w "MaxEnt - Input Parameter"

frame .maxent.type 
.maxent.type configure -relief groove -borderwidth 1
pack .maxent.type  -fill x
radiobutton .maxent.type.maxentRB -text "Maximum Entropy" -anchor w \
            -variable tmpMaxEntPar(AnalysisType) -value 1 -highlightthickness 0
radiobutton .maxent.type.regularizationRB -text "Regularization" -anchor w \
            -variable tmpMaxEntPar(AnalysisType) -value 0 -highlightthickness 0
pack .maxent.type.maxentRB .maxent.type.regularizationRB \
     -expand 1 -side left -padx 2m  -pady 2m -anchor n -in .maxent.type

frame .maxent.inputdata 
pack .maxent.inputdata -fill both -expand 1

frame .maxent.inputdata.iq -relief groove -borderwidth 1
pack .maxent.inputdata.iq -fill both -expand 1 -in .maxent.inputdata -side left
set w .maxent.inputdata.iq.lay
frame $w
pack $w -in .maxent.inputdata.iq -fill y -expand 1
frame $w.qmin
entry $w.qmin.qminentry -textvariable tmpMaxEntPar(Qmin) \
      -relief sunken  -width 10 -highlightthickness 0
label $w.qmin.qmintext -text "Q_min (1/nm):"
frame $w.qmax
entry $w.qmax.qmaxentry -textvariable tmpMaxEntPar(Qmax) \
      -relief sunken -width 10 -highlightthickness 0
label $w.qmax.qmaxtext -text "Q_max (1/nm):"
frame $w.eta 
entry $w.eta.etaentry -textvariable tmpMaxEntPar(eta) \
      -relief sunken -width 10 -highlightthickness 0
label $w.eta.etatext -text "eta:"
frame $w.bkg 
entry $w.bkg.bkgentry -textvariable tmpMaxEntPar(bkg) \
      -relief sunken -width 10 -highlightthickness 0
label $w.bkg.bkgtext -text "bkg:"
frame $w.fac
entry $w.fac.facentry -textvariable tmpMaxEntPar(fac) \
      -relief sunken -width 10 -highlightthickness 0
label $w.fac.factext -text "fac:"
frame $w.err
entry $w.err.errentry -textvariable tmpMaxEntPar(err) \
      -relief sunken -width 10 -highlightthickness 0
label $w.err.errtext -text "err:"
pack $w.qmin -in $w -anchor e -fill y -expand 1 -padx 1m -pady 1m
pack $w.qmin.qminentry $w.qmin.qmintext -in $w.qmin -side right
pack $w.qmax -in $w -anchor e -fill y -expand 1 -padx 1m -pady 1m
pack $w.qmax.qmaxentry $w.qmax.qmaxtext -in $w.qmax -side right
pack $w.eta -in $w -anchor e -fill y -expand 1 -padx 1m -pady 1m
pack $w.eta.etaentry $w.eta.etatext -in $w.eta -side right
pack $w.bkg -in $w -anchor e -fill y -expand 1 -padx 1m -pady 1m
pack $w.bkg.bkgentry $w.bkg.bkgtext -in $w.bkg -side right
pack $w.fac -in $w -anchor e -fill y -expand 1 -padx 1m -pady 1m
pack $w.fac.facentry $w.fac.factext -in $w.fac -side right
pack $w.err -in $w -anchor e -fill y -expand 1 -padx 1m -pady 1m
pack $w.err.errentry $w.err.errtext -in $w.err -side right

frame .maxent.inputdata.sd -relief groove -borderwidth 1
pack .maxent.inputdata.sd -fill both -expand 1 -in .maxent.inputdata -side right
set w .maxent.inputdata.sd.lay
frame $w
pack $w -in .maxent.inputdata.sd -fill y -expand 1
frame $w.dmin
entry $w.dmin.dminentry -textvariable tmpMaxEntPar(Dmin) \
      -relief sunken  -width 10 -highlightthickness 0
label $w.dmin.dmintext -text "D_min (nm):"
frame $w.dmax
entry $w.dmax.dmaxentry -textvariable tmpMaxEntPar(Dmax) \
      -relief sunken -width 10 -highlightthickness 0
label $w.dmax.dmaxtext -text "D_max (nm):"
frame $w.nRadii
entry $w.nRadii.nRadiientry -textvariable tmpMaxEntPar(nRadii) \
      -relief sunken -width 10 -highlightthickness 0
label $w.nRadii.nRadiitext -text "n radii:"
frame $w.vPower
frame $w.vPower.l
frame $w.vPower.r
label $w.vPower.l.label -text "size distribribution type:"
radiobutton $w.vPower.r.zero -text "N(D)" \
            -variable tmpMaxEntPar(vPower) -value 0 -highlightthickness 0
radiobutton $w.vPower.r.one  -text "N(D) V" \
            -variable tmpMaxEntPar(vPower) -value 1 -highlightthickness 0
radiobutton $w.vPower.r.two  -text "N(D) V^2" \
            -variable tmpMaxEntPar(vPower) -value 2 -highlightthickness 0
frame $w.aspectratio
entry $w.aspectratio.aspectratioentry -textvariable tmpMaxEntPar(AspectRatio) \
      -relief sunken  -width 10 -highlightthickness 0
label $w.aspectratio.aspectratiotext -text "Aspect Ratio:"
frame $w.bintype
label $w.bintype.bintypetext -text "bin type:"
radiobutton $w.bintype.lin -text "lin" -variable tmpMaxEntPar(BinType) \
            -value 1 -highlightthickness 0
radiobutton $w.bintype.log -text "log" -variable tmpMaxEntPar(BinType) \
            -value 0 -highlightthickness 0
pack $w.dmin -in $w -anchor e -fill y -expand 1 -padx 1m -pady 1m
pack $w.dmin.dminentry $w.dmin.dmintext -in $w.dmin -side right
pack $w.dmax -in $w -anchor e -fill y -expand 1 -padx 1m -pady 1m
pack $w.dmax.dmaxentry $w.dmax.dmaxtext -in $w.dmax -side right
pack $w.nRadii -in $w -anchor e -fill y -expand 1 -padx 1m -pady 1m
pack $w.nRadii.nRadiientry $w.nRadii.nRadiitext -in $w.nRadii -side right
pack $w.vPower -in $w -anchor e -fill y -expand 1 -padx 1m -pady 1m
pack $w.vPower.l -in $w.vPower
pack $w.vPower.l.label $w.vPower.r -in $w.vPower.l -anchor w
pack $w.vPower.r.zero $w.vPower.r.one $w.vPower.r.two \
     -in $w.vPower.r -side left  -padx 1m
pack $w.aspectratio -in $w -anchor e -fill y -expand 1 -padx 1m -pady 1m
pack $w.aspectratio.aspectratioentry $w.aspectratio.aspectratiotext \
     -in $w.aspectratio -side right
pack $w.bintype -in $w -anchor e -fill y -expand 1 -padx 1m -pady 1m
pack $w.bintype.log $w.bintype.lin -in $w.bintype -side right -padx 1m
pack $w.bintype.bintypetext -in $w.bintype -side right


frame .maxent.method
.maxent.method configure -relief groove -borderwidth 1
pack .maxent.method -fill x
set w .maxent.method
frame $w.par
pack $w.par -fill x -expand 1
frame $w.par.defaultDistLevel
label $w.par.defaultDistLevel.text -text "default dist. level:" 
entry $w.par.defaultDistLevel.entry \
      -textvariable tmpMaxEntPar(defaultDistLevel)  \
      -width 10 -highlightthickness 0
pack $w.par.defaultDistLevel.text $w.par.defaultDistLevel.entry \
     -in $w.par.defaultDistLevel -side left 
frame $w.par.dlambda_lambda
label $w.par.dlambda_lambda.text -text "dlambda/lambda:"
entry $w.par.dlambda_lambda.entry \
      -textvariable tmpMaxEntPar(dlambda_lambda) -width 10 -highlightthickness 0
pack $w.par.dlambda_lambda.text $w.par.dlambda_lambda.entry \
     -in $w.par.dlambda_lambda -side left  
pack $w.par.defaultDistLevel $w.par.dlambda_lambda  \
     -in $w.par -side left -fill x -expand 1 -pady 2m -padx 2m
scale $w.itermax -label "Maximum iterations:" \
     -from 10 -to 300 -showvalue 1 \
     -orient horizontal -variable tmpMaxEntPar(IterMax) -highlightthickness 0
pack $w.par $w.itermax -in $w -fill x -padx 1m -pady 1m


frame .maxent.action 
.maxent.action configure -relief groove -borderwidth 1
pack .maxent.action -fill x
button .maxent.action.fit -text Fit \
       -command {if {[applyMaxEntOptionsCmd tmpMaxEntPar]} {MaxEntFit} } \
       -highlightthickness 0
button .maxent.action.apply -text Apply \
       -command {applyMaxEntOptionsCmd tmpMaxEntPar} -highlightthickness 0
button .maxent.action.dismiss -text Cancel -command {destroy .maxent} \
       -highlightthickness 0
button .maxent.action.help -text "Help..."  \
       -highlightthickness 0

pack .maxent.action.fit .maxent.action.apply .maxent.action.dismiss \
     -side left -padx 1m -pady 1m
pack .maxent.action.help -side right -fill x -padx 1m -pady 2m
}
