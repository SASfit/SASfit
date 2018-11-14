proc Wong2_mapLinear {args} {
	set red {0 0	1 0		2 213	3 230	4 204	5 0		6 86	7 240	8 255}
	set grn {0 0	1 158	2 94	3 159	4 121	5 114	6 180	7 228	8 255}
	set blu {0 0	1 115	2 0		3 0		4 167	5 178	6 233	7 66	8 255}
	return [mapLinear red grn blu $args]
}

proc Wong1_mapLinear {args} {
	set red {0 0	1 0		2 86	3 204	4 0		5 213	6 230	7 240	8 255}
	set grn {0 0	1 114	2 180	3 121	4 158	5 94	6 159	7 228	8 255}
	set blu {0 0	1 178	2 233	3 167		4 115	5 0		6 0		7 66	8 255}
	return [mapLinear red grn blu $args]
}


proc Tol_mapLinear {args} {
	set red {0 0  1 17   2 51  3 136  4 170  5 204  6 68  7 136  8 221  9 255}
	set grn {0 0  1 119  2 34  3 34   4 68   5 102  6 170 7 204  8 204  9 255}
	set blu {0 0  1 51   2 136 3 85   4 153  5 119  6 153 7 238  8 119  9 255}
	return [mapLinear red grn blu $args]
}

proc IBM_mapLinear {args} {
	set red {0 120 1 100 2 220 3 254 4 255}
	set grn {0 94  1 143 2 38  3 97  4 176}
	set blu {0 240 1 255 2 127 3 0   4 50 }
	return [mapLinear red grn blu $args]
}
proc NRWC_mapLinear {args} {
set nrwc [list \
[list 0.00000 0.01961 0.02353 0.01176] \
[list 0.10000 0.21569 0.21569 0.14118] \
[list 0.20000 0.09412 0.24314 0.16078] \
[list 0.30000 0.20392 0.41176 0.27059] \
[list 0.40000 0.24314 0.54118 0.34902] \
[list 0.50000 0.42353 0.63922 0.38824] \
[list 0.60000 0.64706 0.72941 0.43529] \
[list 0.70000 0.90588 0.83529 0.47843] \
[list 0.80000 0.78039 0.65490 0.36078] \
[list 0.90000 0.69020 0.47059 0.22745] \
[list 1.00000 0.84314 0.49804 0.24706] \
]
return [gpf_mapLinear nrwc $args]
}
proc gpf_mapLinear {gpf args} {
upvar $gpf gnupalettefile

set reverse false
set nlev 256.0
set min 0
set max 1
set CTname unknown
set maxval 1
set what2return RGBhex
foreach {name val} $args {set $name $val}

set red {}
set grn {}
set blu {}

if {$maxval <= 1} {
	set factor 255
} else {
	set factor 1
}
#puts "::gpf_mapLinear: $CTname $factor $maxval"
foreach gpfel $gnupalettefile {
	set fract [expr round([lindex $gpfel 0]*$factor)]
	set rel [expr round([lindex $gpfel 1]*$factor)]
	set gel [expr round([lindex $gpfel 2]*$factor)]
	set bel [expr round([lindex $gpfel 3]*$factor)]
	lappend red $fract $rel
	lappend grn $fract $gel
	lappend blu $fract $bel
}

set scale $fract
#puts "$CTname $scale"
set rr {}
set gg {}
set bb {}
set rgbhex {}
for {set i 0} {$i < $nlev} {incr i} {
	set ri [expr round([::math::interpolate::interp-linear $red [expr $i*1.0*$scale/$nlev]])]
	set gi [expr round([::math::interpolate::interp-linear $grn [expr $i*1.0*$scale/$nlev]])]
	set bi [expr round([::math::interpolate::interp-linear $blu [expr $i*1.0*$scale/$nlev]])]
	if {$ri > 255} {set ri 255}
	if {$gi > 255} {set gi 255}
	if {$bi > 255} {set bi 255}
	if {$ri < 0} {set ri 0}
	if {$gi < 0} {set gi 0}
	if {$bi < 0} {set bi 0}
	lappend rr $ri
	lappend gg $gi
	lappend bb $bi
	lappend rgbhex [format "#%.2x%.2x%.2x" $ri $gi $bi ] 
}
if {$reverse} { 
	set rgbhex [lreverse $rgbhex]
	set rr [lreserve $rr]
	set gg [lreserve $gg]
	set bb [lreserve $bb]
} 
switch $what2return {
	RGBhex {return $rgbhex}
	RGB255 {return [list $rr $gg $bb]}
}
}

proc mapLinear {r g b args} {
upvar $r red
upvar $g grn
upvar $b blu

set reverse false
set nlev 256.0
set what2return RGBhex

foreach {name val} $args {set $name $val}

set scale [expr ([llength $red]/2-1)*1.0]
set rr {}
set gg {}
set bb {}
set rgbhex {}
for {set i 0} {$i < $nlev} {incr i} {
	set ri [expr round([::math::interpolate::interp-linear $red [expr $i*1.0*$scale/$nlev]])]
	set gi [expr round([::math::interpolate::interp-linear $grn [expr $i*1.0*$scale/$nlev]])]
	set bi [expr round([::math::interpolate::interp-linear $blu [expr $i*1.0*$scale/$nlev]])]
	if {$ri > 255} {set ri 255}
	if {$gi > 255} {set gi 255}
	if {$bi > 255} {set bi 255}
	if {$ri < 0} {set ri 0}
	if {$gi < 0} {set gi 0}
	if {$bi < 0} {set bi 0}
	set rr [lappend rr $ri]
	set gg [lappend gg $gi]
	set bb [lappend bb $bi]
	lappend rgbhex [format "#%.2x%.2x%.2x" $ri $gi $bi ] 
}
if {$reverse} { 
	set rgbhex [lreverse $rgbhex]
	set rr [lreserve $rr]
	set gg [lreserve $gg]
	set bb [lreserve $bb]
} 
switch $what2return {
	RGBhex {return $rgbhex}
	RGB255 {return [list $rr $gg $bb]}
}
}

proc IBMmapCubic {args} {
set reverse false
set nlev 255.0
set red [list 120 100 220 254 255]
set grn [list 94  143 38  97  176]
set blu [list 240 255 127 0   0  ]
set idx [list 0   1   2   3   4  ]
foreach {name val} $args {set $name $val}
set Rcoeffs [::math::interpolate::prepare-cubic-splines  $idx  $red]
set Gcoeffs [::math::interpolate::prepare-cubic-splines  $idx  $grn]
set Bcoeffs [::math::interpolate::prepare-cubic-splines  $idx  $blu]

set scale [llength $red]
set rr {}
set gg {}
set bb {}
set rgbhex {}
for {set i 0} {$i < $nlev} {incr i} {
	set ri [expr round([::math::interpolate::interp-cubic-splines $Rcoeffs [expr $i*1.0*$scale/$nlev]])]
	set gi [expr round([::math::interpolate::interp-cubic-splines $Gcoeffs [expr $i*1.0*$scale/$nlev]])]
	set bi [expr round([::math::interpolate::interp-cubic-splines $Bcoeffs [expr $i*1.0*$scale/$nlev]])]
	if {$ri > 255} {set ri 255}
	if {$gi > 255} {set gi 255}
	if {$bi > 255} {set bi 255}
	if {$ri < 0} {set ri 0}
	if {$gi < 0} {set gi 0}
	if {$bi < 0} {set bi 0}
	set rr [lappend rr $ri]
	set gg [lappend gg $gi]
	set bb [lappend bb $bi]
	lappend rgbhex [format "#%.2x%.2x%.2x" $ri $gi $bi ] 
}
if {$reverse} { 
	set rgbhex [lreverse $rgbhex]
} 
return $rgbhex
}

if {[info command lreverse] == ""} {
    proc lreverse list {
        set res {}
        set i [llength $list]
        while {$i} {
            lappend res [lindex $list [incr i -1]]
        }
        set res
    } ;# RS
}


proc cmap {args} {
global cubehelix
set start 0.5
set rot -1.5
set gamma  1
set reverse false
set nlev 256
set minSat 0 
set maxSat 1.2 
set minLight 0
set maxLight 1
set luminosity color
foreach {name val} $args {
	set $name $val
#	puts "set $name $val"
}
if {[info exists startHue]} {
    set sH [expr 1.0*abs($startHue)]
	set sH [expr $sH - round($sH)/360. * 360.]
	set sH [expr ($sH/360.-1.)*3.]
	set start $sh
}
if {[info exists endHue]} {
    set eH [expr 1.0*abs($endHue)]
	set eH [expr $eH - round($eH)/360. * 360.]
	set eH [expr ($eH/360.-$sh/3.-1)]
	set rot $eH
}
if {[info exists sat]} {
	set minSat $sat
	set maxSat $sat
}
#blt::vector destroy fract
#blt::vector destroy angle
#blt::vector destroy satar
#blt::vector destroy amp
#blt::vector destroy red
#blt::vector destroy grn
#blt::vector destroy blu
#blt::vector destroy tmp1
#blt::vector destroy tmp2
update
blt::vector create fract($nlev)
blt::vector create angle($nlev)
blt::vector create satar($nlev)
blt::vector create amp($nlev)
blt::vector create red($nlev)
blt::vector create grn($nlev)
blt::vector create blu($nlev)
blt::vector create tmp1($nlev)
blt::vector create tmp2($nlev)

set pi 3.14159265358979323846
#fract seq 0 1 [expr 1.0/($nlev-1.0)]
fract seq 0 [expr $nlev-1]
fract expr {fract/(1.0*($nlev-1.0))}
angle expr {2.0 * $pi * ($start / 3.0 + $rot * fract + 1.*1.)}
fract seq 0 [expr $nlev-1]
fract expr {$minLight+fract*($maxLight-$minLight)/(1.0*($nlev-1.0))}

fract expr {fract^$gamma}

satar seq 0 [expr $nlev-1]
set step [expr 1.0*($maxSat-$minSat)/($nlev-1.0)]
satar expr { $minSat  + satar*$step}
amp expr {satar * fract * (1. - fract) / 2.}

red expr {fract + amp * (-0.14861 * cos(angle) + 1.78277 * sin(angle))}
grn expr {fract + amp * (-0.29227 * cos(angle) - 0.90649 * sin(angle))}
blu expr {fract + amp * (1.97294  * cos(angle))}

tmp1 expr {red>1}
tmp2 expr {red <= 1}
red expr {red*tmp2+tmp1}
tmp1 expr {grn>1}
tmp2 expr {grn <= 1}
grn expr {grn*tmp2+tmp1}
tmp1 expr {blu>1}
tmp2 expr {blu <= 1}
blu expr {blu*tmp2+tmp1}

tmp1 expr {red>=0}
red expr {red*tmp1}
tmp1 expr {grn>=0}
grn expr {grn*tmp1}
tmp1 expr {blu>=0}
blu expr {blu*tmp1}


set rr {}
set gg {}
set bb {}
set rgbhex {}
for {set i 0} {$i < $nlev} {incr i} {
	set ri [expr round($red($i)*255)]
	set gi [expr round($grn($i)*255)]
	set bi [expr round($blu($i)*255)]
	switch $luminosity {
	color {
#	puts "$i color $ri $gi $bi"
			}
	luminosityNTSC {
				set Y [expr round(0.3*$red($i)*255+0.59*$grn($i)*255+0.11*$blu($i)*255)]
				set ri $Y
				set gi $Y
				set bi $Y
#			puts "$i NTSC $Y"		
				}
	luminosityPAL {
				set Y [expr round(0.299*$red($i)*255+0.587*$grn($i)*255+0.114*$blu($i)*255)]
				set ri $Y
				set gi $Y
				set bi $Y
				}
	luminosityHDTV {
				set Y [expr round(0.2126*$red($i)*255+0.7152*$grn($i)*255+0.0722*$blu($i)*255)]
				set ri $Y
				set gi $Y
				set bi $Y
				}
	lightness {
				set minV [::math::min $red($i)*255 $grn($i)*255 $blu($i)*255]
				set maxV [::math::max $red($i)*255 $grn($i)*255 $blu($i)*255]
				set Y [expr round(($minV+$maxV)/2.0)]
				set ri $Y
				set gi $Y
				set bi $Y
				}
	average {
				set Y [expr round(($red($i)*255+$grn($i)*255+$blu($i)*255)/3.)]
				set ri $Y
				set gi $Y
				set bi $Y
				}
	} 
	set rr [lappend rr $ri]
	set gg [lappend gg $gi]
	set bb [lappend bb $bi]
	lappend rgbhex [format "#%.2x%.2x%.2x" $ri $gi $bi ] 
}
if {$reverse} { 
	set rgbhex [lreverse $rgbhex]
} 
return $rgbhex
}

proc CTgenerator_trace_cmd {args} {
global cubehelix
update
foreach {name val} $args {
    puts "$name $val"
	set $name $val
}
if {[info exists nlev]} {
	set localnlev  $nlev
} else {
	set localnlev 512
}
if {$::cubehelix(divergent)} {
	set ::cubehelix(nlev) [expr round($localnlev/2)]
} else {
	set ::cubehelix(nlev) $localnlev
}
update
set nCT [cmap start $::cubehelix(start) \
               rot   $::cubehelix(rot) \
               gamma   $::cubehelix(gamma) \
               nlev   $::cubehelix(nlev) \
               minSat   $::cubehelix(minSat) \
               maxSat   $::cubehelix(maxSat) \
               minLight   $::cubehelix(minLight) \
               maxLight   $::cubehelix(maxLight) \
			   luminosity color]
set nNTSCBW1 [cmap start $::cubehelix(start) \
               rot   $::cubehelix(rot) \
               gamma   $::cubehelix(gamma) \
               nlev   $::cubehelix(nlev) \
               minSat   $::cubehelix(minSat) \
               maxSat   $::cubehelix(maxSat) \
               minLight   $::cubehelix(minLight) \
               maxLight   $::cubehelix(maxLight) \
			   luminosity luminosityNTSC]
set nPALBW1 [cmap start $::cubehelix(start) \
               rot   $::cubehelix(rot) \
               gamma   $::cubehelix(gamma) \
               nlev   $::cubehelix(nlev) \
               minSat   $::cubehelix(minSat) \
               maxSat   $::cubehelix(maxSat) \
               minLight   $::cubehelix(minLight) \
               maxLight   $::cubehelix(maxLight) \
			   luminosity luminosityPAL]
set nHDTVBW1 [cmap start $::cubehelix(start) \
               rot   $::cubehelix(rot) \
               gamma   $::cubehelix(gamma) \
               nlev   $::cubehelix(nlev) \
               minSat   $::cubehelix(minSat) \
               maxSat   $::cubehelix(maxSat) \
               minLight   $::cubehelix(minLight) \
               maxLight   $::cubehelix(maxLight) \
			   luminosity luminosityHDTV]
set nBW2 [cmap start $::cubehelix(start) \
               rot   $::cubehelix(rot) \
               gamma   $::cubehelix(gamma) \
               nlev   $::cubehelix(nlev) \
               minSat   $::cubehelix(minSat) \
               maxSat   $::cubehelix(maxSat) \
               minLight   $::cubehelix(minLight) \
               maxLight   $::cubehelix(maxLight) \
			   luminosity lightness]
set nBW3 [cmap start $::cubehelix(start) \
               rot   $::cubehelix(rot) \
               gamma   $::cubehelix(gamma) \
               nlev   $::cubehelix(nlev) \
               minSat   $::cubehelix(minSat) \
               maxSat   $::cubehelix(maxSat) \
               minLight   $::cubehelix(minLight) \
               maxLight   $::cubehelix(maxLight) \
			   luminosity average]

set rCT {}
set rBW3 {}
set rBW2 {}
set rNTSCBW1 {}
set rPALBW1 {}
set rHDTVBW1 {}

if {$::cubehelix(divergent)} {
	if {$::cubehelix(symmetric)} {
		set rCT [lreverse $nCT]
		set rBW3 [lreverse $nBW3]
		set rBW2 [lreverse $nBW2]
		set rNTSCBW1 [lreverse $nNTSCBW1]
		set rPALBW1 [lreverse $nPALBW1]
		set rHDTVBW1 [lreverse $nHDTVBW1]
	} else {	   
		set rCT [cmap start [expr $::cubehelix(start)+3*$::cubehelix(rot)] \
               rot   [expr $::cubehelix(rot2)] \
               gamma   $::cubehelix(gamma) \
               nlev   $::cubehelix(nlev) \
               minSat   $::cubehelix(maxSat) \
               maxSat   $::cubehelix(maxSat2) \
               minLight   $::cubehelix(maxLight) \
               maxLight   $::cubehelix(maxLight2) \
			   luminosity color] 
		set rNTSCBW1 [cmap start [expr $::cubehelix(start)+3*$::cubehelix(rot)] \
               rot   [expr $::cubehelix(rot2)] \
               gamma   $::cubehelix(gamma) \
               nlev   $::cubehelix(nlev) \
               minSat   $::cubehelix(maxSat) \
               maxSat   $::cubehelix(maxSat2) \
               minLight   $::cubehelix(maxLight) \
               maxLight   $::cubehelix(maxLight2) \
			   luminosity luminosityNTSC]
		set rPALBW1 [cmap start [expr $::cubehelix(start)+3*$::cubehelix(rot)] \
               rot   [expr $::cubehelix(rot2)] \
               gamma   $::cubehelix(gamma) \
               nlev   $::cubehelix(nlev) \
               minSat   $::cubehelix(maxSat) \
               maxSat   $::cubehelix(maxSat2) \
               minLight   $::cubehelix(maxLight) \
               maxLight   $::cubehelix(maxLight2) \
			   luminosity luminosityPAL]
		set rHDTVBW1 [cmap start [expr $::cubehelix(start)+3*$::cubehelix(rot)] \
               rot   [expr $::cubehelix(rot2)] \
               gamma   $::cubehelix(gamma) \
               nlev   $::cubehelix(nlev) \
               minSat   $::cubehelix(maxSat) \
               maxSat   $::cubehelix(maxSat2) \
               minLight   $::cubehelix(maxLight) \
               maxLight   $::cubehelix(maxLight2) \
			   luminosity luminosityHDTV]
		set rBW2 [cmap start [expr $::cubehelix(start)+3*$::cubehelix(rot)] \
               rot   [expr $::cubehelix(rot2)] \
               gamma   $::cubehelix(gamma) \
               nlev   $::cubehelix(nlev) \
               minSat   $::cubehelix(maxSat) \
               maxSat   $::cubehelix(maxSat2) \
               minLight   $::cubehelix(maxLight) \
               maxLight   $::cubehelix(maxLight2) \
			   luminosity lightness]
		set rBW3 [cmap start [expr $::cubehelix(start)+3*$::cubehelix(rot)] \
               rot   [expr $::cubehelix(rot2)] \
               gamma   $::cubehelix(gamma) \
               nlev   $::cubehelix(nlev) \
               minSat   $::cubehelix(maxSat) \
               maxSat   $::cubehelix(maxSat2) \
               minLight   $::cubehelix(maxLight) \
               maxLight   $::cubehelix(maxLight2) \
			   luminosity average]
	}
}
set data1 {}
set dataNTSCbw1 {}
set dataPALbw1 {}
set dataHDTVbw1 {}
set databw2 {}
set databw3 {}
set data2 [concat $nCT $rCT]
set dataNTSCbwr1 [concat $nNTSCBW1 $rNTSCBW1]
set dataPALbwr1 [concat $nPALBW1 $rPALBW1]
set dataHDTVbwr1 [concat $nHDTVBW1 $rHDTVBW1]
set databwr2 [concat $nBW2 $rBW2]
set databwr3 [concat $nBW3 $rBW3]
if {$::cubehelix(reverse)} {
	set data2 [lreverse $data2]
	set dataNTSCbwr1 [lreverse $dataNTSCbwr1]
	set dataPALbwr1 [lreverse $dataPALbwr1]
	set dataHDTVbwr1 [lreverse $dataHDTVbwr1]
	set databwr2 [lreverse $databwr2]
	set databwr3 [lreverse $databwr3]
}
for {set i 0} {$i<$::cubehelix(wedgehight)} {incr i} {
		lappend data1 $data2
		lappend dataNTSCbw1 $dataNTSCbwr1
		lappend dataPALbw1 $dataPALbwr1
		lappend dataHDTVbw1 $dataHDTVbwr1
		lappend databw2 $databwr2
		lappend databw3 $databwr3
}
if {![info exists nlev]} {
	$::Detector2DIQGraph(CTwedge1) put $data1
	#$::Detector2DIQGraph(CTwedge2) put $data2 
	$::Detector2DIQGraph(AverageWedge1) put $databw3 
	#$::Detector2DIQGraph(AverageWedge2) put $databwr3 
	$::Detector2DIQGraph(LightnessWedge1) put $databw2 
	#$::Detector2DIQGraph(LightnessWedge2) put $databwr2 
	$::Detector2DIQGraph(LuminosityNTSCWedge1) put $dataNTSCbw1
	#$::Detector2DIQGraph(LuminosityNTSCWedge2) put $dataNTSCbwr1
	$::Detector2DIQGraph(LuminosityPALWedge1) put $dataPALbw1
	#$::Detector2DIQGraph(LuminosityPALWedge2) put $dataPALbwr1
	$::Detector2DIQGraph(LuminosityHDTVWedge1) put $dataHDTVbw1
	#$::Detector2DIQGraph(LuminosityHDTVWedge2) put $dataHDTVbwr1
} else {
	set ::Detector2DIQGraph(cubehelix0) $data2
}
return $::cubehelix(divergent)
}

proc CTgenerator {args} {
#^^^^^^^^^^^^^^^^^
global sasfit Detector2DIQGraph cubehelix
set CT rainbow.rgb
if {$::cubehelix(divergent)} {
	set ::cubehelix(nlev) 256
} else {
	set ::cubehelix(nlev) 512
}
set cubehelix(wedgehight) 64
foreach {name val} $args {set $name $val}

set w .cubehelix
if {[winfo exists $w]} {destroy $w}
toplevel $w
wm geometry $w
wm title .cubehelix "cubehelix color lookup table"
raise $w
focus $w

frame $w.frame
set wf $w.frame
label $wf.startlabel -text "start="
entry $wf.startval -textvariable  cubehelix(start) -width 5
grid configure $wf.startlabel -row 0 -column 0  -sticky e
grid configure $wf.startval  -row 0 -column 1 -sticky w

label $wf.rotlabel -text "rot="
entry $wf.rotval -textvariable  cubehelix(rot) -width 5
grid configure $wf.rotlabel -row 0 -column 2  -sticky e
grid configure $wf.rotval  -row 0 -column 3 -sticky w

label $wf.gammalabel -text "gamma="
entry $wf.gammaval -textvariable  cubehelix(gamma) -width 5
grid configure $wf.gammalabel -row 0 -column 4  -sticky e
grid configure $wf.gammaval  -row 0 -column 5 -sticky w

label $wf.reverselabel -text "reverse:"
checkbutton $wf.reverseval -variable  cubehelix(reverse) 
grid configure $wf.reverselabel -row 0 -column 6  -sticky e
grid configure $wf.reverseval  -row 0 -column 7 -sticky w

#label $wf.nlevlabel -text "nlev:"
#entry $wf.nlevval -textvariable  cubehelix(nlev) -width 5
#grid configure $wf.nlevlabel -row 0 -column 6  -sticky e
#grid configure $wf.nlevval  -row 0 -column 7 -sticky w

label $wf.minSatlabel -text "minSat:"
entry $wf.minSatval -textvariable  cubehelix(minSat) -width 5
grid configure $wf.minSatlabel -row 1 -column 0  -sticky e
grid configure $wf.minSatval  -row 1 -column 1 -sticky w

label $wf.maxSatlabel -text "maxSat:"
entry $wf.maxSatval -textvariable  cubehelix(maxSat) -width 5
grid configure $wf.maxSatlabel -row 1 -column 2  -sticky e
grid configure $wf.maxSatval  -row 1 -column 3 -sticky w

label $wf.minLightlabel -text "minLight:"
entry $wf.minLightval -textvariable  cubehelix(minLight) -width 5
grid configure $wf.minLightlabel -row 1 -column 4  -sticky e
grid configure $wf.minLightval  -row 1 -column 5 -sticky w

label $wf.maxLightlabel -text "maxLight:"
entry $wf.maxLightval -textvariable  cubehelix(maxLight) -width 5
grid configure $wf.maxLightlabel -row 1 -column 6  -sticky e
grid configure $wf.maxLightval  -row 1 -column 7 -sticky w

label $wf.divergentlabel -text "divergent:"
checkbutton $wf.divergentval -variable  cubehelix(divergent) 
grid configure $wf.divergentlabel -row 2 -column 0  -sticky e
grid configure $wf.divergentval  -row 2 -column 1 -sticky w

label $wf.symmetriclabel -text "symmetric:"
checkbutton $wf.symmetricval -variable  cubehelix(symmetric) 
grid configure $wf.symmetriclabel -row 2 -column 2  -sticky e
grid configure $wf.symmetricval  -row 2 -column 3 -sticky w

label $wf.maxLight2label -text "maxLight2:"
entry $wf.maxLight2val -textvariable  cubehelix(maxLight2) -width 5
grid configure $wf.maxLight2label -row 2 -column 4  -sticky e
grid configure $wf.maxLight2val  -row 2 -column 5 -sticky w

label $wf.maxSat2label -text "maxSat2:"
entry $wf.maxSat2val -textvariable  cubehelix(maxSat2) -width 5
grid configure $wf.maxSat2label -row 2 -column 6  -sticky e
grid configure $wf.maxSat2val  -row 2 -column 7 -sticky w

label $wf.rot2label -text "rot2="
entry $wf.rot2val -textvariable  cubehelix(rot2) -width 5
grid configure $wf.rot2label -row 2 -column 8  -sticky e
grid configure $wf.rot2val  -row 2 -column 9 -sticky w

frame $w.wframe
frame $w.wframegray1
frame $w.wframegray2
frame $w.wframegray3

set data1 {}
set data2 {}
set dataNTSVbw1 {}
set dataPALbw1 {}
set dataHDTVbw1 {}
set databw2 {}
set databw3 {}
set dataNTSCbwr1 {}
set dataPALbwr1 {}
set dataHDTVbwr1 {}
set databwr2 {}
set databwr3 {}
set rCT [lreverse $::Detector2DIQGraph($CT)]
set rNTSCBW1 [lreverse $::Detector2DIQGraph(CET-L1.tbl)]
set rPALBW1 [lreverse $::Detector2DIQGraph(CET-L1.tbl)]
set rHDTVBW1 [lreverse $::Detector2DIQGraph(CET-L1.tbl)]
set rBW2 [lreverse $::Detector2DIQGraph(CET-L1.tbl)]
set rBW3 [lreverse $::Detector2DIQGraph(CET-L1.tbl)]
for {set i 0} {$i<$cubehelix(wedgehight)} {incr i} {
	lappend data1 $::Detector2DIQGraph($CT)
	lappend data2 $rCT
	lappend dataNTSCbw1 $::Detector2DIQGraph(CET-L1.tbl)
	lappend dataPALbw1 $::Detector2DIQGraph(CET-L1.tbl)
	lappend dataHDTVbw1 $::Detector2DIQGraph(CET-L1.tbl)
	lappend databw2 $::Detector2DIQGraph(CET-L1.tbl)
	lappend databw3 $::Detector2DIQGraph(CET-L1.tbl)
	lappend dataNTSCbwr1 $rNTSCBW1
	lappend dataPALbwr1 $rPALBW1
	lappend dataHDTVbwr1 $rHDTVBW1
	lappend databwr2 $rBW2
	lappend databwr3 $rBW3
}
set ::Detector2DIQGraph(CTwedge1) [image create photo] 
$::Detector2DIQGraph(CTwedge1) put $data1
#set ::Detector2DIQGraph(CTwedge2) [image create photo] 
#$::Detector2DIQGraph(CTwedge2) put $data2 
set ::Detector2DIQGraph(AverageWedge1) [image create photo] 
$::Detector2DIQGraph(AverageWedge1) put $databw3 
#set ::Detector2DIQGraph(AverageWedge2) [image create photo] 
#$::Detector2DIQGraph(AverageWedge2) put $databwr3 
set ::Detector2DIQGraph(LightnessWedge1) [image create photo] 
$::Detector2DIQGraph(LightnessWedge1) put $databw2 
#set ::Detector2DIQGraph(LightnessWedge2) [image create photo] 
#$::Detector2DIQGraph(LightnessWedge2) put $databwr2 
set ::Detector2DIQGraph(LuminosityNTSCWedge1) [image create photo] 
$::Detector2DIQGraph(LuminosityNTSCWedge1) put $dataNTSCbw1
#set ::Detector2DIQGraph(LuminosityNTSCWedge2) [image create photo] 
#$::Detector2DIQGraph(LuminosityNTSCWedge2) put $dataNTSCbwr1
set ::Detector2DIQGraph(LuminosityPALWedge1) [image create photo] 
$::Detector2DIQGraph(LuminosityPALWedge1) put $dataPALbw1
#set ::Detector2DIQGraph(LuminosityPALWedge2) [image create photo] 
#$::Detector2DIQGraph(LuminosityPALWedge2) put $dataPALbwr1
set ::Detector2DIQGraph(LuminosityHDTVWedge1) [image create photo] 
$::Detector2DIQGraph(LuminosityHDTVWedge1) put $dataHDTVbw1
#set ::Detector2DIQGraph(LuminosityHDTVWedge2) [image create photo] 
#$::Detector2DIQGraph(LuminosityHDTVWedge2) put $dataHDTVbwr1
button $w.wframe.wedge1 -image $::Detector2DIQGraph(CTwedge1) -relief flat -bd 0 -padx 0
#button $w.wframe.wedge2 -image $::Detector2DIQGraph(CTwedge2) -relief flat -bd 0 -padx 0
button $w.wframe.averagewedge1 -image $::Detector2DIQGraph(AverageWedge1) -bd 0 -padx 0
#button $w.wframe.averagewedge2 -image $::Detector2DIQGraph(AverageWedge2) -bd 0 -padx 0
button $w.wframe.lightnesswedge1 -image $::Detector2DIQGraph(LightnessWedge1) -bd 0 -padx 0
#button $w.wframe.lightnesswedge2 -image $::Detector2DIQGraph(LightnessWedge2) -bd 0 -padx 0
button $w.wframe.luminosityNTSCwedge1 -image $::Detector2DIQGraph(LuminosityNTSCWedge1) -bd 0 -padx 0 
#button $w.wframe.luminosityNTSCwedge2 -image $::Detector2DIQGraph(LuminosityNTSCWedge2) -bd 0 -padx 0
button $w.wframe.luminosityPALwedge1 -image $::Detector2DIQGraph(LuminosityPALWedge1) -bd 0 -padx 0
#button $w.wframe.luminosityPALwedge2 -image $::Detector2DIQGraph(LuminosityPALWedge2) -bd 0 -padx 0
button $w.wframe.luminosityHDTVwedge1 -image $::Detector2DIQGraph(LuminosityHDTVWedge1) -bd 0 -padx 0
#button $w.wframe.luminosityHDTVwedge2 -image $::Detector2DIQGraph(LuminosityHDTVWedge2) -bd 0 -padx 0
frame $w.actions
button $w.actions.ok -text Exit -command {destroy .cubehelix}
button $w.actions.renew -text Renew -command CTgenerator_trace_cmd
pack $w.frame $w.wframe  $w.actions
pack $w.actions.ok $w.actions.renew -side left

grid configure $w.wframe.wedge1 -column 1 -row 0 -sticky e -ipadx 0 -padx 0
#grid configure $w.wframe.wedge2 -column 2 -row 0 -sticky w -ipadx 0 -padx 0
grid configure $w.wframe.luminosityNTSCwedge1 -column 1 -row 1 -sticky e -ipadx 0 -padx 0
#grid configure $w.wframe.luminosityNTSCwedge2 -column 2 -row 1 -sticky w -ipadx 0 -padx 0
grid configure $w.wframe.luminosityPALwedge1 -column 1 -row 2 -sticky e -ipadx 0 -padx 0
#grid configure $w.wframe.luminosityPALwedge2 -column 2 -row 2 -sticky w -ipadx 0 -padx 0
grid configure $w.wframe.luminosityHDTVwedge1 -column 1 -row 3 -sticky e -ipadx 0 -padx 0
#grid configure $w.wframe.luminosityHDTVwedge2 -column 2 -row 3 -sticky w -ipadx 0 -padx 0
grid configure $w.wframe.averagewedge1 -column 1 -row 4 -sticky e -ipadx 0 -padx 0
#grid configure $w.wframe.averagewedge2 -column 2 -row 4 -sticky w -ipadx 0 -padx 0
grid configure $w.wframe.lightnesswedge1 -column 1 -row 5 -sticky e -ipadx 0 -padx 0
#grid configure $w.wframe.lightnesswedge2 -column 2 -row 5 -sticky w -ipadx 0 -padx 0

#trace variable ::cubehelix(start) w CTgenerator_trace_cmd
#trace variable ::cubehelix(rot) w CTgenerator_trace_cmd
#trace variable ::cubehelix(gamma) w CTgenerator_trace_cmd
#trace variable ::cubehelix(nlev) w CTgenerator_trace_cmd
#trace variable ::cubehelix(minSat) w CTgenerator_trace_cmd
#trace variable ::cubehelix(maxSat) w CTgenerator_trace_cmd
#trace variable ::cubehelix(minLight) w CTgenerator_trace_cmd
#trace variable ::cubehelix(maxLight) w CTgenerator_trace_cmd
#trace variable ::cubehelix(symmetric) w CTgenerator_trace_cmd
#trace variable ::cubehelix(reverse) w CTgenerator_trace_cmd
#trace variable ::cubehelix(divergent) w CTgenerator_trace_cmd

bind .cubehelix <Return> CTgenerator_trace_cmd
CTgenerator_trace_cmd
}