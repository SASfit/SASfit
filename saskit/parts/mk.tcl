# part: mk {{{
part::create mk \
    -buildcommand {
        if {$::tcl_platform(platform)=="windows"} {
            set files {
                src/column src/custom src/derived src/fileio src/field src/format
                src/handler src/persist src/remap src/std src/store src/string src/table
                src/univ src/view src/viewx
                tcl/mk4tcl tcl/mk4too
            }
            set ofiles {}; foreach f $files {lappend ofiles [file tail $f.o]}
            set cppfiles {}; foreach f $files {lappend cppfiles [file join [utils::wdrelative [part::srcdir mk]] $f.cpp]}
            out::oexec [concat [list g++ -fpermissive \
                -I[utils::wdrelative [part::srcdir tcl]]/win \
                -I[utils::wdrelative [part::srcdir tcl]]/generic \
                -I[utils::wdrelative [part::srcdir mk]]/include \
                -I[utils::wdrelative [part::srcdir mk]]/src \
                -I[utils::wdrelative [part::srcdir mk]]/win \
                -DBUILD_tcl -DSTATIC_BUILD=1 -c] $cppfiles]
            conf::loadconf [part::configfile tcl] tc tc
            out::oexec [concat [list ar cr mk4tcl.a] $ofiles [list [utils::wdrelative [part::destdir tcl]]/$tc(TCL_LIB_FILE)]]
        }  else  {
            conf::norelplatformconfigure [part::srcdir mk] --with-tcl=[part::srcdir tcl]/generic [conf::c_disen thread threads] [conf::c_disen debug symbols]
            conf::make
            conf::make install
        }
    } \
    -librarycommand {
        if {$::tcl_platform(platform)=="windows"} {
            return [list [part::destdir mk]/mk4tcl.a]
        }  else  {
            return [list [part::destdir mk]/.libs/libmk4tcl.a]
        }
    } \
    -options {debug 0 thread 0} \
    -depend {tcl} \
    -core 1 \

# }}}

