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
