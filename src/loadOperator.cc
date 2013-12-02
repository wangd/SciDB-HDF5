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
#include "loadOperator.hh"

// std
#include <string>
// scidb
#include "query/TypeSystem.h"
#include "system/SystemCatalog.h"
#include "array/Array.h"
#include "array/DBArray.h"
// pkg
#include "H5Array.hh"
#include "loadUtils.hh"

namespace {
    class H5SlabSource : public ScidbArrayCopier::Source {
    public:
        H5SlabSource(H5Array::SlabIter& si) : _si(si) {}
        virtual scidb::Coordinates const& coords() const {
            return *_si;
        }
        virtual Size footprint(int attNo) const { 
            return _si.slabAttrSize(attNo);
        }
        virtual Size elementCount(int attNo, bool clip) const { 
            return _si.elementCount(attNo, clip);
        }
        virtual void copy(int attNo, void* target) {
            _si.readInto(attNo, target);
        }
    private:
        H5Array::SlabIter& _si;
    };

}


void loadHdf(std::string const& filePath, 
             std::string const& hdfPath, 
             std::string const& arrayName,
             boost::shared_ptr<scidb::Query>& q) {
    std::string resultName = arrayName;

    H5Array ha(filePath, hdfPath);

    std::cout << "Retrieving descriptor for " << filePath << " --> " 
              << hdfPath << std::endl;

    boost::shared_ptr<scidb::ArrayDesc> arrayDesc = ha.arrayDesc();
    scidbCreateArray(arrayName, *arrayDesc);
    ScidbArrayCopier copier(*arrayDesc, ha.attrCount(), q);
    
    std::cout << "Added array to catalog and contructed dbarray." 
              << std::endl; 
    
    std::cout << "Iterating... " << std::endl;
    std::cout << "begin: " << ha.begin() << std::endl;
    std::cout << "end: " << ha.end() << std::endl;
    for(H5Array::SlabIter i = ha.begin();
        i != ha.end(); ++i) {
        std::cout << i << std::endl;
        H5SlabSource t(i);
        copier.copyChunks(t);
    }
    
    // Fill results

    // FIXME: want to propagate some form of OK/FAIL result.
    //res.setString(resultName); // Fill in result: name of new array
}
