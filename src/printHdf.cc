//   This file is part of the SciDB-HDF5 project.
//   Copyright 2011  Jacek Becla, Daniel Liwei Wang
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//   
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
#include "HdfDump.hh"
#include <unistd.h>
#include <stdlib.h>

namespace {

class Options {
public:
    // This constructor mangles argv since getopt does it.
    explicit Options(int argc, char* argv[]) :
        _helpOpt(false),
        _quietOpt(false),
        _verboseOpt(false) {

        _import(argc, argv);
    }
    bool helpRequested() const { return _helpOpt;}    
    bool verbose() const { return _verboseOpt;}    
    bool quiet() const { return _quietOpt;}
    std::string const& hdfFile() const { return _hdfFile;}
    std::string const& hdfPath() const { return _hdfPath;}
    
private:
    // _import mangles argv because getopt does it.
    void _import(int argc, char* argv[]) {
        char opts[] = "hqvx:";
        int c;
        
        while ((c = getopt(argc, argv, opts)) 
               != -1) {
            switch (c) {
            case 'h':
                _helpOpt = true;
                break;
            case 'q':
                _quietOpt = true;
                _verboseOpt = false;
                break;
           case 'v':
               _verboseOpt = true;
               _quietOpt = false;
               break;
           case 'x':
               //x = optarg;
               break;
           default:
               ::abort();
           }
        } 
        int i = optind;
        if(i < argc) {
            _hdfFile = argv[i];
        }
        ++i;
        if(i < argc) {
            _hdfPath = argv[i];
        }
     }
    
    // fields:
    bool _helpOpt;
    bool _quietOpt;
    bool _verboseOpt;
    std::string _hdfFile;
    std::string _hdfPath;

};

void loadAndPrint(std::string const& filename, std::string const& path) {
    //Exception::dontPrint();
    std::string prefix = "";

    HdfDump hd(filename);
    //loader.doOneGroup("/", H5G_GROUP, prefix, file);
    hd.doOneGroup(path, H5G_GROUP, prefix);
    // FIXME: dumping data for flattened arrays
    //hd.processDataSet("/Configure:0000/Epics::EpicsPv/EpicsArch.0:NoDevice.0/SXR:SPS:MPA:01:IN/data");

    // FIXME: multi-d arrays
    //hd.processDataSet("/Configure:0000/Run:0000/CalibCycle:0000/Camera::FrameV1/SxrBeamline.0:Opal1000.1/image");

    // DATASET "image" {
    //    DATATYPE  H5T_ARRAY { [1024][1024] H5T_STD_U16LE }
    //    DATASPACE  SIMPLE { ( 42247 ) / ( H5S_UNLIMITED ) }
    // }
}


} // anonymous namespace

int main(int argc, char* argv[]) {
    std::string filename("sxrcom10-r0232.h5");
    std::string dpath("/Configure:0000/Run:0000/CalibCycle:0000/Camera::FrameV1/SxrBeamline.0:Opal1000.1");

    Options opts(argc, argv);
    if(!opts.hdfFile().empty()) {
        filename = opts.hdfFile(); 
    }
    if(!opts.hdfPath().empty()) {
        dpath = opts.hdfPath(); 
    }
    
    try {
        loadAndPrint(filename, dpath);
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
