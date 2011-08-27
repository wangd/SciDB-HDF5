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
    return catalog.addArray(aDesc, scidb::psLocalNode); 
}

ScidbArrayCopier::ScidbArrayCopier(scidb::ArrayID& arrayId, int attrCount)
    : _array(new scidb::DBArray(arrayId)), _attrCount(attrCount) {
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
    outChunk.allocate(source.footprint(attNo));
    outChunk.setSparse(false); // Never sparse
    source.copy(attNo, outChunk.getData());
    outChunk.setCount(source.elementCount(attNo, true));
    outChunk.write();
}

