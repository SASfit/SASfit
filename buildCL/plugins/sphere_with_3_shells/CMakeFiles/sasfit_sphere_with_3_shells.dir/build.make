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
include plugins/sphere_with_3_shells/CMakeFiles/sasfit_sphere_with_3_shells.dir/depend.make

# Include the progress variables for this target.
include plugins/sphere_with_3_shells/CMakeFiles/sasfit_sphere_with_3_shells.dir/progress.make

# Include the compile flags for this target's objects.
include plugins/sphere_with_3_shells/CMakeFiles/sasfit_sphere_with_3_shells.dir/flags.make

plugins/sphere_with_3_shells/CMakeFiles/sasfit_sphere_with_3_shells.dir/sasfit_ff_sphere_with_3_shells.c.obj: plugins/sphere_with_3_shells/CMakeFiles/sasfit_sphere_with_3_shells.dir/flags.make
plugins/sphere_with_3_shells/CMakeFiles/sasfit_sphere_with_3_shells.dir/sasfit_ff_sphere_with_3_shells.c.obj: plugins/sphere_with_3_shells/CMakeFiles/sasfit_sphere_with_3_shells.dir/includes_C.rsp
plugins/sphere_with_3_shells/CMakeFiles/sasfit_sphere_with_3_shells.dir/sasfit_ff_sphere_with_3_shells.c.obj: C:/user/SASfitGit/src/plugins/sphere_with_3_shells/sasfit_ff_sphere_with_3_shells.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\user\SASfitGit\buildCL\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object plugins/sphere_with_3_shells/CMakeFiles/sasfit_sphere_with_3_shells.dir/sasfit_ff_sphere_with_3_shells.c.obj"
	cd /d C:\user\SASfitGit\buildCL\plugins\sphere_with_3_shells && c:\MinGW\bin\gcc.exe  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles\sasfit_sphere_with_3_shells.dir\sasfit_ff_sphere_with_3_shells.c.obj   -c C:\user\SASfitGit\src\plugins\sphere_with_3_shells\sasfit_ff_sphere_with_3_shells.c

plugins/sphere_with_3_shells/CMakeFiles/sasfit_sphere_with_3_shells.dir/sasfit_ff_sphere_with_3_shells.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/sasfit_sphere_with_3_shells.dir/sasfit_ff_sphere_with_3_shells.c.i"
	cd /d C:\user\SASfitGit\buildCL\plugins\sphere_with_3_shells && c:\MinGW\bin\gcc.exe  $(C_DEFINES) $(C_FLAGS) -E C:\user\SASfitGit\src\plugins\sphere_with_3_shells\sasfit_ff_sphere_with_3_shells.c > CMakeFiles\sasfit_sphere_with_3_shells.dir\sasfit_ff_sphere_with_3_shells.c.i

plugins/sphere_with_3_shells/CMakeFiles/sasfit_sphere_with_3_shells.dir/sasfit_ff_sphere_with_3_shells.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/sasfit_sphere_with_3_shells.dir/sasfit_ff_sphere_with_3_shells.c.s"
	cd /d C:\user\SASfitGit\buildCL\plugins\sphere_with_3_shells && c:\MinGW\bin\gcc.exe  $(C_DEFINES) $(C_FLAGS) -S C:\user\SASfitGit\src\plugins\sphere_with_3_shells\sasfit_ff_sphere_with_3_shells.c -o CMakeFiles\sasfit_sphere_with_3_shells.dir\sasfit_ff_sphere_with_3_shells.c.s

plugins/sphere_with_3_shells/CMakeFiles/sasfit_sphere_with_3_shells.dir/sasfit_ff_sphere_with_3_shells.c.obj.requires:

.PHONY : plugins/sphere_with_3_shells/CMakeFiles/sasfit_sphere_with_3_shells.dir/sasfit_ff_sphere_with_3_shells.c.obj.requires

plugins/sphere_with_3_shells/CMakeFiles/sasfit_sphere_with_3_shells.dir/sasfit_ff_sphere_with_3_shells.c.obj.provides: plugins/sphere_with_3_shells/CMakeFiles/sasfit_sphere_with_3_shells.dir/sasfit_ff_sphere_with_3_shells.c.obj.requires
	$(MAKE) -f plugins\sphere_with_3_shells\CMakeFiles\sasfit_sphere_with_3_shells.dir\build.make plugins/sphere_with_3_shells/CMakeFiles/sasfit_sphere_with_3_shells.dir/sasfit_ff_sphere_with_3_shells.c.obj.provides.build
.PHONY : plugins/sphere_with_3_shells/CMakeFiles/sasfit_sphere_with_3_shells.dir/sasfit_ff_sphere_with_3_shells.c.obj.provides

plugins/sphere_with_3_shells/CMakeFiles/sasfit_sphere_with_3_shells.dir/sasfit_ff_sphere_with_3_shells.c.obj.provides.build: plugins/sphere_with_3_shells/CMakeFiles/sasfit_sphere_with_3_shells.dir/sasfit_ff_sphere_with_3_shells.c.obj


plugins/sphere_with_3_shells/CMakeFiles/sasfit_sphere_with_3_shells.dir/interface.c.obj: plugins/sphere_with_3_shells/CMakeFiles/sasfit_sphere_with_3_shells.dir/flags.make
plugins/sphere_with_3_shells/CMakeFiles/sasfit_sphere_with_3_shells.dir/interface.c.obj: plugins/sphere_with_3_shells/CMakeFiles/sasfit_sphere_with_3_shells.dir/includes_C.rsp
plugins/sphere_with_3_shells/CMakeFiles/sasfit_sphere_with_3_shells.dir/interface.c.obj: C:/user/SASfitGit/src/plugins/sphere_with_3_shells/interface.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\user\SASfitGit\buildCL\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object plugins/sphere_with_3_shells/CMakeFiles/sasfit_sphere_with_3_shells.dir/interface.c.obj"
	cd /d C:\user\SASfitGit\buildCL\plugins\sphere_with_3_shells && c:\MinGW\bin\gcc.exe  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles\sasfit_sphere_with_3_shells.dir\interface.c.obj   -c C:\user\SASfitGit\src\plugins\sphere_with_3_shells\interface.c

plugins/sphere_with_3_shells/CMakeFiles/sasfit_sphere_with_3_shells.dir/interface.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/sasfit_sphere_with_3_shells.dir/interface.c.i"
	cd /d C:\user\SASfitGit\buildCL\plugins\sphere_with_3_shells && c:\MinGW\bin\gcc.exe  $(C_DEFINES) $(C_FLAGS) -E C:\user\SASfitGit\src\plugins\sphere_with_3_shells\interface.c > CMakeFiles\sasfit_sphere_with_3_shells.dir\interface.c.i

plugins/sphere_with_3_shells/CMakeFiles/sasfit_sphere_with_3_shells.dir/interface.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/sasfit_sphere_with_3_shells.dir/interface.c.s"
	cd /d C:\user\SASfitGit\buildCL\plugins\sphere_with_3_shells && c:\MinGW\bin\gcc.exe  $(C_DEFINES) $(C_FLAGS) -S C:\user\SASfitGit\src\plugins\sphere_with_3_shells\interface.c -o CMakeFiles\sasfit_sphere_with_3_shells.dir\interface.c.s

plugins/sphere_with_3_shells/CMakeFiles/sasfit_sphere_with_3_shells.dir/interface.c.obj.requires:

.PHONY : plugins/sphere_with_3_shells/CMakeFiles/sasfit_sphere_with_3_shells.dir/interface.c.obj.requires

plugins/sphere_with_3_shells/CMakeFiles/sasfit_sphere_with_3_shells.dir/interface.c.obj.provides: plugins/sphere_with_3_shells/CMakeFiles/sasfit_sphere_with_3_shells.dir/interface.c.obj.requires
	$(MAKE) -f plugins\sphere_with_3_shells\CMakeFiles\sasfit_sphere_with_3_shells.dir\build.make plugins/sphere_with_3_shells/CMakeFiles/sasfit_sphere_with_3_shells.dir/interface.c.obj.provides.build
.PHONY : plugins/sphere_with_3_shells/CMakeFiles/sasfit_sphere_with_3_shells.dir/interface.c.obj.provides

plugins/sphere_with_3_shells/CMakeFiles/sasfit_sphere_with_3_shells.dir/interface.c.obj.provides.build: plugins/sphere_with_3_shells/CMakeFiles/sasfit_sphere_with_3_shells.dir/interface.c.obj


# Object files for target sasfit_sphere_with_3_shells
sasfit_sphere_with_3_shells_OBJECTS = \
"CMakeFiles/sasfit_sphere_with_3_shells.dir/sasfit_ff_sphere_with_3_shells.c.obj" \
"CMakeFiles/sasfit_sphere_with_3_shells.dir/interface.c.obj"

# External object files for target sasfit_sphere_with_3_shells
sasfit_sphere_with_3_shells_EXTERNAL_OBJECTS =

C:/user/SASfitGit/src/plugins/sphere_with_3_shells/lib/libsasfit_sphere_with_3_shells.dll: plugins/sphere_with_3_shells/CMakeFiles/sasfit_sphere_with_3_shells.dir/sasfit_ff_sphere_with_3_shells.c.obj
C:/user/SASfitGit/src/plugins/sphere_with_3_shells/lib/libsasfit_sphere_with_3_shells.dll: plugins/sphere_with_3_shells/CMakeFiles/sasfit_sphere_with_3_shells.dir/interface.c.obj
C:/user/SASfitGit/src/plugins/sphere_with_3_shells/lib/libsasfit_sphere_with_3_shells.dll: plugins/sphere_with_3_shells/CMakeFiles/sasfit_sphere_with_3_shells.dir/build.make
C:/user/SASfitGit/src/plugins/sphere_with_3_shells/lib/libsasfit_sphere_with_3_shells.dll: C:/user/SASfitGit/src/gsl/windows_amd64/lib/libgsl.a
C:/user/SASfitGit/src/plugins/sphere_with_3_shells/lib/libsasfit_sphere_with_3_shells.dll: C:/user/SASfitGit/src/gsl/windows_amd64/lib/libgslcblas.a
C:/user/SASfitGit/src/plugins/sphere_with_3_shells/lib/libsasfit_sphere_with_3_shells.dll: plugins/sphere_with_3_shells/CMakeFiles/sasfit_sphere_with_3_shells.dir/linklibs.rsp
C:/user/SASfitGit/src/plugins/sphere_with_3_shells/lib/libsasfit_sphere_with_3_shells.dll: plugins/sphere_with_3_shells/CMakeFiles/sasfit_sphere_with_3_shells.dir/objects1.rsp
C:/user/SASfitGit/src/plugins/sphere_with_3_shells/lib/libsasfit_sphere_with_3_shells.dll: plugins/sphere_with_3_shells/CMakeFiles/sasfit_sphere_with_3_shells.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=C:\user\SASfitGit\buildCL\CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking C shared module C:\user\SASfitGit\src\plugins\sphere_with_3_shells\lib\libsasfit_sphere_with_3_shells.dll"
	cd /d C:\user\SASfitGit\buildCL\plugins\sphere_with_3_shells && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\sasfit_sphere_with_3_shells.dir\link.txt --verbose=$(VERBOSE)
	cd /d C:\user\SASfitGit\buildCL\plugins\sphere_with_3_shells && "c:\Program Files (x86)\CMake\bin\cmake.exe" -E copy C:/user/SASfitGit/src/plugins/sphere_with_3_shells/lib/./libsasfit_sphere_with_3_shells.dll C:/user/SASfitGit/plugins/sasfit_sphere_with_3_shells.dll
	cd /d C:\user\SASfitGit\buildCL\plugins\sphere_with_3_shells && "c:\Program Files (x86)\CMake\bin\cmake.exe" -E copy C:/user/SASfitGit/src/plugins/sphere_with_3_shells/include/sasfit_sphere_with_3_shells.h C:/user/SASfitGit/plugins

# Rule to build all files generated by this target.
plugins/sphere_with_3_shells/CMakeFiles/sasfit_sphere_with_3_shells.dir/build: C:/user/SASfitGit/src/plugins/sphere_with_3_shells/lib/libsasfit_sphere_with_3_shells.dll

.PHONY : plugins/sphere_with_3_shells/CMakeFiles/sasfit_sphere_with_3_shells.dir/build

plugins/sphere_with_3_shells/CMakeFiles/sasfit_sphere_with_3_shells.dir/requires: plugins/sphere_with_3_shells/CMakeFiles/sasfit_sphere_with_3_shells.dir/sasfit_ff_sphere_with_3_shells.c.obj.requires
plugins/sphere_with_3_shells/CMakeFiles/sasfit_sphere_with_3_shells.dir/requires: plugins/sphere_with_3_shells/CMakeFiles/sasfit_sphere_with_3_shells.dir/interface.c.obj.requires

.PHONY : plugins/sphere_with_3_shells/CMakeFiles/sasfit_sphere_with_3_shells.dir/requires

plugins/sphere_with_3_shells/CMakeFiles/sasfit_sphere_with_3_shells.dir/clean:
	cd /d C:\user\SASfitGit\buildCL\plugins\sphere_with_3_shells && $(CMAKE_COMMAND) -P CMakeFiles\sasfit_sphere_with_3_shells.dir\cmake_clean.cmake
.PHONY : plugins/sphere_with_3_shells/CMakeFiles/sasfit_sphere_with_3_shells.dir/clean

plugins/sphere_with_3_shells/CMakeFiles/sasfit_sphere_with_3_shells.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" C:\user\SASfitGit\src C:\user\SASfitGit\src\plugins\sphere_with_3_shells C:\user\SASfitGit\buildCL C:\user\SASfitGit\buildCL\plugins\sphere_with_3_shells C:\user\SASfitGit\buildCL\plugins\sphere_with_3_shells\CMakeFiles\sasfit_sphere_with_3_shells.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : plugins/sphere_with_3_shells/CMakeFiles/sasfit_sphere_with_3_shells.dir/depend
