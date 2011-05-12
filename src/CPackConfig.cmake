# src/CPackConfig.cmake
#
# Copyright (c) 2008-2009, Paul Scherrer Institute (PSI)
#
# This file is part of SASfit.
#
# SASfit is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# SASfit is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with SASfit.  If not, see <http://www.gnu.org/licenses/>.

# Author(s) of this file:
#   Ingo Bressler (ingo@cs.tu-berlin.de)

SET(CPACK_CMAKE_GENERATOR "Unix Makefiles")
set(CPACK_PACKAGE_DESCRIPTION_FILE "../Readme.txt")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "SASfit is a SAS tool")
set(CPACK_PACKAGE_EXECUTABLES "sasfit")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "sasfit 0.90")
set(CPACK_PACKAGE_NAME "SASfit")
set(CPACK_PACKAGE_VENDOR "PSI")
set(CPACK_PACKAGE_VERSION "0.90")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "90")
set(CPACK_INSTALLED_DIRECTORIES "/home/ingo/code/sasfit/hg/src/../../sasfit_0.93.3_Linux_x86_64;.")
set(CPACK_PACKAGE_FILE_NAME "sasfit_0.93.3_Linux_x86_64")
if(NOT WIN32)
	set(CPACK_GENERATOR "TBZ2")
else(NOT WIN32)
	set(CPACK_GENERATOR "ZIP")
endif(NOT WIN32)

