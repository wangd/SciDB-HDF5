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
#ifndef LOADER_LOADOPERATOR_HH
#define LOADER_LOADOPERATOR_HH

#include <string>
#include <boost/shared_ptr.hpp>

namespace scidb {
    class Query; // forward
    class Array;
}

boost::shared_ptr<scidb::Array> loadHdf(std::string const& filePath, 
                                        std::string const& hdfPath, 
                                        boost::shared_ptr<scidb::Query>& q);



#endif // LOADER_LOADOPERATOR_HH
