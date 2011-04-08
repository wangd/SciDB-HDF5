# -- Check for SciDB tree --
#
# Uses path overrides: SCIDB_PATH 
#
# Defines:
#
#  SCIDB_FOUND - system has SciDB 
#  SCIDB_INCLUDE_DIR - the SciDB include directory
#
include(FindPackageHandleStandardArgs)

# Look for includes and libraries
find_path(SCIDB_INCLUDE_DIR  array/Array.h PATHS $ENV{SCIDB_PATH}/include)

find_package_handle_standard_args(SCIDB  DEFAULT_MSG  SCIDB_INCLUDE_DIR)

if(SCIDB_FOUND)
else(SCIDB_FOUND)
  message("SciDB sources not found. Can't build SciDB loader")
endif(SCIDB_FOUND)

mark_as_advanced(SCIDB_INCLUDE_DIR)

