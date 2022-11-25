# sasfit.vfs/lib/app-sasfit/tcl/sasfit_print.tcl
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
# create a postscript file by the the Tcl-command stored in the variable 
# "pscommand" and a) copy it into a user defined file or b) pipes it
# to a user defined OS command in case you are not on an NT machine or c)
# send it directly to a Windows NT printer. 
#

proc create_ps_and_send_it_to_printer {postscript_command} {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
global sasfit
global printer_vars
if { [winfo exists .print_layout] } {destroy .print_layout}
toplevel .print_layout
set w .print_layout
global sf
cp_arr sasfit sf
set sf(ps_create_cmd) $postscript_command
wm geometry $w
wm title $w $postscript_command

#
# choice if postscript output is printed to a file or is piped to 
# a user defined OS command
#
if {[string compare $printer_vars(OS) windows] == 0} {
frame $w.printcommand -relief groove -borderwidth 1
   pack $w.printcommand -fill x 
frame $w.printcommand.lay1
frame $w.printcommand.lay2
frame $w.printcommand.lay3
   pack $w.printcommand.lay1 $w.printcommand.lay2 $w.printcommand.lay3 \
        -fill x -side top -padx 2m
button $w.printcommand.lay1.printsetup \
          -text "Select Print ..." -highlightthickness 0 \
          -command {
             global print_vars
             do_select_printer
          }
   pack $w.printcommand.lay1.printsetup \
         -side right -padx 2m
label $w.printcommand.lay1.l1 -text "device:" -width 7 -anchor w
label $w.printcommand.lay1.l2 -textvariable printer_vars(device) \
         -padx 1m -pady 1m
   pack $w.printcommand.lay1.l1 $w.printcommand.lay1.l2 \
         -side left
label $w.printcommand.lay2.l1 -text "driver:" -width 7 -anchor w
label $w.printcommand.lay2.l2 -textvariable printer_vars(driver)
   pack $w.printcommand.lay2.l1 $w.printcommand.lay2.l2 \
        -side left

#   button $w.printcommand.lay3.printsetup \
#          -text "Select Print ..." -highlightthickness 0 \
#          -command {
#             global print_vars
#             do_setup_printer
#          }
#  pack $w.printcommand.lay3.printsetup \
#         -side right
label $w.printcommand.lay3.l1 -text "page dimensions \[mm\]:" -anchor w
label $w.printcommand.lay3.l2 -textvariable printer_vars(page\ dimensions) \
         -padx 1m -pady 1m -anchor w
   pack $w.printcommand.lay3.l1 $w.printcommand.lay3.l2 \
         -side left
} else {
frame $w.printcommand -relief groove -borderwidth 1
   pack $w.printcommand -fill x 
frame $w.printcommand.lay1
frame $w.printcommand.lay2
frame $w.printcommand.lay3
   pack $w.printcommand.lay1 $w.printcommand.lay2 $w.printcommand.lay3 \
        -fill x -expand yes -side top  -anchor w
label $w.printcommand.lay1.label -text "Print to:" -width 13 -anchor w
radiobutton $w.printcommand.lay1.printer -text Printer -value Printer \
              -variable sf(print,printto) -highlightthickness 0 \
               -command {
                  .print_layout.printcommand.lay3.file configure \
                      -state disabled
                  .print_layout.printcommand.lay3.file configure \
                     -fg #a3a3a3
                  .print_layout.printcommand.lay3.browse configure \
                     -state disabled
                  .print_layout.printcommand.lay3.browse configure \
                     -fg #a3a3a3
                  .print_layout.printcommand.lay2.command configure \
                     -state normal
                  .print_layout.printcommand.lay2.command configure \
                     -fg Black
                }
radiobutton $w.printcommand.lay1.file -text File -value File \
               -variable sf(print,printto) -highlightthickness 0 \
               -command {
                  .print_layout.printcommand.lay3.file configure \
                     -state normal
                  .print_layout.printcommand.lay3.file configure \
                     -fg Black
                  .print_layout.printcommand.lay3.browse configure \
                     -state normal
                  .print_layout.printcommand.lay3.browse configure \
                     -fg Black
                  .print_layout.printcommand.lay2.command configure \
                     -state disabled
                  .print_layout.printcommand.lay2.command configure \
                     -fg #a3a3a3
                }
   pack $w.printcommand.lay1.label $w.printcommand.lay1.printer \
        $w.printcommand.lay1.file -side left -padx 1m -pady 1m

label $w.printcommand.lay2.label -text "Print Command:" -width 13 -anchor w
   entry $w.printcommand.lay2.command -width 30 -relief sunken \
         -textvariable sf(print,command)
   pack $w.printcommand.lay2.label $w.printcommand.lay2.command \
        -side left -padx 1m -pady 1m

label $w.printcommand.lay3.label -text "File Name:" -width 13 -anchor w
   entry $w.printcommand.lay3.file -width 16 -relief sunken \
      -textvariable sf(print,file)
button $w.printcommand.lay3.browse -text "Browse..." -highlightthickness 0\
        -command {
           global sf
           set psfile [tk_getSaveFile -parent .print_layout \
                                      -title "Select Print File" \
                                      -initialdir "$sf(datadir)" \
                                      -defaultextension ".ps" \
                                      -filetypes {{"Print File" ".ps"} \
                                                  {"All"        ".*"}} 
                      ]
          if { [string length $psfile] != 0 } {
               set sf(print,file) $psfile
	       set sf(datadir) [file dirname $psfile] 
             }       
         }
   if { [string compare $sf(print,printto) Printer] == 0 } { 
      .print_layout.printcommand.lay1.printer invoke 
   } else {
   .print_layout.printcommand.lay1.file invoke
   }
   pack $w.printcommand.lay3.label $w.printcommand.lay3.file \
        $w.printcommand.lay3.browse -side left -padx 1m -pady 1m
}

#
# here the postscript options like color, page dimensions, orientation,
# and offsets are set
#
frame $w.printoptions -relief groove -borderwidth 1
pack $w.printoptions -fill both -expand 1 -ipadx 2m -ipady 2m

label $w.printoptions.colortext -text "Color:"
radiobutton $w.printoptions.mono -text mono -value mono \
            -variable sf(print,colormode) -highlightthickness 0
radiobutton $w.printoptions.gray -text gray -value gray \
            -variable sf(print,colormode) -highlightthickness 0
radiobutton $w.printoptions.color -text color -value color \
            -variable sf(print,colormode) -highlightthickness 0
grid $w.printoptions.colortext -column 0 -row 0 -sticky e -pady 1m
grid $w.printoptions.mono      -column 1 -row 0 -sticky w -pady 1m
grid $w.printoptions.gray      -column 2 -row 0 -sticky w -pady 1m
grid $w.printoptions.color     -column 3 -row 0 -sticky w -pady 1m

label $w.printoptions.unitstext -text "Units:"
radiobutton $w.printoptions.inch -text in -value i \
            -variable sf(print,units) -highlightthickness 0 
radiobutton $w.printoptions.cm   -text cm -value c \
            -variable sf(print,units) -highlightthickness 0
radiobutton $w.printoptions.mm   -text mm -value m \
            -variable sf(print,units) -highlightthickness 0
radiobutton $w.printoptions.pt   -text pt -value p \
            -variable sf(print,units) -highlightthickness 0
grid $w.printoptions.unitstext -column 0 -row 1 -sticky e -pady 1m
grid $w.printoptions.inch      -column 1 -row 1 -sticky w -pady 1m
grid $w.printoptions.cm        -column 2 -row 1 -sticky w -pady 1m
grid $w.printoptions.mm        -column 3 -row 1 -sticky w -pady 1m
grid $w.printoptions.pt        -column 4 -row 1 -sticky w -pady 1m

label $w.printoptions.x1label -text "padx1:"
entry $w.printoptions.x1 -width 5 -relief sunken \
      -textvariable sf(print,padx1)
label $w.printoptions.x2label -text "padx2:"
entry $w.printoptions.x2 -width 5 -relief sunken \
      -textvariable sf(print,padx2)
grid $w.printoptions.x1label -column 0 -row 2 -sticky e -pady 1m
grid $w.printoptions.x1      -column 1 -row 2 -sticky w -padx 2m -pady 1m
grid $w.printoptions.x2label -column 2 -row 2 -sticky e -pady 1m
grid $w.printoptions.x2      -column 3 -row 2 -sticky w -padx 2m -pady 1m

label $w.printoptions.y1label -text "pady1:"
entry $w.printoptions.y1 -width 5 -relief sunken \
      -textvariable sf(print,pady1)
label $w.printoptions.y2label -text "pady2:"
entry $w.printoptions.y2 -width 5 -relief sunken \
      -textvariable sf(print,pady2)
grid $w.printoptions.y1label -column 0 -row 3 -sticky e -pady 1m
grid $w.printoptions.y1      -column 1 -row 3 -sticky w -padx 2m -pady 1m
grid $w.printoptions.y2label -column 2 -row 3 -sticky e -pady 1m
grid $w.printoptions.y2      -column 3 -row 3 -sticky w -padx 2m -pady 1m

label $w.printoptions.ix1label -text "ipadx1:"
entry $w.printoptions.ix1 -width 5 -relief sunken \
      -textvariable sf(print,ipadx1)
label $w.printoptions.ix2label -text "ipadx2:"
entry $w.printoptions.ix2 -width 5 -relief sunken \
      -textvariable sf(print,ipadx2)
grid $w.printoptions.ix1label -column 0 -row 4 -sticky e -pady 1m
grid $w.printoptions.ix1      -column 1 -row 4 -sticky w -padx 2m -pady 1m
grid $w.printoptions.ix2label -column 2 -row 4 -sticky e -pady 1m
grid $w.printoptions.ix2      -column 3 -row 4 -sticky w -padx 2m -pady 1m

label $w.printoptions.iy1label -text "ipady1:"
entry $w.printoptions.iy1 -width 5 -relief sunken \
      -textvariable sf(print,ipady1)
label $w.printoptions.iy2label -text "ipady2:"
entry $w.printoptions.iy2 -width 5 -relief sunken \
      -textvariable sf(print,ipady2)
grid $w.printoptions.iy1label -column 0 -row 5 -sticky e -pady 1m
grid $w.printoptions.iy1      -column 1 -row 5 -sticky w -padx 2m -pady 1m
grid $w.printoptions.iy2label -column 2 -row 5 -sticky e -pady 1m
grid $w.printoptions.iy2      -column 3 -row 5 -sticky w -padx 2m -pady 1m

if {[string compare $printer_vars(OS) windows] != 0} {
frame $w.printoptions.paplay
ComboBox $w.printoptions.paplay.paper -textvariable sf(print,paper) \
         -width 9  -editable no -highlightthickness 0 \
         -label "Size:" -labelwidth 5 -labelanchor w \
         -values $sf(print,papername) \
         -modifycmd {
             global sf
             set sf(print,width) [lindex $sasfit(print,paperxsize) \
                  [.print_layout.printoptions.paplay.paper getvalue]]
             set sf(print,height) [lindex $sasfit(print,paperysize) \
                  [.print_layout.printoptions.paplay.paper getvalue]]
         }
set sf(print,width)  [lindex $sasfit(print,paperxsize) \
                      [.print_layout.printoptions.paplay.paper getvalue]]
set sf(print,height) [lindex $sasfit(print,paperysize) \
                      [.print_layout.printoptions.paplay.paper getvalue]]
label $w.printoptions.paplay.dimlab -text " size \[mm\]: "
label $w.printoptions.paplay.xdim -textvariable sf(print,width)
label $w.printoptions.paplay.ydim -textvariable sf(print,height)
pack $w.printoptions.paplay.paper $w.printoptions.paplay.dimlab $w.printoptions.paplay.xdim  $w.printoptions.paplay.ydim -anchor w -fill x -expand yes -side left
grid $w.printoptions.paplay -column 0 -columnspan 5 -row 6 -sticky w -pady 1m
}

#
# here the buttons "Print", "Cancel" and "Help..." are defined
#
frame $w.printactions -relief groove -borderwidth 1
pack $w.printactions -fill x -ipadx 1m -ipady 2m
button $w.printactions.print -text Print -highlightthickness 0 \
       -command {
        global sasfit sf
        global printer_vars
        set pd1 $sasfit(print,paperwidth) 
        set pd2 $sasfit(print,paperheight)
        cp_arr sf sasfit
        set sasfit(print,paperwidth) $pd1
        set sasfit(print,paperheight) $pd2
        $sasfit(ps_create_cmd)
        if {[string compare $printer_vars(OS) windows] == 0} {
           catch {printer job start}
           catch {printer send -postscipt -file [file join $sasfit(datadir) $sasfit(print,tmpps)]}
           catch {printerv job end}
           file delete [file join $sasfit(datadir) $sasfit(print,tmpps)]
        } else {
           if { [string compare $sasfit(print,printto) Printer] == 0 } { 
              set command "exec $sasfit(print,command) [file join $sasfit(datadir) $sasfit(print,tmpps)] >& /dev/null"
              catch [eval $command]
              file delete [file join $sasfit(datadir) $sasfit(print,tmpps)]
           } else {
              set tmpdir [file dirname $sasfit(print,file)]
	      if {[string compare $tmpdir "."] == 0 } {
                   set sasfit(print,file) [file join $sasfit(datadir) $sasfit(print,file)] 
              } 
           }
       }
       destroy .print_layout
       }
button $w.printactions.cancel -text Cancel -highlightthickness 0 \
       -command {
           destroy .print_layout
       }
button $w.printactions.help -text "Help..." -highlightthickness 0 \
       -command {
        tk_messageBox -message "in work"
       }
pack $w.printactions.print $w.printactions.cancel -side left -padx 2m -pady 2m
pack $w.printactions.help -side right -padx 2m -pady 2m
}

proc print_IQGraph {} {
global IQGraph
print_bltGraph IQGraph
}

proc print_GlobalFitIQGraph {} {
global GlobalFitIQGraph
print_bltGraph GlobalFitIQGraph
}

proc print_ResIQGraph {} {
global ResIQGraph
print_bltGraph ResIQGraph
}

proc print_SDGraph {} {
global SDGraph
print_bltGraph SDGraph
}

proc print_bltGraph {bltGraph} {
upvar $bltGraph Graph
global IQGraph, ResIQGraph, SDGraph
global sasfit

   set ix1 $sasfit(print,ipadx1)
   set ix2 $sasfit(print,ipadx2)
   set iy1 $sasfit(print,ipady1)
   set iy2 $sasfit(print,ipady2)
   set x1  $sasfit(print,padx1)
   set x2  $sasfit(print,padx2)
   set y1  $sasfit(print,pady1)
   set y2  $sasfit(print,pady2)
   switch  $sasfit(print,units) {
      m   {
            set W   $sasfit(print,paperwidth)
            set H   $sasfit(print,paperheight)
          }
      c  {
            set W   [expr $sasfit(print,paperwidth)  / 10.0]
            set H   [expr $sasfit(print,paperheight) / 10.0]
          }
      i   {
            set W   [expr $sasfit(print,paperwidth)  / 25.4]
            set H   [expr $sasfit(print,paperheight) / 25.4]
          }
      p   {
            set W   [expr $sasfit(print,paperwidth)  / 25.4*72.0]
            set H   [expr $sasfit(print,paperheight) / 25.4*72.0]
          }
   }

   set w [expr ($W-$x1-$x2)/1.0 - $ix1 -$ix2]
   set h [expr ($H-$y1-$y2)/1.0 - $iy1 -$iy2]

   $Graph(w) postscript output [file join $sasfit(datadir) $sasfit(print,tmp2ps)] \
        -colormode $sasfit(print,colormode) \
        -landscape yes \
        -paperheight $H$sasfit(print,units) \
        -paperwidth  $W$sasfit(print,units) \
        -height [expr $w/2.0]$sasfit(print,units) \
        -width  [expr $h/2.0]$sasfit(print,units) \
        -maxpect yes
   canvas .printtmp -width  $W$sasfit(print,units)  \
                    -height $H$sasfit(print,units)

   set xpos [expr $x1+$ix1]
   set ypos [expr $y1+$iy1]

   .printtmp create eps $xpos$sasfit(print,units) $ypos$sasfit(print,units) \
             -width  $w$sasfit(print,units) \
             -height $h$sasfit(print,units) \
             -anchor nw -file [file join $sasfit(datadir) $sasfit(print,tmp2ps)]

   .printtmp postscript \
             -file [file join $sasfit(datadir) $sasfit(print,tmpps)] \
             -width  $W$sasfit(print,units) \
             -height $H$sasfit(print,units) \
             -pagewidth  $W$sasfit(print,units) \
             -pageheight $H$sasfit(print,units) \
             -pageanchor sw \
             -pagex 0m -pagey 0m
destroy .printtmp
file delete [file join $sasfit(datadir) $sasfit(print,tmp2ps)]
}

proc print_All {} {

   global IQGraph ResIQGraph SDGraph
   global AnalytPar StructParData
   global sasfit

   set ix1 $sasfit(print,ipadx1)
   set ix2 $sasfit(print,ipadx2)
   set iy1 $sasfit(print,ipady1)
   set iy2 $sasfit(print,ipady2)
   set x1  $sasfit(print,padx1)
   set x2  $sasfit(print,padx2)
   set y1  $sasfit(print,pady1)
   set y2  $sasfit(print,pady2)
   switch  $sasfit(print,units) {
      m   {
            set W   $sasfit(print,paperwidth)
            set H   $sasfit(print,paperheight)
          }
      c  {
            set W   [expr $sasfit(print,paperwidth)  / 10.0]
            set H   [expr $sasfit(print,paperheight) / 10.0]
          }
      i   {
            set W   [expr $sasfit(print,paperwidth)  / 25.4]
            set H   [expr $sasfit(print,paperheight) / 25.4]
          }
      p   {
            set W   [expr $sasfit(print,paperwidth)  / 25.4*72.0]
            set H   [expr $sasfit(print,paperheight) / 25.4*72.0]
          }
   }

   set w [expr ($W-$x1-$x2)/2.0 - $ix1 -$ix2]
   set h [expr ($H-$y1-$y2)/3.0 - $iy1 -$iy2]

   $IQGraph(w) postscript output \
        [file join $sasfit(datadir) $sasfit(print,IQGraph_ps)] \
        -colormode $sasfit(print,colormode) \
        -landscape yes \
        -height $w$sasfit(print,units) \
        -width  $h$sasfit(print,units) \
        -maxpect yes
   $ResIQGraph(w) postscript output \
        [file join $sasfit(datadir) $sasfit(print,ResIQGraph_ps)] \
        -colormode $sasfit(print,colormode) \
        -landscape yes \
        -height $w$sasfit(print,units) \
        -width  $h$sasfit(print,units) \
        -maxpect yes
   $SDGraph(w) postscript output \
        [file join $sasfit(datadir) $sasfit(print,SDGraph_ps)] \
        -colormode $sasfit(print,colormode) \
        -landscape yes \
        -height $w$sasfit(print,units) \
        -width  $h$sasfit(print,units) \
        -maxpect yes
   $AnalytPar(w_MomentsAnalytPar) postscript \
        -file [file join $sasfit(datadir) $sasfit(print,Moments_ps)] \
        -rotate yes \
        -pagewidth $w$sasfit(print,units)
   $AnalytPar(w_AnalytPar) postscript \
        -file [file join $sasfit(datadir) $sasfit(print,AnalytPar_ps)] \
        -rotate yes \
        -pagewidth $w$sasfit(print,units)
   $StructParData(w_StructParData) postscript \
        -file [file join $sasfit(datadir) $sasfit(print,StructParData_ps)] \
        -rotate yes \
        -width [$StructParData(w_StructParData) cget -width] \
        -height [$StructParData(w_StructParData) cget -height] \
        -pagewidth $w$sasfit(print,units)


   canvas .printtmp -width  $W$sasfit(print,units)  \
                    -height $H$sasfit(print,units)

   set xpos [expr $x1+    $ix1                ]
   set ypos [expr $y1+3.0*$iy1+2.0*$iy2+2.0*$h]

   .printtmp create eps $xpos$sasfit(print,units) $ypos$sasfit(print,units) \
             -width  $w$sasfit(print,units) \
             -height $h$sasfit(print,units) \
             -anchor nw \
             -file [file join $sasfit(datadir) $sasfit(print,IQGraph_ps)]

   set xpos [expr $x1+2.0*$ix1+    $ix2+    $w]
   set ypos [expr $y1+3.0*$iy1+2.0*$iy2+2.0*$h]

   .printtmp create eps $xpos$sasfit(print,units) $ypos$sasfit(print,units) \
             -width  $w$sasfit(print,units) \
             -height $h$sasfit(print,units) \
             -anchor nw \
             -file [file join $sasfit(datadir) $sasfit(print,StructParData_ps)]

   set xpos [expr $x1+    $ix1        ]
   set ypos [expr $y1+2.0*$iy1+$iy2+$h]

   .printtmp create eps $xpos$sasfit(print,units) $ypos$sasfit(print,units) \
             -width  $w$sasfit(print,units) \
             -height $h$sasfit(print,units) \
             -anchor nw \
             -file [file join $sasfit(datadir) $sasfit(print,SDGraph_ps)]

   set xpos [expr $x1+2.0*$ix1+$ix2+$w]
   set ypos [expr $y1+2.0*$iy1+$iy2+$h]

   .printtmp create eps $xpos$sasfit(print,units) $ypos$sasfit(print,units) \
             -width  $w$sasfit(print,units) \
             -height $h$sasfit(print,units) \
             -anchor nw \
             -file [file join $sasfit(datadir) $sasfit(print,AnalytPar_ps)]

   set xpos [expr $x1+$ix1]
   set ypos [expr $y1+$iy1]

   .printtmp create eps $xpos$sasfit(print,units) $ypos$sasfit(print,units) \
             -width  $w$sasfit(print,units) \
             -height $h$sasfit(print,units) \
             -anchor nw \
             -file [file join $sasfit(datadir) $sasfit(print,ResIQGraph_ps)]

   set xpos [expr $x1+2.0*$ix1+$ix2+$w]
   set ypos [expr $y1+    $iy1        ]

   .printtmp create eps $xpos$sasfit(print,units) $ypos$sasfit(print,units) \
             -width  $w$sasfit(print,units) \
             -height $h$sasfit(print,units) \
             -anchor nw \
             -file [file join $sasfit(datadir) $sasfit(print,Moments_ps)]


   .printtmp postscript \
             -file [file join $sasfit(datadir) $sasfit(print,tmpps)] \
             -width  $W$sasfit(print,units) \
             -height $H$sasfit(print,units) \
             -pagewidth  $W$sasfit(print,units) \
             -pageheight $H$sasfit(print,units) \
             -pageanchor sw \
             -pagex 0m -pagey 0m
destroy .printtmp
file delete [file join $sasfit(datadir) $sasfit(print,IQGraph_ps)      ]
file delete [file join $sasfit(datadir) $sasfit(print,ResIQGraph_ps)   ]
file delete [file join $sasfit(datadir) $sasfit(print,SDGraph_ps)      ]
file delete [file join $sasfit(datadir) $sasfit(print,AnalytPar_ps)    ]
file delete [file join $sasfit(datadir) $sasfit(print,StructParData_ps)]
file delete [file join $sasfit(datadir) $sasfit(print,Moments_ps)      ]
}

proc print_AnalytParSummary {} {

   global IQGraph ResIQGraph SDGraph GlobalFitIQGraph
   global GlobalAnalytPar AnalytPar StructParData
   global sasfit

   set ix1 $sasfit(print,ipadx1)
   set ix2 $sasfit(print,ipadx2)
   set iy1 $sasfit(print,ipady1)
   set iy2 $sasfit(print,ipady2)
   set x1  $sasfit(print,padx1)
   set x2  $sasfit(print,padx2)
   set y1  $sasfit(print,pady1)
   set y2  $sasfit(print,pady2)
   switch  $sasfit(print,units) {
      m   {
            set W   $sasfit(print,paperwidth)
            set H   $sasfit(print,paperheight)
          }
      c  {
            set W   [expr $sasfit(print,paperwidth)  / 10.0]
            set H   [expr $sasfit(print,paperheight) / 10.0]
          }
      i   {
            set W   [expr $sasfit(print,paperwidth)  / 25.4]
            set H   [expr $sasfit(print,paperheight) / 25.4]
          }
      p   {
            set W   [expr $sasfit(print,paperwidth)  / 25.4*72.0]
            set H   [expr $sasfit(print,paperheight) / 25.4*72.0]
          }
   }
   set w [expr ($W-$x1-$x2)/2.0 - $ix1 -$ix2]
   set h [expr ($H-$y1-$y2)/2.0 - $iy1 -$iy2]

   $IQGraph(w) postscript output \
        [file join $sasfit(datadir) $sasfit(print,IQGraph_ps)] \
        -colormode $sasfit(print,colormode) \
        -landscape yes \
        -height $w$sasfit(print,units) \
        -width  $h$sasfit(print,units) \
        -maxpect yes
   $GlobalFitIQGraph(w) postscript output \
        [file join $sasfit(datadir) $sasfit(print,GlobalIQGraph_ps)] \
        -colormode $sasfit(print,colormode) \
        -landscape yes \
        -height $w$sasfit(print,units) \
        -width  $h$sasfit(print,units) \
        -maxpect yes
   $SDGraph(w) postscript output \
        [file join $sasfit(datadir) $sasfit(print,SDGraph_ps)] \
        -colormode $sasfit(print,colormode) \
        -landscape yes \
        -height $w$sasfit(print,units) \
        -width  $h$sasfit(print,units) \
        -maxpect yes
#   $AnalytPar(w_MomentsAnalytPar) postscript \
#        -file [file join $sasfit(datadir) $sasfit(print,Moments_ps)] \
#        -rotate yes \
#        -pagewidth $w$sasfit(print,units)

   set SingleOrGlobal [catch { $AnalytPar(w_AnalytPar) postscript \
                                  -file [file join $sasfit(datadir) $sasfit(print,AnalytPar_ps)] \
                                  -rotate yes \
                                  -pagewidth $w$sasfit(print,units)
                             } ]

   if {$SingleOrGlobal != 0 } {
                $GlobalAnalytPar(w_AnalytPar) postscript \
                   -file [file join $sasfit(datadir) $sasfit(print,AnalytPar_ps)] \
                   -rotate yes \
                   -pagewidth $w$sasfit(print,units)
                $GlobalAnalytPar(w_MomentsAnalytPar) postscript \
                   -file [file join $sasfit(datadir) $sasfit(print,Moments_ps)] \
                   -rotate yes \
                   -pagewidth $w$sasfit(print,units)
	     } else {
                $AnalytPar(w_MomentsAnalytPar) postscript \
                   -file [file join $sasfit(datadir) $sasfit(print,Moments_ps)] \
                   -rotate yes \
                   -pagewidth $w$sasfit(print,units)
	     }

   canvas .printtmp -width  $W$sasfit(print,units)  \
                    -height $H$sasfit(print,units)


   set xpos [expr $x1+2.0*$ix1+$ix2+$w]
   set ypos [expr $y1+2.0*$iy1+$iy2+$h]

   if {$SingleOrGlobal == 0 } {
      .printtmp create eps $xpos$sasfit(print,units) $ypos$sasfit(print,units) \
                -width  $w$sasfit(print,units) \
                -height $h$sasfit(print,units) \
                -anchor nw \
                -file [file join $sasfit(datadir) $sasfit(print,IQGraph_ps)]
   } else {
      .printtmp create eps $xpos$sasfit(print,units) $ypos$sasfit(print,units) \
                -width  $w$sasfit(print,units) \
                -height $h$sasfit(print,units) \
                -anchor nw \
                -file [file join $sasfit(datadir) $sasfit(print,GlobalIQGraph_ps)]
   }
   set xpos [expr $x1+    $ix1        ]
   set ypos [expr $y1+3.0*$iy1+$iy2+$h]

   .printtmp create eps $xpos$sasfit(print,units) $ypos$sasfit(print,units) \
             -width  $w$sasfit(print,units) \
             -height $h$sasfit(print,units) \
             -anchor nw \
             -file [file join $sasfit(datadir) $sasfit(print,AnalytPar_ps)]

   set xpos [expr $x1+$ix1]
   set ypos [expr $y1+$iy1]

   .printtmp create eps $xpos$sasfit(print,units) $ypos$sasfit(print,units) \
             -width  $w$sasfit(print,units) \
             -height $h$sasfit(print,units) \
             -anchor nw \
             -file [file join $sasfit(datadir) $sasfit(print,SDGraph_ps)]

   set xpos [expr $x1+2.0*$ix1+$ix2+$w]
   set ypos [expr $y1+    $iy1        ]

   .printtmp create eps $xpos$sasfit(print,units) $ypos$sasfit(print,units) \
             -width  $w$sasfit(print,units) \
             -height $h$sasfit(print,units) \
             -anchor nw \
             -file [file join $sasfit(datadir) $sasfit(print,Moments_ps)]


   .printtmp postscript \
             -file [file join $sasfit(datadir) $sasfit(print,tmpps)] \
             -width  $W$sasfit(print,units) \
             -height $H$sasfit(print,units) \
             -pagewidth  $W$sasfit(print,units) \
             -pageheight $H$sasfit(print,units) \
             -pageanchor sw \
             -pagex 0m -pagey 0m
destroy .printtmp
file delete [file join $sasfit(datadir) $sasfit(print,IQGraph_ps)      ]
file delete [file join $sasfit(datadir) $sasfit(print,SDGraph_ps)      ]
file delete [file join $sasfit(datadir) $sasfit(print,GlobalIQGraph_ps)]
file delete [file join $sasfit(datadir) $sasfit(print,AnalytPar_ps)    ]
file delete [file join $sasfit(datadir) $sasfit(print,Moments_ps)      ]
}

proc print_StructParDataSummary {} {

   global IQGraph ResIQGraph SDGraph
   global AnalytPar StructParData
   global sasfit

   set ix1 $sasfit(print,ipadx1)
   set ix2 $sasfit(print,ipadx2)
   set iy1 $sasfit(print,ipady1)
   set iy2 $sasfit(print,ipady2)
   set x1  $sasfit(print,padx1)
   set x2  $sasfit(print,padx2)
   set y1  $sasfit(print,pady1)
   set y2  $sasfit(print,pady2)
   switch  $sasfit(print,units) {
      m   {
            set W   $sasfit(print,paperwidth)
            set H   $sasfit(print,paperheight)
          }
      c  {
            set W   [expr $sasfit(print,paperwidth)  / 10.0]
            set H   [expr $sasfit(print,paperheight) / 10.0]
          }
      i   {
            set W   [expr $sasfit(print,paperwidth)  / 25.4]
            set H   [expr $sasfit(print,paperheight) / 25.4]
          }
      p   {
            set W   [expr $sasfit(print,paperwidth)  / 25.4*72.0]
            set H   [expr $sasfit(print,paperheight) / 25.4*72.0]
          }
   }

   set w [expr ($W-$x1-$x2)/1.0 - $ix1 -$ix2]
   set h [expr ($H-$y1-$y2)/2.0 - $iy1 -$iy2]

   $IQGraph(w) postscript output \
        [file join $sasfit(datadir) $sasfit(print,IQGraph_ps)] \
        -colormode $sasfit(print,colormode) \
        -landscape no \
        -paperheight $h$sasfit(print,units) \
        -paperwidth  $w$sasfit(print,units) \
        -height [expr $h/1.5]$sasfit(print,units) \
        -width  [expr $w/1.5]$sasfit(print,units) \
        -maxpect yes
   $StructParData(w_StructParData) postscript \
        -file [file join $sasfit(datadir) $sasfit(print,StructParData_ps)] \
        -rotate no  \
        -width [$StructParData(w_StructParData) cget -width] \
        -height [$StructParData(w_StructParData) cget -height] \
        -pagewidth $w$sasfit(print,units)

   canvas .printtmp -width  $W$sasfit(print,units)  \
                    -height $H$sasfit(print,units)

   set xpos [expr $x1+    $ix1        ]
   set ypos [expr $y1+2.0*$iy1+$iy2+$h]

   .printtmp create eps $xpos$sasfit(print,units) $ypos$sasfit(print,units) \
             -width  $w$sasfit(print,units) \
             -height $h$sasfit(print,units) \
             -anchor nw \
             -file [file join $sasfit(datadir) $sasfit(print,StructParData_ps)]

   set xpos [expr $x1+$ix1]
   set ypos [expr $y1+$iy1]

   .printtmp create eps $xpos$sasfit(print,units) $ypos$sasfit(print,units) \
             -width  $w$sasfit(print,units) \
             -height $h$sasfit(print,units) \
             -anchor nw \
             -file [file join $sasfit(datadir) $sasfit(print,IQGraph_ps)]

   .printtmp postscript \
             -file [file join $sasfit(datadir) $sasfit(print,tmpps)] \
             -width  $W$sasfit(print,units) \
             -height $H$sasfit(print,units) \
             -pagewidth  $W$sasfit(print,units) \
             -pageheight $H$sasfit(print,units) \
             -pageanchor sw \
             -pagex 0m -pagey 0m
destroy .printtmp
file delete [file join $sasfit(datadir) $sasfit(print,IQGraph_ps)      ]
file delete [file join $sasfit(datadir) $sasfit(print,StructParData_ps)]
}


#------------------------------------------------------------------------------
# 
#
#
proc export_blt_graph {bltGraph} {
upvar $bltGraph Graph
global sasfit
global ExportGraph
cp_arr Graph ExportGraph
if { [winfo exists .export_blt] } {destroy .export_blt}
toplevel .export_blt
set w .export_blt
set sasfit(export,blt) $Graph(w)
wm geometry $w
wm title $w "Export Data"
frame $w.format
frame $w.name
frame $w.actions 
pack $w.format $w.name $w.actions -fill both -expand yes
set format [tk_optionMenu $w.format.format sasfit(export,actualdatatype) \
                           Ascii csv PDH BerSANS]
label $w.format.label -text "File Format:"
pack $w.format.label $w.format.format -side left -fill x

label $w.name.label -text "  File Name:"
entry $w.name.entry -width 40 -relief sunken \
                              -textvariable sasfit(export,filename)
button $w.name.browse -text "Browse..." \
       -command { BrowseCmd .export_blt sasfit(export,filename) \
                            sasfit(export,actualdatatype) "SaveData"
        }
pack $w.name.label $w.name.entry \
     $w.name.browse  -side left -padx 2

button $w.actions.export -text Export -highlightthickness 0 \
       -command {
           global ExportGraph
           if {[string length [string trim $sasfit(export,filename)]] != 0} {
              switch $sasfit(export,actualdatatype) {
                 Ascii   { export_ascii_data  $sasfit(export,filename) ExportGraph}
                 csv     { export_csv_data    $sasfit(export,filename) ExportGraph}
                 default { tk_messageBox -message "unknown export format" }
              }
           } else {
              BrowseCmd .export_blt sasfit(export,filename) \
                            sasfit(export,actualdatatype) "SaveData"
           }
           destroy .export_blt
       }
button $w.actions.cancel -text Cancel -highlightthickness 0 \
       -command {
           destroy .export_blt
       }
button $w.actions.help -text "Help..." -highlightthickness 0 \
       -command {
        tk_messageBox -message "in work"
       }
pack $w.actions.export $w.actions.cancel -side left -padx 2m -pady 2m
pack $w.actions.help -side right -padx 2m -pady 2m
}

proc print_Moments {} {

   global AnalytPar
   global sasfit

   set ix1 $sasfit(print,ipadx1)
   set ix2 $sasfit(print,ipadx2)
   set iy1 $sasfit(print,ipady1)
   set iy2 $sasfit(print,ipady2)
   set x1  $sasfit(print,padx1)
   set x2  $sasfit(print,padx2)
   set y1  $sasfit(print,pady1)
   set y2  $sasfit(print,pady2)
   switch  $sasfit(print,units) {
      m   {
            set W   $sasfit(print,paperwidth)
            set H   $sasfit(print,paperheight)
          }
      c  {
            set W   [expr $sasfit(print,paperwidth)  / 10.0]
            set H   [expr $sasfit(print,paperheight) / 10.0]
          }
      i   {
            set W   [expr $sasfit(print,paperwidth)  / 25.4]
            set H   [expr $sasfit(print,paperheight) / 25.4]
          }
      p   {
            set W   [expr $sasfit(print,paperwidth)  / 25.4*72.0]
            set H   [expr $sasfit(print,paperheight) / 25.4*72.0]
          }
   }
   set w [expr ($W-$x1-$x2)/1.0 - $ix1 -$ix2]
   set h [expr ($H-$y1-$y2)/1.0 - $iy1 -$iy2]

   $AnalytPar(w_MomentsAnalytPar) postscript \
        -file [file join $sasfit(datadir) $sasfit(print,Moments_ps)] \
        -rotate yes \
        -pagewidth $W$sasfit(print,units)

   canvas .printtmp -width  $W$sasfit(print,units)  \
                    -height $H$sasfit(print,units)

   set xpos [expr $x1+$ix1]
   set ypos [expr $y1+$iy1]

   .printtmp create eps $xpos$sasfit(print,units) $ypos$sasfit(print,units) \
             -width  $w$sasfit(print,units) \
             -height $h$sasfit(print,units) \
             -anchor nw \
             -file [file join $sasfit(datadir) $sasfit(print,Moments_ps)]

   .printtmp postscript \
             -file [file join $sasfit(datadir) $sasfit(print,tmpps)] \
             -width  $W$sasfit(print,units) \
             -height $H$sasfit(print,units) \
             -pagewidth  $W$sasfit(print,units) \
             -pageheight $H$sasfit(print,units) \
             -pageanchor sw \
             -pagex 0m -pagey 0m
destroy .printtmp
file delete [file join $sasfit(datadir) $sasfit(print,Moments_ps)]
}

proc print_StructParData {} {

   global StructParData
   global sasfit

   set ix1 $sasfit(print,ipadx1)
   set ix2 $sasfit(print,ipadx2)
   set iy1 $sasfit(print,ipady1)
   set iy2 $sasfit(print,ipady2)
   set x1  $sasfit(print,padx1)
   set x2  $sasfit(print,padx2)
   set y1  $sasfit(print,pady1)
   set y2  $sasfit(print,pady2)
   switch  $sasfit(print,units) {
      m   {
            set W   $sasfit(print,paperwidth)
            set H   $sasfit(print,paperheight)
          }
      c  {
            set W   [expr $sasfit(print,paperwidth)  / 10.0]
            set H   [expr $sasfit(print,paperheight) / 10.0]
          }
      i   {
            set W   [expr $sasfit(print,paperwidth)  / 25.4]
            set H   [expr $sasfit(print,paperheight) / 25.4]
          }
      p   {
            set W   [expr $sasfit(print,paperwidth)  / 25.4*72.0]
            set H   [expr $sasfit(print,paperheight) / 25.4*72.0]
          }
   }
   set w [expr ($W-$x1-$x2)/1.0 - $ix1 -$ix2]
   set h [expr ($H-$y1-$y2)/1.0 - $iy1 -$iy2]

   $StructParData(w_StructParData) postscript \
        -file [file join $sasfit(datadir) $sasfit(print,StructParData_ps)] \
        -rotate yes \
        -pagewidth $W$sasfit(print,units)

   canvas .printtmp -width  $W$sasfit(print,units)  \
                    -height $H$sasfit(print,units)

   set xpos [expr $x1+$ix1]
   set ypos [expr $y1+$iy1]

   .printtmp create eps $xpos$sasfit(print,units) $ypos$sasfit(print,units) \
             -width  $w$sasfit(print,units) \
             -height $h$sasfit(print,units) \
             -anchor nw \
             -file [file join $sasfit(datadir) $sasfit(print,StructParData_ps)]

   .printtmp postscript \
             -file [file join $sasfit(datadir) $sasfit(print,tmpps)] \
             -width  $W$sasfit(print,units) \
             -height $H$sasfit(print,units) \
             -pagewidth  $W$sasfit(print,units) \
             -pageheight $H$sasfit(print,units) \
             -pageanchor sw \
             -pagex 0m -pagey 0m
destroy .printtmp
file delete [file join $sasfit(datadir) $sasfit(print,StructParData_ps)]
}

proc print_AnalytPar {} {

   global AnalytPar
   global GlobalAnalytPar
   global sasfit addsasfit

   set ix1 $sasfit(print,ipadx1)
   set ix2 $sasfit(print,ipadx2)
   set iy1 $sasfit(print,ipady1)
   set iy2 $sasfit(print,ipady2)
   set x1  $sasfit(print,padx1)
   set x2  $sasfit(print,padx2)
   set y1  $sasfit(print,pady1)
   set y2  $sasfit(print,pady2)
   switch  $sasfit(print,units) {
      m   {
            set W   $sasfit(print,paperwidth)
            set H   $sasfit(print,paperheight)
          }
      c  {
            set W   [expr $sasfit(print,paperwidth)  / 10.0]
            set H   [expr $sasfit(print,paperheight) / 10.0]
          }
      i   {
            set W   [expr $sasfit(print,paperwidth)  / 25.4]
            set H   [expr $sasfit(print,paperheight) / 25.4]
          }
      p   {
            set W   [expr $sasfit(print,paperwidth)  / 25.4*72.0]
            set H   [expr $sasfit(print,paperheight) / 25.4*72.0]
          }
   }
   set w [expr ($W-$x1-$x2)/1.0 - $ix1 -$ix2]
   set h [expr ($H-$y1-$y2)/1.0 - $iy1 -$iy2]

   set SingleOrGlobal [catch { $AnalytPar(w_AnalytPar) postscript \
                                  -file [file join $sasfit(datadir) $sasfit(print,AnalytPar_ps)] \
                                  -rotate yes \
                                  -pagewidth $w$sasfit(print,units)
                             } ]  
	      
   if {$SingleOrGlobal != 0 } {
                $GlobalAnalytPar(w_AnalytPar) postscript \
                   -file [file join $sasfit(datadir) $addsasfit(print,GlobalAnalytPar_ps)] \
                   -rotate yes \
                   -pagewidth $w$sasfit(print,units)
	     }
   if {$SingleOrGlobal == 0 } {
      canvas .printtmp -width  $W$sasfit(print,units)  \
                       -height $H$sasfit(print,units)

      set xpos [expr $x1+$ix1]
      set ypos [expr $y1+$iy1]

      .printtmp create eps $xpos$sasfit(print,units) $ypos$sasfit(print,units) \
                -width  $w$sasfit(print,units) \
                -height $h$sasfit(print,units) \
                -anchor nw \
                -file [file join $sasfit(datadir) $sasfit(print,AnalytPar_ps)]

      .printtmp postscript \
                -file [file join $sasfit(datadir) $sasfit(print,tmpps)] \
                -width  $W$sasfit(print,units) \
                -height $H$sasfit(print,units) \
                -pagewidth  $W$sasfit(print,units) \
                -pageheight $H$sasfit(print,units) \
                -pageanchor sw \
                -pagex 0m -pagey 0m
      destroy .printtmp
      file delete [file join $sasfit(datadir) $sasfit(print,AnalytPar_ps)]
   } else {
      canvas .printtmp -width  $W$addsasfit(print,units)  \
                       -height $H$addsasfit(print,units)

      set xpos [expr $x1+$ix1]
      set ypos [expr $y1+$iy1]

      .printtmp create eps $xpos$addsasfit(print,units) $ypos$addsasfit(print,units) \
                -width  $w$addsasfit(print,units) \
                -height $h$addsasfit(print,units) \
                -anchor nw \
                -file [file join $sasfit(datadir) $addsasfit(print,GlobalAnalytPar_ps)]

      .printtmp postscript \
                -file [file join $sasfit(datadir) $addsasfit(print,tmpps)] \
                -width  $W$addsasfit(print,units) \
                -height $H$addsasfit(print,units) \
                -pagewidth  $W$addsasfit(print,units) \
                -pageheight $H$addsasfit(print,units) \
                -pageanchor sw \
                -pagex 0m -pagey 0m
      destroy .printtmp
      file delete [file join $sasfit(datadir) $addsasfit(print,GlobalAnalytPar_ps)]
   }
}
