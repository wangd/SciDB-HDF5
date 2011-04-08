find_package(ZLIB REQUIRED)
# Point at non-system locations via H5INC_PATH and H5LIB_PATH
find_package(Hdf5  REQUIRED) 
find_package(Hdf5  REQUIRED)
find_package(SciDB)

include_directories(${HDF5_INCLUDE_DIR})
add_executable(loader loader.cc printHdf.cc)
target_link_libraries(loader ${HDF5_LIBRARIES})

#include(FeatureSummary)
#print_enabled_features()
# ZLIB_LIBRARIES m 
