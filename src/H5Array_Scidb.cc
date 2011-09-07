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
#include <algorithm>
// Scidb
#include "array/Metadata.h"
// Package
#include "arrayCommon.hh"

namespace {
    class dimConvert {
    public:
        scidb::DimensionDesc operator()(ScidbDimLite const& dim) {
            return scidb::DimensionDesc(dim.name, dim.min, dim.start, 
                                        dim.end, dim.max,
                                        dim.chunkInterval, dim.chunkOverlap, 
                                        dim.typeId, dim.arrayName);
        }
    };
    
    class attConvert {
    public:
        scidb::AttributeDesc operator()(ScidbAttrLite const& att) {
            return scidb::AttributeDesc(0, // Blank attribute id.
                                        att.name, att.typeId,
                                        0, // Flags
                                        0 // default compression method
                                        );
        }
    };
    
    void convertInto(scidb::Attributes& sAtts, SalVector const& atts) {
        sAtts.clear();
        sAtts.reserve(atts.size());
        std::transform(atts.begin(), atts.end(), 
                       std::back_inserter(sAtts), attConvert());
    }

    void convertInto(scidb::Dimensions& sDims, SdlVector const& dims) {
        sDims.clear();
        sDims.reserve(dims.size());
        std::transform(dims.begin(), dims.end(), 
                       std::back_inserter(sDims), dimConvert());
        // std::copy(dims.begin(), dims.end(), 
        //           std::ostream_iterator<ScidbDimLite>(std::cout, "\n"));
    }
}
boost::shared_ptr<scidb::ArrayDesc> 
H5Array::ScidbIface::getArrayDesc(H5Array const& h) {
    std::string name("Unknown");
    scidb::Attributes sAtts;
    scidb::Dimensions sDims;
    convertInto(sAtts, *h.scidbAttrs());
    convertInto(sDims, *h.scidbDims());
    return boost::make_shared<scidb::ArrayDesc>(name,sAtts,sDims); 
}
