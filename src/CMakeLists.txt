find_package(ZLIB REQUIRED)
# Point at non-system locations via H5INC_PATH and H5LIB_PATH
find_package(Hdf5  REQUIRED) 
find_package(Boost REQUIRED )
find_package(Scidb REQUIRED)

include_directories(${HDF5_INCLUDE_DIR} ${SCIDB_INCLUDES} ${Boost_INCLUDE_DIRS})

# SciDB special flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SCIDB_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC")


# file dumper
add_executable(loader loader.cc printHdf.cc)
target_link_libraries(loader ${HDF5_LIBRARIES})

# SciDb plugin
set(plugin_src plugin.cc loadOperator.cc H5Array.cc arrayCommon.cc)
add_library(loadhdf SHARED ${plugin_src})
target_link_libraries(loadhdf ${HDF5_LIBRARIES})

# Scidb lib for building test prog.
find_path(SCIDB_BLD_DIR src/system/catalog/libcatalog_lib.a 
  /u22/danielw/scidb-bld)

set(scidbLibs 
  ${SCIDB_BLD_DIR}/src/network/libnetwork_lib.a
  ${SCIDB_BLD_DIR}/src/util/libutil_lib.a
  ${SCIDB_BLD_DIR}/src/system/libsystem_lib.a
  ${SCIDB_BLD_DIR}/src/query/libqproc_lib.a
  ${SCIDB_BLD_DIR}/src//array/libarray_lib.a
  ${SCIDB_BLD_DIR}/src/smgr/io/libio_lib.a
  ${SCIDB_BLD_DIR}/src/system/catalog/libcatalog_lib.a

)

##  ${SCIDB_BLD_DIR}/src/smgr/io/lib_lib.a)

find_library(LOG4CXXLIB log4cxx)
find_library(BOOST_PROGOPTS boost_program_options)

# test program
add_executable(testHdf testHdf.cc H5Array.cc arrayCommon.cc)
target_link_libraries(testHdf ${HDF5_LIBRARIES})


#include(FeatureSummary)
#print_enabled_features()
# ZLIB_LIBRARIES m 
