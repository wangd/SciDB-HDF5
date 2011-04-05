find_package(Hdf5  REQUIRED)

include_directories(${HDF5_INCLUDE_DIR})
add_executable(loader loader.cc)
target_link_libraries(loader ${HDF5_LIBRARIES})
