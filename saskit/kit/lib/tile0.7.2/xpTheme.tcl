#
# xpTheme.tcl,v 1.27 2005/09/14 17:24:28 jenglish Exp
#
# Tile widget set: XP Native theme
#
# @@@ todo: spacing and padding needs tweaking

namespace eval tile {

    style theme settings xpnative {

	# NOTE: MS documentation says to use "Tahoma 8" in Windows 2000 / XP,
	# although many MS programs still use "MS Sans Serif 8"
	#
	style configure . -font "Tahoma 8"

	style configure . \
	    -background SystemButtonFace \
	    -foreground SystemWindowText \
	    -selectforeground SystemHighlightText \
	    -selectbackground SystemHighlight \
	    ;

	style map "." \
	    -foreground [list disabled SystemGrayText] \
	    ;

	style configure TButton -padding {1 1} -width -11
	style configure TMenubutton -padding {8 4}

	style configure TNotebook -expandtab {2 2 2 2}

	style configure TLabelframe -foreground "#0046d5"

	# OR: -padding {3 3 3 6}, which some apps seem to use.
	style configure TEntry -padding {2 2 2 4}
	style map TEntry \
	    -selectbackground [list !focus "#c3c3c3"] \
	    -selectforeground [list !focus "#000000"] \
	    ;
	style configure TCombobox -padding 2

	style configure Toolbutton -padding {4 4}
    }
}
