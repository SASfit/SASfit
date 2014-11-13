#
# winTheme.tcl,v 1.27 2005/09/14 17:24:28 jenglish Exp
#
# Tile widget set: Windows Native theme
#

namespace eval tile {

    style theme settings winnative {

	variable WinGUIFont "{MS Sans Serif} 8"
	if {$tcl_platform(platform) eq "windows"
	    && $tcl_platform(osVersion) >= 5.0} {
	    set WinGUIFont "Tahoma 8"
	}

	style configure "." \
	    -background SystemButtonFace \
	    -foreground SystemWindowText \
	    -selectforeground SystemHighlightText \
	    -selectbackground SystemHighlight \
	    -troughcolor SystemScrollbar \
	    -font $WinGUIFont \
	    ;

	style map "." -foreground [list disabled SystemGrayText] ;
        style map "." -embossed [list disabled 1] ;

	style configure TButton -width -11 -relief raised -shiftrelief 1
	style configure TCheckbutton -padding "0 2"
	style configure TRadiobutton -padding "0 2"
	style configure TMenubutton -padding "8 4" -arrowsize 3 -relief raised

	style map TButton -relief {{!disabled pressed} sunken}

	style configure TEntry \
	    -padding 2 -selectborderwidth 0 -insertwidth 1
	style map TEntry \
	    -fieldbackground \
	    	[list readonly SystemButtonFace disabled SystemButtonFace] \
	    -selectbackground [list !focus "#c3c3c3"] \
	    -selectforeground [list !focus "#000000"] \
	    ;

	style configure TCombobox -padding 2

	style configure TLabelframe -borderwidth 2 -relief groove

	style configure Toolbutton -relief flat -padding {8 4}
	style map Toolbutton -relief \
	    {disabled flat selected sunken  pressed sunken  active raised}

	style configure TScale -groovewidth 4 -sliderrelief raised
	style map TScale -sliderrelief {pressed sunken}

	style configure TNotebook -expandtab {2 2 2 0}
	style configure TNotebook.Tab -padding {3 1} -borderwidth 1

        style configure TProgressbar -borderwidth 0 -background SystemHighlight
    }
}
