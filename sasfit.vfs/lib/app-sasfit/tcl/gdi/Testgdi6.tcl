# sasfit.vfs/lib/app-sasfit/tcl/gdi/Testgdi6.tcl
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
#   Ingo Bressler (ingo@cs.tu-berlin.de)

package require printer
package require gdi

puts "Printer version: [printer version]"
puts "Gdi version: [gdi version]"
set retval [printer dialog select]
set status [ lindex $retval 1 ]
set hdc    [ lindex $retval 0 ]

if { $status == 0 } {
  error "Print dialog canceled. Not continuing with this test"
}

printer job start -name gdi

printer page start

# GDI STUFF HERE
# Page is about 2000 by 3000

# arcs
gdi arc $hdc 100 100 200 300 -extent -90.0 -start -45.0 -style pieslice -width 10
gdi arc $hdc 300 120 200 300 -extent 50.0 -start -145.0 -style chord -width 4
gdi arc $hdc 500 140 200 300 -extent 190.0 -start -245.0 -style arc -width 1

# bitmap
# image
# line
gdi line $hdc 2000 3000 1800 3000 1800 2800 1600 2800 1600 2400 -width 12

# oval
gdi oval $hdc 1000 1000 1200 1300 -width 4

# polygon
gdi polygon $hdc 1000 2000 800 2000 800 1800 600 1800 600 1400 -width 15

# rectangle
gdi rectangle $hdc 1600 1600 1800 2100 -width 2

# text
gdi text $hdc 700 100 -text "People are strange when you're a stranger" -font {Arial 10 bold}
gdi text $hdc 700 400 -text "Plasticine porters with looking glass ties" -width 150 -font {{Times New Roman} 10 medium}
gdi text $hdc 700 700 -text "Plasticine porters with looking glass ties" -width 150 -justify center -font {Braggadocio 10}
gdi text $hdc 700 1100 -text "Plasticine porters with looking glass ties" -width 150 -justify right -font {Rembrandt 10 black}

# Now, text with anchors.
gdi rectangle $hdc 1300 100 1450 490
set hgt [gdi text $hdc 1300 100 -text "Plasticine porters with looking glass ties" -width 150 -anchor nw]
set bot [expr 100 + $hgt]	;# Text is anchored top, so this should be at bottom
gdi line $hdc 1300 $bot 1450 $bot -fill red -width 5
gdi rectangle $hdc 1300 500 1450 890
gdi text $hdc 1300 500 -text "Plasticine porters with looking glass ties" -width 150 -anchor center
gdi rectangle $hdc 1300 900 1450 1290
set hgt [gdi text $hdc 1300 900 -text "Plasticine porters with looking glass ties" -width 150 -anchor s]
set bot [expr 1290 - $hgt]	;# Text is anchored bottom, so this should be at top
gdi line $hdc 1300 $bot 1450 $bot -fill red -width 5
gdi rectangle $hdc 1300 1300 1450 1690 -fill yellow
gdi text $hdc 1300 1300 -text "Plasticine porters with looking glass ties" -width 150 -anchor se

# Now, a "ruler-like" application for measurement

for { set i 100 } { $i < 10000 } { incr i 500 } {
  for { set j 100 } { $j < 10000 } { incr j 500 } {
    gdi oval $hdc [ expr $i - 8 ] [ expr $j - 8 ] [ expr $i + 8 ] [ expr $j + 8 ]
    gdi text $hdc $i [ expr $j + 8 ] -text "$i,$j" -anchor n -font "Arial 7"
  }
}

# Check various color specifications
# Some "system" colors
gdi rectangle $hdc 100 2800 300 3000 -width 12 -outline systemButtonFace -fill systemActiveCaption
# Some X crayola colors
gdi rectangle $hdc 500 2800 700 3000 -width 12 -outline YellowGreen -fill DeepPink
# Some RGB colors
gdi rectangle $hdc  900 2800 1100 3000 -width 12 -outline "#FF0" -fill "#0FF"
gdi rectangle $hdc 1300 2800 1500 3000 -width 12 -outline "#FFFF00" -fill "#00FFFF"
gdi rectangle $hdc 1700 2800 1900 3000 -width 12 -outline "#FFFFFF000" -fill "#000FFFFFF"
gdi rectangle $hdc 2100 2800 2300 3000 -width 12 -outline "#FFFFFFFF0000" -fill "#0000FFFFFFFF"

# END GDI STUFF

printer page end

printer job end

printer close

if { $tcl_interactive == 0 } {
  exit
}
