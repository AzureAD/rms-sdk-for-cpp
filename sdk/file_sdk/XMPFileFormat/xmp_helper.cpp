#include "xmp_helper.h"

static bool mInitialized = false;
static std::mutex mInitMutex;

namespace mip {
namespace file {

string XMPHelper::GetStringByKey(const SXMPMeta& metadata,const string &labelsItemPath, const XMP_StringPtr fieldName, const XMP_StringPtr alternativeFieldName, const XMP_StringPtr oldestFieldName) {
  std::string path;
  SXMPUtils::ComposeStructFieldPath(kMsipNamespace, labelsItemPath.c_str(), kMsipLabelNamespace, fieldName, &path);
  std::string value;
  metadata.GetProperty(kMsipNamespace, path.c_str(), &value, 0);
  if (value.empty() && alternativeFieldName != nullptr) {
    SXMPUtils::ComposeStructFieldPath(kMsipNamespace, labelsItemPath.c_str(), kMsipLabelNamespace, alternativeFieldName, &path);
    metadata.GetProperty(kMsipNamespace, path.c_str(), &value, 0);
  }

  if(value.empty() && oldestFieldName != nullptr) {
    SXMPUtils::ComposeStructFieldPath(kMsipNamespace, labelsItemPath.c_str(), kMsipLabelNamespace, oldestFieldName, &path);
    metadata.GetProperty(kMsipNamespace, path.c_str(), &value, 0);
  }

  return value;
}

vector<Tag> XMPHelper::Deserialize(const SXMPMeta& metadata) {
  XMP_Index count = metadata.CountArrayItems(kMsipNamespace, kLabelsArrayName);

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

    auto owner = GetStringByKey(metadata, labelsItemPath, "Owner", "AssignedBy", "SetBy");

    auto setDate = GetStringByKey(metadata, labelsItemPath, "SetDate", "DateCreated");
    auto application = GetStringByKey(metadata, labelsItemPath, "Application", nullptr);

    std::string path, method;
    SXMPUtils::ComposeStructFieldPath(kMsipNamespace, labelsItemPath.c_str(), kMsipLabelExtendedNamespace, "MSFT_Method", &path);
    metadata.GetProperty(kMsipNamespace, path.c_str(), &method, 0);

    Method methodEnum = Method::NONE;
    if(EqualsIgnoreCase(method, "automatic"))
      methodEnum = Method::AUTOMATIC;
    else if(EqualsIgnoreCase(method, "manual"))
      methodEnum = Method::MANUAL;

    std::string propVendor, propKey, propValue, extendedStructPath;
    bool exists = true;

    vector<ExtendedProperty> properties;
    exists = metadata.DoesStructFieldExist(kMsipNamespace, labelsItemPath.c_str(), kMsipLabelNamespace, kExtendedStructName);
    if (exists)
    {
      SXMPUtils::ComposeStructFieldPath(kMsipLabelNamespace, labelsItemPath.c_str(), kMsipLabelNamespace, kExtendedStructName, &extendedStructPath);
      XMP_Index propCount = metadata.CountArrayItems(kMsipNamespace, extendedStructPath.c_str());
      for (int j = 1; j <= propCount; j++)
      {
        SXMPUtils::ComposeArrayItemPath(kMsipNamespace, extendedStructPath.c_str(), j, &labelsItemPath);

        SXMPUtils::ComposeStructFieldPath(kMsipNamespace, labelsItemPath.c_str(), kMsipLabelExtendedNamespace, "Vendor", &path);
        metadata.GetProperty(kMsipNamespace, path.c_str(), &propVendor, 0);

        SXMPUtils::ComposeStructFieldPath(kMsipNamespace, labelsItemPath.c_str(), kMsipLabelExtendedNamespace, "Key", &path);
        metadata.GetProperty(kMsipNamespace, path.c_str(), &propKey, 0);

        SXMPUtils::ComposeStructFieldPath(kMsipNamespace, labelsItemPath.c_str(), kMsipLabelExtendedNamespace, "Value", &path);
        metadata.GetProperty(kMsipNamespace, path.c_str(), &propValue, 0);

        ExtendedProperty property;
        property.vendor = propVendor;
        property.key = propKey;
        property.value = propValue;

        properties.push_back(property);
      }
    }

    tags.push_back(Tag(labelId, labelName, parentId, owner, enabled, setDate, methodEnum, siteId, properties));
  }

  return tags;
}

void XMPHelper::Initialize() {
  if (mInitialized)
    return;

  mInitMutex.lock();
  if (mInitialized)
    return;

  if (!SXMPMeta::Initialize())
    throw new std::runtime_error("SXMPMeta Error");

  if (!SXMPFiles::Initialize())
    throw new std::runtime_error("SXMPFiles Error");

  std::string actualPrefix;
  SXMPMeta::RegisterNamespace(kMsipNamespace, "msip", &actualPrefix);
  SXMPMeta::RegisterNamespace(kMsipLabelNamespace, "msip-label", &actualPrefix);
  SXMPMeta::RegisterNamespace(kMsipLabelExtendedNamespace, "msip-label-extended", &actualPrefix);

  mInitialized = true;
  mInitMutex.unlock();
}

const vector<Tag> XMPHelper::GetTags(shared_ptr<IStream> fileStream) {
  Initialize();
  XMPIOOverIStream xmpStream (fileStream);

  SXMPFiles file;
  if (!file.OpenFile(&xmpStream , kXMP_UnknownFile, kXMPFiles_OpenForRead | kXMPFiles_OpenUseSmartHandler) &&
      !file.OpenFile(&xmpStream , kXMP_UnknownFile, kXMPFiles_OpenForRead | kXMPFiles_OpenUsePacketScanning))
    throw std::runtime_error("OpenFile failed");

  try
  {
    SXMPMeta metadata;
    file.GetXMP(&metadata);
    auto tags = Deserialize(metadata);
    file.CloseFile();
    return tags;
  }
  catch(XMP_Error error)
  {
    file.CloseFile();
    throw std::runtime_error(error.GetErrMsg());
  }
  catch(...)
  {
    file.CloseFile();
    throw std::runtime_error("GetXMP failed");
  }
}

} // namespace file
} // namespace mip
