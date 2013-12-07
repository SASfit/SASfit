# part: zlib {{{
part::create zlib -buildcommand {
    utils::sync [part::srcdir zlib] [part::destdir zlib]
    conf::mainconfigure [part::destdir zlib]
    conf::make libz.a
} -depend {} -options {} -core 1
# }}}

