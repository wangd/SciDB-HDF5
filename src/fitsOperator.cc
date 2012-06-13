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
// std
#include <string>

// pkg
#include "FitsArray.hh"
#include "loadUtils.hh"

namespace {
    class FitsSource : public ScidbArrayCopier::Source {
    public:
        FitsSource(FitsArray& fa) : _fa(fa), _coords(fa.rank(), 0) {
        }
        virtual scidb::Coordinates const& coords() const {
            return _coords;
        }
        virtual Size footprint(int attNo) const { 
            return _fa.footprint();
        }
        virtual Size elementCount(int attNo, bool clip) const { 
            return _fa.elementCount();
        }
        virtual void copy(int attNo, void* target) {
            _fa.copyArray(target, _fa.elementCount());
        }
    private:
        FitsArray& _fa;
        std::vector<int64_t> _coords;
    };
}


void loadFits(std::string const& filePath, 
              int hduNumber,
              std::string const& arrayName,
              boost::shared_ptr<scidb::Query>& q) {
    std::string resultName = arrayName;

    // Do something good.
    std::cout << "loadFits invoked with file=" << filePath
              << " hdu=" << hduNumber << " targetArray=" << arrayName 
              << std::endl;
    FitsArray fa(filePath, hduNumber);
    std::cout << "Constructed FitsArray(" << filePath << ", " 
              << hduNumber << ")\n";
    scidb::ArrayID aid = scidbCreateArray(arrayName, *fa.arrayDesc());
    std::cout << "Created array(" << arrayName << ")" << std::endl;

    ScidbArrayCopier copier(aid, 1, q);
    std::cout << "Prepared copier" << std::endl;
    FitsSource f(fa);
    std::cout << "Prepared fitsSource" << std::endl;

    copier.copyChunk(0, f);
    std::cout << "Copied chunk (" << f.elementCount(0, true) 
              << " elements)" << std::endl;
}
