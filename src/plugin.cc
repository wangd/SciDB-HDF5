//   This file is part of the SciDB-HDF5 project.
//   Copyright 2011  Jacek Becla, Daniel Liwei Wang
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//   
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// std
#include <vector>

// scidb
#include "SciDBAPI.h"
#include "query/Operator.h"
#include "query/TypeSystem.h"
//#include "system/ErrorCodes.h"

// hdf5 array
#include "H5Array.hh"
// pkg
#include "loadOperator.hh"

namespace {
    std::string extractPhysicalParam(scidb::OperatorParam& p) {
        typedef scidb::OperatorParamPhysicalExpression OpPhyExpr;
        using scidb::Value;
        
        Value v = dynamic_cast<OpPhyExpr&>(p).getExpression()->evaluate();
        return v.getString();
    }
    std::string extractLogicalParam(scidb::OperatorParam& p,
                                    boost::shared_ptr<scidb::Query> q) {
        typedef scidb::OperatorParamLogicalExpression OpLogExpr;
        using scidb::Value;
        
        Value v = 
            evaluate(dynamic_cast<OpLogExpr&>(p).getExpression(),
                     q,
                     scidb::TID_STRING);
        return v.getString();
    }
} // anonymous namespace    

////////////////////////////////////////////////////////////////////////
// Boilerplate
////////////////////////////////////////////////////////////////////////
// Currently this loader loads into a MemArray and relies on the optimizer
// to redistribute the resulting data.  We could instead make the loader
// a delegate array using special iterators that loads data lazily.

// Logical
class LogicalLoadHdf : public scidb::LogicalOperator {
public:
    LogicalLoadHdf(const std::string& logicalName, const std::string& alias)
        : scidb::LogicalOperator(logicalName, alias) {
        ADD_PARAM_CONSTANT("string"); // Hdf file name
        ADD_PARAM_CONSTANT("string"); // Path to array in file.
        // No initialization needed.
    }

    virtual scidb::ArrayDesc inferSchema(std::vector<scidb::ArrayDesc> s, 
                                         boost::shared_ptr<scidb::Query> q) {
        if (s.size() != 0) {
            throw SYSTEM_EXCEPTION(
                     scidb::SCIDB_SE_INFER_SCHEMA, 
                     scidb::SCIDB_LE_ARRAY_ALREADY_EXIST) << "input array";
        }
        if (_parameters.size() != 2) {
            throw SYSTEM_EXCEPTION(
                     scidb::SCIDB_SE_INFER_SCHEMA, 
                     scidb::SCIDB_LE_WRONG_OPERATOR_ARGUMENTS_COUNT) << 
                "loadhdf" << 2 << _parameters.size();
        }

        std::string filePath = extractLogicalParam(*_parameters[0], q);
        std::string hdf5Path = extractLogicalParam(*_parameters[1], q);
        H5Array ha(filePath, hdf5Path);
        boost::shared_ptr<scidb::ArrayDesc> arrayDesc = ha.arrayDesc();

        return *arrayDesc;
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
    
    bool changesDistribution(vector<scidb::ArrayDesc> const& inputSchemas) const
    {
       return true;
    }

    scidb::ArrayDistribution getOutputDistribution(vector<scidb::ArrayDistribution> const& inputDistributions,
                                                   vector<scidb::ArrayDesc> const& inputSchemas) const
    {
        return scidb::ArrayDistribution(scidb::psLocalInstance);
    }

    boost::shared_ptr<scidb::Array> execute(ArrayVector& inputArrays, 
                                            boost::shared_ptr<scidb::Query> q) {
        boost::shared_ptr<scidb::Array> retVal;
        assert(inputArrays.size() == 0);

        // this is a coordinator-only load
        if (q->isCoordinator())
        {
            retVal = 
                loadHdf(
                    extractPhysicalParam(*_parameters[0]), // path to h5 file
                    extractPhysicalParam(*_parameters[1]), // path to array data set
                    q // "query"
                    );
        }
        else
        {
            retVal.reset(new scidb::MemArray(_schema, q));
        }

        return retVal;
    }
};

namespace scidb {
REGISTER_PHYSICAL_OPERATOR_FACTORY(PhysicalLoadHdf, 
                                   "loadHdf", "physicalLoadHdf");
}



// Physical
