# CMAKE generated file: DO NOT EDIT!
# Generated by "MinGW Makefiles" Generator, CMake Version 3.3

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:


#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:


# Remove some rules from gmake that .SUFFIXES does not remove.
SUFFIXES =

.SUFFIXES: .hpux_make_needs_suffix_list


# Suppress display of executed commands.
$(VERBOSE).SILENT:


# A target that is always out of date.
cmake_force:

.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

SHELL = cmd.exe

# The CMake executable.
CMAKE_COMMAND = "c:\Program Files (x86)\CMake\bin\cmake.exe"

# The command to remove a file.
RM = "c:\Program Files (x86)\CMake\bin\cmake.exe" -E remove -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = C:\user\SASfitGit\src

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = C:\user\SASfitGit\buildCL

# Include any dependencies generated for this target.
include plugins/m1st/CMakeFiles/sasfit_m1st.dir/depend.make

# Include the progress variables for this target.
include plugins/m1st/CMakeFiles/sasfit_m1st.dir/progress.make

# Include the compile flags for this target's objects.
include plugins/m1st/CMakeFiles/sasfit_m1st.dir/flags.make

plugins/m1st/CMakeFiles/sasfit_m1st.dir/sasfit_ff_wasanderes.c.obj: plugins/m1st/CMakeFiles/sasfit_m1st.dir/flags.make
plugins/m1st/CMakeFiles/sasfit_m1st.dir/sasfit_ff_wasanderes.c.obj: plugins/m1st/CMakeFiles/sasfit_m1st.dir/includes_C.rsp
plugins/m1st/CMakeFiles/sasfit_m1st.dir/sasfit_ff_wasanderes.c.obj: C:/user/SASfitGit/src/plugins/m1st/sasfit_ff_wasanderes.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\user\SASfitGit\buildCL\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object plugins/m1st/CMakeFiles/sasfit_m1st.dir/sasfit_ff_wasanderes.c.obj"
	cd /d C:\user\SASfitGit\buildCL\plugins\m1st && c:\MinGW\bin\gcc.exe  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles\sasfit_m1st.dir\sasfit_ff_wasanderes.c.obj   -c C:\user\SASfitGit\src\plugins\m1st\sasfit_ff_wasanderes.c

plugins/m1st/CMakeFiles/sasfit_m1st.dir/sasfit_ff_wasanderes.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/sasfit_m1st.dir/sasfit_ff_wasanderes.c.i"
	cd /d C:\user\SASfitGit\buildCL\plugins\m1st && c:\MinGW\bin\gcc.exe  $(C_DEFINES) $(C_FLAGS) -E C:\user\SASfitGit\src\plugins\m1st\sasfit_ff_wasanderes.c > CMakeFiles\sasfit_m1st.dir\sasfit_ff_wasanderes.c.i

plugins/m1st/CMakeFiles/sasfit_m1st.dir/sasfit_ff_wasanderes.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/sasfit_m1st.dir/sasfit_ff_wasanderes.c.s"
	cd /d C:\user\SASfitGit\buildCL\plugins\m1st && c:\MinGW\bin\gcc.exe  $(C_DEFINES) $(C_FLAGS) -S C:\user\SASfitGit\src\plugins\m1st\sasfit_ff_wasanderes.c -o CMakeFiles\sasfit_m1st.dir\sasfit_ff_wasanderes.c.s

plugins/m1st/CMakeFiles/sasfit_m1st.dir/sasfit_ff_wasanderes.c.obj.requires:

.PHONY : plugins/m1st/CMakeFiles/sasfit_m1st.dir/sasfit_ff_wasanderes.c.obj.requires

plugins/m1st/CMakeFiles/sasfit_m1st.dir/sasfit_ff_wasanderes.c.obj.provides: plugins/m1st/CMakeFiles/sasfit_m1st.dir/sasfit_ff_wasanderes.c.obj.requires
	$(MAKE) -f plugins\m1st\CMakeFiles\sasfit_m1st.dir\build.make plugins/m1st/CMakeFiles/sasfit_m1st.dir/sasfit_ff_wasanderes.c.obj.provides.build
.PHONY : plugins/m1st/CMakeFiles/sasfit_m1st.dir/sasfit_ff_wasanderes.c.obj.provides

plugins/m1st/CMakeFiles/sasfit_m1st.dir/sasfit_ff_wasanderes.c.obj.provides.build: plugins/m1st/CMakeFiles/sasfit_m1st.dir/sasfit_ff_wasanderes.c.obj


plugins/m1st/CMakeFiles/sasfit_m1st.dir/interface.c.obj: plugins/m1st/CMakeFiles/sasfit_m1st.dir/flags.make
plugins/m1st/CMakeFiles/sasfit_m1st.dir/interface.c.obj: plugins/m1st/CMakeFiles/sasfit_m1st.dir/includes_C.rsp
plugins/m1st/CMakeFiles/sasfit_m1st.dir/interface.c.obj: C:/user/SASfitGit/src/plugins/m1st/interface.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\user\SASfitGit\buildCL\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object plugins/m1st/CMakeFiles/sasfit_m1st.dir/interface.c.obj"
	cd /d C:\user\SASfitGit\buildCL\plugins\m1st && c:\MinGW\bin\gcc.exe  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles\sasfit_m1st.dir\interface.c.obj   -c C:\user\SASfitGit\src\plugins\m1st\interface.c

plugins/m1st/CMakeFiles/sasfit_m1st.dir/interface.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/sasfit_m1st.dir/interface.c.i"
	cd /d C:\user\SASfitGit\buildCL\plugins\m1st && c:\MinGW\bin\gcc.exe  $(C_DEFINES) $(C_FLAGS) -E C:\user\SASfitGit\src\plugins\m1st\interface.c > CMakeFiles\sasfit_m1st.dir\interface.c.i

plugins/m1st/CMakeFiles/sasfit_m1st.dir/interface.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/sasfit_m1st.dir/interface.c.s"
	cd /d C:\user\SASfitGit\buildCL\plugins\m1st && c:\MinGW\bin\gcc.exe  $(C_DEFINES) $(C_FLAGS) -S C:\user\SASfitGit\src\plugins\m1st\interface.c -o CMakeFiles\sasfit_m1st.dir\interface.c.s

plugins/m1st/CMakeFiles/sasfit_m1st.dir/interface.c.obj.requires:

.PHONY : plugins/m1st/CMakeFiles/sasfit_m1st.dir/interface.c.obj.requires

plugins/m1st/CMakeFiles/sasfit_m1st.dir/interface.c.obj.provides: plugins/m1st/CMakeFiles/sasfit_m1st.dir/interface.c.obj.requires
	$(MAKE) -f plugins\m1st\CMakeFiles\sasfit_m1st.dir\build.make plugins/m1st/CMakeFiles/sasfit_m1st.dir/interface.c.obj.provides.build
.PHONY : plugins/m1st/CMakeFiles/sasfit_m1st.dir/interface.c.obj.provides

plugins/m1st/CMakeFiles/sasfit_m1st.dir/interface.c.obj.provides.build: plugins/m1st/CMakeFiles/sasfit_m1st.dir/interface.c.obj


# Object files for target sasfit_m1st
sasfit_m1st_OBJECTS = \
"CMakeFiles/sasfit_m1st.dir/sasfit_ff_wasanderes.c.obj" \
"CMakeFiles/sasfit_m1st.dir/interface.c.obj"

# External object files for target sasfit_m1st
sasfit_m1st_EXTERNAL_OBJECTS =

C:/user/SASfitGit/src/plugins/m1st/lib/libsasfit_m1st.dll: plugins/m1st/CMakeFiles/sasfit_m1st.dir/sasfit_ff_wasanderes.c.obj
C:/user/SASfitGit/src/plugins/m1st/lib/libsasfit_m1st.dll: plugins/m1st/CMakeFiles/sasfit_m1st.dir/interface.c.obj
C:/user/SASfitGit/src/plugins/m1st/lib/libsasfit_m1st.dll: plugins/m1st/CMakeFiles/sasfit_m1st.dir/build.make
C:/user/SASfitGit/src/plugins/m1st/lib/libsasfit_m1st.dll: C:/user/SASfitGit/src/gsl/windows_amd64/lib/libgsl.a
C:/user/SASfitGit/src/plugins/m1st/lib/libsasfit_m1st.dll: C:/user/SASfitGit/src/gsl/windows_amd64/lib/libgslcblas.a
C:/user/SASfitGit/src/plugins/m1st/lib/libsasfit_m1st.dll: plugins/m1st/CMakeFiles/sasfit_m1st.dir/linklibs.rsp
C:/user/SASfitGit/src/plugins/m1st/lib/libsasfit_m1st.dll: plugins/m1st/CMakeFiles/sasfit_m1st.dir/objects1.rsp
C:/user/SASfitGit/src/plugins/m1st/lib/libsasfit_m1st.dll: plugins/m1st/CMakeFiles/sasfit_m1st.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=C:\user\SASfitGit\buildCL\CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking C shared module C:\user\SASfitGit\src\plugins\m1st\lib\libsasfit_m1st.dll"
	cd /d C:\user\SASfitGit\buildCL\plugins\m1st && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\sasfit_m1st.dir\link.txt --verbose=$(VERBOSE)
	cd /d C:\user\SASfitGit\buildCL\plugins\m1st && "c:\Program Files (x86)\CMake\bin\cmake.exe" -E copy C:/user/SASfitGit/src/plugins/m1st/lib/./libsasfit_m1st.dll C:/user/SASfitGit/plugins/sasfit_m1st.dll
	cd /d C:\user\SASfitGit\buildCL\plugins\m1st && "c:\Program Files (x86)\CMake\bin\cmake.exe" -E copy C:/user/SASfitGit/src/plugins/m1st/include/sasfit_m1st.h C:/user/SASfitGit/plugins

# Rule to build all files generated by this target.
plugins/m1st/CMakeFiles/sasfit_m1st.dir/build: C:/user/SASfitGit/src/plugins/m1st/lib/libsasfit_m1st.dll

.PHONY : plugins/m1st/CMakeFiles/sasfit_m1st.dir/build

plugins/m1st/CMakeFiles/sasfit_m1st.dir/requires: plugins/m1st/CMakeFiles/sasfit_m1st.dir/sasfit_ff_wasanderes.c.obj.requires
plugins/m1st/CMakeFiles/sasfit_m1st.dir/requires: plugins/m1st/CMakeFiles/sasfit_m1st.dir/interface.c.obj.requires

.PHONY : plugins/m1st/CMakeFiles/sasfit_m1st.dir/requires

plugins/m1st/CMakeFiles/sasfit_m1st.dir/clean:
	cd /d C:\user\SASfitGit\buildCL\plugins\m1st && $(CMAKE_COMMAND) -P CMakeFiles\sasfit_m1st.dir\cmake_clean.cmake
.PHONY : plugins/m1st/CMakeFiles/sasfit_m1st.dir/clean

plugins/m1st/CMakeFiles/sasfit_m1st.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" C:\user\SASfitGit\src C:\user\SASfitGit\src\plugins\m1st C:\user\SASfitGit\buildCL C:\user\SASfitGit\buildCL\plugins\m1st C:\user\SASfitGit\buildCL\plugins\m1st\CMakeFiles\sasfit_m1st.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : plugins/m1st/CMakeFiles/sasfit_m1st.dir/depend
