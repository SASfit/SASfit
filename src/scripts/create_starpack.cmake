# This cmake script will create a starpack (standalone executable)
# based on the virtual filesystem in <root>/sasfit.vfs
# using the precompiled saskit binaries in <root>/saskit
#
# The top-level CMakeLists.txt in <root>/src/ will finally create a package
# directory structure for distribution
#
# SASFIT_ROOT_DIR  - the root directory of sasfit where 
#                    the subdirectories 'src' and 'bin' live in
# SASFIT_PCKG_DIR  - the package directory, the root of the new sasfit-tree,
#                    'bin' and 'src' will be copied here to
# SASFIT_LIST_FILE - full filename (incl. path) to the file which contains
#                  - the list of files to copy

cmake_minimum_required(VERSION 2.6.2)

set(SASKIT_PATH ${SASFIT_ROOT_DIR}/saskit)
set(SASKIT_FILE ${SASKIT_PATH}/${SASKIT_FILENAME})
set(SASFIT_EXEC ${SASFIT_ROOT_DIR}/${SASFIT_EXEC_FILENAME})

message(STATUS "Creating Starpack ...")

if(EXISTS ${SASKIT_FILE})
	message(STATUS "Using ${SASKIT_FILE}")
	if(NOT WIN32)
		execute_process(COMMAND chmod u+x,g+x,o+x ${SASKIT_FILE})
	endif(NOT WIN32)
	file(REMOVE ${SASFIT_EXEC})
	configure_file(${SASKIT_FILE} ${SASKIT_FILE}_rt COPYONLY)
	execute_process(COMMAND ${SASKIT_FILE} "${SASKIT_PATH}/sdx.kit" wrap ${SASFIT_EXEC_FILENAME} -runtime ${SASKIT_FILE}_rt
			WORKING_DIRECTORY ${SASFIT_ROOT_DIR}
			TIMEOUT 80.0
			RESULT_VARIABLE SASKIT_RES
			OUTPUT_VARIABLE SASKIT_OUT
			ERROR_VARIABLE SASKIT_OUT
	)
	if(SASKIT_RES EQUAL 0 OR SASKIT_RES STREQUAL "Process terminated due to timeout")
		message(STATUS "Successfully created ${SASFIT_EXEC}")
	else(SASKIT_RES EQUAL 0 OR SASKIT_RES STREQUAL "Process terminated due to timeout")
		message(STATUS "An error occured while creating ${SASFIT_EXEC}!")
		message("return value: '${SASKIT_RES}'")
		message("output: '${SASKIT_OUT}'")
	endif(SASKIT_RES EQUAL 0 OR SASKIT_RES STREQUAL "Process terminated due to timeout")
	file(REMOVE ${SASKIT_FILE}_rt)
endif(EXISTS ${SASKIT_FILE})

