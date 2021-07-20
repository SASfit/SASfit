# src/cmake/replace_text_in_file.cmake
#
# Copyright 2021 Ingo Breßler (ingo.bressler@bam.de)
#
# This file is subject to the terms and conditions defined in
# file 'COPYING.txt', which is part of this source code package.
#
# A CMake script to replace a matching regular expression *PATTERN*
# in a text file *FILENAME* with a given replacement text *REPLACEMENT*.
# Those variables should be provided by -D argument when running it with cmake.

message("In file '${FILENAME}' replacing '${PATTERN}' by '${REPLACEMENT}'.")
file(READ ${FILENAME} FILE_BODY)
string(REGEX REPLACE "${PATTERN}" "${REPLACEMENT}"
    FILE_BODY_NEW "${FILE_BODY}")
file(WRITE "${FILENAME}" "${FILE_BODY_NEW}")

# vim: set ts=4 sw=4 sts=4 tw=0 et:
