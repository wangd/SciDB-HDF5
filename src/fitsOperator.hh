#ifndef LOADER_FITSOPERATOR_HH
#define LOADER_FITSOPERATOR_HH

#include <string>

void loadFits(std::string const& filePath, 
              int hduNumber,
              std::string const& arrayName);


#endif // LOADER_FITSOPERATOR_HH
