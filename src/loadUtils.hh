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
#ifndef LOADER_LOADUTILS_HH
#define LOADER_LOADUTILS_HH

#include <string>
#include <boost/shared_ptr.hpp>

#include "array/Metadata.h" // Scidb


// Forward
namespace scidb {
    class ArrayDesc;
    class DBArray;
}
// Free functions
scidb::ArrayID scidbCreateArray(std::string const& arrayName, 
                                scidb::ArrayDesc& aDesc);
// Copier class
class ScidbArrayCopier {
public:

    class Source {
    public:
        typedef int64_t Size;
        virtual ~Source() {}
        virtual scidb::Coordinates const& coords() const = 0;
        virtual Size footprint(int attNo) const = 0; 
        virtual Size elementCount(int attNo, bool clip=false) const = 0;
        virtual void copy(int attNo, void* target) = 0;
    };


    ScidbArrayCopier(scidb::ArrayID& arrayId, int attrCount);

    void copyChunks(Source& target);
    void copyChunk(int attNo, Source& target);

private:
    boost::shared_ptr<scidb::DBArray> _array;
    int _attrCount;
};

#endif // LOADER_LOADUTILS_HH
