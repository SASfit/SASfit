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
include plugins/lognorm_fp/CMakeFiles/sasfit_lognorm_fp.dir/depend.make

# Include the progress variables for this target.
include plugins/lognorm_fp/CMakeFiles/sasfit_lognorm_fp.dir/progress.make

# Include the compile flags for this target's objects.
include plugins/lognorm_fp/CMakeFiles/sasfit_lognorm_fp.dir/flags.make

plugins/lognorm_fp/CMakeFiles/sasfit_lognorm_fp.dir/sasfit_sd_lognorm_fp.c.obj: plugins/lognorm_fp/CMakeFiles/sasfit_lognorm_fp.dir/flags.make
plugins/lognorm_fp/CMakeFiles/sasfit_lognorm_fp.dir/sasfit_sd_lognorm_fp.c.obj: plugins/lognorm_fp/CMakeFiles/sasfit_lognorm_fp.dir/includes_C.rsp
plugins/lognorm_fp/CMakeFiles/sasfit_lognorm_fp.dir/sasfit_sd_lognorm_fp.c.obj: C:/user/SASfitGit/src/plugins/lognorm_fp/sasfit_sd_lognorm_fp.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\user\SASfitGit\buildCL\CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object plugins/lognorm_fp/CMakeFiles/sasfit_lognorm_fp.dir/sasfit_sd_lognorm_fp.c.obj"
	cd /d C:\user\SASfitGit\buildCL\plugins\lognorm_fp && c:\MinGW\bin\gcc.exe  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles\sasfit_lognorm_fp.dir\sasfit_sd_lognorm_fp.c.obj   -c C:\user\SASfitGit\src\plugins\lognorm_fp\sasfit_sd_lognorm_fp.c

plugins/lognorm_fp/CMakeFiles/sasfit_lognorm_fp.dir/sasfit_sd_lognorm_fp.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/sasfit_lognorm_fp.dir/sasfit_sd_lognorm_fp.c.i"
	cd /d C:\user\SASfitGit\buildCL\plugins\lognorm_fp && c:\MinGW\bin\gcc.exe  $(C_DEFINES) $(C_FLAGS) -E C:\user\SASfitGit\src\plugins\lognorm_fp\sasfit_sd_lognorm_fp.c > CMakeFiles\sasfit_lognorm_fp.dir\sasfit_sd_lognorm_fp.c.i

plugins/lognorm_fp/CMakeFiles/sasfit_lognorm_fp.dir/sasfit_sd_lognorm_fp.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/sasfit_lognorm_fp.dir/sasfit_sd_lognorm_fp.c.s"
	cd /d C:\user\SASfitGit\buildCL\plugins\lognorm_fp && c:\MinGW\bin\gcc.exe  $(C_DEFINES) $(C_FLAGS) -S C:\user\SASfitGit\src\plugins\lognorm_fp\sasfit_sd_lognorm_fp.c -o CMakeFiles\sasfit_lognorm_fp.dir\sasfit_sd_lognorm_fp.c.s

plugins/lognorm_fp/CMakeFiles/sasfit_lognorm_fp.dir/sasfit_sd_lognorm_fp.c.obj.requires:

.PHONY : plugins/lognorm_fp/CMakeFiles/sasfit_lognorm_fp.dir/sasfit_sd_lognorm_fp.c.obj.requires

plugins/lognorm_fp/CMakeFiles/sasfit_lognorm_fp.dir/sasfit_sd_lognorm_fp.c.obj.provides: plugins/lognorm_fp/CMakeFiles/sasfit_lognorm_fp.dir/sasfit_sd_lognorm_fp.c.obj.requires
	$(MAKE) -f plugins\lognorm_fp\CMakeFiles\sasfit_lognorm_fp.dir\build.make plugins/lognorm_fp/CMakeFiles/sasfit_lognorm_fp.dir/sasfit_sd_lognorm_fp.c.obj.provides.build
.PHONY : plugins/lognorm_fp/CMakeFiles/sasfit_lognorm_fp.dir/sasfit_sd_lognorm_fp.c.obj.provides

plugins/lognorm_fp/CMakeFiles/sasfit_lognorm_fp.dir/sasfit_sd_lognorm_fp.c.obj.provides.build: plugins/lognorm_fp/CMakeFiles/sasfit_lognorm_fp.dir/sasfit_sd_lognorm_fp.c.obj


plugins/lognorm_fp/CMakeFiles/sasfit_lognorm_fp.dir/interface.c.obj: plugins/lognorm_fp/CMakeFiles/sasfit_lognorm_fp.dir/flags.make
plugins/lognorm_fp/CMakeFiles/sasfit_lognorm_fp.dir/interface.c.obj: plugins/lognorm_fp/CMakeFiles/sasfit_lognorm_fp.dir/includes_C.rsp
plugins/lognorm_fp/CMakeFiles/sasfit_lognorm_fp.dir/interface.c.obj: C:/user/SASfitGit/src/plugins/lognorm_fp/interface.c
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=C:\user\SASfitGit\buildCL\CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object plugins/lognorm_fp/CMakeFiles/sasfit_lognorm_fp.dir/interface.c.obj"
	cd /d C:\user\SASfitGit\buildCL\plugins\lognorm_fp && c:\MinGW\bin\gcc.exe  $(C_DEFINES) $(C_FLAGS) -o CMakeFiles\sasfit_lognorm_fp.dir\interface.c.obj   -c C:\user\SASfitGit\src\plugins\lognorm_fp\interface.c

plugins/lognorm_fp/CMakeFiles/sasfit_lognorm_fp.dir/interface.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/sasfit_lognorm_fp.dir/interface.c.i"
	cd /d C:\user\SASfitGit\buildCL\plugins\lognorm_fp && c:\MinGW\bin\gcc.exe  $(C_DEFINES) $(C_FLAGS) -E C:\user\SASfitGit\src\plugins\lognorm_fp\interface.c > CMakeFiles\sasfit_lognorm_fp.dir\interface.c.i

plugins/lognorm_fp/CMakeFiles/sasfit_lognorm_fp.dir/interface.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/sasfit_lognorm_fp.dir/interface.c.s"
	cd /d C:\user\SASfitGit\buildCL\plugins\lognorm_fp && c:\MinGW\bin\gcc.exe  $(C_DEFINES) $(C_FLAGS) -S C:\user\SASfitGit\src\plugins\lognorm_fp\interface.c -o CMakeFiles\sasfit_lognorm_fp.dir\interface.c.s

plugins/lognorm_fp/CMakeFiles/sasfit_lognorm_fp.dir/interface.c.obj.requires:

.PHONY : plugins/lognorm_fp/CMakeFiles/sasfit_lognorm_fp.dir/interface.c.obj.requires

plugins/lognorm_fp/CMakeFiles/sasfit_lognorm_fp.dir/interface.c.obj.provides: plugins/lognorm_fp/CMakeFiles/sasfit_lognorm_fp.dir/interface.c.obj.requires
	$(MAKE) -f plugins\lognorm_fp\CMakeFiles\sasfit_lognorm_fp.dir\build.make plugins/lognorm_fp/CMakeFiles/sasfit_lognorm_fp.dir/interface.c.obj.provides.build
.PHONY : plugins/lognorm_fp/CMakeFiles/sasfit_lognorm_fp.dir/interface.c.obj.provides

plugins/lognorm_fp/CMakeFiles/sasfit_lognorm_fp.dir/interface.c.obj.provides.build: plugins/lognorm_fp/CMakeFiles/sasfit_lognorm_fp.dir/interface.c.obj


# Object files for target sasfit_lognorm_fp
sasfit_lognorm_fp_OBJECTS = \
"CMakeFiles/sasfit_lognorm_fp.dir/sasfit_sd_lognorm_fp.c.obj" \
"CMakeFiles/sasfit_lognorm_fp.dir/interface.c.obj"

# External object files for target sasfit_lognorm_fp
sasfit_lognorm_fp_EXTERNAL_OBJECTS =

C:/user/SASfitGit/src/plugins/lognorm_fp/lib/libsasfit_lognorm_fp.dll: plugins/lognorm_fp/CMakeFiles/sasfit_lognorm_fp.dir/sasfit_sd_lognorm_fp.c.obj
C:/user/SASfitGit/src/plugins/lognorm_fp/lib/libsasfit_lognorm_fp.dll: plugins/lognorm_fp/CMakeFiles/sasfit_lognorm_fp.dir/interface.c.obj
C:/user/SASfitGit/src/plugins/lognorm_fp/lib/libsasfit_lognorm_fp.dll: plugins/lognorm_fp/CMakeFiles/sasfit_lognorm_fp.dir/build.make
C:/user/SASfitGit/src/plugins/lognorm_fp/lib/libsasfit_lognorm_fp.dll: C:/user/SASfitGit/src/gsl/windows_amd64/lib/libgsl.a
C:/user/SASfitGit/src/plugins/lognorm_fp/lib/libsasfit_lognorm_fp.dll: C:/user/SASfitGit/src/gsl/windows_amd64/lib/libgslcblas.a
C:/user/SASfitGit/src/plugins/lognorm_fp/lib/libsasfit_lognorm_fp.dll: plugins/lognorm_fp/CMakeFiles/sasfit_lognorm_fp.dir/linklibs.rsp
C:/user/SASfitGit/src/plugins/lognorm_fp/lib/libsasfit_lognorm_fp.dll: plugins/lognorm_fp/CMakeFiles/sasfit_lognorm_fp.dir/objects1.rsp
C:/user/SASfitGit/src/plugins/lognorm_fp/lib/libsasfit_lognorm_fp.dll: plugins/lognorm_fp/CMakeFiles/sasfit_lognorm_fp.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=C:\user\SASfitGit\buildCL\CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking C shared module C:\user\SASfitGit\src\plugins\lognorm_fp\lib\libsasfit_lognorm_fp.dll"
	cd /d C:\user\SASfitGit\buildCL\plugins\lognorm_fp && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles\sasfit_lognorm_fp.dir\link.txt --verbose=$(VERBOSE)
	cd /d C:\user\SASfitGit\buildCL\plugins\lognorm_fp && "c:\Program Files (x86)\CMake\bin\cmake.exe" -E copy C:/user/SASfitGit/src/plugins/lognorm_fp/lib/./libsasfit_lognorm_fp.dll C:/user/SASfitGit/plugins/sasfit_lognorm_fp.dll
	cd /d C:\user\SASfitGit\buildCL\plugins\lognorm_fp && "c:\Program Files (x86)\CMake\bin\cmake.exe" -E copy C:/user/SASfitGit/src/plugins/lognorm_fp/include/sasfit_lognorm_fp.h C:/user/SASfitGit/plugins

# Rule to build all files generated by this target.
plugins/lognorm_fp/CMakeFiles/sasfit_lognorm_fp.dir/build: C:/user/SASfitGit/src/plugins/lognorm_fp/lib/libsasfit_lognorm_fp.dll

.PHONY : plugins/lognorm_fp/CMakeFiles/sasfit_lognorm_fp.dir/build

plugins/lognorm_fp/CMakeFiles/sasfit_lognorm_fp.dir/requires: plugins/lognorm_fp/CMakeFiles/sasfit_lognorm_fp.dir/sasfit_sd_lognorm_fp.c.obj.requires
plugins/lognorm_fp/CMakeFiles/sasfit_lognorm_fp.dir/requires: plugins/lognorm_fp/CMakeFiles/sasfit_lognorm_fp.dir/interface.c.obj.requires

.PHONY : plugins/lognorm_fp/CMakeFiles/sasfit_lognorm_fp.dir/requires

plugins/lognorm_fp/CMakeFiles/sasfit_lognorm_fp.dir/clean:
	cd /d C:\user\SASfitGit\buildCL\plugins\lognorm_fp && $(CMAKE_COMMAND) -P CMakeFiles\sasfit_lognorm_fp.dir\cmake_clean.cmake
.PHONY : plugins/lognorm_fp/CMakeFiles/sasfit_lognorm_fp.dir/clean

plugins/lognorm_fp/CMakeFiles/sasfit_lognorm_fp.dir/depend:
	$(CMAKE_COMMAND) -E cmake_depends "MinGW Makefiles" C:\user\SASfitGit\src C:\user\SASfitGit\src\plugins\lognorm_fp C:\user\SASfitGit\buildCL C:\user\SASfitGit\buildCL\plugins\lognorm_fp C:\user\SASfitGit\buildCL\plugins\lognorm_fp\CMakeFiles\sasfit_lognorm_fp.dir\DependInfo.cmake --color=$(COLOR)
.PHONY : plugins/lognorm_fp/CMakeFiles/sasfit_lognorm_fp.dir/depend
