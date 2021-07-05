#!/bin/sh
# tools for testing the build products

testIfExists()
{
    local fn; fn="$1"
    local dscr; dscr="$2"
    local indent="    "
    echo "Testing for $dscr $fn:"
    if [ ! -f "$fn" ]; then
        echo "${indent}-> Failure!"
        return 1
    fi
    echo "${indent}-> Success!"
}

# vim: set ts=4 sw=4 sts=4 tw=0 et:
