/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _RMS_LIB_IPFILEHEADERREADER_H_
#define _RMS_LIB_IPFILEHEADERREADER_H_

#include <memory>
#include <IStream.h>
#include "../Common/FrameworkSpecificTypes.h"

namespace rmscore {
namespace pfile {
class PfileHeader;

class IPfileHeaderReader {
public:

  virtual ~IPfileHeaderReader() {}

  virtual std::shared_ptr<PfileHeader>Read(rmscrypto::api::SharedStream stream) = 0;

public:

  static std::shared_ptr<IPfileHeaderReader>Create();
};
} // namespace pfile
} // namespace rmscore
#endif // _RMS_LIB_IPFILEHEADERREADER_H_
