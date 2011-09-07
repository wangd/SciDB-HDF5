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
#ifndef LOADER_ARRAYCOMMONFWD_HH
#ifndef LOADER_ARRAYCOMMON_HH
#define LOADER_ARRAYCOMMONFWD_HH
#include <boost/shared_ptr.hpp>
#include <vector>
class Attr;
typedef std::vector<Attr> AttrVector;
typedef boost::shared_ptr<AttrVector> AttrVectorPtr;

class ScidbAttrLite;
typedef std::vector<ScidbAttrLite> SalVector;
typedef boost::shared_ptr<SalVector> SalVectorPtr;

class Dim;
typedef std::vector<Dim> DimVector;
typedef boost::shared_ptr<DimVector> DimVectorPtr;

class ScidbDimLite;
typedef std::vector<ScidbDimLite> SdlVector;
typedef boost::shared_ptr<SdlVector> SdlVectorPtr;
#endif // LOADER_ARRAYCOMMON_HH
#endif // LOADER_ARRAYCOMMONFWD_HH
