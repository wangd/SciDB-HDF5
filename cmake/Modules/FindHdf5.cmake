# -- Check for the HDF5 library --
#
# Defines:
#
#  HDF5_FOUND - system has the HDF5 library
#  HDF5_INCLUDE_DIR - the HDF5 include directory
#  HDF5_LIBRARIES - The library needed to use HDF5

include(FindPackageHandleStandardArgs)

# Look for includes and libraries
find_path(HDF5_INCLUDE_DIR  H5Cpp.h)
find_library(HDF5_LIBRARY_BASE  hdf5)
find_library(HDF5_LIBRARY_CPP  hdf5_cpp)
find_library(HDF5_LIBRARY_HL  hdf5_hl)
find_library(HDF5_LIBRARY_HL_CPP  hdf5_hl_cpp)
find_library(LIBZ z)
find_library(LIBSZ sz)

find_package_handle_standard_args(HDF5  DEFAULT_MSG  HDF5_LIBRARY_BASE  HDF5_LIBRARY_CPP LIBZ HDF5_INCLUDE_DIR)

if(HDF5_FOUND)
  set(HDF5_LIBRARIES  ${HDF5_LIBRARY_HL_CPP}  ${HDF5_LIBRARY_CPP}   ${HDF5_LIBRARY_HL}  ${HDF5_LIBRARY_BASE} )  
else(HDF5_FOUND)
  set(HDF5_LIBRARIES)
endif(HDF5_FOUND)

mark_as_advanced(HDF5_LIBRARIES HDF5_INCLUDE_DIR)

