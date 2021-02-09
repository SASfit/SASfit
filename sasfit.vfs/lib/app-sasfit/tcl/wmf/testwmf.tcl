#!/usr/local/bin/wish8.2
#
# This test routine creates an enhanced metafile, copies it to the clipboard
# as well as puts it in a file.
#
package require wmf
package require gdi

# A debug puts
proc debug_puts { s } {
    global tcl_interactive
    if { $tcl_interactive != 0 } {
        puts $s
    }
}

proc min { args } {
    set val 0
    if { [llength $args] > 0 } {
        set val [ lindex $args 0 ]
    } else {
        error "min must have arguments"
    }
    foreach arg $args {
        if [ expr $arg < $val ] {
            set val $arg
        }
    }
    return $val
    
}

# A proc to create a canvas out of a WMF
proc wmf_to_canvas { wmf canvas { cw 7c } { ch 7c } { scaletext 1.0 } } {
    if [ winfo exist $canvas ] {
        destroy $canvas
    }
    pack [ canvas $canvas -borderwidth 3 -relief sunken -width $cw -height $ch]
    
    set meta [ wmf info $wmf ]
    foreach fld $meta {
        set att([lindex $fld 0]) [lindex $fld 1]
    }
    set chgt [ $canvas cget -height ]
    set cwid [ $canvas cget -width ]
    set wwid [ expr  [lindex $att(bounding\ box) 2] - [ lindex $att(bounding\ box) 0] ]
    set whgt [ expr  [lindex $att(bounding\ box) 3] - [ lindex $att(bounding\ box) 1] ]
    debug_puts "canvas size $cwid x $chgt ; wmf size $wwid x $whgt "            
    set scalex [ expr ($cwid * 0.9) / $wwid ]
    set scaley [ expr ($chgt * 0.9) / $whgt ]
    set offx [ min [lindex $att(bounding\ box) 2] [ lindex $att(bounding\ box) 0] ]
    set offx [ expr -$offx + 10 ]
    set offy [ min [lindex $att(bounding\ box) 3] [ lindex $att(bounding\ box) 1] ]
    set offy [ expr -$offy + 10 ]
    debug_puts "scaling to $scalex x $scaley ; offsetting by $offx x $offy"
  
    if { $scalex < $scaley && $scaletext == 1.0 } {
        set scaletext $scalex
    } elseif { $scaletext == 1.0 } {
        set scaletext $scaley
    }
    
    unset meta

  set data [ wmf info $wmf -canvas ] 
  set records [ lindex $data [expr [ llength $data ] - 1 ] ]

  foreach record $records {
      # debug_puts "Fields: [llength $record]"
      # Move the anchor coordinates
      switch -glob $record {
          text*	{
              set font_ind -1
              # Find the font record
              for { set ind 0 } { $ind < [llength $record] } { incr ind } {
                  # debug_puts "Examining '[lindex $record $ind]'"
                  if { [lindex $record $ind ] == "-font" } {
                      set font_ind [ expr $ind + 1 ]
                      # debug_puts "Found -font at index $ind"
                      break
                  }
              }
              # debug_puts "Found font at index $font_ind"
              if { $font_ind > 0 } {
                  set font  [lindex $record $font_ind] 
                  # debug_puts "Font: $font"
                  set font  [ lreplace $font 1 1 [ expr int(ceil([ lindex $font 1 ] * $scaletext)) ] ]
                  set record [ lreplace $record $font_ind $font_ind $font ]
              }
              debug_puts "Record: $record"
              set item [ eval $canvas create $record ]
              $canvas move $item $offx $offy
              $canvas scale $item 0 0 $scalex $scaley
     }
          *     { 
              debug_puts "Record: $record"
              set item [ eval $canvas create $record ]
              $canvas move $item $offx $offy
              $canvas scale $item 0 0 $scalex $scaley
          }
          
      }
  }
}

#set hDCname [ wmf open -width 2540 -height 2540 -file c:\\temp\\testwmf.emf ]

# Note: This will cause the metafile to use the "default" context.
# If you wish to make the metafile to use a particular printer context,
# open the printer (printer dialog select) with all the options you wish
# (e.g., color, orientation, etc.) and use that context for the -hdc parameter.
# By leaving off the -width and -height, no clipping will take place and the 
# GDI system will calculate the boundaries for the metafile.
set tnum 0
set hDCname [ wmf open -file testwmf.emf ]
set hDC [hdc addr $hDCname]
while { $hDC == "0x0" && $tnum < 100 } {
    set hDCname [ wmf open -file testwmf$tnum.emf ]

    # Ensure we can still use the number for the HDC as before
    set hDC [hdc addr $hDCname]
    if { $hDC == "0x0" } {
        incr tnum
        continue
    }
    break
}

gdi map $hDC -mode MM_TEXT
gdi line $hDC 60 60 140 140 -arrow both -fill red
gdi oval $hDC 60 70 130 130 -outline blue -fill green 
gdi text $hDC 70 50 -justify center -anchor nw -text "Metafile Greetings DEF" -fill violet

# See what happens if we change mapping modes:
gdi map  $hDC -mode MM_LOMETRIC

# Here, we'll use the HDC name rather than the value...
gdi line $hDCname 1254 254 3500 2500 -arrow both -arrowshape {240 300 90} -fill green
gdi oval $hDCname 1500 500 3000 2000 -outline cyan -fill red
gdi text $hDCname 1270 1270 -justify center -anchor nw -text "Metafile Greetings MM" -fill orange -font {Arial 16}

# Turn the context into a metafile handle
set wmfdc [ wmf close $hDCname ]

# Check out the information
catch {
  set info [ wmf info $wmfdc ]
  if { $tcl_interactive != 0 } {
    puts $info
  }
} err

# Copy to the clipboard
wmf copy $wmfdc

# Copy to a new file
wmf copy $wmfdc -file abc.emf
# abc.emf and testwmf.emf should be identical
wmf copy $wmfdc -type metafile -file abc.wmf
# abc.wmf should be an old-style windows metafile, not identical to abc.emf

# Now we will read back the metafile
set f [ wmf read abc.emf ]

if { [winfo exist .t1] == 0 } {
    toplevel .t1
}
wmf_to_canvas $wmfdc .t1.c1 7c 7c 1.1
if { [ winfo exist .t2 ] == 0 } {
    toplevel .t2
}
wmf_to_canvas $f     .t2.c2 7c 7c 1.1

# Exit, unless run from a wish or tclsh started by hand.
if { $tcl_interactive == 0 } {
  exit
}
