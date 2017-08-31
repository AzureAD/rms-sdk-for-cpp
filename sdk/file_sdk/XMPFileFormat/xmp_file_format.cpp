#include "xmp_file_format.h"

#include "xmp_stream.h"
#include <XMP.incl_cpp>
#include <XMP.hpp>
#include <time.h>
#include <string_utils.h>
#include <mutex>

using std::string;
using std::vector;

namespace mip {
namespace file {

const XMP_StringPtr kMsipNamespace = "http://www.microsoft.com/msip/1.0/";
const XMP_StringPtr kMsipLabelNamespace = "http://www.microsoft.com/msip/label/1.0/";
const XMP_StringPtr kMsipLabelExtendedNamespace = "http://www.microsoft.com/msip/label/extended/1.0/";
const XMP_StringPtr kLabelsArrayName = "labels";
const XMP_StringPtr kExtendedStructName = "extended";

bool Initialized = false;
std::mutex InitMutex;

static string GetStringByKey(_In_ const SXMPMeta* metadata, _In_ const string &labelsItemPath, _In_ const XMP_StringPtr fieldName, _In_ const XMP_StringPtr alternativeFieldName)
{
  std::string path, value;
  SXMPUtils::ComposeStructFieldPath(kMsipNamespace, labelsItemPath.c_str(), kMsipLabelNamespace, fieldName, &path);
  metadata->GetProperty(kMsipNamespace, path.c_str(), &value, 0);
  if (value.empty() && alternativeFieldName != nullptr)
  {
    SXMPUtils::ComposeStructFieldPath(kMsipNamespace, labelsItemPath.c_str(), kMsipLabelNamespace, alternativeFieldName, &path);
    metadata->GetProperty(kMsipNamespace, path.c_str(), &value, 0);
  }

  return value;
}

static vector<Tag> Deserialize(_In_ const SXMPMeta* metadata)
{
  XMP_Index count = metadata->CountArrayItems(kMsipNamespace, kLabelsArrayName);

  vector<Tag> tags;

  for (int i = 1; i <= count; ++i)
  {
    std::string labelsItemPath;

    SXMPUtils::ComposeArrayItemPath(kMsipNamespace, kLabelsArrayName, i, &labelsItemPath);

    auto labelId = GetStringByKey(metadata, labelsItemPath, "Id", nullptr);
    auto labelName = GetStringByKey(metadata, labelsItemPath, "Name", nullptr);
    auto siteId = GetStringByKey(metadata, labelsItemPath, "SiteId", "Ref");
    bool enabled;

    TryParseBool(GetStringByKey(metadata, labelsItemPath, "Enabled", nullptr), enabled);

    auto parentId = GetStringByKey(metadata, labelsItemPath, "ParentId", nullptr);

    auto owner = GetStringByKey(metadata, labelsItemPath, "Owner", "AssignedBy");
    if (owner == "")
      owner = GetStringByKey(metadata, labelsItemPath, "Owner", "SetBy");

    auto setDate = GetStringByKey(metadata, labelsItemPath, "SetDate", "DateCreated");
    auto application = GetStringByKey(metadata, labelsItemPath, "Application", nullptr);

    std::string path, method;
    SXMPUtils::ComposeStructFieldPath(kMsipNamespace, labelsItemPath.c_str(), kMsipLabelExtendedNamespace, "MSFT_Method", &path);
    metadata->GetProperty(kMsipNamespace, path.c_str(), &method, 0);

    Method methodEnum = Method::NONE;
    if(EqualsIgnoreCase(method, "automatic"))
      methodEnum = Method::AUTOMATIC;
    else if(EqualsIgnoreCase(method, "manual"))
      methodEnum = Method::MANUAL;

    std::string propVendor, propKey, propValue, extendedStructPath;
    bool exists = true;

    vector<ExtendedProperty> properties;
    exists = metadata->DoesStructFieldExist(kMsipNamespace, labelsItemPath.c_str(), kMsipLabelNamespace, kExtendedStructName);
    if (exists)
    {
      SXMPUtils::ComposeStructFieldPath(kMsipLabelNamespace, labelsItemPath.c_str(), kMsipLabelNamespace, kExtendedStructName, &extendedStructPath);
      XMP_Index propCount = metadata->CountArrayItems(kMsipNamespace, extendedStructPath.c_str());
      for (int j = 1; j <= propCount; j++)
      {
        SXMPUtils::ComposeArrayItemPath(kMsipNamespace, extendedStructPath.c_str(), j, &labelsItemPath);

        SXMPUtils::ComposeStructFieldPath(kMsipNamespace, labelsItemPath.c_str(), kMsipLabelExtendedNamespace, "Vendor", &path);
        metadata->GetProperty(kMsipNamespace, path.c_str(), &propVendor, 0);

        SXMPUtils::ComposeStructFieldPath(kMsipNamespace, labelsItemPath.c_str(), kMsipLabelExtendedNamespace, "Key", &path);
        metadata->GetProperty(kMsipNamespace, path.c_str(), &propKey, 0);

        SXMPUtils::ComposeStructFieldPath(kMsipNamespace, labelsItemPath.c_str(), kMsipLabelExtendedNamespace, "Value", &path);
        metadata->GetProperty(kMsipNamespace, path.c_str(), &propValue, 0);

        ExtendedProperty property;
        property.vendor = propVendor;
        property.key = propKey;
        property.value = propValue;

        properties.push_back(property);
      }
    }

    tags.push_back(Tag(labelId, labelName, parentId, owner, enabled, setDate, application, methodEnum, siteId, properties));
  }

  return tags;
}

static void Initialize()
{
  if (Initialized)
    return;

  InitMutex.lock();
  if (Initialized)
    return;

  if (!SXMPMeta::Initialize())
    new std::runtime_error("SXMPMeta Error");

  if (!SXMPFiles::Initialize())
    new std::runtime_error("SXMPFiles Error");

  std::string actualPrefix;
  SXMPMeta::RegisterNamespace(kMsipNamespace, "msip", &actualPrefix);
  SXMPMeta::RegisterNamespace(kMsipLabelNamespace, "msip-label", &actualPrefix);
  SXMPMeta::RegisterNamespace(kMsipLabelExtendedNamespace, "msip-label-extended", &actualPrefix);

  Initialized = true;
  InitMutex.unlock();
}

XMPFileFormat::XMPFileFormat(shared_ptr<IStream> file, const string& extension)
  : FileFormat(file, extension)
{
}

const vector<Tag> XMPFileFormat::ReadTags() {
  Initialize();
  XMPStream fileHandler(mFile);

  SXMPFiles file;
  if (!file.OpenFile(&fileHandler, kXMP_UnknownFile, kXMPFiles_OpenForRead | kXMPFiles_OpenUseSmartHandler) &&
      !file.OpenFile(&fileHandler, kXMP_UnknownFile, kXMPFiles_OpenForRead | kXMPFiles_OpenUsePacketScanning))
    throw std::runtime_error("OpenFile failed");

  SXMPMeta metadata;
  file.GetXMP(&metadata);

  try
  {
    auto tags = Deserialize(&metadata);
    file.CloseFile();
    return tags;
  }
  catch(...)
  {
    file.CloseFile();
    throw std::runtime_error("GetXMP failed");
  }

}

void XMPFileFormat::Commit(shared_ptr<IStream> file, string& newExtension) {
  throw std::runtime_error("not implemented");
}

} // namespace file
} // namespace mip
