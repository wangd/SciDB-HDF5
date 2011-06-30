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
#include "scidbConvert.hh"


void loadHdf(std::string const& filePath, 
             std::string const& hdfPath, 
             std::string const& arrayName) {

    // Do something good.
    H5Array ha(filePath, hdfPath);

    std::cout << "Retrieving descriptor for " << filePath << " --> " 
              << hdfPath << std::endl;
    boost::shared_ptr<scidb::ArrayDesc> dptr(ha.getArrayDesc());
    dptr->setName(arrayName);
    std::cout << "Set array name. Getting catalog instance." << std::endl;
    scidb::SystemCatalog& catalog = *scidb::SystemCatalog::getInstance();

    if(catalog.containsArray(arrayName)) { // delete if existing.
        catalog.deleteArray(arrayName);
    }

    // Get array id; hardcode partitioning scheme for now.
    scidb::ArrayID aid = catalog.addArray(*dptr, scidb::psLocalNode); 
    scidb::DBArray array(aid);
    std::cout << "Added array to catalog and contructed dbarray." << std::endl;
    


    // Only handle single-attribute right now.
    int chunkMode = 0; // chunk mode (dense/sparse)
    chunkMode |=  scidb::ChunkIterator::NO_EMPTY_CHECK;
    boost::shared_ptr<scidb::ArrayIterator> ai = array.getIterator(0);
    //ArrayDescPtr ap = newArrayDesc(ha.getScidbAttrs(), ha.getScidbDims());
    std::cout << "Iterating... " << std::endl;
    std::cout << "begin: " << ha.begin() << std::endl;
    std::cout << "end: " << ha.end() << std::endl;
    for(H5Array::SlabIter i = ha.begin();
        i != ha.end(); ++i) {
        // FIXME: need to fix chunkPos and coord: what do they need to be?
        std::cout << i << std::endl;
        //ci = ai->newChunk(*i).getIterator(chunkMode);
        scidb::Chunk& outChunk = ai->newChunk(*i);
        outChunk.allocate(i.byteSize());
        outChunk.setSparse(false); // Never sparse
        memcpy(outChunk.getData(), i.data(), i.byteSize());
        outChunk.setCount(0);
        outChunk.write();
    }
    // Fill results
    //res.setString("SomeArray"); // Fill in result: name of new array
}
