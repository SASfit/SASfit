package require math::special
package require math::constants

trace vdelete ::consiglia(Rg)  w set_WiRgCmd
trace vdelete ::consiglia(nu)  w set_WiRgCmd
trace vdelete ::consiglia(Wi)  w set_WiRgCmd

if {[winfo exists .consiglia]} {destroy .consiglia}
	toplevel .consiglia
	wm geometry .consiglia
	wm title .consiglia "Converting Rg_perp and Wi in Rg_parallel"
	raise .consiglia
	focus .consiglia
#	grab .consiglia
	
#	trace vdelete consiglia(Rg)  w set_WiRgCmd
#	trace vdelete consiglia(nu)  w set_WiRgCmd
#	trace vdelete consiglia(Wi)  w set_WiRgCmd


if {![info exists consiglia]} {
	global consiglia
	set ::consiglia(Rg) 1
	set ::consiglia(nu) 0.5
	set ::consiglia(Wi) 0
	set ::consiglia(Rglin) 0
	set ::consiglia(Rgring) 0
}


proc set_WiRgCmd { name1 name2 opt } {
   global consiglia
	set pi 3.14159265359
	set nu $consiglia(nu)
	set lfac [expr 2*(1+$nu)*(1+2*$nu)/8.*(-4./(1+2*$nu)-4./(1+3*$nu)+9./(1+4*$nu)+8./(1+5*$nu)+4./(1+6*$nu)-2./(1+8*$nu)-16./(1+10.*$nu)+8./(1+12*$nu))]
	set lWi2 [ expr pow($pi,4)*pow($consiglia(Wi),2)/180. ]
	set consiglia(Rglin) [expr $consiglia(Rg)*(1.+$lfac*$lWi2) ]
	set rfac [expr [::math::special::Gamma [expr -2+1.5+2*$nu]]*([::math::special::Gamma [expr -2+0.5+2*$nu]]/(sqrt($pi)*[::math::special::Gamma [expr -2+1.5+4*$nu]])+pow(2.,1.-8*$nu)*[::math::special::Gamma [expr -2+1+6*$nu]]/([::math::special::Gamma [expr -2+1+2*$nu]]*[::math::special::Gamma [expr -2+1.5+6*$nu]])) ]
	set rWi2 [ expr pow($pi,4)*pow($consiglia(Wi),2)/1440. ]
	set consiglia(Rgring) [expr $consiglia(Rg)*(1.+$rfac*$rWi2) ]
	puts "[expr 2/7.] $rfrac"
}

label .consiglia.l_rg -text "Rg =" -width 11 -anchor e
entry .consiglia.v_rg -textvariable ::consiglia(Rg) \
      -relief sunken -width 15 -highlightthickness 0 
	  
label .consiglia.l_wi -text "Wi =" -width 11 -anchor e
entry .consiglia.v_wi -textvariable ::consiglia(Wi) \
      -relief sunken -width 15 -highlightthickness 0 
	
label .consiglia.l_nu -text "nu =" -width 11 -anchor e
entry .consiglia.v_nu -textvariable ::consiglia(nu) \
      -relief sunken -width 15 -highlightthickness 0 
	
	
label .consiglia.l_Rglin_flow -text "Rg(lin,flow) =" -width 11 -anchor e
entry .consiglia.v_Rglin_flow -textvariable ::consiglia(Rglin) \
      -relief sunken -width 15 -highlightthickness 0 

label .consiglia.l_Rgring_flow -text "Rg(ring,flow) =" -width 11 -anchor e
entry .consiglia.v_Rgring_flow -textvariable ::consiglia(Rgring) \
      -relief sunken -width 15 -highlightthickness 0 
	  
grid .consiglia.l_rg -column 0 -row 0  -sticky e
grid .consiglia.v_rg -column 1 -row 0
grid .consiglia.l_nu -column 0 -row 1 -sticky e
grid .consiglia.v_nu -column 1 -row 1
grid .consiglia.l_wi -column 0 -row 2 -sticky e
grid .consiglia.v_wi -column 1 -row 2

grid .consiglia.l_Rglin_flow -column 0 -row 3 -sticky e
grid .consiglia.v_Rglin_flow -column 1 -row 3

grid .consiglia.l_Rgring_flow -column 0 -row 4 -sticky e
grid .consiglia.v_Rgring_flow -column 1 -row 4

trace variable consiglia(Rg)  w set_WiRgCmd
trace variable consiglia(Wi)  w set_WiRgCmd
trace variable consiglia(nu)  w set_WiRgCmd