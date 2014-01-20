# sasfit.vfs/lib/app-sasfit/tcl/gdi/testgdi2.tcl
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

#
package require printer
package require gdi

# Now build a screen....
button .b -text OK -font { Times 12 } -command exit
label  .x -text "Now is the time for all good men to come to the aid of their country" \
          -font { Times 12 bold } -wraplength 100

# Build an output window
toplevel .output
text .output.text
pack .output.text -fill both -expand true

.output.text insert end "Printer package version [printer version]\n"
.output.text insert end "GDI package version [gdi version]\n"

pack .b .x

# Ensure everything is ready
update

wm deiconify .
raise .

set x [printer dialog select]

if { [lindex $x 1] != 1 } {
  # Printing was cancelled!
  .output.text insert end "Cancelled printing."
} else {
  # get just the HDC part
  set x [ lindex $x 0 ]

  .output.text insert end "Selected printer with GDI context $x\n"

  # Allow a repaint
  update

  catch { set result [printer job start $x] } result

  .output.text insert end "Started printer job: $result\n"

  wm deiconify .
  raise .
  update

  catch { set result [gdi copybits $x -destination "100 100 1000 800" ] } result

  .output.text insert end "Copied window using StretchBlt: $result\n"

  catch { set result [printer job end $x] } result

  .output.text insert end "Completed job: $result\n"

  catch { set result [printer close] } result

  .output.text insert end "Closed printer: $result\n"

}

.output.text insert end "\nTest complete. Press OK to exit\n"

