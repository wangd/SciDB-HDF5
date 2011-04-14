#include <vector>
#include "loadOperator.hh"
#include "SciDBAPI.h"
#include "query/FunctionDescription.h"

namespace scidb {
    // Forward
    class BaseLogicalOperatorFactory;
    class BasePhysicalOperatorFactory;
}

////////////////////////////////////////////////////////////////////////
// Boilerplate
////////////////////////////////////////////////////////////////////////
std::vector<scidb::BaseLogicalOperatorFactory*> _logicalOperatorFactories;
EXPORTED_FUNCTION const std::vector<scidb::BaseLogicalOperatorFactory*>& GetLogicalOperatorFactories() {
    return _logicalOperatorFactories;
}

std::vector<scidb::BasePhysicalOperatorFactory*> _physicalOperatorFactories;
EXPORTED_FUNCTION const std::vector<scidb::BasePhysicalOperatorFactory*>& GetPhysicalOperatorFactories() {
    return _physicalOperatorFactories;
}

std::vector<scidb::Type> _types;
EXPORTED_FUNCTION const std::vector<scidb::Type>& GetTypes() {
    return _types;
}

std::vector<scidb::FunctionDescription> _functionDescs;
EXPORTED_FUNCTION const std::vector<scidb::FunctionDescription>& GetFunctions() {
    return _functionDescs;
}

/**
 * Class for registering/unregistering user defined objects
 */
class Instance {
public:
    Instance() {
        using scidb::TypeId;
        using scidb::TID_STRING;
        _functionDescs.push_back(
            scidb::FunctionDescription("loadhdf", 
                                       scidb::argTypes(TypeId(TID_STRING),
                                                       TypeId(TID_STRING)), 
                                       TypeId(TID_STRING), &loadHdf));
    }
} _instance;
