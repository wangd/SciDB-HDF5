# -- Check for the HDF5 library --
#
# Uses path overrides: H5INC_PATH and H5LIB_PATH 
#
# Defines:
#
#  HDF5_FOUND - system has the HDF5 library
#  HDF5_INCLUDE_DIR - the HDF5 include directory
#  HDF5_LIBRARIES - The library needed to use HDF5
#
include(FindPackageHandleStandardArgs)

# Look for includes and libraries
find_path(HDF5_INCLUDE_DIR  H5Cpp.h PATHS $ENV{H5INC_PATH})
find_library(HDF5_LIBRARY_BASE  hdf5  PATHS $ENV{HDFLIB_PATH})
find_library(HDF5_LIBRARY_CPP  hdf5_cpp  PATHS $ENV{HDFLIB_PATH})
find_library(HDF5_LIBRARY_HL  hdf5_hl  PATHS $ENV{HDFLIB_PATH})
find_library(HDF5_LIBRARY_HL_CPP  hdf5_hl_cpp  PATHS $ENV{HDFLIB_PATH})
find_library(LIBZ z)
find_library(SZ_LIB sz PATHS $ENV{H5LIB_PATH})

find_package_handle_standard_args(HDF5  DEFAULT_MSG  HDF5_LIBRARY_BASE  HDF5_LIBRARY_CPP LIBZ HDF5_INCLUDE_DIR)
find_package_handle_standard_args(LIBSZ  DEFAULT_MSG  SZ_LIB)

if(HDF5_FOUND)
  # Some builds of HDF5 require libsz to be linked separately.
  # If it is available, link it. Otherwise if HDF5 is available but not libsz
  # then silently ignore it.
  if(LIBSZ_FOUND)
    message("Found libsz, assuming HDF5 needs it.")
    set(HDF5_LIBRARIES  ${HDF5_LIBRARY_HL_CPP} ${HDF5_LIBRARY_HL} ${HDF5_LIBRARY_CPP} ${HDF5_LIBRARY_BASE}  ${SZ_LIB} ${LIBZ} )  
  else(LIBSZ_FOUND)
    message("Assuming your HDF5 installation works w/o libsz.")
    set(HDF5_LIBRARIES  ${HDF5_LIBRARY_HL_CPP} ${HDF5_LIBRARY_HL} ${HDF5_LIBRARY_CPP} ${HDF5_LIBRARY_BASE}  ${LIBZ})  
  endif(LIBSZ_FOUND)
else(HDF5_FOUND)
  set(HDF5_LIBRARIES)
endif(HDF5_FOUND)

mark_as_advanced(HDF5_LIBRARIES HDF5_INCLUDE_DIR)

