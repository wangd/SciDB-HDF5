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


void loadHdf(const scidb::Value* args, scidb::Value& res, void*) {
    // Extract arguments
    std::string filePath = args[0].getString();
    std::string hdfPath = args[1].getString();
    std::string arrayName = args[2].getString();


    // Do something good.
    H5Array ha(filePath, hdfPath);

    boost::shared_ptr<scidb::ArrayDesc> dptr(ha.getArrayDesc());
    dptr->setName(arrayName);
    scidb::SystemCatalog& catalog = *scidb::SystemCatalog::getInstance();

    // Get array id; hardcode partitioning scheme for now.
    scidb::ArrayID aid = catalog.addArray(*dptr, scidb::psLocalNode); 
    scidb::DBArray array(aid);

    // Only handle single-attribute right now.
    int chunkMode = 0; // chunk mode (dense/sparse)
    chunkMode |=  scidb::ChunkIterator::NO_EMPTY_CHECK;
    boost::shared_ptr<scidb::ArrayIterator> ai = array.getIterator(0);
    boost::shared_ptr<scidb::ChunkIterator> ci; 
    int numChunks = ha.getSlabCount(); // FIXME
    int rank = ha.getRank(); // FIXME
    scidb::Coordinates chunkPos(rank);   
    scidb::Coordinates coord(rank);
    for(int i=0; i < numChunks; ++i) {
        // FIXME: need to fix chunkPos and coord: what do they need to be?
        chunkPos = coord;
        ci = ai->newChunk(chunkPos).getIterator(chunkMode);
    }

    // Fill results
    res.setString("SomeArray"); // Fill in result: name of new array
}
