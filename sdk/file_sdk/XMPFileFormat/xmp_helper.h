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

using std::string;
using std::vector;
using std::shared_ptr;
using std::pair;

namespace {
const XMP_StringPtr kMsipNamespace = "http://www.microsoft.com/msip/1.0/";
}

namespace mip {
namespace file {

class XMPHelper {

public:
  static XMPHelper& GetInstance();

  vector<pair<string, string>> Deserialize(const SXMPMeta& metadata);

  void Serialize(SXMPMeta& metadata, const vector<pair<string, string>>& propertiesToAdd, const vector<string>& keysToRemove);

  const vector<pair<string, string>> GetProperties(shared_ptr<IStream> fileStream);

  void UpdateProperties(shared_ptr<IStream> stream, const vector<pair<string, string>>& propertiesToAdd, const vector<string>& keysToRemove);

  XMPHelper(XMPHelper const&) = delete;

  void operator=(XMPHelper const&) = delete;
private:
  XMPHelper();
  ~XMPHelper(){}
};

} // namespace file
} // namespace mip

#endif // XMP_HELPER_H
