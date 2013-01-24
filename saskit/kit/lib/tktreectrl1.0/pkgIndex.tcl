proc ::TreeCtrlLoad {dir} {
	uplevel #0 source [list [file join $dir treectrl.tcl]]
	uplevel #0 source [list [file join $dir filelist-bindings.tcl]]
	load {} Treectrl
}
package ifneeded treectrl 1.0 [list ::TreeCtrlLoad $dir]

