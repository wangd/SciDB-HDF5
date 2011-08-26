#ifndef LOADER_FITSARRAY_HH
#define LOADER_FITSARRAY_HH

#include <CCfits/CCfits> 
#include <boost/shared_ptr.hpp>

// Forward:
namespace scidb {
    class ArrayDesc;
}

class FitsAttr {
public:
    FitsAttr(int bitPix_, double scale_, double zero_);
    int bitPix;
    int byteSize;
    double scale;
    double zero;
    bool floating;
    bool hasSign;
};

typedef uint32_t FitsDim;

class FitsArray {
public:
    typedef std::vector<FitsDim> DimVector;
    typedef size_t Size;

    // fitsFile: path to FITS file
    // hduNum: 0 = primary HDU, 1+: extension HDU #
    FitsArray(std::string const& fName, int hduNum);

    Size elementCount() const;
    Size footprint() const;

    boost::shared_ptr<scidb::ArrayDesc> arrayDesc() const;


    static void dbgCheckHeader(std::string const& fName);
    static void dbgCheckArrays(std::string const& fName);
    static void dbgDumpArray(std::string const& fName,
                             int hduNum, int numElems, void* buffer);
    
private:
    void _build(CCfits::HDU& hdu); // non-const: reading is non-const.
        
    //private:
    public:
    boost::shared_ptr<CCfits::FITS> _fits;
    boost::shared_ptr<FitsAttr> _attr;
    boost::shared_ptr<DimVector> _dims;
    int _pCount;
    int _gCount;
    std::string _fName;
};

#endif // LOADER_FITSARRAY_HH
