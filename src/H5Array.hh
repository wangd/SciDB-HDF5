#ifndef LOADER_H5ARRAY_HH
#define LOADER_H5ARRAY_HH
#include <string>
#include <boost/shared_ptr.hpp>
#include "arrayCommonFwd.hh"
// Forward
namespace scidb {
    class ArrayDesc; 
}


////////////////////////////////////////////////////////////////////////
// class H5Array
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
        Size byteSize() const;
        char* data();
        friend std::ostream& operator<<(std::ostream& os, SlabIter const& i);

    private:
        friend class H5Array;
        SlabIter(H5Array const& ha, bool makeEnd=false);
        
        H5Array const& _ha;
        Coordinates _coords;
    };
    class ScidbIface {
    public:
        static boost::shared_ptr<scidb::ArrayDesc> getArrayDesc(H5Array const& h);
    };

    friend class SlabIter;
    class DataSet; // not part of interface.

    H5Array(std::string const& fPath, std::string const& path);

    boost::shared_ptr<scidb::ArrayDesc> getArrayDesc() const;
    int getSlabCount() const { return 1; } // FIXME 
    int getRank() const { return 3; } // FIXME 
    SalVectorPtr getScidbAttrs() const;
    SdlVectorPtr getScidbDims() const;
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
