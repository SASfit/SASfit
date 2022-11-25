# This proc puts the records onto the clipboard as a memory metafile using GDI
proc towmf { records } {

  set hDC [ wmf open -width 12000 -height 9000 -app "Tcl" -title "Test Canvas Items" ]

  foreach record $records {
    set r [ linsert $record 1 $hDC ]
    eval gdi $r
  }

  gdi text $hDC 10 300 -text "GDI commands" -anchor w -justify left -font {Arial 30} -fill green
  gdi text $hDC 10 330 -text "GDI commands" -anchor w -justify left -font {Arial 27} -fill red

  set wmfdc [ wmf close $hDC ]
  wmf copy $wmfdc

  return $wmfdc
}

# This proc puts the records onto the canvas, using Tk's canvas commands
proc tocanvas { records } {
  if [ winfo exist .c ] {
    destroy .c
  }
  pack [ canvas .c -width 12c -height 9c]

  foreach record $records {
    eval .c create $record
  }

  .c create text 100 300 -text "Canvas commands"  -font { Arial 10 } -anchor w -justify left
}

# This proc is not used by this application, but provides a way to look at
# getting a wmf (e.g., by wmf paste -type metafile) and rendering it onto
# a canvas while doing some slight amounts of processing.
proc scaletocanvas { wmf { cw 12c } { ch 9c }  {scaletext 1.0} } {
  global att

  if [ winfo exist .c ] {
    destroy .c
  }
  pack [ canvas .c -width $cw -height $ch]

  set meta [ wmf info $wmf ]
  foreach fld $meta {
    set att([lindex $fld 0]) [lindex $fld 1]
  }
  set scalex [ expr [lindex $att(bounding\ box) 2] * 1.0 / [ lindex $att(bounding\ frame) 2] ]
  set scaley [ expr [lindex $att(bounding\ box) 3] * 1.0 / [ lindex $att(bounding\ frame) 3] ]
  unset meta

  set data [ wmf info $wmf -canvas ] 
  set records [ lindex $data [expr [ llength $data ] - 1 ] ]

  foreach record $records {
    puts "Fields: [llength $record]"
    switch -glob $record {
      text*	{
                  set font  [lindex $record 14] ; puts "Font: $font"
                  set font  [ lreplace $font 1 1 [ expr int(ceil([ lindex $font 1 ] * $scaletext)) ] ]
                  set record [ lreplace $record 14 14 $font ] ; puts "Record: $record"
                  eval .c create $record
                }
      *     { eval .c create $record }

    }
  }
}

proc init { } {
  package require wmf
  package require gdi

  global test_records
  
  lappend test_records " rectangle 10 10 55 22 -fill #ff9185 -outline #000000 -width 2"
  lappend test_records " rectangle 10 22 130 75 -fill #ff9185 -outline #000000 -width 2"

  lappend test_records " rectangle 200 10 255 22 -fill #ff9185 -outline #000000 -width 2"
  lappend test_records " rectangle 200 22 330 75 -fill #ff9185 -outline #000000 -width 2"

  lappend test_records " oval 100 100 250 200 -fill yellow -outline black -width 2"
  lappend test_records " rectangle 98 98 252 202 -outline green -width 2"
  lappend test_records " polygon  175 100 250 150 175 200 100 150 -outline blue -fill pink -width 2"

  lappend test_records " line 130 50 200 50 -arrow last -fill red"

}

proc metafile_to_canvas { wmf } {
  if [ winfo exist .q ] {
    destroy .q
  }
  toplevel .q
  canvas   .q.c -width 500 -height 400
  pack     .q.c
  set result ""

  set x [ wmf info $wmf -canvas ]

  foreach item [ lindex $x 7 ] {
    if [ catch { 
        eval .q.c create $item
        } err ] {
      lappend $result "$err\n"
    }
  }
  return $result
}

init

set wmf [ towmf $test_records ]

tocanvas $test_records

metafile_to_canvas $wmf
