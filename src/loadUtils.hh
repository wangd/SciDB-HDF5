#ifndef LOADER_LOADUTILS_HH
#define LOADER_LOADUTILS_HH

#include <string>
#include <boost/shared_ptr.hpp>

#include "array/Metadata.h" // Scidb


// Forward
namespace scidb {
    class ArrayDesc;
    class DBArray;
}
// Free functions
scidb::ArrayID scidbCreateArray(std::string const& arrayName, 
                                scidb::ArrayDesc& aDesc);
// Copier class
class ScidbArrayCopier {
public:

    class Source {
    public:
        typedef int64_t Size;
        virtual ~Source() {}
        virtual scidb::Coordinates const& coords() const = 0;
        virtual Size footprint(int attNo) const = 0; 
        virtual Size elementCount(int attNo, bool clip=false) const = 0;
        virtual void copy(int attNo, void* target) = 0;
    };


    ScidbArrayCopier(scidb::ArrayID& arrayId, int attrCount);

    void copyChunks(Source& target);
    void copyChunk(int attNo, Source& target);

private:
    boost::shared_ptr<scidb::DBArray> _array;
    int _attrCount;
};

#endif // LOADER_LOADUTILS_HH
