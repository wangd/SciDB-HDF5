#define BOOST_TEST_MODULE testFits
#include "boost/test/included/unit_test.hpp"
//#include <algorithm>
//#include <iostream>


#include <CCfits/CCfits> // Temporary
using CCfits::FITS;
using CCfits::Read;
using CCfits::ExtHDU;
using CCfits::PHDU;


struct FitsFixture {

    FitsFixture() :defaultFitsFile("S11.fits") {}

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
            }
        } catch(CCfits::FitsException& fe) {
            // Assume we hit the end of the extensions.
            std::cout << "exception " << fe.message() << std::endl;
        }
        
    }

    ~FitsFixture() {}

    std::string const defaultFitsFile;
};

BOOST_FIXTURE_TEST_SUITE(Fits, FitsFixture)
BOOST_AUTO_TEST_CASE(testFitsArray) {
    //FitsArray fa;
    checkHeader(defaultFitsFile);
    checkArrays(defaultFitsFile);
}



BOOST_AUTO_TEST_SUITE_END()
