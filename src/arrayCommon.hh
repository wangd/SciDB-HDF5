#ifndef LOADER_ARRAYCOMMON_HH
#define LOADER_ARRAYCOMMON_HH
#include <vector>
#include <H5Cpp.h>
////////////////////////////////////////////////////////////////////////
// class Attr
////////////////////////////////////////////////////////////////////////
class Attr {
public:
    Attr() : tS(0), inNA("") {}

    H5T_class_t type;
    size_t tS; // size of this type, e.g. 4 for INT
    std::string attrName;
    const H5::PredType* predType;
    H5T_str_t strPad;   // for STRING only
    H5T_cset_t strCset; // for STRING only
    htri_t strIsVlstr;  // for STRING only
    std::vector< std::pair<std::string, long> > enumMembers; // for ENUM only
    std::string tN; // sciDbTypeName;
    std::string inNA; // "" or name of nested array this elem belongs to 
};

////////////////////////////////////////////////////////////////////////
// class Dim
////////////////////////////////////////////////////////////////////////
class Dim {
public:
    Dim(int _d1, int _d2, int _n) : d1(_d1), d2(_d2), curNElems(_n) {}
        
    int d1; // start (typically 0)
    int d2; // end or UNLIMITED
    int curNElems; // current dimensionality (number of elements)
    static const int UNLIMITED;
    static const int UNKNOWN;
};
#endif // LOADER_ARRAYCOMMON_HH
