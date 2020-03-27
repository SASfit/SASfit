cmake_minimum_required(VERSION 3.0)

# assuming we live in <sasfit-root>/src/plugins
get_filename_component(CURRENT_DIR ${CMAKE_CURRENT_LIST_FILE} PATH)
set(SASFIT_VFS ${CURRENT_DIR}/../../sasfit.vfs)
set(SASFIT_VFS_PLUGINS ${SASFIT_VFS}/lib/app-sasfit/plugins)

# copy plugin definition files to sasfit.vfs
# (virtual filesystem for tclkit/saskit usage)
if(IS_DIRECTORY ${SASFIT_VFS_PLUGINS})
	file(GLOB GROUP_DEF_FILES RELATIVE ${CURRENT_DIR} "groups*.def")
	foreach(DEF_FILE ${GROUP_DEF_FILES})
		configure_file(${CURRENT_DIR}/${DEF_FILE} 
		               ${SASFIT_VFS_PLUGINS}/${DEF_FILE}
		               COPYONLY
		)
	endforeach(DEF_FILE)
endif(IS_DIRECTORY ${SASFIT_VFS_PLUGINS})


#
# determine which plugins are available and generate a list
#

file(GLOB ENTRY_LIST RELATIVE ${CURRENT_DIR} [a-z]*)

set(DIR_LIST)
foreach(FILENAME ${ENTRY_LIST})
	if(IS_DIRECTORY ${CURRENT_DIR}/${FILENAME})
		string(REGEX MATCH "^CMake.*" TESTVAR ${FILENAME})
		if(TESTVAR STREQUAL "")
			add_subdirectory(${FILENAME})
			list(APPEND DIR_LIST "sasfit_${FILENAME}")
		endif(TESTVAR STREQUAL "")
	endif(IS_DIRECTORY ${CURRENT_DIR}/${FILENAME})
endforeach(FILENAME)

set(SASFIT_PLUGINS_INCLUDED ${DIR_LIST} PARENT_SCOPE)

