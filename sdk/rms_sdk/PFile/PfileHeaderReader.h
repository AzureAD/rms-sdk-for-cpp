/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
 */

#ifndef _RMS_LIB_PFILEHEADERREADER_H_
#define _RMS_LIB_PFILEHEADERREADER_H_

#include "IPfileHeaderReader.h"
#include "PfileHeader.h"
#include "../Common/FrameworkSpecificTypes.h"

#include <string>

namespace rmscore {
namespace pfile {
class PfileHeaderReader : public IPfileHeaderReader {
public:

  PfileHeaderReader() {}

  virtual ~PfileHeaderReader() override;

  virtual std::shared_ptr<PfileHeader>Read(rmscrypto::api::SharedStream stream)
  override;
  const static int NOT_PFILE     = 0x800401ffL;
  const static int NOT_SUPPORTED = 0x80070032L;
  const static int BAD_ARGUMENTS = 0x000000a0L;

private:

  std::shared_ptr<PfileHeader>ReadHeader(rmscrypto::api::SharedStream stream,
                                         uint32_t                     majorVersion,
                                         uint32_t                     minorVersion,
                                         const std::string          & cleartextRedirectionHeader);

  void ReadAtOffset(common::ByteArray          & dst,
                    rmscrypto::api::SharedStream stream,
                    uint32_t                     offset,
                    uint32_t                     length);

  void ReadBytes(common::ByteArray          & dst,
                 rmscrypto::api::SharedStream stream,
                 uint32_t                     length);

  void                          CheckPreamble(rmscrypto::api::SharedStream stream);

  std::tuple<uint32_t, uint32_t>ReadVersionNumber(
    rmscrypto::api::SharedStream stream);
  std::string                   ReadCleartextRedirectionHeader(
    rmscrypto::api::SharedStream stream);
  std::string                   ReadExtension(rmscrypto::api::SharedStream stream,
                                              uint32_t                     offset,
                                              uint32_t                     length);
};
} // namespace pfile
} // namespace rmscore
#endif // _RMS_LIB_PFILEHEADERREADER_H_
