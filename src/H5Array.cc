#include "H5Array.hh" // myself
//#pragma GCC diagnostic push
//#pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
#include "array/Metadata.h" // scidb
//#pragma GCC diagnostic pop

#include "arrayCommon.hh"


#include <iostream>
#include <boost/make_shared.hpp>

////////////////////////////////////////////////////////////////////////
// H5Array::DataSet
////////////////////////////////////////////////////////////////////////
// DataSet that understands its type.
// This implementation is mostly based on the original Loader class
////////////////////////////////////////////////////////////////////////
class H5Array::DataSet {
public:
    DataSet(std::string const& f, std::string const& dataSetName) 
        : _h5f(f, H5F_ACC_RDONLY), _dimHint(0) {
        try {
            _h5ds = _h5f.openDataSet(dataSetName);
            _readType();
        } catch(H5::Exception) {
            std::cerr << "Unknown HDF5 exception." << std::endl;
        }
    }
    
    DimVectorPtr getDims() const { return _dims; }
    int getDimHint() const { return _dimHint; }
    AttrVectorPtr getAttrs() const { return _attrs; }
private:
    bool _unwrapVlenComp(H5::CompType& ct, H5T_class_t& tc);
    void _complain(char const* s) {
        std::cout << "Error in H5Array::DataSet: " << s << std::endl;
    }        
    void _readType();
    DimVectorPtr _readArrayDims(H5::ArrayType& at);
    AttrVectorPtr _readSimpleType(H5::DataType const& dt);
    DimVectorPtr _readDimensions(H5::DataSet& ds, bool addExtra);

    H5::H5File _h5f;
    H5::DataSet _h5ds;
    // When array elements are arrays, we flatten their dimensions. 
    DimVectorPtr _dims; 
    AttrVectorPtr _attrs;
    int _dimHint; // If the type has dimensions, then the hint is
    // the number of the first dimension not flattened.
};

/// Test and unwrap a "compound { vlen { compound {???} } }"
/// @param ct : H5::CompType that is modified by unwrapping
/// @return true if we unwrapped and modified the input ct
bool H5Array::DataSet::_unwrapVlenComp(H5::CompType& ct, H5T_class_t& tc) {
    if ( ct.getNmembers() == 1  && 
         ct.getMemberDataType(0).getClass() == H5T_VLEN ) {
        // jump to the inside compound
        H5::VarLenType vt = ct.getMemberVarLenType(0);
        H5::DataType dt = vt.getSuper();
        tc = dt.getClass();
        if(tc == H5T_COMPOUND) {
            //ct = dt; // FIXME: Convert.
        }
        return true;
    }
    return false;
}

void H5Array::DataSet::_readType() {
    H5T_class_t type_class = _h5ds.getTypeClass();
    bool hasExtraDim = false;
    DimVectorPtr dims(new DimVector());
    AttrVectorPtr attrs;
    // Special handling
    if(type_class == H5T_COMPOUND) {
        H5::CompType ct = _h5ds.getCompType();
        if(_unwrapVlenComp(ct, type_class)) {
            hasExtraDim = true;
        }
    }
    switch(type_class) {
    case H5T_NO_CLASS: // error
        _complain("No type class found in dataset.");
        break;
    case H5T_INTEGER: // integers
        std::cout << "int" << std::endl;
        //addAttrIntType(dataSet.getIntType(), "");
        break;
    case H5T_FLOAT: // floats
        std::cout << "float" << std::endl;
        //addAttrFloatType(dataSet.getFloatType(), "");
        break;
    case H5T_TIME: // date and time
    case H5T_STRING: // char string
    case H5T_BITFIELD: // bit field
    case H5T_OPAQUE: // blobs
        _complain("Unexpected type found in dataset.");
        break;
    case H5T_COMPOUND: // compounds
        std::cout << "compound" << std::endl;
        //processCompoundType(compType, "");
        break;
    case H5T_REFERENCE: // refs
    case H5T_ENUM: // enums
    case H5T_VLEN: // vlens
        _complain("Unexpected type found in dataset.");
        break;
    case H5T_ARRAY: { // arrays
        H5::ArrayType arrayType = _h5ds.getArrayType();
        DimVectorPtr addl = _readArrayDims(arrayType);
        dims->insert(dims->end(), addl->begin(), addl->end());
        attrs = _readSimpleType(arrayType.getSuper());
    }
        break;
    default:
        _complain("Unexpected type found in dataset.");
        break;
    } // switch(type_class)
    for(unsigned i=0; i < attrs->size(); ++i) {
        std::cout << "attribute with size " << (*attrs)[i].tS
                  << " order " << (*attrs)[i].lEnd
                  << " sign " << (*attrs)[i].sign << std::endl;
    }
    _dimHint = dims->size();
    // add dimension
    DimVectorPtr d = _readDimensions(_h5ds, hasExtraDim);
    dims->insert(dims->end(), d->begin(), d->end());
    for(unsigned i=0; i < dims->size(); ++i) {
        std::cout << "dim with size " << (*dims)[i].d1
                  << " - " << (*dims)[i].d2
                  << " extent " << (*dims)[i].curNElems << std::endl;
    }

    // Update object
    _dims = dims;
    _attrs = attrs;
}    

DimVectorPtr H5Array::DataSet::_readArrayDims(H5::ArrayType& at) {
    DimVectorPtr dims(new DimVector());
    int rank = at.getArrayNDims();
    assert(rank > 0);
    hsize_t* extents = new hsize_t[rank];
    at.getArrayDims(extents);
    for(int i=0; i < rank; ++i) { 
        dims->push_back(Dim(0, extents[i]-1, extents[i]));
    }
    delete[] extents;
    return dims;
}

AttrVectorPtr H5Array::DataSet::_readSimpleType(H5::DataType const& dt) {
    AttrVectorPtr attrs(new AttrVector());
    switch(dt.getClass()) {
    case H5T_INTEGER: // integers
        attrs->push_back(Attr::makeInteger(dt));
        break;
    case H5T_FLOAT: // floats
        attrs->push_back(Attr::makeFloat(dt));
        break;
    case H5T_STRING: // char string
        attrs->push_back(Attr::makeString(dt));
        break;
    default:
        _complain("Unsupported type found(readSimpleType)");
    }
    return attrs;
}

DimVectorPtr H5Array::DataSet::_readDimensions(H5::DataSet& ds, 
                                               bool addExtra) {
    DimVectorPtr dims(new DimVector());
    if(addExtra) dims->push_back(Dim(0,Dim::UNLIMITED, Dim::UNKNOWN));

    H5::DataSpace dSpace = ds.getSpace();
    if(!dSpace.isSimple()) {  // Simple (nd-array)
        // Assume scalar (singleton), non-null
        dims->push_back(Dim(0,1,1));            
        return dims;
    }

    int rank = dSpace.getSimpleExtentNdims();        
    hsize_t* extents = new hsize_t[rank];
    hsize_t* limits = new hsize_t[rank];  // Ignore for now. 
    // Might be a good layout/usage hint.
    int r = dSpace.getSimpleExtentDims(extents, limits);
    assert(r == rank);
    for(int i=0; i<rank; ++i) {
        hsize_t limit = (limits[i] <= Dim::MAX) ?
            limits[i] : Dim::UNLIMITED;
        dims->push_back(Dim(0, limit, extents[i]));
    }
    delete[] extents;
    delete[] limits;
    return dims;
}    

////////////////////////////////////////////////////////////////////////
// H5Array::SlabIter
////////////////////////////////////////////////////////////////////////
H5Array::SlabIter& H5Array::SlabIter::operator++() {
    for(unsigned i=0; i < _coords.size(); ++i) {
        _coords[i] += (_ha._chunkIncr)[i];
    }
    return *this;
}

H5Array::SlabIter::SlabIter(H5Array const& ha, bool makeEnd) 
    : _ha(ha), _coords(ha._chunkIncr.size()) {

    DimVectorPtr dp = ha._ds->getDims();
    DimVector& d = *dp;
    if(makeEnd) { 
        for(unsigned i=0; i < _coords.size(); ++i) {
            int64_t incr = _ha._chunkIncr[i];
            int64_t nelem = d[i].curNElems;
            if(incr > 0) _coords[i] = d[i].d1 + (nelem / incr);
            else _coords[i] = 0;
        }
    } else {
        for(unsigned i=0; i < _coords.size(); ++i) {
            _coords[i] = d[i].d1;
        }
    }
}
   


////////////////////////////////////////////////////////////////////////
// H5Array
////////////////////////////////////////////////////////////////////////
H5Array::H5Array(std::string const& fPath, std::string const& path) 
    : _filePath(fPath), _path(path), _ds(new DataSet(fPath, path)) {
    // FIXME Need to setup chunkIncr 
}

SdlVectorPtr convert(DimVectorPtr dp) {
    SdlVectorPtr v(new SdlVector(dp->size()));
    transform(dp->begin(), dp->end(), v->begin(), toScidbLite());
    return v;
}

SalVectorPtr H5Array::getScidbAttrs() const {
    assert(_ds.get());
    AttrVectorPtr attrs = _ds->getAttrs();
    SalVectorPtr v(new SalVector(attrs->size()));
    transform(attrs->begin(), attrs->end(), v->begin(), toScidbLite());
    return v;
    
}


boost::shared_ptr<scidb::ArrayDesc> H5Array::getArrayDesc() const {
    return ScidbIface::getArrayDesc(*this);
}


SdlVectorPtr H5Array::getScidbDims() const {
    assert(_ds);
    DimVectorPtr dims = _ds->getDims();
    assert(dims);
    SdlVectorPtr v(new SdlVector(dims->size()));
    transform(dims->begin(), dims->end(), v->begin(), toScidbLite());
    _imposeChunking(v);
    return v;
}

void H5Array::_imposeChunking(SdlVectorPtr dims) const {
    unsigned const minFrag = 1024*1024;
    // Is this the right approach?  
    assert(_ds);
    assert(dims);
    int split = _ds->getDimHint();
    if(split > 0) {
    }
    uint64_t accSize = 0;
    for(unsigned i=0; i < dims->size(); ++i) {
        ScidbDimLite& d = (*dims)[i];
        // Update multiplied slab size 
        if(accSize) accSize *= d.chunkInterval;
        else accSize = d.chunkInterval;
        // Use thin chunks for dims, once slabs are "big enough"
        if(accSize > minFrag) d.chunkInterval = 1; 
            


    }
}

#if 0
_schema = ArrayDesc(versionName, desc.getAttributes(), newDims);
_arrayID = SystemCatalog::getInstance()->addArray(_schema, psRoundRobin);
#endif 
