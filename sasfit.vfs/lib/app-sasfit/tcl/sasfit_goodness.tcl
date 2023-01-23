proc CreateDivergencyParDataTab {DDivergencyPar args} {
	upvar $DDivergencyPar ap
	foreach  divname $ap(div_name) {
		set ap($divname) 0
		set ap($divname,history) {}
	}
	set ap($ap(div_name)_text_export) ""
	set ap($ap(div_name)_text_display) ""
}

proc RefreshDivergencyParDataTab {DDivergencyPar args} {
	upvar $DDivergencyPar ap
	
	set n $ap(div_name)
	tab_text_update ap $n

	# reconfigure copy command
	$ap(${n}_w).popup entryconfigure 0 -command \
		"tab_text_copy2clipboard_cmd $DDivergencyPar ${n}"
	$ap(${n}_w).popup entryconfigure 1 -command \
		"tab_text_copy2file_cmd $DDivergencyPar ${n}"
		
}