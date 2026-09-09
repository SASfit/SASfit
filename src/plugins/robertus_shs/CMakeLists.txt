# cmake_minimum_required(VERSION 3.0)

# the project name
set(PRJ_NAME sasfit_robertus_shs)

# required external libs which are installed on the system.
set(LIBS_EXT
	GSL
)

# source files
set(SOURCE_${PRJ_NAME}
	robertus_shs_core.c
	sasfit_ff_RobertusSHS_CoreShell.c
	sasfit_ff_RobertusSHS_Fuzzy.c
	sasfit_ff_RobertusSHS_GenericDemo.c
	interface.c
)

# add plugin path for finding 'Find*.cmake' files for new <LIBS_EXT>
set(REL_CMAKE_MODULE_PATH .)

#                                                       #
### no adjustments required below this line (usually) ###
#                                                       #

project(${PRJ_NAME} C)

set(LIB_IS_PLUGIN true)

set(SRC_DIR ${${PRJ_NAME}_SOURCE_DIR})
set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${SRC_DIR}/${MODULE_PATH}")

include(SasfitCmakeUtils)
sasfit_cmake_plugin()

# ---------------------------------------------------------------------------
# SUNDIALS (KINSOL): rshs_solve() in robertus_shs_core.c uses SUNDIALS'
# KINSOL as its primary nonlinear solver (matrix-free Newton-Krylov/GMRES)
# for the Percus-Yevick lambda(i,j) system -- scales far better with nclass
# than the plain-GSL dense solver kept as its automatic fallback.
#
# This links against sundials_kinsol_static/sundials_core_static from
# src/sundials7/ (SUNDIALS 7.x, built from a locally-downloaded release
# tarball -- see that directory's CMakeLists.txt), NOT the project's
# existing bundled SUNDIALS 2.7.0 at src/sundials/ that
# src/sasfit_oz/sasfit_oz_solver.c continues to use unmodified.
# Deliberately two independent, separately-versioned SUNDIALS builds:
# rshs_solve() needs the modern SUNContext-based KINSOL API (2.7.0
# predates SUNContext entirely), and migrating the OZ solver's existing,
# working 2.7.0 usage was out of scope and not something this plugin's
# own testing could validate anyway.
#
# Hiding this plugin's statically-linked SUNDIALS 7.x symbols from the
# dynamic symbol table is REQUIRED here, not optional: both SUNDIALS
# versions export same-named-but-incompatible-ABI symbols (e.g. KINCreate
# takes no arguments in 2.7.0, requires a SUNContext in 7.x), and both this
# plugin's .so/.dylib and the main SASfit binary/library (which contains
# the OZ solver's statically-linked 2.7.0 copy) end up loaded in the same
# process at runtime. Verified directly while developing this on Linux:
# WITHOUT hiding these symbols, a minimal reproduction of exactly this
# scenario (one shared library with each SUNDIALS version statically
# linked, both loaded via dlopen) segfaults immediately during dynamic
# symbol resolution, before either version's own code runs at all. WITH
# it, this plugin's statically-linked SUNDIALS 7.x symbols are kept out of
# the dynamic symbol table entirely -- callable internally, invisible
# externally -- and both versions solve correctly side by side regardless
# of load order. On Windows, DLL exports are explicit rather than
# automatic, so the underlying collision risk this addresses does not
# arise there the same way, and nothing extra is needed.
#
# The mechanism is necessarily linker-specific, and CMAKE_C_COMPILER_ID
# alone is NOT a reliable proxy for which linker is actually invoked: a
# genuine GNU gcc (CMAKE_C_COMPILER_ID "GNU") on macOS -- e.g. Homebrew's
# gcc@12, as used for this project's macOS builds -- still drives Apple's
# native ld64 by default, not GNU ld/gold, and ld64 does not understand
# GNU ld's --exclude-libs. That combination (GNU compiler ID, non-GNU
# linker) is exactly what a compiler-ID-only check would get wrong, so
# this is keyed off the target platform instead:
#   - Linux (GNU ld/gold): -Wl,--exclude-libs=ALL, verified as above.
#   - macOS (Apple ld64): -Wl,-load_hidden,<path-to-archive> per static
#     archive, ld64's own equivalent (present since ld64-609, i.e. long
#     available in any Xcode this project targets; also reimplemented in
#     LLVM's lld-macho for compatibility) -- takes a path rather than a
#     -lNAME-style short name, hence the $<TARGET_FILE:...> generator
#     expressions below rather than plain library names. This achieves
#     the same intent but, unlike the Linux flag above, has NOT been
#     verified here against the actual two-SUNDIALS-versions-in-one-
#     process crash scenario (no Mac available while writing this) -- if
#     a similar segfault ever surfaces on macOS specifically, re-check
#     this first rather than assuming it is proven equivalent to the
#     Linux case.
#   - any other platform (currently just Windows, per above): nothing
#     extra needed, matching the original "harmless no-op" fallback intent
#     -- just made explicit/platform-based rather than relying on a
#     compiler-ID check that does not actually track the linker.
#
# Plain (non-keyword) target_link_libraries signature below, deliberately:
# sasfit_cmake_plugin() (in SasfitCmakeUtils.cmake) already called
# target_link_libraries(${PRJ_NAME} ...) on this same target using the
# plain signature (no PUBLIC/PRIVATE/INTERFACE keyword) for GSL -- CMake
# does not allow mixing plain and keyword-style target_link_libraries
# calls on the same target, so this must match that, not use PRIVATE.
#
# The real risk here was the top-level src/CMakeLists.txt leaking the
# OLD sundials/ (2.7.0) include path into every plugin via a directory-
# scope include_directories() call (which propagates into every
# add_subdirectory() processed afterward, this one included) --
# confirmed by a real build to break this plugin's compile, since
# 2.7.0's copy of shared-name headers (e.g. sundials/sundials_types.h)
# doesn't define any of 7.x's new types. That leak has been fixed at
# its source, by converting that call into a target-scoped
# target_include_directories(sasfit PRIVATE ...) there instead (see the
# comment in src/CMakeLists.txt) -- so a plain PRIVATE include here is
# sufficient; no BEFORE/reordering trick is needed on this side.
# ---------------------------------------------------------------------------
if(NOT TARGET sundials_kinsol_static)
	add_subdirectory(${CMAKE_SOURCE_DIR}/sundials7 ${CMAKE_BINARY_DIR}/sundials7)
endif()
target_link_libraries(${PRJ_NAME} sundials_kinsol_static sundials_core_static)
target_include_directories(${PRJ_NAME} PRIVATE ${sundials7_SOURCE_DIR}/include ${sundials7_BINARY_DIR}/include)
if(CMAKE_SYSTEM_NAME STREQUAL "Linux")
	target_link_options(${PRJ_NAME} PRIVATE "-Wl,--exclude-libs=ALL")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
	target_link_options(${PRJ_NAME} PRIVATE
		"-Wl,-load_hidden,$<TARGET_FILE:sundials_kinsol_static>"
		"-Wl,-load_hidden,$<TARGET_FILE:sundials_core_static>")
endif()

# ---------------------------------------------------------------------------
# Optional OpenMP support for robertus_shs_core.c's batch functions
# (rshs_intensity_coreshell_batch/_fuzzy_batch, see robertus_shs_core.h).
#
# This plugin does NOT decide on its own whether to use OpenMP. The main
# SASfit project currently does not use OpenMP anywhere in its own code, and
# defines SASFIT_WITH_OPENMP (default OFF, see src/CMakeLists.txt) as the
# single project-wide switch for that. Since plugins are pulled in via
# add_subdirectory(plugins) from the same top-level configure run, that
# variable is already visible here through ordinary CMake parent/child
# directory scoping -- no extra plumbing needed. This plugin simply mirrors
# it: OpenMP is only looked for/linked when the main project has it switched
# on, so enabling or disabling OpenMP for the whole project and every plugin
# that supports it stays a single switch.
#
# Without this block (or with the option left OFF, its default), the
# #ifdef _OPENMP guards in robertus_shs_core.c simply compile to plain
# sequential loops -- correct, just single-threaded, and that remains the
# out-of-the-box behaviour here too.
#
# SASFIT_WITH_OPENMP may not even be defined if this plugin is ever
# configured standalone, outside the main SASfit source tree (e.g. for
# isolated testing) -- DEFINED is checked first so that case degrades to
# "no OpenMP" rather than an unbound-variable warning.
# ---------------------------------------------------------------------------
if(DEFINED SASFIT_WITH_OPENMP AND SASFIT_WITH_OPENMP)
	find_package(OpenMP)
	if(OpenMP_C_FOUND)
		target_link_libraries(${PRJ_NAME} OpenMP::OpenMP_C)
		message(STATUS "robertus_shs: SASFIT_WITH_OPENMP is ON and OpenMP was found -- batch functions will parallelise across q-points.")
	else()
		message(STATUS "robertus_shs: SASFIT_WITH_OPENMP is ON but OpenMP was not found -- batch functions will run sequentially (still correct, just single-threaded).")
	endif()
else()
	message(STATUS "robertus_shs: SASFIT_WITH_OPENMP is OFF (or undefined) -- batch functions will run sequentially (still correct, just single-threaded).")
endif()
