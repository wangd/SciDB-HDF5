#include "FitsArray.hh"

////////////////////////////////////////////////////////////////////////
// Local helpers
////////////////////////////////////////////////////////////////////////
namespace {
    void readDesc(CCfits::ExtHDU& eh) {
        int bitpix = eh.bitpix();
        double scale = eh.scale();
        double zero = eh.zero();

        std::cout << "bitpixel:" << bitpix;
        assert(bitpix != 0);
        if(bitpix > 0) {
            std::cout << " (int" << bitpix;
            if(zero == (1 <<(bitpix-1))) {
                std::cout << " unsigned)";
            } else {
                std::cout << ")";
            }
            
        } else { // bitpix < 0: float
            switch(bitpix) {
                case -64:
                    std::cout << " (double)";
                    break;
                case -32:
                    std::cout << " (float)";
                    break;
                default:
                    std::cout << " (unsup.float)";
                    break;
                }
        }
        std::cout << " scale: " << scale
                  << " zeroOff: " << zero;

        std::cout << " axes: " << eh.axes();
        for(int i=0; i < eh.axes(); ++i) {
            std::cout << " axis(" << i << "): " << eh.axis(i);
        }
        std::cout << std::endl;
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
            if(far.floating) {
                std::cout << far.byteSize << "-byte float";
            } else {
                std::cout << far.byteSize << "-byte ";
                if(!far.hasSign) {
                    std::cout << "u";
                }
                std::cout << "int";
            }
            // FIXME: print dims
        } // each hdu
    } catch(CCfits::FitsException& fe) {
        // Assume we hit the end of the extensions.
    }        
}

////////////////////////////////////////////////////////////////////////
// private:
////////////////////////////////////////////////////////////////////////

// Fill out att, dims, pcount, gcount
void FitsArray::_build(CCfits::HDU const& hdu) {
    int axes = hdu.axes();
    _attr.reset(new FitsAttr(hdu.bitpix(), hdu.scale(), hdu.zero()));
    _dims.reset(new DimVector(axes));
    for(int i=0; i < axes; ++i) {
        (*_dims)[i] = hdu.axis(i);
    }
    // set pcount, gcount;
}
