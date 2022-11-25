# sasfit.vfs/lib/app-sasfit/tcl/gdi/testtext.tcl
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

#!/usr/local/bin/wish8.0
#
################################################################
# This test program is to test the ability to generate text in proper sizes
# across a variety of rendering devices.
#
# The fonts should be the same size for the same font, and the horizontal lines
# should match for 300dpi devices.
################################################################

package require gdi
package require printer

proc testprint { hdc loc text fontspec modeargs { curmode -default } { textspec "-anchor nw -justify left"} } {
  eval gdi map $hdc $modeargs
  set ys [ lindex $loc 1]
  set xs [ lindex $loc 0]
  eval gdi line $hdc $xs $ys 10000 $ys -fill blue -width 2
  eval gdi text $hdc $loc -font [ list $fontspec ] $textspec -text [list $text ]
  eval gdi map $hdc $curmode
}

proc point_to_milli { pt } {
  return [ expr - ( ( $pt * 1000 ) / 72.0 ) ]
}

proc testpage_1 { hdc mode xo yf } {
  printer page start

  gdi text $hdc 72 36 -font "Helvetica 12" -text "Testing with mode $mode" -anchor nw -justify left

  testprint $hdc "100  150" "Helvetica 12" "Helvetica 12" "-default"
  testprint $hdc "100  300" "Helvetica 14" "Helvetica 14" "-default"
  testprint $hdc "100  450" "Helvetica 16" "Helvetica 16" "-default"
  testprint $hdc "100  600" "Helvetica 16" "Helvetica 16" "-default"
  testprint $hdc "100  750" "Helvetica 20" "Helvetica 20" "-default"
  testprint $hdc "100  900" "Helvetica 24" "Helvetica 24" "-default"
  testprint $hdc "100 1050" "Helvetica 28" "Helvetica 28" "-default"
  testprint $hdc "100 1200" "Helvetica 32" "Helvetica 32" "-default"
  testprint $hdc "100 1350" "Helvetica 36" "Helvetica 36" "-default"

  testprint $hdc [ list $xo [ expr  150 * $yf ] ]  "Helvetica 12" "Helvetica 12" "$mode"
  testprint $hdc [ list $xo [ expr  300 * $yf ] ]  "Helvetica 14" "Helvetica 14" "$mode"
  testprint $hdc [ list $xo [ expr  450 * $yf ] ]  "Helvetica 16" "Helvetica 16" "$mode"
  testprint $hdc [ list $xo [ expr  600 * $yf ] ]  "Helvetica 16" "Helvetica 16" "$mode"
  testprint $hdc [ list $xo [ expr  750 * $yf ] ]  "Helvetica 20" "Helvetica 20" "$mode"
  testprint $hdc [ list $xo [ expr  900 * $yf ] ]  "Helvetica 24" "Helvetica 24" "$mode"
  testprint $hdc [ list $xo [ expr 1050 * $yf ] ]  "Helvetica 28" "Helvetica 28" "$mode"
  testprint $hdc [ list $xo [ expr 1200 * $yf ] ]  "Helvetica 32" "Helvetica 32" "$mode"
  testprint $hdc [ list $xo [ expr 1350 * $yf ] ]  "Helvetica 36" "Helvetica 36" "$mode"

  printer page end
}

proc ibeam { hdc x0 y0 y1 { barlen 20 } } {
  gdi line $hdc $x0 $y0 $x0 $y1 -fill red -width 2
  gdi line $hdc [ expr $x0 - $barlen ] $y0 [ expr $x0 + $barlen ] $y0 -fill #ff0000
  gdi line $hdc [ expr $x0 - $barlen ] $y1 [ expr $x0 + $barlen ] $y1 -fill red
}

proc ibox { hdc x0 y0 x1 y1 leeway } {
  gdi rectangle $hdc [expr $x0 - $leeway ] [ expr $y0 - $leeway ] [ expr $x1 + $leeway ] [ expr $y1 + $leeway ] -outline red
}

proc marker { hdc x0 y0 { leeway 40 } } {
  set a1 [ expr $x0 - $leeway ]
  set b1 [ expr $y0 - $leeway ]
  set a2 [ expr $x0 + $leeway ]
  set b2 [ expr $y0 + $leeway ]
  gdi line $hdc $a1 $b1 $a2 $b2 -fill red
  gdi line $hdc $a1 $b2 $a2 $b1 -fill red
}

proc testwrap { hdc } {
  printer page start
  gdi text $hdc 100 40 -anchor nw -text "Testing text wrapping and text anchors" -font "Helvetica 10"
  set x0 100
  set y0 100
  set text "We hold these truths to be self-evident:\nThat all men are created equal. That they are endowned by their Creator with certain inalienable rights."

  set hgt [ gdi text $hdc $x0 $y0 -anchor nw -text "default: $text" -font "Helvetica 16" ]
  ibeam $hdc $x0 $y0 [ expr $y0 + $hgt ]
  incr y0 $hgt
  incr y0 10

  set hgt [ gdi text $hdc $x0 $y0 -anchor nw -text "width 100000: $text" -font "Helvetica 16" -width 100000 ]
  ibeam $hdc $x0 $y0 [ expr $y0 + $hgt ]
  incr y0 $hgt
  incr y0 10

  set hgt [ gdi text $hdc $x0 $y0 -anchor nw -text "width 100000: single: $text" -font "Helvetica 16" -width 100000 -single ]
  ibeam $hdc $x0 $y0 [ expr $y0 + $hgt ]
  incr y0 $hgt
  incr y0 50

  # Now, check the anchorage
  set hgt [ gdi text $hdc $x0 $y0 -anchor nw -text "anchor nw: width 2000: $text" -font "Helvetica 14" -width 2000 ]
  ibox $hdc $x0 $y0 [ expr $x0 + 2000 ] [ expr $y0 + $hgt ] 20
  marker $hdc $x0 $y0
  incr y0 $hgt

  incr y0 $hgt
  set hgt [ gdi text $hdc $x0 $y0 -anchor w -text "anchor w:  width 2000: $text" -font "Helvetica 14" -width 2000 ]
  ibox $hdc $x0 [ expr $y0 - ( $hgt / 2.0 ) ] [ expr $x0 + 2000 ] [ expr $y0 + ( $hgt / 2.0 ) ] 20
  marker $hdc $x0 $y0
  incr y0 $hgt

  incr y0 $hgt
  set hgt [ gdi text $hdc $x0 $y0 -anchor sw -text "anchor sw: width 2000: $text" -font "Helvetica 14" -width 2000 ]
  ibox $hdc $x0 [ expr $y0 - $hgt ] [ expr $x0 + 2000 ] [ expr $y0 ] 20
  marker $hdc $x0 $y0
  incr y0 [ expr $hgt / 2 ] 

  set  x0 1100
  set hgt [ gdi text $hdc $x0 $y0 -anchor n -text "anchor n:  width 2000: $text" -font "Helvetica 14" -width 2000 ]
  ibox $hdc [ expr $x0 - 1000 ] [ expr $y0 ] [ expr $x0 + 1000 ] [ expr $y0 + $hgt ] 20
  marker $hdc $x0 $y0
  incr y0 $hgt
  incr y0 80

  incr y0 $hgt
  set hgt [ gdi text $hdc $x0 $y0 -anchor s -text "anchor s:  width 2000: $text" -font "Helvetica 14" -width 2000 ]
  ibox $hdc [ expr $x0 - 1000 ] [ expr $y0 - $hgt ] [ expr $x0 + 1000 ] [ expr $y0 ] 20
  marker $hdc $x0 $y0
  incr y0 [ expr $hgt / 2 ]

  set x0 2100
  set hgt [ gdi text $hdc $x0 $y0 -anchor ne -text "anchor ne: width 2000: $text" -font "Helvetica 14" -width 2000 ]
  ibox $hdc [ expr $x0 - 2000 ]  $y0 $x0 [ expr $y0 + $hgt ] 20
  marker $hdc $x0 $y0
  incr y0 $hgt

  incr y0 $hgt
  set hgt [ gdi text $hdc $x0 $y0 -anchor e -text "anchor e:  width 2000: $text" -font "Helvetica 14" -width 2000 ]
  ibox $hdc [ expr $x0 - 2000 ] [ expr $y0 - ( $hgt / 2.0 ) ] $x0 [ expr $y0 + ( $hgt / 2.0 ) ] 20
  marker $hdc $x0 $y0
  incr y0 $hgt

  incr y0 $hgt
  set hgt [ gdi text $hdc $x0 $y0 -anchor se -text "anchor se: width 2000: $text" -font "Helvetica 14" -width 2000 ]
  ibox $hdc [ expr $x0 - 2000 ]  [ expr $y0 - $hgt ] $x0 [ expr $y0 ] 20
  marker $hdc $x0 $y0
  incr y0 $hgt

  set x0 1100
  set hgt [ gdi text $hdc $x0 $y0 -anchor center -text "anchor c:  width 2000: $text" -font "Helvetica 14" -width 2000 ]
  ibox $hdc [ expr $x0 - 1000 ]  [ expr $y0 - ( $hgt / 2.0 ) ] [ expr $x0 + 1000 ] [ expr $y0 + ( $hgt / 2.0 ) ] 20
  marker $hdc $x0 $y0
  incr y0 $hgt

  printer page end
}

proc testmapoffset { hdc } {
  # This proc will test offset only for the map command, using a text for the test
  set text "Fourscore and seven years ago, our forefathers brought forth on this continent a new nation."
  printer page start
  gdi text $hdc 100 40 -anchor nw -text "Testing gdi map command offset behavior" -font "Helvetica 10"
  set hgt 0

  for { set i 100 } { $i < 2000 } { incr i 200 } {
    for { set j 100 } { $j < 2000 } { incr j $hgt } {
      gdi map $hdc -offset [ list $i $j ]
      set siz [ format "(%4d,%4d): " $i $j ]
      set hgt [ gdi text $hdc 10 10 -text $siz$text -anchor nw -width 180 -font "Helvetica 8" ]
      ibox $hdc 10 10 190 [ expr 10 + $hgt ] 0
      incr hgt 10
      gdi map $hdc -offset [ list -$i -$j ]
    }
  }

  printer page end
}

set hdc [printer dialog select]
if { [llength $hdc] > 1 && [lindex $hdc 1] == 0 } {
  error "User abort"
}

set hdc [ lindex $hdc 0]

printer job start

# First, create a separate page with each mapping mode converted to pixels for comparison
testpage_1 $hdc "-mode MM_HIENGLISH" 4500  -3.3333
testpage_1 $hdc "-mode MM_LOENGLISH" 450   -0.33333
testpage_1 $hdc "-mode MM_HIMETRIC"  11430 -8.4582
testpage_1 $hdc "-mode MM_LOMETRIC"  1143  -0.84582
testpage_1 $hdc "-mode MM_TWIPS"     6120  -4.8000 
testpage_1 $hdc "-physical 5000 -logical 10000" 2500  2.0 
testpage_1 $hdc "-physical {5000 5000} -logical {10000 10000}" 2500  2.0 

testwrap $hdc 

testmapoffset $hdc

printer job end
