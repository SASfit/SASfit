################################################################
## attrs.tcl
##
## RCS Version $Revision: 1.1 $
## RCS Last Change Date: $Date: 2000/07/02 23:00:02 $
## Original Author: Michael I. Schwartz, mschwart@nyx.net
##
## This is the base set of procs for a test case to verify the 
## printer open and dialog select commands.
##
## The reader may note how the change_orientation command
## is used on an open printer to change orientation--this may
## be done before any page start command.
##
## There is a bit of odd logic in change_orientation to reflect
## the behavior that some printer drivers exhibit in feeding back
## portrait coordinates and margins with a landscape orientation
################################################################
proc printer_two_val_swap { attribute value {hdc "" } } {
  if { $hdc == "" } {
    set hdctag ""
  } else {
    set hdctag "-hDC $hdc"
  }

  set newval [ list [ lindex $value 1] [ lindex $value 0 ] ]
  eval printer attr $hdctag -set [list [ list [list "$attribute" $newval] ] ]
}

proc printer_four_val_swap { attribute value {hdc "" } } {
  if { $hdc == "" } {
    set hdctag ""
  } else {
    set hdctag "-hDC $hdc"
  }

  set newval [ list [ lindex $value 1] [ lindex $value 0 ] [lindex $value 3] [lindex $value 2] ]
  eval printer attr $hdctag -set [list [ list [list "$attribute" $newval] ] ]
}

proc printer_change_orientation { orientation { hdc "" } } {
  # The following must change:
  # Set page orientation
  # Set page dimensions
  # set page margins
  # set page minimum margins
  # set printer resolution
  # First, get the current orientation:
  if { $hdc == "" } {
    set hdctag ""
  } else {
    set hdctag "-hDC $hdc"
  }

  if { $orientation == "portrait" } {
    # Everything else is landscape... 
  } else {
    set orientation "landscape"
  }

  set orient [ eval printer attr $hdctag -get [list [ list "page orientation" ] ] ]
  if { $orient == "" } {
    # No such attribute--just raise an error
    error "This printer has no page orientation attribute"
  }
  set orient [ lindex [lindex $orient 0] 1 ]
  if { $orientation == $orient } {
    # All done--everything's peachy
  } else {
    eval printer attr $hdctag -set [ list [ list [list "page orientation" $orientation] ] ]

    set val [ eval printer attr $hdctag -get [ list [ list "page dimensions" ] ] ]
    if { $val != "" } {
      # See if dimensions are sensible
      set dims [ lindex [ lindex $val 0 ] 1 ]
      set dimx [ lindex $dims 0 ]
      set dimy [ lindex $dims 1 ]
    
      if { "$orient" == "portrait" } {
        if { $dimx > $dimy } { return }
      } elseif { "$orient" == "landscape" } {
        if { $dimx < $dimy } { return }
      }

      printer_two_val_swap "page dimensions" $dims
    }

    set val [ eval printer attr $hdctag -get [ list [ list "printer resolution" ] ] ]
    if { $val != "" } {
      printer_two_val_swap "printer resolution" [ lindex [ lindex $val 0 ] 1 ]
    }

    set val [ eval printer attr $hdctag -get [ list [ list "page margins" ] ] ]
    if { $val != "" } {
      printer_four_val_swap "page margins" [ lindex [ lindex $val 0 ] 1 ]
    }

    set val [ eval printer attr $hdctag -get [ list [ list "page minimum margins" ] ] ]
    if { $val != "" } {
      printer_four_val_swap "page minimum margins" [ lindex [ lindex $val 0 ] 1 ]
    }
  }
}

proc all_default_attribs { {orientation landscape} { autoclose true } } {
  set pl [lsort [ printer list ] ]
  puts "Printer list:"
  foreach name $pl {
    puts "\t$name"
  }
  set ac [ printer option [ list autoclose $autoclose ] ]
  puts "Using orientation $orientation and options $ac"

  puts "Default attributes for each printer\n"
  foreach name $pl {
    puts $name
    # set hdc [ printer open -name $name -attr [list [list "page orientation" $orientation] ] ]
    set hdc [ printer open -name $name ]
    printer_change_orientation $orientation $hdc

    set attr [ printer attr -hDC $hdc ]

    # Only close the hdc if autoclose is NOT on
    if { $autoclose != "true" } {
      lappend hdclist $hdc
    }
    foreach pair $attr {
      puts [ format "%-20s\t%s" [lindex $pair 0 ] [ lindex $pair 1] ]
    }
    puts ""
    update idletasks
  }
  # If autoclose is on, just close the last (default) printer, and all should be OK
  if { $autoclose == "true" } {
    printer close
  } else {
    if { [llength $hdclist] != [llength $pl] } {
      puts "Not all printers opened successfully"
    }
    foreach hdc $hdclist {
      printer close -hDC $hdc
    }
  }

  # Now check to be sure everything is closed:
  if [ catch { 
    set list [ hdc list ]
     } ]  {
    # HDC unsupported -- can't check
  } else {
    if { [ llength $list ] > 0 } {
      puts "There are [llength list] remaining open HDCs! ($list)"
      foreach h $list { printer close -hDC $h }
    }
  }
}

proc selected_attribs { } {
  set ret [ printer dialog select ] 
  set hdc [ lindex $ret 0 ]
  if { [lindex $ret 1] == 0 } {
    puts "Printer selection canceled"
  } else {
    puts "\nAttributes for selected printer\n"
    set attr [ printer attr -hDC $hdc ]
    foreach pair $attr {
      puts [ format "%-20s\t%s" [lindex $pair 0 ] [ lindex $pair 1] ]
    }
    printer close -hDC $hdc
  }
}


################################################################
## Sample run:
## % all_default_attribs portrait
## Printer list:
## 	WinFax
## 	PS5.1
## 	HP DeskJet 710C
## 	HP DeskJet 540 (Monochrome)
## 	Apple LaserWriter II NT
## 	Acrobat PDFWriter
## 	Acrobat Distiller
## Using orientation portrait and options { autoclose true }
## Default attributes for each printer
## 
## WinFax
## device              	WinFax
## hDC                 	0xb7211790
## hdcname             	printerDc10
## page dimensions     	8500 11000
## page margins        	1000 1000 1000 1000
## page minimum margins	0 0 0 0
## page orientation    	portrait
## pixels per inch     	204 196
## resolution          	Low
## 
## PS5.1
## device              	PS5.1
## hDC                 	0xb8211790
## hdcname             	printerDc11
## page dimensions     	8500 11000
## page margins        	1000 1000 1000 1000
## page minimum margins	13 0 13 0
## page orientation    	portrait
## pixels per inch     	600 600
## resolution          	600 600
## 
## HP DeskJet 710C
## device              	HP DeskJet 710C
## hDC                 	0xb9211790
## hdcname             	printerDc12
## page dimensions     	8500 11000
## page margins        	1000 1000 1000 1000
## page minimum margins	250 40 250 40
## page orientation    	portrait
## pixels per inch     	300 300
## resolution          	300 300
## 
## HP DeskJet 540 (Monochrome)
## device              	HP DeskJet 540 (Monochrome)
## hDC                 	0xba211790
## hdcname             	printerDc13
## page dimensions     	8500 11000
## page margins        	1000 1000 1000 1000
## page minimum margins	250 166 250 166
## page orientation    	portrait
## pixels per inch     	300 300
## resolution          	300 300
## 
## Apple LaserWriter II NT
## device              	Apple LaserWriter II NT
## hDC                 	0xbb211790
## hdcname             	printerDc14
## page dimensions     	8500 11000
## page margins        	1000 1000 1000 1000
## page minimum margins	220 110 220 110
## page orientation    	portrait
## pixels per inch     	300 300
## resolution          	300 300
## 
## Acrobat PDFWriter
## device              	Acrobat PDFWriter
## hDC                 	0xb52117ac
## hdcname             	printerDc15
## page dimensions     	8500 11000
## page margins        	1000 1000 1000 1000
## page minimum margins	250 250 250 250
## page orientation    	portrait
## pixels per inch     	300 300
## resolution          	300 300
## 
## Acrobat Distiller
## device              	Acrobat Distiller
## hDC                 	0x22115c6
## hdcname             	printerDc16
## page dimensions     	8500 11000
## page margins        	1000 1000 1000 1000
## page minimum margins	13 0 13 0
## page orientation    	portrait
## pixels per inch     	600 600
## resolution          	600 600
## 
## 
## % selected_attribs
## 
## Attributes for selected printer
## 
## copies              	1
## device              	Acrobat Distiller
## first page          	0
## hDC                 	0xfa210636
## hdcname             	printerDc29
## last page           	0
## page dimensions     	8500 11000
## page margins        	1000 1000 1000 1000
## page minimum margins	220 110 213 126
## page orientation    	portrait
## pixels per inch     	300 300
## resolution          	300 300
## % selected_attribs
## 
## Attributes for selected printer
## 
## copies              	1
## device              	Acrobat PDFWriter
## first page          	0
## hDC                 	0x72105bc
## hdcname             	printerDc30
## last page           	0
## page dimensions     	11000 8500
## page margins        	1000 1000 1000 1000
## page minimum margins	220 110 213 126
## page orientation    	landscape
## pixels per inch     	300 300
## resolution          	300 300
##
## % selected_attribs
## 
## Attributes for selected printer
## 
## copies              	1
## device              	Apple LaserWriter II NT
## first page          	0
## hDC                 	0x3f210945
## hdcname             	printerDc31
## last page           	0
## page dimensions     	11000 8500
## page margins        	1000 1000 1000 1000
## page minimum margins	220 110 213 126
## page orientation    	landscape
## pixels per inch     	300 300
## resolution          	300 300
## % selected_attribs
## 
## Attributes for selected printer
## 
## copies              	1
## device              	HP DeskJet 540 (Monochrome)
## first page          	0
## hDC                 	0x162112f0
## hdcname             	printerDc32
## last page           	0
## page dimensions     	11000 8500
## page margins        	1000 1000 1000 1000
## page minimum margins	220 110 213 126
## page orientation    	landscape
## pixels per inch     	300 300
## resolution          	300 300
## % selected_attribs
## 
## Attributes for selected printer
## 
## copies              	1
## device              	HP DeskJet 710C
## first page          	0
## hDC                 	0x21210af8
## hdcname             	printerDc33
## last page           	0
## page dimensions     	8500 11000
## page margins        	1000 1000 1000 1000
## page minimum margins	220 110 213 126
## page orientation    	portrait
## pixels per inch     	300 300
## resolution          	300 300
## % selected_attribs
## 
## Attributes for selected printer
## 
## copies              	1
## device              	PS5.1
## first page          	0
## hDC                 	0xf9210d2e
## hdcname             	printerDc34
## last page           	0
## page dimensions     	11000 8500
## page margins        	1000 1000 1000 1000
## page minimum margins	220 110 213 126
## page orientation    	landscape
## pixels per inch     	300 300
## resolution          	300 300
## % selected_attribs
## 
## Attributes for selected printer
## 
## copies              	1
## device              	WinFax
## first page          	0
## hDC                 	0x7211262
## hdcname             	printerDc35
## last page           	0
## page dimensions     	11000 8500
## page margins        	1000 1000 1000 1000
## page minimum margins	220 110 213 126
## page orientation    	landscape
## pixels per inch     	196 204
## resolution          	Low
## % 
################################################################
