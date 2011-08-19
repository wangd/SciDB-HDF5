// std
#include <vector>

// scidb
#include "SciDBAPI.h"
#include "query/Operator.h"
//#include "system/ErrorCodes.h"

// pkg
#include "fitsOperator.hh"

namespace {
    scidb::Value extractValue(scidb::OperatorParam& p) {
        typedef scidb::OperatorParamPhysicalExpression OpExpr;
        return dynamic_cast<OpExpr&>(p).getExpression()->evaluate();
    }
    int extractInt(scidb::OperatorParam& p) {
        return extractValue(p).getInt32();
    }
    std::string extractString(scidb::OperatorParam& p) {
        return extractValue(p).getString();
    }
} // anonymous namespace    

////////////////////////////////////////////////////////////////////////
// Boilerplate
////////////////////////////////////////////////////////////////////////
// FIXME: This could be a pipelined loader that doesn't just load into an array...

// Logical
class LogicalLoadFits : public scidb::LogicalOperator {
public:
    LogicalLoadFits(const std::string& logicalName, const std::string& alias)
        : scidb::LogicalOperator(logicalName, alias) {
        ADD_PARAM_CONSTANT("string"); // FITS file
        ADD_PARAM_CONSTANT("int32"); // HDU number (1=first image)
        ADD_PARAM_CONSTANT("string"); // Target array
        // No initialization needed.
    }
    virtual ~LogicalLoadFits() {}

    virtual scidb::ArrayDesc inferSchema(std::vector<scidb::ArrayDesc> s, 
                                         boost::shared_ptr<scidb::Query> q) {
        typedef boost::shared_ptr<scidb::OperatorParamReference> OpParamRefP;
        using scidb::AttributeDesc;
        using scidb::AttributeID;
        using scidb::DimensionDesc;
        using scidb::TID_INT32;
        using scidb::TID_STRING;
        USER_CHECK(scidb::SCIDB_E_INVALID_OPERAND, s.size() == 0, 
                   "No input array is expected");
        return scidb::ArrayDesc();

        //Not sure what to return right now.
        scidb::Attributes attrs(2);
        attrs[0] = AttributeDesc((AttributeID)0, "fitsfile",  TID_STRING, 0, 0 );
        attrs[1] = AttributeDesc((AttributeID)0, "hdu",  TID_INT32, 0, 0 );
        scidb::Dimensions dims(1);
        dims[0] = DimensionDesc("i", 0, 0, 0, 0, 2, 0);
        return scidb::ArrayDesc("loadfits", attrs, dims);

    }
};
namespace scidb {
REGISTER_LOGICAL_OPERATOR_FACTORY(LogicalLoadFits, "loadfits");
}

class PhysicalLoadFits: public scidb::PhysicalOperator {
public:
    typedef boost::shared_ptr<scidb::Array> ArrayPtr;
    typedef std::vector<ArrayPtr> ArrayVector;

    PhysicalLoadFits(const string& logicalName,
                    const string& physicalName,
                    const Parameters& parameters,
                    const scidb::ArrayDesc& schema)
        : scidb::PhysicalOperator(logicalName, 
                                  physicalName, 
                                  parameters, 
                                  schema) {
    }

    virtual ~PhysicalLoadFits() {}    

    boost::shared_ptr<scidb::Array> execute(ArrayVector& inputArrays, 
                                            boost::shared_ptr<scidb::Query> ) {
        assert(inputArrays.size() == 0);
        loadFits( extractString(*_parameters[1]), // path to FITS file
                  extractInt(*_parameters[2]), // HDU number
                  extractString(*_parameters[0]) // array name
                  );
        // Not sure what to return as an array right now.
        return ArrayPtr();
    }
};

namespace scidb {
REGISTER_PHYSICAL_OPERATOR_FACTORY(PhysicalLoadFits, 
                                   "loadFits", "physicalLoadFits");
}



// Physical
