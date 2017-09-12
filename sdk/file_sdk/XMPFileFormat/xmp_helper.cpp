#include "xmp_helper.h"

using std::pair;

namespace mip {
namespace file {

XMPHelper& XMPHelper::GetInstance()
{
  static XMPHelper instance;
  return instance;
}

XMPHelper::XMPHelper()
{
  if (mInitialized)
    return;

  std::lock_guard<std::mutex> lock(mInitMutex);

  if (mInitialized)
    return;

  if (!SXMPMeta::Initialize())
    throw new std::runtime_error("SXMPMeta Error");

  if (!SXMPFiles::Initialize())
    throw new std::runtime_error("SXMPFiles Error");

  std::string actualPrefix;
  SXMPMeta::RegisterNamespace(kMsipNamespace, "msip", &actualPrefix);

  mInitialized = true;
}

vector<Tag> XMPHelper::Deserialize(const SXMPMeta& metadata) {
  string namespacePrefix;
  vector<Tag> tags;

  if (!metadata.GetNamespacePrefix(kMsipNamespace, &namespacePrefix))
    return tags; // namespace doesn't exist

  string propertyPath;
  string propertyValue;
  vector<pair<string, string>> properties;

  SXMPIterator iterator(metadata, kMsipNamespace, kXMP_IterJustChildren | kXMP_IterOmitQualifiers | kXMP_IterJustLeafName);
  while (iterator.Next(nullptr, &propertyPath, &propertyValue))
  {
    if (propertyPath.length() < namespacePrefix.length())
      continue;

    auto name = propertyPath.erase(0, namespacePrefix.length());
    auto value = propertyValue;

    properties.push_back(pair<string, string>(name, value));
  }


  return Tag::FromProperties(properties);
}

const vector<Tag> XMPHelper::GetTags(shared_ptr<IStream> fileStream) {
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
