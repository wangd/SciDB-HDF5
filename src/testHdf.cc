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
char dpath[] = "/Configure:0000/Run:0000/CalibCycle:0000/Camera::FrameV1/SxrBeamline.0:Opal1000.1/data";
char tpath[] = "/Configure:0000/Run:0000/CalibCycle:0000/Camera::FrameV1/SxrBeamline.0:Opal1000.1/time";

BOOST_FIXTURE_TEST_SUITE(Hdf, HdfFixture)
BOOST_AUTO_TEST_CASE(testH5Array) {
    H5Array h(fName, path);
}

BOOST_AUTO_TEST_CASE(checkDesc) {
    H5Array h(fName, path);
    SalVectorPtr sal = h.scidbAttrs();
    std::cout << "Checking descriptor for file:" << fName << " on path "
              << path << std::endl;
    std::copy(sal->begin(), sal->end(), 
              std::ostream_iterator<ScidbAttrLite>(std::cout, " "));
    SdlVectorPtr sdl = h.scidbDims();
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
        if(count < 80) {
            std::cout << i << std::endl;
        } else if(count == 80) {
            std::cout << "Suppressing..." << std:: endl;
        } else if(count > 100) {
            std::cout << "Stopping after 100 slabs." << std::endl;
            break;
        }

        
        if(!buffer) {
            uint64_t bufSize = i.slabAttrSize(0);
            std::cout << "Allocating buffer of size " << bufSize << std::endl;
            buffer.reset(new char[bufSize]);
        }
        i.readInto(0, buffer.get());
    }
}

BOOST_AUTO_TEST_CASE(CheckCompoundRead) {
    H5Array h(fName, dpath);
    std::cout << "Iterating... " << fName << " --> " << dpath << std::endl;
    std::cout << "begin: " << h.begin() << std::endl;
    std::cout << "end: " << h.end() << std::endl;
    int count =0;
    boost::shared_array<char> buffer;
    uint64_t bufSize = 0;
    SalVectorPtr ap = h.scidbAttrs();
    int attCount = ap->size();
    for(H5Array::SlabIter slabi = h.begin(); slabi != h.end(); ++slabi) {
        ++count;
        if(count < 80) {
            std::cout << slabi << std::endl;
        } else if(count == 80) {
            std::cout << "Suppressing..." << std:: endl;
        } else if(count > 100) {
            std::cout << "Stopping after 100 slabs." << std::endl;
            break;
        }

        for(int attNo=0; attNo < attCount; ++attNo) {
            uint64_t sizeReq = slabi.slabAttrSize(attNo);
            std::cout << "(" << attNo << ")";
            if((!buffer) || sizeReq > bufSize) {
                bufSize = sizeReq;
                std::cout << "Allocating buffer of size " 
                          << bufSize << std::endl;
                buffer.reset(new char[bufSize]);
            }
            slabi.readInto(attNo, buffer.get());
            for(int i=0; i < 10; ++i) {
                std::cout << ((int*)buffer.get())[i] << " ";
            } 
            std::cout << " Count=" << slabi.elementCount(attNo, true)
                      << std::endl;
        }
    }
}



BOOST_AUTO_TEST_SUITE_END()
