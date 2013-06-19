# sasfit.vfs/lib/app-sasfit/tcl/sasfit_graphlayout.tcl
#
# Copyright (c) 2008-2009, Paul Scherrer Institute (PSI)
#
# This file is part of SASfit.
#
# SASfit is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# SASfit is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with SASfit.  If not, see <http://www.gnu.org/licenses/>.

# Author(s) of this file:
#   Joachim Kohlbrecher (joachim.kohlbrecher@psi.ch)

#------------------------------------------------------------------------------
# set parameter for x-axis like plot range, plot type, color, font ticks ...
#
proc set_xaxis_layout  { GraphPar } {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
upvar $GraphPar Par
global tmpxPar
cp_arr Par tmpxPar
if {[winfo exist .xaxis] } {destroy .xaxis}
toplevel .xaxis
set w .xaxis
wm geometry $w
wm title $w "x-axis layout ($Par(GraphName))"
TitleFrame $w.axis -text "X-Axis Style" -relief ridge -bd 2
pack $w.axis -side top -expand yes -fill both -pady 2m -padx 2m
frame  $w.lay1
pack   $w.lay1 -expand yes -fill x 
frame  $w.lay_button 
pack   $w.lay_button -expand yes -fill x -ipadx 2m -padx 2m -pady 2m
TitleFrame $w.scale -text Scale -relief ridge -bd 2
TitleFrame $w.font  -text "X-Axis Font Style" -relief ridge -bd 2
pack $w.scale $w.font -side left -expand yes -fill both -in $w.lay1 -padx 2m -pady 2m
#
# defining framebox for axis scale inputs
#
set af [$w.axis getframe]
set sf [$w.scale getframe]
set ff [$w.font getframe]

frame $sf.lay_from
pack $sf.lay_from -expand yes -fill x -padx 1m -pady 1m
label $sf.lay_from.fromlabel -text "From:" -width 7 -anchor w \
      -highlightthickness 0
entry $sf.lay_from.fromvalue -textvariable tmpxPar(x,min) -width 8  \
      -highlightthickness 0
pack  $sf.lay_from.fromlabel $sf.lay_from.fromvalue -side left 

frame $sf.lay_to
pack $sf.lay_to -expand yes -fill x -padx 1m -pady 1m
label $sf.lay_to.tolabel -text "To:" -width 7 -anchor w \
      -highlightthickness 0
entry $sf.lay_to.tovalue -textvariable tmpxPar(x,max) -width 8 \
      -highlightthickness 0
pack  $sf.lay_to.tolabel $sf.lay_to.tovalue -side left

frame $sf.lay_type
pack $sf.lay_type -expand yes -fill x -padx 1m -pady 1m

ComboBox $sf.lay_type.typevalue -textvariable tmpxPar(x,type) \
         -width 10  -editable no -highlightthickness 0 \
         -text "Type:" -labelwidth 7 -labelanchor w \
         -values $::sasfit(scale_types_x) \
         -modifycmd {
               global tmpxPar
               set tmpxPar(x,min) ""
               set tmpxPar(x,max) ""
               set tmpxPar(y,min) ""
               set tmpxPar(y,max) ""
         }
pack  $sf.lay_type.typevalue -side left

frame $sf.scale
pack $sf.scale -expand yes -fill x -padx 1m -pady 1m
label $sf.scale.factortxt -text "scaling factor:" -anchor w \
      -highlightthickness 0
entry $sf.scale.factorval -textvariable tmpxPar(x,factor) -width 7  \
      -highlightthickness 0
pack  $sf.scale.factortxt $sf.scale.factorval -side left

frame $sf.lay_loose
pack $sf.lay_loose -expand yes -fill x -padx 1m -pady 1m
label $sf.lay_loose.looselabel -text "Loose:" -width 7 -anchor w \
      -highlightthickness 0
radiobutton $sf.lay_loose.looseyes -text yes \
            -value yes -variable tmpxPar(x,loose) -width 3 \
            -highlightthickness 0 -anchor w
radiobutton $sf.lay_loose.looseno -text no \
            -value no -variable tmpxPar(x,loose) -width 3 \
            -highlightthickness 0 -anchor w
pack $sf.lay_loose.looselabel $sf.lay_loose.looseyes \
     $sf.lay_loose.looseno -side left 
#
# defining font style parameters of axis and labels
#
frame $ff.alay_color
pack $ff.alay_color -expand yes -fill x -padx 1m

SelectColor $ff.alay_color.color -variable tmpxPar(x,tickcolor) \
            -type menubutton 
label $ff.alay_color.label -text "x-axis color:" -width 14 -anchor sw
pack $ff.alay_color.label $ff.alay_color.color -side left -anchor sw
frame $ff.alay_font
pack $ff.alay_font -expand yes -fill x -padx 1m
label $ff.alay_font.label -text "x-axis font:"  -width 14 -anchor sw
SelectFont $ff.alay_font.font -type toolbar \
     -command {
        global tmpxPar
        set tmpxPar(x,tickfont) [.xaxis.font.f.alay_font.font cget -font]
}
$ff.alay_font.font configure -font $tmpxPar(x,tickfont)
pack $ff.alay_font.label $ff.alay_font.font -fill x -side top

frame $ff.xlay_color
pack $ff.xlay_color -expand yes -fill x -padx 1m

SelectColor $ff.xlay_color.color -variable tmpxPar(x,titlecolor) \
            -type menubutton 
label $ff.xlay_color.label -text "x-title color:" -width 14 -height 2 -anchor sw
pack $ff.xlay_color.label $ff.xlay_color.color -side left -anchor sw
frame $ff.xlay_font
pack $ff.xlay_font -expand yes -fill x -padx 1m
label $ff.xlay_font.label -text "x-title font:"  -width 14 -anchor sw

SelectFont $ff.xlay_font.font -type toolbar \
     -command {
        global tmpxPar
        set tmpxPar(x,titlefont) [.xaxis.font.f.xlay_font.font cget -font]    
     }
$ff.xlay_font.font configure -font $tmpxPar(x,titlefont)
pack $ff.xlay_font.label $ff.xlay_font.font -fill x -side top

frame $af.lay_show
pack  $af.lay_show -expand yes -fill x -padx 1m -pady 1m
label $af.lay_show.label -text "Show Ticks:" -width 12 -anchor w \
      -highlightthickness 0
radiobutton $af.lay_show.yes -text yes \
            -value yes -variable tmpxPar(x,showticks) -width 7 \
            -highlightthickness 0 -anchor w
radiobutton $af.lay_show.no -text no \
            -value no -variable tmpxPar(x,showticks) -width 7 \
            -highlightthickness 0 -anchor w
pack $af.lay_show.label $af.lay_show.yes $af.lay_show.no \
     -side left

frame $af.lay_ticks
pack $af.lay_ticks -expand yes -fill x -fill x -padx 1m -pady 1m
label $af.lay_ticks.stepsize_label -text "Step Size:" \
      -highlightthickness 0 -width 12 -anchor w
entry $af.lay_ticks.stepsize_value -textvariable tmpxPar(x,stepsize) \
      -highlightthickness 0 -width 7
label $af.lay_ticks.subticks_label -text " Subticks:" \
      -highlightthickness 0 
entry $af.lay_ticks.subticks_value -textvariable tmpxPar(x,subticks) \
      -highlightthickness 0 -width 7
label $af.lay_ticks.ticklength_label -text "Tick Length:" \
      -highlightthickness 0 
entry $af.lay_ticks.ticklength_value -textvariable tmpxPar(x,ticklength) \
      -highlightthickness 0 -width 7
pack $af.lay_ticks.stepsize_label $af.lay_ticks.stepsize_value \
     -side left
pack $af.lay_ticks.subticks_label $af.lay_ticks.subticks_value \
     $af.lay_ticks.ticklength_label $af.lay_ticks.ticklength_value \
     -side left -padx 1m

frame $af.lay_tit
pack  $af.lay_tit -expand yes -fill x -padx 1m -pady 1m
label $af.lay_tit.label -text "X-Title:" -width 12 -anchor w \
      -highlightthickness 0
entry $af.lay_tit.entry -textvariable tmpxPar(x,title)  \
      -highlightthickness 0
pack  $af.lay_tit.label -side left
pack  $af.lay_tit.entry -side right -fill x -expand yes

#
# Defining OK, Apply, Cancel and Help button
#
button $w.lay_button.ok -text OK -command {
                                     global tmpxPar $tmpxPar(GraphName)
                                     destroy .xaxis
                                     cp_arr tmpxPar $tmpxPar(GraphName)
                                     RefreshGraph $tmpxPar(GraphName)
                                     unset tmpxPar
                                 } \
                           -highlightthickness 0
button $w.lay_button.apply -text Apply -command {
                                     global tmpxPar $tmpxPar(GraphName)
                                     cp_arr tmpxPar $tmpxPar(GraphName)
                                     RefreshGraph $tmpxPar(GraphName)
                                 } \
                           -highlightthickness 0
button $w.lay_button.cancel -text Cancel -command { 
                                     global tmpxPar
                                     unset tmpxPar
                                     destroy .xaxis } \
                            -highlightthickness 0
button $w.lay_button.help -text "Help..." -highlightthickness 0
pack $w.lay_button.ok $w.lay_button.apply $w.lay_button.cancel \
     -in $w.lay_button -side left -fill x -padx 1m -pady 1m
pack $w.lay_button.help -in $w.lay_button -side right -fill x -padx 2m
wm resizable $w no no
tkwait visibility $w
grab $w
raise $w
focus $w
}

#------------------------------------------------------------------------------
#
#
proc set_yaxis_layout  { GraphPar } {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
upvar $GraphPar Par
global tmpPar
cp_arr Par tmpPar
if {[winfo exist .yaxis] } {destroy .yaxis}
toplevel .yaxis
set w .yaxis
wm geometry $w
wm title $w "y-axis layout ($Par(GraphName))"

TitleFrame $w.axis -text "Y-Axis Style" -relief ridge -bd 2
pack $w.axis -side top -expand yes -fill both -pady 2m -padx 2m
frame  $w.lay1
pack   $w.lay1 -expand yes -fill x 
frame  $w.lay_button 
pack   $w.lay_button -expand yes -fill x -ipadx 2m -padx 2m -pady 2m
TitleFrame $w.scale -text Scale -relief ridge -bd 2
TitleFrame $w.font  -text "Y-Axis Font Style" -relief ridge -bd 2
pack $w.scale $w.font -side left -expand yes -fill both -in $w.lay1 -padx 2m -pady 2m
#
# defining framebox for axis scale inputs
#
set af [$w.axis getframe]
set sf [$w.scale getframe]
set ff [$w.font getframe]

frame $sf.lay_from
pack $sf.lay_from -expand yes -fill x -padx 1m -pady 1m
label $sf.lay_from.fromlabel -text "From:" -width 7 -anchor w \
      -highlightthickness 0
entry $sf.lay_from.fromvalue -textvariable tmpPar(y,min) -width 8  \
      -highlightthickness 0
pack  $sf.lay_from.fromlabel $sf.lay_from.fromvalue -side left 

frame $sf.lay_to
pack $sf.lay_to -expand yes -fill x -padx 1m -pady 1m
label $sf.lay_to.tolabel -text "To:" -width 7 -anchor w \
      -highlightthickness 0
entry $sf.lay_to.tovalue -textvariable tmpPar(y,max) -width 8 \
      -highlightthickness 0
pack  $sf.lay_to.tolabel $sf.lay_to.tovalue -side left

frame $sf.lay_type
pack $sf.lay_type -expand yes -fill x -padx 1m -pady 1m

ComboBox $sf.lay_type.typevalue -textvariable tmpPar(y,type) \
         -width 10  -editable no -highlightthickness 0 \
         -text "Type:" -labelwidth 7 -labelanchor w \
         -values $::sasfit(scale_types_y) \
         -modifycmd {
               global tmpPar
               set tmpPar(x,min) ""
               set tmpPar(x,max) ""
               set tmpPar(y,min) ""
               set tmpPar(y,max) ""
         }
pack  $sf.lay_type.typevalue -side left

frame $sf.scale
pack $sf.scale -expand yes -fill x -padx 1m -pady 1m
label $sf.scale.factortxt -text "scaling factor:" -anchor w \
      -highlightthickness 0
entry $sf.scale.factorval -textvariable tmpPar(y,factor) -width 7  \
      -highlightthickness 0
pack  $sf.scale.factortxt $sf.scale.factorval -side left

frame $sf.lay_loose
pack $sf.lay_loose -expand yes -fill x -padx 1m -pady 1m
label $sf.lay_loose.looselabel -text "Loose:" -width 7 -anchor w \
      -highlightthickness 0
radiobutton $sf.lay_loose.looseyes -text yes \
            -value yes -variable tmpPar(y,loose) -width 3 \
            -highlightthickness 0 -anchor w
radiobutton $sf.lay_loose.looseno -text no \
            -value no -variable tmpPar(y,loose) -width 3 \
            -highlightthickness 0 -anchor w
pack $sf.lay_loose.looselabel $sf.lay_loose.looseyes \
     $sf.lay_loose.looseno -side left 
#
# defining font style parameters of axis and labels
#
frame $ff.alay_color
pack $ff.alay_color -expand yes -fill x -padx 1m

SelectColor $ff.alay_color.color -variable tmpPar(y,tickcolor) \
            -type menubutton 
label $ff.alay_color.label -text "y-axis color:" -width 13 -anchor sw
pack $ff.alay_color.label $ff.alay_color.color -side left -anchor sw
frame $ff.alay_font
pack $ff.alay_font -expand yes -fill x -padx 1m
label $ff.alay_font.label -text "y-axis font:"  -width 13 -anchor sw
SelectFont $ff.alay_font.font -type toolbar \
     -command {
        global tmpPar
        set tmpPar(y,tickfont) [.yaxis.font.f.alay_font.font cget -font]
}
$ff.alay_font.font configure -font $tmpPar(y,tickfont)
pack $ff.alay_font.label $ff.alay_font.font -fill x -side top

frame $ff.xlay_color
pack $ff.xlay_color -expand yes -fill x -padx 1m

SelectColor $ff.xlay_color.color -variable tmpPar(y,titlecolor) \
            -type menubutton 
label $ff.xlay_color.label -text "y-title color:" -width 13 -height 2 -anchor sw
pack $ff.xlay_color.label $ff.xlay_color.color -side left -anchor sw
frame $ff.xlay_font
pack $ff.xlay_font -expand yes -fill x -padx 1m
label $ff.xlay_font.label -text "y-title font:"  -width 13 -anchor sw

SelectFont $ff.xlay_font.font -type toolbar \
     -command {
        global tmpPar
        set tmpPar(y,titlefont) [.yaxis.font.f.xlay_font.font cget -font]    
     }
$ff.xlay_font.font configure -font $tmpPar(y,titlefont)
pack $ff.xlay_font.label $ff.xlay_font.font -fill x -side top

frame $af.lay_show
pack  $af.lay_show -expand yes -fill x -padx 1m -pady 1m
label $af.lay_show.label -text "Show Ticks:" -width 12 -anchor w \
      -highlightthickness 0
radiobutton $af.lay_show.yes -text yes \
            -value yes -variable tmpPar(y,showticks) -width 7 \
            -highlightthickness 0 -anchor w
radiobutton $af.lay_show.no -text no \
            -value no -variable tmpPar(y,showticks) -width 7 \
            -highlightthickness 0 -anchor w
pack $af.lay_show.label $af.lay_show.yes $af.lay_show.no \
     -side left

frame $af.lay_ticks
pack $af.lay_ticks -expand yes -fill x -fill x -padx 1m -pady 1m
label $af.lay_ticks.stepsize_label -text "Step Size:" \
      -highlightthickness 0 -width 12 -anchor w
entry $af.lay_ticks.stepsize_value -textvariable tmpPar(y,stepsize) \
      -highlightthickness 0 -width 7
label $af.lay_ticks.subticks_label -text " Subticks:" \
      -highlightthickness 0 
entry $af.lay_ticks.subticks_value -textvariable tmpPar(y,subticks) \
      -highlightthickness 0 -width 7
label $af.lay_ticks.ticklength_label -text "Tick Length:" \
      -highlightthickness 0 
entry $af.lay_ticks.ticklength_value -textvariable tmpPar(y,ticklength) \
      -highlightthickness 0 -width 7
pack $af.lay_ticks.stepsize_label $af.lay_ticks.stepsize_value \
     -side left
pack $af.lay_ticks.subticks_label $af.lay_ticks.subticks_value \
     $af.lay_ticks.ticklength_label $af.lay_ticks.ticklength_value \
     -side left -padx 1m

frame $af.lay_tit
pack  $af.lay_tit -expand yes -fill x -padx 1m -pady 1m
label $af.lay_tit.label -text "Y-Title:" -width 12 -anchor w \
      -highlightthickness 0
entry $af.lay_tit.entry -textvariable tmpPar(y,title)  \
      -highlightthickness 0
pack  $af.lay_tit.label -side left
pack  $af.lay_tit.entry -side right -fill x -expand yes

#
# Defining OK, Apply, Cancel and Help button
#
button $w.lay_button.ok -text OK -command {
                                     global tmpPar $tmpPar(GraphName)
                                     destroy .yaxis
                                     cp_arr tmpPar $tmpPar(GraphName)
                                     RefreshGraph $tmpPar(GraphName)
                                     unset tmpPar
                                 } \
                           -highlightthickness 0
button $w.lay_button.apply -text Apply -command {
                                     global tmpPar $tmpPar(GraphName)
                                     cp_arr tmpPar $tmpPar(GraphName)
                                     RefreshGraph $tmpPar(GraphName)
                                 } \
                           -highlightthickness 0
button $w.lay_button.cancel -text Cancel -command { 
                                     global tmpPar
                                     unset tmpPar
                                     destroy .yaxis } \
                            -highlightthickness 0
button $w.lay_button.help -text "Help..." -highlightthickness 0
pack $w.lay_button.ok $w.lay_button.apply $w.lay_button.cancel \
     -in $w.lay_button -side left -fill x -padx 1m -pady 1m
pack $w.lay_button.help -in $w.lay_button -side right -fill x -padx 2m
wm resizable $w no no
tkwait visibility $w
grab $w
raise $w
focus $w
}


#------------------------------------------------------------------------------
#
#
proc set_graph_layout  { GraphPar } {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
upvar $GraphPar Par
global tmpPar
cp_arr Par tmpPar
if {[winfo exist .graph] } {destroy .graph}
toplevel .graph
set w .graph
wm geometry $w
wm title $w "graph layout ($Par(GraphName))"

#
# defining frames
#
TitleFrame $w.tit -text "Title" -relief ridge -bd 2
pack  $w.tit -expand yes -fill both \
      -padx 2m -pady 2m -ipadx 2m -ipady 2m
frame $w.bm_lay
frame $w.button_lay  -bd 2
frame $w.title
pack  $w.title -expand yes -fill both \
      -in [$w.tit getframe]
pack  $w.bm_lay $w.button_lay -expand yes -fill both \
      -padx 2m -pady 2m

#TitleFrame $w.bm_lay.margins -text "Margins" -relief ridge -bd 2
#set mf [$w.bm_lay.margins getframe]
#TitleFrame $w.bm_lay.border  -text "Border"  -relief ridge -bd 2
#set bf [$w.bm_lay.border getframe]
#pack $w.bm_lay.border $w.bm_lay.margins \
#     -side left -fill both -expand yes

#
# defining entry for graph title
#
frame $w.title.txt
frame $w.title.f
frame $w.title.c
pack $w.title.txt $w.title.f -fill x -expand yes

label $w.title.txt.label \
      -text "Title:" -width 6 -anchor w -highlightthickness 0
entry $w.title.txt.entry \
      -textvariable tmpPar(title) -highlightthickness 0 -width 35
pack  $w.title.txt.label -side left
pack  $w.title.txt.entry -side right -fill x -expand yes -pady 2m

label $w.title.f.label -text "Font:"  -width 6 -anchor sw
SelectFont $w.title.f.font -type toolbar  \
     -command {
        global tmpPar
        set tmpPar(font) [.graph.title.f.font cget -font]
}
$w.title.f.font configure -font $tmpPar(font)

pack $w.title.f.label $w.title.f.font -side left -fill both

label $w.title.cl -text "Color:" -anchor w
pack $w.title.cl   -anchor sw -fill x -expand yes
pack $w.title.c -fill x 

label $w.title.c.fg  -text "foreground:" -anchor sw
label $w.title.c.bg  -text "background:" -anchor sw
label $w.title.c.pbg -text "plot background:" -anchor sw
label $w.title.c.fbg -text "fit background:" -anchor sw
SelectColor $w.title.c.fgc -variable tmpPar(graph,fg) \
            -type menubutton 
SelectColor $w.title.c.bgc -variable tmpPar(graph,bg) \
            -type menubutton
SelectColor $w.title.c.pbgc -variable tmpPar(plot,background) \
            -type menubutton
SelectColor $w.title.c.fbgc -variable tmpPar(fit,background) \
            -type menubutton
pack $w.title.c.fg $w.title.c.fgc \
     $w.title.c.bg $w.title.c.bgc \
     $w.title.c.pbg $w.title.c.pbgc \
     $w.title.c.fbg $w.title.c.fbgc \
     -side left -fill x -expand yes

#
# defining border, margin, label, grid, crosshair parameters
#
set wn [NoteBook $w.bm_lay.notebook]
pack $wn

set border_f    [$wn insert 1 1  -text border    -state normal ]
set margin_f    [$wn insert 2 2  -text margin    -state normal ]
set legend_f    [$wn insert 3 3  -text legend    -state normal ]
set grid_f      [$wn insert 4 4  -text grid      -state normal ]
set crosshair_f [$wn insert 5 5  -text crosshair -state normal ]

#
# defining border parameters
#
set wl $border_f
frame $wl.lay_graphrelief
grid $wl.lay_graphrelief -row 0 -column 0
ComboBox $wl.lay_graphrelief.relief \
         -textvariable tmpPar(graph,relief) \
         -width 7  -editable no \
         -highlightthickness 0 \
         -text "Graph Relief:" -labelwidth 11 -labelanchor w \
         -values {"raised" "sunken" "flat" "ridge" "solid" "groove"}
pack $wl.lay_graphrelief.relief -side left

frame $wl.lay_graphbd
grid $wl.lay_graphbd -row 0 -column 1
label $wl.lay_graphbd.label -text "Graph Borderwidth:" -width 18 -anchor w \
      -highlightthickness 0
entry $wl.lay_graphbd.entry -textvariable tmpPar(graph,borderwidth) -width 8 \
      -highlightthickness 0
pack  $wl.lay_graphbd.label $wl.lay_graphbd.entry -side left

frame $wl.lay_plotrelief
grid $wl.lay_plotrelief -row 1 -column 0
ComboBox $wl.lay_plotrelief.relief \
         -textvariable tmpPar(plot,relief) \
         -width 7  -editable no \
         -highlightthickness 0 \
         -text "Plot Relief:" -labelwidth 11 -labelanchor w \
         -values {"raised" "sunken" "flat" "ridge" "solid" "groove"}
pack $wl.lay_plotrelief.relief -side left

frame $wl.lay_plotbd
grid $wl.lay_plotbd -row 1 -column 1
label $wl.lay_plotbd.label -text "Plot Borderwidth:" -width 18 -anchor w \
      -highlightthickness 0
entry $wl.lay_plotbd.entry -textvariable tmpPar(plot,borderwidth) -width 8 \
      -highlightthickness 0
pack  $wl.lay_plotbd.label $wl.lay_plotbd.entry -side left
grid columnconfigure $wl 0 -pad 2m
grid columnconfigure $wl 1 -pad 2m
grid rowconfigure $wl 0 -pad 2m
grid rowconfigure $wl 1 -pad 2m

#
# defining margin parameters
#
set wl $margin_f
frame $wl.lay_bottom 
grid $wl.lay_bottom -row 0 -column 0
label $wl.lay_bottom.label -text "Bottom:" -width 7 \
      -highlightthickness 0 -anchor w
entry $wl.lay_bottom.entry -textvariable tmpPar(margin,bottom) -width 8 \
      -highlightthickness 0
pack  $wl.lay_bottom.label $wl.lay_bottom.entry -side left

frame $wl.lay_top
grid $wl.lay_top -row 1 -column 0 
label $wl.lay_top.label -text "Top:" -width 7 \
      -highlightthickness 0 -anchor w
entry $wl.lay_top.entry -textvariable tmpPar(margin,top) -width 8 \
      -highlightthickness 0
pack  $wl.lay_top.label $wl.lay_top.entry -side left

frame $wl.lay_left
grid $wl.lay_left -row 0 -column 1
label $wl.lay_left.label -text "Left:" -width 6 \
      -highlightthickness 0 -anchor w
entry $wl.lay_left.entry -textvariable tmpPar(margin,left) -width 8 \
      -highlightthickness 0
pack  $wl.lay_left.label $wl.lay_left.entry -side left

frame $wl.lay_right
grid $wl.lay_right -row 1 -column 1
label $wl.lay_right.label -text "Right:" -width 6 \
      -highlightthickness 0 -anchor w
entry $wl.lay_right.entry -textvariable tmpPar(margin,right) -width 8 \
      -highlightthickness 0
pack  $wl.lay_right.label $wl.lay_right.entry -side left

frame $wl.lay_ppx
grid $wl.lay_ppx -row 0 -column 2
label $wl.lay_ppx.label -text "Plot padx:" -width 8 \
      -highlightthickness 0 -anchor w
entry $wl.lay_ppx.entry -textvariable tmpPar(margin,plotpadx) -width 8 \
      -highlightthickness 0
pack  $wl.lay_ppx.label $wl.lay_ppx.entry -side left

frame $wl.lay_ppy
grid $wl.lay_ppy -row 1 -column 2
label $wl.lay_ppy.label -text "Plot pady:" -width 8 \
      -highlightthickness 0 -anchor w
entry $wl.lay_ppy.entry -textvariable tmpPar(margin,plotpady) -width 8 \
      -highlightthickness 0
pack  $wl.lay_ppy.label $wl.lay_ppy.entry -side left

grid columnconfigure $wl 0 -pad 2m
grid columnconfigure $wl 1 -pad 2m
grid columnconfigure $wl 2 -pad 2m
grid rowconfigure $wl 0 -pad 2m
grid rowconfigure $wl 1 -pad 2m

#
# defining legend parameters
#
set wl $legend_f
frame $wl.legendfont
grid  $wl.legendfont -row 0 -column 0 -columnspan 3 -sticky w -padx 3
label $wl.legendfont.label -text "Font:"  -width 5 -anchor sw
SelectFont $wl.legendfont.font -type toolbar  \
     -command {
        global tmpPar
        set tmpPar(l,font) [.graph.bm_lay.notebook.f3.legendfont.font cget -font]
} 
pack $wl.legendfont.label $wl.legendfont.font -side left

frame $wl.hide
pack $wl.hide -in $wl.legendfont -side right -padx 5m
label $wl.hide.label -text "hide:" -width 5 -ancho w
checkbutton $wl.hide.onoff \
      -variable tmpPar(l,hide) \
      -onvalue yes -offvalue no -ancho w
pack  $wl.hide.label $wl.hide.onoff -side left


frame $wl.fg
grid  $wl.fg -row 1 -column 0 -sticky w -padx 3
label $wl.fg.label -width 13 -text "foreground:" -anchor w
SelectColor $wl.fg.color -type menubutton -variable tmpPar(l,fg) 
pack $wl.fg.label $wl.fg.color -side left

frame $wl.bg
grid  $wl.bg -row 2 -column 0 -sticky w -padx 3
label $wl.bg.label -width 13 -text "background:" -anchor w
SelectColor $wl.bg.color -type menubutton -variable tmpPar(l,bg) 
pack $wl.bg.label $wl.bg.color -side left

frame $wl.relief
grid $wl.relief -row 3 -column 0 -sticky w -padx 3
ComboBox $wl.relief.cb \
         -width 7 -editable no \
         -highlightthickness 0 \
         -labelwidth 13 \
         -textvariable tmpPar(l,relief) \
         -text "Relief:" -labelanchor w \
         -values {raised sunken flat ridge solid groove}
pack $wl.relief.cb  -fill x -anchor w -side left 

frame $wl.borderwidth
grid $wl.borderwidth -row 4 -column 0 -sticky w -padx 3
label $wl.borderwidth.label -text "border width:" -width 13 \
      -highlightthickness 0 -anchor w
entry $wl.borderwidth.entry -textvariable tmpPar(l,borderwidth) -width 8 \
      -highlightthickness 0
pack  $wl.borderwidth.label $wl.borderwidth.entry -side left

frame $wl.padx
grid $wl.padx -row 1 -column 1 -sticky w -padx 3
label $wl.padx.label -text "padx:" -width 6 \
      -highlightthickness 0 -anchor w
entry $wl.padx.entry -textvariable tmpPar(l,padx) -width 8 \
      -highlightthickness 0
pack  $wl.padx.label $wl.padx.entry -side left

frame $wl.pady
grid $wl.pady -row 2 -column 1 -sticky w -padx 3
label $wl.pady.label -text "pady:" -width 6 \
      -highlightthickness 0 -anchor w
entry $wl.pady.entry -textvariable tmpPar(l,pady) -width 8 \
      -highlightthickness 0
pack  $wl.pady.label $wl.pady.entry -side left

frame $wl.ipadx
grid $wl.ipadx -row 3 -column 1 -sticky w -padx 3
label $wl.ipadx.label -text "ipadx:" -width 6 \
      -highlightthickness 0 -anchor w
entry $wl.ipadx.entry -textvariable tmpPar(l,ipadx) -width 8 \
      -highlightthickness 0
pack  $wl.ipadx.label $wl.ipadx.entry -side left

frame $wl.ipady
grid $wl.ipady -row 4 -column 1 -sticky w -padx 3
label $wl.ipady.label -text "ipady:" -width 6 \
      -highlightthickness 0 -anchor w
entry $wl.ipady.entry -textvariable tmpPar(l,ipady) -width 8 \
      -highlightthickness 0
pack  $wl.ipady.label $wl.ipady.entry -side left

frame $wl.anchor
grid $wl.anchor -row 1 -column 2 -sticky w -padx 3
ComboBox $wl.anchor.cb \
         -width 7 -editable yes \
         -highlightthickness 0 \
         -labelwidth 9 \
         -textvariable tmpPar(l,anchor) \
         -text "anchor:" -labelanchor w \
         -values {n s w e ne nw se sw center}
pack $wl.anchor.cb  -fill x -anchor w -side left 

frame $wl.position
grid $wl.position -row 2 -column 2 -sticky w -padx 3
ComboBox $wl.position.cb \
         -width 7 -editable no \
         -highlightthickness 0 \
         -labelwidth 9 \
         -textvariable tmpPar(l,position) \
         -text "Position:" -labelanchor w \
         -values {top bottom left right @x,y plotarea}
pack $wl.position.cb  -fill x -anchor w -side left 

frame $wl.posx
grid $wl.posx -row 3 -column 2 -sticky w -padx 3
label $wl.posx.label -text "pos x:" -width 9 \
      -highlightthickness 0 -anchor w
entry $wl.posx.entry -textvariable tmpPar(l,posx) -width 8 \
      -highlightthickness 0
pack  $wl.posx.label $wl.posx.entry -side left

frame $wl.posy
grid $wl.posy -row 4 -column 2 -sticky w -padx 3
label $wl.posy.label -text "pos y:" -width 9 \
      -highlightthickness 0 -anchor w
entry $wl.posy.entry -textvariable tmpPar(l,posy) -width 8 \
      -highlightthickness 0
pack  $wl.posy.label $wl.posy.entry -side left

grid columnconfigure $wl 0 -pad 2m
grid columnconfigure $wl 1 -pad 2m
grid columnconfigure $wl 2 -pad 2m
grid rowconfigure $wl 0 -pad 2m
#grid rowconfigure $wl 1 -pad 1m
#grid rowconfigure $wl 2 -pad 1m2
#grid rowconfigure $wl 3 -pad 1m
#grid rowconfigure $wl 4 -pad 1m

#
# defining grid parameters
#
set wl $grid_f

frame $wl.gridcolor
grid  $wl.gridcolor -row 0 -column 0 -sticky w -padx 3
label $wl.gridcolor.label -width 10  -text "Color:" -anchor w
SelectColor $wl.gridcolor.color -type menubutton -variable tmpPar(g,color) 
pack $wl.gridcolor.label $wl.gridcolor.color -side left

frame $wl.gridlinewidth
grid  $wl.gridlinewidth -row 1 -column 0 -sticky w -padx 3
label $wl.gridlinewidth.label -width 10 -text "linewidth:" -anchor w
entry $wl.gridlinewidth.entry -textvariable tmpPar(g,linewidth) -width 4 \
      -highlightthickness 0 
pack  $wl.gridlinewidth.label $wl.gridlinewidth.entry -side left

frame $wl.dashshape
grid $wl.dashshape -row 0 -column 1 -sticky w -padx 3
set tmpPar(dummy) $wl.dashshape.cp
ComboBox $wl.dashshape.cb \
         -width 7  \
         -highlightthickness 0 \
         -labelwidth 10 \
         -textvariable tmpPar(g,dash) \
         -text "Dash:" -labelanchor w \
         -values $tmpPar(dashpattern) 
pack $wl.dashshape.cb  -fill x -anchor w -padx 1m -pady 1m 

frame $wl.hide
grid  $wl.hide -row 0 -column 2 -sticky w -padx 3
label $wl.hide.label -text "hide:" -width 6 -ancho w
checkbutton $wl.hide.onoff \
      -variable tmpPar(g,hide) \
      -onvalue yes -offvalue no -ancho w
pack  $wl.hide.label $wl.hide.onoff -side left

frame $wl.minor
grid  $wl.minor -row 1 -column 2 -sticky w -padx 3
label $wl.minor.label -text "minor:" -width 6 -ancho w
checkbutton $wl.minor.onoff \
      -variable tmpPar(g,minor) \
      -onvalue yes -offvalue no -ancho w
pack  $wl.minor.label $wl.minor.onoff -side left

grid columnconfigure $wl 0 -pad 2m
grid columnconfigure $wl 1 -pad 2m
grid columnconfigure $wl 2 -pad 2m
grid rowconfigure $wl 0 -pad 2m
grid rowconfigure $wl 1 -pad 2m
grid rowconfigure $wl 2 -pad 2m

#
# defining crosshair parameters
#
set wl $crosshair_f

frame $wl.crosscolor
grid  $wl.crosscolor -row 0 -column 0 -sticky w -padx 3
label $wl.crosscolor.label -width 10  -text "Color:" -anchor w
SelectColor $wl.crosscolor.color -type menubutton -variable tmpPar(c,color) 
pack $wl.crosscolor.label $wl.crosscolor.color -side left

frame $wl.dashshape
grid $wl.dashshape -row 1 -column 0 -sticky w -padx 3
set tmpPar(dummy) $wl.dashshape.cp
ComboBox $wl.dashshape.cb \
         -width 7  \
         -highlightthickness 0 \
         -labelwidth 10 \
         -textvariable tmpPar(c,dash) \
         -text "Dash:" -labelanchor w \
         -values $tmpPar(dashpattern) 
pack $wl.dashshape.cb  -fill x -anchor w -padx 1m -pady 1m 

frame $wl.hide
grid  $wl.hide -row 0 -column 1 -sticky w -padx 3
label $wl.hide.label -text "hide:" -width 6 -ancho w
checkbutton $wl.hide.onoff \
      -variable tmpPar(c,hide) \
      -onvalue yes -offvalue no -ancho w
pack  $wl.hide.label $wl.hide.onoff -side left

frame $wl.crosslinewidth
grid  $wl.crosslinewidth -row 1 -column 1 -sticky w -padx 3
label $wl.crosslinewidth.label -width 10 -text "linewidth:" -anchor w
entry $wl.crosslinewidth.entry -textvariable tmpPar(c,linewidth) -width 4 \
      -highlightthickness 0 
pack  $wl.crosslinewidth.label $wl.crosslinewidth.entry -side left

grid columnconfigure $wl 0 -pad 2m
grid columnconfigure $wl 1 -pad 2m
grid rowconfigure $wl 0 -pad 2m
grid rowconfigure $wl 1 -pad 2m

$wn raise 1
$wn compute_size

#
# Defining OK, Apply, Cancel and help button
#
button $w.button_lay.ok -text OK -command {
                                     global tmpPar $tmpPar(GraphName)
                                     destroy .graph
                                     cp_arr tmpPar $tmpPar(GraphName)
                                     RefreshGraph $tmpPar(GraphName)
                                     unset tmpPar
                                 } \
                           -highlightthickness 0
button $w.button_lay.apply -text Apply -command {
                                     global tmpPar $tmpPar(GraphName)
                                     cp_arr tmpPar $tmpPar(GraphName)
                                     RefreshGraph $tmpPar(GraphName)
                                 } \
                           -highlightthickness 0
button $w.button_lay.cancel -text Cancel -command {
                                     global tmpPar
                                     unset tmpPar
                                     destroy .graph } \
                            -highlightthickness 0
button $w.button_lay.help -text "Help..." -highlightthickness 0
pack $w.button_lay.ok $w.button_lay.apply $w.button_lay.cancel \
     -in $w.button_lay -side left -fill x -padx 2m
pack $w.button_lay.help -in $w.button_lay -side right -fill x -padx 2m
wm resizable $w no no
tkwait visibility $w
grab $w
raise $w
focus $w
}


#------------------------------------------------------------------------------
#
#
proc set_actual_plot_Par { GraphPar tempPar index } {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
upvar $GraphPar Par
upvar $tempPar tmpPar
#CreateGraphPar tmpPar 
#cp_arr Par tmpPar
set tmpPar(a,old_element) $index
set i [expr $index - 1]
set tmpPar(GraphName)      $Par(GraphName)
set tmpPar(w)              $Par(w)
set tmpPar(dashpattern)    $Par(dashpattern)
set tmpPar(stipples)       $Par(stipples)
set tmpPar(dashnames)      $Par(dashnames)

set tmpPar(g,color)        $Par(g,color)
set tmpPar(g,dash)         $Par(g,dash)
set tmpPar(g,hide)         $Par(g,hide)
set tmpPar(g,linewidth)    $Par(g,linewidth)
set tmpPar(g,minor)        $Par(g,minor)

set tmpPar(c,color)        $Par(c,color)
set tmpPar(c,dash)         $Par(c,dash)
set tmpPar(c,hide)         $Par(c,hide)
set tmpPar(c,linewidth)    $Par(c,linewidth)

set tmpPar(a,element)      $index
set tmpPar(l,legendtext)   [join [lrange $Par(l,legendtext)   $i $i]]

set tmpPar(e,linewidth)    [lrange $Par(e,linewidth)    $i $i]
set tmpPar(e,linehide)     [lrange $Par(e,linehide)     $i $i]
set tmpPar(e,dash)         [lindex [lrange $Par(e,dash) $i $i] 0]
set tmpPar(e,dashcolor)    [lrange $Par(e,dashcolor)    $i $i]
set tmpPar(e,offdashcolor) [lrange $Par(e,offdashcolor) $i $i]
set tmpPar(e,smooth)       [lrange $Par(e,smooth)       $i $i]

set tmpPar(e,symbol)       [lrange $Par(e,symbol)       $i $i]
set tmpPar(e,pixels)       [lrange $Par(e,pixels)       $i $i]
set tmpPar(e,scale)        [lrange $Par(e,scale)        $i $i]
set tmpPar(e,symbolhide)   [lrange $Par(e,symbolhide)   $i $i]
set tmpPar(e,fill)         [lindex [lrange $Par(e,fill)         $i $i] 0]
set tmpPar(e,outline)      [lrange $Par(e,outline)      $i $i]
set tmpPar(e,outlinewidth) [lrange $Par(e,outlinewidth) $i $i]

set tmpPar(e,errorhide)    [lrange $Par(e,errorhide)    $i $i]
set tmpPar(e,errlinewidth) [lrange $Par(e,errlinewidth) $i $i]
set tmpPar(e,errdash)      [lindex [lrange $Par(e,errdash) $i $i] 0]
set tmpPar(e,errstipples)  [lrange $Par(e,errstipples)  $i $i]
set tmpPar(e,errfill)      [lrange $Par(e,errfill)      $i $i]
set tmpPar(e,erroutline)   [lrange $Par(e,erroutline)   $i $i]
}

#------------------------------------------------------------------------------
#
#
proc save_actual_plot_Par { GraphPar tempPar index } {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
upvar $GraphPar Par
upvar $tempPar tmpPar
set Par(a,element) $index
set i [expr $index - 1]

set Par(g,color)        $tmpPar(g,color)
set Par(g,dash)         $tmpPar(g,dash)
set Par(g,hide)         $tmpPar(g,hide)
set Par(g,linewidth)    $tmpPar(g,linewidth)
set Par(g,minor)        $tmpPar(g,minor)

set Par(l,legendtext)   [lreplace $Par(l,legendtext)   $i $i \
                                  $tmpPar(l,legendtext)]

set Par(e,linewidth)    [lreplace $Par(e,linewidth)    $i $i \
                                  $tmpPar(e,linewidth)]
set Par(e,linehide)     [lreplace $Par(e,linehide)     $i $i \
                                  $tmpPar(e,linehide)]
set Par(e,dash)         [lreplace $Par(e,dash)         $i $i \
                                  $tmpPar(e,dash)]
set Par(e,dashcolor)    [lreplace $Par(e,dashcolor)    $i $i \
                                  $tmpPar(e,dashcolor)]
set Par(e,offdashcolor) [lreplace $Par(e,offdashcolor) $i $i \
                                  $tmpPar(e,offdashcolor)]
set Par(e,smooth)       [lreplace $Par(e,smooth)       $i $i \
                                  $tmpPar(e,smooth)]

set Par(e,symbol)       [lreplace $Par(e,symbol)       $i $i \
                                  [lindex $tmpPar(e,symbol) 0]]
set Par(e,symbolhide)   [lreplace $Par(e,symbolhide)   $i $i \
                                  $tmpPar(e,symbolhide)]
set Par(e,pixels)       [lreplace $Par(e,pixels)       $i $i \
                                  $tmpPar(e,pixels)]
set Par(e,scale)        [lreplace $Par(e,scale)        $i $i \
                                  $tmpPar(e,scale)]
set Par(e,fill)         [lreplace $Par(e,fill)         $i $i \
                                  $tmpPar(e,fill)]
set Par(e,outline)      [lreplace $Par(e,outline)      $i $i \
                                  $tmpPar(e,outline)]
set Par(e,outlinewidth) [lreplace $Par(e,outlinewidth) $i $i \
                                  $tmpPar(e,outlinewidth)]

set Par(e,errorhide)    [lreplace $Par(e,errorhide)    $i $i \
                                  $tmpPar(e,errorhide)]
set Par(e,errlinewidth) [lreplace $Par(e,errlinewidth) $i $i \
                                  $tmpPar(e,errlinewidth)]
set Par(e,errdash)      [lreplace $Par(e,errdash)      $i $i \
                                  $tmpPar(e,errdash)]
set Par(e,errstipples)  [lreplace $Par(e,errstipples)      $i $i \
                                  $tmpPar(e,errstipples)]
set Par(e,errfill)      [lreplace $Par(e,errfill)      $i $i \
                                  $tmpPar(e,errfill)]
set Par(e,erroutline)   [lreplace $Par(e,erroutline)   $i $i \
                                  $tmpPar(e,erroutline)]
}

#------------------------------------------------------------------------------
#
#
proc set_plot_layout   { GraphPar } {
#^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^
upvar $GraphPar Par
global tmpPar
if {[winfo exist .plotlayout] } {destroy .plotlayout}
toplevel .plotlayout
set w .plotlayout
wm geometry $w
wm title $w "plot layout ($Par(GraphName))"

cp_arr Par tmpPar

set_actual_plot_Par Par tmpPar 1
frame $w.title -relief raised -bd 2
pack $w.title -fill x -expand yes
pack [frame $w.dummy -height 2m]
set wn [NoteBook $w.notebook]
pack $wn
frame $w.button_lay
pack $w.button_lay -fill x -expand yes
#$wn configure -homogeneous yes
#
# Defining Data Set and Legend Text
#
set wl $w.title
frame $wl.dataset_lay
pack  $wl.dataset_lay -padx 2m -pady 1m -side left
label $wl.dataset_lay.label -text "Data Set:" -anchor w
global datasetmenu
set datasetmenu [tk_optionMenu $wl.dataset_lay.setno tmpPar(a,element) 1]
$wl.dataset_lay.setno configure -highlightthickness 0
$datasetmenu entryconfigure 0 -command {
   global tmpPar
   global $tmpPar(GraphName)
   global datasetmenu
   set tmp $tmpPar(GraphName)
   save_actual_plot_Par $tmpPar(GraphName) tmpPar $tmpPar(a,old_element)
   set_actual_plot_Par $tmpPar(GraphName) tmpPar $tmpPar(a,element)
}
for {set i 2} {$i <= $tmpPar(e,element)} {incr i} {
          $datasetmenu add radiobutton \
             -variable tmpPar(a,element) \
             -label $i
          $datasetmenu entryconfigure [expr $i - 1] -command {
             global tmpPar
             global $tmpPar(GraphName)
             global datasetmenu
             set tmp $tmpPar(GraphName)
             save_actual_plot_Par $tmpPar(GraphName) tmpPar \
                  $tmpPar(a,old_element)
             set_actual_plot_Par $tmpPar(GraphName) tmpPar \
                  $tmpPar(a,element)
          }
}
pack $wl.dataset_lay.label $wl.dataset_lay.setno -side left 

frame $wl.legend_lay
pack  $wl.legend_lay -fill x -expand yes -padx 2m -pady 1m -side right
label $wl.legend_lay.label -text "Legend Text:" -anchor w
entry $wl.legend_lay.entry -textvariable tmpPar(l,legendtext) \
      -highlightthickness 0 -width 12 
pack  $wl.legend_lay.label -side left
pack  $wl.legend_lay.entry -side left -fill x -expand yes

set sym_f    [$wn insert 1 1  -text symbol   -state normal ]
set line_f   [$wn insert 2 2  -text line     -state normal ]
set err_f    [$wn insert 3 3  -text error    -state normal ]

#
# defining symbol shape and size parameters
#
set wl $sym_f
frame $wl.shape
grid $wl.shape -row 0 -column 0 -sticky w -padx 3
ComboBox $wl.shape.cb \
         -textvariable tmpPar(e,symbol) \
         -width 7  -editable no  \
         -highlightthickness 0 \
         -labelwidth 7 \
         -text "Shape:" -labelanchor w \
         -values {"none" "square" "circle" "diamond" \
                "plus"  "cross"  "splus"  "scross"  }
pack $wl.shape.cb  -fill x -anchor w -padx 1m -pady 1m 

frame $wl.size
grid  $wl.size -row 1 -column 0 -sticky w -padx 3
label $wl.size.label -text "Size:" -width 7 -anchor w
entry $wl.size.entry -textvariable tmpPar(e,pixels) -width 4 \
      -highlightthickness 0 
pack  $wl.size.label $wl.size.entry -side left

frame $wl.hide
grid  $wl.hide -row 2 -column 0 -sticky w -padx 3
label $wl.hide.label -text "hide:" -width 7 -ancho w
checkbutton $wl.hide.onoff \
      -variable tmpPar(e,symbolhide) \
      -onvalue yes -offvalue no -ancho w
pack  $wl.hide.label $wl.hide.onoff -side left


#
# defining symbol color parameters
#
frame $wl.fill
grid  $wl.fill -row 0 -column 1 -sticky w -padx 3
label $wl.fill.label -width 14 -text "fill color:" -anchor w
SelectColor $wl.fill.color -type menubutton -variable tmpPar(e,fill) 
pack $wl.fill.label $wl.fill.color -side left

frame $wl.outline
grid  $wl.outline -row 1 -column 1 -sticky w -padx 3
label $wl.outline.label -width 14  -text "outline color:" -anchor w
SelectColor $wl.outline.color -type menubutton -variable tmpPar(e,outline) 
pack $wl.outline.label $wl.outline.color -side left

frame $wl.outlinewidth
grid  $wl.outlinewidth -row 2 -column 1 -sticky w -padx 3
label $wl.outlinewidth.label -width 14 -text "outline width:" -anchor w
entry $wl.outlinewidth.entry -textvariable tmpPar(e,outlinewidth) -width 4 \
      -highlightthickness 0 
pack  $wl.outlinewidth.label $wl.outlinewidth.entry -side left

#
# defining line width and color parameters
#
set wl $line_f
frame $wl.width
grid  $wl.width -row 0 -column 0 -sticky w -padx 3
label $wl.width.label -text "line width:" -width 11 -anchor w
entry $wl.width.entry -textvariable tmpPar(e,linewidth) -width 4 \
      -highlightthickness 0 
pack  $wl.width.label $wl.width.entry -side left

frame $wl.dashcolor
grid  $wl.dashcolor -row 1 -column 0 -sticky w -padx 3
label $wl.dashcolor.label -text "dash color:" -width 11 -ancho w
SelectColor $wl.dashcolor.color -type menubutton -variable tmpPar(e,dashcolor) 
pack $wl.dashcolor.label $wl.dashcolor.color -side left

#frame $wl.offdashcolor
#grid  $wl.offdashcolor -row 2 -column 0 -sticky w -padx 3
#label $wl.offdashcolor.label -text "offdash color:" -width 11 -ancho w
#SelectColor $wl.offdashcolor.color -type menubutton -variable tmpPar(e,offdashcolor) 
#pack $wl.offdashcolor.label $wl.offdashcolor.color -side left

#
# defining line shape and hide parameters
#
frame $wl.dashshape
grid $wl.dashshape -row 0 -column 1 -sticky w -padx 3
set tmpPar(dummy) $wl.dashshape.cp
ComboBox $wl.dashshape.cb \
         -width 7  \
         -highlightthickness 0 \
         -labelwidth 7 \
         -textvariable tmpPar(e,dash) \
         -text "Shape:" -labelanchor w \
         -values $tmpPar(dashpattern) 
pack $wl.dashshape.cb  -fill x -anchor w -padx 1m -pady 1m 

frame $wl.smooth
grid  $wl.smooth -row 1 -column 1 -sticky w -padx 3
ComboBox $wl.smooth.cb \
         -width 7  -editable no  \
         -highlightthickness 0 \
         -labelwidth 7 \
         -textvariable tmpPar(e,smooth) \
         -text "smooth:" -labelanchor w \
         -values {linear step natural quadratic}
pack  $wl.smooth.cb  -fill x -anchor w -padx 1m -pady 1m 

frame $wl.linehide
grid  $wl.linehide -row 2 -column 1 -sticky w -padx 3
label $wl.linehide.label -text "hide:" -width 7 -ancho w
checkbutton $wl.linehide.onoff \
      -variable tmpPar(e,linehide) \
      -onvalue 0 -offvalue 1 -ancho w
pack  $wl.linehide.label $wl.linehide.onoff -side left

#
# defining line width and color parameters of error lines
#
set wl $err_f
frame $wl.width
grid  $wl.width -row 0 -column 0 -sticky w -padx 3
label $wl.width.label -text "line width:" -width 11 -anchor w
entry $wl.width.entry -textvariable tmpPar(e,errlinewidth) -width 4 \
      -highlightthickness 0 
pack  $wl.width.label $wl.width.entry -side left

#frame $wl.fillcolor
#grid  $wl.fillcolor -row 2 -column 0 -sticky w -padx 3
#label $wl.fillcolor.label -text "fill color:" -width 14 -ancho w
#SelectColor $wl.fillcolor.color -type menubutton -variable tmpPar(e,errfill) 
#pack $wl.fillcolor.label $wl.fillcolor.color -side left

frame $wl.outlinecolor
grid  $wl.outlinecolor -row 1 -column 0 -sticky w -padx 3
label $wl.outlinecolor.label -text "color:" -width 11 -ancho w
SelectColor $wl.outlinecolor.color -type menubutton -variable tmpPar(e,erroutline) 
pack $wl.outlinecolor.label $wl.outlinecolor.color -side left

#
# defining line shape and hide parameters
#
frame $wl.errshape
grid $wl.errshape -row 0 -column 1 -sticky w -padx 3
ComboBox $wl.errshape.cb \
         -width 7  \
         -highlightthickness 0 \
         -labelwidth 7 \
         -textvariable tmpPar(e,errdash) \
         -text "Shape:" -labelanchor w \
         -values $tmpPar(dashpattern) 
pack $wl.errshape.cb  -fill x -anchor w -padx 1m -pady 1m 

#frame $wl.stipples
#grid  $wl.stipples -row 2 -column 1 -sticky w -padx 3
#ComboBox $wl.stipples.cb \
#         -width 12  -editable no  \
#         -highlightthickness 0 \
#         -labelwidth 7 \
#         -textvariable tmpPar(e,errstipples) \
#         -text "smooth:" -labelanchor w \
#         -values $tmpPar(stipples)
#pack  $wl.stipples.cb  -fill x -anchor w -padx 1m -pady 1m 

frame $wl.errhide
grid  $wl.errhide -row 1 -column 1 -sticky w -padx 3
label $wl.errhide.label -text "hide:" -width 7 -ancho w
checkbutton $wl.errhide.onoff \
      -variable tmpPar(e,errorhide) \
      -onvalue yes -offvalue no -ancho w
pack  $wl.errhide.label $wl.errhide.onoff -side left


for {set i 1} {$i < 4} {incr i} {
   $wn raise $i
}
$wn raise 1
$wn compute_size

#
# Defining OK, Apply, Cancel and help button
#
button $w.button_lay.ok -text OK -command {
                                     global tmpPar $tmpPar(GraphName)
                                     set datasetmenu \
                                        .plotlayout.title.dataset_lay.setno.menu
                                     save_actual_plot_Par $tmpPar(GraphName) \
                                        tmpPar $tmpPar(a,element)
                                     destroy .plotlayout
                                     RefreshGraph $tmpPar(GraphName)
                                     unset tmpPar
                                 } \
                           -highlightthickness 0
button $w.button_lay.apply -text Apply -command {
                                     global tmpPar $tmpPar(GraphName)
                                     set datasetmenu \
                                        .plotlayout.title.dataset_lay.setno.menu
                                     save_actual_plot_Par $tmpPar(GraphName) \
                                        tmpPar $tmpPar(a,element)
                                     RefreshGraph $tmpPar(GraphName)
                                 } \
                           -highlightthickness 0
button $w.button_lay.cancel -text Cancel -command {
                                     global tmpPar
                                     unset tmpPar
                                     destroy .plotlayout 
                               } \
                            -highlightthickness 0
button $w.button_lay.help -text "Help..." -highlightthickness 0
pack $w.button_lay.ok $w.button_lay.apply $w.button_lay.cancel \
      -side left -fill x -padx 2m -pady 2m
pack $w.button_lay.help -side right -fill x -padx 2m -pady 2m

wm resizable $w no no
tkwait visibility $w
raise $w
focus $w
grab $w
}
