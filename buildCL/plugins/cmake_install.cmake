# Install script for directory: C:/user/SASfitGit/src/plugins

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "C:/Program Files (x86)/sasfit")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("C:/user/SASfitGit/buildCL/plugins/anisotropic_objects/cmake_install.cmake")
  include("C:/user/SASfitGit/buildCL/plugins/baba_ahmed/cmake_install.cmake")
  include("C:/user/SASfitGit/buildCL/plugins/boucherprofile/cmake_install.cmake")
  include("C:/user/SASfitGit/buildCL/plugins/ferrofluid/cmake_install.cmake")
  include("C:/user/SASfitGit/buildCL/plugins/fractal_series/cmake_install.cmake")
  include("C:/user/SASfitGit/buildCL/plugins/fuzzysphere/cmake_install.cmake")
  include("C:/user/SASfitGit/buildCL/plugins/juelichcoreshell/cmake_install.cmake")
  include("C:/user/SASfitGit/buildCL/plugins/kratky_sphere/cmake_install.cmake")
  include("C:/user/SASfitGit/buildCL/plugins/langevin_polaris/cmake_install.cmake")
  include("C:/user/SASfitGit/buildCL/plugins/local_planar_micelles/cmake_install.cmake")
  include("C:/user/SASfitGit/buildCL/plugins/lognorm_fp/cmake_install.cmake")
  include("C:/user/SASfitGit/buildCL/plugins/magneticfieldalignedvesicle/cmake_install.cmake")
  include("C:/user/SASfitGit/buildCL/plugins/magnetic_correlation/cmake_install.cmake")
  include("C:/user/SASfitGit/buildCL/plugins/non_particular/cmake_install.cmake")
  include("C:/user/SASfitGit/buildCL/plugins/oientated_primitive_objects/cmake_install.cmake")
  include("C:/user/SASfitGit/buildCL/plugins/ordered_particle_systems/cmake_install.cmake")
  include("C:/user/SASfitGit/buildCL/plugins/parallel_epiped/cmake_install.cmake")
  include("C:/user/SASfitGit/buildCL/plugins/peak/cmake_install.cmake")
  include("C:/user/SASfitGit/buildCL/plugins/pearl_necklace/cmake_install.cmake")
  include("C:/user/SASfitGit/buildCL/plugins/rod_micelle_prism/cmake_install.cmake")
  include("C:/user/SASfitGit/buildCL/plugins/sesans/cmake_install.cmake")
  include("C:/user/SASfitGit/buildCL/plugins/simple_clusters/cmake_install.cmake")
  include("C:/user/SASfitGit/buildCL/plugins/sphere_with_3_shells/cmake_install.cmake")
  include("C:/user/SASfitGit/buildCL/plugins/spline/cmake_install.cmake")
  include("C:/user/SASfitGit/buildCL/plugins/sq_of_oz_solver/cmake_install.cmake")
  include("C:/user/SASfitGit/buildCL/plugins/triax_ellip_shell/cmake_install.cmake")

endif()

