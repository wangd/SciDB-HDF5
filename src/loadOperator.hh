#ifndef LOADER_LOADOPERATOR_HH
#define LOADER_LOADOPERATOR_HH

namespace scidb {
    class Value; // Forward
}

void loadHdf(const scidb::Value* args, scidb::Value& res, void*);


#endif // LOADER_LOADOPERATOR_HH
