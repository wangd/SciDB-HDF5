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
#ifndef LOADER_HH
#define LOADER_HH

#include "H5Cpp.h"
#include <vector>
#include <ostream>

class Loader {
public:
    typedef H5::ArrayType ArrayType;
    typedef H5::CompType CompType;
    typedef H5::DataSet DataSet;
    typedef H5::DataSpace DataSpace;
    typedef H5::DataType DataType;
    typedef H5::EnumType EnumType;
    typedef H5::PredType PredType;
    typedef H5::VarLenType VarLenType;
    
    explicit Loader(std::string const& filename);
    void doOneGroup(const std::string& objName, 
                    H5G_obj_t objType,
                    const std::string& prefix);

    // keep public for now for debugging
    void processDataSet(const std::string& dataSetName);

private:
    void addAttrIntType(const DataType& dt, const H5std_string& mName);
    void addAttrFloatType(const DataType& dt, const H5std_string& mName);
    void addAttrStringType(const DataType& dt, const H5std_string& mName);
    void addAttrEnumType(const DataType& dt, const H5std_string& mName);

    void flattenArray(ArrayType& arrayType, const std::string& mName);
    void processCompoundType(const CompType& ct, const H5std_string& parentMName);
    void processArrayType(ArrayType& arrayType);
    static std::string convertDataSetNameToArrayName(const std::string& dataSetName);
    static std::string print_H5T_class_t_Name(H5T_class_t t);
    static std::string printPredType(const DataType& dt);

    std::string attributesToString() const;
    std::string dimensionsToString() const;

    void dumpSchema(const std::string& dataSetName);
    void dumpData(const std::string& dataSetName, DataSet& dataSet, 
                  H5T_class_t typeClass, hsize_t curDim);
    void dumpData_1dArray_compound(const std::string& dataSetName, 
                                   DataSet& dataSet, 
                                   H5T_class_t typeClass, hsize_t curDim);
    void dumpData_mdArray_nonCompound(const std::string& dataSetName,
                                      DataSet& dataSet, 
                                      H5T_class_t typeClass, hsize_t curDim);
    
private:
    class OneAttr {
    public:
        OneAttr() : tS(0), inNA("") {}

        H5T_class_t type;
        size_t tS; // size of this type, e.g. 4 for INT
        std::string attrName;
        const PredType* predType;
        H5T_str_t strPad;   // for STRING only
        H5T_cset_t strCset; // for STRING only
        htri_t strIsVlstr;  // for STRING only
        std::vector< std::pair<std::string, long> > enumMembers; // for ENUM only
        std::string tN; // sciDbTypeName;
        std::string inNA; // "" or name of nested array this elem belongs to 
    };

    class OneDim {
    public:
        OneDim(int _d1, int _d2, int _n) : d1(_d1), d2(_d2), curNElems(_n) {}
        
        int d1; // start (typically 0)
        int d2; // end or UNLIMITED
        int curNElems; // current dimensionality (number of elements)
        static const int UNLIMITED;
        static const int UNKNOWN;
    };

    std::string const& _filename;
    H5::H5File _h5File;
    std::vector< OneAttr > _attr;  // attributes
    std::vector< OneDim > _dim;    // dimensions
    
    friend std::ostream& operator<<(std::ostream&, const Loader::OneAttr&);
    friend std::ostream& operator<<(std::ostream&, const Loader::OneDim&);
};

#endif // LOADER_HH
