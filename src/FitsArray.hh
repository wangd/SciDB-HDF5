#ifndef LOADER_FITSARRAY_HH
#define LOADER_FITSARRAY_HH

#include <CCfits/CCfits> 
#include <boost/shared_ptr.hpp>

class FitsAttr {
public:
    FitsAttr(int bitPix, double scale_, double zero_) 
        : scale(scale_), zero(zero_) {
        assert(bitPix != 0);
        if(bitPix > 0) {
            byteSize = bitPix / 8;
            hasSign = (zero != (1 <<(bitPix-1)));
        } else {
            byteSize = -bitPix / 8;
            hasSign = true;
            floating = true;
        }
    }
    int byteSize;
    double scale;
    double zero;
    bool floating;
    bool hasSign;
};

typedef uint32_t FitsDim;

class FitsArray {
public:
    // fitsFile: path to FITS file
    // hduNum: 0 = primary HDU, 1+: extension HDU #
    FitsArray(std::string const& fName, int hduNum) {
        std::auto_ptr<CCfits::FITS> infile(new CCfits::FITS(fName, 
                                                            CCfits::Read));
        assert(infile.get());
        if(hduNum == 0) {
            _build(infile->pHDU());
        } else {
            _build(infile->extension(hduNum));
        }        
    }
    private:
        void _build(CCfits::HDU const& hdu) {
        }
    //private:
    public:
    boost::shared_ptr<FitsAttr> attr;
    boost::shared_ptr<std::vector<FitsDim> > dims;
    int pCount;
    int gCount;
};

#endif // LOADER_FITSARRAY_HH
