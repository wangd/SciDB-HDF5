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
            //addAttrIntType(dataSet.getIntType(), "");
            break;
        case H5T_FLOAT: // floats
            //addAttrFloatType(dataSet.getFloatType(), "");
            break;
        case H5T_TIME: // date and time
        case H5T_STRING: // char string
        case H5T_BITFIELD: // bit field
        case H5T_OPAQUE: // blobs
            _complain("Unexpected type found in dataset.");
            break;
        case H5T_COMPOUND: // compounds
            //processCompoundType(compType, "");
            break;
        case H5T_REFERENCE: // refs
        case H5T_ENUM: // enums
        case H5T_VLEN: // vlens
            _complain("Unexpected type found in dataset.");
            break;
        case H5T_ARRAY: // arrays
            //ArrayType arrayType = dataSet.getArrayType();
            //processArrayType(arrayType);
            break;
        default:
            _complain("Unexpected type found in dataset.");
            break;
        } // switch(type_class)
        
        // add dimension
        _readDimensions(hasExtraDim);
        //_dim.push_back(Dim(0, Dim::UNLIMITED, Dim::UNKNOWN));
    }    

    void _readDimensions(bool addExtra) {
        std::vector<Dim> dims;
        hsize_t curDims = 1;     // default for scalar dataSpace
        H5::DataSpace dSpace = _h5ds.getSpace();
        if ( dSpace.isSimple() ) {
            hsize_t maxDims = 1; // maxDims is not set for scalar dataSpace,
            // so set it to 1
            dSpace.getSimpleExtentDims(&curDims, &maxDims);
            Dim oneDim(0, maxDims, curDims);
            static unsigned long long MAXD = 100000000000000ULL;
            if (maxDims > MAXD ) {
                oneDim.d2 = Dim::UNLIMITED;
            }
            dims.push_back(oneDim);
        } else {
            dims.push_back(Dim(0, 1, 1));
        }
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

boost::shared_ptr<scidb::ArrayDesc> H5Array::getArrayDesc() const {
    using scidb::ArrayDesc;
    boost::shared_ptr<ArrayDesc> desc(new ArrayDesc());
    // FIXME
    return desc;
}
