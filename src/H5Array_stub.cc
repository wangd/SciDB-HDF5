#include "H5Array.hh"
#include <boost/make_shared.hpp>

// A stub file with non scidb-dependent implementations where possible.

boost::shared_ptr<scidb::ArrayDesc> 
H5Array::ScidbIface::getArrayDesc(H5Array const& h) {
    return boost::shared_ptr<scidb::ArrayDesc>();
}
