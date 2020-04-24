package require math::special
package require math::constants


if {[winfo exists .consiglia]} {destroy .consiglia}

toplevel .consiglia
wm geometry .consiglia
wm title .consiglia "Converting parameters into Rg_flow and Rg_vorticity"
raise .consiglia
focus .consiglia

#	grab .consiglia
	
#	trace vdelete consiglia(Rg)  w set_WiRgCmd
#	trace vdelete consiglia(nu)  w set_WiRgCmd
#	trace vdelete consiglia(Wi)  w set_WiRgCmd
#	trace vdelete consiglia(sRg)  w set_slopeRgCmd
#	trace vdelete consiglia(f)  w set_slopeRgCmd
#	trace vdelete consiglia(alpha)  w set_slopeRgCmd
#	trace vdelete consiglia(beta)  w set_slopeRgCmd
#	trace vdelete consiglia(gamma)  w set_slopeRgCmd
#	trace vdelete consiglia(delta)  w set_slopeRgCmd


global consiglia
if {![info exists ::consiglia]} {
	set ::consiglia(Rg) 1
	set ::consiglia(nu) 0.5
	set ::consiglia(Wi) 0
	set ::consiglia(Rglin) 0
	set ::consiglia(Rgring) 0
	set ::consiglia(sRg) 1.0
	set ::consiglia(f) 0.05
	set ::consiglia(alpha) 1
	set ::consiglia(beta) 1
	set ::consiglia(gamma) 1
	set ::consiglia(delta) 1
	set ::consiglia(sRglinflow) 0
	set ::consiglia(sRgringflow) 0
	set ::consiglia(sRglinvorticity) 0
	set ::consiglia(sRgringvorticity) 0
}


proc set_WiRgCmd { name1 name2 opt } {
   global consiglia
	set pi 3.14159265359
	set nu $consiglia(nu)
	set lfac [expr 2*(1+$nu)*(1+2*$nu)/8.*(-4./(1+2*$nu)-4./(1+3*$nu)+9./(1+4*$nu)+8./(1+5*$nu)+4./(1+6*$nu)-2./(1+8*$nu)-16./(1+10.*$nu)+8./(1+12*$nu))]
	set lWi2 [ expr pow($pi,4)*pow($consiglia(Wi),2)/180. ]
	set consiglia(Rglin) [expr $consiglia(Rg)*sqrt(1.+$lfac*$lWi2) ]
	set rfac [expr [::math::special::Gamma [expr -2+1.5+2*$nu]]*([::math::special::Gamma [expr -2+0.5+2*$nu]]/(sqrt($pi)*[::math::special::Gamma [expr -2+1.5+4*$nu]])+pow(2.,1.-8*$nu)*[::math::special::Gamma [expr -2+1+6*$nu]]/([::math::special::Gamma [expr -2+1+2*$nu]]*[::math::special::Gamma [expr -2+1.5+6*$nu]])) ]
	set rWi2 [ expr pow($pi,4)*pow($consiglia(Wi),2)/1440. ]
	set consiglia(Rgring) [expr $consiglia(Rg)*sqrt(1.+$rfac*$rWi2) ]
#	puts "[expr 2/7.] $rfac"
}

proc set_slopeRgCmd { name1 name2 opt } {
   global consiglia
	set pi 3.14159265359
	set f $consiglia(f)
	
	set alpha $consiglia(alpha)
	set beta  $consiglia(beta)
	set gamma $consiglia(gamma)
	set delta $consiglia(delta)
	
	set b [expr $alpha*$alpha]
	set c [expr $gamma*$gamma]
	set lfracf [expr sqrt($c+($b-$c)*(3*$f-3*$f*$f+$f*$f*$f))]
	set b [expr $beta*$beta]
	set c [expr $delta*$delta]
	set lfracv [expr sqrt($c+($b-$c)*(3*$f-3*$f*$f+$f*$f*$f))]

	set consiglia(sRglinflow) [expr $consiglia(sRg)*$lfracf ]
	set consiglia(sRglinvorticity) [expr $consiglia(sRg)*$lfracv ]
#	puts "$consiglia(sRglinflow) $consiglia(sRglinvorticity)"
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

trace vdelete consiglia(Rg)  w set_WiRgCmd
trace vdelete consiglia(nu)  w set_WiRgCmd
trace vdelete consiglia(Wi)  w set_WiRgCmd

label .consiglia.l_srg -text "Rg =" -width 11 -anchor e
entry .consiglia.v_srg -textvariable ::consiglia(sRg) \
      -relief sunken -width 15 -highlightthickness 0 
	  
label .consiglia.l_f -text "f =" -width 11 -anchor e
entry .consiglia.v_f -textvariable ::consiglia(f) \
      -relief sunken -width 15 -highlightthickness 0 
	
label .consiglia.l_alpha -text "alpha =" -width 11 -anchor e
entry .consiglia.v_alpha -textvariable ::consiglia(alpha) \
      -relief sunken -width 15 -highlightthickness 0 
	
label .consiglia.l_beta -text "beta =" -width 11 -anchor e
entry .consiglia.v_beta -textvariable ::consiglia(beta) \
      -relief sunken -width 15 -highlightthickness 0 
	  
label .consiglia.l_gamma -text "gamma =" -width 11 -anchor e
entry .consiglia.v_gamma -textvariable ::consiglia(gamma) \
      -relief sunken -width 15 -highlightthickness 0 
	  
label .consiglia.l_delta -text "delta =" -width 11 -anchor e
entry .consiglia.v_delta -textvariable ::consiglia(delta) \
      -relief sunken -width 15 -highlightthickness 0 
	
label .consiglia.l_sRglin_flow -text "Rg(lin,flow) =" -width 11 -anchor e
entry .consiglia.v_sRglin_flow -textvariable ::consiglia(sRglinflow) \
      -relief sunken -width 15 -highlightthickness 0 

label .consiglia.l_sRgring_flow -text "Rg(ring,flow) =" -width 11 -anchor e
entry .consiglia.v_sRgring_flow -textvariable ::consiglia(sRgringflow) \
      -relief sunken -width 15 -highlightthickness 0 
		
label .consiglia.l_sRglin_vorticity -text "Rg(lin,vorticity) =" -width 15 -anchor e
entry .consiglia.v_sRglin_vorticity -textvariable ::consiglia(sRglinvorticity) \
      -relief sunken -width 15 -highlightthickness 0 

label .consiglia.l_sRgring_vorticity -text "Rg(ring,vorticity) =" -width 15 -anchor e
entry .consiglia.v_sRgring_vorticity -textvariable ::consiglia(sRgringvorticity) \
      -relief sunken -width 15 -highlightthickness 0 
	  
grid .consiglia.l_srg -column 2 -row 0  -sticky e
grid .consiglia.v_srg -column 3 -row 0
grid .consiglia.l_f -column 2 -row 1 -sticky e
grid .consiglia.v_f -column 3 -row 1
grid .consiglia.l_alpha -column 2 -row 2 -sticky e
grid .consiglia.v_alpha -column 3 -row 2
grid .consiglia.l_beta -column 2 -row 3 -sticky e
grid .consiglia.v_beta -column 3 -row 3
grid .consiglia.l_gamma -column 2 -row 4 -sticky e
grid .consiglia.v_gamma -column 3 -row 4
grid .consiglia.l_delta -column 2 -row 5 -sticky e
grid .consiglia.v_delta -column 3 -row 5

grid .consiglia.l_sRglin_flow -column 2 -row 6 -sticky e
grid .consiglia.v_sRglin_flow -column 3 -row 6

grid .consiglia.l_sRgring_flow -column 2 -row 7 -sticky e
grid .consiglia.v_sRgring_flow -column 3 -row 7

grid .consiglia.l_sRglin_vorticity -column 4 -row 6 -sticky e
grid .consiglia.v_sRglin_vorticity -column 5 -row 6

grid .consiglia.l_sRgring_vorticity -column 4 -row 7 -sticky e
grid .consiglia.v_sRgring_vorticity -column 5 -row 7

trace variable consiglia(Rg)  w set_WiRgCmd
trace variable consiglia(nu)  w set_WiRgCmd
trace variable consiglia(Wi)  w set_WiRgCmd

trace variable consiglia(sRg)  w set_slopeRgCmd
trace variable consiglia(f)  w set_slopeRgCmd
trace variable consiglia(alpha)  w set_slopeRgCmd
trace variable consiglia(beta)  w set_slopeRgCmd
trace variable consiglia(gamma)  w set_slopeRgCmd
trace variable consiglia(delta)  w set_slopeRgCmd
