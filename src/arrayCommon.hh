#ifndef LOADER_ARRAYCOMMON_HH
#define LOADER_ARRAYCOMMON_HH
#include <vector>
#include <boost/shared_ptr.hpp>
#include <H5Cpp.h>
////////////////////////////////////////////////////////////////////////
// class Attr
////////////////////////////////////////////////////////////////////////
class Attr {
public:
    Attr() : tS(0), inNA("") {}

    H5T_class_t type;
    size_t tS; // size of this type, e.g. 4 for INT
    bool lEnd; // true if little endian false for bigendian
    int sign; // 1 for signed, 0 for unsigned.
    std::string attrName;
    const H5::PredType* predType;
    H5T_str_t strPad;   // for STRING only
    H5T_cset_t strCset; // for STRING only
    htri_t strIsVlstr;  // for STRING only
    std::vector< std::pair<std::string, long> > enumMembers; // for ENUM only
    std::string tN; // sciDbTypeName;
    std::string inNA; // "" or name of nested array this elem belongs to 

    static Attr makeInteger(H5::DataType const& dt); 
    static Attr makeFloat(H5::DataType const& dt); 
    static Attr makeString(H5::DataType const& dt);
};
typedef std::vector<Attr> AttrVector;
typedef boost::shared_ptr<AttrVector> AttrVectorPtr;

////////////////////////////////////////////////////////////////////////
// class Dim
////////////////////////////////////////////////////////////////////////
class Dim {
public:
    Dim(int64_t _d1, int64_t _d2, int64_t _n) 
        : d1(_d1), d2(_d2), curNElems(_n) {}
        
    int64_t d1; // start (typically 0)
    int64_t d2; // end or UNLIMITED
    int64_t curNElems; // current dimensionality (number of elements)
    static const uint64_t MAX;
    static const int64_t UNLIMITED;
    static const int64_t UNKNOWN;
};
typedef std::vector<Dim> DimVector;
typedef boost::shared_ptr<DimVector> DimVectorPtr;
#endif // LOADER_ARRAYCOMMON_HH
