find_package(ZLIB REQUIRED)
# Point at non-system locations via H5INC_PATH and H5LIB_PATH
find_package(Hdf5  REQUIRED) 
find_package(Boost REQUIRED )
find_package(Scidb REQUIRED)

include_directories(${HDF5_INCLUDE_DIR} ${SCIDB_INCLUDE_DIR} ${Boost_INCLUDE_DIRS})
add_executable(loader loader.cc printHdf.cc)
target_link_libraries(loader ${HDF5_LIBRARIES})

set(plugin_src plugin.cc loadOperator.cc H5Array.cc)
add_library(loadhdf SHARED ${plugin_src})


#include(FeatureSummary)
#print_enabled_features()
# ZLIB_LIBRARIES m 
