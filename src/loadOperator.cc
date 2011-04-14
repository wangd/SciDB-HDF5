#include "loadOperator.hh"

// std
#include <string>
// scidb
#include "query/TypeSystem.h"
// pkg
#include "H5Array.hh"


void loadHdf(const scidb::Value* args, scidb::Value& res, void*) {
    std::string filePath = args[0].getString();
    std::string hdfPath = args[1].getString();
    std::string arrayName = args[2].getString();
    H5Array ha(filePath, hdfPath);
    
    // Do something good.
    res.setString("SomeArray"); // Fill in result: name of new array
}
