#define BOOST_TEST_MODULE testHdf
#include "boost/test/included/unit_test.hpp"
#include "H5Array.hh"
#include "arrayCommon.hh"
#include <algorithm>
#include <iostream>

struct HdfFixture {
    HdfFixture() {}
    ~HdfFixture() {}
};
char fName[] = "sxrcom10-r0232.h5";
char path[] = "/Configure:0000/Run:0000/CalibCycle:0000/Camera::FrameV1/SxrBeamline.0:Opal1000.1/image";


BOOST_FIXTURE_TEST_SUITE(Hdf, HdfFixture)
BOOST_AUTO_TEST_CASE(testH5Array) {
    H5Array h(fName, path);
}

BOOST_AUTO_TEST_CASE(checkDesc) {
    H5Array h(fName, path);
    SalVectorPtr sal = h.getScidbAttrs();
    std::cout << "Checking descriptor for file:" << fName << " on path "
              << path << std::endl;
    std::copy(sal->begin(), sal->end(), 
              std::ostream_iterator<ScidbAttrLite>(std::cout, " "));
    SdlVectorPtr sdl = h.getScidbDims();
    std::copy(sdl->begin(), sdl->end(), 
              std::ostream_iterator<ScidbDimLite>(std::cout, " "));
    std::cout << "done." << std::endl;
    
}

BOOST_AUTO_TEST_CASE(checkSlabIter) {
    H5Array h(fName, path);
    std::cout << "Iterating... " << fName << " --> " << path << std::endl;
    std::cout << "begin: " << h.begin() << std::endl;
    std::cout << "end: " << h.end() << std::endl;
    int count =0;
    boost::shared_array<char> buffer;
    for(H5Array::SlabIter i = h.begin(); i != h.end(); ++i) {
        ++count;
        if(count < 80) std::cout << i << std::endl;
        else if (count == 80) std::cout << "Suppressing..." << std:: endl;

        
        if(!buffer) {
            uint64_t bufSize = i.byteSize();
            std::cout << "Allocating buffer of size " << bufSize << std::endl;
            buffer.reset(new char[bufSize]);
        }
        i.readInto(buffer.get());
    }
}


BOOST_AUTO_TEST_SUITE_END()
