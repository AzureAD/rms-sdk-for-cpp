/*
 * ======================================================================
 * Copyright (c) Microsoft Open Technologies, Inc.  All rights reserved.
 * Licensed under the MIT License.
 * See LICENSE.md in the project root for license information.
 * ======================================================================
*/

#ifndef _CRYPTO_STREAMS_LIB_CACHEDBLOCK_H_
#define _CRYPTO_STREAMS_LIB_CACHEDBLOCK_H_

#include <memory>
#include <vector>

namespace rmscrypto {
namespace api {
class SimpleProtectedStream;

class CachedBlock {
public:

  CachedBlock(std::shared_ptr<SimpleProtectedStream>pSimple,
              uint64_t                              u64BlockSize);

  uint64_t GetBlockSize();

  void     UpdateBlock(uint64_t u64Position);

  uint64_t ReadFromBlock(uint8_t *pbBuffer,
                         uint64_t u64Position,
                         uint64_t u64Size);

  uint64_t WriteToBlock(const uint8_t *pbBuffer,
                        uint64_t       u64Position,
                        uint64_t       u64Size);

  void     RewriteFinalBlock(uint64_t newSize);
  bool     Flush();
  uint64_t GetSizeInternal() const;
  void     SizeInternal(uint64_t u64Size);

private:

  uint32_t CalculateBlockNumber(uint64_t u64Position) const;

private:

  std::shared_ptr<SimpleProtectedStream> m_pSimple;
  uint64_t m_u64BlockSize;

  uint64_t m_u64CacheStart;
  uint64_t m_u64CacheSize;
  std::vector<uint8_t> m_cache;
  bool m_bFinalBlockHasBeenWritten;
  bool m_bWritePending;
};
} // namespace api
} // namespace rmscrypto
#endif // ifndef _CRYPTO_STREAMS_LIB_CACHEDBLOCK_H_
