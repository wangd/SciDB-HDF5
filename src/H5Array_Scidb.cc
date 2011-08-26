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
    convertInto(sAtts, *h.getScidbAttrs());
    convertInto(sDims, *h.getScidbDims());
    return boost::make_shared<scidb::ArrayDesc>(name,sAtts,sDims); 
}
