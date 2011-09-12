# CMakeLists.txt for src/ of SciDB-HDF5 
#
#
#   Copyright 2011  Jacek Becla, Daniel Liwei Wang
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#   
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

find_package(ZLIB REQUIRED)
# Point at non-system locations via H5INC_PATH and H5LIB_PATH
find_package(Hdf5  REQUIRED) 
find_package(Boost REQUIRED)
find_package(Scidb REQUIRED)

include_directories(${HDF5_INCLUDE_DIR} ${SCIDB_INCLUDES} ${Boost_INCLUDE_DIRS})

# SciDB special flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${SCIDB_CXX_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -rdynamic")


# file dumper
add_executable(printHdf HdfDump.cc printHdf.cc)
target_link_libraries(printHdf ${HDF5_LIBRARIES})

# SciDb plugin
set(plugin_src plugin.cc loadOperator.cc H5Array.cc H5Array_Scidb.cc
  loadUtils.cc arrayCommon.cc)
add_library(loadhdf SHARED ${plugin_src})
target_link_libraries(loadhdf ${HDF5_LIBRARIES})

# FITS plugin
find_package(Cfitsio)
if(CCFITS_FOUND)
  message("FITS loader enabled")
  set(fitsplugin_src fitsPlugin.cc fitsOperator.cc FitsArray.cc 
    FitsArray_Scidb.cc loadUtils.cc arrayCommon.cc)
  add_library(loadfits SHARED ${fitsplugin_src})

  target_link_libraries(loadfits ${CCFITS_LIBRARY})
else(CCFITS_FOUND)
  message("Disabling FITS loader")
endif(CCFITS_FOUND)


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
add_executable(testHdf testHdf.cc H5Array.cc arrayCommon.cc H5Array_stub.cc)
target_link_libraries(testHdf ${HDF5_LIBRARIES})

## FITS
include_directories(${CCFITS_INCLUDE} )
add_executable(testFits testFits.cc FitsArray.cc)
target_link_libraries(testFits ${CCFITS_LIBRARY} ${CFITSIO_LIBRARY})


#include(FeatureSummary)
#print_enabled_features()
# ZLIB_LIBRARIES m 
