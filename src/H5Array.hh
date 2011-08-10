#ifndef LOADER_H5ARRAY_HH
#define LOADER_H5ARRAY_HH
#include <string>
#include <boost/shared_ptr.hpp>
#include <boost/shared_array.hpp>
#include "arrayCommonFwd.hh"
// Forward
namespace scidb {
    class ArrayDesc; 
}


////////////////////////////////////////////////////////////////////////
// class H5Array
////////////////////////////////////////////////////////////////////////
// Notes on H5Array:
// Example A:  1024x1024x77777.
// Chunk length: size of the chunk in a dimension.  e.g., 256x256x1 or
// 1024x1024x1 for Example A.
// 
// Incrementing for iterating through chunks: Generally, advance first
// dimension by chunk size in that dimension until rollover/carry to
// next dimension. 
// e.g. for length 256x256x1 -> (0,0,0), (256,0,0), (512,0,0),
// (768,0,0), (0,256,0), (256,256,0),... 
// for length 512x1024x1 -> (0,0,0), (512,0,0), (0,0,1), (512,0,1), ...
//
// An iterator should maintain a position in real space, and increment
// by chunk sizes.
//
////////////////////////////////////////////////////////////////////////
class H5Array {
public:
    typedef std::vector<int64_t> Coordinates;
    typedef int64_t Size;

    class SlabIter {
    public:
        SlabIter& operator++(); // Increment 
        Coordinates const& operator*() const; // De-reference
        bool operator==(SlabIter const& rhs) const; // Equality 
        bool operator!=(SlabIter const& rhs) const; // In-equality 
        Size slabSize() const { return _slabSize; }
        Size slabAttrSize(int attNo) const;
        void* readInto(int attNo, void* buffer);
        void* readSlabInto(void* buffer);
        Size getCount(int attNo, bool clipEdges) const;
        friend std::ostream& operator<<(std::ostream& os, SlabIter const& i);

    private:
        friend class H5Array;
        SlabIter(H5Array const& ha, bool makeEnd=false);
        void _initSlabCache();
        void* _readAttrInto(void* buffer, void* slabBuffer, int attNo);
        Size _computeSlabSize() const;

        H5Array const& _ha;
        Coordinates _coords;
        Size _slabSize;
        bool _cacheValid;
        boost::shared_array<char> _slabCache;
        class Cursor;
        boost::shared_ptr<Cursor> _cursor;

    };
    class ScidbIface {
    public:
        static boost::shared_ptr<scidb::ArrayDesc> getArrayDesc(H5Array const& h);
    };

    friend class SlabIter;
    class DataSet; // not part of interface.

    H5Array(std::string const& fPath, std::string const& path);

    boost::shared_ptr<scidb::ArrayDesc> getArrayDesc() const;
    int getSlabCount() const;
    int getRank() const;
    SalVectorPtr getScidbAttrs() const;
    SdlVectorPtr getScidbDims() const;
    int getAttrCount() const;

    SlabIter begin() { return SlabIter(*this); }
    SlabIter end() { return SlabIter(*this, true); }

private:
    void _imposeChunking(SdlVectorPtr sdl) const;
    std::string const _filePath;
    std::string const _path;
    boost::shared_ptr<DataSet> _ds;
    Coordinates _chunkIncr;
    
};

inline H5Array::Coordinates const& H5Array::SlabIter::operator*() const { 
    return _coords; 
}

inline bool H5Array::SlabIter::operator==(H5Array::SlabIter const& rhs) const {
    for(unsigned i=0; i < _coords.size(); ++i)
        if(_coords[i] != rhs._coords[i]) return false;
    return true;
}

inline bool H5Array::SlabIter::operator!=(H5Array::SlabIter const& rhs) const {
    return !((*this) == rhs);
}

#endif // LOADER_H5ARRAY_HH
