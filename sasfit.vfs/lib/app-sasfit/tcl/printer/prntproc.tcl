################################################################
# A set of procs to print text to a printer using
# the GDI and PRINTER extensions.
# Actually, it would be nice to add one to print HTML....
# These procs require version 0.9.1.1 or newer of GDI and
# 0.7.0.1 or newer of printer extension.
#
# $Log: prntproc.tcl $
# Revision 1.7  2004/12/29 00:15:42  michael
# Fix code computing charwidths for characters that may not exist in the
# enumeration.
# Problem noted by Michael Klier                     <mailto:MichaelKlier@siemens.com>
#
# Revision 1.6  1999/09/09 04:56:58  Michael_Schwartz
# *** empty log message ***
#
# Revision 1.5  1999/02/14  05:49:16  Michael_Schwartz
# Added printer page start and printer page end commands.
#
# Revision 1.4  1998/12/09  06:29:09  Michael_Schwartz
# Added patches by Andreas Sievers <Andreas.Sievers@t-mobil.de> to
# handle wrapped lines properly (not repeat last character on the next
# line).
# Added defaults to the page_args command in case the printer driver
# does not supply values.
#
# Revision 1.3  1998/09/29  03:23:41  Michael_Schwartz
# *** empty log message ***
#
# Revision 1.2  1998/04/27  01:35:37  Michael_Schwartz
# Provide documentary comments and package require statements
#
################################################################

package require gdi
package require printer

proc debug_puts { string } {
  # puts $string
  # update
  # after 100
}

################################################################
## page_args
## Description:
##   This is a helper proc used to parse common arguments for
##   text processing in the other commands.
## Args:
##   Name of an array in which to store the various pieces 
##   needed for text processing
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

  debug_puts "Dimensions: $ary(pw) x $ary(pl) ; Margins (tblr): $ary(tm) $ary(bm) $ary(lm) $ary(rm)"
}

################################################################
## print_page_data
## Description:
##   This is the simplest way to print a small amount of text
##   on a page. The text is formatted in a box the size of the
##   selected page and margins.
## Args:
##   data         Text data for printing
##   fontargs     Optional arguments to supply to the text command
################################################################
proc print_page_data { data {fontargs {}} } {

  global printargs
  page_args printargs
  if { ! [info exist printargs(hDC)] } {
    printer open
    page_args printargs
  }

  set tm [ expr $printargs(tm) * $printargs(resy) / 1000 ]
  set lm [ expr $printargs(lm) * $printargs(resx) / 1000 ]
  set pw [ expr ( $printargs(pw)  - $printargs(rm) ) / 1000 * $printargs(resx) ]
  printer job start
  printer page start
  eval gdi text $printargs(hDC) $lm $tm \
          -anchor nw -text [list $data] \
	  -width $pw \
          $fontargs
  printer page end
  printer job end
}

################################################################
## print_page_file
## Description:
##   This is the simplest way to print a small file
##   on a page. The text is formatted in a box the size of the
##   selected page and margins.
## Args:
##   data         Text data for printing
##   fontargs     Optional arguments to supply to the text command
################################################################
proc print_page_file { filename {fontargs {}} } {
  set fn [open $filename r]

  set data [ read $fn ]

  close $fn

  print_page_data $data $fontargs
}

################################################################
## print_data
## Description:
##   This function prints multiple-page files, using a line-oriented
##   function, taking advantage of knowing the character widths.
##   Many fancier things could be done with it:
##     e.g. page titles, page numbering, user-provided boundary to override
##          page margins, HTML-tag interpretation, etc.
## Args: 
##	data	  Text data for printing
##      breaklines If non-zero, keep newlines in the string as
##                 newlines in the output.
##      font      Font for printing
################################################################
proc print_data { data {breaklines 1 } {font {}} } {
  global printargs

  page_args printargs
  if { ! [info exist printargs(hDC)] } {
    printer open
    page_args printargs
  }
  if { $printargs(hDC) == "?" || $printargs(hDC) == 0 } {
    printer open
    page_args printargs
  }

  if { [string length $font] == 0 } {
    eval gdi characters $printargs(hDC) -array printcharwid
  } else {
    eval gdi characters $printargs(hDC) -font $font -array printcharwid
  }

  set pagewid  [ expr ( $printargs(pw) - $printargs(rm) ) / 1000 * $printargs(resx) ]
  set pagehgt  [ expr ( $printargs(pl) - $printargs(bm) ) / 1000 * $printargs(resy) ]
  set totallen [ string length $data ]
  debug_puts "page width: $pagewid; page height: $pagehgt; Total length: $totallen"
  set curlen 0
  set curhgt [ expr $printargs(tm) * $printargs(resy) / 1000 ]

  printer job start
  printer page start
  while { $curlen < $totallen } {
    set linestring [ string range $data $curlen end ]
    if { $breaklines } {
      set endind [ string first "\n" $linestring ]
      if { $endind != -1 } {
        set linestring [ string range $linestring 0 $endind ] 
        # handle blank lines....
        if { $linestring == "" } { 
	  set linestring " " 
	}
      } 
    } 

    set result [print_page_nextline $linestring \
                                    printcharwid printargs $curhgt $font]
    incr curlen [lindex $result 0]
    incr curhgt [lindex $result 1]
    if { [expr $curhgt + [lindex $result 1] ] > $pagehgt } {
      printer page end
      printer page start
      set curhgt [ expr $printargs(tm) * $printargs(resy) / 1000 ]
    }
  }
  printer page end
  printer job end
}

################################################################
## print_file
## Description:
##   This function prints multiple-page files
##   It will either break lines or just let them run over the 
##   margins (and thus truncate).
##   The font argument is JUST the font name, not any additional
##   arguments.
## Args:
##   filename     File to open for printing
##   breaklines   1 to break lines as done on input, 0 to ignore newlines
##   font         Optional arguments to supply to the text command
################################################################
proc print_file { filename {breaklines 1 } { font {}} } {
  set fn [open $filename r]

  set data [ read $fn ]

  close $fn

  print_data $data $breaklines $font
}

################################################################
## print_page_nextline
##
## Args:
##   string           Data to print
##   parray           Array of values for printer characteristics
##   carray           Array of values for character widths
##   y                Y value to begin printing at
##   font             if non-empty specifies a font to draw the line in
## Return:
##   Returns the pair "chars y"
##   where chars is the number of characters printed on the line
##   and y is the height of the line printed
################################################################
proc print_page_nextline { string carray parray y font } {
  upvar #0 $carray charwidths
  upvar #0 $parray printargs

  set endindex 0
  set totwidth 0
  set maxwidth [ expr ( ( $printargs(pw) - $printargs(rm) ) / 1000 ) * $printargs(resx) ]
  set maxstring [ string length $string ]
  set lm [ expr $printargs(lm) * $printargs(resx) / 1000 ]

  for { set i 0 } { ( $i < $maxstring ) && ( $totwidth < $maxwidth ) } { incr i } {
      set ch [ string index $string $i ]
      if [ info exist charwidths($ch) ] {
          incr totwidth $charwidths([string index $string $i])
      } else {
          incr totwidth $charwidths(n)
      }
      # set width($i) $totwidth
  }

  set endindex $i
  set startindex $endindex

  if { $i < $maxstring } {
    # In this case, the whole data string is not used up, and we wish to break on a 
    # word. Since we have all the partial widths calculated, this should be easy.
    set endindex [ expr [string wordstart $string $endindex] - 1 ]
    set startindex [ expr $endindex + 1 ]

    # If the line is just too long (no word breaks), print as much as you can....
    if { $endindex <= 1 } {
      set endindex $i
      set startindex $i
    }
  }

  if { [string length $font] > 0 } {
    set result [ gdi text $printargs(hDC) $lm $y \
                 -anchor nw -justify left \
                 -text [ string trim [ string range $string 0 $endindex ] "\r\n" ] \
                 -font $font ]
  } else {
    set result [ gdi text $printargs(hDC) $lm $y \
                 -anchor nw -justify left \
                 -text [string trim [ string range $string 0 $endindex ] "\r\n" ] ]
  }

  debug_puts "Maxwidth: $maxwidth; Max String: $maxstring ; Ending at $endindex"
  debug_puts "Printed line at ($lm, $y). Now starting at $startindex"
  return "$startindex $result"
}

