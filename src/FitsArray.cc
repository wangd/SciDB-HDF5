#include "FitsArray.hh"
#include <cmath>
#include <CCfits/FITSUtil.h>
////////////////////////////////////////////////////////////////////////
// Local helpers
////////////////////////////////////////////////////////////////////////
namespace {
#if 0
    // @return the CFITSIO datatype for the given attribute.
    // Attribute is one of: TBYTE, TSBYTE, TSHORT, TUSHORT, TINT,
    // TUINT, TLONG, TLONGLONG, TULONG, TFLOAT, TDOUBLE  
    int fitsDatatype(FitsAttr const& a) {
        if(a.floating) {
            switch(a.bitPix) {
            case -32:
                return TFLOAT;
            case -64:
                return TDOUBLE;
            default:
                return TDOUBLE; // Don't know what else to do.
            }
        } else {
            switch(a.byteSize) {
            case 1: 
                if(hasSign) return TSBYTE;
                else return TBYTE;
            case 2: // ???
                if(hasSign) return TSHORT;
                else return TUSHORT;
            case 4: // ???
                if(hasSign) return TINT;
                else return TUINT;
            case 8:
                return TLONGLONG;
                
            }
        }
    }
#endif

    template<typename T>
    void* dumpArray(CCfits::HDU& hdu, 
                    FitsArray::Size numElems, void* buffer) {
        std::valarray<T> contents;
        if(hdu.index() == 0) { // Check index for safe cast
            CCfits::PHDU* phdu = dynamic_cast<CCfits::PHDU*>(&hdu);
            assert(phdu);
            phdu->read(contents, 1, numElems);
        } else { 
            CCfits::ExtHDU* ehdu = dynamic_cast<CCfits::ExtHDU*>(&hdu);
            assert(ehdu);
            ehdu->read(contents, 1, numElems); 
        }
        // FIXME: Is there a better way to extract from a valarray?
        return memcpy(buffer, &contents[0], contents.size() * sizeof(T));
    }
    
}

////////////////////////////////////////////////////////////////////////
// FitsAttr public:
////////////////////////////////////////////////////////////////////////
FitsAttr::FitsAttr(int bitPix_, double scale_, double zero_) 
    : bitPix(bitPix_), scale(scale_), zero(zero_) {
    int neededBitPix = std::labs(bitPix);
    assert(bitPix != 0);
    if(scale > 1) {
        double multiplier = scale;
        while(multiplier > 1) {
            multiplier /= std::pow(2, static_cast<double>(neededBitPix));
            neededBitPix = (neededBitPix << 1);
        }
    }
    byteSize = (neededBitPix >> 3); 
    if(bitPix > 0) {
        hasSign = (zero != (1 <<(bitPix-1)));
    } else {
        hasSign = true;
        floating = true;
    }

}


////////////////////////////////////////////////////////////////////////
// FitsArray public:
////////////////////////////////////////////////////////////////////////
FitsArray::FitsArray(std::string const& fName, int hduNum) 
    :_hduNum(hduNum) {
    _fits.reset(new CCfits::FITS(fName, CCfits::Read));
    assert(_fits.get());
    if(hduNum == 0) {
        _build(_fits->pHDU());
    } else {
        _build(_fits->extension(hduNum));
    }        
}

int FitsArray::rank() const {
    return _dims->size();
}

FitsArray::Size FitsArray::elementCount() const {
    return std::accumulate(_dims->begin(), _dims->end(), 
                           1, std::multiplies<Size>());
}

FitsArray::Size FitsArray::footprint() const {
    return _attr->byteSize * _gCount * (elementCount() + _pCount);
}

void FitsArray::copyArray(void* buffer, int64_t numElems) {
    CCfits::HDU* hdu;
    if(_hduNum) hdu = &_fits->extension(_hduNum);
    else hdu = &_fits->pHDU();

    FitsAttr const& attr = *_attr;
    switch(attr.byteSize) {
    case 1:
        assert(!attr.floating); // no 8-bit floats
        if(attr.hasSign) dumpArray<char>(*hdu, numElems, buffer);
        else dumpArray<unsigned char>(*hdu, numElems, buffer);
        break;
    case 2:
        assert(!attr.floating); // no 16-bit floats
        if(attr.hasSign) dumpArray<int16_t>(*hdu, numElems, buffer);
        else dumpArray<uint16_t>(*hdu, numElems, buffer);
        break;
    case 4:
        if(attr.floating) dumpArray<float>(*hdu, numElems, buffer);
        else if(attr.hasSign) dumpArray<int32_t>(*hdu, numElems, buffer);
        else dumpArray<uint32_t>(*hdu, numElems, buffer);
        break;
    case 8:
        if(attr.floating) dumpArray<double>(*hdu, numElems, buffer);
        else if(attr.hasSign) dumpArray<int64_t>(*hdu, numElems, buffer);
        else dumpArray<uint64_t>(*hdu, numElems, buffer);
        break;
        // FIXME: No support for long double (128bit/16-byte floating)
    default:
        bool valid_bytes_per_size = false;
        assert(valid_bytes_per_size); // error.
        break;
    }    
}


////////////////////////////////////////////////////////////////////////
// public debugging
////////////////////////////////////////////////////////////////////////
void FitsArray::dbgCheckHeader(std::string const& fName) {
    // Construct FITS ref object, read-only
    std::auto_ptr<CCfits::FITS> pInfile(new CCfits::FITS(fName, 
                                                         CCfits::Read));
    assert(pInfile.get());
    std::cout << *pInfile << std::endl;
        
    pInfile->pHDU().readAllKeys(); // Load header
    std::cout << pInfile->pHDU() << std::endl; // Print header        
}

void FitsArray::dbgCheckArrays(std::string const& fName) {
    try {
        const int maxHdu = 10; // read first 10 HDUs (pri + 9 ext)
        for(int i=0; i < maxHdu; ++i) {
            FitsArray fa(fName, i);
            assert(fa._attr.get());
            FitsAttr& far = *fa._attr;

            std::cout << "HDU (" << i << ") ";
            if(fa._dims->size() < 1) {
                std::cout << "(no array)" << std::endl;
                continue; // No data array-->skip.
            }

            if(far.floating) {
                std::cout << far.byteSize << "-byte float";
            } else {
                std::cout << far.byteSize << "-byte ";
                if(!far.hasSign) {
                    std::cout << "u";
                }
                std::cout << "int";
            }
            std::cout << "(scale=" << fa._attr->scale << ")";
            std::cout << " array in dims: ";
            std::copy(fa._dims->begin(), fa._dims->end(), 
                      std::ostream_iterator<FitsDim>(std::cout, ", "));
            std::cout << std::endl;
        } // each hdu
    } catch(CCfits::FitsException& fe) {
        // Assume we hit the end of the extensions.
    }        
}

void FitsArray::dbgDumpArray(std::string const& fName,
                             int hduNum, int numElems, void* buffer) {
    FitsArray fa(fName, hduNum);
    fa.copyArray(buffer, numElems);
}


////////////////////////////////////////////////////////////////////////
// private:
////////////////////////////////////////////////////////////////////////

// Fill out att, dims, pcount, gcount
void FitsArray::_build(CCfits::HDU& hdu) {
    int axes = hdu.axes();
    _attr.reset(new FitsAttr(hdu.bitpix(), hdu.scale(), hdu.zero()));
    _dims.reset(new DimVector(axes));
    for(int i=0; i < axes; ++i) {
        (*_dims)[i] = hdu.axis(i);
    }
    hdu.readAllKeys();
    if(hdu.index() > 0) {
        hdu.readKey("PCOUNT", _pCount);
        hdu.readKey("GCOUNT", _gCount);
        // std::cout << "pcount:" << _pCount << " gcount:" 
        //           << _gCount << std::endl;
    }
}
