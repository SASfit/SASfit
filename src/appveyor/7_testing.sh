#!/bin/sh
# tools for testing the build products

testIfExists()
{
    local fn; fn="$1"
    local dscr; dscr="$2"
    echo "Testing for $dscr $fn:: $(if [ -f "$fn" ]; then echo "success!"; else echo "failure!"; fi)"
}

# vim: set ts=4 sw=4 sts=4 tw=0 et:
