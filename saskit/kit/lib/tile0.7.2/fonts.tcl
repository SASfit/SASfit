#
# fonts.tcl,v 1.10 2005/03/17 22:15:58 jenglish Exp
#
# Tile package: Font specifications.
#
# This file, [source]d from tile.tcl when the package is loaded,
# sets up the following symbolic fonts based on the current platform:
#
# TkDefaultFont	-- default for GUI items not otherwise specified
# TkTextFont	-- font for user text (entry, listbox, others).
# TkHeadingFont	-- headings (column headings, etc)
# TkCaptionFont -- dialog captions (primary text in alert dialogs, etc.)
# TkTooltipFont	-- font to use for tooltip windows
#
# This is a temporary solution until TIP #145 is implemented.
#
# Symbolic fonts listed in TIP #145:
#
# TkDefaultFont	-- the default for all GUI items not otherwise specified.
# TkFixedFont	-- standard fixed width font
# TkMenuFont	-- used for menu items
# TkCaptionFont	-- used for window and dialog caption bars
# TkSmallCaptionFont used for captions on contained windows or tool dialogs.
# TkIconFont	-- font in use for icon captions
# TkTooltipFont	-- font to use for tooltip windows
# 
#
# +++ Platform notes:
#
# Windows:
#	The default system font changed from "MS Sans Serif" to "Tahoma"
# 	in Windows XP/Windows 2000.
#	Should use SystemParametersInfo() instead.
#
# Mac OSX / Aqua:
#	Quoth the Apple HIG: 
#	The _system font_ (Lucida Grande Regular 13 pt) is used for text 
#	in menus, dialogs, and full-size controls.
#	[...] Use the _view font_ (Lucida Grande Regular 12pt) as the default 
#	font of text in lists and tables.
#	[...] Use the _emphasized system font_ (Lucida Grande Bold 13 pt)
#	sparingly. It is used for the message text in alerts.
#	[...] The _small system font_ (Lucida Grande Regular 11 pt) [...]
#	is also the default font for column headings in lists, for help tags,
#	and for small controls.
#
#	Note that the font for column headings (TkHeadingFont) is
#	_smaller_ than the 
#
#	There's also a GetThemeFont() Appearance Manager API call 
#	for looking up kThemeSystemFont dynamically.
#
# Mac classic:
#	Don't know, can't find *anything* on the Web about Mac pre-OSX.
#	Might have used Geneva.  Doesn't matter, this platform
#	isn't supported anymore anyway.
#
# X11:
#	Need a way to tell if Xft is enabled or not.
#	For now, assume patch #971980 applied.
#
#	"Classic" look used Helvetica bold for everything except
#	for entry widgets, which use Helvetica medium.
#	Most other toolkits use medium weight for all UI elements,
#	which is what we do now.
#
#	Font size specified in pixels on X11, not points.
#	This is Theoretically Wrong, but in practice works better; using
#	points leads to huge inconsistencies across different servers.
#

namespace eval tile {

catch {font create TkDefaultFont}
catch {font create TkTextFont}
catch {font create TkHeadingFont}
catch {font create TkCaptionFont}
catch {font create TkTooltipFont}

switch -- [tk windowingsystem] {
    win32 {
	if {$tcl_platform(osVersion) >= 5.0} {
	    variable family "Tahoma"
	} else {
	    variable family "MS Sans Serif"
	}
	variable size 8

	font configure TkDefaultFont -family $family -size $size
	font configure TkTextFont    -family $family -size $size
	font configure TkHeadingFont -family $family -size $size
	font configure TkCaptionFont -family $family -size $size -weight bold
	font configure TkTooltipFont -family $family -size $size
    }
    classic -
    aqua {
	variable family "Lucida Grande"
	variable size 13
	variable viewsize 12
	variable smallsize 11

	font configure TkDefaultFont -family $family -size $size
	font configure TkTextFont    -family $family -size $size
	font configure TkHeadingFont -family $family -size $smallsize
	font configure TkCaptionFont -family $family -size $size -weight bold
	font configure TkTooltipFont -family $family -size $viewsize
    }
    x11 {
	if {![catch {tk::pkgconfig get fontsystem} fs] && $fs eq "xft"} {
	    variable family "sans-serif"
	} else {
	    variable family "Helvetica"
	}
	variable size -12
	variable ttsize -10
	variable capsize -14

	font configure TkDefaultFont -family $family -size $size
	font configure TkTextFont    -family $family -size $size
	font configure TkHeadingFont -family $family -size $size -weight bold
	font configure TkCaptionFont -family $family -size $capsize -weight bold
	font configure TkTooltipFont -family $family -size $ttsize
    }
}

}

#*EOF*
