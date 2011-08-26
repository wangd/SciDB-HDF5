//
#include "FitsArray.hh"
#include <boost/make_shared.hpp>
#include "array/Metadata.h"
#include "arrayCommon.hh"

namespace {
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
    
}

boost::shared_ptr<scidb::ArrayDesc> FitsArray::arrayDesc() const {
    scidb::Attributes sAtts;
    scidb::Dimensions sDims;
    std::string unknown("unknown");
    // Need to map scidb typeid.
    sAtts.push_back(scidb::AttributeDesc(0, // Blank attribute id.
                                         unknown, 
                                         SCIDB_TID_INT64, // FIXME
                                         0, // Flags
                                         0 // default compression method
                                         ));
    convertInto(sDims, *_dims);
    return boost::make_shared<scidb::ArrayDesc>(unknown,sAtts,sDims); 


}
