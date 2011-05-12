# sasfit.vfs/lib/app-sasfit/tcl/sasfit_xsect.tcl
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


# calculates scattering length densities and cross sections for neutrons
#------------------------------------------------------------------------------
#                            Menu Commands
#
proc SLDCalculator {} {
#^^^^^^^^^^^^^^^^^
global sasfit
global nLengthTable
catch {destroy .sldCalc}
toplevel .sldCalc
set w .sldCalc
wm geometry $w
wm title .sldCalc "SLD calculator"
raise $w
focus $w

#
#reading csv-file with the known scattering lenths of isotopes and elements
#

read_ScattLen "$sasfit(tcl)/ScatteringlengthsTable.csv" nLengthTable

frame $w.labframeinput
frame $w.labframemolecule
frame $w.labframeneutron
frame $w.labframexray 
frame $w.frameinput -relief ridge -bd 2
frame $w.framemolecule -relief groove -bd 2
frame $w.frameneutron -relief ridge -bd 2
frame $w.framexray -relief groove -bd 2
frame $w.action
grid $w.labframeinput -column 0 -row 0 -sticky we
grid $w.frameinput -column 0 -row 1 -sticky we
grid $w.labframemolecule -column 0 -row 2 -sticky we
grid $w.framemolecule -column 0 -row 3 -sticky we
grid $w.labframeneutron -column 0 -row 4 -sticky we
grid $w.frameneutron -column 0 -row 5 -sticky we
grid $w.labframexray -column 0 -row 6 -sticky we
grid $w.framexray -column 0 -row 7 -sticky we
grid $w.action  -column 0 -row 8 -sticky we

set wm $w.frame

set wm $w.labframeinput
label $wm.l_inp -anchor w -text "input values:"  -justify left
label $wm.tit  -text "Scattering Length Density Calculator"
grid $wm.tit -column 0 -row 0 -sticky we
grid $wm.l_inp -column 0 -row 2 -sticky we

set wm $w.frameinput
label $wm.l_chemformula -text compound:
entry $wm.e_chemformula -textvariable nLengthTable(compound)
grid $wm.l_chemformula -column 0 -row 0 -sticky e
grid $wm.e_chemformula -column 1 -row 0 -sticky w

label $wm.l_density -text "density: "
label $wm.u_density -text " g/cm^3"
entry $wm.e_density -textvariable nLengthTable(density)
grid $wm.l_density -column 0 -row 1 -sticky e
grid $wm.e_density -column 1 -row 1 -sticky w
grid $wm.u_density -column 2 -row 1 -sticky w

label $wm.dummy -text "   "
grid $wm.dummy -column 2 -row 0

label $wm.l_energy -text "x-ray energy: "
label $wm.u_energy -text " (1<E/keV<24.9)"
entry $wm.e_energy -textvariable nLengthTable(xrayenergy)
grid $wm.l_energy -column 0 -row 2 -sticky e
grid $wm.e_energy -column 1 -row 2 -sticky w
grid $wm.u_energy -column 2 -row 2 -sticky w

label $wm.l_lambda -text "neutron wavelength: "
label $wm.u_lambda -text " nm"
entry $wm.e_lambda -textvariable nLengthTable(lambda)
grid $wm.l_lambda -column 0 -row 3 -sticky e
grid $wm.e_lambda -column 1 -row 3 -sticky w
grid $wm.u_lambda -column 2 -row 3 -sticky w


set wm $w.labframemolecule
label $wm.l_compoundInfo -anchor w -text "compound info:"
label $wm.space -text ""
grid $wm.space -column 0 -row 0 -sticky w
grid $wm.l_compoundInfo -column 0 -row 1 -sticky w

set wm $w.framemolecule

label $wm.dummy1 -text "   "
grid $wm.dummy1 -column 2 -row 0
label $wm.dummy2 -text "   "
grid $wm.dummy2 -column 5 -row 0

label $wm.l_molecularweight -text "molecular weight M: "
label $wm.u_molecularweight -text " Da"
label $wm.v_molecularweight -textvariable nLengthTable(compound,A)
grid $wm.l_molecularweight -column 0 -row 0 -sticky e
grid $wm.v_molecularweight -column 1 -row 0 -sticky w
grid $wm.u_molecularweight -column 2 -row 0 -sticky w

label $wm.l_molecularvolume -text "molecular volume vm: "
label $wm.u_molecularvolume -text " nm^3"
label $wm.v_molecularvolume -textvariable nLengthTable(compound,vm)
grid $wm.l_molecularvolume -column 0 -row 1 -sticky e
grid $wm.v_molecularvolume -column 1 -row 1 -sticky w
grid $wm.u_molecularvolume -column 2 -row 1 -sticky w

label $wm.l_e -text "Z: "
label $wm.v_e -textvariable nLengthTable(compound,Z)
grid $wm.l_e -column 0 -row 2 -sticky e
grid $wm.v_e -column 1 -row 2 -sticky w

set wm $w.labframeneutron
label $wm.l_neutronInfo -anchor w -text "neutron cross-sect. data:"
label $wm.space -text ""
grid $wm.space -column 0 -row 0 -sticky w
grid $wm.l_neutronInfo -column 0 -row 1 -sticky w

set wm $w.frameneutron
label $wm.l_bc -text "bc: "
label $wm.u_bc -text " fm"
label $wm.v_bc -textvariable nLengthTable(compound,b_coh)
grid $wm.l_bc -column 0 -row 0 -sticky e
grid $wm.v_bc -column 1 -row 0 -sticky w
grid $wm.u_bc -column 2 -row 0 -sticky w

label $wm.l_SLD -text "SLD: "
label $wm.u_SLD -text " cm^-2"
label $wm.v_SLD -textvariable nLengthTable(compound,SLD)
grid $wm.l_SLD -column 0 -row 1 -sticky e
grid $wm.v_SLD -column 1 -row 1 -sticky w
grid $wm.u_SLD -column 2 -row 1 -sticky w

set wm $w.labframexray
label $wm.l_xrayInfo -anchor w -text "x-ray cross-sect. data:"
label $wm.space -text ""
grid $wm.space -column 0 -row 0 -sticky w
grid $wm.l_xrayInfo -column 0 -row 1 -sticky w

set wm $w.framexray

if {1 == 2} {
set rowoff 0 
label $wm.l_Crfp -text "f'_Cr(5989eV):"
label $wm.v_Crfp -textvariable nLengthTable(compound,CrKafp,5989eV)
grid $wm.l_Crfp -column 0 -row [expr $rowoff+1] -sticky e
grid $wm.v_Crfp -column 1 -row [expr $rowoff+1] -sticky w -ipadx 5

label $wm.l_Crfpp -text "f''_Cr(5989eV):"
label $wm.v_Crfpp -textvariable nLengthTable(compound,CrKafpp,5989eV)
grid $wm.l_Crfpp -column 2 -row [expr $rowoff+1] -sticky e
grid $wm.v_Crfpp -column 3 -row [expr $rowoff+1] -sticky w -ipadx 5

label $wm.l_etaCr -text "eta(5989eV) in cm^-2:"
label $wm.v_etaCr -textvariable nLengthTable(compound,SLD,Cr)
grid $wm.l_etaCr -column 4 -row [expr $rowoff+1] -sticky e
grid $wm.v_etaCr -column 5 -row [expr $rowoff+1] -sticky w -ipadx 5

label $wm.l_Fefp -text "f'_Fe(7112eV):"
label $wm.v_Fefp -textvariable nLengthTable(compound,FeKafp,7112eV)
grid $wm.l_Fefp -column 0 -row [expr $rowoff+2] -sticky e
grid $wm.v_Fefp -column 1 -row [expr $rowoff+2] -sticky w -ipadx 5

label $wm.l_Fefpp -text "f''_Fe(7112eV):"
label $wm.v_Fefpp -textvariable nLengthTable(compound,FeKafpp,7112eV)
grid $wm.l_Fefpp -column 2 -row [expr $rowoff+2] -sticky e
grid $wm.v_Fefpp -column 3 -row [expr $rowoff+2] -sticky w -ipadx 5

label $wm.l_etaFe -text "eta(7112eV) in cm^-2:"
label $wm.v_etaFe -textvariable nLengthTable(compound,SLD,Fe)
grid $wm.l_etaFe -column 4 -row [expr $rowoff+2] -sticky e
grid $wm.v_etaFe -column 5 -row [expr $rowoff+2] -sticky w -ipadx 5

label $wm.l_Cufp -text "f'_Cu(8979eV):"
label $wm.v_Cufp -textvariable nLengthTable(compound,CuKafp,8979eV)
grid $wm.l_Cufp -column 0 -row [expr $rowoff+3] -sticky e
grid $wm.v_Cufp -column 1 -row [expr $rowoff+3] -sticky w -ipadx 5

label $wm.l_Cufpp -text "f''_Cu(8979eV):"
label $wm.v_Cufpp -textvariable nLengthTable(compound,CuKafpp,8979eV)
grid $wm.l_Cufpp -column 2 -row [expr $rowoff+3] -sticky e
grid $wm.v_Cufpp -column 3 -row [expr $rowoff+3] -sticky w -ipadx 5

label $wm.l_etaCu -text "eta(8979eV) in cm^-2:"
label $wm.v_etaCu -textvariable nLengthTable(compound,SLD,Cu)
grid $wm.l_etaCu -column 4 -row [expr $rowoff+3] -sticky e
grid $wm.v_etaCu -column 5 -row [expr $rowoff+3] -sticky w -ipadx 5

label $wm.l_Mofp -text "f'_Mo(20000eV):"
label $wm.v_Mofp -textvariable nLengthTable(compound,MoKafp,20000eV)
grid $wm.l_Mofp -column 0 -row [expr $rowoff+4] -sticky e
grid $wm.v_Mofp -column 1 -row [expr $rowoff+4] -sticky w -ipadx 5

label $wm.l_Mofpp -text "f''_Mo(20000eV):"
label $wm.v_Mofpp -textvariable nLengthTable(compound,MoKafpp,20000eV)
grid $wm.l_Mofpp -column 2 -row [expr $rowoff+4] -sticky e
grid $wm.v_Mofpp -column 3 -row [expr $rowoff+4] -sticky w -ipadx 5

label $wm.l_etaMo -text "eta(20000eV) in cm^-2:"
label $wm.v_etaMo -textvariable nLengthTable(compound,SLD,Mo)
grid $wm.l_etaMo -column 4 -row [expr $rowoff+4] -sticky e
grid $wm.v_etaMo -column 5 -row [expr $rowoff+4] -sticky w -ipadx 5

label $wm.l_Agfp -text "f'_Ag(25514eV):"
label $wm.v_Agfp -textvariable nLengthTable(compound,AgKafp,25514eV)
grid $wm.l_Agfp -column 0 -row [expr $rowoff+5] -sticky e
grid $wm.v_Agfp -column 1 -row [expr $rowoff+5] -sticky w -ipadx 5

label $wm.l_Agfpp -text "f''_Ag(25514eV):"
label $wm.v_Agfpp -textvariable nLengthTable(compound,AgKafpp,25514eV)
grid $wm.l_Agfpp -column 2 -row [expr $rowoff+5] -sticky e
grid $wm.v_Agfpp -column 3 -row [expr $rowoff+5] -sticky w -ipadx 5

label $wm.l_etaAg -text "eta(25514eV) in cm^-2:"
label $wm.v_etaAg -textvariable nLengthTable(compound,SLD,Ag)
grid $wm.l_etaAg -column 4 -row [expr $rowoff+5] -sticky e
grid $wm.v_etaAg -column 5 -row [expr $rowoff+5] -sticky w -ipadx 5

label $wm.l_Efp -text "f'(E):"
label $wm.v_Efp -textvariable nLengthTable(compound,fpE)
grid $wm.l_Efp -column 0 -row [expr $rowoff+6] -sticky e
grid $wm.v_Efp -column 1 -row [expr $rowoff+6] -sticky w -ipadx 5

label $wm.l_Efpp -text "f''(E):"
label $wm.v_Efpp -textvariable nLengthTable(compound,fppE)
grid $wm.l_Efpp -column 2 -row [expr $rowoff+6] -sticky e
grid $wm.v_Efpp -column 3 -row [expr $rowoff+6] -sticky w -ipadx 5

label $wm.l_etaE -text "eta(E) in cm^-2:"
label $wm.v_etaE -textvariable nLengthTable(compound,SLD,E)
grid $wm.l_etaE -column 4 -row [expr $rowoff+6] -sticky e
grid $wm.v_etaE -column 5 -row [expr $rowoff+6] -sticky w -ipadx 5
} else {
label $wm.l_Efp -text "f'(E): "
label $wm.v_Efp -textvariable nLengthTable(compound,fpE)
label $wm.u_Efp -text " e.u.:"
grid $wm.l_Efp -column 0 -row 1 -sticky e
grid $wm.v_Efp -column 1 -row 1 -sticky w 
grid $wm.u_Efp -column 2 -row 1 -sticky w 

label $wm.l_Efpp -text "f''(E): "
label $wm.v_Efpp -textvariable nLengthTable(compound,fppE)
label $wm.u_Efpp -text " e.u.:"
grid $wm.l_Efpp -column 0 -row 2 -sticky e
grid $wm.v_Efpp -column 1 -row 2 -sticky w
grid $wm.u_Efpp -column 2 -row 2 -sticky w

label $wm.l_etaE -text "SLD(E): "
label $wm.v_etaE -textvariable nLengthTable(compound,SLD,E)
label $wm.u_etaE -text " cm^-2:"
grid $wm.l_etaE -column 0 -row 3 -sticky e
grid $wm.v_etaE -column 1 -row 3 -sticky w
grid $wm.u_etaE -column 2 -row 3 -sticky w
}

set wm $w.action
button $w.action.calc -text calculate -command {
    global nLengthTable
    if {[eval_ChemSumFormula $nLengthTable(compound) nLengthTable]} {
	tk_messageBox -icon error \
		       -title "SLD calculation error"\
		       -message "chemecal formula could  not be interpreted"\
		       -type ok
    }
}
button $wm.ok -text exit -command {destroy .sldCalc}

pack $wm.calc $wm.ok -side left
}




proc eval_ChemSumFormula {ChemSumForm CSV} {
upvar $CSV Data

set m_u 1.660538782E-24  ;#in g
set Na 6.02214179E23     ;#in mol^-1
set re 2.8179402894E-13  ;#cm

#
# removing all empty spaces
#
while {[regsub  {(.*) (.*)} $ChemSumForm {\1\2} res]} {
   set ChemSumForm $res
}
#
# if Sum Formular starts with a number return an error (1)
#
set firstChar [string index $ChemSumForm 0]
while {[regsub  {[0-9]+} $firstChar {\1} res]} {
   return 1
}

#
# mark all natural elements with (n)
#
while {[regsub  {([A-Z][a-z]?)([0-9]+)} $ChemSumForm {\1(n)\2} res]} {
   set ChemSumForm $res
}
while {[regsub  {([A-Z][a-z]?)$} $ChemSumForm {\1(n)1} res]} {
   set ChemSumForm $res
}
while {[regsub  {([A-Z][a-z]?)([A-Z])} $ChemSumForm {\1(n)1\2} res]} {
   set ChemSumForm $res
}
while {[regsub  {([A-Z][a-z]?)(\))} $ChemSumForm {\1(n)1\2} res]} {
   set ChemSumForm $res
}
while {[regsub  {([A-Z][a-z]?\([0-9]?\))(\))} $ChemSumForm {\11\2} res]} {
   set ChemSumForm $res
}
while {[regsub  {([A-Z][a-z]?\([0-9]?\))(\()} $ChemSumForm {\11\2} res]} {
   set ChemSumForm $res
}
while {[regsub  {(\(.+\))(\(.+\))} $ChemSumForm {\1+\2} res]} {
   set ChemSumForm $res
}

#
# extend formula if Sy(i) -> Sy(i)1
#
while {[regsub  {([A-Z][a-z]?)(\([0-9]*n?\))([A-Z]+)} $ChemSumForm {\1\21\3} res]} {
   set ChemSumForm $res
}
while {[regsub  {([A-Z][a-z]?)(\([0-9]*n?\))$} $ChemSumForm {\1\21} res]} {
   set ChemSumForm $res
}

while {[regsub  {(\(.*\))([0-9])} $ChemSumForm {\1*\2} res]} {
   set ChemSumForm $res
}

while {[regsub  {([0-9])(\(.*)} $ChemSumForm {\1+\2} res]} {
   set ChemSumForm $res
}

set saveChemSumForm $ChemSumForm
#
# calculate overall scatterng length of the molecule
#
foreach name $Data(Symbol) A $Data(A) Re_bc_coh  $Data(Re_bc_coh) {
    set Symb [format %s%s $name \\($A\\)]
    while {[regsub  [format "%s%s%s" {(.*)} $Symb {(.*)}] $ChemSumForm \\1+($Re_bc_coh)\\2 res]} {
       set ChemSumForm $res
   }
}

set Data(ChemSumFormula,b_coh) $ChemSumForm
if {[catch {set Data(compound,b_coh) [expr $ChemSumForm]}]} {
   return 1
}

#
# calculate total charge Z of the molecule
#
set ChemSumForm $saveChemSumForm
foreach name $Data(Symbol) A $Data(A) Z  $Data(Z) {
    set Symb [format %s%s $name \\($A\\)]
    while {[regsub  [format "%s%s%s" {(.*)} $Symb {(.*)}] $ChemSumForm \\1+($Z)\\2 res]} {
       set ChemSumForm $res
   }
}

set Data(ChemSumFormula,Z) $res
if {[catch {set Data(compound,Z) [expr $ChemSumForm]}]} {
   return 1
}

#
# calculate total atomic weight A of the molecule
#
set ChemSumForm $saveChemSumForm
foreach name $Data(Symbol) A  $Data(A) u $Data(u) {
    set Symb [format %s%s $name \\($A\\)]
    while {[regsub  [format "%s%s%s" {(.*)} $Symb {(.*)}] $ChemSumForm \\1+($u)\\2 res]} {
       set ChemSumForm $res
   }
}
set Data(compound,A) $res
if {[catch {set Data(compound,A) [expr $ChemSumForm]}]} {
   return 1
} 
set Data(compound,A) [format %3.5f $Data(compound,A)]

#
# calculates x-ray scattering lengths
# Cr K_alpha f' (E=5989eV)
set ChemSumForm $saveChemSumForm
foreach name $Data(Symbol) fp  $Data(CrKafp,5989eV) A $Data(A) {
    set Symb [format %s%s $name \\($A\\)]
    while {[regsub  [format "%s%s%s" {(.*)} $Symb {(.*)}] $ChemSumForm \\1+($fp)\\2 res]} {
       set ChemSumForm $res
   }
}
set Data(compound,CrKafp,5989eV) $res
if {[catch {set Data(compound,CrKafp,5989eV) [expr $ChemSumForm]}]} {
   return 1
} 

#
# calculates x-ray scattering lengths
# Cr K_alpha f'' (E=5989eV)
set ChemSumForm $saveChemSumForm
foreach name $Data(Symbol) fpp  $Data(CrKafpp,5989eV) A $Data(A) {
    set Symb [format %s%s $name \\($A\\)]
    while {[regsub  [format "%s%s%s" {(.*)} $Symb {(.*)}] $ChemSumForm \\1+($fpp)\\2 res]} {
       set ChemSumForm $res
   }
}
set Data(compound,CrKafpp,5989eV) $res
if {[catch {set Data(compound,CrKafpp,5989eV) [expr $ChemSumForm]}]} {
   return 1
} 


#
# calculates x-ray scattering lengths
# Fe K_alpha f' (E=7112eV)
set ChemSumForm $saveChemSumForm
foreach name $Data(Symbol) fp  $Data(FeKafp,7112eV) A $Data(A) {
    set Symb [format %s%s $name \\($A\\)]
    while {[regsub  [format "%s%s%s" {(.*)} $Symb {(.*)}] $ChemSumForm \\1+($fp)\\2 res]} {
       set ChemSumForm $res
   }
}
set Data(compound,FeKafp,7112eV) $res
if {[catch {set Data(compound,FeKafp,7112eV) [expr $ChemSumForm]}]} {
   return 1
} 

#
# calculates x-ray scattering lengths
# Fe K_alpha f'' (E=7112eV)
set ChemSumForm $saveChemSumForm
foreach name $Data(Symbol) fpp  $Data(FeKafpp,7112eV) A $Data(A) {
    set Symb [format %s%s $name \\($A\\)]
    while {[regsub  [format "%s%s%s" {(.*)} $Symb {(.*)}] $ChemSumForm \\1+($fpp)\\2 res]} {
       set ChemSumForm $res
   }
}
set Data(compound,FeKafpp,7112eV) $res
if {[catch {set Data(compound,FeKafpp,7112eV) [expr $ChemSumForm]}]} {
   return 1
} 

#
# calculates x-ray scattering lengths
# Cu K_alpha f' (E=8979eV)
set ChemSumForm $saveChemSumForm
foreach name $Data(Symbol) fp  $Data(CuKafp,8979eV) A $Data(A) {
    set Symb [format %s%s $name \\($A\\)]
    while {[regsub  [format "%s%s%s" {(.*)} $Symb {(.*)}] $ChemSumForm \\1+($fp)\\2 res]} {
       set ChemSumForm $res
   }
}
set Data(compound,CuKafp,8979eV) $res
if {[catch {set Data(compound,CuKafp,8979eV) [expr $ChemSumForm]}]} {
   return 1
} 

#
# calculates x-ray scattering lengths
# Cu K_alpha f'' (E=8979eV)
set ChemSumForm $saveChemSumForm
foreach name $Data(Symbol) fpp  $Data(CuKafpp,8979eV) A $Data(A) {
    set Symb [format %s%s $name \\($A\\)]
    while {[regsub  [format "%s%s%s" {(.*)} $Symb {(.*)}] $ChemSumForm \\1+($fpp)\\2 res]} {
       set ChemSumForm $res
   }
}
set Data(compound,CuKafpp,8979eV) $res
if {[catch {set Data(compound,CuKafpp,8979eV) [expr $ChemSumForm]}]} {
   return 1
} 


#
# calculates x-ray scattering lengths
# Mo K_alpha f' (E=20000eV)
set ChemSumForm $saveChemSumForm
foreach name $Data(Symbol) fp  $Data(MoKafp,20000eV) A $Data(A) {
    set Symb [format %s%s $name \\($A\\)]
    while {[regsub  [format "%s%s%s" {(.*)} $Symb {(.*)}] $ChemSumForm \\1+($fp)\\2 res]} {
       set ChemSumForm $res
   }
}
set Data(compound,MoKafp,20000eV) $res
if {[catch {set Data(compound,MoKafp,20000eV) [expr $ChemSumForm]}]} {
   return 1
} 

#
# calculates x-ray scattering lengths
# Mo K_alpha f'' (E=20000eV)
set ChemSumForm $saveChemSumForm
foreach name $Data(Symbol) fpp  $Data(MoKafpp,20000eV) A $Data(A) {
    set Symb [format %s%s $name \\($A\\)]
    while {[regsub  [format "%s%s%s" {(.*)} $Symb {(.*)}] $ChemSumForm \\1+($fpp)\\2 res]} {
       set ChemSumForm $res
   }
}
set Data(compound,MoKafpp,20000eV) $res
if {[catch {set Data(compound,MoKafpp,20000eV) [expr $ChemSumForm]}]} {
   return 1
} 


#
# calculates x-ray scattering lengths
# Ag K_alpha f' (E=25514eV)
set ChemSumForm $saveChemSumForm
foreach name $Data(Symbol) fp  $Data(AgKafp,25514eV) A $Data(A) {
    set Symb [format %s%s $name \\($A\\)]
    while {[regsub  [format "%s%s%s" {(.*)} $Symb {(.*)}] $ChemSumForm \\1+($fp)\\2 res]} {
       set ChemSumForm $res
   }
}
set Data(compound,AgKafp,25514eV) $res
if {[catch {set Data(compound,AgKafp,25514eV) [expr $ChemSumForm]}]} {
   return 1
} 

#
# calculates x-ray scattering lengths
# Ag K_alpha f'' (E=25514eV)
set ChemSumForm $saveChemSumForm
foreach name $Data(Symbol) fpp  $Data(AgKafpp,25514eV) A $Data(A) {
    set Symb [format %s%s $name \\($A\\)]
    while {[regsub  [format "%s%s%s" {(.*)} $Symb {(.*)}] $ChemSumForm \\1+($fpp)\\2 res]} {
       set ChemSumForm $res
   }
}
set Data(compound,AgKafpp,25514eV) $res
if {[catch {set Data(compound,AgKafpp,25514eV) [expr $ChemSumForm]}]} {
   return 1
} 

#
# calculates x-ray scattering lengths
# for the given energy
# 
set ChemSumForm $saveChemSumForm
foreach name $Data(Symbol) A $Data(A) {
    set Symb [format %s%s $name \\($A\\)]
    if {[regexp "$name" "$ChemSumForm"]} {
#puts "fp: $ChemSumForm $name $Data(xrayenergy)"
	set fp [lindex [get_fpfpp $Data(xrayenergy) $name] 0]
	while {[regsub  [format "%s%s%s" {(.*)} $Symb {(.*)}] $ChemSumForm \\1+($fp)\\2 res]} {
	    set ChemSumForm $res
	}
    }
}
set Data(compound,fpE) $res
if {[catch {set Data(compound,fpE) [expr $ChemSumForm]}]} {
   return 1
} 

#
# calculates x-ray scattering lengths
# for the given energy
#
set ChemSumForm $saveChemSumForm
foreach name $Data(Symbol) A $Data(A) {
    set Symb [format %s%s $name \\($A\\)]
    if {[regexp "$name" "$ChemSumForm"]} {
#puts "fpp: $ChemSumForm $name $Data(xrayenergy)"
	set fpp [lindex [get_fpfpp $Data(xrayenergy) $name] 1]
	while {[regsub  [format "%s%s%s" {(.*)} $Symb {(.*)}] $ChemSumForm \\1+($fpp)\\2 res]} {
	    set ChemSumForm $res
	}
    }
}
set Data(compound,fppE) $res
if {[catch {set Data(compound,fppE) [expr $ChemSumForm]}]} {
   return 1
} 


if {![catch {set vm [expr $Data(compound,A)/($Na*$Data(density))]}]} {
    set Data(compound,SLD) [format %1.5g [expr $Data(compound,b_coh)*1e-13/$vm]]
    set Data(compound,vm) [format %1.5g [expr $vm*1e21]]
} else {
   return 1
}

set Data(compound,SLD,Cr) [format "%1.3E + i %1.3E" [expr ($Data(compound,Z)-pow($Data(compound,Z)/82.5,2.37)+$Data(compound,CrKafp,5989eV))*$re/$vm] [expr $Data(compound,CrKafpp,5989eV)*$re/$vm]]

set Data(compound,SLD,Fe) [format "%1.3E + i %1.3E" [expr ($Data(compound,Z)-pow($Data(compound,Z)/82.5,2.37)+$Data(compound,FeKafp,7112eV))*$re/$vm] [expr $Data(compound,FeKafpp,7112eV)*$re/$vm]]

set Data(compound,SLD,Cu) [format "%1.3E + i %1.3E" [expr ($Data(compound,Z)-pow($Data(compound,Z)/82.5,2.37)+$Data(compound,CuKafp,8979eV))*$re/$vm] [expr $Data(compound,CuKafpp,8979eV)*$re/$vm]]

set Data(compound,SLD,Mo) [format "%1.3E + i %1.3E" [expr ($Data(compound,Z)-pow($Data(compound,Z)/82.5,2.37)+$Data(compound,MoKafp,20000eV))*$re/$vm] [expr $Data(compound,MoKafpp,20000eV)*$re/$vm]]

set Data(compound,SLD,Ag) [format "%1.3E + i %1.3E" [expr ($Data(compound,Z)-pow($Data(compound,Z)/82.5,2.37)+$Data(compound,AgKafp,25514eV))*$re/$vm] [expr $Data(compound,AgKafpp,25514eV)*$re/$vm]]

set Data(compound,SLD,E) [format "%1.3E + i %1.3E" [expr ($Data(compound,Z)-pow($Data(compound,Z)/82.5,2.37)+$Data(compound,fpE))*$re/$vm] [expr $Data(compound,fppE)*$re/$vm]]

return 0
}


proc get_fpfpp {xe Symbol} {
    global sasfit
    if {[info exists sasfit(fp,$Symbol,$xe)]} {
	return [list $sasfit(fp,$Symbol,$xe) $sasfit(fpp,$Symbol,$xe)]
    }
    set E [expr $xe*1000]
    create_ASCIIData Dat
# puts "$Symbol: $E"
#    puts $sasfit(tcl)/x_as/${Symbol}.dat
    if {($xe<=1) || ($xe >= 24.9)} {
	error "enery not between 1-24.9keV"
    }
    if {![file exists $sasfit(tcl)/x_as/${Symbol}.dat]} {
	error "unkown element: $Symbol"
    }
    set ret [catch {read_Ascii "$sasfit(tcl)/x_as/${Symbol}.dat" Dat} msg]
    set indx 0
    while {([lindex $Dat(Q) $indx] <= $E) && ([llength $Dat(Q)]>$indx) } {
	incr indx
    }
    if {[llength $Dat(Q)]<$indx} {
	error "no data for $Symbol at $xe keV"
    }
    set E1  [lindex $Dat(Q) [expr $indx-1]]
    set E2  [lindex $Dat(Q) [expr $indx]]
    set fp1 [lindex $Dat(I) [expr $indx-1]]
    set fp2 [lindex $Dat(I) [expr $indx]]
    set fpp1 [lindex $Dat(DI) [expr $indx-1]]
    set fpp2 [lindex $Dat(DI) [expr $indx]]

    set fp  [expr ($E-$E1)*($fp2 -$fp1 )/($E2-$E1)+$fp1 ]
    set fpp [expr ($E-$E1)*($fpp2-$fpp1)/($E2-$E1)+$fpp1]
    set sasfit(fp,$Symbol,$xe) $fp 
    set sasfit(fpp,$Symbol,$xe) $fpp
    return [list $fp $fpp]
}