#define BOOST_TEST_MODULE testFits
#include "boost/test/included/unit_test.hpp"
//#include <algorithm>
//#include <iostream>


#include <CCfits/CCfits> // Temporary
#include "FitsArray.hh"
using CCfits::FITS;
using CCfits::Read;
using CCfits::ExtHDU;
using CCfits::PHDU;


struct FitsFixture {

    FitsFixture() :defaultFitsFile("S11.fits") {}
    ~FitsFixture() {}


    void checkHeader(std::string const& fName) {        
        // Construct FITS ref object, read-only
        std::auto_ptr<FITS> pInfile(new FITS(fName, Read));
        assert(pInfile.get());
        std::cout << *pInfile << std::endl;
        
        pInfile->pHDU().readAllKeys(); // Load header
        std::cout << pInfile->pHDU() << std::endl; // Print header        
    }

    void checkArrays(std::string const& fName) {
        // Build FITS ref object, read-only
        std::auto_ptr<FITS> pInfile(new FITS(fName, Read)); 
        assert(pInfile.get());

        pInfile->pHDU().readAllKeys(); // Load header
        std::cout << pInfile->pHDU() << std::endl; // Print header
        try {
            for(int i=1; i < 10; ++i) {
                CCfits::ExtHDU& eh = pInfile->extension(i);
                eh.readAllKeys();
                std::cout << "extHDU(" << i << ") with " << eh.axes()
                          << " axes" << std::endl;
                readDesc(eh);
            }
        } catch(CCfits::FitsException& fe) {
            // Assume we hit the end of the extensions.
        }
        
    }

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

    

    std::string const defaultFitsFile;
};

BOOST_FIXTURE_TEST_SUITE(Fits, FitsFixture)
BOOST_AUTO_TEST_CASE(testFitsArray) {
    //FitsArray fa;
    checkHeader(defaultFitsFile);
    checkArrays(defaultFitsFile);
}



BOOST_AUTO_TEST_SUITE_END()
