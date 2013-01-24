#
# icons.tcl,v 1.1 2005/07/20 21:23:12 jenglish Exp
#
# Tile package -- stock icons.
# 
# Usage:
#	$w configure -image [tile::stockIcon $context/$icon]
#
# At present, only includes icons for dialog boxes,
# dialog/info, dialog/warning, dialog/error, etc.
#
# This list should be expanded.
#

namespace eval tile {
    variable Icons		;# Map: icon name -> image
    variable IconData 		;# Map: icon name -> image data
}

# stockIcon $name --
#	Returns a Tk image for built-in icon $name. 
#
proc tile::stockIcon {name} {
    variable Icons
    variable IconData
    if {![info exists Icons($name)]} {
    	set Icons($name) [image create photo -data $IconData($name)]
	unset IconData($name)
    }
    return $Icons($name)
}

#
# Stock icons for dialogs 
#
# SOURCE: dialog icons taken from BWidget toolkit.
#
array set tile::IconData {

dialog/error {
    R0lGODlhIAAgALMAAIQAAISEhPf/Mf8AAP//////////////////////////
    /////////////////////yH5BAEAAAIALAAAAAAgACAAAASwUMhJBbj41s0n
    HmAIYl0JiCgKlNWVvqHGnnA9mnY+rBytw4DAxhci2IwqoSdFaMKaSBFPQhxA
    nahrdKS0MK8ibSoorBbBVvS4XNOKgey2e7sOmLPvGvkezsPtR3M2e3JzdFIB
    gC9vfohxfVCQWI6PII1pkZReeIeWkzGJS1lHdV2bPy9koaKopUOtSatDfECq
    phWKOra3G3YuqReJwiwUiRkZwsPEuMnNycslzrIdEQAAOw==}

dialog/info {
    R0lGODlhIAAgALMAAAAAAAAA/4SEhMbGxvf/Mf//////////////////////
    /////////////////////yH5BAEAAAQALAAAAAAgACAAAAStkMhJibj41s0n
    HkUoDljXXaCoqqRgUkK6zqP7CvQQ7IGsAiYcjcejFYAb4ZAYMB4rMaeO51sN
    kBKlc/uzRbng0NWlnTF3XAAZzExj2ET3BV7cqufctv2Tj0vvFn11RndkVSt6
    OYVZRmeDXRoTAGFOhTaSlDOWHACHW2MlHQCdYFebN6OkVqkZlzcXqTKWoS8w
    GJMhs7WoIoC7v7i+v7uTwsO1o5HHu7TLtcodEQAAOw==}

dialog/question {
    R0lGODlhIAAgALMAAAAAAAAA/4SEhMbGxvf/Mf//////////////////////
    /////////////////////yH5BAEAAAQALAAAAAAgACAAAAS2kMhJibj41s0n
    HkUoDljXXaCoqqRgUkK6zqP7CnS+AiY+D4GgUKbibXwrYEoYIIqMHmcoqGLS
    BlBLzlrgzgC22FZYAJKvYG3ODPLS0khd+awDX+Qieh2Dnzb7dnE6VIAffYdl
    dmo6bHiBFlJVej+PizRuXyUTAIxBkSGBNpuImZoVAJ9roSYAqH1Yqzetrkmz
    GaI3F7MyoaYvHhicoLe/sk8axcnCisnKBczNxa3I0cW+1bm/EQAAOw==}

dialog/warning {
    R0lGODlhIAAgALMAAAAAAISEAISEhMbGxv//AP//////////////////////
    /////////////////////yH5BAEAAAUALAAAAAAgACAAAASrsMhJZ7g16y0D
    IQPAjZr3gYBAroV5piq7uWcoxHJFv3eun0BUz9cJAmHElhFow8lcIQBgwHOu
    aNJsDfk8ZgHH4TX4BW/Fo12ZjJ4Z10wuZ0cIZOny0jI6NTbnSwRaS3kUdCd2
    h0JWRYEhVIGFSoEfZo6FipRvaJkfUZB7cp2Cg5FDo6RSmn+on5qCPaivYTey
    s4sqtqswp2W+v743whTCxcbHyG0FyczJEhEAADs=}
}

#*EOF*
