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
#include "FitsArray.hh"
#include <boost/make_shared.hpp>
#include "array/Metadata.h"
#include "arrayCommon.hh"

namespace {
    ////////////////////////////////////////////////////////////////////
    // Conversion for dimensions
    ////////////////////////////////////////////////////////////////////
    class dimConvert {
    public:
        dimConvert() : dimNum(0) {}
        std::string nextName() {
            std::stringstream ss;
            ss << "dim" << dimNum;
            ++dimNum;
            return ss.str();
        }

        scidb::DimensionDesc operator()(FitsDim const& fd) {
            return scidb::DimensionDesc(nextName(), 
                                        0, // min
                                        0, // start
                                        fd, // end
                                        fd, // max
                                        fd, // chunkInterval
                                        0, // overlap
                                        SCIDB_TID_INT64, // dim type
                                        0,   // flags
                                        ""); //array name
        }
        int dimNum;
    };

    void convertInto(scidb::Dimensions& sDims, 
                     FitsArray::DimVector const& dims) {
        sDims.clear();
        sDims.reserve(dims.size());
        std::transform(dims.begin(), dims.end(), 
                           std::back_inserter(sDims), dimConvert());
    }
    ////////////////////////////////////////////////////////////////////
    // Conversion for attributes
    ////////////////////////////////////////////////////////////////////
    scidb::TypeId mapToScidb(FitsAttr const& attr) {
        switch(attr.byteSize) {
        case 1:
            assert(!attr.floating);
            if(attr.hasSign) return scidb::TID_INT8;
            else return scidb::TID_UINT8;
        case 2:
            assert(!attr.floating);
            if(attr.hasSign) return scidb::TID_INT16;
            else return scidb::TID_UINT16;
        case 4:
            if(attr.floating) return scidb::TID_FLOAT;
            else if(attr.hasSign) return scidb::TID_INT32;
            else return scidb::TID_UINT32;
        case 8:
            if(attr.floating) return scidb::TID_DOUBLE;
            else if(attr.hasSign) return scidb::TID_INT64;
            else return scidb::TID_UINT64;
        default:
            std::cerr << "FitsArray invalid type conversion." << std::endl;
            return scidb::TID_UINT64; // Is this the right thing?
        }
    }
}

boost::shared_ptr<scidb::ArrayDesc> FitsArray::arrayDesc() const {
    scidb::Attributes sAtts;
    scidb::Dimensions sDims;
    // Consider using something from the FITS metadata.
    std::string unknown("unknown"); 
    
    sAtts.push_back(scidb::AttributeDesc(0, // Blank attribute id.
                                         unknown, 
                                         mapToScidb(*_attr),
                                         0, // Flags
                                         0 // default compression method
                                         ));
    convertInto(sDims, *_dims);
    return boost::make_shared<scidb::ArrayDesc>(unknown,sAtts,sDims); 
}

