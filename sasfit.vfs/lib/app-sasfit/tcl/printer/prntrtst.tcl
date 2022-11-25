################################################################
##
## prntrtst.tcl:
##   Test just printer functions, establish versions, etc.
##
################################################################
proc init_printer { } {
  if [ catch { package require printer} err ]  {
    if [ catch { load ./printer.dll } err ] {
      error "Can't load printer: $err"
    }
  }
}

proc init_main_ui { } {
  global mailer

  if [ winfo exist .frame ] {
    destroy .frame
  }
  frame .frame

  # The menu part...
  menu  .frame.menu -tearoff 0
  menu  .frame.menu.file -tearoff 0
  .frame.menu add cascade -label File -menu .frame.menu.file -underline 0
  .frame.menu.file add command -label Exit -underline 0 -command { exit 0 }
  .frame.menu.file add separator
  .frame.menu.file add command -label About -underline 0 -command about

  menu .frame.menu.options -tearoff 0
  .frame.menu add cascade -label Options -menu .frame.menu.options -underline 0
  .frame.menu.options add command -label "List Printers" -underline 0 \
				-command { list_printers }
  .frame.menu.options add command -label "List Printers - Verbose" -underline 0 \
				-command { list_printers verbose }
  .frame.menu.options add command -label "Default Printer" -underline 0 \
  				-command { default_printer }
  .frame.menu.options add command -label "Select Printer" -underline 0 \
  				-command { select_printer }
  .frame.menu.options add command -label "Page Setup" -underline 0 \
  				-command { pagesetup_printer }
  .frame.menu.options add command -label "Printer Attributes" -underline 8 \
  				-command { attr_printer }
  .frame.menu.options add command -label "Test Printers" -underline 1 \
  				-command do_test_printers_ui
  .frame.menu.options add separator
  .frame.menu.options add command -label "Clear Output" -underline 0 \
  				-command { cleartext }
  .frame.menu.options add separator
  .frame.menu.options add command -label "Toggle Console" -underline 7 \
				-command { toggle_console }
  . configure -menu .frame.menu

  # The work part
  frame .frame.text
  text  .frame.text.body -font {Courier 10} -yscroll ".frame.text.vscroll set"
  scrollbar .frame.text.vscroll -orient vertical -command ".frame.text.body yview"

  frame .frame.buttons
  button .frame.buttons.ok       -text Exit -command exit
  button .frame.buttons.about    -text About... -command about
  button .frame.buttons.list     -text "List\nPrinters" -command list_printers
  button .frame.buttons.listv    -text "List\nPrinters\nVerbose" -command "list_printers verbose"
  button .frame.buttons.default  -text "Default\nPrinter" -command default_printer
  button .frame.buttons.select   -text "Select\nPrinter" -command select_printer
  button .frame.buttons.setup    -text "Page\nSetup" -command pagesetup_printer
  button .frame.buttons.attr     -text "Printer\nAttributes" -command attr_printer
  button .frame.buttons.test     -text "Test\nPrinters" -command do_test_printers_ui
  button .frame.buttons.clear    -text "Clear\nOutput" -command cleartext

  set row 0
  grid configure .frame.text.body -column 0 -row $row -sticky news
  grid configure .frame.text.vscroll -column 1 -row $row -sticky ns
  pack .frame.text -fill both -side right -expand true

  pack .frame.buttons.ok .frame.buttons.about \
       .frame.buttons.list .frame.buttons.listv .frame.buttons.default \
       .frame.buttons.select .frame.buttons.setup \
       .frame.buttons.attr \
       .frame.buttons.clear \
       -side top -fill x
  pack .frame.buttons -side left -fill y

  pack .frame -side top -fill both

  # Titling part
  set tl [ winfo toplevel .frame ]
  wm title $tl "Printer Function Test"
}

proc cleartext { } {
  .frame.text.body delete 0.0 end
}

proc addtext { args } {
  set text [ join $args ]
  .frame.text.body insert end $text\n
  .frame.text.body see end
}

proc list_printers { {how terse} } {
  addtext "Printer List - $how\n"
  if { $how == "verbose" } {
    foreach name [printer list -verbose] {
      addtext [lindex [lindex $name 0 ] 1 ]
      foreach att $name {
	addtext [ format "    %-20s    %s" [lindex $att 0] [lindex $att 1] ]
      }
      # This could take a while...
      update idletasks
    }
  } else {
    foreach name [printer list] {
      addtext $name
    }
  }
  addtext "----------------------------------------------"
}

proc select_printer { } {
  addtext "Select Printer\n"
  set r [ printer dialog select ]
  if { [lindex $r 1] == 0 } {
    addtext "Printer selection canceled\n"
    return
  }

  set attrs [ printer attr ]
  foreach attr $attrs {
    set tag [lindex $attr 0]
    if { $tag == "device" } {
      addtext "The selected printer is: [lindex $attr 1]"
      break
    }
  }
  addtext "----------------------------------------------"
}

proc pagesetup_printer { } {
  addtext "Printer Page Setup\n"
  set r [ printer dialog page_setup ]
  if { $r == 0 } {
    addtext Page setup canceled
  } else {
    addtext Page setup completed
  }
  addtext "----------------------------------------------"
}

proc default_printer { } {
  addtext "Default Printer"
  printer open
  set attrs [ printer attr ]
  foreach attr $attrs {
    set tag [lindex $attr 0]
    if { $tag == "device" } {
      addtext "The default printer is: [lindex $attr 1]"
      break
    }
  }
  addtext "----------------------------------------------"
}

proc attr_printer { {hdc ""} } {
  addtext "Attributes of Current Printer " $hdc
  if { $hdc == "" } {
    set attrs [ printer attr ]
  } else {
    set attrs [ printer attr -hDC $hdc ]
  }

  foreach attr $attrs {
    set tag [lindex $attr 0]
    set val [lindex $attr 1]
    addtext [format "%-20s  %s" $tag $val]
  }
  addtext "----------------------------------------------"
}

proc do_test_printers_ui { } {
  global chooseprinter
  if [ winfo exist .chooseprinter ] {
    wm deiconify .chooseprinter
    raise .chooseprinter
  } else {
    # Bring up a UI for testing any of a set of printers
    set pl [ printer list ]
    set c [toplevel .chooseprinter]
    wm title $c "Select printers to test"
    set f [frame $c.printers]
    set b [frame $c.buttons]
    button $b.cancel -text Cancel -command "wm withdraw .chooseprinter"
    button $b.ok     -text OK     -command {
                                            set list ""
                                            foreach name [array names chooseprinter ] {
					      if { $chooseprinter($name) == 1 } {
					        lappend list $name
					      }
                                            }
					    test_printers $list
					    wm withdraw .chooseprinter
    }
    set num 0
    set period 4
    foreach p [lsort $pl] {
      set chooseprinter($p) 1
      checkbutton $f.p$num -text $p -variable chooseprinter($p)
      grid configure $f.p$num \
          -column [expr $num % $period] -row [expr $num / $period] -sticky nws
      incr num
    }
    grid configure $b.ok -column 0 -row 0 -sticky ns -padx 3c
    grid configure $b.cancel -column 1 -row 0 -sticky ns -padx 1c
    pack $f -side top -fill both -expand true
    pack $b -side bottom -fill x -expand true
  }
}

proc test_printers { printer_list } {
  foreach printer [lsort $printer_list] {
    if [ catch {
         addtext "Opening $printer with default attributes"
         set hdc [printer open -name $printer]
	 addtext "Showing attributes for printer $printer"
         attr_printer $hdc
         addtext "Ready for first page"
         printer job start -name "Test for $printer"
         printer page start

         # Now see if we can get the gdi package for some output
         if [ catch {
                package require gdi
                    } pkgload ] {
           addtext "Can't get GDI package--trying plain text"
           printer send -hDC $hdc -data "This is a test in text-only mode\n"
         } else {
           global parray
           # Next line is a kludge until I figure out what's going on....
           printer attr -hdc $hdc
           page_args parray $hdc

           # Box in the printable area
           # If the margins seem too big, shrink them....
           if { $parray(lm) <  100 } { set parray(lm)   500 }
           if { $parray(rm) <  100 } { set parray(rm)   500 }
           if { $parray(tm) <  100 } { set parray(tm)   500 }
           if { $parray(bm) <  100 } { set parray(bm)   500 }

           set ulx [ expr $parray(lm) * $parray(resx) / 1000 ]
           set uly [ expr $parray(tm) * $parray(resy) / 1000 ]
           set lrx [ expr ($parray(pw) - $parray(rm)) * $parray(resx) / 1000 ]
           set lry [ expr ($parray(pl) - $parray(bm)) * $parray(resy) / 1000 ]

           gdi rectangle $hdc  $ulx $uly $lrx $lry \
	                       -width 20 -outline "#FF00FF" -fill "#A0A0A0"
	   gdi line $hdc $ulx $uly $lrx $lry \
	   		       -width 10 -fill "#FFFF00"
           # Put in some centered text in Arial 18
	   gdi text $hdc [expr ($parray(pw) * $parray(resx) / 1000) / 2 ] \
	   		 [expr ($parray(pl) * $parray(resy) / 1000) / 2 ] \
			 -fill "#008F8F" -font { Arial 18} -text "Testing printer $printer"
         }

         addtext "Closing up the job"
         printer page end
         printer job end
         printer close -hDC $hdc
         } err ] {
      addtext Cannot test printer $printer: $err
      printer close -hDC $hdc
    }
    addtext "Completed $printer"
    update idletasks
  }
}

proc about { } {
  global tcl_patchLevel
  global tcl_platform
  addtext [ subst {
    Version information (in case of problem reports):
       Printer version: [printer version]
       Tcl version:     $tcl_patchLevel
       Tcl platform:    $tcl_platform(platform)/$tcl_platform(os)/$tcl_platform(osVersion)/$tcl_platform(machine)
    Please send suggestions, comments, good ideas you've implemented, 
    and bug reports to the author:
            Michael I. Schwartz
            mschwart@nyx.net
            PO Box 24536
            Denver, CO 80224
  } ]
  addtext "----------------------------------------------"
}

proc toggle_console { } {
  global consolestate

  if { [info command console] == "" } {
    addtext "Sorry -- the console function is not implemented on your platform"
    return 
  }
  if [ info exist consolestate ] {
    if { $consolestate == "on" } {
      set consolestate off
      console hide
    } else {
      set consolestate on
      console show
    }
  } else {
    set consolestate on
    console title "Printer Test Console"
    console show
  }
}

proc page_args { array { hdc "" } } {
  upvar #0 $array ary

  # First we check whether we have a valid hDC
  # (perhaps we can later make this also an optional argument, defaulting to 
  #  the default printer)
  if { $hdc == "" } {
    set attr [ printer attr ]
  } else {
    set attr [ printer attr -hDC $hdc ]
  }

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

  if { ( [ info exist ary(hDC) ] == 0 ) || ($ary(hDC) == 0x0) } {
    error "Can't get printer attributes"
  }
 
  # Now, set "reasonable" defaults if some values were unavailable
  if { [ info exist ary(resx) ] == 0 } { set ary(resx) 200 }
  if { [ info exist ary(resy) ] == 0 } { set ary(resy) 200 }
  if { [ info exist ary(tm) ] == 0 } { set ary(tm) 1000 }
  if { [ info exist ary(bm) ] == 0 } { set ary(bm) 1000 }
  if { [ info exist ary(lm) ] == 0 } { set ary(lm) 1000 }
  if { [ info exist ary(rm) ] == 0 } { set ary(rm) 1000 }
  if { [ info exist ary(pw) ] == 0 } { set ary(pw) 8500 }
  if { [ info exist ary(pl) ] == 0 } { set ary(pl) 11000 }
  if { [ info exist ary(copies) ] == 0 } { set ary(copies) 1 }

}

init_printer
init_main_ui
