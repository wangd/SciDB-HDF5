#include "loadOperator.hh"

// std
#include <string>

// pkg
#include "FitsArray.hh"
#include "loadUtils.hh"

namespace {
}


void loadFits(std::string const& filePath, 
              int hduNumber,
              std::string const& arrayName) {
    std::string resultName = arrayName;

    // Do something good.
    FitsArray fa(filePath, hduNumber);
    scidb::ArrayID aid = scidbCreateArray(arrayName, *fa.arrayDesc());

#if 0
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
    #endif
}
