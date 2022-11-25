#!/usr/local/bin/wish8.1
################################################################
## This script will allow the trial user to test the printer 
## extension. If the gdi extension is present, it will allow
## using parts of it as well.
################################################################

proc init_printer_globals { } {
  global printer_vars
  set printer_vars(have_gdi) 0
}

proc init_printer { } {
  global printer_vars

  init_printer_globals

  if [ catch { package require printer } err ] {
    if [ catch { load ./printer.dll } err ] {
      error "Can't load printer package: $err"
    }
  }

  set printer_vars(have_gdi) 1

  if [ catch { package require gdi } err ] {
    if [ catch { load ./gdi.dll } err ] {
      set printer_vars(have_gdi) 0
    }
  }

  if { $printer_vars(have_gdi) == 1 } {
    # Try to get the prntcanv procedure
    if [ catch { source ./prntcanv.tcl } err ] {
      set printer_vars(have_gdi_print) 0
    } else {
      set printer_vars(have_gdi_print) 1
    }
  } else {
    set printer_vars(have_gdi_print) 0
  }
}

proc init_printer_main_ui { } {
  global printer_vars
  global tcl_patchLevel

  if [ winfo exist .frame ] {
    destroy .frame
  }
  frame .frame

  # The menu part ...
  if { [string index $tcl_patchLevel 0] == 7 }  {
    menubutton .frame.menu -menu .frame.menu.file
    menu .frame.menu.file -tearoff 0
  } else {
    menu .frame.menu -tearoff 0
    menu .frame.menu.file -tearoff 0
    .frame.menu add cascade -label File -menu .frame.menu.file -underline 0
  }
  .frame.menu.file add command -label "Select printer" -underline 0 -command { do_select_printer}
  .frame.menu.file add command -label "Page Setup"  -underline 2 -command { do_setup_printer }
  .frame.menu.file add command -label "Attributes" -underline 0 -command { do_printer_attributes }
  .frame.menu.file add separator
  .frame.menu.file add command -label "Print canvas" -underline 0 -command do_select_and_print
  ## Not implemented yet
  # .frame.menu.file add command -label "Print all" -underline 6 -command do_select_and_print_all
  
  ## Not ready until prntcanv can handle open printer connections
  # .frame.menu.file add command -label "Print to selected" -underline 4 -command do_print
  .frame.menu.file add separator
  .frame.menu.file add command -label Exit -underline 1 -command { exit 0 }
  
  if { [string index $tcl_patchLevel 0] == 7 }  {
    pack .frame.menu -side top -fill x
  } else {
    . configure -menu .frame.menu
  }

  # The work part
  frame .frame.test -borderwidth 2 -relief groove
  frame .frame.test.attributes -relief groove -borderwidth 2
  frame .frame.test.canvas -relief groove -borderwidth 2

  label .frame.test.currenttag -text "Current Printer" -justify left -anchor nw
  label .frame.test.currentlabel -textvariable printer_vars(current) -justify left -anchor nw

  label .frame.test.defaulttag -text "Default Printer" -justify left -anchor nw
  label .frame.test.defaultlabel -textvariable printer_vars(default) -justify left -anchor nw

  label .frame.test.printerlisttag   -text "Printer List" -justify left -anchor nw
  label .frame.test.printerlistlabel -textvariable printer_vars(list) -justify left -anchor nw
  canvas .frame.test.canvas.c -height 300 -width 200
  pack .frame.test.canvas.c -side top -fill both -expand true
  
  #####################################################
  ## Contents of the test canvas
  #####################################################
  # A set of lines
  .frame.test.canvas.c create line 1 1 199 199  -dash "4 4" -arrow both -fill blue -width 3
  .frame.test.canvas.c create line 199 1 1 199 -fill red -width 4
  .frame.test.canvas.c create line 1 201 199 201 199 250 1 250 -fill orange -width 5 -dash "5 2" -arrow first
  
  # A nice yellow circle with some words inside.
  .frame.test.canvas.c create oval 1  1  199 199 -outline green -fill yellow
  .frame.test.canvas.c create text 100 100 -anchor center -font { Times 24 } -width 180 -text "Test String For Printer Package"
  
  # polygon
  .frame.test.canvas.c create polygon 100 201 199 240 140 299 60 299 1 240 -outline green -fill yellow -width 2
  
  # Smooth polygon and line
  .frame.test.canvas.c create polygon 100 211 189 240 140 289 60 289 11 240 -outline green -fill yellow -width 2 -smooth true -splinesteps 1
  .frame.test.canvas.c create line 95 10 190 70 95 130 190 190 -fill gray -width 3 -smooth 1
  
  # rectangle
  .frame.test.canvas.c create rectangle 50 235 150 275 -outline violet -fill pink
  
  # image
  set imgf pwrdLogo75.gif
  set img [ image create photo -file $imgf ]
  .frame.test.canvas.c create image 75 220 -image $img -anchor nw
  
  # bitmap
  set bmpf folder.xbm
  set bmp [ image create bitmap -file $bmpf ]
  .frame.test.canvas.c create image 125 240 -image $bmp -anchor nw
  
  # arc
  .frame.test.canvas.c create arc 10 10 190 190 -start 90 -extent -180 -outline blue -width 2
  
  grid configure .frame.test.currenttag       -column 0 -row 0 -sticky news
  grid configure .frame.test.currentlabel     -column 1 -row 0 -sticky news
  grid configure .frame.test.defaulttag       -column 0 -row 1 -sticky news
  grid configure .frame.test.defaultlabel     -column 1 -row 1 -sticky news
  grid configure .frame.test.printerlisttag   -column 0 -row 2 -sticky news
  grid configure .frame.test.printerlistlabel -column 1 -row 2 -sticky news
  grid configure .frame.test.attributes       -column 1 -row 3 -sticky news -rowspan 1
  grid configure .frame.test.canvas           -column 0 -row 3 -sticky news -columnspan 1
  pack .frame.test -side top -expand true -fill both

  # The buttons part
  frame .frame.buttons
  button .frame.buttons.ok -text Exit -command exit
  button .frame.buttons.about    -text About... -command printer_about
  pack .frame.buttons.ok .frame.buttons.about -side left -fill x -expand true

  # The status part
  frame .frame.status
  label .frame.status.status -textvariable printer_vars(status) -justify left -anchor w
  pack  .frame.status.status -expand true -fill both
  pack .frame.status -side bottom -fill x -expand false

  pack .frame.buttons -side bottom -fill x

  pack .frame -side top -fill both

  # Title part
  set tl [ winfo toplevel .frame ]
  wm title $tl "Test for package Printer"
}

proc get_default_printer { } {
  printer open
  set attr [ printer attr -get device ]
  do_printer_attributes
  return [ lindex [ lindex $attr 0 ] 1]
}

proc do_select_printer { } {
  global printer_vars
  set retval [printer dialog select]
  set stat [ lindex $retval 1]
  set hdc  [ lindex $retval 0]
  
  if { $stat == 0 } {
    set printer_vars(status) "Canceled printer selection"
  } else {
    set attr [ printer attr -get device ]
    set printer_vars(current) [ lindex [ lindex $attr 0 ] 1]
    do_printer_attributes
  }
}

proc do_setup_printer { } {
  global printer_vars
  set retval [printer dialog page_setup]
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

proc do_select_and_print_all { } {
    # Not implemented
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
  # If GDI is present, use the canvas print--otherwise use the Postscript print.
  if { $printer_vars(have_gdi_print) } {
    printer::print_widget .frame.test.canvas.c xxx "Test Printer"
    set printer_vars(status) "Printing using the print_widget command"
  } else {
    set ps [ .frame.test.canvas.c postscript]
    printer send -postscript -data $ps
    set printer_vars(status) "Printing using postscript; GDI not available"
  }
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

  set top .frame.test.attributes
  catch {
    eval destroy [ winfo child $top ]
  } err
  set x [ printer attr ]
  set attrlist ""
  
  foreach pair $x {
    set tag [ lindex $pair 0 ]
    set val [ lindex $pair 1 ]
    set printer_vars($tag) $val
    lappend attrlist $tag
  }
  set count 0
  label $top.titlelabel -text "Printer Attributes" -anchor center
  grid configure $top.titlelabel -in $top -column 0 -row $count -columnspan 2 -sticky news

  incr count
  foreach name [ lsort $attrlist ] {
    # Could "trace" this, but it seems a special case...
    if { $name == "device" } {
        set printer_vars(current) $printer_vars($name)
    }
    label $top.frametag$count -text $name -justify left -anchor nw
    label $top.framelabel$count -text $printer_vars($name) -justify right -anchor ne
    grid configure $top.frametag$count -in $top -row $count -column 0 -sticky news
    grid configure $top.framelabel$count -in $top -row $count -column 1 -sticky news
    incr count
  }
}

init_printer
init_printer_main_ui
printer_about
do_list_printers
if [ catch { get_default_printer } name ] {
  set printer_vars(default) "No default printer"
  set printer_vars(current) "No current printer"
} else {
  set printer_vars(default) [join $name]
  set printer_vars(current) $printer_vars(default)
}

