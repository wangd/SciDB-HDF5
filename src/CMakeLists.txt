#find_package(ZLIB REQUIRED)
find_package(Hdf5  REQUIRED)

include_directories(loader ${HDF5_INCLUDE_DIR})
add_executable(loader loader.cc)
target_link_libraries(loader ${HDF5_LIBRARIES})

include(FeatureSummary)
print_enabled_features()
# ZLIB_LIBRARIES m 