# sasfit.vfs/lib/app-sasfit/tcl/sasfit_gnom.tcl
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
#  save the parameters in file and run the program "gnom" from D. Svergun
#
proc GnomFit {} {
#^^^^^^^^^^^^^^^^^^
global GnomPar IQGraph ResIQGraph SDGraph StructParData sasfit
write_gnom_inp_file
write_gnom_expdata_file
#
# create a window for the output of the program gnom
#
if { [winfo exists .gnomoutput] } {destroy .gnomoutput}
toplevel .gnomoutput
set w .gnomoutput
wm geometry $w
wm title $w "Output -- GNOM"
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
set f [open "|$GnomPar(GnomRunCmd)" r]
set tmp "?"
fileevent $f readable { 
   global f
   global execm
   global f_close
   global tmp
   if {![eof $f]} {
      gets $f execmessage
      $execm insert end [append execmessage "\n"]
      update idletasks
   } else { 
        close $f 
        catch {set f_close yes}
   }
}
trace variable f_close w  {
global GnomPar IQGraph ResIQGraph SDGraph StructParData sasfit
global f_close
global tmp
#
# check if fit converged, then read and display fit result
#

#
#  reading and displaying intensity data
#

#
# reading and displaying residual intensity data
#

#
# reading and displaying calculated size distribution
#
catch {unset f_close}
}
}


#------------------------------------------------------------------------------
#   test, if the contents of the input fields make sense
#
proc applyGnomOptionsCmd {tmpGnomPar} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
global GnomPar
upvar $tmpGnomPar Par
cp_arr Par GnomPar
return 1
}

#------------------------------------------------------------------------------
#
proc GnomRunCmd {} {
#^^^^^^^^^^^^^^^^^^^^^
global sasfit
global GnomPar
global tmpGnomPar

cp_arr GnomPar tmpGnomPar

if {[winfo exists .gnom]} {destroy .gnom}
toplevel .gnom
set w .gnom
wm geometry $w
wm title $w "Gnom - Input Parameter"

frame $w.inputdata
pack $w.inputdata -fill both -expand 1

frame $w.inputdata.col1 -relief groove -borderwidth 1
pack $w.inputdata.col1 -fill both -expand 1 -in .gnom.inputdata -side left
set w .gnom.inputdata.col1.lay
frame $w
pack $w -in .gnom.inputdata.col1 -fill y -expand 1

frame $w.printer
entry $w.printer.printerentry -textvariable tmpGnomPar(Printer,v) \
      -relief sunken -width 12 -highlightthickness 0
label $w.printer.printertext -text "Printer:"
frame $w.expert
entry $w.expert.expertentry -textvariable tmpGnomPar(Expert,v) \
      -relief sunken -width 12 -highlightthickness 0
label $w.expert.experttext -text "Expert:"
frame $w.input1
entry $w.input1.input1entry -textvariable tmpGnomPar(Input1,v) \
      -relief sunken -width 12 -highlightthickness 0
label $w.input1.input1text -text "INPUT1:"
frame $w.input2
entry $w.input2.input2entry -textvariable tmpGnomPar(Input2,v) \
      -relief sunken -width 12 -highlightthickness 0
label $w.input2.input2text -text "INPUT2:"
frame $w.evaerr
entry $w.evaerr.evaerrentry -textvariable tmpGnomPar(EvaErr,v) \
      -relief sunken -width 12 -highlightthickness 0
label $w.evaerr.evaerrtext -text "EVAERR: Calculate Errors (Y/N):"
frame $w.lkern
entry $w.lkern.lkernentry -textvariable tmpGnomPar(LKern,v) \
      -relief sunken -width 12 -highlightthickness 0
label $w.lkern.lkerntext -text "LKERN: Kernel file status (Y/N):"
frame $w.kernel
entry $w.kernel.kernelentry -textvariable tmpGnomPar(Kernel,v) \
      -relief sunken -width 12 -highlightthickness 0
label $w.kernel.kerneltext -text "KERNEL: Kernel-storage file:"
frame $w.jobtyp
entry $w.jobtyp.jobtypentry -textvariable tmpGnomPar(JobTyp,v) \
      -relief sunken -width 12 -highlightthickness 0
label $w.jobtyp.jobtyptext -text "JOBTYP: system type (0/1/2/3/4/5/6):"
frame $w.bckgrd
entry $w.bckgrd.bckgrdentry -textvariable tmpGnomPar(BckGrd,v) \
      -relief sunken -width 12 -highlightthickness 0
label $w.bckgrd.bckgrdtext -text "BCKGRD: background:"
frame $w.rmin
entry $w.rmin.rminentry -textvariable tmpGnomPar(Rmin,v) \
      -relief sunken -width 12 -highlightthickness 0
label $w.rmin.rmintext -text "RMIN: R_min:"
frame $w.rmax
entry $w.rmax.rmaxentry -textvariable tmpGnomPar(Rmax,v) \
      -relief sunken -width 12 -highlightthickness 0
label $w.rmax.rmaxtext -text "RMAX: R_max:"
frame $w.lzrmin
entry $w.lzrmin.lzrminentry -textvariable tmpGnomPar(LZRmin,v) \
      -relief sunken -width 12 -highlightthickness 0
label $w.lzrmin.lzrmintext -text "LZRMIN: zero condition at r=R_min (Y/N):"
frame $w.lzrmax
entry $w.lzrmax.lzrmaxentry -textvariable tmpGnomPar(LZRmax,v) \
      -relief sunken -width 12 -highlightthickness 0
label $w.lzrmax.lzrmaxtext -text "LZRMAX: zero condition at r=R_max (Y/N):"
frame $w.idet
entry $w.idet.idetentry -textvariable tmpGnomPar(Idet,v) \
      -relief sunken -width 12 -highlightthickness 0
label $w.idet.idettext -text "IDET: experimental set up (0/1/2):"
frame $w.alpha
entry $w.alpha.alphaentry -textvariable tmpGnomPar(Alpha,v) \
      -relief sunken -width 12 -highlightthickness 0
label $w.alpha.alphatext -text "ALPHA: initial ALPHA:"
frame $w.nreal
entry $w.nreal.nrealentry -textvariable tmpGnomPar(NReal,v) \
      -relief sunken -width 12 -highlightthickness 0
label $w.nreal.nrealtext -text "NREAL: number of points in r-space:"

pack $w.printer -in $w -anchor e -fill y -expand 1 -padx 1m -pady 1m
pack $w.printer.printerentry $w.printer.printertext -in $w.printer -side right
pack $w.expert -in $w -anchor e -fill y -expand 1 -padx 1m -pady 1m
pack $w.expert.expertentry $w.expert.experttext -in $w.expert -side right
pack $w.input1 -in $w -anchor e -fill y -expand 1 -padx 1m -pady 1m
pack $w.input1.input1entry $w.input1.input1text -in $w.input1 -side right
pack $w.input2 -in $w -anchor e -fill y -expand 1 -padx 1m -pady 1m
pack $w.input2.input2entry $w.input2.input2text -in $w.input2 -side right
pack $w.evaerr -in $w -anchor e -fill y -expand 1 -padx 1m -pady 1m
pack $w.evaerr.evaerrentry $w.evaerr.evaerrtext -in $w.evaerr -side right
pack $w.lkern -in $w -anchor e -fill y -expand 1 -padx 1m -pady 1m
pack $w.lkern.lkernentry $w.lkern.lkerntext -in $w.lkern -side right
pack $w.kernel -in $w -anchor e -fill y -expand 1 -padx 1m -pady 1m
pack $w.kernel.kernelentry $w.kernel.kerneltext -in $w.kernel -side right
pack $w.jobtyp -in $w -anchor e -fill y -expand 1 -padx 1m -pady 1m
pack $w.jobtyp.jobtypentry $w.jobtyp.jobtyptext -in $w.jobtyp -side right
pack $w.bckgrd -in $w -anchor e -fill y -expand 1 -padx 1m -pady 1m
pack $w.bckgrd.bckgrdentry $w.bckgrd.bckgrdtext -in $w.bckgrd -side right
pack $w.rmin -in $w -anchor e -fill y -expand 1 -padx 1m -pady 1m
pack $w.rmin.rminentry $w.rmin.rmintext -in $w.rmin -side right
pack $w.rmax -in $w -anchor e -fill y -expand 1 -padx 1m -pady 1m
pack $w.rmax.rmaxentry $w.rmax.rmaxtext -in $w.rmax -side right
pack $w.lzrmin -in $w -anchor e -fill y -expand 1 -padx 1m -pady 1m
pack $w.lzrmin.lzrminentry $w.lzrmin.lzrmintext -in $w.lzrmin -side right
pack $w.lzrmax -in $w -anchor e -fill y -expand 1 -padx 1m -pady 1m
pack $w.lzrmax.lzrmaxentry $w.lzrmax.lzrmaxtext -in $w.lzrmax -side right
pack $w.idet -in $w -anchor e -fill y -expand 1 -padx 1m -pady 1m
pack $w.idet.idetentry $w.idet.idettext -in $w.idet -side right
pack $w.alpha -in $w -anchor e -fill y -expand 1 -padx 1m -pady 1m
pack $w.alpha.alphaentry $w.alpha.alphatext -in $w.alpha -side right
pack $w.nreal -in $w -anchor e -fill y -expand 1 -padx 1m -pady 1m
pack $w.nreal.nrealentry $w.nreal.nrealtext -in $w.nreal -side right

set w .gnom
frame $w.inputdata.col2 -relief groove -borderwidth 1
pack $w.inputdata.col2 -fill both -expand 1 -in .gnom.inputdata -side right
set w .gnom.inputdata.col2.lay
frame $w
pack $w -in .gnom.inputdata.col2 -fill y -expand 1

frame $w.coef
entry $w.coef.coefentry -textvariable tmpGnomPar(Coef,v) \
      -relief sunken -width 12 -highlightthickness 0
label $w.coef.coeftext -text "COEF: scale factor for second run:"
frame $w.rad56
entry $w.rad56.rad56entry -textvariable tmpGnomPar(RAD56,v) \
      -relief sunken -width 12 -highlightthickness 0
label $w.rad56.rad56text -text "RAD56: radius/thickness (JOB=5,6):"
frame $w.forfac
entry $w.forfac.forfacentry -textvariable tmpGnomPar(ForFac,v) \
      -relief sunken -width 12 -highlightthickness 0
label $w.forfac.forfactext -text "FORFAC: form factor file (JOB=2):"
frame $w.deviat
entry $w.deviat.deviatentry -textvariable tmpGnomPar(Deviat,v) \
      -relief sunken -width 12 -highlightthickness 0
label $w.deviat.deviattext -text "DEVIAT: Default input error level:"
frame $w.fwhm1
entry $w.fwhm1.fwhm1entry -textvariable tmpGnomPar(FWHM1,v) \
      -relief sunken -width 12 -highlightthickness 0
label $w.fwhm1.fwhm1text -text "FWHM1: FWHM for 1st run:"
frame $w.fwhm2
entry $w.fwhm2.fwhm2entry -textvariable tmpGnomPar(FWHM2,v) \
      -relief sunken -width 12 -highlightthickness 0
label $w.fwhm2.fwhm2text -text "FWHM2: FWHM for 2nd run:"
frame $w.ah1
entry $w.ah1.ah1entry -textvariable tmpGnomPar(AH1,v) \
      -relief sunken -width 12 -highlightthickness 0
label $w.ah1.ah1text -text "AH1: slit-height parameter AH (1st run):"
frame $w.lh1
entry $w.lh1.lh1entry -textvariable tmpGnomPar(LH1,v) \
      -relief sunken -width 12 -highlightthickness 0
label $w.lh1.lh1text -text "LH1: slit-height parameter LH (1st run):"
frame $w.aw1
entry $w.aw1.aw1entry -textvariable tmpGnomPar(AW1,v) \
      -relief sunken -width 12 -highlightthickness 0
label $w.aw1.aw1text -text "AW1: slit-width parameter AW (1st run):"
frame $w.lw1
entry $w.lw1.lw1entry -textvariable tmpGnomPar(LW1,v) \
      -relief sunken -width 12 -highlightthickness 0
label $w.lw1.lw1text -text "LW1: slit-width parameter LW (1st run):"
frame $w.ah2
entry $w.ah2.ah2entry -textvariable tmpGnomPar(AH2,v) \
      -relief sunken -width 12 -highlightthickness 0
label $w.ah2.ah2text -text "AH2: slit-height parameter AH (2nd run):"
frame $w.lh2
entry $w.lh2.lh2entry -textvariable tmpGnomPar(LH2,v) \
      -relief sunken -width 12 -highlightthickness 0
label $w.lh2.lh2text -text "LH2: slit-height parameter LH (2nd run):"
frame $w.aw2
entry $w.aw2.aw2entry -textvariable tmpGnomPar(AW2,v) \
      -relief sunken -width 12 -highlightthickness 0
label $w.aw2.aw2text -text "AW2: slit-width parameter AW (2nd run):"
frame $w.lw2
entry $w.lw2.lw2entry -textvariable tmpGnomPar(LW2,v) \
      -relief sunken -width 12 -highlightthickness 0
label $w.lw2.lw2text -text "LW2: slit-width parameter LW (2nd run):"
frame $w.spot1
entry $w.spot1.spot1entry -textvariable tmpGnomPar(Spot1,v) \
      -relief sunken -width 12 -highlightthickness 0
label $w.spot1.spot1text -text "Spot1: beam profile file (1st run):"
frame $w.spot2
entry $w.spot2.spot2entry -textvariable tmpGnomPar(Spot2,v) \
      -relief sunken -width 12 -highlightthickness 0
label $w.spot2.spot2text -text "Spot2: beam profile file (2nd run):"


pack $w.coef -in $w -anchor e -fill y -expand 1 -padx 1m -pady 1m
pack $w.coef.coefentry $w.coef.coeftext -in $w.coef -side right
pack $w.rad56 -in $w -anchor e -fill y -expand 1 -padx 1m -pady 1m
pack $w.rad56.rad56entry $w.rad56.rad56text -in $w.rad56 -side right
pack $w.forfac -in $w -anchor e -fill y -expand 1 -padx 1m -pady 1m
pack $w.forfac.forfacentry $w.forfac.forfactext -in $w.forfac -side right
pack $w.deviat -in $w -anchor e -fill y -expand 1 -padx 1m -pady 1m
pack $w.deviat.deviatentry $w.deviat.deviattext -in $w.deviat -side right
pack $w.fwhm1 -in $w -anchor e -fill y -expand 1 -padx 1m -pady 1m
pack $w.fwhm1.fwhm1entry $w.fwhm1.fwhm1text -in $w.fwhm1 -side right
pack $w.fwhm2 -in $w -anchor e -fill y -expand 1 -padx 1m -pady 1m
pack $w.fwhm2.fwhm2entry $w.fwhm2.fwhm2text -in $w.fwhm2 -side right
pack $w.ah1 -in $w -anchor e -fill y -expand 1 -padx 1m -pady 1m
pack $w.ah1.ah1entry $w.ah1.ah1text -in $w.ah1 -side right
pack $w.lh1 -in $w -anchor e -fill y -expand 1 -padx 1m -pady 1m
pack $w.lh1.lh1entry $w.lh1.lh1text -in $w.lh1 -side right
pack $w.aw1 -in $w -anchor e -fill y -expand 1 -padx 1m -pady 1m
pack $w.aw1.aw1entry $w.aw1.aw1text -in $w.aw1 -side right
pack $w.lw1 -in $w -anchor e -fill y -expand 1 -padx 1m -pady 1m
pack $w.lw1.lw1entry $w.lw1.lw1text -in $w.lw1 -side right
pack $w.ah2 -in $w -anchor e -fill y -expand 1 -padx 1m -pady 1m
pack $w.ah2.ah2entry $w.ah2.ah2text -in $w.ah2 -side right
pack $w.lh2 -in $w -anchor e -fill y -expand 1 -padx 1m -pady 1m
pack $w.lh2.lh2entry $w.lh2.lh2text -in $w.lh2 -side right
pack $w.aw2 -in $w -anchor e -fill y -expand 1 -padx 1m -pady 1m
pack $w.aw2.aw2entry $w.aw2.aw2text -in $w.aw2 -side right
pack $w.lw2 -in $w -anchor e -fill y -expand 1 -padx 1m -pady 1m
pack $w.lw2.lw2entry $w.lw2.lw2text -in $w.lw2 -side right
pack $w.spot1 -in $w -anchor e -fill y -expand 1 -padx 1m -pady 1m
pack $w.spot1.spot1entry $w.spot1.spot1text -in $w.spot1 -side right
pack $w.spot2 -in $w -anchor e -fill y -expand 1 -padx 1m -pady 1m
pack $w.spot2.spot2entry $w.spot2.spot2text -in $w.spot2 -side right


set w .gnom
frame $w.action
$w.action configure -relief groove -borderwidth 1
pack $w.action -fill x
button $w.action.fit -text Fit \
       -command {if {[applyGnomOptionsCmd tmpGnomPar]} {GnomFit} } \
       -highlightthickness 0
button $w.action.apply -text Apply \
       -command {applyGnomOptionsCmd tmpGnomPar} -highlightthickness 0
button $w.action.dismiss -text Cancel -command {destroy .gnom} \
       -highlightthickness 0
button $w.action.help -text "Help..."  \
       -highlightthickness 0

pack $w.action.fit $w.action.apply $w.action.dismiss \
     -side left -padx 1m -pady 1m
pack $w.action.help -side right -fill x -padx 1m -pady 2m
}
