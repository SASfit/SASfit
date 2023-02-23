source sasfit_readData.tcl
set infid [open Frolov.c w]
create_ASCIIData sparse
for {set dim 2} {$dim <=6} {incr dim} { 
	for {set level 1} {$level <=16} {incr level} { 
		set fn "Frolov_${dim}_${level}_basic"
		puts "read $fn.dat"
		read_Ascii_Cols $fn.dat sparse $dim
		set header [file rootname $fn.dat]
		set i 0
		puts "generate $header.inc"
		set fid [open ${header}.inc w]
		for {set j 0} {$j < [llength $sparse(0)]} {incr j} {
			for {set i 0} {$i < $dim} {incr i} {
				puts -nonewline $fid "x\[[expr $j*$dim+$i]\]=[lindex $sparse($i) $j]; "
			}
			puts -nonewline $fid "\n"
		}
	puts $infid "int ${header} ( double *x, double *w, int *npoints) \{"
	puts $infid "\t*npoints = [llength $sparse(0)];"
	puts $infid "\t*w = [lindex $sparse(Comment) 3];"
	puts $infid "\tx = ( double * ) malloc ($dim*[llength $sparse(0)] * sizeof (double) );"
	puts $infid "\tif (x==NULL) return 1;"
	puts $infid "\t#include \"${header}.inc\""
	puts $infid "\treturn 0;"
	puts $infid "\}\n"
	close $fid
	}
}
puts $infid "int Frolov(int dim, int level, double *x, double *w, int *npoints) \{"
puts $infid "\tif (dim<2||dim>6||level<1||level>16) return 2;"
for {set dim 2} {$dim <=6} {incr dim} { 
	for {set level 1} {$level <=16} {incr level} {
		puts $infid "\tif (dim==$dim && level==$level) \{"
		puts $infid "\t\treturn Frolov_${dim}_${level}_basis (x,w,npoints);"
		if {$dim==6 && $level == 16} {
			puts $infid "\t\}"
		} else {
			puts -nonewline $infid "\t\} else "
		}
	}
}
puts $infid "\treturn 3;"
puts $infid "\}\n"
close $infid

