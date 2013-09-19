# sasfit.vfs/lib/app-sasfit/tcl/wmf/wnd2clip.tcl
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

#!/usr/local/bin/wish8.2
#
# This routine puts a given window on the clipboard
#
package require wmf
package require gdi

# Haven't tracked it down yet, but without the -scale argument,
# the clipboard ends up being blank?!
proc window_to_clipboard { window } {
  raise [winfo toplevel $window]
  update
  set emfdc [wmf open]
  set size [gdi copybits $emfdc -window $window -calc]
  eval gdi copybits $emfdc -window $window -source [ list $size ] -scale 1.01
  set emf [wmf close $emfdc]
  wmf copy $emf
  wmf delete $emf
}
