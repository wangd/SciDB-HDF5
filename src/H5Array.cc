//   This file is part of the SciDB-HDF5 project.
//   Copyright 2011  Jacek Becla, Daniel Liwei Wang
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//   
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
#include "H5Array.hh" // myself
//#pragma GCC diagnostic push
//#pragma GCC diagnostic ignored "-Wnon-virtual-dtor"
#include "array/Metadata.h" // scidb
//#pragma GCC diagnostic pop

#include "arrayCommon.hh"
#include <iostream>
#include <boost/make_shared.hpp>
#include <boost/shared_array.hpp>

namespace {
    ////////////////////////////////////////////////////////////////////
    // Helpers for H5Array::DataSet
    ////////////////////////////////////////////////////////////////////
    void adjustChunking(DimVector& dv) {
        // Choose to chunk full dimension extent slabs up to
        // least-rapidly-varying dimension (or until a threshold is
        // reached.
        // (Can we read chunking decisions from HDF? Actually, we need
        // to, for 1D of 2D = 3D case. -Daniel
        //
        typedef DimVector::reverse_iterator Iter;
        Iter end = dv.rend();
        int64_t accumulated = 0;
        int64_t threshold = 1024*1024; // Kind of arbitrary....
        for(Iter i = dv.rbegin(); i != end; ++i) {
            int64_t thisDim = i->curNElems;
            int64_t next = accumulated * thisDim;
            if(next == 0) next = thisDim;
            if(next > threshold) {
                // FIXME: need to handle big first dimensions.
                assert(accumulated > 1); 
                i->chunkLength = 1;
            } else {
                i->chunkLength = thisDim;
                accumulated = next;
            }
        }
#if 0
        std::cout << "ChunkSizes: ";
        for(Iter i = dv.begin(); i != end; ++i) {
            std::cout << i->chunkLength << ",";
        }
        std::cout << std::endl;
#endif
    }

    ////////////////////////////////////////////////////////////////////
    // Helpers for H5Array 
    ////////////////////////////////////////////////////////////////////
    class extractIncr {
    public:
        typedef scidb::Coordinates::value_type RetVal;
        inline RetVal operator()(DimVector::value_type const& d) {
            return d.chunkLength;  // This isn't quite right.
        }
    };

    SdlVectorPtr convert(DimVectorPtr dp) {
        SdlVectorPtr v(new SdlVector(dp->size()));
        transform(dp->begin(), dp->end(), v->begin(), toScidbLite());
        return v;
    }
    
}

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
            adjustChunking(*_dims);
        } catch(H5::Exception) {
            std::cerr << "Unknown HDF5 exception." << std::endl;
        }
    }
    
    DimVectorPtr getDims() const { return _dims; }
    int getDimHint() const { return _dimHint; }
    AttrVectorPtr getAttrs() const { return _attrs; }
    Size getTypeSize() const;
    H5::DataSpace getSpace() { return _h5ds.getSpace(); }

    void readInto(void* buffer, H5::DataSpace& mem, H5::DataSpace& file);

private:
    bool _unwrapVlenComp(H5::CompType& ct, H5T_class_t& tc);
    void _complain(char const* s) {
        std::cout << "Error in H5Array::DataSet: " << s << std::endl;
    }        
    void _readType();
    DimVectorPtr _readArrayDims(H5::ArrayType& at);
    AttrVectorPtr _readSimpleType(H5::DataType const& dt);
    AttrVectorPtr _readCompType(H5::CompType const& ct);
    DimVectorPtr _readDimensions(H5::DataSet& ds, bool addExtra);

    H5::H5File _h5f;
    H5::DataSet _h5ds;
    // When array elements are arrays, we flatten their dimensions. 
    DimVectorPtr _dims; 
    AttrVectorPtr _attrs;
    int _dimHint; // If the type has dimensions, then the hint is
    // the number of the first dimension not flattened.
};

H5Array::Size H5Array::DataSet::getTypeSize() const {
    return _h5ds.getDataType().getSize();
}

void H5Array::DataSet::readInto(void* buffer, 
                                H5::DataSpace& mem, H5::DataSpace& file) {
    _h5ds.read(buffer, _h5ds.getDataType(), mem, file);
}


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
        attrs = _readSimpleType(_h5ds.getIntType());
        break;
    case H5T_FLOAT: // floats
        std::cout << "float" << std::endl;
        attrs = _readSimpleType(_h5ds.getFloatType());
        break;
    case H5T_TIME: // date and time
    case H5T_STRING: // char string
    case H5T_BITFIELD: // bit field
    case H5T_OPAQUE: // blobs
        _complain("Unexpected type found in dataset.");
        break;
    case H5T_COMPOUND: {// compounds
        std::cout << "compound" << std::endl;
        H5::CompType cType = _h5ds.getCompType();
        attrs = _readCompType(cType);
    }
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
        std::cout << "attribute ("  << (*attrs)[i].attrName
                  << ") with size " << (*attrs)[i].tS
                  << " order " << (*attrs)[i].lEnd
                  << " sign " << (*attrs)[i].sign << std::endl;
    }
    _dimHint = dims->size();
    // add dimension
    DimVectorPtr d = _readDimensions(_h5ds, hasExtraDim);
    dims->insert(dims->begin(), d->begin(), d->end());
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
        dims->push_back(Dim(0, extents[i]-1, extents[i], true));
    }
    delete[] extents;
    return dims;
}

AttrVectorPtr H5Array::DataSet::_readSimpleType(H5::DataType const& dt) {
    AttrVectorPtr attrs(new AttrVector());
    std::stringstream ss;
    switch(dt.getClass()) {
    case H5T_INTEGER: // integers
        attrs->push_back(Attr::makeInteger(dt));
        ss << "int" << attrs->size();
        attrs->back().attrName = ss.str();
        break;
    case H5T_FLOAT: // floats
        attrs->push_back(Attr::makeFloat(dt));
        ss << "float" << attrs->size();
        attrs->back().attrName = ss.str();
        break;
    case H5T_STRING: // char string
        attrs->push_back(Attr::makeString(dt));
        ss << "str" << attrs->size();
        attrs->back().attrName = ss.str();
        break;
    default:
        _complain("Unsupported type found(readSimpleType)");
    }
    return attrs;
}

AttrVectorPtr H5Array::DataSet::_readCompType(H5::CompType const& ct) {
    // Read each member of the compound.
    // For now, only support simple types.
    int total = ct.getNmembers();
    AttrVectorPtr attrs(new AttrVector());
    for(int i = 0; i < total; ++i) {
        H5::DataType dt = ct.getMemberDataType(static_cast<unsigned>(i));
        AttrVectorPtr a = _readSimpleType(dt);
        if(!a || (a->size() > 1)) {
            _complain("Invalid type in compound.");
            break;
        }
        // Use compound member's name.
        (*a)[0].attrName = ct.getMemberName(i); 
        attrs->push_back((*a)[0]);
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
        // Not sure how to translate UNLIMITED into SciDB, 
        // so clamp at MAX
        hsize_t limit = (limits[i] <= Dim::MAX) ?
            limits[i] : Dim::MAX;
        dims->push_back(Dim(0, limit, extents[i]));
    }
    delete[] extents;
    delete[] limits;
    return dims;
}    


////////////////////////////////////////////////////////////////////////
// H5Array::SlabIter
////////////////////////////////////////////////////////////////////////
class H5Array::SlabIter::Cursor {
public:
    void setStart(Coordinates& c) {
        assert(_start.get());
        assert(_extent.get());
        std::copy(c.begin(), c.end(), _start.get());        
        fileSpace.selectHyperslab(H5S_SELECT_SET, 
                                   _extent.get(), _start.get());
        memSpace.selectHyperslab(H5S_SELECT_SET, 
                                  _extent.get(), _start0.get());
        assert(memSpace.selectValid());
        assert(fileSpace.selectValid());
        assert(memSpace.getSelectNpoints() == fileSpace.getSelectNpoints());
#if 0
        std::cout << "rank is " << rank << std::endl;
        std::cout << "slab size is " << bufferSize << std::endl;
        
        std::cout << "target space has " << memSpace.getSelectNpoints() 
                  << " points, src has " << fileSpace.getSelectNpoints()
                  << " points" << std::endl;
#endif
    }

    Cursor(DimVector& d, H5Array::DataSet& ds) {
        int rank = _hRank = d.size();
        
        // Setup dimensionalities
        _start.reset(new hsize_t[rank]);
        _extent.reset(new hsize_t[rank]);
        std::transform(d.begin(), d.end(), _extent.get(), extractIncr());
        _start0.reset(new hsize_t[rank]);
        memset(_start0.get(), 0, sizeof(hsize_t)*rank);    
        
        // Alter rank to reflect HDF5 array of arrays
        for(int i=0; i < rank; ++i) {
            if(d[i].inside) { // Reset rank to first "inside" dimension.
                _hRank = i;
                break;
            }
        }
        // Setup spaces
        fileSpace = ds.getSpace();
        memSpace = H5::DataSpace(_hRank, _extent.get());    
    }

    H5::DataSpace fileSpace;
    H5::DataSpace memSpace;

private:
    boost::shared_array<hsize_t> _start;
    boost::shared_array<hsize_t> _start0;
    boost::shared_array<hsize_t> _extent;
    int _hRank;
};

////////////////////////////////////////////////////////////////////////
// H5Array::SlabIter
////////////////////////////////////////////////////////////////////////
H5Array::SlabIter& H5Array::SlabIter::operator++() {
    DimVectorPtr dp = _ha._ds->getDims();
    DimVector& dv = *dp;
    bool end = true;
    for(unsigned i=0; i < _coords.size(); ++i) {
        Coordinates::value_type length = _coords[i];
        length += dv[i].chunkLength;
        if(length >= (dv[i].d1 + dv[i].curNElems)) {
            _coords[i] = dv[i].d1; // d1 = 0 usually
        } else {
            _coords[i] = length;
            end = false;
            break;
        }
    }
    if(end) {
        unsigned last = _coords.size() - 1;
        _coords[last] = dv[last].d1 + dv[last].curNElems;
    }
    _cacheValid = false;
    return *this;
}

H5Array::SlabIter::SlabIter(H5Array const& ha, bool makeEnd) 
    : _ha(ha), _coords(ha._chunkIncr.size()), 
      _cacheValid(false) {

    DimVectorPtr dp = ha._ds->getDims();
    DimVector& d = *dp;
    for(unsigned i=0; i < _coords.size(); ++i) {
        _coords[i] = d[i].d1;
    }
    if(makeEnd) { 
        unsigned last = _coords.size() - 1;
        _coords[last] += d[last].curNElems;
    }
    assert(_ha._ds.get());
    _slabSize = _computeSlabSize();
    _cursor.reset(new Cursor(d, *_ha._ds));
}

/// @return size of attribute #attNo in bytes.
H5Array::Size H5Array::SlabIter::slabAttrSize(int attNo) const {    
    Size s = elementCount(attNo, false);
    // Apply datatype size.
    AttrVectorPtr ap = _ha._ds->getAttrs();
    AttrVector& a = *ap;
    Size typeSize = a[attNo].tS;
    s *= typeSize;
    return s;
}

/// @return size of slab chunk (all attrs) in bytes.
H5Array::Size H5Array::SlabIter::_computeSlabSize() const {    
    DimVectorPtr dp = _ha._ds->getDims();
    DimVector& d = *dp;
    Size s = 1;
    // Count elements, but not embedded ones.
    for(unsigned i=0; i < d.size(); ++i) {
        if(d[i].inside) break; // stop counting.
        s *= d[i].chunkLength;

    }
    // Apply datatype size.
    s *= _ha._ds->getTypeSize();
    return s;
}

void H5Array::SlabIter::_initSlabCache() {
    _slabCache.reset(new char[slabSize()]);
}



void* H5Array::SlabIter::_readAttrInto(void* buffer, 
                                      void* slabBuffer,
                                      int attNo) {
    // Perform gymnastics to copy the particular attribute's values
    // from the slab buffer into a single-attribute buffer.
    // Find stride and offset
    size_t stride = _ha._ds->getTypeSize();
    // Apply datatype size.
    AttrVectorPtr ap = _ha._ds->getAttrs();
    AttrVector& a = *ap;
    size_t offset = 0;
    for(int i=0; i < attNo; ++i) {
        offset += a[i].tS;
    }
    size_t eltSize = a[attNo].tS;
    size_t elts = slabAttrSize(attNo) / eltSize;
    size_t strideWithout = stride - eltSize;
    char* dest = reinterpret_cast<char*>(buffer);
    char* src = reinterpret_cast<char*>(slabBuffer);
    src += offset; // Shift for attr offset
    for(unsigned i=0; i < elts; ++i) {
        for(unsigned c=0; c < eltSize; ++c) { // copy element
            *dest++ = *src++;
        }
        //if(i < 10) std::cout << "("<< *reinterpret_cast<int*>(dest-4) <<")";

        src += strideWithout; // Move to next array point.
    }
    return buffer;
}

/// Copy slab for compound attr into buffer
void* H5Array::SlabIter::readSlabInto(void* buffer) {
    assert(_cursor);
    _cursor->setStart(_coords);
    memset(buffer, 0, _slabSize);
    _ha._ds->readInto(buffer, _cursor->memSpace, _cursor->fileSpace);
    return buffer;
}

/// Copy slab for attr #attNo into buffer
void* H5Array::SlabIter::readInto(int attNo, void* buffer) {
    if(_ha._ds->getAttrs()->size()  > 1) {
        // Need to maintain buffer.
        if(!_cacheValid) {
            if(!_slabCache) _initSlabCache();
            readSlabInto(_slabCache.get());
            _cacheValid = true;
        }
        return _readAttrInto(buffer, _slabCache.get(), attNo);
    } else {
        return readSlabInto(buffer);
    }
}

void H5Array::SlabIter::_readAttrIntoChunk(scidb::ChunkIterator& ci, 
                                            void* slabBuffer,
                                            int attNo) {
    // Perform gymnastics to copy the particular attribute's values
    // from the slab buffer into a chunk using chunk iterators
    // Find stride and offset
    size_t stride = _ha._ds->getTypeSize();
    // Apply datatype size.
    AttrVectorPtr ap = _ha._ds->getAttrs();
    AttrVector& a = *ap;
    size_t offset = 0;
    for(int i=0; i < attNo; ++i) {
        offset += a[i].tS;
    }
    size_t eltSize = a[attNo].tS;
    size_t elts = slabAttrSize(attNo) / eltSize;
    size_t strideWithout = stride - eltSize;
    char* src = reinterpret_cast<char*>(slabBuffer);
    src += offset; // Shift for attr offset
    for(unsigned i=0; i < elts; ++i) {
        ScidbIface::readValueIntoChunk(ci, src, eltSize);
        src += eltSize;
        src += strideWithout; // Move to next array point.
    }
}

/// Copy slab for attr #attNo into chunk using chunk iterator
void H5Array::SlabIter::readIntoChunk(int attNo, scidb::ChunkIterator& ci)
{
    // Need to maintain buffer.
    if(!_cacheValid) {
        if(!_slabCache) _initSlabCache();
        readSlabInto(_slabCache.get());
        _cacheValid = true;
    }
    _readAttrIntoChunk(ci, _slabCache.get(), attNo);
}

// Find non-empty members in this slab. 
// Always equal to slab extent, since everything is non-empty.
// ...except when array edge is not on a chunk boundary.
H5Array::Size  H5Array::SlabIter::elementCount(int attNo, bool clipEdges) const {
    DimVectorPtr dp = _ha._ds->getDims();
    DimVector& d = *dp;
    Size s = 1;
    // Count elements
    for(unsigned i=0; i < d.size(); ++i) {
        Size ext = d[i].chunkLength;
        if(clipEdges) {
            Size nElem = d[i].curNElems;
            Size pos = _coords[i];
            if(ext > (nElem - pos)) ext = nElem - pos;
        } 
        s *= ext;
    }
    return s; 
}

std::ostream& operator<<(std::ostream& os, H5Array::SlabIter const& i) {
    os << "SlabIter @ ";
    std::copy(i._coords.begin(), i._coords.end(), 
              std::ostream_iterator<int64_t>(os, ","));
    return os;
}


////////////////////////////////////////////////////////////////////////
// H5Array
////////////////////////////////////////////////////////////////////////
H5Array::H5Array(std::string const& fPath, std::string const& path) 
    : _filePath(fPath), _path(path), _ds(new DataSet(fPath, path)) {
    // Pull it from the dims.
    DimVectorPtr dims = _ds->getDims();
    _chunkIncr.resize(dims->size());
    std::transform(dims->begin(), dims->end(), _chunkIncr.begin(), 
                   extractIncr());
}

boost::shared_ptr<scidb::ArrayDesc> H5Array::arrayDesc() const {
    return ScidbIface::getArrayDesc(*this);
}

SalVectorPtr H5Array::scidbAttrs() const {
    assert(_ds.get());
    AttrVectorPtr attrs = _ds->getAttrs();
    SalVectorPtr v(new SalVector(attrs->size()));
    transform(attrs->begin(), attrs->end(), v->begin(), toScidbLite());
    return v;
    
}

SdlVectorPtr H5Array::scidbDims() const {
    assert(_ds);
    DimVectorPtr dims = _ds->getDims();
    assert(dims);
    SdlVectorPtr v(new SdlVector(dims->size()));
    transform(dims->begin(), dims->end(), v->begin(), toScidbLite());
    return v;
}

#if 0
int H5Array::slabCount() const {
    // Perform "dimensional arithmetic" and divide the dimensional
    // extents by the chunk increment using long division and rounding
    // up if we have a remainder. 
    return 1; // FIXME
}
#endif

int H5Array::rank() const {
    return _ds->getDims()->size();
}

int  H5Array::attrCount() const {
    return _ds->getAttrs()->size();
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
