if {![package vsatisfies [package provide Tcl] 8.2]} {return}
package ifneeded math                    1.2.3 [list source [file join $dir math.tcl]]
package ifneeded math::geometry          1.0.3 [list source [file join $dir geometry.tcl]]
package ifneeded math::fuzzy             0.2   [list source [file join $dir fuzzy.tcl]]
package ifneeded math::statistics        0.2   [list source [file join $dir statistics.tcl]]
package ifneeded math::complexnumbers    1.0.1 [list source [file join $dir qcomplex.tcl]]
package ifneeded math::special           0.2   [list source [file join $dir special.tcl]]
package ifneeded math::constants         1.0.1 [list source [file join $dir constants.tcl]]
package ifneeded math::polynomials       1.0.1 [list source [file join $dir polynomials.tcl]]
package ifneeded math::rationalfunctions 1.0.1 [list source [file join $dir rational_funcs.tcl]]
package ifneeded math::fourier           1.0.1 [list source [file join $dir fourier.tcl]]

if {![package vsatisfies [package provide Tcl] 8.3]} {return}
package ifneeded math::roman             1.0   [list source [file join $dir romannumerals.tcl]]

if {![package vsatisfies [package provide Tcl] 8.4]} {return}
package ifneeded math::optimize          1.0   [list source [file join $dir optimize.tcl]]
package ifneeded math::calculus          0.6.1 [list source [file join $dir calculus.tcl]]
package ifneeded math::interpolate       1.0.1 [list source [file join $dir interpolate.tcl]]
package ifneeded math::linearalgebra     1.0   [list source [file join $dir linalg.tcl]]
package ifneeded math::bignum            3.1   [list source [file join $dir bignum.tcl]]
package ifneeded math::bigfloat          1.2   [list source [file join $dir bigfloat.tcl]]
