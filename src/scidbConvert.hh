#ifndef LOADER_SCIDBCONVERT_HH
#define LOADER_SCIDBCONVERT_HH

#include "arrayCommonFwd.hh"
namespace scidb {
    class ArrayDesc;
}
typedef boost::shared_ptr<scidb::ArrayDesc> ArrayDescPtr;

ArrayDescPtr newArrayDesc(SalVectorPtr attrs, SdlVectorPtr dims);

#endif // LOADER_SCIDBCONVERT_HH
