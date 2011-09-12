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
#ifndef LOADER_FITSARRAY_HH
#define LOADER_FITSARRAY_HH

#include <CCfits/CCfits> 
#include <boost/shared_ptr.hpp>

// Forward:
namespace scidb {
    class ArrayDesc;
}

class FitsAttr {
public:
    FitsAttr(int bitPix_, double scale_, double zero_);
    int bitPix;
    int byteSize;
    double scale;
    double zero;
    bool floating;
    bool hasSign;
};

typedef uint32_t FitsDim;

////////////////////////////////////////////////////////////////////////
// FitsArray - abstraction for reading an array out of a FITS file
// Unlike H5Array, there is no chunking involved.  FITS files do not
// have a provision for chunking, and FitsArray does not impose any
// chunking scheme.  FITS arrays are generally small enough to fit
// in-memory, probably to accomodate older FITS software, so the lack
// of chunking should not be a problem to FitsArray clients.
////////////////////////////////////////////////////////////////////////
class FitsArray {
public:
    typedef std::vector<FitsDim> DimVector;
    typedef size_t Size;

    // fitsFile: path to FITS file
    // hduNum: 0 = primary HDU, 1+: extension HDU #
    FitsArray(std::string const& fName, int hduNum);

    int rank() const;
    Size elementCount() const;
    Size footprint() const;
    void copyArray(void* buffer, int64_t numElems);

    boost::shared_ptr<scidb::ArrayDesc> arrayDesc() const;


    static void dbgCheckHeader(std::string const& fName);
    static void dbgCheckArrays(std::string const& fName);
    static void dbgDumpArray(std::string const& fName,
                             int hduNum, int numElems, void* buffer);
    
private:
    void _build(CCfits::HDU& hdu); // non-const: reading is non-const.
        
    //private:
    public:
    int _hduNum;
    boost::shared_ptr<CCfits::FITS> _fits;
    boost::shared_ptr<FitsAttr> _attr;
    boost::shared_ptr<DimVector> _dims;
    int _pCount;
    int _gCount;
    std::string _fName;
};

#endif // LOADER_FITSARRAY_HH
