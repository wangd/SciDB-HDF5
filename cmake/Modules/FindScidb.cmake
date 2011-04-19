# -- Check for the SciDB source tree --
#
# Uses path overrides: SCIDBINC_PATH SCIDB_DIR
#
# Defines:
#
#  SCIDB_FOUND - system has a SciDB source checkout
#  SCIDB_INCLUDE_DIR - the SciDB include directory
#  SCIDB_SRC_DIR - SciDB src dir
#  SCIDB_LIBRARIES
#
include(FindPackageHandleStandardArgs)

# Look for includes and libraries
find_path(SCIDB_INCLUDE_ARRAY  array/Array.h PATHS $ENV{SCIDBINC_PATH} ${SCIDB_DIR}/include ${SCIDB_DIR}/src)
find_path(SCIDB_INCLUDE_CFG  network/SciDBConfig.h PATHS $ENV{SCIDBINC_PATH} ${SCIDB_DIR}/include ${SCIDB_DIR}/src)
find_path(SCIDB_SRC_DIR array/Metadata.cpp PATH ${SCIDB_DIR}/src)

find_package_handle_standard_args(SCIDB DEFAULT_MSG SCIDB_INCLUDE_ARRAY SCIDB_INCLUDE_CFG SCIDB_SRC_DIR)

if(SCIDB_FOUND)
  set(SCIDB_INCLUDES ${SCIDB_INCLUDE_ARRAY} ${SCIDB_INCLUDE_CFG})
endif(SCIDB_FOUND)
mark_as_advanced(Scidb_FOUND SCIDB_INCLUDES SCIDB_SRC_DIR)



