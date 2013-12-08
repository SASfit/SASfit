#
# altTheme.tcl,v 1.30 2005/09/29 15:34:54 jenglish Exp
#
# Tile widget set: Alternate theme
#

namespace eval tile::theme::alt {

    variable colors
    array set colors {
	-frame 		"#d9d9d9"
	-darker 	"#c3c3c3"
	-activebg 	"#ececec"
	-disabledfg	"#a3a3a3"
	-selectbg	"#4a6984"
	-selectfg	"#ffffff"
    }

    style theme settings alt {

	style configure "." \
	    -background 	$colors(-frame) \
	    -foreground 	black \
	    -troughcolor	$colors(-darker) \
	    -selectbackground 	$colors(-selectbg) \
	    -selectforeground 	$colors(-selectfg) \
	    -font 		TkDefaultFont \
	    ;

	style map "." -background \
	    [list disabled $colors(-frame)  active $colors(-activebg)] ;
	style map "." -foreground [list disabled $colors(-disabledfg)] ;
        style map "." -embossed [list disabled 1] ;

	style configure TButton \
	    -width -11 -padding "1 1" -relief raised -shiftrelief 1 \
	    -highlightthickness 1 -highlightcolor $colors(-frame)

	style map TButton -relief {
	    {pressed !disabled} 	sunken
	    {active !disabled} 	raised
	} -highlightcolor {alternate black}

	style configure TCheckbutton -indicatorcolor "#ffffff"
	style configure TRadiobutton -indicatorcolor "#ffffff"
	style map TCheckbutton -indicatorcolor \
	    [list  disabled $colors(-frame)  pressed $colors(-frame)]
	style map TRadiobutton -indicatorcolor \
	    [list  disabled $colors(-frame)  pressed $colors(-frame)]

	style configure TMenubutton -width -11 -padding "3 3" -relief raised

	style configure TEntry -relief sunken -borderwidth 1 -padding 1
	style map TEntry -fieldbackground \
		[list readonly $colors(-frame) disabled $colors(-frame)]
	style map TCombobox -fieldbackground \
		[list readonly $colors(-frame) disabled $colors(-frame)]

	style configure Toolbutton -relief flat -padding 2
	style map Toolbutton -relief \
	    {disabled flat selected sunken pressed sunken active raised}
	style map Toolbutton -background \
	    [list pressed $colors(-darker)  active $colors(-activebg)]

	style configure TScrollbar -relief raised

	style configure TLabelframe -relief groove -borderwidth 2

	style configure TNotebook -expandtab {2 2 1 0}
	style configure TNotebook.Tab \
	    -padding {4 2} -background $colors(-darker)
	style map TNotebook.Tab -background [list selected $colors(-frame)]

	style configure TScale -groovewidth 4 -troughrelief sunken
	style map TScale -sliderrelief {pressed sunken  {} raised}
	style configure TProgressbar \
	    -background $colors(-selectbg) -borderwidth 0
    }
}
