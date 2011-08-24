#include "FitsArray.hh"
#include <cmath>
////////////////////////////////////////////////////////////////////////
// Local helpers
////////////////////////////////////////////////////////////////////////
namespace {
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
FitsArray::FitsArray(std::string const& fName, int hduNum) {
    std::auto_ptr<CCfits::FITS> infile(new CCfits::FITS(fName, 
                                                        CCfits::Read));
    assert(infile.get());
    if(hduNum == 0) {
        _build(infile->pHDU());
    } else {
        _build(infile->extension(hduNum));
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
