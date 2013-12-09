//   This file is part of the SciDB-HDF5 project.
//   Copyright 2011  Jacek Becla, Daniel Liwei Wang
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//   
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
#include "H5Array.hh"
#include <boost/make_shared.hpp>

// A stub file with non scidb-dependent implementations where possible.

boost::shared_ptr<scidb::ArrayDesc> 
H5Array::ScidbIface::getArrayDesc(H5Array const& h) {
    return boost::shared_ptr<scidb::ArrayDesc>();
}

void 
H5Array::ScidbIface::readValueIntoChunk(scidb::ChunkIterator& ci,
                                        char* src,
                                        size_t size) {
}
