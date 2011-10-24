# sasfit.vfs/lib/app-sasfit/tcl/gdi/testgdi8.tcl
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

#################################################################
# Tcl Extension for Windows
# RCS Version $Revision: 1.4 $
# RCS Last Change Date: $Date: 2000/12/26 05:48:36 $
# Original Author: Michael I. Schwartz, mschwart@nyx.net
# 
# {LICENSE}
# 
# THIS SOFTWARE IS PROVIDED BY THE AUTHOR WITH PERMISSION TO USE, COPY, MODIFY, AND
# DISTRIBUTE IT FOR ANY PURPOSE WITH THE FOLLOWING CONDITIONS:
# 1) IN SOURCE FORM, THIS HEADER MUST BE PRESERVED AND THESE CONDITIONS PROPOGATED
# 2) IN BINARY FORM, THE ORIGINAL AUTHOR MUST BE ACKNOWLEDGED IN DOCUMENTATION AND
#    CREDITS SCREEN (IF ANY)
# 3) FOR ANY COMMERCIAL SALE, THE AUTHOR MUST BE NOTIFIED OF THE USE OF THIS CODE
#    IN A PARTICULAR PRODUCT; THE ORIGINAL AUTHOR MAY CITE 
#    THE COMMERCIAL SELLER AS A BENEFICIARY OF THE CODE.
# 
# 
# IN NO EVENT SHALL THE AUTHOR BE LIABLE TO ANY PARTY FOR DIRECT,
# INDIRECT, SPECIAL, INCIDENTAL,  OR CONSEQUENTIAL DAMAGES ARISING OUT OF
# THE USE OF THIS SOFTWARE, ITS DOCUMENTATION,  OR ANY DERIVATIVES
# THEREOF, EVEN IF THE AUTHORS HAVE BEEN ADVISED OF THE POSSIBILITY OF
# SUCH DAMAGE.
# 
# THE AUTHORS AND DISTRIBUTORS SPECIFICALLY DISCLAIM ANY WARRANTIES,
# INCLUDING, BUT NOT LIMITED TO,  THE IMPLIED WARRANTIES OF
# MERCHANTABILITY,FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT. 
# THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, AND THE AUTHORS AND
# DISTRIBUTORS HAVE NO OBLIGATION  TO PROVIDE MAINTENANCE, SUPPORT,
# UPDATES, ENHANCEMENTS, OR MODIFICATIONS.
################################################################

################################################################
## Somewhat simple proc to parse arguments 
################################################################
proc page_args { array } {
  upvar #0 $array ary

  # First we check whether we have a valid hDC
  # (perhaps we can later make this also an optional argument, defaulting to 
  #  the default printer)
  set attr [ printer attr ]
  foreach attrpair $attr {
    set key [lindex $attrpair 0]
    set val [lindex $attrpair 1]
    switch -exact $key {
      "hDC"       { set ary(hDC) $val }
      "copies"    { if { $val >= 0 } { set ary(copies) $val } }
      "page dimensions" {
                          set wid [lindex $val 0]
                          set hgt [lindex $val 1]
                          if { $wid > 0 } { set ary(pw) $wid }
                          if { $hgt > 0 } { set ary(pl) $hgt }
                        }
      "page margins"    {
                          if { [scan [lindex $val 0] %d tmp] > 0 } {
			    set ary(lm) [ lindex $val 0 ]
			    set ary(tm) [ lindex $val 1 ]
			    set ary(rm) [ lindex $val 2 ]
			    set ary(bm) [ lindex $val 3 ]
			  }
                        }
      "resolution"      {
                          if { [scan [lindex $val 0] %d tmp] > 0 } {
			    set ary(resx) [ lindex $val 0 ]
			    set ary(resy) [ lindex $val 1 ]
			  } else {
			    set ary(resx) 200	;# Set some defaults for this...
			    set ary(resy) 200
			  }
                        }
    }
  }
}

################################################################
## Get the packages we need.
## Since this sample relies on fairly recent changes to 
## GDI, require at least specific versions of the packages.
################################################################
proc init_pkgs { } {
  package require printer 0.7
  package require gdi 0.9
}

# A simple UI, done crudely
proc init_ui { } {
  global status
  global scale
  global tk_version

  if { $tk_version < 8.0 } {
    # Definitely NOT pretty....
    catch {
      eval destroy [winfo ch .]
      destroy .m.menu
    } err
    menubutton .m -text Menu
    menu .m.menu
    menu .m.menu.tests
    menu .m.menu.option
    .m configure -menu .m.menu
    set menu .m.menu
    pack .m -side top -fill x -expand true
  } else {
    catch {
      eval destroy [winfo ch .]
      destroy .menu
    } err
    menu   .menu
    menu   .menu.tests
    menu   .menu.option
    . configure -menu .menu
    set menu .menu
  }

  canvas .c
  frame  .f
  frame  .s

  $menu add cascade -label Tests -menu $menu.tests -underline 0
  $menu add cascade -label Options -menu $menu.option -underline 0

  button .f.b -text OK -command exit
  pack   .f.b -fill both -expand 0

  label  .s.l -textvariable status
  pack   .s.l -fill both -expand 0

  pack .c -side top -fill both -expand true
  pack .s -side bottom -fill x -expand false
  pack .f -side bottom -fill x -expand false

  # Put a little stuff on the canvas
  .c configure -background white
  .c create line 0 0 100 100 -fill green -width 2
  .c create oval 100 100 200 200 -outline red -width 2
  .c create line 200 200 200 0 -fill blue -width 3
  .c create text 150 100 -font { Arial 16 bold } -anchor center -width 2i \
            -text "This is a canvas with a few items on it for testing" -fill magenta
  set status "Printer version: [printer version]\tGdi version: [gdi version]"

  init_ui_optionmenu $menu.option
  init_ui_testmenu   $menu.tests
}

################################################################
## Sets up option menu based on provided data
################################################################
proc init_ui_optionmenu { menu } {
  global scale

  $menu add radiobutton -label 25% -variable scale -value 0.25
  $menu add radiobutton -label 50% -variable scale -value 0.5
  $menu add radiobutton -label 75% -variable scale -value 0.75
  $menu add radiobutton -label 100% -variable scale -value 1.00
  $menu add radiobutton -label 125% -variable scale -value 1.25
  $menu add radiobutton -label 250% -variable scale -value 2.5
  $menu add radiobutton -label 500% -variable scale -value 5.0
  $menu add radiobutton -label "Fill Page"  -variable scale -value 0.0

  set scale 0.0
}

################################################################
## Sets up initial menu
################################################################
proc init_ui_testmenu { menu } {

  $menu add command -label "Print Screen" \
		-command "test_gdi_print {Complete Screen Print} screen "
  $menu add command -label "Print Window" \
		-command "test_gdi_print {Complete Window Print} . "
  $menu add command -label "Print Client Area" \
		-command "test_gdi_print {Client Area Window Print} . 1"
  $menu add command -label "Print Canvas" \
		-command "test_gdi_print {Window Print of Canvas} .c "
  $menu add separator
  $menu add command -label Exit -command exit
}

################################################################
## Start a printer job
################################################################
proc start_print_job { } {
  global hdc
  set hdc [lindex [printer dialog select] 0]
  printer job start -name gdi
  printer page start
}

################################################################
## Finish a printer job and unload the printer driver
################################################################
proc end_print_job { } {
  global hdc
  printer page end
  printer job end
  printer close
  set hdc 0x0
}

################################################################
## Main routine to produce printed pages.
################################################################
proc test_gdi_print { reporttitle {window screen} {client 0} {reporttail "End of print" } } {
  global hdc
  global attr
  global scale
  global status

  # This is not a fully general test, but should get the idea across...
  start_print_job

  page_args attr
  if { [info exist attr(lm)] == 0 } { set attr(lm) 1000 }
  if { [info exist attr(rm)] == 0 } { set attr(rm) 1000 }
  if { [info exist attr(tm)] == 0 } { set attr(tm) 1000 }
  if { [info exist attr(bm)] == 0 } { set attr(bm) 1000 }
  if { [info exist attr(pw)] == 0 } { set attr(pw) 8500 }
  if { [info exist attr(pl)] == 0 } { set attr(pl) 11000 }
  if { [info exist attr(resx)] == 0 } { set attr(resx) 300 }
  if { [info exist attr(resy)] == 0 } { set attr(resy) 300 }

  set tm [expr $attr(tm) / $attr(resy) * 72 ]
  set bm [expr $attr(bm) / $attr(resy) * 72 ]
  set rm [expr $attr(rm) / $attr(resx) * 72 ]
  set lm [expr $attr(lm) / $attr(resx) * 72 ]
  set pw [expr $attr(pw) / $attr(resx) * 72 ]
  set pl [expr $attr(pl) / $attr(resy) * 72 ]

  # First, we'll raise the window of interest, 
  # and update everything so the screen will look OK.
  if { $window != "screen" } {
    raise [ winfo toplevel $window ]
  }
  update

  # Print the title
  set wid [expr $pw ]
  set hgt [gdi text $hdc $lm $tm -text $reporttitle -anchor nw -width $wid -font { Arial 18 } ]

  # Ensure the scaling is correct
  if { $scale <= 0.0 } {
    set maxx [ expr $pw - $rm ]
    set maxy [ expr $pl - $bm ]
    # set dest "$lm [expr $tm + $hgt + 10] $maxx $maxy"
    set dest "$lm [expr $tm + $hgt + 10] $maxx"
    if { $window == "screen" } {
      set retval [gdi copybits $hdc -screen -destination $dest]
      set status "Copied screen to $dest"
    } else {
      if { $client } {
        set retval [gdi copybits $hdc -window $window -destination $dest -client]
      } else {
        set retval [gdi copybits $hdc -window $window -destination $dest]
      }
      set status "Copied window $window to $dest"
    }
  } else {
    # Dump the bitmap
    set dest "$lm [expr $tm + $hgt + 10 ]"
    if { $window == "screen" } {
      set retval [gdi copybits $hdc -screen -destination $dest -scale $scale]
      set status "Copied screen to $dest at scale $scale"
    } else {
      if { $client } {
        set retval [gdi copybits $hdc -window $window -destination $dest -scale $scale -client]
      } else {
        set retval [gdi copybits $hdc -window $window -destination $dest -scale $scale]
      }
      set status "Copied window $window to $dest at scale $scale"
    }
  }

  # Now, if we have retval, let's put out some text below that!
  # A second hgt is not needed below because the top corner of the text is the
  # anchor (nw). The second 10 is used to provide a wee bit of spacing
  set endtexttop [ expr [lindex $retval 3] + $tm + $hgt + 10 + 10 ]
  gdi text $hdc $lm $endtexttop -text $reporttail -anchor nw -width $wid -font { Arial 18 }

  end_print_job
}

################ Do the actual script ################
init_pkgs
init_ui

