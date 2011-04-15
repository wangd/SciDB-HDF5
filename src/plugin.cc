// std
#include <vector>

// scidb
#include "SciDBAPI.h"
#include "query/Operator.h"
//#include "system/ErrorCodes.h"

// pkg
#include "loadOperator.hh"

namespace {
    std::string extractParam(scidb::OperatorParam& p) {
        typedef scidb::OperatorParamPhysicalExpression OpExpr;
        using scidb::Value;
        
        Value v = dynamic_cast<OpExpr&>(p).getExpression()->evaluate();
        return v.getString();
    }
} // anonymous namespace    

////////////////////////////////////////////////////////////////////////
// Boilerplate
////////////////////////////////////////////////////////////////////////
EXPORTED_FUNCTION void GetPluginVersion(uint32_t& major, uint32_t& minor, 
                                        uint32_t& patch, uint32_t& build)
{
    // Provide correct values here. SciDB check it and does not allow to load too new plugins.
    major = 1;
    minor = 0;
    patch = 0;
    build = 0;
}

// FIXME: This could be a pipelined loader that doesn't just load into an array...

// Logical
class LogicalLoadHdf : public scidb::LogicalOperator {
public:
    LogicalLoadHdf(const std::string& logicalName, const std::string& alias)
        : scidb::LogicalOperator(logicalName, alias) {
        ADD_PARAM_CONSTANT("string"); // New array name
        ADD_PARAM_CONSTANT("string"); // Hdf file name
        ADD_PARAM_CONSTANT("string"); // Path to array in file.
        // No initialization needed.
    }

    virtual scidb::ArrayDesc inferSchema(std::vector<scidb::ArrayDesc> s, 
                                         boost::shared_ptr<scidb::Query> q) {
        typedef boost::shared_ptr<scidb::OperatorParamReference> OpParamRefP;
        using scidb::AttributeDesc;
        using scidb::AttributeID;
        using scidb::DimensionDesc;
        using scidb::TID_STRING;
        USER_CHECK(scidb::SCIDB_E_INVALID_OPERAND, s.size() == 0, 
                   "No input array is expected");
        return scidb::ArrayDesc();

        //Not sure what to return right now.
        scidb::Attributes attrs(2);
        attrs[0] = AttributeDesc((AttributeID)0, "hdffile",  TID_STRING, 0, 0 );
        attrs[1] = AttributeDesc((AttributeID)0, "hdfpath",  TID_STRING, 0, 0 );
        scidb::Dimensions dims(1);
        dims[0] = DimensionDesc("i", 0, 0, 0, 0, 2, 0);
        return scidb::ArrayDesc("loadhdf", attrs, dims);

    }
};
namespace scidb {
REGISTER_LOGICAL_OPERATOR_FACTORY(LogicalLoadHdf, "loadhdf");
}

class PhysicalLoadHdf: public scidb::PhysicalOperator {
public:
    typedef boost::shared_ptr<scidb::Array> ArrayPtr;
    typedef std::vector<ArrayPtr> ArrayVector;

    PhysicalLoadHdf(const string& logicalName,
                    const string& physicalName,
                    const Parameters& parameters,
                    const scidb::ArrayDesc& schema)
        : scidb::PhysicalOperator(logicalName, 
                                  physicalName, 
                                  parameters, 
                                  schema) {
    }
    
    boost::shared_ptr<scidb::Array> execute(ArrayVector& inputArrays, 
                                            boost::shared_ptr<scidb::Query> ) {
        assert(inputArrays.size() == 0);
        loadHdf(extractParam(*_parameters[0]), 
                extractParam(*_parameters[1]), 
                extractParam(*_parameters[2]));
        // Not sure what to return as an array right now.
        return ArrayPtr();
    }
};

namespace scidb {
REGISTER_PHYSICAL_OPERATOR_FACTORY(PhysicalLoadHdf, 
                                   "loadHdf", "physicalLoadHdf");
}



// Physical
