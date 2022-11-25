################################################################
## tstprint.tcl
## $Revision: 1.4 $
## $Log: TSTPRINT.TCL $
## Revision 1.4  1999/09/09 04:56:58  Michael_Schwartz
## *** empty log message ***
##
## Revision 1.3  1999/01/03  09:14:32  Michael_Schwartz
## Removed obsolete GDI calls
##
## Revision 1.2  1997/09/25  07:07:31  Michael_Schwartz
## *** empty log message ***
##
# Revision 1.1  1997/09/02  02:37:35  Michael_Schwartz
# Initial revision
#
################################################################
# Assumes that packages are installed!
package require printer

# Use the GDI package to do direct drawing in windows
package require gdi

lappend result "Printer version: [ printer version ]"
lappend result "GDI version: [ gdi version ]"

if [ catch {
  # Open the printer. In this case, just get the default
  set hdc [printer open]
  # Close it, since we don't necessarily want the default
  printer close
  lappend result "Default printer test OK"
  } err ] {
  # Can't get the default printer. Probably there isn't one.
  lappend result "Can't get default printer. May not be one. ($err)"
}

# Similarly, the printer could be selected using the select dialog:
set retval [printer dialog select]
if [ lindex $retval 1 ] {
  set hdc [ lindex $retval 0 ]
  lappend result "Print dialog OK"
} else {
  set hdc 0x0
  lappend result "Print dialog canceled"
}

# Start the job. Until the job is started, nothing is spooled.
printer job start

# We're using multiple pages--we could let the printer know...
# printer page start

# The supported attributes of the job are contained here, including the DC handle:
set attr [printer attr]

# Now run a few GDI commands to test the job
gdi rectangle $hdc 100 100 200 200

gdi text $hdc 300 300 -anchor w -justify left \
	-text "This is a sample of text output in the default font"

# Now end the page
printer page end

lappend result "First page OK"

# Start a new page
printer page start

# Now add some gdi commands to the same page.
# There are several different mapping modes. We'll reset the default here.
gdi map  $hdc -default
gdi text $hdc 100 250 -anchor w -justify left \
	-text "This is some more information on the new page."


# Now add some postscript to the same page, in a different CTM.
if [ catch {
  # send some raw data to the page
  printer send -data "%!PS\n72 72 moveto\n144 244 lineto\nstroke\n"
  } err ] {
  lappend result "Passthrough escape not supported. Sorry! ($err)"
}

# Now end the job
printer job end

lappend result "Second page completed OK"

# Another job could be started, run, and ended here.

# Closing the printer releases the DC, and it is nulled.
printer close

lappend result "Test complete"

# To have a test complete, execute this statement
# exit 0

# If we are in a Wish, display results in a window. Otherwise, display results through puts
if [ info exist tk_patchLevel ] {
  tk_dialog .t "Print results" [ join $result "\n" ] info 0 OK
  exit 0
} else {
  puts [ join $result "\n" ]
  exit 0
}
