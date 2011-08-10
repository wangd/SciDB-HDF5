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
        Copier(scidb::ArrayID& arrayId, int attrCount)
            : _array(arrayId), _attrCount(attrCount) {
            // Nothing for now.
        }

        void copyChunks(H5Array::SlabIter& si) {
            for(int i=0; i < _attrCount; ++i) {
                copyChunk(si, i);
            }
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
            outChunk.setCount(si.getCount(attNo));
            outChunk.write();
        }
    private:
        scidb::DBArray _array;
        int _attrCount;
    };
}


void loadHdf(std::string const& filePath, 
             std::string const& hdfPath, 
             std::string const& arrayName) {
    std::string resultName = arrayName;

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
    Copier copier(aid, ha.getAttrCount());
    
    std::cout << "Added array to catalog and contructed dbarray." 
              << std::endl; 
    
    std::cout << "Iterating... " << std::endl;
    std::cout << "begin: " << ha.begin() << std::endl;
    std::cout << "end: " << ha.end() << std::endl;
    for(H5Array::SlabIter i = ha.begin();
        i != ha.end(); ++i) {
        std::cout << i << std::endl;
        copier.copyChunks(i);
    }
    
    // Fill results

    // FIXME: want to propagate some form of OK/FAIL result.
    //res.setString(resultName); // Fill in result: name of new array
}
