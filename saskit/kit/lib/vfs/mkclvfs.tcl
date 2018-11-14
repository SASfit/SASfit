# mkclvfs.tcl -- Metakit Compatible Lite Virtual File System driver
# Rewritten from mk4vfs.tcl, orig by by Matt Newman and Jean-Claude Wippler 
# $Id: mkclvfs.tcl,v 1.4 2005/11/04 21:59:54 jcw Exp $

# 1.0	initial release
# 1.1	view size renamed to count, vlerq package renamed to thrill
# 1.2	replace view calls by vget (simpler and slightly faster)

package provide vfs::mkcl 1.2
package require vfs
package require thrill

namespace eval vfs::mkcl {
  namespace import ::thrill::*

  namespace eval v {
    variable seq 0  ;# used to generate a unique db handle
    variable rootv  ;# maps handle to root view (well, actually "dirs")
    variable dname  ;# maps handle to cached list of directory names
    variable prows  ;# maps handle to cached list of parent row numbers
  }

# public
  proc Mount {mkfile local args} {
    set db mkclvfs[incr v::seq]
    set v::rootv($db) [vget [vopen $mkfile] 0 dirs]
    set v::dname($db) [vget $v::rootv($db) * 0]
    set v::prows($db) [vget $v::rootv($db) * 1]
    ::vfs::filesystem mount $local [list ::vfs::mkcl::handler $db]
    ::vfs::RegisterMount $local [list ::vfs::mkcl::Unmount $db]
    return $db
  }
  proc Unmount {db local} {
    ::vfs::filesystem unmount $local
    unset v::rootv($db) v::dname($db) v::prows($db)
  }
# private
  proc handler {db cmd root path actual args} {
    #puts [list MKCL $db <$cmd> r: $root p: $path a: $actual $args]
    switch $cmd {
      matchindirectory	{ eval [linsert $args 0 $cmd $db $path $actual] }
      fileattributes	{ eval [linsert $args 0 $cmd $db $root $path] } 
      default		{ eval [linsert $args 0 $cmd $db $path] }
    }
  }
  proc fail {code} {
    ::vfs::filesystem posixerror $::vfs::posix($code)
  }
  proc lookUp {db path} {
    set dirs $v::rootv($db)
    set parent 0
    set elems [file split $path]
    set remain [llength $elems]
    foreach e $elems {
      set r ""
      foreach r [lsearch -exact -int -all $v::prows($db) $parent] {
	if {$e eq [lindex $v::dname($db) $r]} {
	  set parent $r
	  incr remain -1
	  break
	}
      }
      if {$parent != $r} {
	if {$remain == 1} {
	  set files [vget $dirs $parent 2]
	  set i [lsearch -exact [vget $files * 0] $e]
	  if {$i >= 0} {
	    # evaluating this 3-item result returns the info about one file
	    return [list vget $files $i]
	  }
	}
	fail ENOENT
      }
    }
    # evaluating this 4-item result returns the files subview
    return [list vget $dirs $parent 2]
  }
  proc isDir {tag} {
    return [expr {[llength $tag] == 4}]
  }
# methods
  proc matchindirectory {db path actual pattern type} {
    set o {}
    if {$type == 0} { set type 20 }
    set tag [lookUp $db $path]
    if {$pattern ne ""} {
      set c {}
      if {[isDir $tag]} {
	# collect file names
	if {$type & 16} {
	  set c [eval [linsert $tag end * 0]]
	}
	# collect directory names
	if {$type & 4} {
	  foreach r [lsearch -exact -int -all $v::prows($db) [lindex $tag 2]] {
	    lappend c [lindex $v::dname($db) $r]
	  }
	}
      }
      foreach x $c {
	if {[string match $pattern $x]} {
	  lappend o [file join $actual $x]
	}
      }
    } elseif {$type & ([isDir $tag]?4:16)} {
      set o [list $actual]
    }
    return $o
  }
  proc fileattributes {db root path args} {
    switch -- [llength $args] {
      0 { return [::vfs::listAttributes] }
      1 { set index [lindex $args 0]
	  return [::vfs::attributesGet $root $path $index] }
      2 { fail EROFS }
    }
  }
  proc open {db file mode permissions} {
    if {$mode ne "" && $mode ne "r"} { fail EROFS }
    set tag [lookUp $db $file]
    if {[isDir $tag]} { fail ENOENT }
    foreach {name size date contents} [eval $tag] break
    if {[string length $contents] != $size} {
      set contents [vfs::zip -mode decompress $contents]
    }
    set fd [vfs::memchan]
    fconfigure $fd -translation binary
    puts -nonewline $fd $contents
    fconfigure $fd -translation auto
    seek $fd 0
    return [list $fd]
  }
  proc access {db path mode} {
    if {$mode & 2} { fail EROFS }
    lookUp $db $path
  }
  proc stat {db path} {
    set tag [lookUp $db $path]
    set l 1
    if {[isDir $tag]} {
      set t directory
      set s 0
      set d 0
      set c ""
      incr l [eval [linsert $tag end #]]
      incr l [llength [lsearch -exact -int -all $v::prows($db) [lindex $tag 2]]]
    } else {
      set t file
      foreach {n s d c} [eval $tag] break
    }
    return [list type $t size $s atime $d ctime $d mtime $d nlink $l \
		  csize [string length $c] gid 0 uid 0 ino 0 mode 0777]
  }
}
