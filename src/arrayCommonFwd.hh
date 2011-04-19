#ifndef LOADER_ARRAYCOMMONFWD_HH
#ifndef LOADER_ARRAYCOMMON_HH
#define LOADER_ARRAYCOMMONFWD_HH
#include <boost/shared_ptr.hpp>
#include <vector>
class Attr;
typedef std::vector<Attr> AttrVector;
typedef boost::shared_ptr<AttrVector> AttrVectorPtr;

class ScidbAttrLite;
typedef std::vector<ScidbAttrLite> SalVector;
typedef boost::shared_ptr<SalVector> SalVectorPtr;

class Dim;
typedef std::vector<Dim> DimVector;
typedef boost::shared_ptr<DimVector> DimVectorPtr;

class ScidbDimLite;
typedef std::vector<ScidbDimLite> SdlVector;
typedef boost::shared_ptr<SdlVector> SdlVectorPtr;
#endif // LOADER_ARRAYCOMMON_HH
#endif // LOADER_ARRAYCOMMONFWD_HH
