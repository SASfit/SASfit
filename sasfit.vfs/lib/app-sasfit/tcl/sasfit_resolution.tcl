# sasfit.vfs/lib/app-sasfit/tcl/sasfit_resolution.tcl
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

#------------------------------------------------------------------------------
# procedure set_resolutionPar open menu for parameter input of resolution
# parameters.
#     input paremeters:
#
#          The array resolution is a global variable and should be set do
#          the default values before this procedure is called.
#          All lengths are by default in millimeters. At the beginning the
#          collimation length and sample detector distance are converted in
#          meters and at the end back into millimeters.
#          The wavelength resolution Dlambda is also first transformed in a
#          dimensionless relative value Dlambda/lambda and transformed back
#          at the end.
#          The sample aperture r1 and n_guide width r2 are multiplied at the
#          beginning width 2 (radius -> diameter, half width -> width) and
#          at the end divided by 2.
# 
#             resolution(r1)      : half width of the neutron guide (in mm)
#             resolution(r2)      : radius of sample aperture (in mm)
#             resolution(d)       : detector resolution (in mm)
#             resolution(Dd)      : step size of radial average (in mm)
#             resolution(lambda)  : wavelength (in nm)
#             resolution(Dlambda) : wavelength resolution Dlambda/lambda (FWHM)
#             resolution(l1)      : collimation length (in m)
#             resolution(l2)      : sample detector distance (in m)
#             resolution(Q)       : Q-values
#             filename            : name of the data file, for which the resolution
#                                   parameters are read in.
#
#             side effect         : the procedure is running in "grab -force"-mode
#
proc set_resolutionPar {filename} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
set w .resolution_input
global resolution tmpres
cp_arr resolution tmpres
if {$tmpres(lambda) != 0} {
   set tmpres(Dlambda) [expr $tmpres(Dlambda)/$tmpres(lambda)]
} else {
   set tmpres(Dlambda) 0.1
}
set tmpres(l1) [expr $tmpres(l1)/1000.0]
set tmpres(l2) [expr $tmpres(l2)/1000.0]

toplevel $w
wm geometry $w
wm title $w "Resolution: [file tail $filename]"
raise $w
focus $w
grab set $w
frame  $w.lay1 -relief ridge -borderwidth 2
frame  $w.lay2 -relief ridge -borderwidth 2
pack   $w.lay1 $w.lay2 -expand yes -fill both -side top -ipadx 2m -ipady 2m

label  $w.lay1.n_guide_label                -text "radius of source size (e.g. neutron guide exit) \[mm\]:"
label  $w.lay1.sample_aperture_label        -text "radius of sample aperture \[mm\]:"
label  $w.lay1.collimation_label            -text "collimation length \[m\]:"
label  $w.lay1.sample_detector_label        -text "sample<->detector distance \[m\]:"
label  $w.lay1.wavelength_label             -text "wavelength lambda \[in reciprocal units of Q\]:"
label  $w.lay1.wavelength_resolution_label  -text "wavelength spread Dlambda/lambda (FWHM):"
label  $w.lay1.detector_resolution_label    -text "detector pixel size \[mm\]:"
label  $w.lay1.radial_average_step_label    -text "radial average step size \[mm\]:"
entry  $w.lay1.n_guide_entry \
       -textvariable tmpres(r1) -width 10
entry  $w.lay1.sample_aperture_entry \
       -textvariable tmpres(r2) -width 10 
entry  $w.lay1.collimation_entry \
       -textvariable tmpres(l1) -width 10
entry  $w.lay1.sample_detector_entry \
       -textvariable tmpres(l2) -width 10
entry  $w.lay1.wavelength_entry \
       -textvariable tmpres(lambda) -width 10
entry  $w.lay1.wavelength_resolution_entry \
       -textvariable tmpres(Dlambda) -width 10 
entry  $w.lay1.detector_resolution_entry \
       -textvariable tmpres(d) -width 10 
entry  $w.lay1.radial_average_step_entry \
       -textvariable tmpres(Dd) -width 10

set tw $w.lay1

grid $tw.n_guide_label               -sticky e -column 0 -row 0 -padx 1m
grid $tw.sample_aperture_label       -sticky e -column 0 -row 1 -padx 1m
grid $tw.collimation_label           -sticky e -column 0 -row 2 -padx 1m
grid $tw.sample_detector_label       -sticky e -column 0 -row 3 -padx 1m

grid $tw.n_guide_entry               -sticky w -column 1 -row 0 -padx 1m
grid $tw.sample_aperture_entry       -sticky w -column 1 -row 1 -padx 1m
grid $tw.collimation_entry           -sticky w -column 1 -row 2 -padx 1m
grid $tw.sample_detector_entry       -sticky w -column 1 -row 3 -padx 1m

grid $tw.wavelength_label            -sticky e -column 0 -row 4 -padx 1m
grid $tw.wavelength_resolution_label -sticky e -column 0 -row 5 -padx 1m
grid $tw.detector_resolution_label   -sticky e -column 0 -row 6 -padx 1m
grid $tw.radial_average_step_label   -sticky e -column 0 -row 7 -padx 1m

grid $tw.wavelength_entry            -sticky w -column 1 -row 4 -padx 1m
grid $tw.wavelength_resolution_entry -sticky w -column 1 -row 5 -padx 1m
grid $tw.detector_resolution_entry   -sticky w -column 1 -row 6 -padx 1m
grid $tw.radial_average_step_entry   -sticky w -column 1 -row 7 -padx 1m

grid columnconfigure $tw 0 -pad 2m
grid columnconfigure $tw 1 -pad 2m
button $w.lay2.ok -text OK -highlightthickness 0 \
                  -command { global resolution tmpres
                             set tmpres(Dlambda) [expr $tmpres(Dlambda)*$tmpres(lambda) ]
                             set tmpres(l1)      [expr $tmpres(l1)*1000.0               ]
                             set tmpres(l2)      [expr $tmpres(l2)*1000.0               ]
                             cp_arr tmpres resolution
                             set widcnt $resolution(widcnt)
                             set tmpsasfit(file,r1) \
                                 [lreplace $tmpsasfit(file,r1) \
                                           $widcnt $widcnt $resolution(r1)      ]
                             set tmpsasfit(file,r2) \
                                 [lreplace $tmpsasfit(file,r2) \
                                           $widcnt $widcnt $resolution(r2)      ]
                             set tmpsasfit(file,l1) \
                                 [lreplace $tmpsasfit(file,l1) \
                                           $widcnt $widcnt $resolution(l1)      ]
                             set tmpsasfit(file,l2) \
                                 [lreplace $tmpsasfit(file,l2) \
                                           $widcnt $widcnt $resolution(l2)      ]
                             set tmpsasfit(file,lambda) \
                                 [lreplace $tmpsasfit(file,lambda) \
                                           $widcnt $widcnt $resolution(lambda)  ]
                             set tmpsasfit(file,Dlambda) \
                                 [lreplace $tmpsasfit(file,Dlambda) \
                                           $widcnt $widcnt $resolution(Dlambda) ]
                             set tmpsasfit(file,d) \
                                 [lreplace $tmpsasfit(file,d) \
                                           $widcnt $widcnt $resolution(d)       ]
                             set tmpsasfit(file,Dd) \
                                 [lreplace $tmpsasfit(file,Dd) \
                                           $widcnt $widcnt $resolution(Dd)      ]
                             set tmpsasfit(file,res,calc) \
                                 [lreplace $tmpsasfit(file,res,calc) $widcnt $widcnt \
                                           [sasfit_res $resolution(lambda)  \
                                                       $resolution(Dlambda) \
                                                       $resolution(r1)      \
                                                       $resolution(l1)      \
                                                       $resolution(r2)      \
                                                       $resolution(l2)      \
                                                       $resolution(d)       \
                                                       $resolution(Dd)      \
                                                       $resolution(Q)       \
                                           ]\
                                 ]
                             destroy .resolution_input
                             grab .addfile
                             raise .addfile
                             focus .addfile}
button $w.lay2.cancel -text Cancel -command {destroy .resolution_input
                                             grab .addfile 
                                             raise .addfile
                                             focus .addfile} \
       -highlightthickness 0
button $w.lay2.help -text "Help..."  -highlightthickness 0
pack $w.lay2.ok  $w.lay2.cancel \
     -side left  -fill x -padx 4
pack $w.lay2.help \
     -side right -fill x -padx 4
}
