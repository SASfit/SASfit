07/09/1999
BWidget ToolKit 1.2.1
Copyright (c) 1998-1999 UNIFIX. 

--------------------------------------------------------------------------
WHAT IS BWIDGET ?

The BWidget Toolkit is a high-level Widgets Set for Tcl/Tk built using
native Tcl/Tk 8.x namespaces.

The BWidgets have a professional look&feel as in other well known Toolkits 
(Tix or Incr Widget) but the concept is radically different because everything is 
native so no platform compilation, no compiled extension library are needed. 
The code is 100% Pure Tcl/Tk.


--------------------------------------------------------------------------
WIDGET LIST (1.2.1)

Simple Widgets 
      Label           Extended Label widget
      Entry           Extended Entry widget
      Button          Extended Button widget
      ArrowButton     Button widget with an arrow shape.
      ProgressBar     Progress indicator widget
      ScrollView      Display the visible area of a scrolled window
      Separator       3D separator widget

Manager Widgets 
      MainFrame       Manage toplevel with menu, toolbar and statusbar 
      LabelFrame      Frame with a Label
      TitleFrame      Frame with a title
      ScrolledWindow  Generic scrolled widget
      ScrollableFrame Scrollable frame containing widget
      PanedWindow     Tiled layout manager widget
      ButtonBox       Set of buttons with horizontal or vertical layout
      PagesManager    Pages manager widget
      NoteBook        Notebook manager widget
      Dialog          Dialog abstraction with custom buttons

Composite Widgets 
      LabelEntry      LabelFrame containing an Entry widget. 
      ComboBox        ComboBox widget
      SpinBox         SpinBox widget
      Tree            Tree widget
      ListBox         ListBox widget
      MessageDlg      Message dialog box
      ProgressDlg     Progress indicator dialog box
      PasswdDlg       Login/Password dialog box (contributed by Stephane Lavirotte)
      SelectFont      Font selection widget
      SelectColor     Color selection widget

Commands Classes 
      Widget          The Widget base class
      DynamicHelp     Provide help to Tk widget or BWidget
      DragSite        Commands set for Drag facilities
      DropSite        Commands set for Drop facilities
      BWidget         Utilities

--------------------------------------------------------------------------
INSTALLATION AND USE

- On Unix Platform:
  Uncompress the file BWidget-1.2.1.tar.Z or BWidget-1.2.1.tar.gz

  To use the BWidget:
  - If you have uncompressed the archive file under the Tcl Library Path
    directory, you only need to do:
      % package require BWidget
  - If not, you have to specify the BWidget installation path in auto_path
    global variable:
      % lappend auto_path <install_path>
      % package require BWidget

  To launch the demo, you need to cd into the demo subdirectory:
      $ cd <install_path>/demo
      $ wish demo.tcl

- On Windows and others Platforms:
  Uncompress the file BWidget-1.2.1.zip

  To use the BWidget:
  - If you uncompressed the archive file under the Tcl Library Path
    directory, you only need to do:
      % package require BWidget
  - If not, you have to specify the BWidget installation path in auto_path
    global variable:
      % lappend auto_path your_path
      % package require BWidget

  To launch the demo :
      Double click on demo.tcl in the demo subdirectory


Distribution contains these directories:

BWidget-1.2   Root directory and BWidget Tcl sources
   BWman        HTML manual pages
   images       images used by BWidget
   lang         Resources for language customization
   demo         Demo sources


--------------------------------------------------------------------------
DOCUMENTATION

HTML manual pages are available in the BWman subdirectory.
Points to index.html for frame version with tree navigation,
or to contents.html for no frame version.

Look at http://www.unifix-online.com/BWidget for last revision
of manual pages.


--------------------------------------------------------------------------
LICENSE

BWidget ToolKit 1.2.1
Copyright (c) 1998-1999 UNIFIX. 

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Library General Public
License as published by the Free Software Foundation; either
version 2 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Library General Public License for more details.

You should have received a copy of the GNU Library General Public
License along with this library; if not, write to the
Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA  02111-1307, USA.


License is also in LICENSE.txt. You can find LGPL.txt too.

--------------------------------------------------------------------------
CONTACTS

Mail         mailto:bwidget@unifix-online.com
Home page    http://www.unifix-online.com/BWidget/

Bug Report and FAQ : See Home Page



