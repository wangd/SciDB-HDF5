#include "loadOperator.hh"

// std
#include <string>

// pkg
#include "FitsArray.hh"
#include "loadUtils.hh"

namespace {
    class FitsSource : public ScidbArrayCopier::Source {
    public:
        FitsSource(FitsArray& fa) : _fa(fa), _coords(fa.rank(), 0) {
        }

        virtual scidb::Coordinates const& coords() const {
            return _coords;
        }
        virtual Size footprint(int attNo) const { 
            return _fa.footprint();
        }
        virtual Size elementCount(int attNo, bool clip) const { 
            return _fa.elementCount();
        }
        virtual void copy(int attNo, void* target) {
            _fa.copyArray(target, _fa.elementCount());
        }
    private:
        FitsArray& _fa;
        std::vector<int64_t> _coords;
    };
}


void loadFits(std::string const& filePath, 
              int hduNumber,
              std::string const& arrayName) {
    std::string resultName = arrayName;

    // Do something good.
    FitsArray fa(filePath, hduNumber);
    scidb::ArrayID aid = scidbCreateArray(arrayName, *fa.arrayDesc());
    
    ScidbArrayCopier copier(aid, 1);
    FitsSource f(fa);
    copier.copyChunk(0, f);
}
