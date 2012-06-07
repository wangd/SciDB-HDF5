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
#include "loadUtils.hh"

// scidb
#include "query/TypeSystem.h"
#include "system/SystemCatalog.h"
#include "array/Array.h"
#include "array/DBArray.h"

scidb::ArrayID scidbCreateArray(std::string const& arrayName, 
                                scidb::ArrayDesc& aDesc) {

    scidb::SystemCatalog& catalog = *scidb::SystemCatalog::getInstance();
    if(catalog.containsArray(arrayName)) { // delete if existing.
        catalog.deleteArray(arrayName);
    }
    aDesc.setName(arrayName);
    return catalog.addArray(aDesc, scidb::psLocalInstance); 
}

ScidbArrayCopier::ScidbArrayCopier(scidb::ArrayID& arrayId, 
                                   int attrCount,
                                   boost::shared_ptr<scidb::Query>& q)
    : _array(new scidb::DBArray(arrayId, q)), 
      _query(q),
      _attrCount(attrCount)
{
    // Nothing for now.
}

void ScidbArrayCopier::copyChunks(Source& source) {
    for(int i=0; i < _attrCount; ++i) {
        copyChunk(i, source);
    }
}

void ScidbArrayCopier::copyChunk(int attNo, Source& source) {
    scidb::Coordinates const& coords = source.coords(); 
    boost::shared_ptr<scidb::ArrayIterator> ai;
    ai = _array->getIterator(attNo);
    scidb::Chunk& outChunk = ai->newChunk(coords);
    //std::cout << "chunk footprint=" << source.footprint(attNo) << std::endl;
    outChunk.allocate(source.footprint(attNo));
    outChunk.setSparse(false); // Never sparse
    source.copy(attNo, outChunk.getData());
    outChunk.setCount(source.elementCount(attNo, true));
    outChunk.write(_query);
}

