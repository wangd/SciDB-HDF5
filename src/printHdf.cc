#include "loader.hh"

int main (void) {
    std::string const filename("sxrcom10-r0232.h5");
    try {
        //Exception::dontPrint();
        std::string prefix = "";

        Loader loader(filename);
        //loader.doOneGroup("/", H5G_GROUP, prefix, file);
        loader.doOneGroup("/Configure:0000/Run:0000/CalibCycle:0000/Camera::FrameV1/SxrBeamline.0:Opal1000.1", H5G_GROUP, prefix);
        // FIXME: dumping data for flattened arrays
        //loader.processDataSet("/Configure:0000/Epics::EpicsPv/EpicsArch.0:NoDevice.0/SXR:SPS:MPA:01:IN/data");

        // FIXME: multi-d arrays
        //loader.processDataSet("/Configure:0000/Run:0000/CalibCycle:0000/Camera::FrameV1/SxrBeamline.0:Opal1000.1/image");

        // DATASET "image" {
        //    DATATYPE  H5T_ARRAY { [1024][1024] H5T_STD_U16LE }
        //    DATASPACE  SIMPLE { ( 42247 ) / ( H5S_UNLIMITED ) }
        // }
        
    } catch( H5::FileIException error ) {
        error.printError();
        return -1;
    } catch( H5::DataSetIException error ) {
        error.printError();
        return -1;
    } catch( H5::DataSpaceIException error ) {
        error.printError();
        return -1;
    } catch( H5::DataTypeIException error ) {
        error.printError();
        return -1;
    }
    return 0;
}
