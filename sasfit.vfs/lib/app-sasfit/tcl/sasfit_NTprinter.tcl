# sasfit.vfs/lib/app-sasfit/tcl/sasfit_NTprinter.tcl
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


proc init_printer_globals { } {
  global printer_vars
  set printer_vars(OS)  $::tcl_platform(platform)
  set printer_vars(printer_name) ""
  set printer_vars(have_hdc) 0
  set printer_vars(have_gdi) 0
}

init_printer_globals

proc init_printer { } {
  global printer_vars
  global sasfit
   if { "$printer_vars(OS)" == "windows" && 
	 [ catch { load $sasfit(tcl)/printer/printer.dll } result ] } {
	puts stderr "error while loading $sasfit(tcl)/printer/printer.dll: $result"
 #     return $result
   }

  set printer_vars(have_gdi) 1
  set printer_vars(have_hdc) 1
  set printer_vars(have_wmf) 1

   if {[string compare windows $printer_vars(OS)] == 0} {
      if {[ catch { load $sasfit(tcl)/hdc/hdc.dll } err ] != 0} {
         set printer_vars(have_hdc) 0
         set printer_vars(have_gdi) 0k
         puts stderr "error while loading $sasfit(tcl)/hdc/hdc.dll: $err"
      } else {
        if {[ catch { load $sasfit(tcl)/gdi/gdi.dll } err ] != 0} {
           set printer_vars(have_gdi) 0
           puts stderr "error while loading $sasfit(tcl)/gdi/gdi.dll: $err"
        } 
        if {[ catch { load $sasfit(tcl)/wmf/wmf.dll } err ] != 0} {
           set printer_vars(have_wmf) 0
           puts stderr "error while loading $sasfit(tcl)/wmf/wmf.dll: $err"
        } 
      }
  }

#  set printer_vars(have_gdi) 1
#  set printer_vars(have_hdc) 1
#  set printer_vars(OS) NT
#  set printer_vars(have_gdi_print) 0

  if { $printer_vars(have_gdi) == 1 } {
    # Try to get the prntcanv procedure
    if [ catch { source $sasfit(tcl)/printer/prntcanv.tcl } err ] {
      set printer_vars(have_gdi_print) 0
      puts stderr "Can't get the prntcanv procedure: $err"
    } else {
      set printer_vars(have_gdi_print) 1
    }
  } else {
    set printer_vars(have_gdi_print) 0
  }
  return 1
}

proc get_default_printer { } {
  printer open
  set attr [ printer attr -get device ]
  do_printer_attributes
  return [ lindex [ lindex $attr 0 ] 1]
}

proc do_select_printer { } {
  global printer_vars
  global sasfit
  set retval [printer dialog select]
  set stat [ lindex $retval 1]
  set hdc  [ lindex $retval 0]
  
  if { $stat == 0 } {
    set printer_vars(status) "Canceled printer selection"
  } else {
    set attr [ printer attr -get device ]
    set printer_vars(current) [ lindex [ lindex $attr 0 ] 1]
    do_printer_attributes
    set pd1 [expr round([lindex $printer_vars(page\ dimensions) 0] *0.0254)]
    set pd2 [expr round([lindex $printer_vars(page\ dimensions) 1] *0.0254)]
    set printer_vars(page\ dimensions) [list $pd1 $pd2] 
    set sasfit(print,paperwidth) $pd1
    set sasfit(print,paperheight) $pd2
  }
}

proc do_setup_printer { } {
  global printer_vars
  set retval [printer dialog page_setup -flag 0x00000004]
  set stat [ lindex $retval 1]
  
  if { $stat == 0 } {
    set printer_vars(status) "Canceled printer page setup"
  } else {
    set printer_vars(status) "Updated page setup"
    do_printer_attributes
  }
}

proc do_list_printers { } {
  global tcl_patchLevel

  # list in alphabetical order
  global printer_vars
  if { [string index $tcl_patchLevel 0] == 7 } {
    set printer_vars(list) [join [lsort [printer list]] \n]
  } else {
    set printer_vars(list) [join [lsort -index 0 [printer list]] \n]
  }
}

proc do_select_and_print { } {
  # For now, no differences==this requires a change in prntcanv.tcl
  do_print
}

proc do_print { } {
  global printer_vars
  set attr [ printer attr -get device ]
  set printer_vars(current) [ lindex [ lindex $attr 0 ] 1]
  set printer_vars(status) "Printing to $printer_vars(current)"
  printer send -postscript -file sasfit.ps
}

proc printer_about { } {
  global printer_vars
  if $printer_vars(have_gdi) {
    set printer_vars(status) "printer version: [printer version]\ngdi version: [gdi version]"
  } else {
    set printer_vars(status) "printer version: [printer version]\ngdi version: Not found"
  }
}

proc do_printer_attributes { } {
  global printer_vars
  set x [ printer attr ]
  foreach pair $x {
    set tag [ lindex $pair 0 ]
    set val [ lindex $pair 1 ]
    set printer_vars($tag) $val
  }
}

if { [string match "Windows*" $::env(OS)] \
 && ![string match "*64" $::env(PROCESSOR_ARCHITECTURE)] } {
  init_printer
}
if {[string compare $printer_vars(OS) windows] == 0} {
   printer_about
   do_list_printers
   if [ catch { get_default_printer } name ] {
     set printer_vars(default) "No default printer"
     set printer_vars(current) "No current printer" 
   } else { 
     set printer_vars(default) [join $name] 
     set printer_vars(current) $printer_vars(default)
    set pd1 [expr round([lindex $printer_vars(page\ dimensions) 0] *0.0254)]
    set pd2 [expr round([lindex $printer_vars(page\ dimensions) 1] *0.0254)]
    set printer_vars(page\ dimensions) [list $pd1 $pd2] 
   }
}
