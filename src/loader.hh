
#ifndef LOADER_HH
#define LOADER_HH

#include "H5Cpp.h"
#include <vector>
#include <ostream>

#ifndef H5_NO_NAMESPACE
using namespace H5;
#endif

class Loader {
public:
    Loader();
    void doOneGroup(const std::string&, 
                    H5G_obj_t,
                    const std::string&,
                    H5File);

    // keep public for now for debugging
    void processDataSet(const std::string &);

private:
    void addAttrIntType(const DataType&, const H5std_string&);
    void addAttrFloatType(const DataType&, const H5std_string&);
    void addAttrStringType(const DataType&, const H5std_string&);
    void addAttrEnumType(const DataType&, const H5std_string&);

    void flattenArray(ArrayType&, const std::string&);
    void processCompoundType(const CompType&, const H5std_string&);
    void processArrayType(ArrayType&);
    static std::string covertDataSetNameToArrayName(const std::string &);
    static std::string print_H5T_class_t_Name(H5T_class_t t);
    static std::string printPredType(const DataType&);

    std::string attributesToString() const;
    std::string dimensionsToString() const;

    void dumpSchema(const std::string &);
    void dumpData(const std::string &, DataSet &, H5T_class_t, hsize_t);
    void dumpData_1dArray_compound(const std::string &, DataSet &, 
                                   H5T_class_t, hsize_t);
    void dumpData_mdArray_nonCompound(const std::string &, DataSet &, 
                                      H5T_class_t, hsize_t);
    
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

    std::vector< OneAttr > _attr;  // attributes
    std::vector< OneDim > _dim;    // dimensions
    
    friend std::ostream& operator<<(std::ostream&, const Loader::OneAttr&);
    friend std::ostream& operator<<(std::ostream&, const Loader::OneDim&);
};

#endif
