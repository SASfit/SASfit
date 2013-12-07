proc tclInit {} {
  rename tclInit {}
  if {[info exists ::tclInitDebug]} {
    set debug $::tclInitDebug
    unset ::tclInitDebug
  }  else  {
    set debug 0
  }

  global auto_path tcl_library tcl_libPath
  global tcl_version tcl_rcFileName
  
  if {[info exists ::othernoe]} {
    set noe $::othernoe
    unset ::othernoe
  }  else  {
    set noe [info nameofexecutable]
  }
  if {$debug} {
    set dputs puts
    $dputs "NOE='$noe'"
  }

  set tcl_library [file join $noe lib tcl$tcl_version]
  set tcl_libPath [list $tcl_library [file join $noe lib]]

  # get rid of a build residue
  unset -nocomplain ::tclDefaultLibrary
  if {$debug} {
    $dputs "LIBPATH=$tcl_libPath"
  }

  # the following code only gets executed once on startup
  namespace eval ::vfs {}
  set go 0
  if {![info exists ::vfs::kitInitialized]} {
    set ::vfs::kitInitialized 1
    set go 1

    # lookup and emulate "source" of lib/vfs/{vfs*.tcl,mk4vfs.tcl}
    # must use raw MetaKit calls because VFS is not yet in place

    set d [mk::select exe.dirs parent 0 name lib]
    set d [mk::select exe.dirs parent $d name vfs]
    
    set code {
      load {} dqkitpwb
      load {} vfs
      load {} Mk4tcl
      load {} zlib
    }
    foreach x {vfsUtils vfslib mk4vfs} {
      set n [mk::select exe.dirs!$d.files name $x.tcl]
      set s [mk::get exe.dirs!$d.files!$n contents]
      catch {set s [zlib decompress $s]}
      append code $s \n
    }

    if {$debug} {
      $dputs "Evaluating code - [string length $code]"
    }
    uplevel #0 $code

    # mount the executable, i.e. make all runtime files available
    if {$debug} {
      $dputs "Mounting as $noe"
    }
    vfs::filesystem mount $noe [list ::vfs::mk4::handler exe]
    if {$debug} {
      $dputs "NOFILES=[llength [glob -nocomplain -directory $noe *]]"
    }

    # alter path to find encodings
    if {$debug} {
      $dputs "Library path [info library]"
    }

    # Tcl 8.5 [encoding dirs] fix
    if {[catch {encoding dirs [info nameofexecutable]/lib/tcl[info tclversion]/encoding}]} {
	pwblibrarypath [info library] [info nameofexecutable]
    }

    # fix system encoding, if it wasn't properly set up (200207.004 bug)
    if {$debug} {
      $dputs "Encoding patch - [llength [glob -nocomplain $noe/*]] [llength [glob -nocomplain $noe/lib/tcl[info tclversion]/encoding/*]]"
    }
    if {[encoding system] eq "identity"} { encoding system iso8859-1 }
    if {$debug} {
      $dputs "Unmounting"
    }

    # now remount the executable with the correct encoding
    vfs::filesystem unmount $noe
    set tcl_library [file join $noe lib tcl$tcl_version]
    set tcl_libPath [list $tcl_library [file join $noe lib]]

    if {$debug} {
      $dputs "Remounting (nameofexe=[info nameofexecutable])"
    }
    vfs::filesystem mount $noe [list ::vfs::mk4::handler exe]
    # quickhack
    if {$debug} {
      $dputs "Setting up variables"
    }
    eval [list unset -nocomplain ::tclDefaultLibrary]
    eval [list namespace eval ::vfs { variable dqkit_version 0.10 }]
    # not really sure what it does
    eval [list namespace eval ::vfs { variable tclkit_version 200403 }]
    eval [list uplevel #0 [list source [file join $tcl_library init.tcl]]]
    eval [list set ::auto_path $::tcl_libPath]
    if {$debug} {
      $dputs "Stat done"
    }
    if {[info exists ::env(DQKITLIBPATH)]} {
      if {$::tcl_platform(platform) == "windows"} {
        set split "\;"
      }  elseif {$::tcl_platform(platform) == "mac"} {
        set split "\;"
      }  else  {
        set split ":"
      }
      foreach dir [split $::env(DQKITLIBPATH) $split] {
        if {$debug} {
          $dputs "Libpath: $dir"
        }
        if {[catch {file stat $dir st}]} {continue}
        if {$st(type) == "file"} {
          if {[catch {vfs::mk4::Mount $dir $dir -readonly}]} {
            puts stderr "Unable to mount $dir:\n$::errorInfo"
            continue
          }
          lappend auto_path $dir
          catch {
            if {[file exists $dir/lib]} {
              lappend auto_path $dir/lib
            }
          }
          if {$debug} {
            $dputs "Mounted KIT $dir"
          }
        }  elseif {$st(type) == "directory"} {
          foreach dirfile [glob -nocomplain -directory $dir *.kit] {
            if {[catch {vfs::mk4::Mount $dirfile $dirfile -readonly}]} {
              puts stderr "Unable to mount $dirfile:\n$::errorInfo"
              continue
            }
            lappend auto_path $dirfile
            catch {
              if {[file exists $dirfile/lib]} {
                lappend auto_path $dirfile/lib
              }
            }
            if {$debug} {
              $dputs "Mounted KIT $dirfile"
            }
          }
          if {$debug} {
            $dputs "Adding directory $dir"
          }
          lappend ::auto_path $dir
        }
      }
    }
    if {([package vcompare [package require Tcl] 8.5] >= 0) && [info exists ::env(DQKITTMPATH)]} {
      if {$::tcl_platform(platform) == "windows"} {
        set split "\;"
      }  elseif {$::tcl_platform(platform) == "mac"} {
        set split "\;"
      }  else  {
        set split ":"
      }

      foreach dir [split $::env(DQKITTMPATH) $split] {
        if {$debug} {
          $dputs "Tcl module path: $dir"
        }
        if {[catch {file stat $dir st}]} {continue}
        if {$st(type) == "file"} {
          if {[catch {vfs::mk4::Mount $dir $dir -readonly}]} {
            puts stderr "Unable to mount $dir:\n$::errorInfo"
            continue
          }
          if {$debug} {
            $dputs "Mounted KIT $dir"
          }
        }  elseif {$st(type) == "directory"} {
        }  else  {
          puts stderr "Unknown type for $dir - not using for TclModules path."
          continue
        }
        tcl::tm::roots $dir
      }
    }
    if {$debug} {
      $dputs "Auto path:\n  [join $::auto_path \n\ \ ]\n"
      if {[package vcompare [package require Tcl] 8.5] >= 0} {
        $dputs "Tcl module auto path:\n  [join [tcl::tm::list] \n\ \ ]\n"
      }
    }
  }
}

