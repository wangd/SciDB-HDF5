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

BOOST_AUTO_TEST_CASE(testDumpArray) {
    const int eltCount=100;
    const int maxEltSize=8;
    std::cout << "touched?" << std::endl;
    void* buffer = malloc(eltCount * maxEltSize * sizeof(char));
    BOOST_ASSERT(buffer);
    FitsArray::dbgDumpArray(defaultFitsFile, 1, 100, buffer);
    std::copy((float*)buffer, static_cast<float*>(buffer)+100, 
              std::ostream_iterator<float>(std::cout, ", "));
    std::cout << std::endl;
    FitsArray::dbgDumpArray(defaultFitsFile, 2, 100, buffer);
    std::copy((uint16_t*)buffer, static_cast<uint16_t*>(buffer)+100, 
              std::ostream_iterator<uint16_t>(std::cout, ", "));
    std::cout << std::endl;
    FitsArray::dbgDumpArray(defaultFitsFile, 3, 100, buffer);
    std::copy((float*)buffer, static_cast<float*>(buffer)+100, 
              std::ostream_iterator<float>(std::cout, ", "));

}



BOOST_AUTO_TEST_SUITE_END()
