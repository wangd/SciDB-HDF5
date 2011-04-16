#include "H5Array.hh" // myself
#include "array/Metadata.h" // scidb
#include "arrayCommon.hh"
#include <iostream>

////////////////////////////////////////////////////////////////////////
// H5Array::DataSet
////////////////////////////////////////////////////////////////////////
// DataSet that understands its type.
// This implementation is mostly based on the original Loader class
class H5Array::DataSet {
public:
    DataSet(std::string const& f, std::string const& dataSetName) 
        : _h5f(f, H5F_ACC_RDONLY) {
        _h5ds = _h5f.openDataSet(dataSetName);
        _readType();
    }
    
private:
    /// Test and unwrap a "compound { vlen { compound {???} } }"
    /// @param ct : H5::CompType that is modified by unwrapping
    /// @return true if we unwrapped and modified the input ct
    bool _unwrapVlenComp(H5::CompType& ct, H5T_class_t& tc) {
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
    void _complain(char const* s) {
        std::cout << "Error in H5Array::DataSet: " << s << std::endl;
    }        
    void _readType() {
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
        // add dimension
        _readDimensions(_h5ds, hasExtraDim);

    }    
    DimVectorPtr _readArrayDims(H5::ArrayType& at) {
        DimVectorPtr dims(new DimVector());
        int rank = at.getArrayNDims();
        assert(rank > 0);
        hsize_t* extents = new hsize_t[rank];
        at.getArrayDims(extents);
        for(int i=0; i < rank; ++i) { 
            dims->push_back(Dim(0,extents[i], extents[i]));
        }
        delete[] extents;
        return dims;
    }
    AttrVectorPtr _readSimpleType(H5::DataType const& dt) {
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
    DimVectorPtr _readDimensions(H5::DataSet& ds, bool addExtra) {
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

    H5::H5File _h5f;
    H5::DataSet _h5ds;
};
#if 0
void _getDataSetMetadata(std::string const& dataSetName) {
    // Reopen file. FIXME: Is this necessary?
    H5::H5File file( _filename, H5F_ACC_RDONLY );

    DataSet dataSet = file.openDataSet(dataSetName);
    H5T_class_t type_class = dataSet.getTypeClass();

    CompType compType;

    if ( type_class == H5T_INTEGER ) {
        addAttrIntType(dataSet.getIntType(), "");
    } else if ( type_class == H5T_FLOAT ) {
        addAttrFloatType(dataSet.getFloatType(), "");
    } else if ( type_class == H5T_COMPOUND ) {
        processCompoundType(compType, "");
    } else if ( type_class = H5T_ARRAY ) {
        ArrayType arrayType = dataSet.getArrayType();
        processArrayType(arrayType);
    } else {
        cout << "Unexpected type " << print_H5T_class_t_Name(type_class) 
             << endl;
        throw(1);
    }
    hsize_t curDims = 1;     // default for scalar dataSpace
    DataSpace dataSpace = dataSet.getSpace();
    if ( dataSpace.isSimple() ) {
        hsize_t maxDims = 1; // maxDims is not set for scalar dataSpace,
                             // so set it to 1
        dataSpace.getSimpleExtentDims(&curDims, &maxDims);
        OneDim oneDim(0, maxDims, curDims);
        static unsigned long long MAXD = 100000000000000ULL;
        if (maxDims > MAXD ) {
            oneDim.d2 = OneDim::UNLIMITED;
        }
        _dim.push_back(oneDim);
    } else {
        _dim.push_back(OneDim(0, 1, 1));
    }

}
#endif

H5Array::H5Array(std::string const& fPath, std::string const& path) 
    : _filePath(fPath), _path(path), _ds(new DataSet(fPath, path)) {

    // Initialize for using the data object at path in file at fPath.
    // FIXME

}

// boost::shared_ptr<scidb::ArrayDesc> H5Array::getArrayDesc() const {
//     using scidb::ArrayDesc;
//     boost::shared_ptr<ArrayDesc> desc(new ArrayDesc());
//     // FIXME
//     return desc;
// }
