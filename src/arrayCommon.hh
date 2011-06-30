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
// class ScidbAttrLite - A class containing the values needed to
// construct scidb::AttributeDesc, with none of the behavior and none
// of the dependencies.
////////////////////////////////////////////////////////////////////////
class ScidbAttrLite {
public:
    ScidbAttrLite() {}
    ScidbAttrLite(std::string const& name_, std::string const& typeId_);
    uint32_t id; // Attribute # in array (index in attribute vector)
    std::string name; // Attribute name
    std::string typeId; // scidb::TypeId
    // Avoid
    // int16_t flags;
    // uint16_t defaultCompressionMethod;
    // std::set<std::string> aliases;
    // int16_t reserve;
    friend std::ostream& operator<<(std::ostream& os, 
                                    ScidbAttrLite const& sal);
};
typedef std::vector<ScidbAttrLite> SalVector;
typedef boost::shared_ptr<SalVector> SalVectorPtr;

////////////////////////////////////////////////////////////////////////
// class Dim
////////////////////////////////////////////////////////////////////////
class Dim {
public:
    Dim(int64_t _d1, int64_t _d2, int64_t _n) 
        : d1(_d1), d2(_d2), curNElems(_n), chunkLength(_n) {}
        
    int64_t d1; // start (typically 0)
    int64_t d2; // end or UNLIMITED
    int64_t curNElems; // current dimensionality (number of elements)
    int chunkLength; // Chunk size in this dimension.
    static const uint64_t MAX;
    static const int64_t UNLIMITED;
    static const int64_t UNKNOWN;
};
typedef std::vector<Dim> DimVector;
typedef boost::shared_ptr<DimVector> DimVectorPtr;

////////////////////////////////////////////////////////////////////////
// class ScidbDimLite - A class containing the values needed to
// construct scidb::DimensionDesc, with none of the behavior and none
// of the dependencies.
////////////////////////////////////////////////////////////////////////
class ScidbDimLite {
public:
    ScidbDimLite() {}
    ScidbDimLite(std::string const& name, Dim const& dim);
    
    std::string name; // Dimension name
    int64_t min; // Dimension absolute minimum (units)
    int64_t start; // Dimension start (units)
    int64_t end; // Dimension end (units)
    int64_t max; // Dimension absolute maximum (units)
    int64_t chunkInterval; // Chunk size in this dimension
    int64_t chunkOverlap; // Chunk overlap in this dimension
    std::string typeId; // SciDB TypeId*
    std::string arrayName; // Containing array's name

    // Notes:
    // * Avoid compiled dependency on SciDB's TypeId since it would
    // make this code dependent on the rest of SciDB and untestable in
    // isolation.
    friend std::ostream& operator<<(std::ostream& os, ScidbDimLite const& sdl);
};
typedef std::vector<ScidbDimLite> SdlVector;
typedef boost::shared_ptr<SdlVector> SdlVectorPtr;

////////////////////////////////////////////////////////////////////////
// Conversion functions
////////////////////////////////////////////////////////////////////////
struct toScidbLite {
public:
    toScidbLite() : count(0) {}
    ScidbDimLite operator()(Dim const& d);
    ScidbAttrLite operator()(Attr const& a);
    int count;
};

////////////////////////////////////////////////////////////////////////
// Scidb constants
////////////////////////////////////////////////////////////////////////
extern char const* SCIDB_TID_INT64;

#endif // LOADER_ARRAYCOMMON_HH
