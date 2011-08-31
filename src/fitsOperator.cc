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
    std::cout << "loadFits invoked with file=" << filePath
              << " hdu=" << hduNumber << " targetArray=" << arrayName 
              << std::endl;
    FitsArray fa(filePath, hduNumber);
    std::cout << "Constructed FitsArray(" << filePath << ", " 
              << hduNumber << ")\n";
    scidb::ArrayID aid = scidbCreateArray(arrayName, *fa.arrayDesc());
    std::cout << "Created array(" << arrayName << std::endl;

    ScidbArrayCopier copier(aid, 1);
    std::cout << "Prepared copier" << std::endl;
    FitsSource f(fa);
    std::cout << "Prepared fitsSource" << std::endl;

    copier.copyChunk(0, f);
    std::cout << "Copied chunk." << std::endl;
}
