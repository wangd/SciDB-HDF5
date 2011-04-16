#define BOOST_TEST_MODULE testHdf
#include "boost/test/included/unit_test.hpp"
#include "H5Array.hh"

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

BOOST_AUTO_TEST_SUITE_END()
