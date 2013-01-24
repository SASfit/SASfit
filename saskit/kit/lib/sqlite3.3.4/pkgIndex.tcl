package ifneeded sqlite3 3.3.4 {
    load {} Sqlite3
    namespace eval ::tcldb {set sqlite3_listeval 1}
    if {[info commands ::sqlite] == ""} {
        interp alias {} sqlite {} sqlite3
    }
}

