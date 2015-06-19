/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _MICROSOFT_PROTECTION_PFILEHEADERWRITER_H_
#define _MICROSOFT_PROTECTION_PFILEHEADERWRITER_H_

#include "IPfileHeaderWriter.h"
#include "../Common/CommonTypes.h"
#include "../Common/FrameworkSpecificTypes.h"
#include <string>

namespace rmscore {
namespace pfile {
const std::string CleartextRedirectHeader(
  "\r\n\r\n\r\nThis file uses Microsoft Information Protection solutions.\r\nOpen it using an application that supports protected files.\r\n\r\nYou can download Microsoft's protected file viewer from: http://go.microsoft.com/fwlink/?LinkId=280381 \r\nLearn more about Information Protection solutions at http://www.microsoft.com/rms \r\n\r\nDo not change this file in any way -- doing so will result in data loss.\r\n\r\n");

class PfileHeader;
class PfileHeaderWriter : public IPfileHeaderWriter {
public:

  virtual ~PfileHeaderWriter();

  virtual size_t Write(rmscrypto::api::SharedStream           stream,
                       const std::shared_ptr<PfileHeader>header) override;

private:

  uint32_t WritePreamble(rmscrypto::api::SharedStream writer);
  uint32_t WriteVersionNumber(rmscrypto::api::SharedStream           writer,
                              const std::shared_ptr<PfileHeader>header);
  uint32_t WriteCleartextRedirection(rmscrypto::api::SharedStream           writer,
                                     const std::shared_ptr<PfileHeader>header);
  void     WriteHeader(rmscrypto::api::SharedStream           writer,
                       const std::shared_ptr<PfileHeader>header,
                       size_t                            headerOffset);
  void     WriteExtension(rmscrypto::api::SharedStream           writer,
                          const std::shared_ptr<PfileHeader>header);
  void     WritePublishingLicense(rmscrypto::api::SharedStream           writer,
                                  const std::shared_ptr<PfileHeader>header);
  void     WriteMetadata(rmscrypto::api::SharedStream           writer,
                         const std::shared_ptr<PfileHeader>header);
};
} // namespace pfile
} // namespace rmscore

#endif // _MICROSOFT_PROTECTION_PFILEHEADERWRITER_H_
