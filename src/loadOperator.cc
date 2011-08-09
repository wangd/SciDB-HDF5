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

namespace {
    class Copier {
    public:
        Copier(scidb::ArrayID& arrayId) : _array(arrayId) {
            // Nothing for now.
        }

        void copyChunk(H5Array::SlabIter& si,
                       int attNo) {
            // Not sure about coordinate order. HDF: minor precedes major.
            scidb::Coordinates const& coords = *si; 
            boost::shared_ptr<scidb::ArrayIterator> ai;
            ai = _array.getIterator(attNo);
            scidb::Chunk& outChunk = ai->newChunk(coords);

            outChunk.allocate(si.slabAttrSize(attNo));
            outChunk.setSparse(false); // Never sparse
            // scidb::Chunk* outChunkPtr = &outChunk;
            // std::cout << "writing to buffer at " 
            //           << (void*) outChunk.getData() << std::endl;
            si.readInto(attNo, outChunk.getData());
            outChunk.setCount(0); // FIXME: Count = num of elements.
            outChunk.write();
        }
    private:
        scidb::DBArray _array;
    };
}


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
    Copier copier(aid);

    std::cout << "Added array to catalog and contructed dbarray." << std::endl;
    
    // Only handle single-attribute right now.
    int chunkMode = 0; // chunk mode (dense/sparse)
    chunkMode |=  scidb::ChunkIterator::NO_EMPTY_CHECK;
    std::cout << "Iterating... " << std::endl;
    std::cout << "begin: " << ha.begin() << std::endl;
    std::cout << "end: " << ha.end() << std::endl;
    for(H5Array::SlabIter i = ha.begin();
        i != ha.end(); ++i) {
        std::cout << i << std::endl;
        copier.copyChunk(i, 0);



    }
    // Fill results

    //res.setString("SomeArray"); // Fill in result: name of new array
}
