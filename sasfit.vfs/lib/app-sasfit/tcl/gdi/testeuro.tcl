# sasfit.vfs/lib/app-sasfit/tcl/gdi/testeuro.tcl
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
set hdc [printer dialog select]

if { [lindex $hdc 1] == 0 } {
  # Print canceled
  return
}
set hdc [lindex $hdc 0]

printer job start -name gdi

printer page start

# GDI STUFF HERE
# Page is about 2000 by 3000

set text1 "European users have unusual characters like Â‰ˆ≈ƒ÷"

# text
gdi text $hdc 700 200 -text $text1 -font {Arial 10 bold} -anchor w
gdi text $hdc 700 600 -text $text1 -width 250 -font { {Times New Roman} 12 bold} -anchor w
gdi text $hdc 700 1000 -text $text1 -width 200 -font {Arial 12} -anchor c -justify center
gdi text $hdc 700 1400 -text $text1 -font { {Times New Roman} 10 } -anchor c
# END GDI STUFF

printer page end

printer job end

printer close

if { $tcl_interactive == 0 } {
  exit
}
