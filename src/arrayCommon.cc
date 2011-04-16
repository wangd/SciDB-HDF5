#include "arrayCommon.hh"
////////////////////////////////////////////////////////////////////////
// Dim
////////////////////////////////////////////////////////////////////////
const int64_t Dim::UNLIMITED = -1;
const int64_t Dim::UNKNOWN = -2;
const uint64_t Dim::MAX = 100000000000000ULL;

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
    return a;
}
Attr Attr::makeFloat(H5::DataType const& dt) {
    H5::FloatType obj(dt.getId());
    Attr a;
    a.type = H5T_FLOAT;
    a.tS = obj.getSize();
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
