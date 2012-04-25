# -- Check for the SciDB source tree --
#
# Required: SciDB source checkout. Specify using SCIDB_ROOT_DIR
#
# Defines:
#
#  SCIDB_FOUND - system has a SciDB source checkout
#  SCIDB_INCLUDE_DIR - the SciDB include directory
#  SCIDB_SRC_DIR - SciDB src dir
#  SCIDB_LIBRARIES
#  SCIDB_CXX_FLAGS - SciDB flags needed, e.g., PROJECT_ROOT
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
include(FindPackageHandleStandardArgs)

# Look for includes and libraries
find_path(SCIDB_INCLUDE_ARRAY  array/Array.h 
  HINTS ${SCIDB_ROOT_DIR}
  PATH_SUFFIXES include src
  DOC "Path to include directory for array/Array.h in Scidb source tree")

find_path(SCIDB_SRC_DIR array/Metadata.cpp 
  HINTS ${SCIDB_ROOT_DIR}
  PATH_SUFFIXES src)

find_package_handle_standard_args(SCIDB DEFAULT_MSG 
  SCIDB_INCLUDE_ARRAY 
  SCIDB_SRC_DIR)

if(SCIDB_FOUND)
  set(SCIDB_INCLUDES ${SCIDB_INCLUDE_ARRAY}) 
  set(SCIDB_CXX_FLAGS "-DPROJECT_ROOT=\\\"${SCIDB_ROOT_DIR}/\\\"")

endif(SCIDB_FOUND)
mark_as_advanced(Scidb_FOUND SCIDB_INCLUDES SCIDB_SRC_DIR SCIDB_CXX_FLAGS)



