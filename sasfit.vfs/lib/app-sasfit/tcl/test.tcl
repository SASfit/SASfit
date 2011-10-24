# sasfit.vfs/lib/app-sasfit/tcl/test.tcl
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

cp_arr sasfit tmpsasfit
catch {destroy .openfile}
catch {destroy .addfile}
if {[catch {set  ReadSuccess [ReadClipboardCmd tmpsasfit]}]} {
   tk_messageBox -icon error \
                 -message "no valid ASCII Data in Clpboard"
} else {
if {[string compare $ReadSuccess no] == 0} {
   set tmpsasfit(file,Q)   {}
   set tmpsasfit(file,I)   {}
   set tmpsasfit(file,DI)  {}
   set tmpsasfit(file,res) {}
   lappend tmpsasfit(file,Q)   $tmpsasfit(Q)
   lappend tmpsasfit(file,I)   $tmpsasfit(I)
   lappend tmpsasfit(file,DI)  $tmpsasfit(DI)
   lappend tmpsasfit(file,res) $tmpsasfit(res)
   set tmpsasfit(file,widcnt) 0
   set tmpsasfit(file,n) 1
   set tmpsasfit(file,name)  {}
   lappend tmpsasfit(file,name) "data from clipboard"
   set tmpsasfit(file,widcnt)     0
   set tmpsasfit(file,divisor)   {1}
   set tmpsasfit(file,firstskip) {0}
   set tmpsasfit(file,lastskip)  {0}
   set tmpsasfit(file,hide)      {no}
   set tmpsasfit(file,r1)        [list $resolution(r1)]
   set tmpsasfit(file,r2)        [list $resolution(r2)]
   set tmpsasfit(file,l1)        [list $resolution(l1)]
   set tmpsasfit(file,l2)        [list $resolution(l2)]
   set tmpsasfit(file,lambda)    [list $resolution(lambda)]
   set tmpsasfit(file,Dlambda)   [list $resolution(Dlambda)]
   set tmpsasfit(file,d)         [list $resolution(d)]
   set tmpsasfit(file,Dd)        [list $resolution(Dd)]
   set tmpsasfit(file,widname)   {{.addfile.lay2 0}}
   catch {destroy .openfile}
} else {
   tk_messageBox -icon error \
                 -message "no valid ASCII Data in Clpboard"
} 
          
cp_arr tmpsasfit sasfit
NewDataCmd sasfit
}
