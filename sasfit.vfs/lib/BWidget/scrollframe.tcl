# ------------------------------------------------------------------------------
#  scrollframe.tcl
#  This file is part of Unifix BWidget Toolkit
#  $Id: scrollframe.tcl,v 1.2 2000/02/09 12:23:09 cvs Exp $
# ------------------------------------------------------------------------------
#  Index of commands:
#     - ScrollableFrame::create
#     - ScrollableFrame::configure
#     - ScrollableFrame::cget
#     - ScrollableFrame::getframe
#     - ScrollableFrame::see
#     - ScrollableFrame::xview
#     - ScrollableFrame::yview
#     - ScrollableFrame::_resize
# ------------------------------------------------------------------------------

namespace eval ScrollableFrame {
    Widget::declare ScrollableFrame {
        {-background        TkResource "" 0 frame}
        {-width             Int        0  0 {}}
        {-height            Int        0  0 {}}
        {-areawidth         Int        0  0 {}}
        {-areaheight        Int        0  0 {}}
        {-constrainedwidth  Boolean    0 0}
        {-constrainedheight Boolean    0 0}
        {-xscrollcommand    TkResource "" 0 canvas}
        {-yscrollcommand    TkResource "" 0 canvas}
        {-xscrollincrement  TkResource "" 0 canvas}
        {-yscrollincrement  TkResource "" 0 canvas}
        {-bg                Synonym    -background}
    }

    Widget::addmap ScrollableFrame "" :cmd {
        -background {} -width {} -height {} 
        -xscrollcommand {} -yscrollcommand {}
        -xscrollincrement {} -yscrollincrement {}
    }
    Widget::addmap ScrollableFrame "" .frame {-background {}}

    variable _widget

    bind BwScrollableFrame <Configure> {ScrollableFrame::_resize %W}
    bind BwScrollableFrame <Destroy>   {Widget::destroy %W; rename %W {}}

    proc ::ScrollableFrame { path args } { return [eval ScrollableFrame::create $path $args] }
    proc use {} {}
}


# ------------------------------------------------------------------------------
#  Command ScrollableFrame::create
# ------------------------------------------------------------------------------
proc ScrollableFrame::create { path args } {
    Widget::init ScrollableFrame $path $args

    set canvas [eval canvas $path [Widget::subcget $path :cmd] \
                    -highlightthickness 0 -borderwidth 0 -relief flat]

    set frame  [eval frame $path.frame [Widget::subcget $path .frame] \
                    -highlightthickness 0 -borderwidth 0 -relief flat]

    $canvas create window 0 0 -anchor nw -window $frame -tags win \
        -width  [Widget::cget $path -areawidth] \
        -height [Widget::cget $path -areaheight]

    bind $frame <Configure> "$canvas:cmd configure -scrollregion {0 0 %w %h}"
    bindtags $path [list $path BwScrollableFrame [winfo toplevel $path] all]

    rename $path ::$path:cmd
    proc ::$path { cmd args } "return \[eval ScrollableFrame::\$cmd $path \$args\]"

    return $canvas
}


# ------------------------------------------------------------------------------
#  Command ScrollableFrame::configure
# ------------------------------------------------------------------------------
proc ScrollableFrame::configure { path args } {
    set res [Widget::configure $path $args]
    set upd 0

    set modcw [Widget::hasChanged $path -constrainedwidth cw]
    set modw  [Widget::hasChanged $path -areawidth w]
    if { $modcw || (!$cw && $modw) } {
        if { $cw } {
            set w [winfo width $path]
        }
        set upd 1
    }

    set modch [Widget::hasChanged $path -constrainedheight ch]
    set modh  [Widget::hasChanged $path -areaheight h]
    if { $modch || (!$ch && $modh) } {
        if { $ch } {
            set h [winfo height $path]
        }
        set upd 1
    }

    if { $upd } {
        $path:cmd itemconfigure win -width $w -height $h
    }
    return $res
}


# ------------------------------------------------------------------------------
#  Command ScrollableFrame::cget
# ------------------------------------------------------------------------------
proc ScrollableFrame::cget { path option } {
    return [Widget::cget $path $option]
}


# ------------------------------------------------------------------------------
#  Command ScrollableFrame::getframe
# ------------------------------------------------------------------------------
proc ScrollableFrame::getframe { path } {
    return $path.frame
}


# ------------------------------------------------------------------------------
#  Command ScrollableFrame::see
# ------------------------------------------------------------------------------
proc ScrollableFrame::see { path widget {vert top} {horz left}} {
    set x0  [winfo x $widget]
    set y0  [winfo y $widget]
    set x1  [expr {$x0+[winfo width  $widget]}]
    set y1  [expr {$y0+[winfo height $widget]}]
    set xb0 [$path:cmd canvasx 0]
    set yb0 [$path:cmd canvasy 0]
    set xb1 [$path:cmd canvasx [winfo width  $path]]
    set yb1 [$path:cmd canvasy [winfo height $path]]
    set dx  0
    set dy  0

    if { ![string compare $horz "left"] } {
	if { $x1 > $xb1 } {
	    set dx [expr {$x1-$xb1}]
	}
	if { $x0 < $xb0+$dx } {
	    set dx [expr {$x0-$xb0}]
	}
    } elseif { ![string compare $horz "right"] } {
	if { $x0 < $xb0 } {
	    set dx [expr {$x0-$xb0}]
	}
	if { $x1 > $xb1+$dx } {
	    set dx [expr {$x1-$xb1}]
	}
    }

    if { ![string compare $vert "top"] } {
	if { $y1 > $yb1 } {
	    set dy [expr {$y1-$yb1}]
	}
	if { $y0 < $yb0+$dy } {
	    set dy [expr {$y0-$yb0}]
	}
    } elseif { ![string compare $vert "bottom"] } {
	if { $y0 < $yb0 } {
	    set dy [expr {$y0-$yb0}]
	}
	if { $y1 > $yb1+$dy } {
	    set dy [expr {$y1-$yb1}]
	}
    }

    if { $dx != 0 } {
	set x [expr {($xb0+$dx)/[winfo width $path.frame]}]
	$path:cmd xview moveto $x
    }
    if { $dy != 0 } {
	set y [expr {($yb0+$dy)/[winfo height $path.frame]}]
	$path:cmd yview moveto $y
    }
}


# ------------------------------------------------------------------------------
#  Command ScrollableFrame::xview
# ------------------------------------------------------------------------------
proc ScrollableFrame::xview { path args } {
    return [eval $path:cmd xview $args]
}


# ------------------------------------------------------------------------------
#  Command ScrollableFrame::yview
# ------------------------------------------------------------------------------
proc ScrollableFrame::yview { path args } {
    return [eval $path:cmd yview $args]
}


# ------------------------------------------------------------------------------
#  Command ScrollableFrame::_resize
# ------------------------------------------------------------------------------
proc ScrollableFrame::_resize { path } {
    if { [Widget::getoption $path -constrainedwidth] } {
        $path:cmd itemconfigure win -width [winfo width $path]
    }
    if { [Widget::getoption $path -constrainedheight] } {
        $path:cmd itemconfigure win -height [winfo height $path]
    }
}


