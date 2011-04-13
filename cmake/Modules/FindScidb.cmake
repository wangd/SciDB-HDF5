# -- Check for the SciDB source tree --
#
# Uses path overrides: SCIDBINC_PATH SCIDB_DIR
#
# Defines:
#
#  SCIDB_FOUND - system has a SciDB source checkout
#  SCIDB_INCLUDE_DIR - the SciDB include directory
#  SCIDB_LIBRARIES
#
include(FindPackageHandleStandardArgs)

# Look for includes and libraries
find_path(SCIDB_INCLUDE_DIR  array/Array.h PATHS $ENV{SCIDBINC_PATH} ${SCIDB_DIR}/include)

find_package_handle_standard_args(Scidb DEFAULT_MSG  SCIDB_INCLUDE_DIR)

if(Scidb_FOUND)
  set(SCIDB_INCLUDES ${SCIDB_INCLUDE_DIR})
endif(Scidb_FOUND)
mark_as_advanced(SCIDB_FOUND SCIDB_INCLUDE_DIR)



