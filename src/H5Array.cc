#include "H5Array.hh"
#include "array/Metadata.h" // scidb

H5Array::H5Array(std::string const& fPath, std::string const& path) {
    // Initialize for using the data object at path in file at fPath.
    // FIXME

}

boost::shared_ptr<scidb::ArrayDesc> H5Array::getArrayDesc() const {
    using scidb::ArrayDesc;
    boost::shared_ptr<ArrayDesc> desc(new ArrayDesc());
    // FIXME
    return desc;
}
