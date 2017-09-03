#ifndef XMP_HELPER_H
#define XMP_HELPER_H

#define TXMP_STRING_TYPE std::string
#define XMP_INCLUDE_XMPFILES 1

#include "xmpio_over_istream.h"
#include <XMP.incl_cpp>
#include <XMP.hpp>
#include <time.h>
#include <string_utils.h>
#include <mutex>
#include "tag.h"

using std::string;
using std::vector;
using std::shared_ptr;

namespace {
const XMP_StringPtr kMsipNamespace = "http://www.microsoft.com/msip/1.0/";
const XMP_StringPtr kMsipLabelNamespace = "http://www.microsoft.com/msip/label/1.0/";
const XMP_StringPtr kMsipLabelExtendedNamespace = "http://www.microsoft.com/msip/label/extended/1.0/";
const XMP_StringPtr kLabelsArrayName = "labels";
const XMP_StringPtr kExtendedStructName = "extended";
}

namespace mip {
namespace file {

class XMPHelper {

public:

static string GetStringByKey(const SXMPMeta& metadata, const string &labelsItemPath, const XMP_StringPtr fieldName, const XMP_StringPtr alternativeFieldName, const XMP_StringPtr oldestFieldName = nullptr);

vector<Tag> Deserialize(const SXMPMeta& metadata);

static void Initialize();

const vector<Tag> GetTags(shared_ptr<IStream> fileStream);

};

} // namespace file
} // namespace mip

#endif // XMP_HELPER_H
