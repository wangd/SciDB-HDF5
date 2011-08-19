# -- Check for the HDF5 library --
#
# Uses path overrides: H5INC_PATH and H5LIB_PATH 
#
# Defines:
#
#  CFITSIO_FOUND - system has the CFITSIO library
#  CFITSIO_INCLUDE_DIR - the CFITSIO include directory
#  CFITSIO_LIBRARY - The library needed to use CFITSIO
#
#  CCFITS_FOUND
#  CCFITS_INCLUDE
#  CCFITS_LIBRARY
#
# Affected by environment variables:
#  CFITSIOINC_PATH
#  CFITSIOLIB_PATH
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
find_path(CFITSIO_INCLUDE_DIR  fitsio.h PATHS $ENV{CFITSIOINC_PATH})
find_library(CFITSIO_LIBRARY  cfitsio  PATHS $ENV{CFITSIOLIB_PATH})

find_path(CCFITS_INCLUDE_DIR  CCfits/CCfits PATHS $ENV{CFITSIOINC_PATH})
find_library(CCFITS_LIBRARY  CCfits PATHS $ENV{CFITSIOLIB_PATH})

find_package_handle_standard_args(CFITSIO  DEFAULT_MSG  
  CFITSIO_INCLUDE_DIR CFITSIO_LIBRARY)
find_package_handle_standard_args(CCFITS  DEFAULT_MSG  
  CCFITS_INCLUDE_DIR CCFITS_LIBRARY)
#find_package_handle_standard_args(LIBSZ  DEFAULT_MSG  SZ_LIB)

mark_as_advanced(CFITSIO_LIBRARY CFITSIO_INCLUDE_DIR
  CCFITS_LIBRARY CCFITS_INCLUDE_DIR)

