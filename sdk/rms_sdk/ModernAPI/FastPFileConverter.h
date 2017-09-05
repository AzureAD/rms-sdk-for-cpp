#ifndef PFILECONVERTER_H
#define PFILECONVERTER_H

#include "IAuthenticationCallback.h"
#include "IConsentCallback.h"

namespace rmscore {
namespace modernapi {
class DLL_PUBLIC_RMS FastPFileConverter
{
public:
    FastPFileConverter();
    bool FastPFileConverter::ConvertFromPFile(
      const std::string                       & userId,
	  const std::string						  & inFile,
	  const std::string						  &	outFile,
      IAuthenticationCallback                 & auth,
      IConsentCallback                        & consent);  
};
} //modernapi
} //rmscore
#endif // PFILECONVERTER_H
