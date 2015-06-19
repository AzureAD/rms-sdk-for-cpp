/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _RMS_LIB_IPFILEHEADERWRITER_H_
#define _RMS_LIB_IPFILEHEADERWRITER_H_

#include <memory>
#include <IStream.h>
#include "../Common/FrameworkSpecificTypes.h"
namespace rmscore {
namespace pfile {
class PfileHeader;

class IPfileHeaderWriter {
public:

  virtual ~IPfileHeaderWriter() {}

  virtual size_t Write(rmscrypto::api::SharedStream stream,
                       const std::shared_ptr<PfileHeader> header) = 0;

public:

  static std::shared_ptr<IPfileHeaderWriter>Create();
};
} // namespace pfile
} // namespace rmscore

#endif // _RMS_LIB_IPFILEHEADERWRITER_H_
