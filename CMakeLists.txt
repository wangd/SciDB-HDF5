########################################################################
## SciDB-HDF5 
########################################################################
cmake_minimum_required (VERSION 2.6)
project (SciDB-HDF5)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/Modules/")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pedantic  -Wall -Wno-long-long -fPIC -Wno-unused-parameter")

add_subdirectory("src")


