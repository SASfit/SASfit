proc CreateDivergencyParDataTab {DDivergencyPar args} {
	upvar $DDivergencyPar ap
	foreach  divname $ap(div_name) {
		set ap($divname) 0
		set ap($divname,history) {}
	}
}