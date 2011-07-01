#include "arrayCommon.hh"
#include <sstream>

////////////////////////////////////////////////////////////////////////
// Attr
////////////////////////////////////////////////////////////////////////
Attr Attr::makeInteger(H5::DataType const& dt) {
    H5::IntType obj(dt.getId());
    Attr a;
    a.type = H5T_INTEGER;
    a.tS = obj.getSize();
    assert(H5Tget_size(obj.getId()) == a.tS);
    a.lEnd = (obj.getOrder() == H5T_ORDER_LE);
    assert((obj.getOrder() == H5T_ORDER_LE) ||
           (obj.getOrder() == H5T_ORDER_BE));
    a.sign = obj.getSign();
    switch(a.tS) {
    case sizeof(int16_t): a.tN = "int16"; break;
    case sizeof(int32_t): a.tN = "int32"; break;
    case sizeof(int64_t): a.tN = "int64"; break;
    default: 
        std::stringstream ss;
        ss << "int" << (a.tS * 8);
        a.tN = ss.str();
        break;
    }
    if(a.sign == 0) { a.tN = "u" + a.tN; }
    
    return a;
}

Attr Attr::makeFloat(H5::DataType const& dt) {
    H5::FloatType obj(dt.getId());
    Attr a;
    a.type = H5T_FLOAT;
    a.tS = obj.getSize();
    switch(a.tS) {
    case sizeof(float): a.tN = "float"; break;
    case sizeof(double): a.tN = "double"; break;
    default: 
        std::stringstream ss;
        ss << "float" << (a.tS * 8);
        a.tN = ss.str();
        break;
    }
    return a;
}
Attr Attr::makeString(H5::DataType const& dt) {
    Attr a;
    H5::StrType obj(dt.getId());
    a.type = H5T_STRING;
    a.tS = obj.getSize();
    a.strPad = obj.getStrpad();
    a.strCset = obj.getCset();
    a.strIsVlstr = H5Tis_variable_str(obj.getId());
    // FIXME: not sure we need the more complex handling from loader.cc
    return a;
}
////////////////////////////////////////////////////////////////////////
// ScidbAttrLite
////////////////////////////////////////////////////////////////////////
ScidbAttrLite::ScidbAttrLite(std::string const& name_, 
                             std::string const& typeId_) 
    : name(name_), typeId(typeId_) {
}

//AttributeDesc(0, "value", attrSet.getType(), 0, 0);
std::ostream& operator<<(std::ostream& os, ScidbAttrLite const& sal) {
    return os << "Attr '" << sal.name << "' (" << sal.typeId << ")";
}

////////////////////////////////////////////////////////////////////////
// Dim
////////////////////////////////////////////////////////////////////////
const int64_t Dim::UNLIMITED = -1;
const int64_t Dim::UNKNOWN = -2;
const uint64_t Dim::MAX = (1ULL << 63)-1;

////////////////////////////////////////////////////////////////////////
// ScidbDimLite
////////////////////////////////////////////////////////////////////////
ScidbDimLite::ScidbDimLite(std::string const& name_, Dim const& dim) 
    : name(name_), 
      min(dim.d1),
      start(dim.d1), end(dim.d1 + dim.curNElems - 1), 
      max(dim.d2),
      chunkInterval(end-start+1), chunkOverlap(0), 
      typeId(SCIDB_TID_INT64), arrayName("") {
}

std::ostream& operator<<(std::ostream& os, ScidbDimLite const& sdl) {
    return os << "Dim '" << sdl.name << "' [" 
              << sdl.start << "-" << sdl.end << " of " 
              << sdl.min << "-" << sdl.max << " chunk=" 
              << sdl.chunkInterval << " ovl=" << sdl.chunkOverlap
              << "] (" << sdl.typeId << ")";
}

////////////////////////////////////////////////////////////////////////
// Conversion functions
////////////////////////////////////////////////////////////////////////
std::string const unnamed("unnamed");
std::string toStr(int i) {
    std::stringstream s;
    s << i;
    return s.str();
}
ScidbDimLite toScidbLite::operator()(Dim const& d) {
    return ScidbDimLite(unnamed + toStr(count++), d); // Don't have a name now.
}
ScidbAttrLite toScidbLite::operator()(Attr const& a) {
    return ScidbAttrLite(a.attrName, a.tN);
}

////////////////////////////////////////////////////////////////////////
// Scidb constants
////////////////////////////////////////////////////////////////////////
char const* SCIDB_TID_INT64 = "int64";

