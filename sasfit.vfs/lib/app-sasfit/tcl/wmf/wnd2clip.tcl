#!/usr/local/bin/wish8.2
#
# This routine puts a given window on the clipboard
#
package require wmf
package require gdi

# Haven't tracked it down yet, but without the -scale argument,
# the clipboard ends up being blank?!
proc window_to_clipboard { window } {
  raise [winfo toplevel $window]
  update
  set emfdc [wmf open]
  set size [gdi copybits $emfdc -window $window -calc]
  eval gdi copybits $emfdc -window $window -source [ list $size ] -scale 1.01
  set emf [wmf close $emfdc]
  wmf copy $emf
  wmf delete $emf
}
