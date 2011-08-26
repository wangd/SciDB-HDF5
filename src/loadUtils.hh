#ifndef LOADER_LOADUTILS_HH
#define LOADER_LOADUTILS_HH

#include <string>
#include <boost/shared_ptr.hpp>

#include "array/Metadata.h" // Scidb


// Forward
namespace scidb {
    class ArrayDesc;

}

scidb::ArrayID scidbCreateArray(std::string const& arrayName, 
                                scidb::ArrayDesc& aDesc);

#endif // LOADER_LOADUTILS_HH
