#ifndef XMP_HELPER_H
#define XMP_HELPER_H

#define TXMP_STRING_TYPE std::string
#define XMP_INCLUDE_XMPFILES 1

#include "xmpio_over_istream.h"
#include <XMP.incl_cpp>
#include <XMP.hpp>
#include <time.h>
#include <Common/string_utils.h>
#include <mutex>
#include "Common/tag.h"

using std::string;
using std::vector;
using std::shared_ptr;

namespace {
const XMP_StringPtr kMsipNamespace = "http://www.microsoft.com/msip/1.0/";
}

namespace mip {
namespace file {

class XMPHelper {

public:
  static XMPHelper& GetInstance();

  vector<Tag> Deserialize(const SXMPMeta& metadata);

  const vector<Tag> GetTags(shared_ptr<IStream> fileStream);

  XMPHelper(XMPHelper const&) = delete;

  void operator=(XMPHelper const&) = delete;
private:
  XMPHelper();
  bool mInitialized = false;
  std::mutex mInitMutex;

};

} // namespace file
} // namespace mip

#endif // XMP_HELPER_H
