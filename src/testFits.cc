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


    

    std::string const defaultFitsFile;
};

BOOST_FIXTURE_TEST_SUITE(Fits, FitsFixture)
BOOST_AUTO_TEST_CASE(testFitsArray) {
    //FitsArray fa;
    FitsArray::dbgCheckHeader(defaultFitsFile);
    FitsArray::dbgCheckArrays(defaultFitsFile);
}



BOOST_AUTO_TEST_SUITE_END()
