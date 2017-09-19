#include "xmp_helper.h"
#include "Common/exceptions.h"

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
  try{
    if (!SXMPMeta::Initialize())
      throw new Exception("SXMPMeta Error");

    if (!SXMPFiles::Initialize(kXMPFiles_IgnoreLocalText))
      throw new Exception("SXMPFiles Error");

    std::string actualPrefix;
    SXMPMeta::RegisterNamespace(kMsipNamespace, "msip", &actualPrefix);
  }
  catch(XMP_Error ex){
    throw new Exception(ex.GetErrMsg());
  }
}

void XMPHelper::Serialize(SXMPMeta& metadata, const vector<Tag>& tags) {
  string propertyPath;
  vector<std::string> propertiesToDelete;

  SXMPIterator iterator(metadata, kMsipNamespace, kXMP_IterJustChildren);
  while (iterator.Next(nullptr, &propertyPath))
    propertiesToDelete.push_back(propertyPath);

  for(unsigned i =0; i< propertiesToDelete.size(); i++)
    metadata.DeleteProperty(kMsipNamespace, propertiesToDelete[i].c_str());

  auto properties = mip::Tag::ToProperties(tags);

  for(unsigned j =0; j< properties.size(); j++){
    string value = properties[j].first;
    if (value.empty())
      continue;

    metadata.SetProperty(kMsipNamespace, properties[j].first.c_str(), properties[j].second);
  }
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
    throw Exception("OpenFile failed");

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
    throw Exception(error.GetErrMsg());
  }
  catch(...)
  {
    file.CloseFile();
    throw Exception("GetXMP failed");
  }
}

void XMPHelper::SetTags(shared_ptr<IStream> stream, const vector<Tag>& tags)
{
  XMPIOOverIStream xmpStream (stream);

  SXMPFiles file;
  if (!file.OpenFile(&xmpStream , kXMP_UnknownFile, kXMPFiles_OpenForUpdate | kXMPFiles_OpenUseSmartHandler) &&
      !file.OpenFile(&xmpStream , kXMP_UnknownFile, kXMPFiles_OpenForUpdate | kXMPFiles_OpenUsePacketScanning))
    throw Exception("OpenFile failed");

  try
  {
    SXMPMeta metadata;
    file.GetXMP(&metadata);
    Serialize(metadata, tags);

    if (!file.CanPutXMP(metadata))
    {
      file.CloseFile();
      throw Exception("CanPutXMP failed");
    }

    stream->Seek(0);
    file.PutXMP(metadata);
    file.CloseFile();
  }
  catch(XMP_Error error)
  {
    file.CloseFile();
    throw Exception(error.GetErrMsg());
  }
  catch(...)
  {
    file.CloseFile();
    throw Exception("SetTags faield");
  }
}

} // namespace file
} // namespace mip
